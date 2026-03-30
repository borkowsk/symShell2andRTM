// implementation of the aWorld class.
//	Virtual output methods 
//----------------------------------------------
//Its is rather simply implementation. You can replace it if you need.
//////////////////////////////////////////////////////////////////////
#include "cworld.h"

int aWorld::implement_output(ostream& o) const
{
	int ret=world::implement_output(o);
	if(ret!=1) return ret;
	o<<separator;
	o<<MyWidth<<separator;	//Obwod (?) torusa
	o<<anAgent::max_sila<<separator;	//Maksymalna sila agenta
	o<<anAgent::min_sila<<separator;
	o<<anAgent::ile_kate<<separator;	//Ilosc kategori w mapach
//	o<<IleSasiad<<separator;	//8==Gestosc sasiedztwa
//	o<<OdlSasiad<<separator;	//Rozmiar sasiedztwa
//	o<<UseSelf<<separator;	//Czy ma brac siebie pod uwage
//	o<<Noise<<separator<<endl;		//Szum informacyjny
	o<<Agenci<<endl;
	return 1;
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


