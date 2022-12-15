/** \file stricmp.c
 * \brief Implementation of stricmp
 *
 * \author Peter 'png' Hille <peter@das-system-networks.de>
 *
 * https://github.com/png85/dsncompat/blob/master/string/stricmp.c
 */

#include <string.h>
#include <assert.h>
#include "compatyb.h"

#ifdef unix
#define HAVE_STRCASECMP 1
#endif

/** W MSVC funkcja stricmp() jest dostępna z biblioteki */

#ifndef _MSC_VER

/** \brief Compare Strings without Case Sensitivity
 *
 * stricmp compares s1 and s2 without sensitivity to case. All alphabetic
 * characters in the two arguments s1 and s2 are converted to lowercase
 * before the comparision.
 *
 * The function operates on null-ended strings. The string arguments to the
 * function are expected to contain a null character (\0) at the end of the
 * string.
 *
 * \param s1 Pointer to a \0-terminated C string
 * \param s2 Pointer to a \0-terminated C string
 *
 * \return A value indicating the relationship between the two strings similar
 * to the one returned by strcmp(3)
 *
 * \see http://publib.boulder.ibm.com/infocenter/iseries/v7r1m0/index.jsp?topic=%2Frtref%2Fstricmp.htm
 * \see strcasecmp
 * \see strnicmp
 *
 * \note In debug builds the two pointer arguments are checked for nullptr
 * values using assert(3)
 *
 * \attention This really shouldn't be used in new projects because it doesn't
 * do any bounds checking on the underlying memory areas!
 */
#ifdef __cplusplus
extern "C" {
#endif

int stricmp(const char *s1, const char *s2)
{                                                                                      assert(s1 != NULL && s2 != NULL);
#ifdef HAVE_STRCASECMP
    return strcasecmp(s1, s2);
#else
    while (tolower((unsigned char) *s1) == tolower((unsigned char) *s2)) {
      if (*s1 == '\0')
        return 0;
      s1++; s2++;
    }

    return (int) tolower((unsigned char) *s1) -
      (int) tolower((unsigned char) *s2);
#endif /* !HAVE_STRCASECMP */
}

const char *strlwr(char *what)
{
    if (what == NULL)
        return NULL;

    char *pom = what;
    while (*pom) {
        *pom = tolower(*pom);
        pom++;
    }
    return what;
}

const char *strupr(char *what)
{
    if (what == NULL)
        return NULL;

    char *pom = what;
    while (*pom) {
        *pom = toupper(*pom);
        pom++;
    }
    return what;
}

#ifdef __cplusplus
} //extern "C"
#endif

#endif

/* *******************************************************************/
/*	       WBRTM  version 2006 - renovated in 2022.12                */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  htt...                                               */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
