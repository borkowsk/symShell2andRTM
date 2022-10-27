// wb_smartlog.cpp: implementation of the wb_smartlog class.
//
//*////////////////////////////////////////////////////////////////////

#include <cerrno>
#include <cstdlib>
#include <fstream>
#include "compatyb.hpp"

#include "wb_cpucl.hpp"
#include "wb_smartlog.hpp"

using namespace wbrtm;

//*////////////////////////////////////////////////////////////////////
// Construction/Destruction
//*////////////////////////////////////////////////////////////////////

//LOCAL_CHANGE_LOG_LEVEL( 1 ); //USE FOR CHANGE LEVEL IN BLOCK

wb_cpu_clock ApplicationClock; //Zegar czasu procesora
wb_smartlog TheApplicationLog; //Domyślny log aplikacji powiązany z clog (o ile user nie zmieni)

wb_smartlog::wb_smartlog(const char* logname):
                filename(logname),ptrToMyStream(nullptr)
{}

wb_smartlog::~wb_smartlog()
{
	if(ptrToMyStream)
		ALOG( *this , 0 , <<(filename.OK()?filename.get():"LOG")<<" CLOSED BY DESTRUCTOR"<<endl );
	Disconnect();
}

//////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////
const char*  wb_smartlog::MyName()
{
	return filename.get();
}

double wb_smartlog::GetTime()
{
	return double(ApplicationClock);
}

bool wb_smartlog::SetName(const char* logname)
{
    if(ptrToMyStream==nullptr)
	{
		assert(!filename.OK());
		filename.take(clone_str(logname));
		return true;//Udalo sie
	}
	else
	{
		return false;
	}
}


bool	wb_smartlog::Connect()
//Wiaze ze strumieniem - Otwiera log jako plik albo wiaze z clog
{
	if(filename.OK()==0)
		{
			ptrToMyStream=&clog;
		}
		else
		{
			ofstream* ptrOfstream=new ofstream(filename.get(), ios::trunc );

            assert(ptrOfstream!=nullptr);
			if(ptrOfstream->is_open() && !ptrOfstream->bad())
			{
				ptrToMyStream=ptrOfstream;
			}
			else
			{
				cerr<<filename<<':'<<strerror(errno)<<endl;
				delete ptrOfstream;
				return false;
			}
		}

		return true;
}

bool	wb_smartlog::Disconnect()
//Odwiazuje od strumienia
{
    if(ptrToMyStream==nullptr)
		return false; //Nia ma czego zamykac
	
	if(!filename.OK())
	{
        ptrToMyStream=nullptr;
	}
	else
	{
		if(ptrToMyStream) //Moze nie byc bo nie uzywany
		{
			ofstream* pom=dynamic_cast<ofstream*>(ptrToMyStream);
            assert(pom!=nullptr);
			delete pom;
            ptrToMyStream=nullptr;
		}
	}
	
	return true;
}

ostream& wb_smartlog::MyStream()
{
    if(ptrToMyStream==nullptr)
    {
		if(!Connect())
		{
            cerr<<"LOG NOT SET (nullptr)."<<endl;
			exit(-1);//NA RAZIE - PROWIZORKA
			//powinien zglosic Exception!!!
            return *(ostream*)nullptr;//Bo uzycie tego na pewno zle sie skonczy
		}
    }
    else
    {
        if(ptrToMyStream->bad())
        {
	        cerr<<"LOG IS IN BAD STATE."<<endl;
			exit(-1);//NA RAZIE - PROWIZORKA
			//powinien zglosic Exception!!!
            return *(ostream*)nullptr;//Bo uzycie tego na pewno zle sie skonczy
        }
    }

	return *ptrToMyStream;
}

int	wb_smartlog::CurrCallLevel()
//Daje aktualny poziom wywolania - ze sledzenia przez block markery
{
	return call_level;
}

int		wb_smartlog::log_level=0;
int		wb_smartlog::use_fname=1;
int		wb_smartlog::call_level=0;//Glebokosc wywolan funckji liczona przez Block_checker'y
const char*	wb_smartlog::SEPAR="\t";

#ifdef unix
const char	wb_smartlog::SLASH_FOR_LOG='/';//Co separuje nazwy  w strukturze katalogow
#else
const char	wb_smartlog::SLASH_FOR_LOG='\\';//Co separuje nazwy  w strukturze katalogow
#endif


/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/



