/// @file
/// @brief **Implementation of the world type -- output operator <<  ** /<br>
///         _Implementacja operatora << dla klasy świata._
/// @date 2026-05-19 (modified)
// ----------------------------------------------
// Its is rather simply implementation. You can replace it if you need.
//=====================================================================

#include "world.hpp"

/// Wersja globalna. Na to wygląda...
ostream& operator << (ostream& o,const sym2::shell::world& w)
{
    if(!o || o.bad())
    {
        cerr<<"Sorry! Invalid or bad stream."<<endl;
        exit(2);
    }

    o<<w.SimulName<<w.sepa();
    if(o.fail())
    {
        cerr<<"Sorry! Write failed."<<endl;
        exit(2);
    }

    o<<'{'<<endl;
    w.implement_output(o); //Call virtual output method.
    o<<'}'<<endl;

    if(o.fail())
    {
        cerr<<"Sorry! Write failed."<<endl;
        exit(2);
    }
    return o;
}


/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/            */
/*        MAIL:  wborkowski@uw.edu.pl                               */
/*                               (Don't change or remove this note) */
/********************************************************************/


