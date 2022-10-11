#ifndef _ERRORHANDLE_HPP_
#define _ERRORHANDLE_HPP_
/* C++ EXCEPTIONS AND ERROR SUPPORT */

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

class  WB_Exception_base; ///< Base for all exceptions in wbrtm

class error_handling
{
public:
/// Error support driven by Exception hierarchy
/// \return  1 if may try to cleanup
///          0 if may try to resume
static int Error(const WB_Exception_base&);

/// \brief USABLE ENUM TYPE FOR RunTime... & Text... Exceptions.
enum ecode { OK=0,  ///< OK must be 0
	     NULL_USE_OBSOLETE,
	     ALLOC_ERROR_OBSOLETE,
	     NOT_FOUND,
	     INVALID_KEY,
	     INTERNAL_FAULT,
	     RANGE_ERROR,
	     IOERROR,
	     OTHER_ERROR=256 ///< 256 PREDEFINED CODES. "OTHER" MUST BE LAST!
        };
};

typedef error_handling errh;
} //namespace

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
#endif