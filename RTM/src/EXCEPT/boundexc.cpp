#define IMPLEMENT_BOUNDED
#define USES_IOSTREAM
#include "INCLUDE/uses_wb.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

void BoundExcp::PrintTo(ostream& o) const
{
o<<"BOUNDS FAULT:"<< low <<"<="<< val <<"<="<< high <<" RAISED";
ExcpRaisePosition::PrintTo(o);
}

} //namespace

/********************************************************************/
/*			          WBRTM  version 2006                           */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/