/** \file struprstrlwr.c
 * \brief Implementation of old strlwr & strupr
 * \details Splited from stricmp.c on 11.10.22.
 * \author borkowsk
 * \date 2022-10-25 (last modification)
*/
#include <ctype.h>
#include <string.h>

/* Te funkcje są zdefiniowane w MSVC więc ich kompilowanie jest zbędne! */

#ifndef _MSC_VER 

/// \param   what - tekst do zmiany
/// \details Zmiany dotyczą bezpośrednio parametru \p 'what'
const char *strlwr(char *what)
{
    if (what == NULL) return NULL;

    char *pom = what;
    while (*pom) {
        *pom = tolower(*pom);
        pom++;
    }
    return what;
}

/// \param   what - tekst do zmiany
/// \details Zmiany dotyczą bezpośrednio parametru \p 'what'
const char *strupr(char *what)
{
    if (what == NULL) return NULL;

    char *pom = what;
    while (*pom) {
        *pom = toupper(*pom);
        pom++;
    }
    return what;
}

#endif


/* *******************************************************************/
/*	       WBRTM  version 2006 - renovated in 2022                   */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  htt...                                               */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/