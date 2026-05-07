/// @file
/// @brief Virtual Input method implementation of the aWorld class.
/// @date 2026-05-07 (modified)
///       ------------------------------------------------------------
/// @details
///     Its is rather simply implementation. You can replace it if you need.
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "cworld.h"

int aWorld::implement_input(istream& i)
{
    int ret=world::implement_input(i);
    if(ret!=1) return ret;
    i>>MyWidth;
    i>>anAgent::MaxStrength;
    i>>anAgent::MinStrength;
    i>>anAgent::NumOfCate;

    symshell2::rectangle_geometry* Geom=(symshell2::rectangle_geometry*)Agenci.get_geometry();
                                                                                                     assert(Geom!=NULL);
    if(Geom->get_width()!=MyWidth||
       Geom->get_height()!=MyWidth )
            Geom->set(MyWidth,MyWidth,1);
    i>>Agenci;
    return 0;
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




