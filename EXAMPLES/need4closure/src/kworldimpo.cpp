/// @file
/// @brief
///     @EN{ Implementation of the type 'kWorld'. Virtual output method. }
///     @PL{ Implementacja typu 'kWorld'. Metoda wirtualnego wyjścia. }
/// @date 2026-05-29 (modified)
///       ===========================================================================
/// @details
///     @EN{ Old example for SymShell implementing a Kruglanski's like model. } @PL{ .... }
///     @EN{ Its is rather simply implementation. You can replace it if you need. } @PL{ .... }
//======================================================================================================================

#include "kWorld.h"

int kWorld::implement_output(ostream& o) const
{
    int ret=world::implement_output(o);
    if(ret!=1) return ret;
    o<<separator;
    o<<MyWidth<<separator;	//Obwód (?) torusa
    o << MaxStrength << separator;	//Maksymalna siła agenta
    o << NeighDens << separator;	//8 == Gęstość sąsiedztwa
    o<<WeightOfSelf<<separator; //Jak ma brać siebie pod uwagę
    o<<NeedForClosure<<separator; //Jak ma brać innych (waga)
    o<<Noise<<separator<<endl;		//Szum informacyjny
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




