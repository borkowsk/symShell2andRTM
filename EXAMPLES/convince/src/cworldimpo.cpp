/// @file
/// Virtual output methods implementation of the aWorld class.
/// Its is rather simply implementation. You can replace it if you need.
//======================================================================
/// @date 2026-05-17 (modified)

#include "cworld.h"

int aWorld::implement_output(ostream& o) const
{
    int ret=world::implement_output(o);
    if(ret!=1) return ret;
    o<<separator;
    o<<MyWidth<<separator;
    o << anAgent::MaxStrength << separator;
    o << anAgent::MinStrength << separator;
    o << anAgent::NumOfCate << separator;

    o<<Agenci<<endl;
    return 1;
}

/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */


