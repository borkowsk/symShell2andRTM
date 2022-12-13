/**  \file     compatyb.h
* \details  Plik definiujący zastępcze funkcje "C" (strlwr,strupr,sticmp)
*    	    których brak w poszczególnych kompilatorach i systemach
* \date  2022-10-25 (last modification)
* **************************************************************************
*/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#define __MSVC__	(true)
#define wb_unused_attr    /*__attribute__((unused))*/

/** Funkcje  stricmp, strlwr i strupr są dostępne w MSVC, 
*   więc nie ma powodu ich tu dodawać, ale można przedefiniować strcasecmp()
*/

#define strcasecmp( A , B )    _stricmp((A),(B))

#else
#define wb_unused_attr    /*__attribute__((unused))*/

#ifdef __cplusplus
extern "C" {
#endif
	//Zdefiniowana w osobnym pliku z rozszerzeniem .c !!!

	wb_unused_attr
		/// \brief Porównanie stringów char* ignorujące wielkość liter.
		int stricmp(const char* s1, const char* s2);

	wb_unused_attr
		/// \brief Przekształcenie łańCucHa char* na wersję małoliterową "in place".
		const char* strlwr(char* what);

	wb_unused_attr
		/// \brief Przekształcenie łańCucHa char* na wersję WIELKOLITEROWĄ "in place".
		const char* strupr(char* what);

#ifdef __cplusplus
} //extern "C"
#endif

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


