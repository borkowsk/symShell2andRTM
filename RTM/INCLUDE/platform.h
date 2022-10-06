/********************************************************************/
/* PLATFORM CHECKING FOR LIBRARIES MADE BY Wojciech Borkowski       */
/********************************************************************/

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#ifndef NEW_FASHION_CPP
#define NEW_FASHION_CPP		(1)  /* Now almoust all are "new fasioned" */
#endif

/* __MSVC__	- defined for Microsoft Visual C++	*/

/* __MSDOS__  	- selfdefined by Borland and djgpp	*/

/* __BORLANDC__ - selfdefined by Borland		*/
/* __DJGPP__  	- selfdefined only by version 2.	*/

/* __BORLANDC16__ - defined for 16 bit Borland C mode	*/

/* __DOS16__    - defined for 16bit DOS platform	*/
/* __DOS32__    - defined for 32bit DOS platform	*/
/* __WIN16__	- defined for MS Windows 16bit aplications */
/* __WIN32__	- defined for MS Windows 32bit aplications */
/* __IRIX32__ 	- defined for Delta/C++ on IRIX 5.3 	*/
/* __IRIX64__ 	- defined for Delta/C++ on IRIX 6.X 	*/

# define ENABLE_LARGE_STATIC_DATA      1	/*default*/


#if defined(_MSC_VER)	/* && (defined( _WINDOWS ) || defined( _CONSOLE )) */
						/* MSVC++ specific defines */
						
#define __MSVC__		//????

#if _MSC_VER > 1300
#define __MSVC_2000__
#endif

#if defined(WIN32) || defined(_WIN32) //OBSOLETE!!!
#define __WIN32__
#else
#define __WIN16__
#endif

#if defined( _WINDOWS )
#define _Windows /* BCC compatybylity */
#endif

/*end MSVC++ specyfic */

#elif defined(__BORLANDC__)
/*#error UNTESTED OR UNSUPPORTED PLATFORM*/
# if __BCPLUSPLUS__ <= 0x0310
# define __BORLANDC16__
# define __DOS16__
# define _CAREFULL_FOR_16_BIT_WORD_  // For size dependent #ifdef... code
# undef	 ENABLE_LARGE_STATIC_DATA
# define ENABLE_LARGE_STATIC_DATA      0
# endif

#if __BCPLUSPLUS__ > 0x0310
/*#error UNTESTED OR UNSUPPORTED PLATFORM*/
#	ifndef _Windows
# define __BORLANDC16__
# define __DOS16__
#	else
#		ifdef __WIN32__
# 	define __BORLANDC32__
# 		else
# 	define __BORLANDC16__
#  define __WIN16__
#	endif
# 	endif
# endif

#elif defined( __GNUC__) && defined( __MSDOS__ )

#	if	!defined( __DJGPP__ )
#define __DJGPP__  	1
#	endif
# define __DOS32__

#elif defined(mips) || defined(__mips) || defined( __host_mips )
#	if _MIPS_SZINT ==32
/* IRIX  32bit unix  platform */
#	define __IRIX32__
#	define   IRIX32  /* Stara wersja define'u */
#	elif _MIPS_SZINT ==64
/* IRIX  64bit unix  platform */
#	define __IRIX64__
#error  UNTESTED OR UNSUPPORTED PLATFORM
#	endif
#endif


/* SETTING BYTE ORDER INFORMATION */
#if defined( __WIN32__ ) 
#define INTEL_BYTE_ORDER
#endif

#if defined( __IRIX32__)
#define MIPS_BYTE_ORDER
#endif


/* MAX LEN OF PATH */
#ifndef PATH_MAX

#if defined( __BORLANDC__ )
#define PATH_MAX MAXPATH
#elif defined( __MSVC__)
#define PATH_MAX _MAX_PATH
#else
#define PATH_MAX MAXPATHLEN
#endif

#endif


#ifdef __MSVC__
#ifdef __MSVC_2000__
#include "kill_huge_etc.h"
#endif
#ifdef _CPPRTTI
#include <typeinfo.h>
#endif

#endif


#ifdef unix
#include <unistd.h>
#endif

#include <limits.h>

#ifndef __MSVC__
#include <values.h> //W nowym raczej nie ma
#endif

#endif /* FILE platform.h */


/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/



