/// @file
/// @brief
///  @EN{ Variations on Conway's Life — main Program. }
///  @PL{ Wariacje na temat life Conway-a — program główny. }
/// @date 2026-05-30 (modified)
///       =========================================================
/// @details ....
//======================================================================================================================

const char* WINDOW_HEADER="RAND-LIFE version 1.01d (by Wojciech Borkowski)";
const char* SIMULATION_NAME="rand_life_v1.01d";

#include <cstdlib>
#include <iostream>

#include "compatyb.h"
#include "lrand.h"
#include "lworld.h"

#pragma ide diagnostic push
#pragma ide diagnostic ignored "cert-err34-c"

using namespace sym2;

/// @name @PL{ Rozmiar okna i bok świata symulacji. } @EN{ Window size and side of the simulation world. }
/// @{
unsigned short SCR_WIDTH=750;
unsigned short SCR_HEIGHT=550;
unsigned short WorldWidth=50;  ///< @brief @PL{ Bok świata. } @EN{ Side of the world. }
/// @}

constexpr bool SYNCHRONOUSLY=true;

/// @name @PL{ Parametry modelu. } @EN{ Model parametrów. }
/// @{
const short NofCategories=2;  ///< @brief @PL{ Zawsze 2. W Life Conway-a nie może być więcej. } @EN{ For Conways Life always 2. }
double          MutationP=0;  ///< @brief @PL{ Ewentualne losowe zmiany stanu. } @EN{ Possible random state changes. }
short       NeighborhoodR=1;  ///< @brief @PL{ Promień sąsiedztwa (1 oznacza 3 × 3 - 1) } @EN{ Neighborhood radius. }
short       NeighDensity=-1;  ///< @brief @PL{ Gdy -1 to wszyscy sąsiedzi — nielosowo. Gdy 8. to losowo!!! }
                              ///<        @EN{ When -1, all neighbors — not random. When 8, then random!!! }
bool           UpdateType=SYNCHRONOUSLY; ///< @brief @PL{ Symulacja synchroniczna (chyba żeby nie, wtedy Monte Carlo). }
                                         ///<       @EN{ Synchronous simulation (unless, of course, Monte Carlo then). }
/// @}

/// @name @PL{ Różne parametry aplikacji. } @EN{ Various application parameters. }
/// @{
char  LogName[512]="rand_life.log\0-------------------+--";
char HistName[512]="\0--+---------rand_life.otx----------";
char MapLName[512]="\0--+---------rand_life.gif----------";

unsigned iMaxIterations=0xffffffff; ///< @brief @PL{ Maksymalna liczba kroków pojedynczej symulacji. } @EN{ Maximum number of steps for a single simulation. }
unsigned iViewRatio=1;      ///< @brief @PL{ Częstość wizualizacji względem symulacji. } @EN{ Visualization rate vs. simulation. }
unsigned iLogRatio=1;       ///< @brief @PL{ Częstość zapisu do logu. } @EN{ Log writing frequency. }
unsigned internal_log=7000; ///< @PL{ Długość historii. @note Nieobiektowo przekazywane do metody inicjalizacji źródeł. } @EN{ History length. @note Non-object passed to the sources initialization method. }

int  AUTO_EXIT=0;
int	 AUTO_START=0;
int  Replay=0; ///< @brief @PL{ Odtwarzanie zapisanej symulacji zamiast symulowania. } @EN{ Playing a saved simulation instead of simulating. }
/// @}

/// @brief @PL{ Analiza parametrów wywołania. } @EN{ Analysis of call parameters. }
int parse_options(const int argc,const char* argv[])
{
    for(int i=1;i<argc;i++)
    {
        if( *argv[i]=='-' ) /* Opcja X lub symshell-a */
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
            MutationP=atof(pom + 5);
            if(MutationP < 0 || MutationP > 100)
            {
                cerr << "Bad SPCH =" << MutationP << " (must be in <0,100>)" << endl;
                return 0;
            }
            cerr << "SPCH (spn. change percent) = " << MutationP << endl;
            MutationP/=100; //Ułamek, a nie procent tak naprawdę
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
        if((pom=strstr(rob,"INDI="))!=nullptr) //Nie nullptr, czyli jest
        {
            int tmpRozmiar=atoi(pom+5);
            if(tmpRozmiar>=1 && tmpRozmiar < WorldWidth / 2 - 1)
                {
                    NeighborhoodR=(short)tmpRozmiar;
                cerr << "INDI=" << NeighborhoodR << endl;
                }
            else
                {
                cerr << "Bad INDI=" << tmpRozmiar << " Must from 1 to " << WorldWidth / 2 - 1 << endl;
                return 0;
                }
        }
        else
        if((pom=strstr(rob,"PRTR="))!=nullptr) //Nie nullptr, czyli jest
            {
                int IluZOtocz=atoi(pom + 5);
                NeighDensity=-1;
                if(IluZOtocz == -1)
                {
                    cerr<<"PRTR = all"<<endl; //To, co domyślnie.
                }
                else
                    if(IluZOtocz > 1 && IluZOtocz <= sqr(NeighborhoodR * 2 + 1) - 1)
                    {
                        cerr << "PRTR=" << IluZOtocz << endl;
                        NeighDensity=(short)IluZOtocz;
                    }
                    else
                    {
                        cerr << "Bad PRTR=" << IluZOtocz
                             << " (Must be from 2 to " << sqr(NeighborhoodR * 2 + 1) - 1 << ")" << endl;
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
                    AUTO_EXIT=1; //TODO ZAWSZE???
                //cerr<<"STOP="<<(AUTO_EXIT?"Yes":"No")<<endl;
                cerr<<"STOP=Yes !!!"<<endl;
                }
        }
        else
        if((pom=strstr(rob,"STOP="))!=nullptr) //Nie nullptr, czyli jest
        {
            AUTO_EXIT=(toupper(pom[5]) == 'Y');
            cerr << "STOP=" << (AUTO_EXIT?"Yes":"No") << endl;
        }
        else  //SYNC
        if((pom=strstr(rob,"SYNC="))!=nullptr) //Nie nullptr, czyli jest
        {
            UpdateType= toupper(pom[5]) != 'Y';
            cerr << "SYNC=" << (UpdateType == 0?"Yes":"No") << endl;
        }
        else
        if((pom=strstr(rob,"ILOG="))!=nullptr) //Nie nullptr, czyli jest
        {
            internal_log=atoi(pom+5);
            if(internal_log<50)
                    {
                    internal_log=50;
                    cerr<<"The internal log value is too small. Reset to the default minimum ="<<internal_log<<endl;
                    }
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
            cerr<<"Map of randlifes from file \""<<MapLName<<"\"\n";
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
            cerr<<"Unknown parameter \""<<argv[i]<<"\"\n";
            cerr<<"YOU CAN USE:\n";
            cerr<<" REPL=hist.otx - not simulate, but replay simulation history file.\n";
            cerr<<" MAPL=initL.gif (or BMP)- file with an initialization map of randlifes (RANDOM)\n";
            cerr << " WIDTH=NN - matrix size (" << WorldWidth << ")\n";
            // cerr<<" CLSS=NN - number of class. Must be power of 2. ("<<NofCategories<<")\n";
            cerr << " SYNC=Y/N - synchronic (Y) or Monte-Carlo simulation mode (" << (UpdateType == 0?"Yes":"No") << ")\n";
            cerr << " PRTR=2...WIDTH^2-1 - number of interaction partners (-1 = all neighbourhood) (" << NeighDensity << ")\n";
            cerr << " INDI=1...WIDTH/2-1 - interaction distance (" << NeighborhoodR << ")\n";
            // cerr<<" SPCH=NN - percentage of spontaneous changes of classes ("<<MutationP*100<<")\n";
            cerr<<" MAX=NNNN - max simulation step ("<<iMaxIterations<<")\n";
            cerr<<" ILOG=NNNN - length of internal statistic logs ("<<internal_log<<")\n";
            cerr << " STOP=N/Y - exit after MAX steps (" << (AUTO_EXIT?"Yes":"No") << ")\n";
            cerr<<" VIEV=N - visualisation frequency ("<<iViewRatio<<")\n";
            cerr<<" LOGC=N - log file saving frequency ("<<iLogRatio<<")\n";
            cerr<<" LOGF=name.log - file for simulation log ("<<LogName<<")\n";
            cerr<<" HIST=hist.otx - file for full history of simulation.\n";
            cerr << " WIDTHWIN,HEIGHTWIN=XXX - initial window size.(" << SCR_WIDTH << 'x' << SCR_HEIGHT << "\n";
            cerr << " AUTO=XXX - number of auto-repetition of simulation.(" << AUTO_START << ")\n";
        return 0;
        }
    }
    return 1;
}


/** @brief @PL{ OGÓLNA FUNKCJA MAIN. } @EN{ GENERAL MAIN FUNCTION. } */
/* ***************************************************************** */

int main(const int argc,const char* argv[])
{
    cout<<WINDOW_HEADER<<", compilation: "<<__DATE__<<' '<<__TIME__<<endl;

    if(!parse_options(argc,argv))
            exit(1);

    main_area_manager Lufciki(24, SCR_WIDTH, SCR_HEIGHT, 28);
    if(!Lufciki.start(WINDOW_HEADER,argc,argv,1))
        {
        cerr<<"Can't initialize graphics"<<endl;
        exit(1);
        }

    //INICJALIZACJA SYMULACJI
    life_world& theWorld=*new life_world(
            WorldWidth,
            LogName,
            MapLName,
            NeighborhoodR,
            NeighDensity,
            UpdateType, //Synchroniczna czy nie
                                MutationP
                               );

//    if(&theWorld==nullptr) //OD C++11 nie ma możliwości żeby `new` zwróciło `nullptr`
//        {
//        cerr<<"Can't allocate the simulation world!\n"<<endl;
//        exit(1);
//        }

    //INICJALIZACJA:
    RANDOMIZE() //; //inicjalizacja globalnego randomizer-a
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
        theWorld.read_loop(AUTO_EXIT);
    }
    else
    {
        theWorld.initialize(&Lufciki); //inicjalizacja wizualizacji i warstw symulacji
        cout<<WINDOW_HEADER<<": INITIALISED."<<endl;
        if(!AUTO_START)
        {
            Lufciki.process_input(); //Pierwsze zdarzenia. Kończą się po ctrl-B
            //GŁÓWNA PĘTLA SYMULACJI
            cout<<WINDOW_HEADER<<": STARTED."<<endl;
            theWorld.simulation_loop(AUTO_EXIT);
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


/* STATIC ALLOCATION */
//unsigned agent::max=0; //jaki jest największy taxon

#pragma ide diagnostic pop
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

