/* Global operator new for forcing constructor calling */
#ifndef _VOIDNEW_HPP_
#define _VOIDNEW_HPP_
#include <new>

//namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

#ifdef _CPPUNWIND
inline void* operator new (size_t,void* p) 	{ return p; } //Placement new - need to be global?

//Exception handling required plecement delete for each plecement new.
//but I am not shure that this solution will work properly.
//It is not tested with exception handling. It is possible that 
//this delete may sometimes cause double calling of destructors
//of some objects, for example items covered in array_of_class<...>
inline void operator delete(void *pMem,void* p)//Placement delete
{
	//Nothing to do!!!
}
#endif

//} //namespace

/********************************************************************/
/*			          WBRTM  version 2006/2022                      */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif