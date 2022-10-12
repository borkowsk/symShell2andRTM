/**  \file     compatyb.h
* \details  Plik definiujący zastępcze funkcje "C",
*    	    których brak w poszczególnych kompilatorach i systemach
* **************************************************************************
*/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifdef _cplusplus
extern "C" {
#endif
//Zdefiniowana w osobnym pliku z rozszerzeniem .c !!!

/// \brief Porównanie stringów ignorujące wielkość liter.
int stricmp(const char *s1, const char *s2);

/// \brief Przekształcenie łańCucHa na wersję małoliterową.
const char *strlwr(char *what);

/// \brief Przekształcenie łańCucHa na wersję WIELKOLITEROWĄ.
const char *strupr(char *what);

#ifdef _cplusplus
} //extern "C"
#endif


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


