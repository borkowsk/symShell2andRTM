/// @file
/// @brief __Definitions of basic (interface) data source class__ /<br>
///         _Definicje podstawowej klasy źródła danych (interfejsu)._
/// @date 2026-05-16 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_DATA_SOURCE_BASE_HPP_INCLUDED_
#define SYMSHELL2_DATA_SOURCE_BASE_HPP_INCLUDED_

#ifndef __cplusplus
#error C++ required
#endif

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
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

/// SymShell2 library.
namespace sym2 { namespace data {

/// @defgroup GRUPA_DATACLSS System źródeł danych
/// @brief	Różne źródła danych bazujące na wspólnym interfejsie i z możliwościami czerpania od siebie.
/// @{

constexpr unsigned ZAPAS_NA_CYFRY = (DBL_DIG * 2); ///< Do wyświetlania: DBL_DIG+zapas na znaki i wykładnik

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @class data_source_base
/// @brief Definition of the data source's interface.
/// @details Each data source must implement these methods, but may also have others.
#endif

/// @brief Definicja INTERFACE-u źródła danych.
/// @details Każde źródło musi mieć zaimplementowane takie metody, ale może mieć też inne.
class data_source_base
//--------------------
{
public:
    /// Makro definiujące testowanie czy dany blok pamięci jest źródłem danych.
    MEMORY_GUARD(unsigned, 0xAB0C0DAD);

    /// @name SKRÓTY NAZW TYPÓW
    /// @{
    typedef symshell2::iterator_h iterator_h; ///< Skrót dla typu uchwytu iterator-a.
    typedef iterator_h iteratorh; ///< Kompatybilność wsteczna ze starą formą.
    typedef symshell2::geometry_base geometry_base;
    typedef geometry_base geometry;
    typedef geometry::index_t  index_t;
    static_assert(sizeof(index_t)==sizeof(iterator_h), "Types `index_t` and `iterator_h` must have the same sizes!");
    /// @}

private:
    long  cur_step;    ///< Numer kolejnej wersji danych.
    long no_change;    ///< Od ilu kroków nie było zmiany.

protected:
    double    miss;    ///< Missing value/wartość symbolizująca brak danych.
    double   y_min;    ///< Dany lub wydedukowany zakres Y.
    double   y_max;    ///< Jeśli jest dany, to nie należy go dedukować.

    geometry_base *my_geometry; ///< Wskaźnik do geometrii danych.
    bool        local_geometry; ///< Określa, czy geometria należy do tego obiektu.

public:
// / Liczy @c `INF.` (wg. `IEEE`) — jako znacznik braku (może generować SIGFPE na części platform).
//static double inf();
// / Liczy @c NAN (wg IEEE) — jako znacznik braku (użycie może generować SIGFPE na części platform).
//static double nan();

// accessors:
//-----------

    virtual /// @brief Ustala "missing value" takie, jakie chce użytkownik klasy.
    void set_missing(double i_miss);

    /// @brief Zapewnia właściwa inicjacje i obsługę wartości "miss".
    /// @details Wystarczy wywołać raz, przed iteracją, a potem używać tylko
    ///         `is_missing()` lub po prostu `miss`.
    double get_missing();

    /// @brief Sprawdzanie, czy`Source->get` nie dało `missing`.
    int is_missing(double val) const;

    /// @brief Ustala minimum i maksimum, żeby uniknąć próbkowania danych.
    /// @details Podanie równych wartości, np. 0 i 0 może ponownie włączać próbkowanie.
    void set_min_max(double i_min, double i_max);

    /// @name OBSŁUGA VERSIONING-u DANYCH
    /// @details DATA VERSIONING SUPPORT
    //----------------------------------
    /// @{

    /// @brief Ustalanie informacji o wersji danych.
    virtual void new_data_version(int change = 1, unsigned increment = 1);

    /// @brief Uaktualnia wersje wg. podanego źródła i wtedy zwraca 1.
    /// @returns Jeśli wersje są the_same, to zwraca 0.
    virtual int update_version_from(data_source_base *Source);

    /// @brief Numer wersji danych. Domyślnie prosty akcesor "ro".
    virtual long data_version() { return cur_step; }

    /// @brief Podaje, od ilu wersji dane się nie zmieniły.
    virtual long how_old_data() { return no_change; }

    /// @brief Restartuje "versioning" źródeł. W wypadku pod-źródeł powinna być reimplementacja!
    virtual void restart_counting() { cur_step = -1; no_change = 0; }
    /// @}

    /// @name OBSŁUGA GEOMETRII SERII
    /// @details SERIES GEOMETRY SUPPORT
    //----------------------------------
    /// @{

    /// @brief Zwraca wskaźnik do obowiązującej geometrii danych.
    /// @details Domyślne `NULL` oznacza dane nie-zgeometryzowane, wyłącznie z dostępem sukcesywnym.
    virtual geometry_base *get_geometry() { return NULL; }

    /// @brief WYMAGANA IMPLEMENTACJA przetwarzania indeksu z geometrii na wartość z serii.
    /// @details O ile jest geometria i możliwe jest czytanie w losowej kolejności
    ///          , które domyślnie NIE JEST MOŻLIWE i powoduje błąd wykonania.
    virtual double get(size_t index_from_geometry)=0;
    /// @}

    /// @name INNE METODY CZYSTO WIRTUALNE, KTÓRYCH IMPLEMENTACJA JEST WYMAGANA.
    /// @details PURE virtual INTERFACE — need to be defined.
    // ------------------------------------------------------
    /// @{

    /// @brief WYMAGANA IMPLEMENTACJA musi zwracać nazwę serii albo pusty tekst "" — NIE NULL.
    /// @details Może nie być tym samym tekstem, które zwróciłoby `title_util`, zazwyczaj używane w klasach potomnych.
    virtual const char *name() = 0;

    /// @brief WYMAGANA IMPLEMENTACJA musi ustawić na parametrach, ile jest elementów, jaka jest wartość minimalna i jaka maksymalna.
    virtual void bounds(size_t &N, double &min, double &max) = 0;

    /// @brief WYMAGANA IMPLEMENTACJA dostarcza iterator do odczytywania kolejnych wartości ustawiony na start.
    /// @return `iterator_h` jest uchwytem dla jakiegoś obiektu `iterator`.
    /// @note Implementacja iteratora całkowicie zależy od implementatora i nie trzeba w niej grzebać ani nawet zaglądać.
    virtual iterator_h reset() = 0;

    /// @brief WYMAGANA IMPLEMENTACJA ma dać następną z N liczb na podstawie iteratora.
    /// @details Po ostatniej (N-ej) powinna zwalniać iterator.
    virtual double get(iterator_h &) = 0;

    /// @brief WYMAGANA IMPLEMENTACJA ma zwalniać/niszczyć iterator.
    /// @details O ile nie został zwolniony przez końcowe wywołanie `get`
    ///          , ale nadmiarowe użycie nie powinno nic uszkadzać (jak w przypadku delete NULL).
    virtual void close(iterator_h &) = 0;
    /// @}

    /// @name CONSTRUCTION/DESTRUCTION
    //--------------------------------
    /// @{

    /// Constructor.
    data_source_base()
    : my_geometry(NULL), local_geometry(false), cur_step(-1), no_change(0),
      y_min(0), y_max(0), miss(symshell2::default_missing<double>())
    {}

    /// Destructor. Wymuszenie wirtualności.
    virtual ~data_source_base() = default;
    /// @}
};

/// @}

// ACCESSORS INLINE IMPLEMENTATIONS:
//----------------------------------

inline
void data_source_base::set_missing(double i_miss)
// Ustala "missing value" takie, jakie chce użytkownik klasy.
{
    miss = i_miss;
}

inline
int data_source_base::is_missing(double val) const
/// @internal Domyślne sprawdzanie, czy @c get() nie dało "missing".
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

inline
double data_source_base::get_missing()
// Zapewnia właściwą inicjację i obsługę wartości "miss"
// Ale, czy to "really" potrzebne?
{
    double tmp = symshell2::default_missing<double>(); //Klasy szablonowe muszą to reimplementować
    // Używamy porównania zawartości pamięci, bo niekoniecznie tmp musi być poprawną liczbą typu double.
    if(memcmp(&miss, &tmp, sizeof(miss)) != 0) //TODO A TU NIE POWINNO BYĆ == zamiast != ???
        return miss; // Jeśli ustawione lub już domyślne.
    else
        return miss = tmp; //Zapewnia, że będzie ustawione tak jak default_missing<...>()
}

inline
void data_source_base::set_min_max(double i_min, double i_max)
// Ustala arbitralne minimum i maksimum, żeby uniknąć próbkowania (dedukowania).
// Podanie równych włącza znowu próbkowanie.
{
    assert(i_min <= i_max); //Podanie równych włącza znowu próbkowanie (dedukcję)!!!
    y_min = i_min;
    y_max = i_max;
}


// OBSŁUGA WERSJI DANYCH:
// ----------------------

inline
void data_source_base::new_data_version(int change, unsigned increment)
// Ustalanie informacji o wersji danych.
{
    cur_step += increment;
    if(change)
        no_change = 0;
    else
        no_change += increment;
    assert(cur_step >= no_change);
}

inline
int data_source_base::update_version_from(data_source_base *Source)
// Uaktualnia wersje wg podanego źródła i wtedy zwraca 1.
// Jeśli wersje są the_same, to zwraca 0.
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
// -------------------------------

inline
double data_source_base::get(size_t index_from_geometry)
// Przetwarza index uzyskany z geometrii
// na wartość z serii, o ile jest możliwe czytanie losowe.
{
    assert(!"Random access get() not implemented");
    return miss; //To jest używane w kompilacji Release!!!
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
#endif //SYMSHELL2_DATA_SOURCE_BASE_HPP_INCLUDED_
