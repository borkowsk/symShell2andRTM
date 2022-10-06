//      LIMITS FOR SCALAR TYPES
//-------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////
#ifndef _WB_LIMITS_H_
#define _WB_LIMITS_H_
#include <float.h>
#include <limits.h>

//Szablon klas implementujacych limity dla skalarów
//z mozliwoscia dodania wlasnych specjalizacji
//--------------------------------------------------
//W przypadku konfliktu specjalizacji  zdefiniuj
// ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H przed inkludowaniem tego pliku
//
template<class Scalar>
class wb_limit
{
public:
    static Scalar Max();
    static Scalar Min();
};

inline wb_limits<double>::Max() {return DBL_MAX;}
inline wb_limits<float>::Max() {return FLT_MAX;}
inline wb_limits<long>::Max() {return LONG_MAX;}
inline wb_limits<unsigned long>::Max() {return ULONG_MAX;}
inline wb_limits<int>::Max() {return INT_MAX;}
inline wb_limits<unsigned int>::Max() {return UINT_MAX;}
inline wb_limits<signed char>::Max() {return CHAR_MAX;}
inline wb_limits<unsigned char>::Max() {return UCHAR_MAX;}


inline wb_limits<double>::Min() {return DBL_MIN;}
inline wb_limits<float>::Min() {return FLT_MIN;}
inline wb_limits<long>::Min() {return LONG_MIN;}
inline wb_limits<unsigned long>::Min() {return ULONG_MIN;}
inline wb_limits<int>::Min() {return INT_MIN;}
inline wb_limits<unsigned int>::Min() {return UINT_MIN;}
inline wb_limits<signed char>::Min() {return CHAR_MIN;}
inline wb_limits<unsigned char>::Min() {return UCHAR_MIN;}

#ifndef ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H
template<class Scalar>
inline wb_limits<Scalar>::Max() {return DBL_MAX;}
template<class Scalar>
inline wb_limits<Scalar>::Min() {return DBL_MIN;}
#endif

//Klasa do tworzenia sensownego "missing values" oraz 
//sprawdzania minimum i maksimum dla wszystkich typów skalarnych
template<class Scalar>
class default_missing
{
    Scalar miss;
public:
    default_missing();
    operator Scalar() {return miss;}

};

inline default_missing<double>::default_missing() {miss= -DBL_MAX;}
inline default_missing<float>::default_missing() {miss= -FLT_MAX;}
inline default_missing<long>::default_missing() {miss= LONG_MIN;}
inline default_missing<unsigned long>::default_missing() {miss= ULONG_MAX;}
inline default_missing<int>::default_missing() {miss= INT_MIN;}
inline default_missing<unsigned int>::default_missing() {miss= UINT_MAX;}
inline default_missing<signed char>::default_missing() {miss= CHAR_MIN;}
inline default_missing<unsigned char>::default_missing() {miss= UCHAR_MAX;}

#ifndef ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H
template<class Scalar>
inline default_missing<Scalar>::default_missing() {miss= -DBL_MAX;}
#endif

//Zeby nie zasmicac przestrzeni nazw
#undef ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H

#endif
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://www.iss.uw.edu.pl/~borkowsk/                 */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/
