// Dosyć prosta symulacja zmiany poglądów — attitude wg. modelu A. Nowak-a.
// Uzyskana przez uproszczenie programu LANGUAGES
// ////////////////////////////////////////////////////////////////////////
// @date 2026-05-06 (modification)
const char* WINDOW_HEADER="ATTITUDES version 1.01c";
const char* SIMULATION_NAME="attitudes_v1.01c";
// * 15.02.2006 - 1.01: Zrekompilowane z nową wersją biblioteki lufcików.
// * 07.04.2026 - 1.01: Zrekompilowane z nową wersją biblioteki lufcików po 20 latach!

#include <cstdlib>

#include <iostream>
#include <compatyb.h>

#include "arand.h"
#include "aworld.h"

using namespace symshell2;

unsigned	SCR_WIDTH=750;
unsigned	SCR_HEIGHT=550;

unsigned	InternalLogLen=7000;	//Nieobiektowo przekazywane do metody inicjalizacji źródeł.

char	LogName[512]="attitude.log\0-------------------+--";
char	HistName[512]="\0--+---------attitude.otx----------";
char	MapLName[512]="\0--+---------attitude.gif----------";
char	MapPName[512]="\0--+---------powers.gif------------";
char	MaskName[512]="\0--+---------mask.gif--------------";

unsigned	WorldWidth=50;
unsigned	MaxNumOfIterations=0xffffffff;
unsigned	LogWriteRatio=1;
unsigned	ScrViewRatio=1;

int	GrowingStrength=0;			//Czy siła ma się powiększać "z wiekiem"
int	MaximalStrength=100;		//Jaka największa siła
int	ThresholdPercent=100;		//Powyżej jakiej siły zmiany "poglądu" są już niemożliwe.

int	NumberOfAttitudes=2;
int	NoisePercent=0;
double	MutationProb=0;
int	NeighborhoodR=1;	//Promień sąsiedztwa. Jeśli 1 to sąsiedztwo 3 × 3 - 1.
int	NeighborhoodD=-1;	//Wszyscy sąsiedzi — nielosowo. 8 - losowo!!!
int	SelfTaking=1;		//8+1... Branie siębie do zliczania presji.
bool	SimulationMode=false;	//Tryb symulacji. 0 == synchroniczna.

int	AutoExit=0;			//Automatyczne wychodzenie z programu po końcu symulacji.
int	Replay=0;			//Odtwarzanie z zapisu.
int	AUTOSTART=0;


int parse_options(const int argc,const char* argv[])
{
    for(int i=1;i<argc;i++)
        {
        if( *argv[i]=='-' ) /* Opcja X11 lub symshell-a */
            continue;
        //Uppercasing
        wb_pchar hand( clone_str(argv[i]) );
        char*    rob=hand.get_ptr_val();
        char*    pom=strchr(rob,'=');
        if(pom==NULL)
                goto ERROR; //NA PEWNO ZLE
        *pom='\0';
        strupr(rob);
        *pom='=';
        if((pom=strstr(rob,"SPCH="))!=NULL) //Nie NULL, czyli jest
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
        if((pom=strstr(rob,"NOIP="))!=NULL) //Nie NULL, czyli jest
        {
        NoisePercent=atoi(pom + 5);
        if(NoisePercent < 0 || NoisePercent > 100)
            {
            cerr << "Bad NOIP =" << NoisePercent << " (must be in <0,100>)" << endl;
            return 0;
            }
        cerr << "NOIP (noise percent) = " << NoisePercent << endl;
        }
        else
        if((pom=strstr(rob,"CLSS="))!=NULL) //Nie NULL, czyli jest
        {
        NumberOfAttitudes=atoi(pom + 5);
        if(NumberOfAttitudes < 2)
            {
            cerr << "Bad CLSS =" << NumberOfAttitudes << " (must be >2)" << endl;
            return 0;
            }
        }
        else
        if((pom=strstr(rob,"MPOW="))!=NULL) //Nie NULL, czyli jest
        {
        MaximalStrength=atoi(pom + 5);
        if(MaximalStrength < 0) //0 czy 1???
            {
            cerr << "Bad MPOW =" << MaximalStrength << " (must be >=1 )" << endl;
            return 0;
            }
        }
        else
        if((pom=strstr(rob,"WPOW="))!=NULL) //Nie NULL, czyli jest
        {
        GrowingStrength=atoi(pom + 5);
        if(GrowingStrength < 0)
            {
            cerr << "Bad WPOW =" << GrowingStrength << " (must be >=0 )" << endl;
            return 0;
            }
        cerr << "WPOW=" << GrowingStrength << endl;
        }
        else
        if((pom=strstr(rob,"TRSP="))!=NULL) //Nie NULL, czyli jest
        {
            ThresholdPercent=atoi(pom + 5);
        if(ThresholdPercent < 0 || ThresholdPercent > 100)
            {
            cerr << "Bad TRSP = " << int(ThresholdPercent) << "(must be in <0,100>" << endl;
            return 0;
            }
        else
            {
            cerr << "Threshold of strength for change parameters = " << int(ThresholdPercent) << "%" << endl;
            if(GrowingStrength == 0) //Nie ma sensu TRSP, jeśli nie jest ruchoma siła
                {
                    GrowingStrength=1;
                cerr << "Automatically set WPOW to " << GrowingStrength << endl;
                }
            }
        }
        else
        if((pom=strstr(rob,"WIDTH="))!=NULL) //Nie NULL, czyli jest
        {
        WorldWidth=atol(pom + 6);
        if(WorldWidth < 3 || WorldWidth >= SCR_WIDTH)
            {
            cerr << "Bad WIDTH = " << WorldWidth << "(must be in <3," << SCR_WIDTH << ">" << endl;
            return 0;
            }
        }
        else
        if((pom=strstr(rob,"WIDTHWIN="))!=NULL) //Nie NULL, czyli jest
        {
            SCR_WIDTH=atol(pom + 9);
        if(SCR_WIDTH < 50)
            {
            cerr << "Bad WIDTHWIN = " << SCR_WIDTH << " (must be >50)" << endl;
            return 0;
            }
        }
        else
        if((pom=strstr(rob,"HEIGHTWIN="))!=NULL) //Nie NULL, czyli jest
        {
            SCR_HEIGHT=atol(pom + 10);
        if(SCR_HEIGHT < 50)
            {
            cerr << "Bad HEIGHTWIN = " << SCR_HEIGHT << " (must be >50)" << endl;
            return 0;
            }
        }
        else
        if((pom=strstr(rob,"MAX="))!=NULL) //Nie NULL, czyli jest
        {
            MaxNumOfIterations=atol(pom + 4);
        if(MaxNumOfIterations <= 0)
            {
            cerr<<"Bad MAX iterations. Must be >0"<<endl;
            return 0;
            }
        else
            {
                InternalLogLen= MaxNumOfIterations + 1;
            }
        }
        else
        if((pom=strstr(rob,"LOGC="))!=NULL) //Nie NULL, czyli jest
        {
            LogWriteRatio=atol(pom + 5);
        if(LogWriteRatio <= 0)
            {
            cerr<<"Bad LOGC (write to log frequency). Must be >0"<<endl;
            return 0;
            }
        }
        else
        if((pom=strstr(rob,"VIEW="))!=NULL) //Nie NULL, czyli jest
        {
            ScrViewRatio=atol(pom + 5);
        if(ScrViewRatio <= 0)
            {
            cerr<<"Bad VIEW (visualization frequency). Must be >0"<<endl;
            return 0;
            }
        }
        else
        if((pom=strstr(rob,"SELF="))!=NULL) //Nie NULL, czyli jest
        {
            SelfTaking=(toupper(pom[5]) == 'Y');
        cerr << "SELF=" << (SelfTaking ? "Yes" : "No") << endl;
        }
        else
        if((pom=strstr(rob,"INDI="))!=NULL) //Nie NULL, czyli jest
        {
        NeighborhoodR=atoi(pom + 5);
        if(NeighborhoodR >= 1 && NeighborhoodR < WorldWidth / 2 - 1)
            {
            cerr << "INDI=" << NeighborhoodR << endl;
            }
            else
            {
            cerr << "Bad INDI=" << NeighborhoodR << " Must from 1 to " << WorldWidth / 2 - 1 << endl;
            return 0;
            }
        }
        else
        if((pom=strstr(rob,"PRTR="))!=NULL) //Nie NULL, czyli jest
            {
                NeighborhoodD=atoi(pom + 5);
                if(NeighborhoodD == -1)
                {
                    cerr<<"PRTR = all"<<endl;
                }
                else
                    if(NeighborhoodD > 1 && NeighborhoodD <= sqr(NeighborhoodR * 2 + 1) - 1)
                    {
                        cerr << "PRTR=" << NeighborhoodD << endl;
                    }
                    else
                    {
                        cerr << "Bad PRTR=" << NeighborhoodD
                             << " Must from 2 to " << sqr(NeighborhoodR * 2 + 1) - 1 << endl;
                        return 0;
                    }
            }
        else
        if((pom=strstr(rob,"AUTO="))!=NULL) //Nie NULL, czyli jest
        {
        AUTOSTART=atoi(pom+5);
        cerr<<"AUTO="<<AUTOSTART<<endl;
        if(AUTOSTART)
            {
                AutoExit=1;
            cerr << "STOP=" << (AutoExit ? "Yes" : "No") << endl;
            }
        }
        else
        if((pom=strstr(rob,"STOP="))!=NULL) //Nie NULL, czyli jest
        {
        AutoExit=(toupper(pom[5]) == 'Y');
        cerr << "STOP=" << (AutoExit ? "Yes" : "No") << endl;
        }
        else  //SYNC
        if((pom=strstr(rob,"SYNC="))!=NULL) //Nie NULL, czyli jest
        {
        SimulationMode=!(toupper(pom[5]) == 'Y');
        cerr << "SYNC=" << (SimulationMode == 0 ? "Yes" : "No") << endl;
        }
        else
        if((pom=strstr(rob,"ILOG="))!=NULL) //Nie NULL, czyli jest
        {
        InternalLogLen=atoi(pom + 5);
        if(InternalLogLen < 50)
                {
                InternalLogLen=50;
                cerr << "An internal log to short. Reset to a default minimum =" << InternalLogLen << endl;
                }
        }
        else
        if((pom=strstr(rob,"LOGF="))!=NULL) //Nie NULL, czyli jest
        {
        strcpy(LogName,pom+5);
        }else
        if((pom=strstr(rob,"MAPL="))!=NULL) //Nie NULL, czyli jest
        {
        strcpy(MapLName,pom+5);
        cerr<<"Map of attitudes from file \""<<MapLName<<"\"\n";
        }
        else
        if((pom=strstr(rob,"MAPP="))!=NULL) //Nie NULL, czyli jest
        {
        strcpy(MapPName,pom+5);
        cerr<<"Map of individual power from file \""<<MapPName<<"\"\n";
        }
        else
        if((pom=strstr(rob,"MASK="))!=NULL) //Nie NULL, czyli jest
        {
        strcpy(MaskName,pom+5);
        cerr<<"Mask for live agents from file \""<<MaskName<<"\"\n";
        }
        else
        if((pom=strstr(rob,"HIST="))!=NULL) //Nie NULL, czyli jest
        {
        strcpy(HistName,pom+5);
        cerr<<"The history of the simulation will be saved to \""<<HistName<<"\"\n";
        }
        else
        if((pom=strstr(rob,"REPL="))!=NULL) //Nie NULL, czyli jest
        {
        strcpy(HistName,pom+5);
        Replay=1;
        cerr<<"The simulation will be replayed from \""<<HistName<<"\"\n";
        }
        else
        /* Ostatecznie wychodzi, że nie ma takiej opcji */
        {
    ERROR:
            cerr<<"Unknown parameter \""<<argv[i]<<"\"\n";
            cerr<<"YOU CAN USE:\n";
            cerr<<"\tREPL=hist.otx - not simulate, but replay a simulation history file.\n";
            cerr<<"\tMAPL=initL.gif (or BMP)- file with an initialization map of attitudes (RANDOM)\n";
            cerr<<"\tMAPP=initP.gif (or BMP)- file with an initialization map of powers (RANDOM)\n";
            cerr<<"\tMASK=mask.gif	(or BMP)- mask file for alive (not black) agents (ALL ALIVE)\n";
            cerr << "\tWIDTH=NN - matrix size (" << WorldWidth << ")\n";
            cerr << "\tCLSS=NN - number of class. Must be power of 2. (" << NumberOfAttitudes << ")\n";
            cerr << "\tMPOW=NN - max strength for initialization (" << MaximalStrength << ")\n"	;
            cerr << "\tWPOW=N	- walking step of strength	(" << GrowingStrength << ")\n";
            cerr << "\tTRSP=N - % of threshold of strength (" << ThresholdPercent << ")\n";
            cerr << "\tSYNC=Y/N - synchronic (Y) or Monte-Carlo simulation mode (" << (SimulationMode == 0 ? "Yes" : "No") << ")\n";
            cerr << "\tPRTR=2..WIDTH^2-1 - number of interaction partners (-1 = all neighbourhood) (" << NeighborhoodD << ")\n";
            cerr << "\tINDI=1..WIDTH/2-1 - interaction distance (" << NeighborhoodR << ")\n";
            cerr << "\tSELF=N/Y - use self for calculations (" << (SelfTaking ? "Yes" : "No") << ")\n";
            cerr << "\tNOIP=NN - percent of noise (" << NoisePercent << ")\n";
            cerr << "\tSPCH=NN - percent of spontaneity change of attitudes (" << MutationProb * 100 << ")\n";
            cerr << "\tMAX=NNNN - max simulation step (" << MaxNumOfIterations << ")\n";
            cerr << "\tILOG=NNNN - length of internal statistic logs (" << InternalLogLen << ")\n";
            cerr << "\tSTOP=N/Y - exit after MAX steps (" << (AutoExit ? "Yes" : "No") << ")\n";
            cerr << "\tVIEV=N - visualisation frequency (" << ScrViewRatio << ")\n";
            cerr << "\tLOGC=N - log file saving frequency (" << LogWriteRatio << ")\n";
            cerr<<"\tLOGF=name.log - file for simulation log ("<<LogName<<")\n";
            cerr<<"\tHIST=hist.otx - file for full history of simulation.\n";
            cerr << "\tWIDTHWIN,HEIGHTWIN=XXX - initial window size.(" << SCR_WIDTH << 'x' << SCR_HEIGHT << "\n";
            cerr<<"\nAUTO=XXX - number of auto-repetition of simulation.("<<AUTOSTART<<")\n";
        return 0;
        }
        }
    return 1;
}


/*  OGÓLNA FUNKCJA MAIN  */
/* ********************* */

int main(const int argc,const char* argv[])
{
    cout<<WINDOW_HEADER<<", compilation: "<<__DATE__<<' '<<__TIME__<<endl;
    cout<<"Programmed by W. Borkowski for A. Nowak & R. Vallacher"<<endl;
    cout<<"======================================================="<<endl;
    cout.flush();

    if(!parse_options(argc,argv))
            exit(1);

    main_area_menager Lufciki(24, SCR_WIDTH, SCR_HEIGHT, 28);
    if(!Lufciki.start(WINDOW_HEADER,argc,argv,1))
        {
        cerr<<"Can't initialize graphics"<<endl;
        exit(1);
        }

    //INICJALIZACJA SYMULACJI
    aworld& theWorld=*new aworld(WorldWidth,
                                 LogName,
                                 MapLName,
                                 MapPName,
                                 MaskName,
                                 NoisePercent / 100.0, //Szum od 0-1
                               MaximalStrength, //Żeby była w przedziale
                               NumberOfAttitudes,
                                 NeighborhoodR,
                                 NeighborhoodD,
                                 SelfTaking,
                                 SimulationMode == 0, //Synchroniczna czy nie
                               GrowingStrength,
                                 MaximalStrength * ThresholdPercent / 100.0,
                                 MutationProb
                               );

    if(&theWorld == NULL)
        {
        cerr<<"Can't allocate a simulation world!\n"<<endl;
        exit(1);
        }

    //INICJALIZACJA
    RANDOMIZE() //Makro inicjalizacja globalnego randomizer-a
    theWorld.set_max_iteration(MaxNumOfIterations); //Ile najwięcej kroków
    theWorld.set_input_ratio(ScrViewRatio);
    theWorld.set_log_ratio(LogWriteRatio);
    cout<<WINDOW_HEADER<<": LOADED."<<endl;
    theWorld.set_history_stream(HistName);

    if(Replay)
    {
        theWorld.initialize(&Lufciki, 1); //inicjalizacja wizualizacji
        cout<<WINDOW_HEADER<<": PREPARED FOR READING. WAITING!"<<endl;
        Lufciki.process_input(); //Pierwsze zdarzenia. Kończą się po ctrl-B
        theWorld.read_loop(AutoExit);
    }
    else
    {
        theWorld.initialize(&Lufciki); //inicjalizacja wizualizacji i warstw symulacji
        cout<<WINDOW_HEADER<<": INITIALISED."<<endl;
        if(!AUTOSTART)
        {
            Lufciki.process_input(); //Pierwsze zdarzenia. Kończą się po ctrl-B.
            //GŁÓWNA PĘTLA SYMULACJI
            cout<<WINDOW_HEADER<<": STARTED."<<endl;
            theWorld.simulation_loop(AutoExit);
        }
        else
        {
            int statusWin=Lufciki.search("STATUS");
            Lufciki.maximize(statusWin);
            for(int symulacja=0;symulacja<AUTOSTART;symulacja++)
                {
                //GŁÓWNA PĘTLA SYMULACJI
                cout<<WINDOW_HEADER<<": SIMULATION "<<symulacja<<" STARTED."<<endl;
                theWorld.simulation_loop(1);
                cout<<WINDOW_HEADER<<": SIMULATION "<<symulacja<<" DONE."<<endl;
                if(symulacja<AUTOSTART-1)
                    {
                    //Reinicjalizacja:
                    theWorld.restart();
                    }
                }
        }

    }

    cout<<WINDOW_HEADER<<": CLOSING."<<endl;

    cout.flush();

    delete &theWorld; //Dealokacja świata wraz ze wszystkimi składowymi.
    cout<<"----------> See you later!!! <--------------\n"<<endl<<flush;
    return 0;
}

/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */
