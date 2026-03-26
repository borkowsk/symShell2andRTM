/// @file
/// @date 2026-03-26 (modified)
/// Implementation of the lifeworld class - virtual Input method
//---------------------------------------------------------------
/// Its is rather simply implementation. You can replace it if you need.
// /////////////////////////////////////////////////////////////////////
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

/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */

