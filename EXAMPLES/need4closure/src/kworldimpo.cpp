/// @file
/// @brief ... (old example for SymShell implementing the Kruglanski's like model)
//===========================================================================
/// @date 2026-05-18 (modified)
// implementation of the kworld class.
//	Virtual output methods 
//----------------------------------------------
// Its is rather simply implementation. You can replace it if you need.
//=====================================================================
#include "kworld.h"

int kworld::implement_output(ostream& o) const
{
    int ret=world::implement_output(o);
    if(ret!=1) return ret;
    o<<separator;
    o<<MyWidth<<separator;	//Obwód (?) torusa
    o<<MaxSila<<separator;	//Maksymalna siła agenta
    o<<IleSasiad<<separator;	//8 == Gęstość sąsiedztwa
    o<<WeightOfSelf<<separator; //Jak ma brać siebie pod uwagę
    o<<NeedForClosure<<separator; //Jak ma brać innych (waga)
    o<<Noise<<separator<<endl;		//Szum informacyjny
    o<<Agenci<<endl;
    return 1;
}

/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*                                                                  */
/*      Instytut Studiów Społecznych Uniwersytetu Warszawskiego     */
/*                                                                  */
/*        WWW:  http://www.iss.uw.edu.pl/~borkowsk                  */
/*        MAIL: borkowsk@samba.iss.uw.edu.pl                        */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/


