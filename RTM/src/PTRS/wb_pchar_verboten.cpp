//	Implementation of I/O for wb_sptr<char> and wb_pchar
//	Verboten character that must be enclosed in printing  
// /////////////////////////////////////////////////////////////////////
#include "wb_ptr.hpp"
#include "wb_ptrio.h"
//#include "wb_pchario.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

const char* WB_PCHAR_VERBOTEN_CHARS=" \t\n\r\'\"\0\01\02\03\04\05\06\07\010"; //Znaki zabronione dla zwykłego zapisu

} //end of namespace wbrtm

/* *******************************************************************/
/*			           WBRTM  version 2022                           */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*         WWW:  ...                                                 */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
