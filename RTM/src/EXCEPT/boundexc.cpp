/** @file
 *  @brief
 *  @PL{  }
 *  @EN{ Print functions for exceptions for wbrtm::bounded }
 *  @date 2026-05-19 (last modification)
 *        =================================================================
 *  \author borkowsk
 */
#include "bounded.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

///    \ingroup ERRORHANDLING
void BoundExcp::PrintTo(std::ostream& o) const
{
    o<<"BOUNDS FAULT:"<< low <<"<="<< val <<"<="<< high <<" RAISED";
    ExcpRaisePosition::PrintTo(o);
}

} //namespace

/* *******************************************************************/
/*                      WBRTM  version 2026                          */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  htt...                                               */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
