/// @file
/// @brief __INTERFACES of the most basic data source classes__ /<br>
///         _INTERFACE-y najbardziej podstawowych klas źródeł danych._
/// @date 2026-05-16 (modified)
// *********************************************************************************************************************
//
#ifndef SYMSHELL2_DATA_SOURCES_HPP_INCLUDED_
#define SYMSHELL2_DATA_SOURCES_HPP_INCLUDED_

#include "sourbase.hpp" //Podstawowy interface

using wbrtm::wb_dynarray;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

namespace sym2 { namespace data {

/// @defgroup GRUPA_DATACLSS
/// @{

#ifdef USE_ENGLISH_IF_POSSIBLE
/// \EN{ A base class of data source passing a single data/value. }
class scalar_source_base : public data_source_base, public title_util
#else
/// \PL{ Klasa bazowa źródła danych przekazującego pojedynczą daną/wartość. }
class scalar_source_base : public data_source_base, public sym2::title_util
#endif
//-------------------------------------------------------------------
{
protected:
    int CheckMinMax; ///< Określa, czy należy sprawdzić min i max., czy są/będą podane.

    /// Constructor.
    /// @param nam to nazwa źródła.
    /// @param min to wymuszone minimum zakresu.
    /// @param max to wymuszone maksimum zakresu.
    explicit scalar_source_base(const char *nam, double min = 0, double max = 0)
    : title_util(nam)
    {
        y_min = min;
        y_max = max;
        CheckMinMax = !(y_min == 0 && y_max == 0);
    }

    /// Destructor.
    ~scalar_source_base() override = default;

public:
    /// Nazwa serii przechowywana w `title_util`.
    /// @return zwracać nazwę serii albo "" — NIE NULL!!!
    const char *name() override
    { return title_util::name(); }

    /// Podaje ile elementów w serii (zawsze 1), wartość minimalna i maksymalna.
    /// Zakres może być większy niż ta jedyna pamiętana liczba, bo użytkownik klasy może go zmienić.
    /// Zakres MOŻE też być pamięcią poprzednich wartości przechowywanych w tym obiekcie.
    void bounds(size_t &N, double &min, double &max) override
    { N = 1; min = y_min; max = y_max; }

    /// Umożliwia iteracje od początku.
    /// W tym przypadku iterator może zawierać tylko `1' albo NULL.
    iterator_h reset() override
    { return (iterator_h) 1; }

    /// Zwalnia iterator, czyli w tym przypadku zeruje go.
    void close(iterator_h &I) override
    { I = NULL; }


    double get(iterator_h &I) override = 0; /// @internal Nadal WYMAGA IMPLEMENTACJA.
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// Template class for typed single-element sources.
template<class T>
class template_scalar_source_base : public scalar_source_base
#else
/// Szablon klasa dla utypowionych źródeł jedno-elementowych.
template<class T>
class template_scalar_source_base : public scalar_source_base
#endif
//-----------------------------------------------------------
{
protected:
    /// Constructor.
    /// @param nam to nazwa źródła.
    /// @param min to wymuszone minimum zakresu.
    /// @param max  to wymuszone maksimum zakresu.
    explicit template_scalar_source_base(const char *nam, double min = 0, double max = 0)
    : scalar_source_base(nam, min, max)
    { miss = symshell2::default_missing<T>(); }

    /// Destructor.
    ~template_scalar_source_base() override = default;

public:
    using scalar_source_base::iterator_h;

    /// Implementacja get dla trywialnej iteracji jednego elementu.
    /// "Zwalnia" iterator i wywołuje wirtualne bezparametrowe `get`.
    double get(iterator_h &I) override
    {
        assert(I != nullptr); //Jak już zwolniony to nie powinien być ponownie wywołany.
        I = nullptr;
        return get();
    }

    /// Przetwarzanie indeksu uzyskanego z geometrii dla źródła skalarnego daje zawsze tę samą wartość.
    double get(symshell2::geometry::index_t ) override
    {
        return get();
    }

    /// WYMAGANA IMPLEMENTACJA dostępu do wartości. Mamy zawsze jedną wartość, więc można to uprościć.
    // TODO @return const T& albo T — TU TRZEBA ZMIENIĆ GDY `source_base` stanie się szablonem.
    virtual double get() = 0;

};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// A data source that filters data from another source.
class filter_source_base : public data_source_base, public title_util
#else
/// Źródło filtrujące dane z innego źródła.
class filter_source_base : public data_source_base, public title_util
#endif
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
    /// \param ini to źródło, z którego ma czerpać filtr.
    /// \param format to wzorzec budowania nazwy filtra z nazwy źródła.
    explicit filter_source_base( data_source_base *ini = NULL, const char *format = "F(%s)" )
    : title_util(format), Source(NULL), source_miss(INFINITY)
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
    const data_source_base *get_source() { return Source; }

// Virtual accessors:
// ------------------

    /// Sprawdza aktualność serii źródłowej i zwraca jej `data_version`.
    long data_version() override
    {
        check_version_();
        return data_source_base::data_version();
    }

    /// Sprawdza aktualność serii źródłowej i podaje, od ilu wersji dane się nie zmieniły.
    long how_old_data() override
    {
        check_version_();
        return data_source_base::how_old_data();
    }

    /// Podaje nazwę tego filtra utworzoną na podstawie formatu i nazwy jego źródła danych.
    const char *name() override;

    /// Zwraca wskaźnik do obowiązującej geometrii, która jest domyślnie taka, jak w źródle danych.
    geometry_base *get_geometry() override
    { return Source->get_geometry(); }

// DOSTĘP DO DANYCH:
// -----------------

    /// Implementacja domyślna sprawdza aktualność źródła danych i podaje jego bounds.
    void bounds(size_t &N, double &min, double &max) override
    {                 //Być może wartości te trzeba przekonwertować (?)
        check_version_();
        Source->bounds(N, min, max);
        if(y_min < y_max) //Jeśli ustawiono poprawnie to "overwrite"
        {
            min = y_min;
            max = y_max;
        }
    }

    /// Implementacja domyślna sprawdza aktualność źródła danych i podaje jego iterator.
    /// Dla pewności aktualizuje też `source_miss`.
    iterator_h reset() override
    {
        check_version_();  //Żeby źródło miało szanse na "update" wersji danych.
        iterator_h pom = Source->reset();
        source_miss = Source->get_missing(); //Dla pewności — może się zmieniło.
        return pom;
    }

    /// Implementacja domyślna zwalnia iterator, używając `close` ze źródła danych.
    void close(iterator_h &I) override { Source->close(I); }

    /// WYMAGANA implementacja iteracji z filtrowaniem.
    /// Najczęściej wystarczy przeliczyć wartość źródłową lub przekazać ją alno "missing".
    double get(iterator_h &I) override=0;

    /// WYMAGANA implementacja dostępu do wartości na podstawie indeksu z geometrii.
    /// Najczęściej wystarczy przeliczyć wartość źródłową lub przekazać ją alno "missing".
    double get(symshell2::geometry::index_t index) override=0;
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// Data source template filtering data from another source — for greater efficiency (reducing virtual calls?)
template<class SOURCE_TYPE>
class template_filter_source_base : public filter_source_base
#else
/// Szablon źródła filtrujące dane z innego źródła — dla większej efektywności (ograniczenie wywołań wirtualnych?)
template<class SOURCE_TYPE>
class template_filter_source_base : public filter_source_base
#endif
//-----------------------------------------------------------
{
protected:
    /// Constructor.
    /// @param ini to źródło, z którego ma czerpać filtr.
    /// @param format to sposób budowania nazwy filtra z nazwy źródła.
    explicit template_filter_source_base(SOURCE_TYPE *ini = NULL, const char *format = "F(%s)")
    : filter_source_base(ini, format)
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

    iterator_h reset() override
    {
        check_version_(); return reinterpret_cast<SOURCE_TYPE*>(Source)->reset();
    }

    void close(iterator_h &I) override
    { reinterpret_cast<SOURCE_TYPE*>(Source)->close(I); }

    /// Pobieranie danej za pomocą iteratora WYMAGA zaimplementowania w klasach potomnych.
    /// Przy kompilacji "Release" ta implementacja po prostu kopiuje wynik ze źródła, a przy "Debug" wyrzuca asercje.
    double get(iterator_h &I) override;

    /// Pobieranie danej za pomocą geometrii WYMAGA zaimplementowania w klasach potomnych.
    /// Przy kompilacji "Release" ta implementacja po prostu wywołuje czytanie z geometrii w źródle, a przy "Debug" wyrzuca asercje.
    double get(size_t index_from_geometry) override;

};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// Base class for sources passing data linearly.
class linear_source_base : public data_source_base, public title_util
#else
/// Klasa bazowa dla źródeł przekazujących dane liniowo.
class linear_source_base : public data_source_base, public title_util
#endif
//-------------------------------------------------------------------
{
protected:
    size_t N; ///< Ile elementów. Chronione, a nie prywatne, bo np. `fifo_source<> modyfikuje`.

    /// Constructor only for derived classes.
    /// @param i_N to oczekiwana liczba elementów (do sprawdzeń).
    /// @param i_tit to nazwa tego źródła danych.
    linear_source_base(size_t i_N, const char *i_tit)
    : N(i_N), title_util(i_tit)
    {}

    /// Wewnętrzna implementacja przemieszczenia iteratora o jednostkę.
    /// Zeruje, jeśli koniec tablicy.
    size_t _next(iterator_h &p) const
    {
        assert(p != NULL); //Nie wolno wywołać dla NULL
        size_t pom = ((size_t) p) - 1;
        if(pom + 1 >= N)
            p = NULL;
        else
            p = (iteratorh) (pom + 2);
        return pom;
    }

    /// Virtual destructor.
    ~linear_source_base() override = default;

public:
    // Methods:
    //=========

    /// Do WYMUSZANIA ZMIANY ROZMIARU SERII.
    /// @note STOSOWAĆ Z WIELKĄ OSTROŻNOŚCIĄ. PRZYDAJE SIĘ TYLKO, GDY SERIA JEST UCHWYTEM DO ZEWNĘTRZNYCH DANYCH!
    virtual void _change_size(size_t New_N) //Be careful!!!
    {
        N = New_N;
    }

    /// Akcesor dostępu do oczekiwanego rozmiaru ciągu danych.
    size_t get_size() const { return N; }

    const char *name() override { return title_util::name(); }

    iterator_h reset() override { return (iterator_h) 1; }

    void close(iterator_h &p) override { p = NULL; }

    /// Wymagana implementacja iteracji liniowej.
    double get(iterator_h &I) override=0;

    /// Wymagana implementacja pobierania danej na podstawie indeksu z geometrii.
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
public:
    typedef symshell2::rectangle_geometry rect_geometry;
private:
    symshell2::rectangle_geometry *my_geometry; ///< Wskaźnik do geometrii danych.
    bool             local_geometry; ///< Określa CZY geometria jest jego własna, czy pożyczona.

protected:
    /// Constructor with private geometry/Konstruktor z prywatną geometrią.
    /// \param i_tit to nazwa serii danych.
    /// \param i_A, i_B wymiary tablicy danych dla geometrii.
    /// \param i_torus określa, czy włączyć geometrie torusa. Lepsze, bo wtedy nie trzeba używać "miss".
    /// \param i_miss to nietypowa wartość podawana przy skanowaniu wycinka wychodzącego poza macierz.
    /// \param i_section czteroelementowa prosta tablica C ustalająca wycinek danych: {start_X, len_X, start_Y, len_Y}.
    /// @note `i_section` tutaj nie działa! TODO!
    rectangle_source_base(
            const char  *i_tit, size_t i_A, size_t i_B, int i_torus,
            const int   *i_section = NULL, //AKTUALNIE NIE UŻYWANE, ALE MOŻE KIEDYŚ ZNOWU.
            double       i_miss = symshell2::default_missing<double>()
            )
    : title_util(i_tit), my_geometry(NULL), local_geometry(false)
    {
        set_missing(i_miss);
        my_geometry = new symshell2::rectangle_geometry(i_A, i_B, i_torus);
        assert(my_geometry != NULL);
        local_geometry = true;
        //TODO What about `i_section`? WYPADŁO Z UŻYCIA.
        //my_geometry.set_view_info(NULL); //Reset a default
    }

    /// Constructor with borrowed geometry/Konstruktor z zapożyczoną geometrią.
    /// \param i_tit to nazwa serii danych.
    /// \param geom to referencja do geometrii. Jest zapamiętywany adres!!! Nie będzie dealokowany.
    /// \param i_miss to nietypowa wartość podawana przy skanowaniu wycinka wychodzącego poza macierz.
    rectangle_source_base(
            const char *i_tit,
            symshell2::rectangle_geometry &geom,  //Geometria z zewnątrz — dealokacja nie będzie zarządzana
            double i_miss = symshell2::default_missing<double>(),
            const int *i_section = NULL // NIE UŻYWANE, ALE MOŻE KIEDYŚ...
            )
    : title_util(i_tit), my_geometry(NULL), local_geometry(false) //Nie będzie zarządzać dealokacją geometrii.
    {
        set_missing(i_miss);
        my_geometry = &geom;
        assert(my_geometry != NULL);
        //my_geometry.set_view_info(NULL); //Reset a default
    }

    /// Korzystając z geometrii, zwraca indeks do aktualnego elementu i przesuwa iterator.
    /// @returns `FULL`, jeśli brak danej... @note Zeruje iterator, jeśli koniec danych.
    size_t _next(iterator_h &p)
    {
        return my_geometry->get_next(p);
    }

    /// Destruktor zwalnia geometrię, jeśli nie jest pożyczona.
    ~rectangle_source_base() override
    {
        if(local_geometry) //Czy to własna geometria, czy "pożyczona"?
            delete my_geometry;
    }

public:
    /// Zwraca nazwę serii, po prostu czytając z bazowego `title_util`.
    const char *name() override { return title_util::name(); }

    /// Czyta wskaźnik do obowiązującej geometrii danych.
    /// @return NULL, jeśli dane nie mają znanej geometrii.
    geometry_base *get_geometry() override
    {
        assert(my_geometry != NULL); // NULL oznaczałby dane nie-zgeometryzowane
        return my_geometry;
    }

    /// Non-virtual (!!!) shortcut.
    /// @returns geom-ptr `my_geometry`.
    rect_geometry *get_rect_geometry()
    {
        assert(my_geometry != NULL);
        return my_geometry;
    }

    /// Korzystając z geometrii, tworzy iterator po wszystkich obiektach/agentach.
    iterator_h reset() override
    { return my_geometry->make_global_iterator(); }

    /// Korzystając z geometrii, usuwa iterator.
    void close(iterator_h &p) override
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
class function_source_base : public data_source_base, public title_util
#else
/// Klasa podstawowa źródła funkcyjnego. Żeby zminimalizować liczbę powtarzających się metod.
class function_source_base : public data_source_base, public title_util
#endif
//---------------------------------------------------------------------
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
    function_source_base(size_t iN, double i_x_min, double i_x_max,
                         const char *i_tit,
                         double i_y_min, double i_y_max)
    : N(iN), title_util(i_tit),x_min(i_x_min), x_max(i_x_max) //pola własne.
    {
        y_min = i_y_min;
        y_max = i_y_max; //pola dziedziczone.
        assert(x_min < x_max);
        assert(y_min <= y_max); //Jeśli równe to klasa potomna musi liczyć.
        step = (x_max - x_min) / double(N - 1); // Obliczenie długości kroku próbkowania.
    }

    /// Destructor.
    ~function_source_base() override = default;

public:
    /// Zwraca nazwę serii, po prostu czytając z bazowego `title_util`.
    const char *name() override { return title_util::name(); }

    /// Ta implementacja po prostu czyta to, co ma zapisane w atrybutach.
    void bounds(size_t &num, double &min, double &max) override
    { num = N; min = y_min; max = y_max; }

    /// Dostarcza iterator ustawiony na 0.
    /// Nie należy sprawdzać, aby, czy nie zwrócił NULL, bo właśnie to jest to samo!
    /// Jednakże dla funkcji to po prostu oznaczenie początku.
    iterator_h reset() override { return 0; }

    /// Po prostu zeruje iterator. Co przypadkiem pozwala zacząć od początku.
    /// Taki efekt nie był zamierzony, ale tak wyszło.
    void close(iterator_h &p) override { p = NULL; }

};

/// @}

// TODO Szablon źródła funkcyjnego. Sparametryzowany typem funkcyjnym.
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
double filter_source_base::get(iterator_h &I)
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
    assert(!"Random access 'get()' not implemented");
    return Source->get(index_from_geometry); //Używane w trybie "Release.
}

template<class SOURCE_TYPE> inline
double template_filter_source_base<SOURCE_TYPE>::get(size_t index_from_geometry)
{
    assert(!"Random access get() not implemented"); //DEBUG
    return reinterpret_cast<SOURCE_TYPE*>(Source)->get(index_from_geometry);
}

template<class SOURCE_TYPE> inline
double template_filter_source_base<SOURCE_TYPE>::get(iterator_h &I)
{
    assert(!"Linear access get() not implemented");
    return reinterpret_cast<SOURCE_TYPE*>(Source)->get(I);
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
#endif //SYMSHELL2_DATA_SOURCES_HPP_INCLUDED_




