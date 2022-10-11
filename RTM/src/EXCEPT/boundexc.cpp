/** \file excbounded.cpp
 *  \author borkowsk
 *  \brief Print functions for exceptions for wbrtm::bounded
 *  \date 2022-10-11 (last modification)
 */
#include "bounded.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

void BoundExcp::PrintTo(std::ostream& o) const
{
o<<"BOUNDS FAULT:"<< low <<"<="<< val <<"<="<< high <<" RAISED";
ExcpRaisePosition::PrintTo(o);
}

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