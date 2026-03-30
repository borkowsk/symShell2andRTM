//Dosyc prosta symulacja zmiany pogladow - attitude
//Uzyskana przez uproszczenie programu LANGUAGES
/////////////////////////////////////////////////////////////////
const char* WINDOW_HEADER="CONVINCE version 0.01a";
const char* SYMULATION_NAME="convinces_v0.01";

#include <stdlib.h>
#include "platform.hpp"
#ifdef NEW_FASHION_CPP
#include <iostream>
#else
#include <iostream.h>
#endif
#include "wbminmax.hpp"
#include "crand.h"
#include "cworld.h"

unsigned iWidth=50;
double	 NewProbability=0.01;
double	 InfectionProb=0.5;
double	 SupportLev=0.9;

char  LogName[512]="attitude.log\0-------------------+--";
char HistName[512]="\0--+---------convinces.otx----------";
char MapLName[512]="\0--+---------convinces.gif----------";
char MapPName[512]="\0--+---------powers.gif------------";
char MaskName[512]="\0--+---------mask.gif--------------";


unsigned SWIDTH=750;
unsigned SHEIGHT=550;
unsigned internal_log=7000; //Nieobiektowo przekazywane do metody inicializacji zrodel 
unsigned iLogRatio=1;
unsigned iViewRatio=1;
unsigned iMaxIterations=0xffffffff;

//int  RuchomaSila=0;			//Czy sila ma sie powiekrzac "z wiekiem"
int  MaksymalnaSila=1;		//Jaka najwieksza sila - integer wiec jak 1 to plasko
int  MinimalnaSila=1;		//Nie moze byc 0! CHYBA?
//char TresProcent=100;		//Powyzej jakiej sily zmiany "pogladu" sa juz niemozliwe

int  IloscKlas=2;
//int  ProcentSzumu=0;
//double MutacjeSpon=0;
//int  RozmiarSasiedztwa=1;//3x3-1
//int  IleSasiadow=-1;//Wszyscy sasiedzi - nielosowo. 8 - losowo!!!
//int  BranieSiebie=1;//8+1
//bool TypSymulacji=0;//Synchroniczna

int	 AUTOSTART=0;
int  iWychodzenie=0;//Techniczne bardzo
int  Replay=0;		//Czy odtarzanie z pliku zamiast symulacji?

int parse_options(const int argc,const char* argv[])
{
for(int i=1;i<argc;i++)
    {
    if( *argv[i]=='-' ) /* Opcja X lub symshella */
		continue;
	//Uppercasing
	wb_pchar hand( clone_str(argv[i]) );
	char*    rob=hand.get_ptr_val();
	char*    pom=strchr(rob,'=');
	if(pom==NULL) 
			goto ERROR; //NA PEWNO ZLE
	*pom='\0';
	_strupr(rob);
	*pom='=';
	/*
	if((pom=strstr(rob,"SPCH="))!=NULL) //Nie NULL czyli jest
	{
	MutacjeSpon=atof(pom+5);
    if(MutacjeSpon<0 || MutacjeSpon>100)
		{
		cerr<<"Bad SPCH ="<<MutacjeSpon<<" (must be in <0,100> )"<<endl;
		return 0;
		}
	cerr<<"SPCH (spn. change percent) = "<<MutacjeSpon<<endl;
	MutacjeSpon/=100;//Ulamek a nie procent tak naprawde
	}
    else
	if((pom=strstr(rob,"NOIP="))!=NULL) //Nie NULL czyli jest
	{
	ProcentSzumu=atol(pom+5);
    if(ProcentSzumu<0 || ProcentSzumu>100)
		{
		cerr<<"Bad NOIP ="<<ProcentSzumu<<" (must be in <0,100> )"<<endl;
		return 0;
		}
	cerr<<"NOIP (noise percent) = "<<ProcentSzumu<<endl;
	}
    else */
	if((pom=strstr(rob,"CLSS="))!=NULL) //Nie NULL czyli jest
	{
	IloscKlas=atol(pom+5);
    if(IloscKlas<2)
		{
		cerr<<"Bad CLSS ="<<IloscKlas<<" (must be >2 )"<<endl;
		return 0;
		}
	}
    else
	if((pom=strstr(rob,"MPOW="))!=NULL) //Nie NULL czyli jest
	{
	MaksymalnaSila=atol(pom+5);
    if(MaksymalnaSila<0)//0 czy 1???
		{
		cerr<<"Bad MPOW ="<<MaksymalnaSila<<" (must be >=1 )"<<endl;
		return 0;
		}
	}
    else /*
	if((pom=strstr(rob,"WPOW="))!=NULL) //Nie NULL czyli jest
	{
	RuchomaSila=atol(pom+5);
    if(RuchomaSila<0)
		{
		cerr<<"Bad WPOW ="<<RuchomaSila<<" (must be >=0 )"<<endl;
		return 0;
		}
	cerr<<"WPOW="<<RuchomaSila<<endl;
	}
	else
    if((pom=strstr(rob,"TRSP="))!=NULL) //Nie NULL czyli jest
	{
	TresProcent=atol(pom+5);
    if(TresProcent<0 || TresProcent>100)
		{
		cerr<<"Bad TRSP = "<<int(TresProcent)<<"(must be in <0,100>"<<endl;
		return 0;
		}
		else
		{
		cerr<<"Treshold of strenght for change parameters = "<<int(TresProcent)<<"%"<<endl;
		if(RuchomaSila==0)//Nie ma sensu TRSP jesli nie jest ruchoma sila
			{
			RuchomaSila=1;
			cerr<<"Automatically set WPOW to "<<RuchomaSila<<endl;
			}
		}
	}
	else */
    if((pom=strstr(rob,"WIDTH="))!=NULL) //Nie NULL czyli jest
	{
	iWidth=atol(pom+6);
    if(iWidth<3 || iWidth>=SWIDTH)
		{
		cerr<<"Bad WIDTH = "<<iWidth<<"(must be in <3,"<<SWIDTH<<">"<<endl;
		return 0;
		}
	}
	else
    if((pom=strstr(rob,"WIDTHWIN="))!=NULL) //Nie NULL czyli jest
	{
	SWIDTH=atol(pom+9);
    if(SWIDTH<50)
		{
		cerr<<"Bad WIDTHWIN = "<<SWIDTH<<" (must be >50)"<<endl;
		return 0;
		}
	}
	else
	if((pom=strstr(rob,"HEIGHTWIN="))!=NULL) //Nie NULL czyli jest
	{
	SHEIGHT=atol(pom+10);
    if(SHEIGHT<50)
		{
		cerr<<"Bad HEIGHTWIN = "<<SHEIGHT<<" (must be >50)"<<endl;
		return 0;
		}
	}
    else
    if((pom=strstr(rob,"MAX="))!=NULL) //Nie NULL czyli jest
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
    if((pom=strstr(rob,"LOGC="))!=NULL) //Nie NULL czyli jest
    {
    iLogRatio=atol(pom+5);
    if(iLogRatio<=0)
		{
		cerr<<"Bad LOGC (write to log frequency). Must be >0"<<endl;
		return 0;
        }
    }
	else
    if((pom=strstr(rob,"VIEW="))!=NULL) //Nie NULL czyli jest
    {
    iViewRatio=atol(pom+5);
    if(iViewRatio<=0)
		{
		cerr<<"Bad VIEW (visualisation frequency). Must be >0"<<endl;
		return 0;
        }
    }    
    else /*
	if((pom=strstr(rob,"SELF="))!=NULL) //Nie NULL czyli jest
    {
    BranieSiebie=(toupper(pom[5])=='Y');
	cerr<<"SELF="<<(BranieSiebie?"Yes":"No")<<endl;
    }
    else
	if((pom=strstr(rob,"INDI="))!=NULL) //Nie NULL czyli jest
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
	if((pom=strstr(rob,"PRTR="))!=NULL) //Nie NULL czyli jest
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
	else */
	if((pom=strstr(rob,"AUTO="))!=NULL) //Nie NULL czyli jest
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
	if((pom=strstr(rob,"STOP="))!=NULL) //Nie NULL czyli jest
    {
    iWychodzenie=(toupper(pom[5])=='Y');
	cerr<<"STOP="<<(iWychodzenie?"Yes":"No")<<endl;
    }
    else /* //SYNC
	if((pom=strstr(rob,"SYNC="))!=NULL) //Nie NULL czyli jest
    {
    TypSymulacji=!(toupper(pom[5])=='Y');
	cerr<<"SYNC="<<(TypSymulacji==0?"Yes":"No")<<endl;
    }
    else */
	if((pom=strstr(rob,"ILOG="))!=NULL) //Nie NULL czyli jest
    {
    internal_log=atoi(pom+5);
	if(internal_log<50)
			{
			internal_log=50;
			cerr<<"Internal log to short. Reset to default minimum ="<<internal_log<<endl;
			}
    }
	else
	if((pom=strstr(rob,"LOGF="))!=NULL) //Nie NULL czyli jest
    {
    strcpy(LogName,pom+5);
    }else
    if((pom=strstr(rob,"MAPL="))!=NULL) //Nie NULL czyli jest
    {
    strcpy(MapLName,pom+5);
	cerr<<"Map of attitudes from file \""<<MapLName<<"\"\n";
    }
	else
	if((pom=strstr(rob,"MAPP="))!=NULL) //Nie NULL czyli jest
    {
    strcpy(MapPName,pom+5);
	cerr<<"Map of individual power from file \""<<MapPName<<"\"\n";
    }
	else
	if((pom=strstr(rob,"MASK="))!=NULL) //Nie NULL czyli jest
    {
    strcpy(MaskName,pom+5);
	cerr<<"Mask for alive agents from file \""<<MaskName<<"\"\n";
    }	
	else
	if((pom=strstr(rob,"HIST="))!=NULL) //Nie NULL czyli jest
    {
    strcpy(HistName,pom+5);
	cerr<<"History of the symulation will be saved to \""<<HistName<<"\"\n";
    }
	else
	if((pom=strstr(rob,"REPL="))!=NULL) //Nie NULL czyli jest
    {
    strcpy(HistName,pom+5);
	Replay=1;
	cerr<<"The symulation will be replayed from \""<<HistName<<"\"\n";
    }
	else
	/* Ostatecznie wychodzi ze nie ma takiej opcji */
	{
ERROR:
		cerr<<"Unknown parameter \""<<argv[i]<<"\"\n";
		cerr<<"YOU CAN USE:\n";
		cerr<<"\tREPL=hist.otx - not symulate but replay symulation history file.\n";
		cerr<<"\tMAPL=initL.gif (or BMP)- file with initialization map of attitudes (RANDOM)\n";
		cerr<<"\tMAPP=initP.gif (or BMP)- file with initialization map of powers (RANDOM)\n";
		cerr<<"\tMASK=mask.gif	(or BMP)- mask file for alive (not black) agents (ALL ALIVE)\n";
		cerr<<"\tWIDTH=NN - matrix size ("<<iWidth<<")\n";
		cerr<<"\tCLSS=NN - number of class. Must be power of 2. ("<<IloscKlas<<")\n";
		cerr<<"\tMPOW=NN - max strength for initilization ("<<MaksymalnaSila<<")\n"	;
//		cerr<<"\tWPOW=N	- walking step of strenght	("<<RuchomaSila<<")\n";
//		cerr<<"\tTRSP=N - % of treshold of strenght ("<<TresProcent<<")\n";
//		cerr<<"\tSYNC=Y/N - synchronic (Y) or Monte-Carlo symulation mode ("<<(TypSymulacji==0?"Yes":"No")<<")\n";
//		cerr<<"\tPRTR=2..WIDTH^2-1 - number of interaction partners (-1 = all neighbourhood) ("<<IleSasiadow<<")\n";
//		cerr<<"\tINDI=1..WIDTH/2-1 - interaction distance ("<<RozmiarSasiedztwa<<")\n";
//		cerr<<"\tSELF=N/Y -use self for calculations ("<<(BranieSiebie?"Yes":"No")<<")\n";
//		cerr<<"\tNOIP=NN - percent of noise ("<<ProcentSzumu<<")\n";
//		cerr<<"\tSPCH=NN - percent of spontanic change of attitudes ("<<MutacjeSpon*100<<")\n";
		cerr<<"\tMAX=NNNN - max simulation step ("<<iMaxIterations<<")\n";
		cerr<<"\tILOG=NNNN - lenght of internal statistic logs ("<<internal_log<<")\n";
		cerr<<"\tSTOP=N/Y - exit after MAX steps ("<<(iWychodzenie?"Yes":"No")<<")\n";
		cerr<<"\tVIEV=N - visualisation frequency ("<<iViewRatio<<")\n";
		cerr<<"\tLOGC=N - log file saving frequency ("<<iLogRatio<<")\n";
		cerr<<"\tLOGF=name.log - file for simulation log ("<<LogName<<")\n";
		cerr<<"\tHIST=hist.otx - file for full history of symulation.\n";
		cerr<<"\tWIDTHWIN,HEIGHTWIN=XXX - initial window size.("<<SWIDTH<<'x'<<SHEIGHT<<"\n";
		cerr<<"\nAUTO=XXX - number of auto-repetition of symulation.("<<AUTOSTART<<")\n";
	return 0;
	}
    }
return 1;
}


/*  OGOLNA FUNKCJA MAIN */
/************************/

int main(const int argc,const char* argv[])
{
cout<<WINDOW_HEADER<<", compilation: "<<__DATE__<<' '<<__TIME__<<endl;
cout<<"Programmed by W.Borkowski for A.Nowak & D.Stauffer"<<endl;
cout<<"=========================================================="<<endl;
cout.flush();

if(!parse_options(argc,argv))
        exit(1);

main_area_menager Lufciki(24,SWIDTH,SHEIGHT,28);
if(!Lufciki.start(WINDOW_HEADER,argc,argv,1))
	{
	cerr<<"Can't initialize graphics"<<endl;
	exit(1);
	}

//INICJALIZACJA SYMULACJI
aWorld& tenSwiat=*new aWorld(iWidth,
							 NewProbability,
							 InfectionProb,
							 SupportLev,
						   LogName,
						   MapLName,
						   MapPName,
						   MaskName,
						   //ProcentSzumu/100.0,//Szum od 0-1
						   MaksymalnaSila,//Zeby byla w przedziale
						   MinimalnaSila
						   //IloscKlas,
						   //RozmiarSasiedztwa,
						   //IleSasiadow,
						   //BranieSiebie,
						   //(TypSymulacji==0?true:false),//Synchroniczna czy nie
						   //RuchomaSila,
						   //MaksymalnaSila*TresProcent/100.0,
						   //MutacjeSpon
						   );

if(&tenSwiat==NULL)
    {
    cerr<<"Can't allocate simulation world!\n"<<endl;
    exit(1);
    }

//INICJALIZACJA
RANDOMIZE(); //inicjalizacja globalnego randomizera 
tenSwiat.set_max_iteration(iMaxIterations);//Ile najwiecej krokow
tenSwiat.set_input_ratio(iViewRatio);
tenSwiat.set_log_ratio(iLogRatio);
cout<<WINDOW_HEADER<<": LOADED."<<endl;
tenSwiat.set_history_stream(HistName);

if(Replay)
{	
	tenSwiat.initialize(&Lufciki,1);//inicjalizacja wizualizacji
	cout<<WINDOW_HEADER<<": PREPARED FOR READING. WAITING!"<<endl;
	Lufciki.process_input();//Pierwsze zdazenia. Koncza sie po ctrl-B
	tenSwiat.read_loop(iWychodzenie);
}
else
{
	tenSwiat.initialize(&Lufciki);//inicjalizacja wizualizacji i warst symulacji
	cout<<WINDOW_HEADER<<": INITIALISED."<<endl;
	if(!AUTOSTART)
	{
		Lufciki.process_input();//Pierwsze zdazenia. Koncza sie po ctrl-B	
		//GLOWNA PETLA SYMULACJI
		cout<<WINDOW_HEADER<<": STARTED."<<endl;
		tenSwiat.simulation_loop(iWychodzenie);
	}
	else
	{
		int statusWin=Lufciki.search("STATUS");
		Lufciki.maximize(statusWin);
		for(int symulacja=0;symulacja<AUTOSTART;symulacja++)
			{
			//GLOWNA PETLA SYMULACJI
			cout<<WINDOW_HEADER<<": SYMULATION "<<symulacja<<" STARTED ."<<endl;
			tenSwiat.simulation_loop(1);
			cout<<WINDOW_HEADER<<": SYMULATION "<<symulacja<<" DONE ."<<endl;
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

delete &tenSwiat;//Dealokacja swiata wraz ze wszystkimi skladowymi
cout<<"----------> See you later!!! <--------------\n"<<endl<<flush;
return 0;
}


/* STATIC ALLOCATION */
//unsigned agent::max=0;//jaki jest najwiekszy taxon

