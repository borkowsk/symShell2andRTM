// Implementacja wejścia wyjścia dla `char*`
//-----------------------------------------------------------------------------
#include <cassert>
#include <ctype.h>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "ioexcep.hpp"
#include "excpmem.hpp"
#include "iocharp.hpp"
#include "compatyb.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

int  IgnoreToEOF(istream& file,const vobject& user,char delimiter)
// Ignoruje wszystko do znaku=delimiter lub konca pliku
{
int znak;
assert(&file!=NULL);
if(file.eof())
	return EOF;

while((znak=file.get())!=delimiter)
	{
	if(znak==EOF)
		return EOF;//Koniec pliku

	if(!file.good())
		{
		user.Raise(ExcpIO(NULL,file.tellg(),"Invalid stream"));
		return EOF;
		}
	}
return znak;
}


char* ReadToEOF(istream& file,const vobject& user,char delimiter,bool NULLIfEmpty)
{
const int MINLEN=6;
int  znak;
size_t jednostek=1;// Ile jednostek alokacji juz jest
size_t licznik=0;  // Ile znakow wczytano
char* pom=new char[MINLEN+1];// Bufor na znaki
if(pom==NULL) goto BRAK_PAMIECI;

while((znak=file.get())!=EOF && znak!=delimiter)
	{
	JEST_MIEJSCE:
	if( (licznik+1)<(jednostek*MINLEN) )//+1 na konczace \0
		{
		pom[licznik]=char(znak);
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

pom[licznik]='\0';
if(::strlen(pom)==0 &&  (NULLIfEmpty || znak==EOF) )
//Zwraca pusty gdy koniec pliku i gdy pusta linia i tak chce user
	{
	delete pom;
	return NULL;
	}
	else
	return pom;

BRAK_PAMIECI:
if(pom!=NULL)
	delete pom;// Zeby pamiec nie wyciekala
user.Raise(OutOfMemoryExcp(jednostek*MINLEN+1,__FILE__,__LINE__));
return NULL;// Moze cos byc juz wczytane
}

char* ReadToSeparators(istream& file,const vobject& user,const char* delimiters,int& what_delim,int ZAPAS)
{
const int MINLEN=6;
int  znak;
size_t jednostek=1;// Ile jednostek alokacji juz jest
size_t ilealokowano=jednostek*MINLEN+1+ZAPAS;//Ile znakow faktycznie alokowano
size_t licznik=0;  // Ile znakow wczytano
char* pom=new char[ilealokowano];// Bufor na znaki
if(pom==NULL)
		goto BRAK_PAMIECI;


while((znak=file.get())!=EOF && ::strchr(delimiters,znak)==NULL )
	{
	JEST_MIEJSCE:
	if( (licznik+1) //jak jest licznik 0 to potrzebny jest jeden znak
		<
		(ilealokowano-1-ZAPAS) )//-1 na konczace \0
		{
		pom[licznik]=char(znak);
		licznik++;
		}
		else
		{
		jednostek*=2;
		ilealokowano=jednostek*MINLEN+1+ZAPAS;//Ile znakow alokowac
		char* pom2=new char[ilealokowano];
		if(pom2==NULL)
			goto BRAK_PAMIECI;
		memcpy(pom2,pom,licznik);
		delete pom;
		pom=pom2;
		goto JEST_MIEJSCE;
		}
	}

if(licznik==0)
	{
		delete pom;//Nic sie nie wczytalo, ale alokacja jest
		return NULL;
	}
	else
	{
		pom[licznik]='\0';
		assert(::strlen(pom)+1+ZAPAS<=ilealokowano);
		what_delim=znak;
		return pom;
	}

BRAK_PAMIECI:
if(pom!=NULL)
	delete pom;// Zeby pamiec nie wyciekala
user.Raise(OutOfMemoryExcp(jednostek*MINLEN+1,__FILE__,__LINE__));
return NULL;// Moze cos byc juz wczytane
}

    int WriteEnclosedString(ostream& file,const char* str,const vobject& user,char delimiter)
// Pisze lancuch w cudzyslowach lub czyms zamiast,
// zwraca !=0 jesli jakis blad zamaskowany przez user'a
    {
        file.put(delimiter);
        while(*str!='\0')
        {
            if(file.bad())
                if(user.Raise(ExcpIO(NULL,file.tellp(),"string write"))==1)
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


    char* ReadEnclosedString(istream& file,const vobject& user,char delimiter)
// Czyta łańcuch w cudzysłowie, lub czymś zamiast.
// Zwraca NULL, jeśli błąd-np brak pamięci lub brak zamknięcia
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


        eat_blanks(file); //Zjadanie odstępów wg. W.Borkowskiego

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
            if(znak==EOF) //Koniec lancucha
                if(user.Raise(ExcpIO(NULL,file.tellg(),
                                     "Unexpected end of file"))==1)
                {
                    delete pom;
                    return NULL;
                }
            if(znak=='\\' && (znak=file.get())==EOF)//Zeby moc uzyc znaku "delimitera"
                if(user.Raise(ExcpIO(NULL,file.tellg(),
                                     "Syntax error or EOF"))==1)
                {
                    delete pom;
                    return NULL;
                }
            JEST_MIEJSCE:
            if( (licznik+1)<(jednostek*MINLEN) )
            {
                pom[licznik]=char(znak);
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

} //namespace

/* *******************************************************************/
/* 			          WBRTM  version 2006-2022                       */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
