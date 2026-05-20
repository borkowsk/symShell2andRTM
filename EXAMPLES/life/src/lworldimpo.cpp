/// @file
/// @brief
///     @EN{ Implementation of the life_world class. Virtual output method. }
///     @PL{  }
/// @date 2026-05-21 (modified)
/// =========================================================
/// @details
/// Its is rather simply implementation. You can replace it if you need.
//======================================================================

#include "lworld.h"

int life_world::implement_output(ostream& o) const
{
    int ret=world::implement_output(o);
    if(ret!=1) return ret;
    o<<separator;
    o<<MyWidth<<separator;	    //Obwód (?) torusa

    o<<IleKate<<separator;	    //Liczba kategorii w mapach
    o<<IleSasiad<<separator;	//8 == gęstość sąsiedztwa
    o<<OdlSasiad<<separator;	//Rozmiar sąsiedztwa

    o<<Noise<<separator<<endl;	//Szum informacyjny
    o<<Agenci<<endl;
    return 1;
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




