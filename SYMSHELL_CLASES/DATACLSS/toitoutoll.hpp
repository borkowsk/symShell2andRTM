/// @file
/// @brief **Templates of functions that convert various numeric types with saturation** /<br>
///         _Szablony funkcji konwertujących z wysyceniem różne typy liczbowe._
/// @date 2026-05-29 (modified)
///       =====================================================================================
/// @details
///      Created by borkowsk on 13.05.26.
//
#ifndef SYMSHELL2_TOI_TOU_TOLL_HPP_INCLUDED_
#define SYMSHELL2_TOI_TOU_TOLL_HPP_INCLUDED_

#include <climits>
#include <limits>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"

namespace sym2
{
    using namespace std;

    /// @brief @PL{ Wysycająca konwersja na `short int`. @note Nie z `long double`! }
    ///        @EN{  }
    /// @details Używa `long long` jako wspólnej reprezentacji wszystkich liczb całkowitych, co jest kosztowne.
    template<class number>
    inline
    int tos(const number &v)
    {
        constexpr long long low = numeric_limits<short int>::min();
        constexpr long long high = numeric_limits<short int>::max();
        const long long val = v;
        return static_cast<short int>(max(low, min(val, high)));
    }

    /// @brief @PL{ Wysycająca konwersja na `int`. @note Nie z `long double`! }
    ///        @EN{  }
    /// @details Używa `long long` jako wspólnej reprezentacji wszystkich liczb całkowitych, co jest kosztowne.
    template<class number>
    inline
    int toi(const number &v)
    {
        constexpr long long low = numeric_limits<int>::min();
        constexpr long long high = numeric_limits<int>::max();
        const long long val = v;
        return static_cast<int>(max(low, min(val, high)));
    }

    /// @brief @PL{ Wysycająca konwersja na `long`. @note Nie z `long double`! }
    ///        @EN{  }
    /// @details Używa `long long` jako wspólnej reprezentacji wszystkich liczb całkowitych, co jest kosztowne.
    ///          A i tak nie działa dla `double`.
    template<class number>
    inline
    long tol(const number &v)
    {
        constexpr long long low = numeric_limits<long>::min();
        constexpr long long high = numeric_limits<long>::max();
        const long long val = v;
        return static_cast<long>(max(low, min(val, high)));
    }

    /// @brief @PL{ Specjalizacja: Wysycająca konwersja `unsigned int` na `short int`. }
    ///        @EN{  }
    inline short int tos(const unsigned &p)
    {
        constexpr unsigned high = SHRT_MAX;
        if(p < high)
            return static_cast<short int>(p); //Maskuje warning-i z obcinania
        else
            return high;
    }

    /// @brief @PL{ Specjalizacja: Wysycająca konwersja `unsigned int` na `int`. }
    ///        @EN{  }
    inline int toi(const unsigned &p)
    {
        constexpr unsigned high = INT_MAX;
        if(p < high)
            return int(p); //Maskuje warning-i z obcinania
        else
            return high;
    }

    /// @brief @PL{ Specjalizacja: Wysycająca konwersja `unsigned long` na `int`. }
    ///        @EN{  }
    inline int toi(const unsigned long &p)
    {
        constexpr unsigned long high = INT_MAX;
        if(p < high)
            return int(p); //Maskuje warning-i z obcinania
        else
            return high;
    }

    /// @brief @PL{ Specjalizacja: Wysycająca konwersja `int` na `short int`. }
    ///        @EN{  }
    inline short int tos(const int &p)
    {
        constexpr int low = numeric_limits<short int>::min();
        constexpr int high = numeric_limits<short int>::max();
        return static_cast<short int>(max(low, min(p, high)));
    }

    /// @brief @PL{ Specjalizacja: Wysycająca konwersja `long` na `int`. }
    ///        @EN{  }
    inline int toi(const long &p)
    {
        constexpr long low = numeric_limits<int>::min();
        constexpr long high = numeric_limits<int>::max();
        return static_cast<int>(max(low, min(p, high)));
    }

    /// @brief @PL{ Specjalizacja: Wysycająca konwersja `double` na `short int`. }
    ///        @EN{  }
    inline short int tos(const double &p)
    {
        // Definiujemy limity dla `int`
        constexpr double low = SHRT_MIN;
        constexpr double high = SHRT_MAX;
        return static_cast<short int>(max(low, min(p, high)));
    }

    /// @brief @PL{ Specjalizacja: Wysycająca konwersja `double` na `int`. }
    ///        @EN{  }
    inline int toi(const double &p)
    {
        // Definiujemy limity dla `int`
        constexpr double low = INT_MIN;
        constexpr double high = INT_MAX;
        return static_cast<int>(max(low, min(p, high)));
    }

    /// @brief @PL{ Wysycająca konwersja `double` na `int` z własną nazwą. }
    ///        @EN{  }
    inline int dtoi(const double &p)
    {
        // Definiujemy limity dla `int`
        constexpr double low = INT_MIN;
        constexpr double high = INT_MAX;
        return static_cast<int>(max(low, min(p, high)));
    }

    /// @brief @PL{ Wysycająca konwersja z `double` na `unsigned`. }
    ///        @EN{  }
    /// @details Metoda bardziej ogólna.
    inline unsigned int dtou(const double &p)
    {
        // Definiujemy limity dla `unsigned int`
        constexpr double low = 0.0;
        constexpr double high = UINT_MAX; //static_cast<double>(std::numeric_limits<unsigned int>::max());

        // Nasycenie:
        // 1. Jeśli `p < 0`, zwróci 0
        // 2. Jeśli `p > 4294967295`, zwróci `4294967295long long val` = v;
        // 3. W przeciwnym razie zwróci obcięte `p`.
        return static_cast<unsigned int>(max(low, min(p, high)));
    }

    /// @brief @PL{ Specjalizacja konwersji wysycającej z `double` na `long`. }
    ///        @EN{  }
    inline long tol(const double &v)
    {
        constexpr double low = static_cast<double>(numeric_limits<long>::min());
        constexpr double high = static_cast<double>(numeric_limits<long>::max());

        if(v >= high) return numeric_limits<long>::max();
        if(v <= low) return numeric_limits<long long>::min();

        return static_cast<long long>(v);
    }

    /// @brief @PL{ "Bezpieczna" konwersja `double` na `long long`. }
    ///        @EN{  }
    inline long long dtoll(const double &p)
    {
        constexpr double low = static_cast<double>(numeric_limits<long long>::min());
        constexpr double high = static_cast<double>(numeric_limits<long long>::max());

        if(p >= high) return numeric_limits<long long>::max();
        if(p <= low) return numeric_limits<long long>::min();

        return static_cast<long long>(p);
    }

} //Namespace sym2

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
#endif //SYMSHELL2_TOI_TOU_TOLL_HPP_INCLUDED_
