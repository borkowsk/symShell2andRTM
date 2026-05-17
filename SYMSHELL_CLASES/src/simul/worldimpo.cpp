/// @file
/// @brief  **Implementation of the world class -- Virtual output methods and operator <<  **
/// @date 2026-05-17 (modified)
///         ----------------------------------------------
/// @details
///     Its is rather simply implementation. You can replace it if you need.
//======================================================================================================================

#include "world.hpp"

char  sym2::shell::world::separator='\t';

int sym2::shell::world::implement_output(ostream& o) const
{
    o << StepCounter << separator;
    o<<TimeStamp<<separator<<endl;
    o<<MaxIterations<<separator;
    o<<LogRatio<<separator;
    return 1;
}

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



