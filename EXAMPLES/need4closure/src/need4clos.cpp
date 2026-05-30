/// @file
/// @brief
///     @EN{ Old example for SymShell implementing the Kruglanski's like model. }
///     @PL{ Stary przykład SymShell-a implementujący model podobnego do modelu Kruglanski-ego. }
/// @date 2026-05-30 (modified)
///       =======================================================================================
/// @details
///  Symulacja Need for closure wg teorii Arie Kruglanski-ego
///  Uzyskana z przerobienia programu ATTITUDE-s.
//====================================================================================
const char* WINDOW_HEADER="NEED FOR CLOSURE version 0.22b";
const char* SIMULATION_NAME="need-4-clos_v0.22b";

/// HISTORY:
/// ---------------- Attitude -v-v-v-v-v-v-v-v- ---------------------------------
/// v. 0.6 - dodanie losowania nieproporcjonalnego, znaczne zmiany w statystykach, zmiany z bazowej bibliotece
/// v. 0.61 - użycie innego histogramu

/// ---------------- need4clos --v-v-v-v-v-v-v- ---------------------------------
/// v. 0.21 - uzupełnienie o menu i wywoływanie strony autorskiej.
/// v. 0.22 - poprawienie kodu ze względu na konsekwencje użycia symbolu FULL typu unsigned long.
///           Rekompilacja z poprawionym symshell-em.
/// v. 0.22b - "Doxygenizacja" komentarzy.

#include <cstdlib>
#include <iostream>
#include "toitoutoll.hpp"
#include "compatyb.h"

#include "krand.h"
#include "kWorld.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err34-c"

using namespace sym2;
using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

/// @name @PL{ Rozmiary użytkowej przestrzeni okna i bok świata. } @EN{ Dimensions of usable window space and side of the world. }
/// @{
int       SCR_WIDTH=750;
int      SCR_HEIGHT=550;
unsigned WorldWidth=100;
/// @}

/// @name @PL{ Parametry Modelu. } @EN{ Model Parameters. }
/// @{
double Fill=1;				///< @brief @PL{ 0.001 do 1 - początkowe wypełnienie przestrzeni agentów (wykonywane przez losowe usuwanie!!!). }
                            ///<         @EN{ 0.001 to 1 - initial filling of agent space (performed by random removal!!!). }
double Majority=0.050;		///< @brief @PL{ Jaka część społeczeństwa będzie początkowo wyznawać "czarny" pogląd. } @EN{ What proportion of society will initially hold the "black" view? }
double Minority=0.025;		///< @brief @PL{ Jaka część społeczeństwa będzie początkowo wyznawać "biały" pogląd. } @EN{ What proportion of society will initially hold the "white" view? }
double SelfWeight=9;		///< @brief @PL{ Waga własnej opinii. Zebrane opinie ważą 9:1. } @EN{ The weight of your own opinion. Collected opinions weigh 9:1 }
double NeedForClosure=1;	///< @brief @PL{ Intensywność poszukiwań i potęga siły(?). } @EN{ The intensity of the search and the power of the force(?). }
double MigrationProb=0;		///< @brief @PL{ Prawdopodobieństwo migracji, jeśli presja otoczenia wskazuje na zmianę poglądów. }
                            ///<         @EN{ Likelihood of migration if peer pressure indicates a change of mind. }
short  HowManyToDraw=1;		///< @brief @PL{ Ilu wybierać losowo do rozmowy. } @EN{ How many to choose randomly for conversation. }
short  MaximalPower=100;	///< @brief @PL{ Największa siła. } @EN{ The greatest strength. }
int    NoisePercent=0;		///< @brief @PL{ Szum informacyjny przy zbieraniu danych. } @EN{ Information noise when collecting data. }
short  Threshold=100;		///< @brief @PL{ Wobec jakiego wpływu zmiana "poglądu" się fiksuje. } @EN{ Against what influence does the change of "view" become fixed. }
double MutationProb=0;		///< @brief @PL{ Prawdopodobieństwo spontanicznej zmiany. } @EN{ Likelihood of spontaneous change. }
bool   AsyncUpdate=false;	///< @brief @PL{ Jak `false` to synchroniczna? } @EN{ `false` means sync? }
/// @}

/// @name @PL{ Parametry sterujące obliczaniem statystyk. } @EN{ Parameters controlling the calculation of statistics. }
/// @details @PL{ Nieobiektowo przekazywane do metody inicjalizacji źródeł. } @EN{ Non-objectively passed to the source initialization method. }
/// @{
unsigned internal_log=7000;				///< @brief @PL{ Długość wewnętrznych logów statystyk. } @EN{ Length of internal statistics logs. }
unsigned spatial_correlation_mode=50;	///<  @brief @PL{ Gęstość próbkowania korelacji przestrzennej. } @EN{ Spatial correlation sampling density. }
/// @}

/// @name @PL{ Parametry aplikacji — techniczna obsługa symulacji. } @EN{ Application parameters — technical support for simulation. }
/// @{
char LogName[512]="need4clos.log\0-------------------+--";
char HistName[512]="\0--+---------need4clos.otx----------";
char MapLName[512]="\0--+---------need4clos.gif----------";
char MapPName[512]="\0--+---------powers.gif------------";
char MaskName[512]="\0--+---------mask.gif--------------";

unsigned iMaxIterations=0xffffffff;
unsigned iLogRatio=1;
unsigned iViewRatio=1;

int  AUTO_END=0;
int  Replay=0;
int	 AUTO_START=0;
/// @}

/// @brief @PL{ Analiza parametrów wywołania. } @EN{ Analysis of call parameters. }
int parse_options(const int argc,const char* argv[])
{
for(int i=1;i<argc;i++)
    {
    if( *argv[i]=='-' ) /* Opcja X11 lub symshell-a */
        continue;
    //Uppercasing
    wb_pchar hand(clone_str(argv[i]));
    char*    rob=hand.get_ptr_val();
    char*    pom=strchr(rob,'=');
    if(pom==nullptr)
            goto ERROR; //NA PEWNO ZLE
    *pom='\0';
    strupr(rob);
    *pom='=';
    if((pom=strstr(rob,"SPCH="))!=nullptr) //Nie nullptr, czyli jest
    {
        MutationProb=atof(pom + 5);
    if(MutationProb < 0 || MutationProb > 100)
        {
        cerr << "Bad SPCH =" << MutationProb << " (must be in <0,100>)" << endl;
        return 0;
        }
    cerr << "SPCH (spn. change percent) = " << MutationProb << endl;
        MutationProb/=100; //Ułamek, a nie procent tak naprawdę
    }
    else
    if((pom=strstr(rob,"FILL="))!=nullptr) //Nie nullptr, czyli jest
    {
        Fill=atof(pom+5);
        if(Fill<0 || Fill>100)
        {
            cerr<<"Bad FILL = "<<Fill<<" (must be in range <0,1> or from 1.x% to 100%)"<<endl;
            return 0;
        }
        if(Fill>1)
        {
            cerr<<"FILL (fill percent) = "<<Fill<<endl;
            Fill/=100; //Ułamek, a nie procent tak naprawdę
        }
        else
            cerr<<"FILL = "<<Fill<<endl;
    }
    else
    if((pom=strstr(rob,"ILEF="))!=nullptr) //Nie nullptr, czyli jest
    {
        Majority=atof(pom+5);
        if(Majority<0 || Majority>100)
        {
            cerr<<"Bad ILEF = "<<Majority<<" (must be in range <0,1> or from 1.x% to 100%)"<<endl;
            return 0;
        }
        if(Majority>1)
        {
            cerr<<"ILEF (\"left\" percent) = "<<Majority<<endl;
            Majority/=100; //Ułamek, a nie procent tak naprawdę
        }
        else
            cerr<<"ILEF = "<<Majority<<endl;
    }
    else
    if((pom=strstr(rob,"IRIG="))!=nullptr) //Nie nullptr, czyli jest
    {
        Minority=atof(pom+5);
        if(Minority<0 || Minority>100)
        {
            cerr<<"Bad IRIG = "<<Minority<<" (must be in range <0,1> or from 1.x% to 100%)"<<endl;
            return 0;
        }
        if(Minority>1)
        {
            cerr<<"IRIG (\"right\" percent) = "<<Minority<<endl;
            Minority/=100; //Ułamek, a nie procent tak naprawdę
        }
        else
            cerr<<"IRIG = "<<Minority<<endl;
    }
    else
    if((pom=strstr(rob,"PMIG="))!=nullptr) //Nie nullptr, czyli jest
    {
        MigrationProb=atof(pom + 5);
        if(MigrationProb < 0 || MigrationProb > 100)
        {
            cerr<<"Bad PMIG = "<<Fill<<" (must be in range <0,1> or from 1.x% to 100%)"<<endl;
            return 0;
        }
        if(MigrationProb > 1)
        {
            cerr << "PMIG (probability of migration) = " << MigrationProb << '%' << endl;
            MigrationProb/=100; //Ułamek, a nie procent tak naprawdę
        }
        else
            cerr << "PMIG (probability of migration) = " << MigrationProb << endl;
    }
    else
    if((pom=strstr(rob,"NOIP="))!=nullptr) //Nie nullptr, czyli jest
    {
        NoisePercent=toi(atol(pom + 5) );
    if(NoisePercent < 0 || NoisePercent > 100)
        {
        cerr << "Bad NOIP =" << NoisePercent << " (must be in <0,100>)" << endl;
        return 0;
        }
    cerr << "NOIP (noise percent) = " << NoisePercent << endl;
    }
    else
    if((pom=strstr(rob,"MPOW="))!=nullptr) //Nie nullptr, czyli jest
    {
        MaximalPower=tos(atoi(pom + 5));
        if(MaximalPower < 0) //0 czy 1???
            {
            cerr << "Bad MPOW = " << MaximalPower << " (must be >=1 )" << endl;
            return 0;
            }
    }
    else
    if((pom=strstr(rob,"WIDTH="))!=nullptr) //Nie nullptr, czyli jest
    {
        WorldWidth=atol(pom + 6);
    if(WorldWidth < 3 || WorldWidth >= SCR_WIDTH)
        {
        cerr << "Bad WIDTH = " << WorldWidth << "(must be in <3," << SCR_WIDTH << ">" << endl;
        return 0;
        }
    }
    else
    if((pom=strstr(rob,"WIDTHWIN="))!=nullptr) //Nie nullptr, czyli jest
    {
        SCR_WIDTH=atoi(pom + 9);
        if(SCR_WIDTH < 50)
            {
            cerr << "Bad WIDTHWIN = " << SCR_WIDTH << " (must be >50)" << endl;
            return 0;
            }
    }
    else
    if((pom=strstr(rob,"HEIGHTWIN="))!=nullptr) //Nie nullptr, czyli jest
    {
        SCR_HEIGHT=atoi(pom + 10);
        if(SCR_HEIGHT < 50)
            {
            cerr << "Bad HEIGHTWIN = " << SCR_HEIGHT << " (must be >50)" << endl;
            return 0;
            }
    }
    else
    if((pom=strstr(rob,"MAX="))!=nullptr) //Nie nullptr, czyli jest
    {
    iMaxIterations=atol(pom+4);
    if(iMaxIterations<=0)
        {
        cerr<<"Bad MAX iterations. Must be >0"<<endl;
        return 0;
        }    
    else
        {
            internal_log=iMaxIterations+1;
        }
    }
    else
    if((pom=strstr(rob,"LOGC="))!=nullptr) //Nie nullptr, czyli jest
    {
    iLogRatio=atol(pom+5);
    if(iLogRatio<=0)
        {
        cerr<<"Bad LOGC (write to log frequency). Must be >0"<<endl;
        return 0;
        }
    }
    else
    if((pom=strstr(rob,"VIEW="))!=nullptr) //Nie nullptr, czyli jest
    {
    iViewRatio=atol(pom+5);
    if(iViewRatio<=0)
        {
        cerr<<"Bad VIEW (visualization frequency). Must be >0"<<endl;
        return 0;
        }
    }    
    else
    if((pom=strstr(rob,"SELF="))!=nullptr) //Nie nullptr, czyli jest
    {
        SelfWeight=atof(pom + 5);
    cerr << "SELF = " << SelfWeight << endl;
    }
    else
    if((pom=strstr(rob,"NFOC="))!=nullptr) //Nie nullptr, czyli jest
    {
        NeedForClosure=atof(pom + 5);
    cerr << "NFOC = " << NeedForClosure << endl;
    }
    else		
    if((pom=strstr(rob,"PRTR="))!=nullptr) //Nie nullptr, czyli jest
        {
            HowManyToDraw=tos(atoi(pom + 5));
            if(HowManyToDraw == -1)
            {
                cerr<<"PRTR = all"<<endl;
            }
            else
                if(HowManyToDraw > 1)
                {
                    cerr << "PRTR=" << HowManyToDraw << endl;
                }
                else
                {
                    cerr << "Bad PRTR=" << HowManyToDraw << endl;
                    return 0;
                }
        }
    else
    if((pom=strstr(rob,"AUTO="))!=nullptr) //Nie nullptr, czyli jest
    {
        AUTO_START=atoi(pom + 5);
        cerr << "AUTO=" << AUTO_START << endl;
        if(AUTO_START)
            {
                AUTO_END=1;
            cerr<<"STOP = Yes"<<endl;
            }
    }
    else
    if((pom=strstr(rob,"STOP="))!=nullptr) //Nie nullptr, czyli jest
    {
        AUTO_END=(toupper(pom[5]) == 'Y');
    cerr << "STOP = " << (AUTO_END?"Yes":"No") << endl;
    }
    else  //SYNC
    if((pom=strstr(rob,"SYNC="))!=nullptr) //Nie nullptr, czyli jest
    {
        AsyncUpdate=(toupper(pom[5]) != 'Y' );
    cerr << "SYNC = " << (AsyncUpdate == 0?"Yes":"No") << endl;
    }
    else
    if((pom=strstr(rob,"ILOG="))!=nullptr) //Nie nullptr, czyli jest
    {
    internal_log=atoi(pom+5);
    if(internal_log<50)
            {
            internal_log=50;
            cerr<<"An internal log to short. Reset to the default minimum ="<<internal_log<<endl;
            }
    }
    else
    //cerr<<"\tRSPC=N/Y or 1..WIDTH - Random calculation of spatial correlation ("<<(spatial_correlation_mode==0?"N":"Y")<<")\n";
    if((pom=strstr(rob,"RSPC="))!=nullptr)
    {
        const char* lpom=pom+5;
        if(toupper(*lpom)=='N')
            spatial_correlation_mode=0;
        else if(toupper(*lpom)=='Y')
            spatial_correlation_mode=16;
        else
            spatial_correlation_mode=atoi(lpom);

        cerr<<"Random calculation of spatial correlation is "
            <<(spatial_correlation_mode==0?"d i s a b l e d":"e n a b l e d")<<". Multiplication="<<spatial_correlation_mode<<"\n";
    }
    else
    if((pom=strstr(rob,"LOGF="))!=nullptr) //Nie nullptr, czyli jest
    {
    strcpy(LogName,pom+5);
    }
    else
    if((pom=strstr(rob,"MAPL="))!=nullptr) //Nie nullptr, czyli jest
    {
    strcpy(MapLName,pom+5);
    cerr<<"Map of attitudes is from file \""<<MapLName<<"\"\n";
    }
    else
    if((pom=strstr(rob,"MAPP="))!=nullptr) //Nie nullptr, czyli jest
    {
    strcpy(MapPName,pom+5);
    cerr<<"Map of individual power is from file \""<<MapPName<<"\"\n";
    }
    else
    if((pom=strstr(rob,"MASK="))!=nullptr) //Nie nullptr, czyli jest
    {
    strcpy(MaskName,pom+5);
    cerr<<"Mask for live agents is from file \""<<MaskName<<"\"\n";
    }	
    else
    if((pom=strstr(rob,"HIST="))!=nullptr) //Nie nullptr, czyli jest
    {
    strcpy(HistName,pom+5);
    cerr<<"History of the simulation will be saved to \""<<HistName<<"\"\n";
    }
    else
    if((pom=strstr(rob,"REPL="))!=nullptr) //Nie nullptr, czyli jest
    {
    strcpy(HistName,pom+5);
    Replay=1;
    cerr<<"The simulation will be replayed from \""<<HistName<<"\"\n";
    }
    else
    /* Ostatecznie wychodzi, że nie ma takiej opcji */
    {
ERROR:
        cout<<"Unknown parameter \""<<argv[i]<<"\"\n";
        cout<<"YOU CAN USE:\n";
        cout<<"\tREPL=hist.otx - not simulate, but replay simulation history file.\n";
    //	cout<<"\tMAPL=initL.gif (or BMP)- file with initialization map of attitudes (RANDOM)\n";
        cout<<"\tMAPP=initP.gif (or BMP)- file with an initialization map of powers (RANDOM)\n";
        cout<<"\tMASK=mask.gif	(or BMP)- mask file for alive (not black) agents (ALL ALIVE)\n";

    //	cout<<"\tSYNC=Y/N - synchronic (Y) or Monte-Carlo simulation mode ("<<(AsyncUpdate==0?"Yes":"No")<<")\n";
        cout << "\tNFOC=0...inf - need for closure [meaning depend on a model](" << NeedForClosure << ")\n";
        cout << "\tSELF=0...1 - use self for calculations (" << (SelfWeight) << ")\n";

        cout << "\tWIDTH=NN - matrix size (" << WorldWidth << ")\n";
        cout<<"\tFILL=0.001..1 - ratio of live agents ("<<Fill<<")\n";	
        cout<<"\tILEF=0.001..0.999 - part of society with \"black\" attitude ("<<Majority<<")\n"; 
        cout<<"\tIRIG=0.001..0.999 - part of society with \"white\" attitude ("<<Minority<<")\n"; 
//      c out<<"\tPMIG=0.001..1 - probability of migration under pressure ("<<MigrationProb<<")\n";
        cout << "\tMPOW=NN - max strength for initialization (" << MaximalPower << ")\n"	;
        cout << "\tTRSP=N - Threshold of strength (" << Threshold << ")\n";

        cout << "\tPRTR=1...N - number of interaction partners (-1 = all in a neighborhood) (" << HowManyToDraw << ")\n";
        cout << "\tNOIP=NN - percent of noise (" << NoisePercent << ")\n";
//		c out<<"\tSPCH=NN - percent of spontaneity changes of attitudes ("<<MutationProb*100<<")\n";

        cout<<"\tMAX=NNNN - max simulation step ("<<iMaxIterations<<")\n";
        cout<<"\tRSPC=N/Y or 1...WIDTH - Random calculation of spatial correlation ("<<(spatial_correlation_mode?"N":"Y")<<")\n";
        cout<<"\tILOG=NNNN - length of internal statistic logs ("<<internal_log<<")\n";
        cout << "\tSTOP=N/Y - exit after MAX steps (" << (AUTO_END?"Yes":"No") << ")\n";
        cout<<"\tVIEV=N - visualisation frequency ("<<iViewRatio<<")\n";
        cout<<"\tLOGC=N - log file saving frequency ("<<iLogRatio<<")\n";
        cout<<"\tLOGF=name.log - file for simulation log ("<<LogName<<")\n";
//		c out<<"\tHIST=hist.otx - file for full history of simulation.\n";
        cout << "\tWIDTHWIN=YYY,HEIGHTWIN=XXX - initial window size.(" << SCR_WIDTH << 'x' << SCR_HEIGHT << "\n";
        cout << "\nAUTO=XXX - number of auto-repetition of simulation.(" << AUTO_START << ")\n";
        cout<<flush;
    return 0;
    }
  }
return 1;
}


/** @brief @PL{ OGÓLNA FUNKCJA MAIN. } @EN{ GENERAL MAIN FUNCTION. }   */
/* ************************************************************************ */

int main(const int argc,const char* argv[])
{
cout<<endl<<WINDOW_HEADER<<", compilation: "<<__DATE__<<' '<<__TIME__<<endl;
cout<<"Programmed by W. Borkowski as implementation of A. Kruglanski theory"<<endl;
cout<<"======================================================================="<<endl;
cout.flush();

if(!parse_options(argc,argv))
        exit(1);

main_area_manager Lufciki(24, SCR_WIDTH, SCR_HEIGHT, 28);
if(!Lufciki.start(WINDOW_HEADER,argc,argv,1))
    {
    cerr<<"Can't initialize graphics"<<endl;
    exit(1);
    }

//Utworzenie sensownej nazwy pliku(-ów) do zrzutów ekranu
{
wb_pchar buf(strlen(SIMULATION_NAME) + 20);
buf.prn("%s_%ld", SIMULATION_NAME, time(nullptr));
Lufciki.set_dump_name(buf.get());
}


/// @internal @PL{ PRZYGOTOWANIE ŚWIATA SYMULACJI } @EN{ PREPARATION OF THE SIMULATION WORLD }
kWorld& theWorld=*new kWorld(WorldWidth,
                             LogName,
                             MapLName,
                             MapPName,
                             MaskName,
                             NoisePercent / 100.0, //Szum od 0-1
                             MaximalPower, //Żeby była w przedziale
                             HowManyToDraw,
                             SelfWeight,
                             NeedForClosure,
                             AsyncUpdate == 0, //Synchroniczna czy nie?
                             Threshold,
                             MutationProb,
                             Fill,
                             MigrationProb,
                             Majority,
                             Minority
                           );

/// Od C++11 new nie powinno zwrócić "nullptr" tylko wylecieć z exception!
// if(&theWorld==nullptr)
//     {
//     cerr<<"Can't allocate simulation world!\n"<<endl;
//     exit(1);
//     }

//INICJALIZACJA:
RANDOMIZE() //inicjalizacja globalnego randomizer-a
theWorld.set_max_iteration(iMaxIterations); //Ile najwięcej kroków
theWorld.set_input_ratio(iViewRatio);
theWorld.set_log_ratio(iLogRatio);
cout<<WINDOW_HEADER<<": LOADED."<<endl;
theWorld.set_history_stream(HistName);

if(Replay)
{
    theWorld.initialize(&Lufciki, 1); //inicjalizacja wizualizacji
    cout<<WINDOW_HEADER<<": PREPARED FOR READING. WAITING!"<<endl;
    Lufciki.process_input(); //Pierwsze zdarzenia. Kończą się po ctrl-B
    theWorld.read_loop(AUTO_END);
}
else
{
    // REAL INITIALIZATION IS HERE!!!
    theWorld.initialize(&Lufciki); //inicjalizacja wizualizacji i warstw symulacji
    cout<<WINDOW_HEADER<<": INITIALISED."<<endl;

    if(!AUTO_START)
    {
        Lufciki.process_input(); //Pierwsze zdarzenia. Kończą się po ctrl-B
        //GŁÓWNA PĘTLA SYMULACJI
        cout<<WINDOW_HEADER<<": STARTED."<<endl;
        theWorld.simulation_loop(AUTO_END);
    }
    else
    {
        int statusWin=Lufciki.search("STATUS");
        Lufciki.maximize(statusWin);
        for(int symulacja=0; symulacja < AUTO_START; symulacja++)
            {
            //GŁÓWNA PĘTLA SYMULACJI
            cout<<WINDOW_HEADER<<": SIMULATION "<<symulacja<<" STARTED."<<endl;
            theWorld.simulation_loop(1);
            cout<<WINDOW_HEADER<<": SIMULATION "<<symulacja<<" DONE."<<endl;
            if(symulacja < AUTO_START - 1)
                {
                //Reinicjalizacja:
                theWorld.restart();
                }
            }
    }

}

cout<<WINDOW_HEADER<<": CLOSING."<<endl;

cout.flush();

delete &theWorld; //Dealokacja świata wraz ze wszystkimi składowymi
cout<<"----------> See you later!!! <--------------\n"<<endl<<flush;
return 0;
}

#pragma clang diagnostic pop
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



