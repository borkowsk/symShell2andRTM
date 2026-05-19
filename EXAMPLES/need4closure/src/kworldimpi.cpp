/// @file
/// @brief
///     @EN{ Implementation of the 'kworld' class.	Virtual Input methods  }
///     @PL{  }
/// @date 2026-05-19 (modified)
/// =========================================================
/// @details
/// Old example for SymShell implementing a Kruglanski's like model)
/// Its is rather simply implementation. You can replace it if you need.
//========================================================================

#include "kworld.h"

int kworld::implement_input(istream& i)
{
    int ret=world::implement_input(i);
    if(ret!=1) return ret;
    i>>MyWidth;		//Obwód torusa
    i>>MaxSila;		//Maksymalna siła agenta
    i>>IleSasiad;	//8 == Gęstość sąsiedztwa
    i>>WeightOfSelf; //Jak ma brać siebie pod uwagę
    i>>NeedForClosure; //Jak ma brać innych (waga)
    i>>Noise;		//Szum informacyjny
    sym2::rectangle_geometry* Geom=( sym2::rectangle_geometry*)Agenci.get_geometry();
    assert(Geom!=nullptr);
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



