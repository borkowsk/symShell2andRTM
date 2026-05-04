/// @file
/// @brief Definitions of basic (interface) data source class /
///        Definicje podstawowej klasy źródła danych (interfejsu).
/// @date 2026-05-04 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_DATA_SOURCE_BASE_HPP_INCLUDED_
#define SYMSHELL2_DATA_SOURCE_BASE_HPP_INCLUDED_

#include <cassert>
#include <climits>
#include <cfloat>
#include <cstdio>

#include "wb_limits.hpp"
#include "wb_ptr.hpp"
#include "wb_clone.hpp"

#include "titleuti.hpp"
#include "rectgeom.hpp" /* GEOMETRY INTERFACE */
#include "mem_guard.h"  /* Pomocnik do szukania przedwczesnych destrukcji obiektów! */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

const unsigned ZAPAS_NA_CYFRY = (DBL_DIG * 2); //Do wyświetlania: DBL_DIG+zapas na znaki i wykładnik

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief Definition of the data source's interface.
/// Each data source must implement these methods, but may also have others.
#else
/// @brief Definicja INTERFACE-u źródła danych.
/// Każde źródło musi mieć zaimplementowane takie metody, ale może mieć też inne.
#endif
class data_source_base
//--------------------
{
public:
    MEMORY_GUARD(unsigned, 0xAB0C0DAD);

private:
    long  cur_step;    ///< Numer kolejnej wersji danych.
    long no_change;    ///< Od ilu kroków nie było zmiany.

#if __GNUC__ >= 3   //Dziwne...
public:
#else
    protected:
#endif
    double  miss;    ///< Missing value/wartość symbolizująca brak danych.
    double y_min;    ///< Dany lub wydedukowany zakres Y.
    double y_max;    ///< Jeśli jest dany, to nie należy go dedukować.

    rectangle_geometry *my_geometry; ///< Wskaźnik do geometrii danych.
    bool             local_geometry; ///< Określa, czy geometria należy do tego obiektu.

public:
    typedef ::iteratorh   iteratorh; ///< Skrót dla typu uchwytu iterator-a. TODO inna nazwa?

// / Liczy @c `INF.` (wg. `IEEE`) — jako znacznik braku (może generować SIGFPE na części platform).
//static double inf();
// / Liczy @c NAN (wg IEEE) — jako znacznik braku (użycie może generować SIGFPE na części platform).
//static double nan();

// accessors:
//-----------

    /// Ustala "missing value" takie, jakie chce użytkownik klasy.
    void set_missing(double i_miss);

    /// Sprawdzanie, czy`Source->get` nie dało `missing`.
    int is_missing(double val) const;

    /// Zapewnia właściwa inicjacje i obsługę wartości "miss".
    /// Wystarczy wywołać raz, przed iteracją, a potem używać tylko
    /// `is_missing()` lub po prostu `miss`.
    double get_missing();

    /// Ustala minimum i maksimum, żeby uniknąć próbkowania danych.
    /// Podanie równych wartości, np. 0 i 0 może ponownie włączać próbkowanie.
    void set_min_max(double i_min, double i_max);

    /// @name OBSŁUGA VERSIONING-u DANYCH
    /// @details DATA VERSIONING SUPPORT
    //----------------------------------
    /// @{

    /// Ustalanie informacji o wersji danych.
    virtual
    void new_data_version(int change = 1, unsigned increment = 1);

    /// Uaktualnia wersje wg podanego źródła i wtedy zwraca 1.
    /// Jeśli wersje są zgodne, to zwraca 0.
    virtual
    int update_version_from(data_source_base *Source);

    /// Numer wersji danych.
    virtual
    long data_version()
    { return cur_step; }

    /// Podaje, od ilu wersji dane się nie zmieniły.
    virtual
    long how_old_data()
    { return no_change; }

    /// Restartuje "versioning" źródeł. W wypadku pod-źródeł powinna być reimplementacja!
    virtual
    void restart_counting()
    { cur_step = -1; no_change = 0; }
    /// @}

    /// @name OBSŁUGA GEOMETRII SERII
    /// @details SERIES GEOMETRY SUPPORT
    //----------------------------------
    /// @{

    /// Zwraca wskaźnik do obowiązującej geometrii danych.
    /// `NULL` oznacza dane nie-zgeometryzowane, wyłącznie z dostępem sukcesywnym.
    virtual
    geometry_base *get_geometry()
    { return NULL; }

    /// Przetwarza index uzyskany z geometrii na wartość z serii.
    /// O ile jest geometria i możliwe jest czytanie w losowej kolejności
    /// , które domyślnie NIE JEST MOŻLIWE i powoduje błąd wykonania.
    virtual
    double get(size_t index_from_geometry);
    /// @}

    /// @name METODY CZYSTO WIRTUALNE — DO ZDEFINIOWANIA
    /// @details PURE virtual INTERFACE — need to be defined
    //------------------------------------------------------
    /// @{

    /// Musi zwracać nazwę serii albo pusty tekst "" — NIE NULL.
    /// Może nie być tym samym tekstem, które zwróciłoby `title_util`, zazwyczaj używane w klasach potomnych.
    virtual
    const char *name() = 0;

    /// Musi ustawić na parametrach, ile jest elementów, jaka jest wartość minimalna i jaka maksymalna.
    virtual
    void bounds(size_t &N, double &min, double &max) = 0;

    /// Umożliwia czytanie od początku poprzez iterator.
    /// @return `iteratorh` jest uchwytem dla jakiegoś obiektu `iterator`.
    /// @note Implementacja iteratora całkowicie zależy od implementatora i nie trzeba w niej grzebać ani nawet zaglądać.
    virtual
    iteratorh reset() = 0;

    /// Daje następną z N liczb na podstawie iteratora. Po N-tej zwalnia iterator.
    virtual
    double get(iteratorh &) = 0;

    /// Zwalnia/niszczy iterator. O ile nie został zwolniony przez końcowe wywołanie `get`.
    virtual
    void close(iteratorh &) = 0;
    /// @}

    /// @name CONSTRUCTION/DESTRUCTION
    //--------------------------------
    /// @{

    /// Constructor.
    data_source_base() :
            my_geometry(NULL), local_geometry(false),
            cur_step(-1), no_change(0),
            y_min(0), y_max(0),
            miss(default_missing<double>())
    {}

    /// Destructor. Wymuszenie wirtualności.
    virtual ~data_source_base() = default;
    /// @}
};

// ACCESSORS INLINE IMPLEMENTATIONS:
//----------------------------------

// Ustala "missing value" takie, jakie chce użytkownik klasy.
inline
void data_source_base::set_missing(double i_miss)
{
    miss = i_miss;
}

// Sprawdzanie, czy"get()" nie dało "missing".
inline
int data_source_base::is_missing(double val) const
{
    if(val == miss)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Zapewnia właściwą inicjację i obsługę wartości "miss"
// Ale, czy to "really" potrzebne?
inline
double data_source_base::get_missing()
{
    double tmp = default_missing<double>(); //Klasy szablonowe muszą to reimplementować
    // Używamy `memcmp` bo niekoniecznie tmp musi być poprawną liczbą typu double.
    if(memcmp(&miss, &tmp, sizeof(miss)) != 0) //TODO A TU NIE POWINNO BYĆ == zamiast != ???
        return miss; // Jeśli ustawione lub już domyślne.
    else
        return miss = tmp; //Zapewnia, że będzie ustawione tak jak default_missing<...>()
}

// Ustala arbitralne minimum i maksimum, żeby uniknąć próbkowania (dedukowania).
// Podanie równych włącza znowu próbkowanie.
inline
void data_source_base::set_min_max(double i_min, double i_max)
{
    assert(i_min <= i_max); //Podanie równych włącza znowu próbkowanie (dedukcję)!!!
    y_min = i_min;
    y_max = i_max;
}


// OBSŁUGA WERSJI DANYCH:
// //////////////////////

// Ustalanie informacji o wersji danych.
inline
void data_source_base::new_data_version(int change, unsigned increment)
{
    cur_step += increment;
    if(change)
        no_change = 0;
    else
        no_change += increment;
    assert(cur_step >= no_change);
}

// Uaktualnia wersje wg podanego źródła i wtedy zwraca 1.
// Jeśli wersje są zgodne, to zwraca 0.
inline
int data_source_base::update_version_from(data_source_base *Source)
{
    if(Source->data_version() > data_source_base::data_version()) //Tu może startować ewentualna propagacja
    {
        new_data_version(
                Source->how_old_data() == 0?1:0,
                Source->data_version() - data_source_base::data_version() //Żeby nie było rekurencji nie wprost
        );
        return 1;
    } else
        return 0;
}


// OBSŁUGA GEOMETRYZOWANYCH SERII:
// ///////////////////////////////

// Przetwarza index uzyskany z geometrii
// na wartość z serii, o ile jest możliwe czytanie losowe.
inline
double data_source_base::get(size_t index_from_geometry)
{
    assert(!"Random access get() not implemented");
    return miss; //To jest używane w kompilacji Release!!!
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
#endif //SYMSHELL2_DATA_SOURCE_BASE_HPP_INCLUDED_
