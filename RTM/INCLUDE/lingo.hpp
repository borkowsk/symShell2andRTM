#ifndef _WBRTM_LINGO_HPP_
#define _WBRTM_LINGO_HPP_

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

#define _LPL  (wbrtm::_lingo)
#define lang  (wbrtm::_lingo)

/*  //Insert this where you want:
#ifndef LOCAL_VERSION
	unsigned wbrtm::_lingo_selector=0;
#else
	unsigned wbrtm::_lingo_selector=1;
#endif
*/

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
#endif
