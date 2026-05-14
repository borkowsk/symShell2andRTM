/// @file
/// @brief UNUSED correlation filter class - a two-source, single-value data filter/
///        NIEUŻYWANA klasa  filtru korelacji — dwuźródłowego, jednowartościowego filtru danych.
/// @date 2026-05-14 (modified)
// ********************************************************************************************************************
//
//#error SKOMPLIKOWANE W IMPLEMENTACJI, ALE MOŻNA BY KIEDYŚ SPRÓBOWAĆ.

#ifndef SYMSHELL2_CORR_SOUR_HPP_INCLUDED_
#define SYMSHELL2_CORR_SOUR_HPP_INCLUDED_

#include "bifilter.hpp"

namespace sym2 { namespace data {

/// Klasa  filtru korelacji — dwuźródłowego, jednowartościowego filtru danych.
/// Nie wygląda na używaną.
class correlation_source : public template_scalar_source_base<double>,
                           public bi_filter_source_base
//---------------------------------------------------------------------
{
    double corr_value; ///< Cache na wartość korelacji.

protected:
    /// Liczy korelacje i zapisuje w `corr_value`.
    /// NIE ZNAJDUJE IMPLEMENTACJI... DZIWNE.
    virtual void _calculate();

public:
    /// Konstruktor.
    /// \param ini1 pierwsza źródłowa seria danych.
    /// \param ini2 druga źródłowa seria danych.
    /// \param format sposób tworzenia nazwy filtra z nazw serii źródłowych.
    correlation_source(data_source_base *ini1,
                       data_source_base *ini2,
                       const char *format = "CORR(%s , %s)")
    : bi_filter_source_base(ini1, ini2, format),
      template_scalar_source_base<double>("",/*min*/-1,/*max*/1),
      corr_value(0)
    {}

    /// Raczej nieużywana implementacja decyzji o wartości minimalnej i maksymalnej.
    virtual void _bounds(double &min1, double &max1,
                         double &min2, double &max2,
                         double &min, double &max)
    {
        //jeśli nie ustawione, to przyjmujemy typowy zakres korelacji.
        min = -1;
        max = 1;
    }

    /// Raczej nieużywane. Zawsze  zwraca tylko raz policzona korelacje, chyba że dane się zmieniły.
    virtual double _get(double val1, double val2)
    {
        return corr_value;
    }

    /// Zwraca policzoną korelację.
    virtual double get()
    {
        if(check_version_())
            _calculate();
        return corr_value;
    }
};

}} // end-of-namespaces sym2::data

/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
#endif //SYMSHELL2_CORR_SOUR_HPP_INCLUDED_
