/// @file
/// @brief
///  @EN{ A fairly simple simulation of a change of opinion implementing Professor D. Stauffer's model. }
///  @PL{ Dość prosta symulacja zmiany opinii wykorzystująca model profesora D. Stauffer-a. }
/// @date 2026-05-29 (modified)
///       =========================================================
/// @details
///       (Obtained by simplifying the LANGUAGES program)
//======================================================================================================================

const char* SIMULATION_NAME="convinces_v0.01";
const char* WINDOW_HEADER="CONVINCE ver. 0.01d";

#include <cstdlib>
#include <iostream>
#include <compatyb.h>
#include "crand.h"
#include "cworld.h"
#include "toitoutoll.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "cert-err34-c"

using namespace sym2;

/// @name @PL{ Wymiary okna i świata. } @EN{  }
/// @details @PL{ Rozmiar świata musi być wyraźnie mniejszy od wymiaru okna, bo inaczej niewiele się wyświetli. }
///          @EN{  }
/// @{
unsigned	ScreenWidth=750;
unsigned	ScreenHeight=550;
unsigned	WorldWidth=50;
/// @}

/// @name @PL{ Parametry modelu. } @EN{ Model parameters. }
/// @details
///       @PL{ Poza zakresem siły można zmieniać tylko w kodzie. }
///       @EN{ Apart from the force range, they can only be changed in code. }
/// @{
double	 NewProbability=0.01;	//!< @EN{ Likelihood of a loner spontaneously changing his views on a new type of entertainment. } @PL{ ... }
double	 InfectionProb=0.5;		//!< @EN{ Probability of reversal of views to 0 (???). } @PL{ ... }
double	 SupportLev=0.9;		//!< @EN{ The power of support when the agent has some company for a given game. } @PL{ ... }

int	     MaximumStrength=1;  //!< @brief @EN{ What is the agent's greatest strength? } @PL{ Jaka jest największa siła agenta? }
                             //!< @EN{ If it's an integer, it's like 1, so there's no randomness. }
                             //!< @EN{ Everyone has the same strength. }
int      MinimalStrength=1;	 //!< @brief @EN{ The minimum force can't possibly be 0! } @PL{ Minimalna siła nie może wynosić 0. }
/// @}

/// @name @PL{ Parametry obsługi symulacji. } @EN{  }
/// @{
char	LogName[512]="attitude.log\0-------------------+--";
char	HistName[512]="\0--+---------convinces.otx----------";
char	MapLName[512]="\0--+---------convinces.gif----------";
char	MapPName[512]="\0--+---------powers.gif------------";
char	MaskName[512]="\0--+---------mask.gif--------------";

unsigned	InternalLogLen=7000;	//!< @brief @EN{ Length of internal history logs. NOTE! }
                                    //!<        @EN{ Not Object-wise passed to the source initialization method. }
unsigned	LogWriteRatio=1;		//!< @brief @EN{ After how many simulation steps will a log be saved? }
unsigned	ScrViewRatio=1;			//!< @brief @EN{ After how many simulation steps is the visualization performed? }
unsigned	MaxIterations=0xffffffff;

int	AUTO_START=0;
int	AUTO_END=0;			//!< Technically very... ;-)
int	Replay=0;			//!< Playback from a file instead of simulation?
/// @}

/// @brief @PL{ Czytanie parametrów z linii wywołania programu. } @EN{  }
int parse_options(const int argc,const char* argv[])
{
    for(int i=1;i<argc;i++)
        {
        if( *argv[i]=='-' ) /* This is an X11 or symshell option */
            continue;
        //Uppercasing
        wb_pchar hand( clone_str(argv[i]) );
        char*    rob=hand.get_ptr_val();
        char*    pom=strchr(rob,'=');
        if(pom==NULL)
                goto ERROR; //IT'S DEFINITELY BAD. THIS CAN'T BE.
        *pom='\0';
        strupr(rob);
        *pom='=';

//        if((pom=strstr(rob,"CLSS="))!=NULL) //Not NULL, i.e. exists
//        {
//            NoOfClasses=atoi(pom + 5);
//            if(NoOfClasses < 2)
//                {
//                cerr << "Bad CLSS =" << NoOfClasses << " (must be >2)" << endl;
//                return 0;
//                }
//        }
//        else
        if((pom=strstr(rob,"MPOW="))!=NULL) //Not NULL, i.e. exists
        {
            MaximumStrength=atoi(pom + 5);
            if(MaximumStrength < 0) //0 czy 1???
                {
                cerr << "Bad MPOW =" << MaximumStrength << " (must be >=1 )" << endl;
                return 0;
                }
        }
        else
        if((pom=strstr(rob,"WIDTH="))!=NULL) //Not NULL, i.e. exists
        {
            WorldWidth=atol(pom + 6);
            if(WorldWidth < 3 || WorldWidth >= ScreenWidth)
                {
                cerr << "Bad WIDTH = " << WorldWidth << "(must be in <3," << ScreenWidth << ">" << endl;
                return 0;
                }
        }
        else
        if((pom=strstr(rob,"WIDTHWIN="))!=NULL) //Not NULL, i.e. exists
        {
            ScreenWidth=atol(pom + 9);
            if(ScreenWidth < 50)
                {
                cerr << "Bad WIDTHWIN = " << ScreenWidth << " (must be >50)" << endl;
                return 0;
                }
        }
        else
        if((pom=strstr(rob,"HEIGHTWIN="))!=NULL) //Not NULL, i.e. exists
        {
            ScreenHeight=atol(pom + 10);
            if(ScreenHeight < 50)
                {
                cerr << "Bad HEIGHTWIN = " << ScreenHeight << " (must be >50)" << endl;
                return 0;
                }
        }
        else
        if((pom=strstr(rob,"MAX="))!=NULL) //Not NULL, i.e. exists
        {
            MaxIterations=atol(pom + 4);
            if(MaxIterations <= 0)
            {
                cerr<<"Bad MAX iterations. Must be >0"<<endl;
                return 0;
            }
            else
            {
                InternalLogLen= MaxIterations + 1;
            }
        }
        else
        if((pom=strstr(rob,"LOGC="))!=NULL) //Not NULL, i.e. exists
        {
            LogWriteRatio=atol(pom + 5);
            if(LogWriteRatio <= 0)
                {
                cerr<<"Bad LOGC (write to log frequency). Must be >0"<<endl;
                return 0;
                }
        }
        else
        if((pom=strstr(rob,"VIEW="))!=NULL) //Not NULL, i.e. exists
        {
            ScrViewRatio=atol(pom + 5);
            if(ScrViewRatio <= 0)
                {
                cerr<<"Bad VIEW (visualization frequency). Must be >0"<<endl;
                return 0;
                }
        }
        else
        if((pom=strstr(rob,"AUTO="))!=NULL) //Not NULL, i.e. exists
        {
            AUTO_START=atoi(pom + 5);
            cerr << "AUTO=" << AUTO_START << endl;
            if(AUTO_START)
                {
                    AUTO_END=1;
                cerr<<"STOP=Yes"<<endl;
                }
        }
        else
        if((pom=strstr(rob,"STOP="))!=NULL) //Not NULL, i.e. exists
        {
            AUTO_END=(toupper(pom[5]) == 'Y');
            cerr << "STOP=" << (AUTO_END?"Yes":"No") << endl;
        }
        else
        if((pom=strstr(rob,"ILOG="))!=NULL)  //Not NULL, i.e. exists
        {
            InternalLogLen=atoi(pom + 5);
            if(InternalLogLen < 50)
                    {
                        InternalLogLen=50;
                        cerr << "An internal log to short. Reset to a default minimum =" << InternalLogLen << endl;
                    }
        }
        else
        if((pom=strstr(rob,"LOGF="))!=NULL) //Not NULL, i.e. exists
        {
            strcpy(LogName,pom+5);
            cerr<<"Log name is '"<<LogName<<"'\n";
        }
        else
        if((pom=strstr(rob,"MAPL="))!=NULL) //Not NULL, i.e. exists
        {
            strcpy(MapLName,pom+5);
            cerr<<"Map of attitudes from file \""<<MapLName<<"\"\n";
        }
        else
        if((pom=strstr(rob,"MAPP="))!=NULL) //Not NULL, i.e. exists
        {
            strcpy(MapPName,pom+5);
            cerr<<"Map of individual power from file \""<<MapPName<<"\"\n";
        }
        else
        if((pom=strstr(rob,"MASK="))!=NULL) //Not NULL, i.e. exists
        {
            strcpy(MaskName,pom+5);
            cerr<<"Mask for live agents from file \""<<MaskName<<"\"\n";
        }
        else
        if((pom=strstr(rob,"HIST="))!=NULL) //Not NULL, i.e. exists
        {
            strcpy(HistName,pom+5);
            cerr<<"The History of the simulation will be saved to \""<<HistName<<"\"\n";
        }
        else
        if((pom=strstr(rob,"REPL="))!=NULL) //Not NULL, i.e. exists
        {
            strcpy(HistName,pom+5);
            Replay=1;
            cerr<<"The simulation will be replayed from \""<<HistName<<"\"\n";
        }
        else
        /* Ultimately, it turns out that there is no such option. */
        {
    ERROR:
            cerr<<"Unknown parameter \""<<argv[i]<<"\"\n";
            cerr<<"YOU CAN USE:\n";
            cerr<<"\tREPL=hist.otx - not simulate, but replay a simulation history file.\n";
            cerr<<"\tMAPL=initL.gif (or BMP)- file with an initialization map of attitudes (RANDOM)\n";
            cerr<<"\tMAPP=initP.gif (or BMP)- file with an initialization map of powers (RANDOM)\n";
            cerr<<"\tMASK=mask.gif	(or BMP)- mask file for alive (not black) agents (ALL ALIVE)\n";
            cerr << "\tWIDTH=NN - matrix size (" << WorldWidth << ")\n";
        //    cerr << "\tCLSS=NN - number of class. Must be power of 2. (" << NoOfClasses << ")\n";
            cerr << "\tMPOW=NN - max strength for initialization (" << MaximumStrength << ")\n"	;
            cerr << "\tMAX=NNNN - max simulation step (" << MaxIterations << ")\n";
            cerr << "\tILOG=NNNN - length of internal statistic logs (" << InternalLogLen << ")\n";
            cerr << "\tSTOP=N/Y - exit after MAX steps (" << (AUTO_END?"Yes":"No") << ")\n";
            cerr << "\tVIEV=N - visualisation frequency (" << ScrViewRatio << ")\n";
            cerr << "\tLOGC=N - log file saving frequency (" << LogWriteRatio << ")\n";
            cerr<<"\tLOGF=name.log - file for simulation log ("<<LogName<<")\n";
            cerr<<"\tHIST=hist.otx - file for full history of simulation.\n";
            cerr << "\tWIDTHWIN,HEIGHTWIN=XXX - initial window size.(" << ScreenWidth << 'x' << ScreenHeight << "\n";
            cerr << "\nAUTO=XXX - number of auto-repetition of simulation.(" << AUTO_START << ")\n";
        return 0;
        }
        }
    return 1;
}


/** @brief @EN{ GENERAL MAIN FUNCTION. } @EN{ OGÓLNA FUNKCJA MAIN. } */
/* ***************************************************************** */

int main(const int argc,const char* argv[])
{
    cout<<WINDOW_HEADER<<", compilation: "<<__DATE__<<' '<<__TIME__<<endl;
    cout<<"Programmed by W. Borkowski for A. Nowak & D. Stauffer"<<endl;
    cout<<"=========================================================="<<endl;
    cout.flush();

    if(!parse_options(argc,argv))
            exit(1);

    main_area_manager Lufciki(24, toi(ScreenWidth), toi(ScreenHeight), 28);
    if(!Lufciki.start(WINDOW_HEADER,argc,argv,1))
        {
        cerr<<"Can't initialize graphics"<<endl;
        exit(1);
        }

    //INITIALIZATION OF THE SIMULATION WORLD:
    convWorld& theWorld=*new convWorld(WorldWidth,
                                       NewProbability,
                                       InfectionProb,
                                       SupportLev,
                                       LogName,
                                       MapLName,
                                       MapPName,
                                       MaskName,
                                       tos(MaximumStrength),
                                       tos(MinimalStrength)
                               );

//    if(&theWorld == NULL) //NULL po `new` już się nie powinien zdarzać w C+=11 i wyżej.
//        {
//        cerr<<"Can't allocate the simulation world!\n"<<endl;
//        exit(1);
//        }

    //INICJALIZACJA
    RANDOMIZE() //Global Randomizer Initialization Macro
    theWorld.set_max_iteration(MaxIterations); //How many simulation steps at most?
    theWorld.set_input_ratio(ScrViewRatio);
    theWorld.set_log_ratio(LogWriteRatio);
    cout<<WINDOW_HEADER<<": LOADED."<<endl;
    theWorld.set_history_stream(HistName);

    if(Replay)
    {
        theWorld.initialize(&Lufciki, 1); //visualization initialization
        cout<<WINDOW_HEADER<<": PREPARED FOR READING. WAITING!"<<endl;
        Lufciki.process_input(); //First interface events. Ends after ctrl-B
        theWorld.read_loop(AUTO_END); //Reading a previously saved simulation.
    }
    else
    {
        theWorld.initialize(&Lufciki); //initialization of visualization and simulation layers
        cout<<WINDOW_HEADER<<": INITIALISED."<<endl;
        if(!AUTO_START)
        {
            Lufciki.process_input(); //First interface events. Ends after ctrl-B
            cout<<WINDOW_HEADER<<": STARTED."<<endl;
            theWorld.simulation_loop(AUTO_END); //MAIN SIMULATION LOOP
        }
        else
        {
            int statusWin=Lufciki.search("STATUS");
            Lufciki.maximize(statusWin);
            for(int symulacja=0; symulacja < AUTO_START; symulacja++)
                {
               //====MAIN SIMULATION LOOP STEP BY STEP
                cout<<WINDOW_HEADER<<": SIMULATION "<<symulacja<<" STARTED."<<endl;
                theWorld.simulation_loop(1);
                cout<<WINDOW_HEADER<<": SIMULATION "<<symulacja<<" DONE."<<endl;
                if(symulacja < AUTO_START - 1)
                    {
                    theWorld.restart(); //Reinitialize for the next pass.
                    }
                }
        }
    }

    cout<<WINDOW_HEADER<<": CLOSING."<<endl;

    cout.flush();

    delete &theWorld; //Deallocation of the world with all its components.
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
