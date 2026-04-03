/// @file
/// @brief Virtual output method implementation of the aworld class. (kattitude old example for SymShell)
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @date 2026-04-03 (modified)
/// @details
///     Its is rather simply implementation. You can replace it if you need.
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "aworld.h"

int aworld::implement_output(ostream& o) const
{
    int ret=world::implement_output(o);
    if(ret!=1) return ret;
    o<<separator;
    o<<MyWidth<<separator;		//Obwód (?) torusa
    o << MaxPower << separator;	//Maksymalna sila agenta
    o<<IleKate<<separator;		//Liczba kategorii w mapach
    o << NeiDens << separator;	//Gęstość sąsiedztwa
    o << NeiSize << separator;	//Rozmiar sąsiedztwa
    o<<WeightOfSelf<<separator;	//Jak ma brać siebie pod uwagę?
    o<<NeedForClosure<<separator;	//Jak ma brać innych (waga)?
    o<<Noise<<separator<<endl;		//Szum informacyjny
    o<<Agenci<<endl;
    return 1;
}

/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */



