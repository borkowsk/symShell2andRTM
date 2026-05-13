/// @file
/// @brief ...
/// @date 2026-05-13 (modified)
/// @details
///      Created by borkowsk on 13.05.26.
//
#ifndef SYMSHELL2_TOI_TOU_TOLL_HPP_INCLUDED_
#define SYMSHELL2_TOI_TOU_TOLL_HPP_INCLUDED_

#include <climits>
#include <limits>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"

/// Wysycająca konwersja na `int`. @note Nie z `long double`!
/// @details Używa `long long` jako wspólnej reprezentacji wszystkich liczb całkowitych, co jest kosztowne.
template<class number> inline
int toi(const number& v)
{
    constexpr long long low = numeric_limits<int>::min();
    constexpr long long high = numeric_limits<int>::max();
    const long long val=v;
    return static_cast<int>(max(low, min(val, high)));
}

/// Wysycająca konwersja na `long`. @note Nie z `long double`!
/// @details Używa `long long` jako wspólnej reprezentacji wszystkich liczb całkowitych, co jest kosztowne.
///          A i tak nie działa dla `double`.
template<class number> inline
long tol(const number& v)
{
    constexpr long long low = numeric_limits<long>::min();
    constexpr long long high = numeric_limits<long>::max();
    const long long val=v;
    return static_cast<long>(max(low, min(val, high)));
}

/// Specjalizacja: Wysycająca konwersja `unsigned int` na `int`.
inline int toi(const unsigned& p)
{
    constexpr unsigned high = INT_MAX;
    if(p<high)
        return int(p); //Maskuje warning-i z obcinania
    else
        return high;
}

/// Specjalizacja: Wysycająca konwersja `long` na `int`.
inline int toi(const long& p)
{
    constexpr long low = numeric_limits<int>::min();
    constexpr long high = numeric_limits<int>::max();
    return static_cast<int>(max(low, min(p, high)));
}

/// Specjalizacja: Wysycająca konwersja `unsigned long` na `int`.
inline int toi(const unsigned long& p)
{
    constexpr unsigned long high = INT_MAX;
    if(p<high)
        return int(p); //Maskuje warning-i z obcinania
    else
        return high;
}

/// Specjalizacja: Wysycająca konwersja `double` na `int`.
inline int toi(const double& p)
{
    // Definiujemy limity dla `int`
    constexpr double low  = INT_MIN;
    constexpr double high = INT_MAX;
    return static_cast<int>(max(low, min(p, high)));
}

/// Wysycająca konwersja `double` na `int` z własną nazwą.
inline int dtoi(const double& p)
{
    // Definiujemy limity dla `int`
    constexpr double low  = INT_MIN;
    constexpr double high = INT_MAX;
    return static_cast<int>(max(low, min(p, high)));
}

/// Wysycająca konwersja z `double` na `unsigned`.
/// Metoda bardziej ogólna
inline unsigned int dtou(const double& p)
{
    // Definiujemy limity dla `unsigned int`
    constexpr double low  = 0.0;
    constexpr double high = UINT_MAX; //static_cast<double>(std::numeric_limits<unsigned int>::max());

    // Nasycenie:
    // 1. Jeśli p < 0, zwróci 0
    // 2. Jeśli p > 4294967295, zwróci 4294967295long long val = v;
    // 3. W przeciwnym razie zwróci obcięte p
    return static_cast<unsigned int>(max(low, min(p, high)));
}

/// Specjalizacja konwersji wysycającej z `double` na `long`
inline long tol(const double& v)
{
    constexpr double low = static_cast<double>(numeric_limits<long>::min());
    constexpr double high = static_cast<double>(numeric_limits<long>::max());

    if (v >= high) return numeric_limits<long>::max();
    if (v <= low) return numeric_limits<long long>::min();

    return static_cast<long long>(v);
}

/// "Bezpieczna" konwersja `double` na `long long`.
inline long long dtoll(const double& p)
{
    constexpr double low = static_cast<double>(numeric_limits<long long>::min());
    constexpr double high = static_cast<double>(numeric_limits<long long>::max());

    if (p >= high) return numeric_limits<long long>::max();
    if (p <= low) return numeric_limits<long long>::min();

    return static_cast<long long>(p);
}

#pragma clang diagnostic pop
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
#endif //SYMSHELL2_TOI_TOU_TOLL_HPP_INCLUDED_
