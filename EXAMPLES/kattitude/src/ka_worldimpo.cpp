/// @file
/// @brief
///  @EN{ Virtual output method implementation of the ka_world class. }
///  @PL{ Implementacja metody wirtualnego wyjścia klasy ka_world. }
/// @date 2026-05-29 (modified)
///       ============================================================
/// @details (kattitude old example for SymShell)
///     Its is rather simply implementation. You can replace it if you need.
//======================================================================================================================

#include "ka_world.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

int ka_world::implement_output(ostream& o) const
{
    int ret=world::implement_output(o);
    if(ret!=1) return ret;
    o <<separator;
    o <<MyWidth  << separator;		//Obwód (?) torusa
    o <<MaxPower << separator;		//Maksymalna siła agenta
    o <<IleKate  << separator;		//Liczba kategorii w mapach
    o <<NeiDens  << separator;		//Gęstość sąsiedztwa
    o <<NeiSize  << separator;		//Rozmiar sąsiedztwa
    o <<WeightOfSelf  << separator;	//Jak ma brać siebie pod uwagę?
    o <<NeedForClosure<< separator;	//Jak ma brać innych (waga)?
    o <<Noise  << separator<<endl;	//Szum informacyjny
    o << Agents << endl;
    return 1;
}

#pragma clang diagnostic pop
/* ****************************************************************** */
/*         SYMSHELL2 EXAMPLES version 2006/2022/2026                  */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/              */
/*        MAIL:  wborkowski@uw.edu.pl                                 */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
