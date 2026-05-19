/// @file
/// @brief
///  @EN{ A fairly simple simulation of a change of opinion implementing Professor D. Stauffer's model. }
///  @PL{  }
/// @date 2026-05-19 (modified)
///       =========================================================
/// @details
///       (Obtained by simplifying the LANGUAGES program)
//======================================================================================================================

const char* SIMULATION_NAME="convinces_v0.01";
const char* WINDOW_HEADER="CONVINCE ver. 0.01c";

#include <cstdlib>
#include <iostream>
#include <compatyb.h>
#include "crand.h"
#include "cworld.h"

using namespace sym2;

unsigned	WorldWidth=50;

double	 NewProbability=0.01;
double	 InfectionProb=0.5;
double	 SupportLev=0.9;

char	LogName[512]="attitude.log\0-------------------+--";
char	HistName[512]="\0--+---------convinces.otx----------";
char	MapLName[512]="\0--+---------convinces.gif----------";
char	MapPName[512]="\0--+---------powers.gif------------";
char	MaskName[512]="\0--+---------mask.gif--------------";

unsigned	ScreenWidth=750;
unsigned	ScreenHeight=550;
unsigned	InternalLogLen=7000;	//!< Length of internal history logs. NOTE!
                                    //!< Not Object-wise passed to the source initialization method
unsigned	LogWriteRatio=1;		//!< After how many simulation steps will a log be saved?
unsigned	ScrViewRatio=1;			//!< After how many simulation steps is the visualization performed?
unsigned	MaxIterations=0xffffffff;

int	MaximumStrength=1;	//!< What is the agent's greatest strength?
                        //!< If it's an integer, it's like 1, so there's no randomness.
                        //!< Everyone has the same strength.
int	MinimalStrength=1;	//!< The minimum force can't possibly be 0!

int	NoOfClasses=2;


int	AUTOSTART=0;
int	iWychodzenie=0;		//!< Technically very... ;-)
int	Replay=0;			//!< Playback from a file instead of simulation?

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

        if((pom=strstr(rob,"CLSS="))!=NULL) //Not NULL, i.e. exists
        {
        NoOfClasses=atoi(pom + 5);
        if(NoOfClasses < 2)
            {
            cerr << "Bad CLSS =" << NoOfClasses << " (must be >2)" << endl;
            return 0;
            }
        }
        else
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
        AUTOSTART=atoi(pom+5);
        cerr<<"AUTO="<<AUTOSTART<<endl;
        if(AUTOSTART)
            {
            iWychodzenie=1;
            cerr<<"STOP=Yes"<<endl;
            }
        }
        else
        if((pom=strstr(rob,"STOP="))!=NULL) //Not NULL, i.e. exists
        {
        iWychodzenie=(toupper(pom[5])=='Y');
        cerr<<"STOP="<<(iWychodzenie?"Yes":"No")<<endl;
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
        }else
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
            cerr << "\tCLSS=NN - number of class. Must be power of 2. (" << NoOfClasses << ")\n";
            cerr << "\tMPOW=NN - max strength for initialization (" << MaximumStrength << ")\n"	;
            cerr << "\tMAX=NNNN - max simulation step (" << MaxIterations << ")\n";
            cerr << "\tILOG=NNNN - length of internal statistic logs (" << InternalLogLen << ")\n";
            cerr<<"\tSTOP=N/Y - exit after MAX steps ("<<(iWychodzenie?"Yes":"No")<<")\n";
            cerr << "\tVIEV=N - visualisation frequency (" << ScrViewRatio << ")\n";
            cerr << "\tLOGC=N - log file saving frequency (" << LogWriteRatio << ")\n";
            cerr<<"\tLOGF=name.log - file for simulation log ("<<LogName<<")\n";
            cerr<<"\tHIST=hist.otx - file for full history of simulation.\n";
            cerr << "\tWIDTHWIN,HEIGHTWIN=XXX - initial window size.(" << ScreenWidth << 'x' << ScreenHeight << "\n";
            cerr<<"\nAUTO=XXX - number of auto-repetition of simulation.("<<AUTOSTART<<")\n";
        return 0;
        }
        }
    return 1;
}


/* GENERAL MAIN FUNCTION */
/* ********************* */

int main(const int argc,const char* argv[])
{
    cout<<WINDOW_HEADER<<", compilation: "<<__DATE__<<' '<<__TIME__<<endl;
    cout<<"Programmed by W. Borkowski for A. Nowak & D. Stauffer"<<endl;
    cout<<"=========================================================="<<endl;
    cout.flush();

    if(!parse_options(argc,argv))
            exit(1);

    main_area_manager Lufciki(24, ScreenWidth, ScreenHeight, 28);
    if(!Lufciki.start(WINDOW_HEADER,argc,argv,1))
        {
        cerr<<"Can't initialize graphics"<<endl;
        exit(1);
        }

    //INITIALIZATION OF THE SIMULATION WORLD:
    aWorld& theWorld=*new aWorld(WorldWidth,
                                 NewProbability,
                                 InfectionProb,
                                 SupportLev,
                                 LogName,
                                 MapLName,
                                 MapPName,
                                 MaskName,
                                 MaximumStrength,
                                 MinimalStrength
                               );

    if(&theWorld == NULL)
        {
        cerr<<"Can't allocate the simulation world!\n"<<endl;
        exit(1);
        }

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
        theWorld.read_loop(iWychodzenie); //Reading a previously saved simulation.
    }
    else
    {
        theWorld.initialize(&Lufciki); //initialization of visualization and simulation layers
        cout<<WINDOW_HEADER<<": INITIALISED."<<endl;
        if(!AUTOSTART)
        {
            Lufciki.process_input(); //First interface events. Ends after ctrl-B
            cout<<WINDOW_HEADER<<": STARTED."<<endl;
            theWorld.simulation_loop(iWychodzenie); //MAIN SIMULATION LOOP
        }
        else
        {
            int statusWin=Lufciki.search("STATUS");
            Lufciki.maximize(statusWin);
            for(int symulacja=0;symulacja<AUTOSTART;symulacja++)
                {
               //====MAIN SIMULATION LOOP STEP BY STEP
                cout<<WINDOW_HEADER<<": SIMULATION "<<symulacja<<" STARTED."<<endl;
                theWorld.simulation_loop(1);
                cout<<WINDOW_HEADER<<": SIMULATION "<<symulacja<<" DONE."<<endl;
                if(symulacja<AUTOSTART-1)
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

/* ****************************************************************** */
/*        SYMSHELL2 EXAMPLE  version 2006/2022/2026                   */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */



