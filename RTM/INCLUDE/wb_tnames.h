/** \file wb_tnames.h
 *  \brief
 *        @PL{  }
 *        @EN{ Templates for input and output of 'wbrtm::wb_pchar' objects. }
 *  @date 2026-06-03 (last modification)
 *          ----------------------------------------------------------------
 *  \details ... ...
 *           ... ...
 *
 *  \author Wojciech Borkowski @ Institut for Social Studies, University of Warsaw
 *  \note Like many others wbrtm files created around year 1996.
 */
#ifndef TYPENAMES_H_INCLUDED_
#define TYPENAMES_H_INCLUDED_

#include <cstddef>
#include <climits>

/**
 * @defgroup TypesService Informacja o typach i nazewnictwo
 * \brief @PL{ Ładne i przenośne nazwy typów, limity typów itp. } @EN{ ... }
 */
/// @{

#if defined( __TURBOC__ ) && defined( __MSDOS__ ) //Very old Borland in 16bit DOS platform.
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

#ifdef IRIX32 //IRIX  32bit unix  platform
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
typedef word32  object_size_t;
typedef word32        address;  //unsigned type as long as pointer
typedef void*         pointer;
#endif

#if defined( __WIN32__) //MS WINDOWS NT and later.
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
typedef void*         pointer;
typedef size_t        address;  //unsigned type as long as pointer
#endif

#if defined( __GNUC__ ) // 32 and 64bit bit linux compilers.
#include <cstddef>
#include <cstdint>
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
typedef void*         pointer;
typedef uintptr_t     address;  // unsigned type as long as pointer
#endif

#if false //sizeof(void*)==4
#define  FULL			  ((void*)(0xffffffff) )   // MAX 32bit POINTER
#else
#define  FULL			  ((void*)(0xffffffffffffffff))   // MAX of 64bit POINTER. Nie może być constexpr bo reinterpret cast.
#endif

constexpr auto MAXOBJECTSIZE = (object_size_t)( ULONG_MAX );
constexpr auto MAXSIZE 		 = (size_t) ( ULONG_MAX );

static_assert( sizeof (FULL) == sizeof (void*) );

// Very portable types (?).
typedef unsigned char   uchar;
typedef signed   char   schar;
typedef float           single;
typedef long double     ldouble;

/// @}

/* ******************************************************************/
/*                     WBRTM version TODO 2026                           */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif/* END OF HEADER */




