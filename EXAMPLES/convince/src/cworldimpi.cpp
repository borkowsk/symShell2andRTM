// implementation of the aWorld class.
//	Virtual Input methods 
//----------------------------------------------
//Its is rather simply implementation. You can replace it if you need.
//////////////////////////////////////////////////////////////////////
#include "cworld.h"

int aWorld::implement_input(istream& i)
{
	int ret=world::implement_input(i);
	if(ret!=1) return ret;
	i>>MyWidth;		//Obwod torusa
	i>>anAgent::max_sila;		//Maksymalna sila agenta
	i>>anAgent::min_sila;		//i minimalna sila agenta
	i>>anAgent::ile_kate;		//Ilosc kategori w mapach
//	i>>IleSasiad;	//8==Gestosc sasiedztwa
//	i>>OdlSasiad;	//Rozmiar sasiedztwa
//	i>>UseSelf;		//Czy ma brac siebie pod uwage
//	i>>Noise;		//Szum informacyjny
	rectangle_geometry* Geom=(rectangle_geometry*)Agenci.get_geometry();
								assert(Geom!=NULL);
	if(Geom->get_width()!=MyWidth||
	   Geom->get_height()!=MyWidth )
			Geom->set(MyWidth,MyWidth,1);
	i>>Agenci;
	return 0;
}

/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/

