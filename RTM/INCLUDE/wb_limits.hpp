/// \file wb_limits.hpp
/// \brief    LIMITS FOR SCALAR TYPES in wbrtm::
///          -----------------------------------
///
/// \details Szablon klas implementujących limity dla skalarów
///          z możliwością dodania własnych specjalizacji.
///          Ten jest w przestrzeni nazw 'wbrtm::'.
///          Pomysł z czasów gdy nie było tego jeszcze w standardzie C++
///          W przypadku konfliktu specjalizacji  zdefiniuj
///          ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H przed włączeniem
///          tego pliku
///
/// \author borkowsk
/// \date   2022-10-12 (last modification)
/// @ingroup OBSOLETE
#ifndef _WB_LIMITS_H_INCLUDED_
#define _WB_LIMITS_H_INCLUDED_

#ifndef _MSC_VER //# warning still not work under Microsoft C++
#warning  "This code is OBSOLETE and not tested in C++11 standard"
#endif

#include <float.h>
#include <limits.h>

/**
 * @defgroup TypesService Informacja o typach i nazewnictwo
 * \brief Ładne i przenośne nazwy typów itp.
 */
/// @{

namespace wbrtm { // Przestrzeń nazw biblioteki WBRTM

    template<class Scalar>
    class limit {
    public:
        static Scalar Max();

        static Scalar Min();
    };

    template<>
    inline double limit<double>::Max() { return DBL_MAX; }

    template<>
    inline float limit<float>::Max() { return FLT_MAX; }

    template<>
    inline long limit<long>::Max() { return LONG_MAX; }

    template<>
    inline unsigned long limit<unsigned long>::Max() { return ULONG_MAX; }

    template<>
    inline int limit<int>::Max() { return INT_MAX; }

    template<>
    inline unsigned int limit<unsigned int>::Max() { return UINT_MAX; }

    template<>
    inline short limit<short>::Max() { return SHRT_MAX; }

    template<>
    inline unsigned short limit<unsigned short>::Max() { return USHRT_MAX; }

    template<>
    inline signed char limit<signed char>::Max() { return CHAR_MAX; }

    template<>
    inline unsigned char limit<unsigned char>::Max() { return UCHAR_MAX; }

    template<>
    inline bool limit<bool>::Max() { return true; }


    template<>
    inline double limit<double>::Min() { return -DBL_MAX; }

    template<>
    inline float limit<float>::Min() { return -FLT_MAX; }

    template<>
    inline long limit<long>::Min() { return LONG_MIN; }

    template<>
    inline unsigned long limit<unsigned long>::Min() { return 0; }

    template<>
    inline int limit<int>::Min() { return INT_MIN; }

    template<>
    inline unsigned int limit<unsigned int>::Min() { return 0; }

    template<>
    inline short limit<short>::Min() { return SHRT_MIN; }

    template<>
    inline unsigned short limit<unsigned short>::Min() { return 0; }

    template<>
    inline signed char limit<signed char>::Min() { return CHAR_MIN; }

    template<>
    inline unsigned char limit<unsigned char>::Min() { return 0; }

    template<>
    inline bool limit<bool>::Min() { return false; }

#ifndef ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H

    template<class Scalar>
    inline Scalar limit<Scalar>::Max() { return DBL_MAX; }

    template<class Scalar>
    inline Scalar limit<Scalar>::Min() { return DBL_MIN; }

#endif

//Klasa do tworzenia sensownego "missing values" oraz 
//sprawdzania minimum i maksimum dla wszystkich typów skalarnych
    template<class Scalar>
    class default_missing
    {
        Scalar miss;
    public:
        default_missing();

        operator Scalar() {
            return miss;
        }
    };

    template<>
    inline default_missing<double>::default_missing() { miss = DBL_MAX; }

    template<>
    inline default_missing<float>::default_missing() { miss = FLT_MAX; }

    template<>
    inline default_missing<long>::default_missing() { miss = LONG_MIN; }

    template<>
    inline default_missing<unsigned long>::default_missing() { miss = ULONG_MAX; }

    template<>
    inline default_missing<int>::default_missing() { miss = INT_MIN; }

    template<>
    inline default_missing<unsigned int>::default_missing() { miss = UINT_MAX; }

    template<>
    inline default_missing<signed char>::default_missing() { miss = CHAR_MIN; }

    template<>
    inline default_missing<unsigned char>::default_missing() { miss = UCHAR_MAX; }

#ifndef ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H

    template<class Scalar>
    inline default_missing<Scalar>::default_missing() { miss = -DBL_MAX; }

#endif

} //namespace wbrtm
///@}

/* ******************************************************************/
/*                WBRTM  version 2022 for GuestXR                   */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif //_WB_LIMITS_H_INCLUDED_

