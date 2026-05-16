/// @file
/// @brief Implementation of the lifeworld class - virtual Input method
/// @date 2026-05-16 (modified)
//---------------------------------------------------------------------
// Its is rather simply implementation. You can replace it if you need.
// /////////////////////////////////////////////////////////////////////


#include "lworld.h"

int lifeworld::implement_input(istream& i)
{
    int ret=world::implement_input(i);
    if(ret!=1) return ret;
    i>>MyWidth;		//Obwód torusa

    i>>IleKate;		//Liczba kategorii w mapach
    i>>IleSasiad;	//8 == Gęstość sąsiedztwa
    i>>OdlSasiad;	//Rozmiar sąsiedztwa

    i>>Noise;		//Szum informacyjny
    sym2::rectangle_geometry* Geom=(sym2::rectangle_geometry*)Agenci.get_geometry();    assert(Geom != nullptr);
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

