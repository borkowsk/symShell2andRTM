/// @file
/// @brief
///  @EN{ Virtual Input method implementation of the aworld class. }
///  @PL{  }
/// @date 2026-05-19 (modified)
///       =========================================================
/// @details (kattitude old example for SymShell)
///     Its is rather simply implementation. You can replace it if you need.
//======================================================================================================================

#include "aworld.h"

int aworld::implement_input(istream& i)
{
    int ret=world::implement_input(i);
    if(ret!=1) return ret;
    i>>MyWidth;			//Obwód torusa
    i >> MaxPower;		//Maksymalna siła agenta
    i>>IleKate;			//Liczba kategorii w mapach
    i >> NeiDens;		//Gęstość sąsiedztwa
    i >> NeiSize;		//Rozmiar sąsiedztwa
    i>>WeightOfSelf;	//Jak ma brać siebie pod uwagę
    i>>NeedForClosure;	//Jak ma brać innych (waga)
    i>>Noise;			//Szum informacyjny
    sym2::rectangle_geometry* Geom=(sym2::rectangle_geometry*)Agenci.get_geometry();                          assert(Geom != nullptr);
    if(Geom->get_width()!=MyWidth||
       Geom->get_height()!=MyWidth )
            Geom->set(MyWidth,MyWidth,1);
    i>>Agenci;

    return 0;
}

/* ****************************************************************** */
/*         SYMSHELL2 EXAMPLES version 2006/2022/2026                  */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */


