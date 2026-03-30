// implementation of the kworld class.
//	Virtual output methods 
//----------------------------------------------
//Its is rather simply implementation. You can replace it if you need.
//////////////////////////////////////////////////////////////////////
#include "kworld.h"

int kworld::implement_output(ostream& o) const
{
	int ret=world::implement_output(o);
	if(ret!=1) return ret;
	o<<separator;
	o<<MyWidth<<separator;	//Obwod (?) torusa
	o<<MaxSila<<separator;	//Maksymalna sila agenta
	o<<IleSasiad<<separator;	//8==Gestosc sasiedztwa
	o<<WeightOfSelf<<separator; //Jak ma brac siebie pod uwage
	o<<NeedForClosure<<separator;//Jak ma brac in nych (waga)	
	o<<Noise<<separator<<endl;		//Szum informacyjny
	o<<Agenci<<endl;
	return 1;
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


