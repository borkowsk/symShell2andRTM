/// @file
/// @brief __The type that queues values from a scalar source.__ /<br>
///         _Klasa kolejkująca wartości ze źródła skalarnego._
/// @date 2026-05-18 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_FIFO_SOUR_HPP_INCLUDED_
#define SYMSHELL2_FIFO_SOUR_HPP_INCLUDED_

#include "datasour.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

/// @brief Klasa kolejkująca wartości ze źródła skalarnego.
/// @tparam T — typ wartości w buforze. Nie koniecznie `double` !!!
template<class T>
class fifo_source : public linear_source_base
//----------------------------------------------
{
    typedef template_scalar_source_base<T> SOURCE; ///< Typ źródłowy fla filtra.
    wb_dynarray<char> _name;
    SOURCE *Source;
    wb_dynarray<T> bufor;
    int next_one; ///< Granica wpisania, gdy idzie cyklicznie

    /// Metoda obsługująca FIFO.
    /// Ładuje wartość do bufora i ewentualnie zmienia N.
    /// Aktualnie zależne od sprawdzania MISSING VALUE więc musi być z `double`.
    //const T& — TO DO ZMIANY, GDY `data_source_base` będzie SZABLONEM TAKŻE WZGLĘDEM WARTOŚCI POBIERANEJ/PRZECHOWYWANEJ.
    void add(const double &val);

    /// Sprawdza, czy i jak zmieniły się dane w źródle i ładuje następną z wartości.
    int check_version()
    {
        if(Source->data_version() == -1)
            return 0; //Przed inicjalizacja nic nie zapisuje

        int ret = update_version_from(Source);
        if(ret == 1)	//Gdy są nowe dane.
        {
            double pom = Source->get();
            add(pom);
        }
        return ret;
    }

public:
    /// Akcesor źródła danych.
    SOURCE *_get_source()
    { return Source; }

    /// KONSTRUKTOR.
    /// \param i_source to wskaźnik do źródłowej "serii" danych (źródła skalarnego!).
    /// \param buff_size to liczba danych, jaką może przechować kolejka.
    /// \param format to sposób wygenerowania nazwy fifo z nazwy serii źródłowej i wielkości bufora.
    /// \param i_min to przewidywane minimum zakresu danych.
    /// \param i_max to przewidywane maksimum zakresu danych.
    fifo_source(SOURCE *i_source, size_t buff_size, const char *format = "%s(last %0.3g steps)",
                double i_min = 0, double i_max = 0
    ) :
            linear_source_base(0, format), Source(i_source),
            bufor(buff_size), next_one(0)	//,Pierwszy(-1)
    {
        assert(Source != NULL);
        size_t dummy; //Nie istotne
        linear_source_base::set_missing(Source->get_missing()); //Tu przenosimy "na żywca"
        Source->bounds(dummy, y_min, y_max);
        if(y_min == y_max) //Nieustalone
        {
            y_min = i_min;
            y_max = i_max;
        } else
        {
            if(y_min > i_min) y_min = i_min;
            if(y_max < i_max) y_max = i_max;
        }
        check_version();
    }

    /// DESTRUKTOR.
    ~fifo_source() override = default;

    /// Restartuje "versioning" źródeł. W wypadku pod-źródeł powinna być reimplementacja!
    void restart_counting() override
    {
        linear_source_base::restart_counting();
        next_one = 0; // Jak w konstruktorze
        linear_source_base::N = 0; //Nie ma już aktualnych wartości.
        //`linear_source_base::???`
        //Pierwszy=-1; //
    }

    /// Przedefiniowana obsługa "missing values", bo wartości w buforze mogą nie być typu `double`!!!
    void set_missing(const T &i_miss)
    {
        linear_source_base::set_missing(double(i_miss));
    }


private:
    /// A stara obsługa "missing values" zasłonięta — widoczna tylko jako wirtualna
    void set_missing(double i_miss) override
    {
        assert(wbrtm::limit<T>::Min() <= i_miss && i_miss <= wbrtm::limit<T>::Max());
        linear_source_base::set_missing(double(i_miss));
    }

public:
    /// Pobranie "missing value" musi być specyficznie zdefiniowana dla `float`, `long` , `int`, `unsigned` itp.
    /// I tu się zaczyna problem z "przykrywaniem" funkcji z klasy bazowej.
    T get_missing();

    int is_missing(const T &val)
    {
        double val_dbl = double(val); //Zakładamy, że w tę stronę zawsze jest OK.
        return linear_source_base::is_missing(val_dbl);
    }

    /// Zmiana ostatniej wartości na wypadek, gdy monitorowana zmienna zmienia się na skutek działania użytkownika.
    void force_last(double val);

    /// @name Przedefiniowane akcesory wirtualne
    /// @{

    /// Kombinowana nazwa seri FIFO.
    const char *name() override;

    long data_version() override	//numer wersji danych
    { check_version(); return data_source_base::data_version(); }

    long how_old_data() override	//od ilu wersji dane się nie zmieniły
    { check_version(); return data_source_base::how_old_data(); }

    /// Sprawdza źródło i podaje ile elementów, wartości minimalna i maksymalna
    void bounds(size_t &num, double &min, double &max) override;

    iterator_h reset() override //Rozpoczęcie czytania bufora
    { check_version(); return linear_source_base::reset(); }

    double get(iterator_h &ptr_to_iterator) override; //Daje następną z N liczb.
    double get(geometry::index_t index_from_geometry) override; //Daje konkretną z N liczb.
    /// @}
};


// IMPLEMENTACJE:
//===============

template<> inline
double fifo_source<double>::get_missing()
{
    double pom = linear_source_base::get_missing();
    double pomT(pom);                   //NOTE: missing should be inside T
    //assert(double(pomT)==double(pom));  //check this constrain!
    return pomT;
}

template<> inline
float fifo_source<float>::get_missing()
{
    double pom = linear_source_base::get_missing();
    float pomT = float(pom);                   //NOTE: missing should be inside T
    if(double(pomT) != double(pom))  //check this constraining!
        pomT = default_missing<float>();
    return pomT;
}

template<> inline
long fifo_source<long>::get_missing()
{
    double pom = linear_source_base::get_missing();
    long pomT = long(pom);                   //NOTE: missing should be inside T
    if(double(pomT) != double(pom))  //check this constraining!
        pomT = default_missing<long>();
    return pomT;
}

template<> inline
int fifo_source<int>::get_missing()
{
    double pom = linear_source_base::get_missing();
    int pomT = int(pom);                   //NOTE: missing should be inside T
    if(double(pomT) != double(pom))  //check this constraining!
        pomT = default_missing<int>();
    return pomT;
}

template<> inline
unsigned long fifo_source<unsigned long>::get_missing()
{
    double pom = linear_source_base::get_missing();
    unsigned long pomT((unsigned long) (pom));                   //NOTE: missing should be inside T
    if(double(pomT) != double(pom))  //check this constraining!
        pomT = default_missing<unsigned long>();
    return pomT;
}

template<> inline
unsigned int fifo_source<unsigned int>::get_missing()
{
    double pom = linear_source_base::get_missing();
    unsigned int pomT((unsigned int) (pom));                   //NOTE: missing should be inside T
    if(double(pomT) != double(pom))  //check this constraining!
        pomT = default_missing<unsigned int>();
    return pomT;
}

template<class T> inline
T fifo_source<T>::get_missing()
{
    double pom = linear_source_base::get_missing();
    T pomT{default_missing<T>()};  //NOTE: missing value should be inside type T
    assert(double(pomT)==double(pom));  //check this constraining!
    return pomT;
}

template<class T> inline
void fifo_source<T>::add(const double &val)
//Metoda obsługująca FIFO ładuje wartości do bufora i ewentualnie zmienia N
{
    size_t TN = bufor.get_size(); //Ile się mieści w tablicy

    //Implementacja cykliczności bufora
    next_one %= TN; //Żeby nie wyszło z tablicy. CYKL!

    if(!Source->is_missing(val))
    {
        if(y_max < val)
            y_max = val;
        if(y_min > val)
            y_min = val;
        //Zapamiętanie
        bufor[next_one] = T(val); //Uwaga — `bufor` typu `T`, a `val` typu `double`!
    } else
        bufor[next_one] = T(get_missing());

    //Przemieszenie indeksu pustego miejsca
    next_one++;

    //Gospodarka miejscem
    if(N < TN)
    {
        N++; //Wypełnianie pustego miejsca w buforze
        //if(Pierwszy==-1)
        //	Pierwszy++;
    }

}

template<class T> inline
void fifo_source<T>::force_last(double val)
//Zmienia zawartość ostatniej komórki — np. gdy użytkownik zmieni wartość zmiennej
{
    size_t TN = bufor.get_size(); //Ile się mieści w tablicy

    //Implementacja cykliczności bufora
    size_t ind = (next_one > 0?next_one - 1:0);
    ind %= TN; //Żeby nie wyszło z tablicy. CYKL! (niepotrzebne tutaj?)

    if(!Source->is_missing(val))
    {
        if(y_max < val)
            y_max = val;
        if(y_min > val)
            y_min = val;
        //Zapamiętanie
        bufor[ind] = T(val); //Uwaga — bufor typu T, a val typu double
    } else
        bufor[ind] = T(get_missing());
}

template<class T> inline
void fifo_source<T>::bounds(size_t &num, double &min, double &max)
{
    check_version();
    Source->bounds(num, min, max);
    if(min < max)	// Zbadane wcześniej lub ustawione przez kogos
    {
        if(y_min > min) y_min = min;
        if(y_max < max) y_max = max;
    }
    num = N;
    min = y_min;
    max = y_max;
}


template<class T> inline
double fifo_source<T>::get(data_source_base::iterator_h &ptr_to_iterator)
//Daje następną z N liczb!!!
{
    size_t TN = bufor.get_size();
    size_t poz = _next(ptr_to_iterator);
    if(N == TN)
    {
        poz += next_one;
        //poz+=(Pierwszy); //-1? Bo to pierwszy, ale dla następnego kroku!!!
        poz %= TN; //Przewiniecie przez koniec bufora w razie co
    }
    return bufor[poz];
}

template<class T> inline
double fifo_source<T>::get(geometry::index_t index_from_geometry)
//Daje konkretna z N liczb, ale po przewinięciu działa do d... (???)
{
    if(index_from_geometry < N)
        return bufor[index_from_geometry];
    else
        return miss;
}

template<class T> inline
const char *fifo_source<T>::name()
//Musi zwracać zawsze aktualną kombinowaną nazwę serii
{
    const char *pom = Source->name();
    if(!_name.IsOK() || strstr(_name.get_ptr_val(), pom) == NULL)
        //Jeśli jeszcze nie ma albo zmieniło się w obiekcie źródła
    {
        _name.alloc(strlen(title_util::name()) + strlen(pom) + ZAPAS_NA_CYFRY + 1);
        sprintf(_name.get_ptr_val(), title_util::name(), pom, double(bufor.get_size()));
    }
    return _name.get_ptr_val();
}

}} // end-of-namespaces sym2::data

#pragma clang diagnostic pop
/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
#endif //SYMSHELL2_FIFO_SOUR_HPP_INCLUDED_




