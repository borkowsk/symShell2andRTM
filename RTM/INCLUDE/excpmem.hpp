//	Generic "out of memory" & "invalid pointer" exception.
////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _EXCP_OUT_MEMORY_HPP_
#define _EXCP_OUT_MEMORY_HPP_
#include "excpbase.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

class OutOfMemoryExcp:public MemoryExcp
{
EXCP_NECESSARY(OutOfMemoryExcp)
object_size_t size;
public:
OutOfMemoryExcp(object_size_t req_size,const char* fn=0,const int fl=0):MemoryExcp(fn,fl),size(req_size){}
void PrintTo(ostream&) const;
};

class InvalidPtrUseExcp:public MemoryExcp
{
EXCP_NECESSARY(InvalidPtrUseExcp)
pointer ptr;
public:
InvalidPtrUseExcp(pointer iptr,const char* fn=0,const int fl=0):MemoryExcp(fn,fl),ptr(iptr){}
void PrintTo(ostream&) const;
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