/// @file
/// Wariacje na temat life Conway'a — program główny.
// ////////////////////////////////////////////////////
/// @date 2026-03-26 (modified)

const char* WINDOW_HEADER="RAND-LIFE version 1.01c (by Wojciech Borkowski)";
const char* SYMULATION_NAME="randlife_v1.01c";

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <compatyb.h>
#include "lrand.h"
#include "lworld.h"
//#include "wbminmax.hpp"

unsigned SWIDTH=750;
unsigned SHEIGHT=550;

unsigned internal_log=7000; //Nieobiektowo przekazywane do metody inicializacji źródeł
char  LogName[512]="randlife.log\0-------------------+--";
char HistName[512]="\0--+---------randlife.otx----------";
char MapLName[512]="\0--+---------randlife.gif----------";

unsigned iWidth=50;
unsigned iMaxIterations=0xffffffff;
unsigned iLogRatio=1;
unsigned iViewRatio=1;


int  IloscKlas=2;
int  ProcentSzumu=10;
double MutacjeSpon=0;
int  RozmiarSasiedztwa=1; //3x3-1
int  IleSasiadow=-1; //Wszyscy sąsiedzi — nielosowo. 8. - losowo!!!
bool TypSymulacji=0; //Synchroniczna

int  iWychodzenie=0;
int  Replay=0;
int	 AUTOSTART=0;


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
	MutacjeSpon=atof(pom+5);
    if(MutacjeSpon<0 || MutacjeSpon>100)
		{
		cerr<<"Bad SPCH ="<<MutacjeSpon<<" (must be in <0,100> )"<<endl;
		return 0;
		}
	cerr<<"SPCH (spn. change percent) = "<<MutacjeSpon<<endl;
	MutacjeSpon/=100; //Ułamek, a nie procent tak naprawdę
	}
    else
	if((pom=strstr(rob,"NOIP="))!=nullptr) //Nie nullptr, czyli jest
	{
	ProcentSzumu=atol(pom+5);
    if(ProcentSzumu<0 || ProcentSzumu>100)
		{
		cerr<<"Bad NOIP ="<<ProcentSzumu<<" (must be in <0,100> )"<<endl;
		return 0;
		}
	cerr<<"NOIP (noise percent) = "<<ProcentSzumu<<endl;
	}
	/*
    else
	if((pom=strstr(rob,"CLSS="))!=nullptr) //Nie nullptr czyli jest
	{
	IloscKlas=atol(pom+5);
    if(IloscKlas<2)
		{
		cerr<<"Bad CLSS ="<<IloscKlas<<" (must be >2 )"<<endl;
		return 0;
		}
	}*/
    else
    if((pom=strstr(rob,"WIDTH="))!=nullptr) //Nie nullptr, czyli jest
	{
	iWidth=atol(pom+6);
    if(iWidth<3 || iWidth>=SWIDTH)
		{
		cerr<<"Bad WIDTH = "<<iWidth<<"(must be in <3,"<<SWIDTH<<">"<<endl;
		return 0;
		}
	}
	else
    if((pom=strstr(rob,"WIDTHWIN="))!=nullptr) //Nie nullptr, czyli jest
	{
	SWIDTH=atol(pom+9);
    if(SWIDTH<50)
		{
		cerr<<"Bad WIDTHWIN = "<<SWIDTH<<" (must be >50)"<<endl;
		return 0;
		}
	}
	else
	if((pom=strstr(rob,"HEIGHTWIN="))!=nullptr) //Nie nullptr, czyli jest
	{
	SHEIGHT=atol(pom+10);
    if(SHEIGHT<50)
		{
		cerr<<"Bad HEIGHTWIN = "<<SHEIGHT<<" (must be >50)"<<endl;
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
		cerr<<"Bad VIEW (visualisation frequency). Must be >0"<<endl;
		return 0;
        }
    }    
    else
	if((pom=strstr(rob,"INDI="))!=nullptr) //Nie nullptr, czyli jest
    {
    RozmiarSasiedztwa=atol(pom+5);
    if(RozmiarSasiedztwa>=1 && RozmiarSasiedztwa<iWidth/2-1)
		{
		cerr<<"INDI="<<RozmiarSasiedztwa<<endl;;
		}
		else
		{
		cerr<<"Bad INDI="<<RozmiarSasiedztwa<<" Must from 1 to "<<iWidth/2-1<<endl;
		return 0;
        }
    }
    else		
	if((pom=strstr(rob,"PRTR="))!=nullptr) //Nie nullptr, czyli jest
		{
			IleSasiadow=atol(pom+5);
			if(IleSasiadow==-1)
			{
				cerr<<"PRTR = all"<<endl;
			}
			else
				if(IleSasiadow>1 && IleSasiadow<=sqr(RozmiarSasiedztwa*2+1)-1)
				{
					cerr<<"PRTR="<<IleSasiadow<<endl;
				}
				else
				{
					cerr<<"Bad PRTR="<<IleSasiadow
						<<" Must from 2 to "<<sqr(RozmiarSasiedztwa*2+1)-1<<endl;
					return 0;
				}
		}
	else
	if((pom=strstr(rob,"AUTO="))!=nullptr) //Nie nullptr, czyli jest
    {
    AUTOSTART=atol(pom+5);
	cerr<<"AUTO="<<AUTOSTART<<endl;
	if(AUTOSTART)
		{
		iWychodzenie=1;
		cerr<<"STOP="<<(iWychodzenie?"Yes":"No")<<endl;
		}
    }
	else
	if((pom=strstr(rob,"STOP="))!=nullptr) //Nie nullptr, czyli jest
    {
    iWychodzenie=(toupper(pom[5])=='Y');
	cerr<<"STOP="<<(iWychodzenie?"Yes":"No")<<endl;
    }
    else  //SYNC
	if((pom=strstr(rob,"SYNC="))!=nullptr) //Nie nullptr, czyli jest
    {
    TypSymulacji=!(toupper(pom[5])=='Y');
	cerr<<"SYNC="<<(TypSymulacji==0?"Yes":"No")<<endl;
    }
    else
	if((pom=strstr(rob,"ILOG="))!=nullptr) //Nie nullptr, czyli jest
    {
    internal_log=atoi(pom+5);
	if(internal_log<50)
			{
			internal_log=50;
			cerr<<"Internal log to short. Reset to default minimum ="<<internal_log<<endl;
			}
    }
	else
	if((pom=strstr(rob,"LOGF="))!=nullptr) //Nie nullptr, czyli jest
    {
    strcpy(LogName,pom+5);
    }else
    if((pom=strstr(rob,"MAPL="))!=nullptr) //Nie nullptr, czyli jest
    {
    strcpy(MapLName,pom+5);
	cerr<<"Map of randlifes from file \""<<MapLName<<"\"\n";
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
		cerr<<"Unknown parameter \""<<argv[i]<<"\"\n";
		cerr<<"YOU CAN USE:\n";
		cerr<<" REPL=hist.otx - not simulate, but replay simulation history file.\n";
		cerr<<" MAPL=initL.gif (or BMP)- file with an initialization map of randlifes (RANDOM)\n";
		cerr<<" WIDTH=NN - matrix size ("<<iWidth<<")\n";
//		cerr<<" CLSS=NN - number of class. Must be power of 2. ("<<IloscKlas<<")\n";
		cerr<<" SYNC=Y/N - synchronic (Y) or Monte-Carlo simulation mode ("<<(TypSymulacji==0?"Yes":"No")<<")\n";
		cerr<<" PRTR=2..WIDTH^2-1 - number of interaction partners (-1 = all neighbourhood) ("<<IleSasiadow<<")\n";
		cerr<<" INDI=1..WIDTH/2-1 - interaction distance ("<<RozmiarSasiedztwa<<")\n";
		cerr<<" NOIP=NN - percent of initially life cells. ("<<ProcentSzumu<<")\n";
	//	cerr<<" SPCH=NN - percent of spontanic change of randlifes ("<<MutacjeSpon*100<<")\n";
		cerr<<" MAX=NNNN - max simulation step ("<<iMaxIterations<<")\n";
		cerr<<" ILOG=NNNN - lenght of internal statistic logs ("<<internal_log<<")\n";
		cerr<<" STOP=N/Y - exit after MAX steps ("<<(iWychodzenie?"Yes":"No")<<")\n";
		cerr<<" VIEV=N - visualisation frequency ("<<iViewRatio<<")\n";
		cerr<<" LOGC=N - log file saving frequency ("<<iLogRatio<<")\n";
		cerr<<" LOGF=name.log - file for simulation log ("<<LogName<<")\n";
		cerr<<" HIST=hist.otx - file for full history of simulation.\n";
		cerr<<" WIDTHWIN,HEIGHTWIN=XXX - initial window size.("<<SWIDTH<<'x'<<SHEIGHT<<"\n";
		cerr<<" AUTO=XXX - number of auto-repetition of simulation.("<<AUTOSTART<<")\n";
	return 0;
	}
    }
return 1;
}


/* OGÓLNA FUNKCJA MAIN */
/* ******************* */

int main(const int argc,const char* argv[])
{
cout<<WINDOW_HEADER<<", compilation: "<<__DATE__<<' '<<__TIME__<<endl;

if(!parse_options(argc,argv))
        exit(1);

main_area_menager Lufciki(24,SWIDTH,SHEIGHT,28);
if(!Lufciki.start(WINDOW_HEADER,argc,argv,1))
	{
	cerr<<"Can't initialize graphics"<<endl;
	exit(1);
	}

//INICJALIZACJA SYMULACJI
lifeworld& tenSwiat=*new lifeworld(
                           iWidth,
						   LogName,
						   MapLName,
						   ProcentSzumu/100.0,//Szum od 0-1
						   IloscKlas,
						   RozmiarSasiedztwa,
						   IleSasiadow,
						   (TypSymulacji==0?true:false), //Synchroniczna czy nie
						   MutacjeSpon
						   );

if(&tenSwiat==nullptr)
    {
    cerr<<"Can't allocate the simulation world!\n"<<endl;
    exit(1);
    }

//INICJALIZACJA
RANDOMIZE(); //inicjalizacja globalnego randomizer-a
tenSwiat.set_max_iteration(iMaxIterations); //Ile najwięcej kroków
tenSwiat.set_input_ratio(iViewRatio);
tenSwiat.set_log_ratio(iLogRatio);
cout<<WINDOW_HEADER<<": LOADED."<<endl;
tenSwiat.set_history_stream(HistName);

if(Replay)
{	
	tenSwiat.initialize(&Lufciki,1); //inicjalizacja wizualizacji
	cout<<WINDOW_HEADER<<": PREPARED FOR READING. WAITING!"<<endl;
	Lufciki.process_input(); //Pierwsze zdarzenia. Kończą się po ctrl-B
	tenSwiat.read_loop(iWychodzenie);
}
else
{
	tenSwiat.initialize(&Lufciki); //inicjalizacja wizualizacji i warstw symulacji
	cout<<WINDOW_HEADER<<": INITIALISED."<<endl;
	if(!AUTOSTART)
	{
		Lufciki.process_input(); //Pierwsze zdarzenia. Kończą się po ctrl-B
		//GŁÓWNA PĘTLA SYMULACJI
		cout<<WINDOW_HEADER<<": STARTED."<<endl;
		tenSwiat.simulation_loop(iWychodzenie);
	}
	else
	{
		int statusWin=Lufciki.search("STATUS");
		Lufciki.maximize(statusWin);
		for(int symulacja=0;symulacja<AUTOSTART;symulacja++)
			{
            //GŁÓWNA PĘTLA SYMULACJI
			cout<<WINDOW_HEADER<<": SIMULATION "<<symulacja<<" STARTED."<<endl;
			tenSwiat.simulation_loop(1);
			cout<<WINDOW_HEADER<<": SIMULATION "<<symulacja<<" DONE."<<endl;
			if(symulacja<AUTOSTART-1)
				{
				//Reinicjalizacja
				tenSwiat.restart();
				}
			}
	}
	
}

cout<<WINDOW_HEADER<<": CLOSING."<<endl;

cout.flush();

delete &tenSwiat; //Dealokacja świata wraz ze wszystkimi składowymi
cout<<"----------> See you later!!! <--------------\n"<<endl<<flush;
return 0;
}


/* STATIC ALLOCATION */
//unsigned agent::max=0; //jaki jest największy taxon

// /////////////////////////////////////////////
// Example for SYMSHELL CLASSES library.
/// @author Wojciech Borkowski, iss.uw.edu.pl
// https://github.com/borkowsk/symShell2andRTM
/// @copyright 2000 - 2026
// /////////////////////////////////////////////
