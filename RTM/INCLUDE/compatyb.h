/**  \file     compatyb.h
* \details  Plik definiujący zastępcze funkcje "C",
*    	    których brak w poszczególnych kompilatorach i systemach
* \date  2022-10-19 (last modification)
* **************************************************************************
*/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define wb_unused_attr    __attribute__((unused))

#ifdef _cplusplus
extern "C" {
#endif
//Zdefiniowana w osobnym pliku z rozszerzeniem .c !!!

wb_unused_attr
/// \brief Porównanie stringów char* ignorujące wielkość liter.
int stricmp(const char *s1, const char *s2);

wb_unused_attr
/// \brief Przekształcenie łańCucHa char* na wersję małoliterową "in place".
const char *strlwr(char *what);

wb_unused_attr
/// \brief Przekształcenie łańCucHa char* na wersję WIELKOLITEROWĄ "in place".
const char *strupr(char *what);

#ifdef _cplusplus
} //extern "C"
#endif

#undef wb_unused_attr

/* *******************************************************************/
/*                     WBRTM  version 2022                           */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*         WWW:  ...                                                 */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/


