/// @file
/// Virtual Input method implementation of the aWorld class.
/// Its is rather simply implementation. You can replace it if you need.
// /////////////////////////////////////////////////////////////////////
/// @date 2026-04-07 (modified)

#include "cworld.h"

int aWorld::implement_input(istream& i)
{
    int ret=world::implement_input(i);
    if(ret!=1) return ret;
    i>>MyWidth;
    i>>anAgent::max_sila;
    i>>anAgent::min_sila;
    i>>anAgent::ile_kate;

    rectangle_geometry* Geom=(rectangle_geometry*)Agenci.get_geometry();
                                assert(Geom!=NULL);
    if(Geom->get_width()!=MyWidth||
       Geom->get_height()!=MyWidth )
            Geom->set(MyWidth,MyWidth,1);
    i>>Agenci;
    return 0;
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



