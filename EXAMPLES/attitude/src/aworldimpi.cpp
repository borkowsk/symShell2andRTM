// Virtual Input methods  implementation of the aworld class.
// Its is rather simply implementation. You can replace it if you need.
// ////////////////////////////////////////////////////////////////////
// @date 2026-05-07 (modification)

#include "aworld.h"

int aworld::implement_input(istream& i)
{
    int ret=symshell2::world::implement_input(i);
    if(ret!=1) return ret;
    i >> MyWidth;		//Obwod torusa
    i >> MaxSila;		//Maksymalna siła agenta
    i >> NofCateg;		//Liczba kategori w mapach
    i >> NofNeigh;	//8 == Gęstość sąsiedztwa
    i >> NeighRadius;	//Rozmiar sąsiedztwa
    i >> UseSelf;		//Czy ma brac siębie pod uwage
    i >> Noise;		//Szum informacyjny
    symshell2::rectangle_geometry* Geom=(symshell2::rectangle_geometry*)Agents.get_geometry();
    assert(Geom!=NULL);
    if(Geom->get_width()!=MyWidth||
       Geom->get_height()!=MyWidth )
            Geom->set(MyWidth,MyWidth,1);
    i >> Agents;
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


