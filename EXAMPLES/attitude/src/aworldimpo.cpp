/// @file
/// @brief
///  @EN{ Virtual output method implementation of the 'aworld' class. }
///  @PL{ Implementacja wirtualnej metody wyjścia klasy 'aworld'. }
/// @date 2026-05-20 (modified)
/// ====================================================================
/// @details
///     Its is rather simply implementation. You can replace it if you need.
//======================================================================================================================

#include "aworld.h"

int aworld::implement_output(ostream& o) const
{
    int ret=world::implement_output(o);
    if(ret!=1) return ret;
    o << separator;
    o << MyWidth<<separator;			//Obwód (?) torusa
    o << MaxSila<<separator;			//Maksymalna siła agenta
    o << NofCategories << separator;	//Liczba kategorii w mapach
    o << NofNeigh << separator;			//8 == Gęstość sąsiedztwa
    o << NeighRadius << separator;		//Rozmiar sąsiedztwa
    o << UseSelf<<separator;			//Czy ma brać siębie pod uwagę
    o << Noise<<separator<<endl;		//Szum informacyjny
    o << Agents << endl;
    return 1;
}

/* ****************************************************************** */
/*        SYMSHELL2 EXAMPLE  version 2006/2022/2026                   */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/              */
/*        MAIL:  wborkowski@uw.edu.pl                                 */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */

