/// @file
/// @brief
///  @PL{ Implementacja klasy wb_smartlog. }
///  @EN{ Implementation of the wb_smartlog class. }
/// @date 2026-05-30 (modified)
///       =====================================================================

#include <cerrno>
#include <cstdlib>
#include <fstream>
#include "compatyb.hpp"

#include "wb_cpucl.hpp"
#include "wb_smartlog.hpp"

using namespace wbrtm;

//=====================================================================
// Construction/Destruction
//=====================================================================

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

//======================================================================
// Methods
//======================================================================
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
        return true; //Udało się
    }
    else
    {
        return false;
    }
}


bool	wb_smartlog::Connect()
//Wiąże ze strumieniem — otwiera log jako plik albo wiąże z clog
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
//Odwiązuje od strumienia
{
    if(ptrToMyStream==nullptr)
        return false; //Nie ma czego zamykać

    if(!filename.OK())
    {
        ptrToMyStream=nullptr;
    }
    else
    {
        if(ptrToMyStream) //Może nie być, bo nie używany
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
            exit(-1); //NA RAZIE - PROWIZORKA
            //powinien zgłosić Exception!!!
            return *(ostream*)nullptr; //Bo użycie tego na pewno zle się skończy
        }
    }
    else
    {
        if(ptrToMyStream->bad())
        {
            cerr<<"LOG IS IN BAD STATE."<<endl;
            exit(-1); //NA RAZIE - PROWIZORKA
            //powinien zgłosić Exception!!!
            return *(ostream*)nullptr; //Bo użycie tego na pewno zle się skończy
        }
    }

    return *ptrToMyStream;
}

int	wb_smartlog::CurrCallLevel()
//Daje aktualny poziom wywołania — ze śledzenia przez block markery
{
    return call_level;
}

int		wb_smartlog::log_level=0;
int		wb_smartlog::use_fname=1;
int		wb_smartlog::call_level=0; //Głębokość wywołań funkcji liczona przez Block_checker'y
const char*	wb_smartlog::SEPAR="\t";

#ifdef unix
const char	wb_smartlog::SLASH_FOR_LOG='/'; //Co separuje nazwy  w strukturze katalogów
#else
const char	wb_smartlog::SLASH_FOR_LOG='\\'; //Co separuje nazwy  w strukturze katalogów
#endif

/* ****************************************************************** */
/*                WB RTM  version 2006/2022/2026                      */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/              */
/*        MAIL:  wborkowski@uw.edu.pl                                 */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */



