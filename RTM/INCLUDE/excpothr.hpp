/**
* \file
* \brief   Generic "invalid index" exception.
*        ------------------------------------------------------------------------
* \details
*          Contents:
*          - class wbrtm::InvalidIndexExcp;
*
* \date    2022-10-13 (last modification)
* \author  borkowsk
* @ingroup ERRORHANDLING
*/


#ifndef _EXCPOTHR_HPP_
#define _EXCPOTHR_HPP_

#include "excpbase.hpp"

/**
 * @ingroup ERRORHANDLING Funkcje o klasy obsługi sytuacji błędnych i wyjątkowych
 */
///@{

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \brief Obsolete class \ingroup ERRORHANDLING
class InvalidIndexExcp:public RangCheckExcp
{
EXCP_NECESSARY(InvalidIndexExcp)
unsigned long I;
public:
InvalidIndexExcp(unsigned long i,const char* fn=0,const int fl=0):RangCheckExcp(fn,fl),I(i){}
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