// implementation of the kworld class.
//	Virtual Input methods 
//----------------------------------------------
//Its is rather simply implementation. You can replace it if you need.
//////////////////////////////////////////////////////////////////////
#include "kworld.h"

int kworld::implement_input(istream& i)
{
	int ret=world::implement_input(i);
	if(ret!=1) return ret;
	i>>MyWidth;		//Obwod torusa
	i>>MaxSila;		//Maksymalna sila agenta
	i>>IleSasiad;	//8==Gestosc sasiedztwa
	i>>WeightOfSelf; //Jak ma brac siebie pod uwage
	i>>NeedForClosure;//Jak ma brac in nych (waga)		
	i>>Noise;		//Szum informacyjny
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
/*                                                                  */
/*      Instytut Studiow Spolecznych Uniwersytetu Warszawskiego     */
/*                                                                  */
/*        WWW:  http://www.iss.uw.edu.pl/~borkowsk                  */
/*        MAIL: borkowsk@samba.iss.uw.edu.pl                        */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/

