/*
C++ CHECK PLATFORM FOR PLATFORM RELEATED DEFINITIONS
*/
#ifndef _PLATFORM_HPP_
#define _PLATFORM_HPP_

#ifndef __cplusplus
#error C++ required
#endif

#include "platform.h"

#if defined(__MSVC_2000__) // || defined()

#ifndef NEW_FASHION_CPP // MSC++ use standard C++ library etc. 
#define NEW_FASHION_CPP  (1) 
#endif

#ifndef _CRT_SECURE_NO_DEPRECATE
#	define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4996)
//TYMCZASEM - OSTRZE¯ENIA O "conversion from 'A' to 'B', possible loss of data"
#pragma warning(disable : 4267) 
#pragma warning(disable : 4244)
#endif

//using namespace std;
#endif

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

extern unsigned _lingo_selector;

inline
const char* _lingo(const char* _PL,const char* _IN)
{
	if(_lingo_selector==0)//LOCAL_VERSION
		return _PL;
	else
		return _IN;
}

} //namespace
using wbrtm::_lingo;

#define _LPL  (_lingo)

/*  //Insert this where you want:
#ifndef LOCAL_VERSION
	unsigned wbrtm::_lingo_selector=0;
#else
	unsigned wbrtm::_lingo_selector=1;
#endif
*/

/********************************************************************/
/*			          WBRTM  version 2013                           */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://borkowski.iss.uw.edu.pl/                     */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif