/** @file     compatyb.h
 * @brief Plik definiujący zastępcze funkcje "C" (strlwr,strupr,sticmp).
 * @details
 *    Funkcji tych  brak w poszczególnych lub większości kompilatorów.
 *    Prawdopodobnie pochodzą z rozszerzeń Borland C++ albo MSVC
 * @date  2026-03-26 (last modification)
 * **************************************************************************
 */
#ifndef _COMPATYB_H_INCLUDED_
#define _COMPATYB_H_INCLUDED_

#ifdef __cplusplus
#include <cctype>
#include <cstdlib>
#include <cstring>
#else
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#endif

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
		/// \brief Przekształcenie łańcucha char* na wersję małoliterową "in place".
		const char* strlwr(char* what);

	wb_unused_attr
		/// \brief Przekształcenie łańcucha char* na wersję WIELKOLITEROWĄ "in place".
		const char* strupr(char* what);

#ifdef __cplusplus
} //extern "C"
#endif

#endif

#undef wb_unused_attr

/* ************************************************************* ** */
/*                     WBRTM  version 2026                          */
/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* **************************************************************** */
#endif


