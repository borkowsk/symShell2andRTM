/// @file
/// @brief **Different types of dual-source data filters** /<br>
///         _Różne klasy dwuźródłowych filtrów danych._
/// @date 2026-05-20 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_BI_FILTER_SOUR_HPP_INCLUDED_
#define SYMSHELL2_BI_FILTER_SOUR_HPP_INCLUDED_

//#include "math.h" /*DLA FILTRÓW */
#include "bifilter.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

/// @brief @EN{ A simple class that SUMs two sources without using a cache. }
///        @PL{ Prosta klasa SUMUJĄCA dwa źródła bez użycia cache'a. }
/// @details ...
class summ_source : public bi_filter_source_base
//----------------------------------------------------
{
public:
    /// Konstruktor podwójnego filtra sumującego.
    /// \param ini1 pierwsza źródłowa seria danych.
    /// \param ini2 druga źródłowa seria danych.
    /// \param format to sposób utworzenia nazwy tej seri z nazw serii źródłowych.
    explicit summ_source( data_source_base *ini1 = NULL,
                          data_source_base *ini2 = NULL,
                          const char *format = "SUMM(%s , %s)")
    : bi_filter_source_base(ini1, ini2, format)
    {}

//DOSTĘP DO DANYCH:
//UWAGA: Złożona iteracja wykonuje się do końca krótszego ze źródeł.
//--------------------------------------------------------------------

    /// Reimplementacja decyzji o wartości minimalnej i maksymalnej.
    void _bounds(double &min1, double &max1,
                         double &min2, double &max2,
                         double &min, double &max) override
    {
        //jeśli jest ustawione to zostawiamy
        if(y_min < y_max)
        {
            min = y_min;
            max = y_max;
            return;
        }
        //Jeśli nie ustawione, to liczymy
        //Wersja prowizoryczna.
        //Tak naprawdę powinien być cache wartości.
        min = min1 < min2?min1:min2;
        max = max1 + max2;
    }

    /// Implementacja konkretnej operacji pobierania danych.
    /// Musi sprawdzać, czy któraś z wartości lub obie nie są "missing".
    double _get(double val1, double val2) override
    {
        if(from_first_is_missing(val1) || from_second_is_missing(val2))
            return miss;
        else
            return val1 + val2;
    }

};

/// @brief @EN{ A simple class that SUMs two sources without using a cache. }
///        @PL{ Klasa zwracająca wartości z drugiego źródła, jeśli pierwsze, traktowane jako warunek zwraca wartość "nie-missing". }
/// @details ...
class if_then_source : public bi_filter_source_base
//---------------------------------------------------
{
    geometry_base *the_geom; ///< Musi mięć jakąś geometrię.

public:
    /// Konstruktor podwójnego filtra warunkowego.
    /// \param ini1 pierwsza źródłowa seria danych.
    /// \param ini2 druga źródłowa seria danych.
    /// \param format to sposób utworzenia nazwy tej seri z nazw serii źródłowych.
    explicit if_then_source( data_source_base *ini1 = NULL,
                             data_source_base *ini2 = NULL,
                             const char *format = "IF %s THEN %s ;")
    : bi_filter_source_base(ini1, ini2, format),
      the_geom(reinterpret_cast<geometry_base *>(ULONG_MAX)) //Ustala w razie potrzeby.
    {}

//DOSTĘP DO DANYCH:
//UWAGA: Liniowa iteracja wykonuje się do końca krótszego ze źródeł.
//--------------------------------------------------------------------

    /// Reimplementacja decyzji o wartości minimalnej i maksymalnej
    void _bounds(double &min1, double &max1,
                         double &min2, double &max2,
                         double &min, double &max) override
    {
        //jeśli jest ustawione to zostawiamy
        if(y_min < y_max)
        {
            min = y_min;
            max = y_max;
            return;
        }

        //jeśli nie ustawione to bierzemy z drugiego źródła, bo jego
        //wartości faktycznie trafiają na wyjście
        min = min2;
        max = max2;

        the_geom = reinterpret_cast<geometry_base *>(ULONG_MAX); //Żeby znowu sprawdził, gdy będzie potrzebna geometria.
    }

    /// Implementacja konkretnej operacji pobierania danej.
    /// Musi sprawdzać, czy któraś z wartości lub obie nie są "missing".
    double _get(double val1, double val2) override
    {
        //Jeśli pierwsza seria ma "missing" to zwracamy "missing".
        //== Realizacja warunku na "false".
        if(from_first_is_missing(val1))
            return miss;

        //jeśli druga seria ma i tak missing to konwertujemy
        //wartość na lokalne "miss", a jeśli wartość
        //jest właściwa, to ją zwracamy.
        if(from_second_is_missing(val2))
            return miss;
        else
            return val2;
    }

    /// Zwraca wskaźnik do obowiązującej geometrii danych.
    /// @note Sprawdza, czy oba źródła mają tę samą geometrię i zwraca `NULL`, jeśli nie!
    geometry_base *get_geometry() override
    {
        if(the_geom != reinterpret_cast<geometry_base *>(ULONG_MAX))
        {
            return the_geom;
        }
        else
        {
            if((the_geom = source2->get_geometry()) == Source->get_geometry())
                return the_geom;
            else
                return the_geom = NULL;
        }
    }

};

}} // end-of-namespaces sym2::data

#pragma clang diagnostic pop
/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/              */
/*        MAIL:  wborkowski@uw.edu.pl                                 */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
#endif



