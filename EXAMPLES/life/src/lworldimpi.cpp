/// @file
/// Implementation of the lifeworld class - virtual Input method
//---------------------------------------------------------------
/// Its is rather simply implementation. You can replace it if you need.
// ////////////////////////////////////////////////////
/// @date 2026-03-27 (modified)

#include "lworld.h"

int lifeworld::implement_input(istream& i)
{
    int ret=world::implement_input(i);
    if(ret!=1) return ret;
    i>>MyWidth;		//Obwód torusa

    i>>IleKate;		//Ilość kategorii w mapach
    i>>IleSasiad;	//8 == Gęstość sąsiedztwa
    i>>OdlSasiad;	//Rozmiar sąsiedztwa

    i>>Noise;		//Szum informacyjny
    rectangle_geometry* Geom=(rectangle_geometry*)Agenci.get_geometry();                          assert(Geom!=nullptr);
    if(Geom->get_width()!=MyWidth||
       Geom->get_height()!=MyWidth )
            Geom->set(MyWidth,MyWidth,1);
    i>>Agenci;
    return 0;
}

// /////////////////////////////////////////////
// Example for SYMSHELL CLASSES library.
/// @author Wojciech Borkowski, iss.uw.edu.pl
// https://github.com/borkowsk/symShell2andRTM
/// @copyright 2000 - 2026
// /////////////////////////////////////////////

