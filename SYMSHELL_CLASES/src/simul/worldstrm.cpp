/// @file
/// @brief **Implementation of the world class -- output operator <<  ** /<br>
///         _Implementacja operatora << dla klasy świata._
/// @date 2026-05-17 (modified)
///       -------- ----------------------------------------------
/// @details
///     Its is rather simply implementation. You can replace it if you need.
//=====================================================================================================================
#include "world.hpp"

namespace sym2 { namespace shell {


    ostream &operator<<(ostream &o, const world &w)
    {
        o << w.SimulName << w.sepa();
        o << '{' << endl;
        w.implement_output(o); //Call virtual output method.
        o << '}' << endl;
        return o;
    }

}}

/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */


