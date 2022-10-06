//	Szablony  do wprowadzania i wyprowadzania stringu
// ///////////////////////////////////////////////////////////////////////
#ifndef _WB_PCHARIO_HPP_
#define _WB_PCHARIO_HPP_

#include "platform.hpp"

#include <iostream>
#include <cstring>

#include "ioexcep.hpp"
#include "excpmem.hpp"

#include "compatyb.hpp" // użyteczne funkcje nie wszędzie zaimplementowane

//W szczególności:
//int eat_blanks(istream& file);//OGÓLNE i ZAMIAST file.eatwhite() z MSVC++

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

//Znaki zabronione dla zwyklego zapisu. Mozna podstawic wlasna wersje przy linkowaniu, 
//   albo nawet zmieniac w trakcie!!! 
extern const char* WB_PCHAR_VERBOTEN_CHARS;//=" \t\n\r\'\"\0\01\02\03\04\05\06\07\010";

template<class Caller>
char* ReadEnclosedString(istream& file,const Caller& user,char delimiter)
// Czyta lancuch w cudzyslowach lub czyms zamiast,
// zwraca NULL jesli blad-np brak pamieci lub brak zamkniecia
{
const int MINLEN=6;
int  znak;
size_t jednostek=1;// Ile jednostek alokacji juz jest
size_t licznik=0;  // Ile znakow wczytano
                                                                                assert(&file!=NULL);
if(!file.good())
	{
	if(user.Raise(ExcpIO(NULL,file.tellg(),"Invalid stream"))==1)
		return NULL;
	}

wbrtm::eat_blanks(file);

znak=file.get();//Powinien byc delimiter
if(znak!=delimiter)
	{
	if(user.Raise(ExcpIO(NULL,file.tellg(),"First delimiter not find"))==1)
		return NULL;
	}

char* pom=new char[MINLEN+1];// Bufor na znaki
if(pom==NULL) goto BRAK_PAMIECI;

while((znak=file.get())!=delimiter)
	{
	if(znak==cin.eof()) //Koniec lancucha
	   if(user.Raise(ExcpIO(NULL,file.tellg(),
						"Unexpected end of file"))==1)
		{
		delete pom;
		return NULL;
		}
	if(znak=='\\' && (znak=file.get())==cin.eof())//Zeby moc uzyc znaku "delimitera"
	   if(user.Raise(ExcpIO(NULL,file.tellg(),
						"Syntax error or EOF"))==1)
		{
		delete pom;
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
		delete pom;
		pom=pom2;
	
		goto JEST_MIEJSCE;
		}
	}

assert(MINLEN>=1);
pom[licznik]='\0';//Zakonczenie lancucha

return pom;//Juz gotowy

BRAK_PAMIECI:
if(pom!=NULL)
	delete pom;// Zeby pamiec nie wyciekala
user.Raise(OutOfMemoryExcp(jednostek*MINLEN+1,__FILE__,__LINE__));
return NULL;// Moze cos byc juz wczytane, ale bufor zwolniono
}

template<class Caller>
int WriteEnclosedString(ostream& file,const char* str,const Caller& user,char delimiter)
// Pisze lancuch w cudzyslowach lub czyms zamiast,
// zwraca !=0 jesli jakis blad zamaskowany przez user'a
{
file.put(delimiter);
while(*str!='\0')
	{
	if(file.bad())
		if(user.Raise( ExcpIO( NULL, file.tellp() ,"string write") )==1)
			return -1;
	if(*str==delimiter || *str=='\\')
		file.put('\\');
	file.put(*str);
	str++;
	}
file.put(delimiter);
if(file.bad())
	if(user.Raise(ExcpIO(NULL,file.tellp(),"string write"))==1)
			return -1;
return 0;
}
} //namespace

/* *******************************************************************/
/*			           WBRTM  version 2022                           */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*         WWW:  ...                                                 */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
#endif


