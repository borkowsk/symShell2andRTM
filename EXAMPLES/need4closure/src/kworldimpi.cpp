/// @file
/// @brief
///     @EN{ Implementation of the type 'kWorld'.	Virtual Input method. }
///     @PL{ Implementacja typu „kWorld”. Metoda wirtualnego wejścia. }
/// @date 2026-05-21 (modified)
///       =========================================================
/// @details
///     @EN{ Old example for SymShell implementing a Kruglanski's like model. } @PL{ .... }
///     @EN{ Its is rather simply implementation. You can replace it if you need. } @PL{ .... }
//======================================================================================================================

#include "kWorld.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

int kWorld::implement_input(istream& i)
{
    int ret=world::implement_input(i);
    if(ret!=1) return ret;
    i>>MyWidth;		//Obwód torusa
    i >> MaxStrength;		//Maksymalna siła agenta
    i >> NeighDens;	//8 == Gęstość sąsiedztwa
    i>>WeightOfSelf; //Jak ma brać siebie pod uwagę
    i>>NeedForClosure; //Jak ma brać innych (waga)
    i>>Noise;		//Szum informacyjny
    rectangle_geometry* Geom=( rectangle_geometry*)Agenci.get_geometry();
    assert(Geom!=nullptr);
    if(Geom->get_width()!=MyWidth||
       Geom->get_height()!=MyWidth )
            Geom->set(MyWidth,MyWidth,1);
    i>>Agenci;
    return 0;
}

#pragma clang diagnostic pop
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



