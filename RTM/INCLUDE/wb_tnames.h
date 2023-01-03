#ifndef _TYPENAMES_H_ /* BEGIN OF HEADER */
#define _TYPENAMES_H_

#include <stddef.h>
#include <limits.h>
//#include "platform.hpp"

#if false //sizeof(void*)==4
#define  FULL			((void*)(0xffffffff) )   // MAX 32bit POINTER
#else
typedef  unsigned long int object_size_t;
constexpr auto MAXOBJECTSIZE = (object_size_t)( ULONG_MAX );
constexpr auto MAXSIZE 		 = (size_t) ( ULONG_MAX );
#define  FULL			  ((void*)(0xffffffffffffffff))   // MAX of 64bit POINTER. Nie może być constexpr bo reinterpret cast.
#endif

static_assert( sizeof (FULL) == sizeof (void*) );

// Very portable
typedef unsigned char   uchar;
typedef signed   char   schar;
typedef float	        single;
typedef long double     ldouble;

#if defined( __TURBOC__ ) && defined( __MSDOS__ )
//Borland in 16bit DOS platform
#include <dos.h>
typedef unsigned long  word32;
typedef unsigned long  uint32;
typedef signed   long   int32;
typedef unsigned short word16;
typedef unsigned short uint16;
typedef signed   short  int16;
typedef unsigned char   word8;
typedef unsigned char   uint8;
typedef signed   char    int8;
class                 address
	{          word32 val; //unsigned type as long as pointer
	public:
	address(void* iptr) { val=FP_SEG(iptr)*16L+FP_OFF(iptr); }
	operator word32 ()  { return val; }
	operator void*  ()  { word16 seg=val/16;
			      word16 off=val%16;
			      return MK_FP(seg,off); }
	};
typedef word32  object_size_t;
typedef void   *huge  pointer; // Allow object much longer than 16bit size_t
typedef word32          off_t; // Because not in types.h
#endif

#if defined( __WIN32__)
typedef unsigned int  word32;
typedef unsigned int  uint32;
typedef signed   int   int32;
typedef unsigned short word16;
typedef unsigned short uint16;
typedef signed   short  int16;
typedef unsigned char   word8;
typedef unsigned char   uint8;
typedef signed   char    int8;
typedef word32	      address;  //unsigned type as long as pointer
typedef word32  object_size_t;
typedef void*         pointer;
#endif

#if defined( __GNUC__ )
#include <stddef.h>
typedef unsigned int   word32;
typedef unsigned int   uint32;
typedef signed   int    int32;
typedef unsigned short word16;
typedef unsigned short uint16;
typedef signed   short  int16;
typedef unsigned char   word8;
typedef unsigned char   uint8;
typedef signed   char    int8;
typedef size_t  object_size_t;
typedef size_t	      address;  //unsigned type as long as pointer
typedef void*         pointer;
#endif

#ifdef IRIX32
#include <stddef.h>
//IRIX  32bit unix  platform
typedef unsigned int   word32;
typedef unsigned int   uint32;
typedef signed   int    int32;
typedef unsigned short word16;
typedef unsigned short uint16;
typedef signed   short  int16;
typedef unsigned char   word8;
typedef unsigned char   uint8;
typedef signed   char    int8;
typedef word32  object_size_t;
typedef word32	      address;  //unsigned type as long as pointer
typedef void*         pointer;
#endif




#endif/* END OF HEADER */


/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/



