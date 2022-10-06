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

template<> inline double wb_limit<double>::Max() {return DBL_MAX;}
template<> inline float wb_limit<float>::Max() {return FLT_MAX;}

template<> inline long wb_limit<long>::Max() {return LONG_MAX;}
template<> inline unsigned long wb_limit<unsigned long>::Max() {return ULONG_MAX;}

template<> inline int wb_limit<int>::Max() {return INT_MAX;}
template<> inline unsigned int wb_limit<unsigned int>::Max() {return UINT_MAX;}

template<> inline short wb_limit<short>::Max() {return SHRT_MAX;}
template<> inline unsigned short wb_limit<unsigned short>::Max() {return USHRT_MAX;}

template<> inline signed char wb_limit<signed char>::Max() {return CHAR_MAX;}
template<> inline unsigned char wb_limit<unsigned char>::Max() {return UCHAR_MAX;}

template<> inline bool wb_limit<bool>::Max() {return true;}


template<> inline double wb_limit<double>::Min() {return -DBL_MAX;}
template<> inline float wb_limit<float>::Min() {return -FLT_MAX;}

template<> inline long wb_limit<long>::Min() {return LONG_MIN;}
template<> inline unsigned long wb_limit<unsigned long>::Min() {return 0;}

template<> inline int wb_limit<int>::Min() {return INT_MIN;}
template<> inline unsigned int wb_limit<unsigned int>::Min() {return 0;}

template<> inline short wb_limit<short>::Min() {return SHRT_MIN;}
template<> inline unsigned short wb_limit<unsigned short>::Min() {return 0;}

template<> inline signed char wb_limit<signed char>::Min() {return CHAR_MIN;}
template<> inline unsigned char wb_limit<unsigned char>::Min() {return 0;}

template<> inline bool wb_limit<bool>::Min() {return false;}

#ifndef ADD_OWN_SPECIALISATION_TO_WB_LIMITS_H
template<class Scalar>
inline Scalar wb_limit<Scalar>::Max() {return DBL_MAX;}
template<class Scalar>
inline Scalar wb_limit<Scalar>::Min() {return DBL_MIN;}
#endif

//Klasa do tworzenia sensownego "missing values" oraz 
//sprawdzania minimum i maksimum dla wszystkich typów skalarnych
template<class Scalar>
class default_missing
{
    Scalar miss;
public:
    default_missing();
    
    operator Scalar() 
    {
        return miss;
    }
};

template<> inline default_missing<double>::default_missing() {miss= DBL_MAX;}
template<> inline default_missing<float>::default_missing() {miss= FLT_MAX;}
template<> inline default_missing<long>::default_missing() {miss= LONG_MIN;}
template<> inline default_missing<unsigned long>::default_missing() {miss= ULONG_MAX;}
template<> inline default_missing<int>::default_missing() {miss= INT_MIN;}
template<> inline default_missing<unsigned int>::default_missing() {miss= UINT_MAX;}
template<> inline default_missing<signed char>::default_missing()  {miss= CHAR_MIN;}
template<> inline default_missing<unsigned char>::default_missing() {miss= UCHAR_MAX;}

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
