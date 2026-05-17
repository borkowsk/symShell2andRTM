// Virtual output methods implementation of the aworld class.
//Its is rather simply implementation. You can replace it if you need.
//=====================================================================
// @date 2026-05-17 (modification)
#include "aworld.h"

int aworld::implement_output(ostream& o) const
{
    int ret=world::implement_output(o);
    if(ret!=1) return ret;
    o << separator;
    o << MyWidth<<separator;	//Obwod (?) torusa
    o << MaxSila<<separator;	//Maksymalna siła agenta
    o << NofCateg << separator;	//Liczba kategori w mapach
    o << NofNeigh << separator;	//8 == Gęstość sąsiedztwa
    o << NeighRadius << separator;	//Rozmiar sąsiedztwa
    o << UseSelf<<separator;	//Czy ma brac siębie pod uwage
    o << Noise<<separator<<endl;		//Szum informacyjny
    o << Agents << endl;
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


