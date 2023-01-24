/** \file wb_pchario.hpp
 *  \brief Templates for input and output of wbrtm::wb_pchar objects
 *         ---------------------------------------------------------
 *  \author Wojciech Borkowski @ Institut for Social Studies, University of Warsaw
 *  \date   2022-10-25 (last modification)
 *  \details ...
 *  \note Like many others wbrtm files created around year 1996.
 *  \ingroup DYNMEMORY
 */
#ifndef _WB_PCHARIO_HPP_INCLUDED_
#define _WB_PCHARIO_HPP_INCLUDED_

#include <iostream>
#include <cstring>

#include "wb_ptr.hpp"
#include "ioexcep.hpp"
#include "excpmem.hpp"

#include "compatyb.hpp" // użyteczne funkcje nie wszędzie zaimplementowane


namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \brief Znaki zabronione dla zwykłego zapisu.
/// \note Można podstawić własna wersje przy linkowaniu, a nawet zmieniać w trakcie!!!
extern const char* WB_PCHAR_VERBOTEN_CHARS;//=" \t\n\r\'\"\0\01\02\03\04\05\06\07\010";

/// \brief Czyta łańcuch zamknięty w cudzysłowie lub czymś zamiast niego.
/// \return NULL, jeśli błąd - np. brak pamięci lub brak znaku zamknięcia.
template<class Caller>
char* ReadEnclosedString(istream& file, const Caller& caller, char delimiter)
{
const int MINLEN=6;
int  znak;
size_t jednostek=1; // Ile jednostek alokacji już jest
size_t licznik=0;   // Ile znaków wczytano?
                                                                                                    assert(&file!=NULL);
if(!file.good())
{
	if(caller.Raise(ExcpIO(NULL, file.tellg(), "Invalid stream")) == 1)
		return NULL;
}

eat_blanks(file);

znak=file.get();//Powinien byc delimiter
if(znak!=delimiter)
{
	if(caller.Raise(ExcpIO(NULL, file.tellg(), "First delimiter not find")) == 1)
		return NULL;
}

char* pom=new char[MINLEN+1]; // Bufor na znaki
if(pom==NULL) goto BRAK_PAMIECI;

while((znak=file.get())!=delimiter)
{
	if(znak==EOF ) //(Niespodziewany) koniec łańcucha
	   if(caller.Raise(ExcpIO(NULL, file.tellg(), "Unexpected end of file")) == 1)
		{
			delete[] pom;
			return NULL;
		}

	if(znak=='\\' && (znak=file.get())==EOF ) //Żeby móc użyć znaku "delimitera"
	   if(caller.Raise(ExcpIO(NULL, file.tellg(), "Syntax error or EOF")) == 1)
	   {
			delete[] pom;
			return NULL;
	   }

	JEST_MIEJSCE:
	if( (licznik+1)<(jednostek*MINLEN) )
    {
		pom[licznik]=znak;
		licznik++;
    }
    else
    {
		jednostek*=2;
		char* pom2=new char[jednostek*MINLEN+1];
		if(pom2==NULL) goto BRAK_PAMIECI;
		memcpy(pom2,pom,licznik);
		delete[] pom;
		pom=pom2;
	
		goto JEST_MIEJSCE;
    }
}
                                                                                                         assert(MINLEN>=1);
pom[licznik]='\0'; //Zakończenie łańcucha

return pom; //Już gotowy

BRAK_PAMIECI: //???
if(pom!=NULL)
	delete[] pom; // Żeby pamięć nie wyciekała
caller.Raise(OutOfMemoryExcp(jednostek * MINLEN + 1, __FILE__, __LINE__));
return NULL;// Może coś być juz wczytane, ale bufor zwolniono
}

/// \brief Zapisuje łańcuch w cudzysłowie lub czymś zamiast niego.
/// \return wartość !=0, jeśli jakiś błąd zamaskowany przez użytkownika.
template<class Caller>
int WriteEnclosedString(ostream& file, const char* str, const Caller& caller, char delimiter)
{
file.put(delimiter);
while(*str!='\0')
	{
	if(file.bad())
		if(caller.Raise(ExcpIO(NULL, file.tellp() , "string write") ) == 1)
			return -1;
	if(*str==delimiter || *str=='\\')
		file.put('\\');
	file.put(*str);
	str++;
	}
file.put(delimiter);
if(file.bad())
	if(caller.Raise(ExcpIO(NULL, file.tellp(), "string write")) == 1)
			return -1;
return 0;
}

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
#endif //_WB_PCHARIO_HPP_INCLUDED_


