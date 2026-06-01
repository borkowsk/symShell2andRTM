/** @file
 * @brief
 *        @PL{ Plik definiujący zastępcze funkcje "C" (strlwr,strupr,sticmp). }
 *        @EN{ File defining "C" replacement functions (strlwr,strupr,sticmp). }
 * @date 2026-06-01 (last modification)
 *       =======================================================================
 * @details
 * @if POLISH
 *    Funkcji tych  brak w poszczególnych lub większości kompilatorów.
 *    Prawdopodobnie pochodzą z rozszerzeń Borland C++ albo MSVC
 * @elseif ENGLISH
 *    These functions are missing from some or most compilers.
 *    They likely come from Borland C++ or MSVC extensions.
 * @endif
 * @note Cała biblioteka WB_RTM to jest nieco odremontowane muzeum kodu z wieku XX.!!!
 * ********************************************************************************************************************
 */
#ifndef COMPATIBILITY_H_INCLUDED_
#define COMPATIBILITY_H_INCLUDED_

#ifdef __cplusplus
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>
#else
#include <stdio.h>
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
        /// \brief @PL{ Porównanie stringów char* ignorujące wielkość liter. } @EN{  }
        int stricmp(const char* s1, const char* s2);

    wb_unused_attr
        /// \brief @PL{ Przekształcenie łańcucha char* na wersję małoliterową "in place". } @EN{  }
        const char* strlwr(char* what);

    wb_unused_attr
        /// \brief @PL{ Przekształcenie łańcucha char* na wersję WIELKOLITEROWĄ "in place". } @EN{  }
        const char* strupr(char* what);

    wb_unused_attr inline
        /// \brief @PL{ Konwersja funkcyjna liczby całkowitej na string. } @EN{  }
        ///	\details
        ///     @PL{ Funkcja nie jest częścią standardu C, ale jest powszechna w systemach Windows/MSVC }
        ///     @EN{  }
        const char* ltoa(long value, char* str, int radix)
        {
            if (radix == 10) {
                sprintf(str, "%ld", value);
            } else if (radix == 16) {
                sprintf(str, "%lx", value);
            } else {
                // Opcjonalnie: obsługa innych podstaw, jeśli zaszłaby taka potrzeba
                return "ltoa: bases other than 10 * 16 are not implemented!\0";
            }
            return str;
        }

    wb_unused_attr inline
        /// \brief  @PL{ Konwersja funkcyjna liczby zmiennoprzecinkowej na string. } @EN{  }
        ///	\details
        ///     @PL{ Funkcja nie jest częścią standardu C, ale jest użyteczna. }
        ///     @EN{  }
        const char* dtoa(double value, char* str)
        {
            sprintf(str, "%g", value);
            return str;
        }

#ifdef __cplusplus
} //extern "C"
#endif

#endif

#undef wb_unused_attr

// #ifndef NULL
// #define NULL nullptr
// #endif

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


