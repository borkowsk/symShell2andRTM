/// \file
/// \brief    LIMITS FOR SCALAR TYPES
///          --------------------------
/// \details Szablon klas implementuj¹cych limity dla skalarów
///          z mo¿liwoœci¹ dodania w³asnych specjalizacji.
///          Pomys³ z czasów gdy nie by³o tego jeszcze w standardzie C++
///          W przypadku konfliktu specjalizacji  zdefiniuj
///          ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H przed w³¹czeniem
///          tego pliku
/// \author borkowsk
/// \date   2022-10-12 (last modification)
/// @ingroup OBSOLETE
#warning  "This code is OBSOLETE and not tested in C++11 standard"

#ifndef _WB_LIMITS_H_
#define _WB_LIMITS_H_
#include <float.h>
#include <limits.h>

/// \brief Szablon klas implementuj¹cych limity dla skalarów.
/// \details Oczywiœcie z mo¿liwoœci¹ dodania w³asnych specjalizacji.
template<class Scalar>
class wb_limit
{
public:
    static Scalar Max();
    static Scalar Min();
};

// Specjalizacje dla standardowych typów skalarnych
// //////////////////////////////////////////////////

template<>
inline double wb_limit<double>::Max() {return DBL_MAX;}                 ///< Limit Max dla typu double
template<>
inline float  wb_limit<float>::Max() {return FLT_MAX;}                  ///< Limit Max dla typu float
template<>
inline long   wb_limit<long>::Max() {return LONG_MAX;}                  ///< Limit Max dla typu long
template<>
inline unsigned long wb_limit<unsigned long>::Max() {return ULONG_MAX;} ///< Limit Max dla typu unsigned long
template<>
inline signed int    wb_limit<signed int>::Max() {return INT_MAX;}      ///< Limit Max dla typu int
template<>
inline unsigned int  wb_limit<unsigned int>::Max() {return UINT_MAX;}   ///< Limit Max dla typu unsigned int
template<>
inline signed char   wb_limit<signed char>::Max() {return CHAR_MAX;}    ///< Limit Max dla typu signed char
template<>
inline unsigned char wb_limit<unsigned char>::Max() {return UCHAR_MAX;} ///< Limit Max dla typu unsigned char

template<> inline
double wb_limit<double>::Min() {return DBL_MIN;}                       ///< Specjalizacja szablonu dla double
template<> inline
float  wb_limit<float>::Min() {return FLT_MIN;}                        ///< Specjalizacja szablonu dla float
template<> inline
signed   long  wb_limit<long>::Min() {return LONG_MIN;}                ///< Specjalizacja szablonu dla long
template<> inline
unsigned long  wb_limit<unsigned long>::Min() {return 0;}              ///< Specjalizacja szablonu dla ulong
template<> inline
signed int     wb_limit<int>::Min() {return INT_MIN;}                  ///< Specjalizacja szablonu dla int
template<> inline
unsigned int   wb_limit<unsigned int>::Min() {return 0;}               ///< Specjalizacja szablonu dla uint
template<> inline
signed char    wb_limit<signed char>::Min() {return CHAR_MIN;}         ///< Specjalizacja szablonu dla char
template<> inline
unsigned char  wb_limit<unsigned char>::Min() {return 0;}              ///< Specjalizacja szablonu dla uchar

#ifndef ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H

/// \brief limit maksymalny dla wszystkich innych skalarów. TODO TEST!
template<class Scalar>
inline Scalar wb_limit<Scalar>::Max() {return DBL_MAX;}

/// \brief limit minimalny dla wszystkich innych skalarów. TODO TEST!
template<class Scalar>
inline Scalar wb_limit<Scalar>::Min() {return DBL_MIN;}

#endif

/// \brief Klasa do tworzenia sensownego "missing values"
template<class Scalar>
class default_missing
{
    Scalar miss;
public:
    default_missing();
    operator Scalar() {return miss;}

};

template<> inline default_missing<double>::default_missing() { miss= -DBL_MAX;} ///< Specjalizacja szablonu dla double
template<> inline default_missing<float>::default_missing()  { miss= -FLT_MAX;} ///< Specjalizacja szablonu dla float
template<> inline default_missing<signed   long>::default_missing() { miss= LONG_MIN;} ///< Specjalizacja  dla long
template<> inline default_missing<unsigned long>::default_missing() { miss= ULONG_MAX;} ///< Specjalizacja  dla ulong
template<> inline default_missing<signed    int>::default_missing() { miss= INT_MIN;} ///< Specjalizacja  dla int
template<> inline default_missing<unsigned  int>::default_missing() { miss= UINT_MAX;} ///< Specjalizacja  dla uint
template<> inline default_missing<signed   char>::default_missing() { miss= CHAR_MIN;} ///< Specjalizacja  dla char
template<> inline default_missing<unsigned char>::default_missing() { miss= UCHAR_MAX;}///< Specjalizacja  dla uchar

#ifndef ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H

/// \brief Wartoœæ oznaczaj¹ca brak danych domyœlna dla wszystkich skalarów.
template<class Scalar>
inline default_missing<Scalar>::default_missing() {miss= -DBL_MAX;}

#endif

// ¯eby nie zaœmiecaæ globalnej przestrzeni nazw? Jednak mo¿e lepiej, ¿eby zosta³a taka informacja!?
// #undef ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H

#endif // _WB_LIMITS_H_
/* ******************************************************************/
/*              SYMSHELLLIGHT  version 2022                         */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Spo³ecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
