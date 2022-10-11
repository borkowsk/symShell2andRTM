#ifndef _EXCPOTHR_HPP_
#define _EXCPOTHR_HPP_
#include "excpbase.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

class InvalidIndexExcp:public RangCheckExcp
{
EXCP_NECESSARY(InvalidIndexExcp)
unsigned long I;
public:
InvalidIndexExcp(unsigned long i,const char* fn=0,const int fl=0):RangCheckExcp(fn,fl),I(i){}
void PrintTo(std::ostream&) const;
};

} //namespace

/********************************************************************/
/*			          WBRTM  version 2006                           */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif