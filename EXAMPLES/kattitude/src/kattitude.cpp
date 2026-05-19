/// @file
/// @brief
///  @EN{  }
///  @PL{ Symulacja zmiany poglądów - attitudes. }
/// @date 2026-05-19 (modified)
///       =========================================================
/// @details (kattitude old example for SymShell)
///     Uzyskana niegdyś przez uproszczenie programu LANGUAGES.
///     Uzupełniona o wagi opinii innych, jako fragmentaryczna implementacja teorii Arie Kruglanskiego.
///     Dodano także inicjowanie z niekompletnym wypełnieniem przestrzeni symulacji
///     oraz różnych procentów jednej z klas (minority vs. majority).
//======================================================================================================================

/// Historia:
/// =========
/// * v 0.6 - dodanie losowania nieproporcjonalnego, znaczne zmiany w statystykach, zmiany z bazowej bibliotece
/// * v 0.61 - użycie innego histogramu
const char* SIMULATION_NAME="attitudes_k_v0.71c";
const char* WINDOW_HEADER="ATTITUDES K version 0.71c";

#include <cstdlib>
#include <iostream>

#include "compatyb.h"
//#include "wbminmax.hpp"
#include "arand.h"
#include "aworld.h"

using namespace sym2;

unsigned SCR_WIDTH=750;
unsigned SCR_HEIGHT=550;

unsigned internal_log=7000;            ///< Długość logu. Nieobiektowo przekazywane do metody inicjalizacji źródeł
unsigned spatial_correlation_mode=50;  ///< Dystans korelacji(?). Nieobiektowo przekazywane do metody inicjalizacji źródeł

char  LogName[512]="kattitude.log\0-----------------------";
char HistName[512]="\0--+---------kattitudes.otx----------";
char MapLName[512]="\0--+---------kattitudes.gif----------";
char MapPName[512]="\0--+---------powers.gif--------------";
char MaskName[512]="\0--+---------mask.gif----------------";

unsigned iWidth=50;
unsigned iMaxIterations=0xffffffff;
unsigned iLogRatio=1;
unsigned iViewRatio=1;

int  RuchomaSila=0;			///< Określa, czy siła się powiększa "z wiekiem".
int  MaksymalnaSila=100;	///< Jaka największa siła.
int  TresProcent=100;		///< Powyżej jakiej wartości siły zmiany "poglądu" są już niemożliwe.

int    LiczbaKlas=2;		///< Tu oznacza liczbę poglądów.
double Fill=1;				///< 0.001 do 1 - początkowe wypełnienie przestrzeni agentów (wykonywane przez losowe usuwanie!!!).
double MajorMinor=-1;		///< Jaka część społeczeństwa będzie wyznawać "biały" pogląd (mniejszość vs. większość).
double ProbMig=0;			///< Prawdopodobieństwo migracji, gdy presja otoczenia wskazuje na zmianę poglądów.
int    ProcentSzumu=0;		///< ...
double MutacjeSpon=0;		///< ...
int    NeighborhoodRadius=1;	///< 3x3-1
int    NumberOfNeighbors=-1;	///< Gdy -1 to wszyscy sąsiedzi nielosowo, a gdy 8 to losowo!!!

double WagaSiebie=1;		///< 8+1
double NeedForClosure=1;	///< Waga opinii innych.

bool   TypSymulacji=false;	///< Synchroniczna

int  iWychodzenie=0;
int  Replay=0;
int	 AUTOSTART=0;


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
            goto ERROR; //TO FATALNA SYTUACJA ;-) Parametr musi zaczynać się od `-` albo miec w środku ` = `.
    *pom='\0';
    strupr(rob);
    *pom='=';
    if((pom=strstr(rob,"SPCH="))!=nullptr) //Nie nullptr, czyli jest
    {
    MutacjeSpon=atof(pom+5);
    if(MutacjeSpon<0 || MutacjeSpon>100)
        {
        cerr<<"Bad SPCH ="<<MutacjeSpon<<" (must be in <0,100>)"<<endl;
        return 0;
        }
    cerr<<"SPCH (spn. change percent) = "<<MutacjeSpon<<endl;
    MutacjeSpon/=100; //Ułamek nie procent tak naprawdę
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
            Fill/=100; //Ułamek nie procent tak naprawdę
        }
        else
            cerr<<"FILL = "<<Fill<<endl;
    }
    else
    if((pom=strstr(rob,"BALA="))!=nullptr) //Nie nullptr, czyli jest
    {
        MajorMinor=atof(pom+5);
        if(MajorMinor<0 || MajorMinor>100)
        {
            cerr<<"Bad BALA = "<<MajorMinor<<" (must be in range <0,1> or from 1.x% to 100%)"<<endl;
            return 0;
        }
        if(MajorMinor>1)
        {
            cerr<<"BALA (majority percent) = "<<MajorMinor<<endl;
            MajorMinor/=100; //Ułamek, a nie procent tak naprawdę
        }
        else
            cerr<<"BALA = "<<MajorMinor<<endl;
    }
    else
    if((pom=strstr(rob,"PMIG="))!=nullptr) //Nie nullptr, czyli jest
    {
        ProbMig=atof(pom+5);
        if(ProbMig<0 || ProbMig>100)
        {
            cerr<<"Bad PMIG = "<<Fill<<" (must be in range <0,1> or from 1.x% to 100%)"<<endl;
            return 0;
        }
        if(ProbMig>1)
        {
            cerr<<"PMIG (probability of migration) = "<<ProbMig<<'%'<<endl;
            ProbMig/=100; //Ułamek, a nie procent tak naprawdę
        }
        else
            cerr<<"PMIG (probability of migration) = "<<ProbMig<<endl;
    }
    else
    if((pom=strstr(rob,"NOIP="))!=nullptr) //Nie nullptr, czyli jest
    {
    ProcentSzumu=atoi(pom+5);
    if(ProcentSzumu<0 || ProcentSzumu>100)
        {
        cerr<<"Bad NOIP ="<<ProcentSzumu<<" (must be in <0,100>)"<<endl;
        return 0;
        }
    cerr<<"NOIP (noise percent) = "<<ProcentSzumu<<endl;
    }
    else
    if((pom=strstr(rob,"CLSS="))!=nullptr) //Nie nullptr, czyli jest
    {
    LiczbaKlas=atoi(pom + 5);
    if(LiczbaKlas < 2)
        {
        cerr << "Bad parameter CLSS = " << LiczbaKlas << " (must be > 2)" << endl;
        return 0;
        }
    }
    else
    if((pom=strstr(rob,"MPOW="))!=nullptr) //Nie nullptr, czyli jest
    {
    MaksymalnaSila=atoi(pom+5);
    if(MaksymalnaSila<0) //0 czy 1???
        {
        cerr<<"Bad MPOW = "<<MaksymalnaSila<<" (must be >=1 )"<<endl;
        return 0;
        }
    }
    else
    if((pom=strstr(rob,"WPOW="))!=nullptr) //Nie nullptr, czyli jest
    {
    RuchomaSila=atoi(pom+5);
    if(RuchomaSila<0)
        {
        cerr<<"Bad WPOW = "<<RuchomaSila<<" (must be >=0 )"<<endl;
        return 0;
        }
    cerr<<"WPOW = "<<RuchomaSila<<endl;
    }
    else
    if((pom=strstr(rob,"TRSP="))!=nullptr) //Nie nullptr, czyli jest
    {
    TresProcent=atoi(pom+5);
    if(TresProcent<0 || TresProcent>100)
        {
        cerr<<"Bad TRSP = "<<int(TresProcent)<<"(must be in <0,100>"<<endl;
        return 0;
        }
    else
        {
        cerr<<"Threshold of strength for change parameters = "<<int(TresProcent)<<"%"<<endl;
        if(RuchomaSila==0) //Nie ma sensu TRSP, gdy nie jest ruchoma siła
            {
            RuchomaSila=1;
            cerr<<"Automatically set WPOW to "<<RuchomaSila<<endl;
            }
        }
    }
    else
    if((pom=strstr(rob,"WIDTH="))!=nullptr) //Nie nullptr, czyli jest
    {
    iWidth=atol(pom+6);
    if(iWidth<3 || iWidth >= SCR_WIDTH)
        {
        cerr << "Bad WIDTH = " << iWidth << "(must be in <3," << SCR_WIDTH << ">" << endl;
        return 0;
        }
    }
    else
    if((pom=strstr(rob,"WIDTHWIN="))!=nullptr) //Nie nullptr, czyli jest
    {
        SCR_WIDTH=atol(pom + 9);
    if(SCR_WIDTH < 50)
        {
        cerr << "Bad WIDTHWIN = " << SCR_WIDTH << " (must be >50)" << endl;
        return 0;
        }
    }
    else
    if((pom=strstr(rob,"HEIGHTWIN="))!=nullptr) //Nie nullptr, czyli jest
    {
        SCR_HEIGHT=atol(pom + 10);
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
    WagaSiebie=atof(pom+5);
    cerr<<"SELF = "<<WagaSiebie<<endl;
    }
    else
    if((pom=strstr(rob,"NFOC="))!=nullptr) //Nie nullptr, czyli jest
    {
        NeedForClosure=atof(pom + 5);
    cerr << "NFOC = " << NeedForClosure << endl;
    }
    else
    if((pom=strstr(rob,"INDI="))!=nullptr) //Nie nullptr, czyli jest
    {
    NeighborhoodRadius=atoi(pom + 5);
    if(NeighborhoodRadius >= 1 && NeighborhoodRadius < iWidth / 2 - 1)
        {
        cerr << "INDI=" << NeighborhoodRadius << endl;
        }
    else
        {
        cerr << "Bad INDI=" << NeighborhoodRadius << " Must from 1 to " << iWidth / 2 - 1 << endl;
        return 0;
        }
    }
    else
    if((pom=strstr(rob,"PRTR="))!=nullptr) //Nie nullptr, czyli jest
        {
            NumberOfNeighbors=atoi(pom + 5);
            if(NumberOfNeighbors == -1)
            {
                cerr<<"PRTR = all"<<endl;
            }
            else
                if(NumberOfNeighbors > 1 && NumberOfNeighbors <= sqr(NeighborhoodRadius * 2 + 1) - 1)
                {
                    cerr << "PRTR=" << NumberOfNeighbors << endl;
                }
                else
                {
                    cerr << "Bad PRTR=" << NumberOfNeighbors
                         << " Must from 2 to " << sqr(NeighborhoodRadius * 2 + 1) - 1 << endl;
                    return 0;
                }
        }
    else
    if((pom=strstr(rob,"AUTO="))!=nullptr) //Nie nullptr, czyli jest
    {
    AUTOSTART=atoi(pom+5);
    cerr<<"AUTO="<<AUTOSTART<<endl;
    if(AUTOSTART)
        {
        iWychodzenie=1;
        cerr<<"STOP = Yes"<<endl;
        }
    }
    else
    if((pom=strstr(rob,"STOP="))!=nullptr) //Nie nullptr, czyli jest
    {
    iWychodzenie=(toupper(pom[5])=='Y');
    cerr<<"STOP = "<<(iWychodzenie?"Yes":"No")<<endl;
    }
    else  //SYNC
    if((pom=strstr(rob,"SYNC="))!=nullptr) //Nie nullptr, czyli jest
    {
    TypSymulacji=!(toupper(pom[5])=='Y');
    cerr<<"SYNC = "<<(TypSymulacji==0?"Yes":"No")<<endl;
    }
    else
    if((pom=strstr(rob,"ILOG="))!=nullptr) //Nie nullptr, czyli jest
    {
    internal_log=atoi(pom+5);
    if(internal_log<50)
            {
            internal_log=50;
            cerr<<"The internal log to short. Reset to a default minimum ="<<internal_log<<endl;
            }
    }
    else
    //cerr<<"\tRSPC=N/Y or 1..WIDTH - Random calculation of spatial correlation ("<<(spatial_correlation_mode==0?"N":"Y")<<")\n";
    if((pom=strstr(rob,"RSPC="))!=nullptr) 
    {
        const char* l_pom= pom + 5;

        if(toupper(*l_pom) == 'N')
            spatial_correlation_mode=0;
        else if(toupper(*l_pom) == 'Y')
            spatial_correlation_mode=16;
        else
            spatial_correlation_mode=atoi(l_pom);

        cerr<<"Random calculation of spatial correlation is "<<
                (spatial_correlation_mode==0?"d i s a b l e d":"e n a b l e d")
                <<". Multiplication="<<spatial_correlation_mode<<"\n";
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
    cerr<<"Map of attitudes from file \""<<MapLName<<"\"\n";
    }
    else
    if((pom=strstr(rob,"MAPP="))!=nullptr) //Nie nullptr, czyli jest
    {
    strcpy(MapPName,pom+5);
    cerr<<"Map of individual power from file \""<<MapPName<<"\"\n";
    }
    else
    if((pom=strstr(rob,"MASK="))!=nullptr) //Nie nullptr, czyli jest
    {
    strcpy(MaskName,pom+5);
    cerr<<"The mask for live agents from file \""<<MaskName<<"\"\n";
    }
    else
    if((pom=strstr(rob,"HIST="))!=nullptr) //Nie nullptr, czyli jest
    {
    strcpy(HistName,pom+5);
    cerr<<"The history of the simulation will be saved to \""<<HistName<<"\"\n";
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
        cout<<"\tMAPL=initL.gif (or BMP)- file with an initialization map of attitudes (RANDOM)\n";
        cout<<"\tMAPP=initP.gif (or BMP)- file with an initialization map of powers (RANDOM)\n";
        cout<<"\tMASK=mask.gif	(or BMP)- mask file for alive (not black) agents (ALL ALIVE)\n";

        cout<<"\tSYNC=Y/N - synchronic (Y) or Monte-Carlo simulation mode ("<<(TypSymulacji==0?"Yes":"No")<<")\n";
        cout << "\tNFOC=0..inf - need for closure  [meaning depend on a model](" << NeedForClosure << ")\n";
        cout<<"\tSELF=0..1 - use self for calculations ("<<(WagaSiebie)<<")\n";

        cout<<"\tWIDTH=NN - matrix size ("<<iWidth<<")\n";
        cout << "\tCLSS=NN - number of class. Must be power of 2. (" << LiczbaKlas << ")\n";
        cout<<"\tFILL=0.001..1 - ratio of live agents ("<<Fill<<")\n";	
        cout<<"\tBALA=0.001..0.999 - part of society with \"white\" attitude ("<<MajorMinor<<")\n"; 
        cout<<"\tPMIG=0.001..1 - probability of migration under pressure ("<<ProbMig<<")\n";	
        cout<<"\tMPOW=NN - max strength for initialization ("<<MaksymalnaSila<<")\n"	;
        cout<<"\tWPOW=N	- walking step of strength	("<<RuchomaSila<<")\n";
        cout<<"\tTRSP=N - % of threshold of strength ("<<TresProcent<<")\n";
        cout << "\tPRTR=2..WIDTH^2-1 - number of interaction partners (-1 = all in a neighborhood) (" << NumberOfNeighbors << ")\n";
        cout << "\tINDI=1..WIDTH/2-1 - interaction distance (" << NeighborhoodRadius << ")\n";

        cout<<"\tNOIP=NN - percent of noise ("<<ProcentSzumu<<")\n";
        cout<<"\tSPCH=NN - percent of spontaneity change of attitudes ("<<MutacjeSpon*100<<")\n";
        cout<<"\tMAX=NNNN - max simulation step ("<<iMaxIterations<<")\n";

        cout<<"\tRSPC=N/Y or 1..WIDTH - Random calculation of spatial correlation ("<<(spatial_correlation_mode?"N":"Y")<<")\n";
        cout<<"\tILOG=NNNN - length of internal statistic logs ("<<internal_log<<")\n";
        cout<<"\tSTOP=N/Y - exit after MAX steps ("<<(iWychodzenie?"Yes":"No")<<")\n";
        cout<<"\tVIEV=N - visualisation frequency ("<<iViewRatio<<")\n";
        cout<<"\tLOGC=N - log file saving frequency ("<<iLogRatio<<")\n";
        cout<<"\tLOGF=name.log - file for simulation log ("<<LogName<<")\n";
        cout<<"\tHIST=hist.otx - file for full history of simulation.\n";
        cout << "\tWIDTHWIN=YYY,HEIGHTWIN=XXX - initial window size.(" << SCR_WIDTH << 'x' << SCR_HEIGHT << "\n";
        cout<<"\nAUTO=XXX - number of auto-repetition of simulation.("<<AUTOSTART<<")\n";
        cout<<flush;
    return 0;
    }
    }
return 1;
}


/*  OGÓLNA FUNKCJA MAIN  */
/* ********************* */

int main(const int argc,const char* argv[])
{
cout<<endl<<WINDOW_HEADER<<", compilation: "<<__DATE__<<' '<<__TIME__<<endl;
cout<<"Programmed by W. Borkowski from Warsaw University"<<endl;
cout<<"==================================================================="<<endl;
cout.flush();

if(!parse_options(argc,argv))
        exit(1);

main_area_manager Lufciki(24, SCR_WIDTH, SCR_HEIGHT, 28);
if(!Lufciki.start(WINDOW_HEADER,argc,argv,1))
    {
    cerr<<"Can't initialize graphics"<<endl;
    exit(1);
    }

//Utworzenie sensownej nazwy pliku(-ów) dla zrzutów ekranu
{
wb_pchar buf(strlen(SIMULATION_NAME) + 20);
buf.prn("%s_%ld", SIMULATION_NAME, time(nullptr));
Lufciki.set_dump_name(buf.get());
}


//INICJALIZACJA SYMULACJI
aworld& theWorld=*new aworld(iWidth,
                             LogName,
                             MapLName,
                             MapPName,
                             MaskName,
                           ProcentSzumu/100.0, //Szum od 0-1
                           MaksymalnaSila, //Żeby była w przedziale
                           LiczbaKlas,
                             NeighborhoodRadius,
                             NumberOfNeighbors,
                             WagaSiebie,
                             NeedForClosure,
                             (TypSymulacji==0?true:false), //Synchroniczna czy nie
                           RuchomaSila,
                             MaksymalnaSila*TresProcent/100.0,
                             MutacjeSpon,
                             Fill,
                             ProbMig,
                             MajorMinor
                           );

//if(&theWorld == nullptr) //To się już nie może zdarzyć od C++11
//    {
//    cerr<<"Can't allocate simulation world!\n"<<endl;
//    exit(1);
//    }


//INICJALIZACJA
RANDOMIZE() //inicjalizacja globalnego randomizer-a (przez makro)
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
    theWorld.read_loop(iWychodzenie);
}
else
{
    theWorld.initialize(&Lufciki); //inicjalizacja wizualizacji i warstw symulacji
    cout<<WINDOW_HEADER<<": INITIALISED."<<endl;
    if(!AUTOSTART)
    {
        Lufciki.process_input(); //Pierwsze zdarzenia. Kończą się po ctrl-B
        //GŁÓWNA PĘTLA SYMULACJI:
        cout<<WINDOW_HEADER<<": STARTED."<<endl;
        theWorld.simulation_loop(iWychodzenie);
    }
    else
    {
        int statusWin=Lufciki.search("STATUS");
        Lufciki.maximize(statusWin);
        for(int symulacja=0;symulacja<AUTOSTART;symulacja++)
            {
            //GŁÓWNA PĘTLA SYMULACJI:
            cout<<WINDOW_HEADER<<": SIMULATION "<<symulacja<<" STARTED."<<endl;
            theWorld.simulation_loop(1);
            cout<<WINDOW_HEADER<<": SIMULATION "<<symulacja<<" DONE."<<endl;
            if(symulacja<AUTOSTART-1)
                {
                //Re-inicjalizacja:
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


/* STATIC ALLOCATION */
//unsigned agent::max=0; //jaki jest największy taxon


/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */

