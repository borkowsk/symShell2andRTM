/// @file
/// @brief
///  @EN{ Virtual Input method implementation of the 'convWorld' type. }
///  @PL{  }
/// @date 2026-05-21 (modified)
///       ===========================================================
/// @details
///     Its is rather simply implementation. You can replace it if you need.
//======================================================================================================================

#include "cworld.h"

using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

int convWorld::implement_input(istream& i)
{
    int ret=world::implement_input(i);
    if(ret!=1) return ret;
    i>>MyWidth;
    i >> convAgent::MaxStrength;
    i >> convAgent::MinStrength;
    i >> convAgent::NumOfCate;

    rectangle_geometry* Geom=(rectangle_geometry*)Agenci.get_geometry();
                                                                                                     assert(Geom!=NULL);
    if(Geom->get_width()!=MyWidth||
       Geom->get_height()!=MyWidth )
            Geom->set(MyWidth,MyWidth,1);
    i>>Agenci;
    return 0;
}

/* ****************************************************************** */
/*        SYMSHELL2 EXAMPLE  version 2006/2022/2026                   */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/              */
/*        MAIL:  wborkowski@uw.edu.pl                                 */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */






