/** \file   bounded.hpp
 *  \author borkowsk
 *  \brief  wbrtm::bounded class & dedicated exception
 *  \date   2022-12-31 (last modification)
 *  @ingroup OBSOLETE
 */
#ifndef _MSC_VER //# warning still not work under Microsoft C++
//#warning  "This code is not tested in C++11 standard"
#endif

#ifndef _BOUNDED_HPP_
#define _BOUNDED_HPP_
#include "excpbase.hpp"
#include "errorhan.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \brief klasa wyjątku specjalnie dla szablonu wbrtm::bounded
class BoundExcp:public RangCheckExcp
{
EXCP_NECESSARY(BoundExcp)
double low;
double val;
double high;
public:
BoundExcp(double llow,double lval,double lhigh,
	  const char* fname=0,const int fline=0):RangCheckExcp(fname,fline)
	  {low=llow;high=lhigh;val=lval;}
void PrintTo(std::ostream&) const;
};

/// \brief Klasa zapewniająca kontrole wartości przypisywanych na zmienną
/// \details
///     Jest to szablon parametryzowany typem T.
///     Wymagane jest zdefiniowanie operatorów <= i >= dla typu T
///     i liczb (?).
///     Generuje wyjątek gdy wartość inicjująca lub przypisywana jest spoza zakresu.
/// \tparam T - typ wartości
/// \param low - najmniejsza dopuszczalna wartość
/// \param high - największa dopuszczalna wartość
template<class T,T low,T high>
class bounded
{
protected:
    T val;  ///< przechowywana wartość
public:

    /// \brief konstruktor inicjujący wartość. \details Generuje wyjątek gdy wartośc spoza zakresu.
    bounded(const T ini=low)
    {
    if( low<=ini && ini<=high)
        val=ini;
        else
        error_handling::Error(BoundExcp(low,ini,high,__FILE__,__LINE__));
    }

    /// \brief operator przypisana.\details Generuje wyjątek gdy wartośc spoza zakresu.
    bounded& operator = (const T ival)
    {
        if( low<=ival && ival<=high)
            val=ival;
        else
            error_handling::Error(BoundExcp(low,ival,high,__FILE__,__LINE__));
    }

    operator T () {return val;}

};

} //namespace wbrtm

/* *******************************************************************/
/*	       WBRTM  version 2006 - renovated in 2022                   */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  htt...                                               */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
#endif