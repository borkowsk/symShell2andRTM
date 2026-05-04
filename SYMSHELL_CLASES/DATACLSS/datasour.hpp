/// @file
/// @brief INTERFACES of the most basic data source classes /
///        INTERFACE-y najbardziej podstawowych klas źródeł danych.
/// @date 2026-05-04 (modified)
// *********************************************************************************************************************
//
#ifndef SYMSHELL2_DATA_SOURCES_HPP_INCLUDED_
#define SYMSHELL2_DATA_SOURCES_HPP_INCLUDED_

#ifndef __cplusplus
#error C++ required
#endif

#include "sourbase.hpp" //Podstawowy interface

using wbrtm::wb_dynarray;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

#ifdef USE_ENGLISH_IF_POSSIBLE
/// A base class of data source passing a single data/value.
#else
/// Klasa bazowa źródła danych przekazującego pojedynczą daną/wartość.
#endif
class scalar_source_base : public data_source_base, public title_util
//-----------------------------------------------------------------
{
#if __GNUC__ >= 3 //Problemy z widocznością poniższej zmiennej, ale i tak nie pomaga!!!
public:
#else
protected:
#endif
    int CheckMinMax; ///< Określa, czy należy sprawdzić min i max., czy są/będą podane.

    /// Constructor.
    /// @param nam to nazwa źródła.
    /// @param min to wymuszone minimum zakresu.
    /// @param max to wymuszone maksimum zakresu.
    explicit scalar_source_base(const char *nam, double min = 0, double max = 0) :
            title_util(nam)
    {
        y_min = min;
        y_max = max;
        CheckMinMax = !(y_min == 0 && y_max == 0);
    }

    /// Destructor.
    ~scalar_source_base() override = default;

public:
    /// Nazwa serii przechowywana w `title_util`. @return zwracać nazwę serii albo "" — NIE NULL!!!
    const char *name() override
    { return title_util::name(); }

    /// Podaje ile elementów w serii (zawsze 1), wartość minimalna i maksymalna.
    /// Zakres może być większy niż ta jedyna pamiętana liczba, bo użytkownik klasy może go zmienić.
    /// Zakres MOŻE też być pamięcią poprzednich wartości przechowywanych w tym obiekcie.
    void bounds(size_t &N, double &min, double &max) override
    {
        N = 1;
        min = y_min;
        max = y_max;
    }

    /// Umożliwia iteracje od początku.
    /// W tym przypadku iterator może zawierać tylko `1' albo NULL.
    iteratorh reset() override
    { return (iteratorh) 1; }

    /// Zwalnia, czyli w tym przypadku zeruje iterator.
    void close(iteratorh &I) override
    { I = 0; }

    /// Daje następną, czyli w tym przypadku jedyną liczbę.
    /// Ta metoda do podstawienia w klasach potomnych.
    /// Poza podaniem liczby metoda powinna zwolnić, czyli wyzerować iterator!
    double get(iteratorh &I) override = 0;
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// Template class for typed single-element sources.
#else
/// Szablon klasa dla utypowionych źródeł jedno-elementowych.
#endif
template<class T>
class template_scalar_source_base : public scalar_source_base
//---------------------------------------------------------
{
protected:
    /// Constructor
    /// @param nam to nazwa źródła.
    /// @param min to wymuszone minimum zakresu.
    /// @param max  to wymuszone maksimum zakresu.
    explicit template_scalar_source_base(const char *nam, double min = 0, double max = 0) :
            scalar_source_base(nam, min, max)
    { miss = default_missing<T>(); }

    /// Destructor.
    ~template_scalar_source_base() override = default;

public:
    /// Implementacja get dla trywialnej iteracji jednego elementu.
    /// "Zwalnia" iterator i wywołuje wirtualne bezparametrowe `get`.
    double get(iteratorh &I) override
    {
        assert(I != nullptr); //Jak już zwolniony to nie powinien być ponownie wywołany.
        I = nullptr;
        return get();
    }

    /// Mamy zawsze jedną wartość, więc można uprościć dostęp.
    // TODO @return const T& albo T — TU TRZEBA ZMIENIĆ GDY `source_base` stanie się szablonem.
    virtual double get() = 0;
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// A data source that filters data from another source.
#else
/// Źródło filtrujące dane z innego źródła.
#endif
class filter_source_base : public data_source_base, public title_util
//-------------------------------------------------------------------
{
protected:
    /// @name Atrybuty chronione, dosyć mocno nadużywane przez klasy potomne.
    /// @{
    wb_dynarray<char>  _name; ///< Utworzona nazwa, zbudowana z formatu i nazwy źródła.
    data_source_base *Source; ///< Źródło danych, z którego czerpie filtr.
    double       source_miss; ///< Wartość missing źródła danych.
    /// @}

    /// Wewnętrzne sprawdzenie, czy i jak zmieniły się dane w źródle.
    /// @return 1, jeśli źródło dla filtra zmieniło wersje, jednak zwraca 0, jeśli wersje się nadal zgadzają.
    /// @note W filtrach cache-ujących może powodować opróżnienie lub ponowne napełnienie!
    virtual int check_version_()
    {
        return update_version_from(Source);
    }

    /// Pomocnicze uzupełnienie o sprawdzanie, czy `Source->get` nie dało missing.
    virtual int from_source_is_missing_(double val)
    {
        if(val == source_miss)
            return 1;
        else
            return 0;
    }

    /// Constructor.
    explicit filter_source_base( data_source_base *ini = NULL, ///< Źródło, z którego ma czerpać filtr.
                                 const char *format = "F(%s)"  ///< Wzorzec budowania nazwy filtra z nazwy źródła.
        ) :
            title_util(format), Source(NULL), source_miss(INFINITY)
    {
        set_source(ini);
    }

    /// Virtual destructor.
    ~filter_source_base() override = default;

public:
    /// Ustawia źródło dla filtra i zapamiętuje jego wartość "missing".
    void set_source(data_source_base *ini)
    {
        Source = ini;
        source_miss = Source->get_missing();
    }

    /// Zwraca wskaźnik do seri źródłowej.
    const data_source_base *get_source()
    { return Source; }

// Virtual accessors:
// ------------------

    long data_version() override //numer wersji danych.
    {
        check_version_(); // Wewnętrzne sprawdzenie, czy i jak zmieniły się dane w źródle.
        return data_source_base::data_version();
    }

    long how_old_data() override //od ilu wersji dane się nie zmieniły.
    {
        check_version_(); // Wewnętrzne sprawdzenie, czy i jak zmieniły się dane w źródle.
        return data_source_base::how_old_data();
    }

    const char *name() override; //Musi zwracać nazwę serii albo "" — NIE NULL!!!

    geometry_base *get_geometry() override //Zwraca wskaźnik do obowiązującej geometrii danych
    { return Source->get_geometry(); } //domyślnie taka jak w źródle.

    // DOSTĘP DO DANYCH:
    // -----------------

    void bounds(size_t &N, double &min, double &max) override //Ile elementów, wartość minimalna i maksymalna
    {                                                    //Być może wartości te trzeba przekonwertować (?)
        check_version_();
        Source->bounds(N, min, max);
        if(y_min < y_max) //Jeśli ustawiono poprawnie to "overwrite"
        {
            min = y_min;
            max = y_max;
        }
    }

    iteratorh reset() override // Umożliwia czytanie od początku — `iteratorh` jest uchwytem iteratora.
    {
        check_version_();  //Żeby źródło miało szanse na "update" wersji danych.
        iteratorh pom = Source->reset();
        source_miss = Source->get_missing(); //Dla pewności — może się zmieniło.
        return pom;
    }

    void close(iteratorh &I) override //Obiekt źródłowy zwalnia iterator, jeśli nie został zwolniony przez `get`.
    { Source->close(I); }

    /// Implementacja iteracji z filtrowanie.
    double get(iteratorh &I) override=0;

    /// Ta metoda jest też do podstawienia, bo też musi filtrować.
    double get(size_t index_from_geometry) override=0;
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// Data source template filtering data from another source — for greater efficiency (reducing virtual calls?)
#else
/// Szablon źródła filtrujące dane z innego źródła — dla większej efektywności (ograniczenie wywołań wirtualnych?)
#endif
template<class SOURCE_TYPE>
class template_filter_source_base : public filter_source_base
//-----------------------------------------------------------
{
protected:
    /// Constructor.
    /// @param ini to źródło, z którego ma czerpać filtr.
    /// @param format to sposób budowania nazwy filtra z nazwy źródła.
    explicit template_filter_source_base(SOURCE_TYPE *ini = NULL, const char *format = "F(%s)") :
            filter_source_base(ini, format)
    {}

    /// Virtual destructor.
    ~template_filter_source_base() override = default;

public:

    void bounds(size_t &N, double &min, double &max) override
    {
        check_version_();
        reinterpret_cast<SOURCE_TYPE*>(Source)->bounds(N, min, max);
        if(y_min < y_max)	//Jeśli są ustawione poprawnie to "overwrite"(?)
        {
            min = y_min;
            max = y_max;
        }
    }

    iteratorh reset() override
    {
        check_version_(); return reinterpret_cast<SOURCE_TYPE*>(Source)->reset();
    }

    void close(iteratorh &I) override
    { reinterpret_cast<SOURCE_TYPE*>(Source)->close(I); }

    double get(iteratorh &I) override
    // Wymaga realnego zaimplementowania w klasach potomnych.
    // Przy kompilacji "Release" ta metoda po prostu kopiuje wynik ze źródła
    //, a przy debug wyrzuca asercje.
    {
        assert(!"Linear access get() not implemented");
        return reinterpret_cast<SOURCE_TYPE*>(Source)->get(I);
    }

    double get(size_t index_from_geometry) override
    // Wymaga realnego zaimplementowania w klasach potomnych.
    // Przy kompilacji "Release" ta metoda po prostu kopiuje wynik ze źródła
    //, a przy debug wyrzuca asercje.
    {
        assert(!"Random access get() not implemented"); //DEBUG
        return reinterpret_cast<SOURCE_TYPE*>(Source)->get(index_from_geometry);
    }

};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// Base class for sources passing data linearly.
#else
/// Klasa bazowa dla źródeł przekazujących dane liniowo.
#endif
class linear_source_base : public data_source_base, public title_util
//--------------------------------------------------------
{
protected:
    size_t N; ///< Ile elementów. Chronione, a nie prywatne, bo np. `fifo_source<> modyfikuje`.

    /// Constructor only for derived classes.
    /// @param i_N to oczekiwana liczba elementów (do sprawdzeń).
    /// @param i_tit to nazwa tego źródła danych.
    linear_source_base(size_t i_N, const char *i_tit) :
            N(i_N), title_util(i_tit)
    {}

    /// Wewnętrzna implementacja przemieszczenia iteratora o jednostkę. Zeruje, jeśli koniec tablicy.
    size_t _next(iteratorh &p) const
    {
        assert(p != NULL); //Nie wolno wywołać dla NULL
        size_t pom = ((size_t) p) - 1;
        if(pom + 1 >= N)
            p = NULL;
        else
            p = (iteratorh) (pom + 2);
        return pom;
    }

    /// Do WYMUSZANIA ZMIANY ROZMIARU SERII.
    /// PRZYDAJE SIĘ TYLKO, GDY SERIA JEST UCHWYTEM DO ZEWNĘTRZNYCH DANYCH!
    virtual void _change_size(size_t New_N) //Be careful!!!
    {
        N = New_N;
    }

    /// Virtual destructor.
    ~linear_source_base() override = default;

public:
    // Methods:
    //=========

    /// Akcesor dostępu do oczekiwanego rozmiaru ciągu danych.
    size_t get_size() const
    { return N; }

    /// Akcesor nazwy serii.
    const char *name() override
    { return title_util::name(); }

    iteratorh reset() override
    { return (iteratorh) 1; }

    void close(iteratorh &p) override
    { p = NULL; }

    /// Implementacja iteracji liniowej.
    double get(iteratorh &I) override=0;

    /// Ta metoda jest też do podstawienia.
    double get(size_t index_from_geometry) override=0;
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// Base class for sources providing rectangular data with slices.
#else
/// Klasa bazowa dla źródeł udostępniających dane prostokątne wraz z wycinkami.
#endif
class rectangle_source_base : public data_source_base, public title_util
//--------------------------------------------------------------
{
private:
    rectangle_geometry *my_geometry; ///< Wskaźnik do geometrii danych.
    bool             local_geometry; ///< Określa CZY geometria jest jego własna, czy pożyczona.

protected:
    /// Constructor with private geometry/Konstruktor z prywatną geometrią.
    /// \param i_tit to nazwa serii danych.
    /// \param i_A, i_B wymiary tablicy danych dla geometrii.
    /// \param i_torus określa, czy włączyć geometrie torusa. Lepsze, bo wtedy nie trzeba używać "miss".
    /// \param i_subs czteroelementowa tablica C ustalająca wycinek danych: {start_X, len_X, start_Y, len_Y}.
    /// \param i_miss to nietypowa wartość podawana przy skanowaniu wycinka wychodzącego poza macierz.
    /// @note `i_subs` tutaj nie działa! TODO!
    rectangle_source_base(
            const char *i_tit,
            size_t i_A, size_t i_B, int i_torus,
            int *i_subs = NULL,double i_miss = default_missing<double>()
            ) :
            title_util(i_tit), my_geometry(NULL), local_geometry(false)
    {
        set_missing(i_miss);
        my_geometry = new rectangle_geometry(i_A, i_B, i_torus);
        assert(my_geometry != NULL);
        local_geometry = true;
        //TODO What about `i_subs`?
        //my_geometry.set_view_info(NULL); //Reset a default
    }

    /// Constructor with borrowed geometry/Konstruktor z zapożyczoną geometrią.
    /// \param i_tit to nazwa serii danych.
    /// \param geom to referencja do geometrii. Jest zapamiętywany adres!!!
    /// \param i_miss to nietypowa wartość podawana przy skanowaniu wycinka wychodzącego poza macierz.
    rectangle_source_base(
            const char *i_tit,
            rectangle_geometry &geom,  //Geometria z zewnątrz — dealokacja nie będzie zarządzana
            double i_miss = default_missing<double>()	//Wartość podawana przy
            //skanowaniu wycinka wychodzącego poza macierz
            ) :
            title_util(i_tit), my_geometry(NULL), local_geometry(false) //Nie będzie zarządzać dealokacją geometrii.
    {
        set_missing(i_miss);
        my_geometry = &geom;
        assert(my_geometry != NULL);
        //my_geometry.set_view_info(NULL); //Reset a default
    }

    /// Korzystając z geometrii, zwraca indeks do aktualnego elementu i przesuwa iterator.
    /// @returns `ULONG_MAX`/`FULL`, jeśli brak danej... @note Zeruje iterator, jeśli koniec danych.
    size_t _next(iteratorh &p)
    {
        return my_geometry->get_next(p);
    }

    ~rectangle_source_base() override
    {
        if(local_geometry) //Czy to własna geometria, czy "pożyczona"?
            delete my_geometry;
    }

public:
    const char *name() override    //Zwraca nazwę serii.
    { return title_util::name(); }

    geometry_base *get_geometry() override // Zwraca wskaźnik do obowiązującej geometrii danych.
    {
        assert(my_geometry != NULL); // NULL oznaczałby dane nie-zgeometryzowane
        return my_geometry;
    }

    rectangle_geometry *get_rect_geometry() //Non virtual (!!!) shortcut
    {
        assert(my_geometry != NULL);
        return my_geometry;
    }

    /// Korzystając z geometrii, tworzy iterator.
    iteratorh reset() override
    { return my_geometry->make_global_iterator(); }

    /// Korzystając z geometrii, usuwa iterator.
    void close(iteratorh &p) override
    { my_geometry->destroy_iterator(p); }

    //Stare podawanie parametrów geometrii źródła na `out_tab` i liczby wymiarów.
    /*
    int box(int* out_tab)
    {
        if(out_tab)
        {
            out_tab[0]=my_geometry.get_width();
            out_tab[1]=my_geometry.get_height();
        }
        return 2;
    };
    */
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// Base class for the function source. To minimize the number of duplicate methods.
#else
/// Klasa podstawowa źródła funkcyjnego. Żeby zminimalizować liczbę powtarzających się metod.
#endif
class function_source_base : public data_source_base, public title_util
//-----------------------------------------------------------------------------------
{
protected:
    double x_min; ///< Początek zadanego zakresu X.
    double x_max; ///< Koniec zadanego zakresu X.
    double  step; ///< Rozmiar kroku próbkującego.

    size_t    N; ///< Liczba wirtualnych punktów danych.

    /// Constructor.
    /// \param iN to oczekiwana liczba elementów (punktów danych).
    /// \param i_x_min, i_x_max to zakres po X-ach.
    /// \param i_tit to nazwa funkcji, do podpisu na wykresie.
    /// \param i_y_min, i_y_max to oczekiwany zakres na Y-ach. Oszczędza liczenia.
    function_source_base(size_t iN,
                         double i_x_min, double i_x_max,
                         const char *i_tit,
                         double i_y_min, double i_y_max
            ) :
            N(iN), title_util(i_tit),
            x_min(i_x_min), x_max(i_x_max) //pola własne.
    {
        y_min = i_y_min;
        y_max = i_y_max; //pola dziedziczone.
        assert(x_min < x_max);
        assert(y_min <= y_max); //Jeśli równe to klasa potomna musi liczyć.
        step = (x_max - x_min) / double(N - 1); // Obliczenie długości kroku próbkowania.
    }


public:

    const char *name() override    // Zwraca nazwę serii.
    { return title_util::name(); }

    void bounds(size_t &num, double &min, double &max) override
    {
        num = N;
        min = y_min;
        max = y_max;
    }

    iteratorh reset() override
    { return 0; }

    void close(iteratorh &p) override
    {  p = NULL; }

};

// Szablon źródła funkcyjnego.
// Sparametryzowany typem funkcyjnym.
// @parametr F musi być klasą z bezparametrowym konstruktorem i
// metodą: `double operator () (double) `.
// Może mieć natomiast dowolne pola pomocnicze.

//template<class F>
//class function_source:function_source_base  --- #include "func-sour.hpp"
//{...};

//	INLINE IMPLEMENTATION(S):
// //////////////////////////

inline
const char* filter_source_base::name()
//Musi zwracać nazwę serii albo "" - NIE NULL!!!
{
    const char *pom = Source->name();
    if(!_name.IsOK() || strstr(_name.get_ptr_val(), pom) == NULL)
        //Jeśli jeszcze nie ma albo zmieniło się w obiekcie źródła.
    {
        _name.alloc(strlen(title_util::name()) + strlen(pom) + 1);
        sprintf(_name.get_ptr_val(), title_util::name(), pom);
    }
    return _name.get_ptr_val();
}

inline
double filter_source_base::get(iteratorh &I)
//Daje następną z N liczb!!! Po N-tej obiekt źródłowy zwalnia iterator!
//Ta metoda do podstawienia.
{
    assert(!"Linear access get() not implemented");
    return Source->get(I); //Używane w trybie "Release.
}

inline
double filter_source_base::get(size_t index_from_geometry)
//Przetwarza index uzyskany z geometrii
//na wartość z serii, o ile jest możliwe czytanie losowe.
{
    assert(!"Random access get() not implemented");
    return Source->get(index_from_geometry); //Używane w trybie "Release.
}

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
#endif //SYMSHELL2_DATA_SOURCES_HPP_INCLUDED_




