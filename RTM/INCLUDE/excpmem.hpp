/**
* \file
* \brief   Generic "out of memory" & "invalid pointer" exception.
*        ------------------------------------------------------------------------
* \details
*          Contents:
*          - class wbrtm::OutOfMemoryExcp;
*          - class wbrtm::InvalidPtrUseExcp;
*
* \date    2022-10-13 (last modification)
* \author  borkowsk
* @ingroup ERRORHANDLING
*/

#ifndef _EXCP_OUT_MEMORY_HPP_
#define _EXCP_OUT_MEMORY_HPP_
#include "excpbase.hpp"
#include <iostream>

/**
 * @ingroup ERRORHANDLING Funkcje o klasy obsługi sytuacji błędnych i wyjątkowych
 */
///@{

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \brief Obsolete class  \ingroup ERRORHANDLING
class OutOfMemoryExcp:public MemoryExcp
{
EXCP_NECESSARY(OutOfMemoryExcp)
object_size_t size;
public:
OutOfMemoryExcp(object_size_t req_size,const char* fn=0,const int fl=0):MemoryExcp(fn,fl),size(req_size){}
void PrintTo(std::ostream&) const;
};

/// \brief Obsolete class \ingroup ERRORHANDLING
class InvalidPtrUseExcp:public MemoryExcp
{
EXCP_NECESSARY(InvalidPtrUseExcp)
pointer ptr;
public:
InvalidPtrUseExcp(pointer iptr,const char* fn=0,const int fl=0):MemoryExcp(fn,fl),ptr(iptr){}
void PrintTo(std::ostream&) const;
};

} //namespace
///@}
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