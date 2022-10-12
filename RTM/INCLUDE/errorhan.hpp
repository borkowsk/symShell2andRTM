/** \file
 *  \author  borkowsk
 *  \brief   C++ EXCEPTIONS AND ERROR SUPPORT
 *  \date   2022-10-12 (last modification)
 */
#ifndef _ERRORHANDLE_HPP_
#define _ERRORHANDLE_HPP_

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

class  WB_Exception_base; ///< Base for all exceptions in wbrtm

/// \brief Obsolete class TODO renew
class error_handling
{
public:

/// Error support driven by Exception hierarchy
/// \return  1 if may try to cleanup
///          0 if may try to resume
static int Error(const WB_Exception_base&);

/// \brief USABLE ENUM TYPE FOR RunTime... & Text... Exceptions.
enum ecode { OK=0,        ///< OK must be 0
	     NULL_USE_OBSOLETE,
	     ALLOC_ERROR_OBSOLETE,
	     NOT_FOUND,      ///< General "something not found" error.
	     INVALID_KEY,    ///< Invalid key error (in associative arrays)
	     INTERNAL_FAULT, ///< Internal error
	     RANGE_ERROR,    ///< Range check error
	     IOERROR,        ///< Input/output error
	     OTHER_ERROR=256 ///< 256 PREDEFINED CODES. "OTHER" MUST BE LAST!
        };
};

typedef error_handling err_handl;
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