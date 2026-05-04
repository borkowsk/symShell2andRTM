/// @file
/// @brief INTERFACE-y najbardziej podstawowych klas źródeł.
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

/// Klasa źródła przekazującego pojedynczą daną.
class scalar_source_base : public data_source_base, public title_util
//-----------------------------------------------------------------
{
#if __GNUC__ >= 3 //Problemy z widocznością poniższej zmiennej, ale i tak nie pomaga!!!
public:
#else
    protected:
#endif
    int CheckMinMax;

public:
/// Constructor.
    explicit scalar_source_base(const char *nam, double min = 0, double max = 0) :
            title_util(nam)
    {
        ymin = min;
        ymax = max;
        CheckMinMax = !(ymin == 0 && ymax == 0);
    }

    const char *name() override //Musi zwracać nazwę serii albo "" - NIE NULL!!!
    { return title_util::name(); }

    void bounds(size_t &N, double &min, double &max) override
    //Ile elementów, wartość minimalna i maksymalna
    //sczytane z wewnętrznych pol. UWAGA: Może być `min == max == 0`
    {
        N = 1;
        min = ymin;
        max = ymax;
    }

/// Umożliwia czytanie od początku — `iteratorh` jest uchwytem iterator-a.
/// Domyślnie z obiektu Source, ale czasem nie.
    iteratorh reset() override
    { return (iteratorh) 1; }

    virtual double get(iteratorh &I) = 0;
    //Daje następną, czyli jedyna liczby!!!
    //Po czym obiekt źródłowy zwalnia iterator!
    //Ta metoda do podstawienia

    void close(iteratorh &I) override
    //Obiekt źródłowy zwalnia iterator, jeśli nie został zwolniony przez `get(N)`
    { I = 0; }

};

/// Klasa podstawowa dla utypowionych źródeł jedno-elementowych.
template<class T>
class template_scalar_source_base : public scalar_source_base
//---------------------------------------------------------
{
public:
//Constructor
    explicit template_scalar_source_base(const char *nam, double min = 0, double max = 0) :
            scalar_source_base(nam, min, max)
    { miss = default_missing<T>(); }

    double get(iteratorh &I) override //"Zwalnia"(?) iterator i wywołuje wirtualne `get()`
    {
        assert(I != nullptr); //Jak już zwolniony to nie powinien być wywołany.
        I = nullptr;
        return get();
    }

//const T&  - - - TU TRZEBA ZMIENIĆ GDY `source_base` stanie szablonem
    virtual double get() = 0; //Ma zawsze jedna wartość, więc można tędy uprościć dostęp.

};

/// Źródło filtrujące dane z innego źródła.
class filter_source_base : public data_source_base, public title_util
//-----------------------------------------------------------------
{
protected:
    data_source_base *Source;
    double source_miss;
    wb_dynarray<char> _name;

    virtual int check_version()
    // Sprawdza,, czyi jak zmieniły się dane w źródle.
    // W filtrach cache'ujących może powodować opróżnienie lub ponowne napełnienie.
    // Zwraca 1, jeśli zmieniło wersje.
    // Jednakże zwraca 0, jeśli wersje się nadal zgadzają.
    {
        return update_version_from(Source);
    }

public:
/// Constructor.
    explicit filter_source_base(data_source_base *ini = NULL, const char *format = "F(%s)") :
            title_util(format), Source(NULL), source_miss(INFINITY)
    {
        set_source(ini);
    }

    void set_source(data_source_base *ini)
    {
        Source = ini;
        source_miss = Source->get_missing();
    }

    const data_source_base *get_source() //Zwraca wskaźnik do seri źródłowej.
    { return Source; }

// Virtual accessors:
// ------------------

    long data_version() override //numer wersji danych.
    {
        check_version();
        return data_source_base::data_version();
    }

    long how_old_data() override //od ilu wersji dane się nie zmieniły.
    {
        check_version();
        return data_source_base::how_old_data();
    }

    const char *name() override; //Musi zwracać nazwę serii albo "" - NIE NULL!!!


    geometry_base *getgeometry() override //Zwraca wskaźnik do obowiązującej geometrii danych
    { return Source->getgeometry(); } //domyślnie taka jak w źródle.


// DOSTĘP DO DANYCH:
// -----------------

    void bounds(size_t &N, double &min, double &max) override //Ile elementów, wartość minimalna i maksymalna
    {                                                    //Być może wartości te trzeba przekonwertować.
        check_version();
        Source->bounds(N, min, max);
        if(ymin < ymax) //Jeśli ustawiono to "overwrite"
        {
            min = ymin;
            max = ymax;
        }
    }

    iteratorh reset() override
    // Umożliwia czytanie od początku — `iteratorh` jest uchwytem iterator-a
    // domyślnie z obiektu Source, ale czasem nie
    {
        check_version();  //Żeby źródło miało szanse na "update".
        iteratorh pom = Source->reset();
        source_miss = Source->get_missing(); //Dla pewności — może się zmieniło.
        return pom;
    }

    virtual double get(iteratorh &I)
    //Daje następną z N liczb!!! Po N-tej obiekt źródłowy zwalnia iterator!
    //Ta metoda zapewne najczęściej do podstawienia.
    {
        assert(!"Linear access get() not implemented");
        return Source->get(I);
    }

    virtual double get(size_t index_from_geometry)
    //Przetwarza index uzyskany z geometrii
    //na wartość z serii, o ile jest możliwe czytanie losowe.
    //Ta metoda jest też najczęściej do podstawienia.
    {
        assert(!"Random access get() not implemented");
        return Source->get(index_from_geometry);
    }

    void close(iteratorh &I) override
    //Obiekt źródłowy zwalnia iterator, jeśli nie został zwolniony przez get(N)
    { Source->close(I); }

/// Uzupełnienie o sprawdzanie,, czySource->get nie dało missing.
    virtual int FromSourceIsMissing(double val)
    {
        if(val == source_miss)
            return 1;
        else
            return 0;
    }
};

/// Szablon źródła filtrujące dane z innego źródła — dla większej efektywności.
template<class SOURCE_TYPE>
class template_filter_source_base : public filter_source_base
//---------------------------------------------------
{
public:
/// Constructor.
    explicit template_filter_source_base(SOURCE_TYPE *ini = NULL, const char *format = "F(%s)") :
            filter_source_base(ini, format)
    {}

    void bounds(size_t &N, double &min, double &max) override
    //Ile elementów, wartość minimalna i maksymalna
    //Być może wartości te trzeba przekonwertować.
    {
        check_version();
        ((SOURCE_TYPE *) Source)->bounds(N, min, max);
        if(ymin < ymax)	//Jeśli są ustawione to "overwrite"
        {
            min = ymin;
            max = ymax;
        }
    }

    iteratorh reset() override
    //Umożliwia czytanie od początku — `iteratorh` jest uchwytem iterator-a, czyli domyślnie z obiektu `Source`.
    {
        check_version();
        return ((SOURCE_TYPE *) Source)->reset();
    }

    double get(iteratorh &I)
    //Daje następna z N liczb!!! Po N-tej obiekt źródłowy zwalnia iterator!
    //Ta metoda zapewne najczęściej do podstawienia
    {
        assert(!"Linear access get() not implemented");
        return ((SOURCE_TYPE *) Source)->get(I);
    }

    double get(size_t index_from_geometry)
    //Przetwarza index uzyskany z geometrii
    //na wartość z serii, o ile jest możliwe czytanie losowe
    //Ta metoda tez najczęściej do podstawienia
    {
        assert(!"Random access get() not implemented"); //DEBUG
        return ((SOURCE_TYPE *) Source)->get(index_from_geometry);
    }

    void close(iteratorh &I) override
    //Obiekt źródłowy zwalnia iterator, jeśli nie został zwolniony przez get(N)
    { ((SOURCE_TYPE *) Source)->close(I); }

};

/// Klasa bazowa dla źródeł przekazujących dane liniowo.
class linear_source_base : public data_source_base, public title_util
//--------------------------------------------------------
{
protected:
    size_t N; ///< Ile elementów.

// Constructor
    linear_source_base(size_t iN, const char *itit) :
            N(iN), title_util(itit)
    {}

// Przemieszcza iterator o jednostkę. Zeruje, jeśli koniec tablicy.
    size_t _next(iteratorh &p)
    {
        assert(p != NULL); //Nie wolno wywołać dla NULL
        size_t pom = ((size_t) p) - 1;
        if(pom + 1 >= N)
            p = NULL;
        else
            p = (iteratorh) (pom + 2);
        return pom;
    }

public:
// Methods:
//=========

    virtual void _change_size(size_t NewN) //Be careful!!!
    { //WYMUSZA ZMIANĘ ROZMIARU SERII. PRZYDAJE SIĘ TYLKO GDY SERIA
        //JEST UCHWYTEM DO ZEWNĘTRZNYCH DANYCH!
        N = NewN;
    }

    size_t get_size()
    { return N; }

    const char *name() override    //Zwraca nazwę serii.
    { return title_util::name(); }

    iteratorh reset() override
//Umożliwia czytanie od początku
    { return (iteratorh) 1; }

    void close(iteratorh &p) override
    {
        p = NULL;
    }

};

/// Klasa bazowa dla źródeł udostępniających dane prostokątne wraz z wycinkami.
class rectangle_source_base : public data_source_base, public title_util
//--------------------------------------------------------------
{
private:
    rectangle_geometry *my_geometry;
    bool local_geometry;

protected:

/// Constructor with private my_geometry.
    rectangle_source_base(
            const char *i_tit,
            size_t iA, size_t iB,
            int i_torus,     //Określa,, czywłączyć geometrie torusa. Default, bo wtedy nie trzeba używać "miss".
            int *i_subs = NULL,     //Ustala wycinek tablicy: `startX, lenX, startY, lenY`.
            double i_miss = default_missing<double>() //Wartość podawana przy
            //skanowaniu wycinka wychodzącego poza macierz.
    ) :
            title_util(i_tit), my_geometry(NULL), local_geometry(false)
    {
        set_missing(i_miss);
        my_geometry = new rectangle_geometry(iA, iB, i_torus);
        assert(my_geometry != NULL);
        local_geometry = true;
        //TODO What about `i_subs`?
        //my_geometry.set_view_info(NULL); //Reset a default
    }

/// Constructor with borrowed geometry.
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

    ~rectangle_source_base() override
    {
        if(local_geometry)
            delete my_geometry;
    }

// Zwraca indeks do aktualnego i przesuwa iterator.
// Zeruje iterator, jeśli koniec danych. Zwraca `ULONG_MAX`, jeśli missing...
    size_t _next(iteratorh &p)
    {
        return my_geometry->get_next(p);
    }

public:
//Zwraca wskaźnik do obowiązującej geometrii danych. NULL oznaczałby dane nie-zgeometryzowane
    geometry_base *getgeometry() override
    {
        assert(my_geometry != NULL);
        return my_geometry;
    }

    rectangle_geometry *getrectgeometry() //Non virtual (!!!) shortcut
    {
        assert(my_geometry != NULL);
        return my_geometry;
    }

    const char *name() override    //Zwraca nazwę serii.
    { return title_util::name(); }

    iteratorh reset() override
//Umożliwia czytanie od początku
//tablicy lub wycinka
    { return my_geometry->make_global_iterator(); }

    void close(iteratorh &p) override
    { my_geometry->destroy_iterator(p); }


//Stare podawanie parametrów geometrii źródła na `outtab` i liczby wymiarów.
/*
int box(int* outtab)
    {
    if(outtab)
        {
        outtab[0]=my_geometry.get_width();
        outtab[1]=my_geometry.get_height();
        }
    return 2;
    };
*/
};


/// Podstawa źródła funkcyjnego — żeby zminimalizować liczbę powtarzających się metod.
class function_source_base : public data_source_base, public title_util
//-----------------------------------------------------------------------------------
{
protected:
    double xmin, xmax; ///< Dany z gory zakres X.
    double step; ///< Rozmiar kroku próbkującego.
    size_t N;

/// Constructor.
    function_source_base(size_t iN,
                         double ixmin, double ixmax, ///< Zakres po X-ach.
                         const char *itit,           ///< Nazwa funkcji, do podpisu na wykresie.
                         double iymin, double iymax) :///< Zakres na Y-ach. Oszczędza liczenia.
            N(iN), title_util(itit),
            xmin(ixmin), xmax(ixmax) //pola własne.
    {
        ymin = iymin;
        ymax = iymax; //pola dziedziczone.
        assert(xmin < xmax);
        assert(ymin <= ymax); //Jeśli równe to klasa potomna musi liczyć.
        step = (xmax - xmin) / (N - 1); // Obliczenie długości kroku próbkowania.
    }


public:

    const char *name() override    // Zwraca nazwę serii.
    { return title_util::name(); }

    void bounds(size_t &num, double &min, double &max) override
//Ile elementów, a także wartość minimalna i maksymalna. Często nietrywialna, jednak tu w sposób uproszczony.
    {
        num = N;
        min = ymin;
        max = ymax;
    }

    iteratorh reset() override // Umożliwia czytanie od początku.
    { return 0; }

    void close(iteratorh &p) override
    {
        p = NULL;
    }

};

// Szablon źródła funkcyjnego.
// Sparametryzowany typem funkcyjnym.
// @parametr F musi być klasą z bezparametrowym konstruktorem i
// metodą: `double operator () (double) `.
// Może mieć natomiast dowolne pola pomocnicze.

//template<class F>
//class function_source:function_source_base  --- #include "funcsour.hpp"
//{...};

//		IMPLEMENTATION:
// ////////////////////

inline
const char *filter_source_base::name()
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




