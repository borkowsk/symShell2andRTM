// Funkcje pomocnicze dla wejscia/wyjscia kodowanych lancuchow 
//---------------------------------------------------------------
#ifndef __IOCHARP_HPP__
#define __IOCHARP_HPP__

#include <iostream>
using namespace std;

#include "vobject.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

int		IgnoreToEOF(istream& file,const vobject& user,char delimiter='\n');
// Funkcja ignoruje wszystko do znaku=delimiter lub konca pliku
// Zwraca delimiter lub EOF

char*	ReadToEOF(istream& file,const vobject& user,char delimiter='\n',bool NULLIfEmpty=true);
//Czyta do konca lini, lub do konca pliku zwracajac dynamicznie alokowany lancuch lub NULL

char*	ReadToSeparators(istream& file,const vobject& user,const char* delimiters,int& what_delim,int zapas=0);
//Czyta do konca pliku jednego z zadanych separatorow zwracajac dynamicznie alokowany lancuch lub NULL, zapas to ilosc znakow ktore mozna dopisac bez realokacji

char*	ReadEnclosedString(istream& file,const vobject& user,char delimiter='"');
// Czyta lancuch w cudzyslowach lub czyms zamiast,
// zwraca NULL jesli blad-np brak pamieci lub brak zamkniecia

int		WriteEnclosedString(ostream& file,const char* str,const vobject& user,char delimiter='"');
// Zapisuje lancuch w cudzyslowie, oznaczajac znaki specjlne
// Zwraca 0 lub -1 w wypadku zamaskowanego bledu

} //namespace

/* ******************************************************************/
/*                WBRTM  version 2022 for GuestXR                   */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif