/// @file
/// @brief **Implementation of `any_layer` class -- Input operator >>  **
/// @date 2026-05-17 (modified)
///       ----------------------------------------------
/// @details
///      Its is rather simply implementation. You can replace it if you need.
//==============================================================================
//
#include "layer.hpp"

using namespace std;

namespace sym2 { namespace shell {

    istream &operator>>(istream &i, any_layer_base &w)
    {
        char cpom;
        i >> cpom;
        if(cpom != '{')
        {
            cerr << "Incorrect syntax: '{' expected." << endl;
        }

        w.implement_input(i); //Call virtual input method.

        i >> cpom;
        if(cpom != '}')
        {
            cerr << "Incorrect syntax: '}' expected." << endl;
        }
        return i;
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

