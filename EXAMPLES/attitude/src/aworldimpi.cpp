/// @file
/// @brief
///  @EN{ Virtual Input method implementation of the aworld class. }
///  @PL{ Implementacja metody wirtualnego wejścia klasy świata. }
/// @date 2026-05-21 (modified)
/// =========================================================
/// @details
///     Its is rather simply implementation. You can replace it if you need.
//======================================================================================================================

#include "aworld.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

int attitude_world::implement_input(istream& i)
{
    int ret=world::implement_input(i);
    if(ret!=1) return ret;
    i >> MyWidth;		//Obwód torusa
    i >> MaxSila;		//Maksymalna siła agenta
    i >> NofCategories;		//Liczba kategorii w mapach
    i >> NofNeigh;	//8 == Gęstość sąsiedztwa
    i >> NeighRadius;	//Rozmiar sąsiedztwa
    i >> UseSelf;		//Czy ma brać siębie pod uwagę
    i >> Noise;		//Szum informacyjny
    rectangle_geometry* Geom=(rectangle_geometry*)Agents.get_geometry();
    assert(Geom!=NULL);
    if(Geom->get_width()!=MyWidth||
       Geom->get_height()!=MyWidth )
            Geom->set(MyWidth,MyWidth,1);
    i >> Agents;
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



