//Symulacja zmiany pogladow - attitudes
//Uzyskana niegdys przez uproszczenie programu LANGUAGES
//Uzupelniona o wagi opini innych jako fragmentaryczna implementacja teori Arie Kruglanskiego
// a takze o inicjowanie niepelnego wypelnienia przestrzeni symulacji i roznego procentu jednej 
// z klas  (minority/majority)(
///////////////////////////////////////////////////////////////////////////////////////////////////////
//v 0.6 - dodanie losowania nieproporcjonalnego, znaczne zmiany w statystykach, zmiany z bazowej bibliotece
//v 0.61 - uzycie innego histogramu
const char* WINDOW_HEADER="ATTITUDES K version 0.71b";
const char* SYMULATION_NAME="attit_k_v0.71b";

#include <stdlib.h>
#include <iostream.h>
#include "wbminmax.hpp"
#include "arand.h"
#include "aworld.h"

unsigned SWIDTH=750;
unsigned SHEIGHT=550;

unsigned internal_log=7000;             //Nieobiektowo przekazywane do metody inicializacji zrodel 
unsigned spatial_correlation_mode=50;  //-------------//------------------//---------------------

char  LogName[512]="kattitude.log\0-------------------+--";
char HistName[512]="\0--+---------kattitudes.otx----------";
char MapLName[512]="\0--+---------kattitudes.gif----------";
char MapPName[512]="\0--+---------powers.gif------------";
char MaskName[512]="\0--+---------mask.gif--------------";
unsigned iWidth=50;
unsigned iMaxIterations=0xffffffff;
unsigned iLogRatio=1;
unsigned iViewRatio=1;

int  RuchomaSila=0;			//Czy sila ma sie powiekrzac "z wiekiem"
int  MaksymalnaSila=100;	//Jaka najwieksza sila
char TresProcent=100;		//Powyzej jakiej sily zmiany "pogladu" sa juz niemozliwe

int    IloscKlas=2;
double Fill=1;      //0.001 do 1 - poczatkowe wypelnienie przestrzeni agentow (wykonywane przez losowe usuwanie!!!)
double MajorMinor=-1;//Jaka czesc spoleczenstwa bedzie wyznawac "bialy" poglad - mniejszosc|wiekszosc
double ProbMig=0;   //Prawdopodobienstwo migracji jesli presja otoczenia wskazuje na zmiane pogladow
int    ProcentSzumu=0;
double MutacjeSpon=0;
int    RozmiarSasiedztwa=1;//3x3-1
int    IleSasiadow=-1;//Wszyscy sasiedzi - nielosowo. 8 - losowo!!!

double WagaSiebie=1;//8+1
double NeedForClousure=1;//Waga opini innych

bool   TypSymulacji=0;//Synchroniczna

int  iWychodzenie=0;
int  Replay=0;
int	 AUTOSTART=0;


int parse_options(const int argc,const char* argv[])
{
for(int i=1;i<argc;i++)
    {
    if( *argv[i]=='-' ) /* Opcja X lub symshella */
		continue;
	//Uppercasing
	wb_pchar hand(clone_str(argv[i]));
	char*    rob=hand.get_ptr_val();
	char*    pom=strchr(rob,'=');
	if(pom==NULL) 
			goto ERROR; //NA PEWNO ZLE
	*pom='\0';
	strupr(rob);
	*pom='=';
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
    if((pom=strstr(rob,"FILL="))!=NULL) //Nie NULL czyli jest
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
            Fill/=100;//Ulamek a nie procent tak naprawde
        }
        else
            cerr<<"FILL = "<<Fill<<endl;
    }
    else
    if((pom=strstr(rob,"BALA="))!=NULL) //Nie NULL czyli jest
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
            MajorMinor/=100;//Ulamek a nie procent tak naprawde
        }
        else
            cerr<<"BALA = "<<MajorMinor<<endl;
    }
    else
    if((pom=strstr(rob,"PMIG="))!=NULL) //Nie NULL czyli jest
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
            ProbMig/=100;//Ulamek a nie procent tak naprawde
        }
        else
            cerr<<"PMIG (probability of migration) = "<<ProbMig<<endl;
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
    else
	if((pom=strstr(rob,"CLSS="))!=NULL) //Nie NULL czyli jest
	{
	IloscKlas=atol(pom+5);
    if(IloscKlas<2)
		{
		cerr<<"Bad CLSS = "<<IloscKlas<<" (must be >2 )"<<endl;
		return 0;
		}
	}
    else
	if((pom=strstr(rob,"MPOW="))!=NULL) //Nie NULL czyli jest
	{
	MaksymalnaSila=atol(pom+5);
    if(MaksymalnaSila<0)//0 czy 1???
		{
		cerr<<"Bad MPOW = "<<MaksymalnaSila<<" (must be >=1 )"<<endl;
		return 0;
		}
	}
    else
	if((pom=strstr(rob,"WPOW="))!=NULL) //Nie NULL czyli jest
	{
	RuchomaSila=atol(pom+5);
    if(RuchomaSila<0)
		{
		cerr<<"Bad WPOW = "<<RuchomaSila<<" (must be >=0 )"<<endl;
		return 0;
		}
	cerr<<"WPOW = "<<RuchomaSila<<endl;
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
	else
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
    else
	if((pom=strstr(rob,"SELF="))!=NULL) //Nie NULL czyli jest
    {
    WagaSiebie=atof(pom+5);
	cerr<<"SELF = "<<WagaSiebie<<endl;
    }
    else
	if((pom=strstr(rob,"NFOC="))!=NULL) //Nie NULL czyli jest
    {
    NeedForClousure=atof(pom+5);
	cerr<<"NFOC = "<<NeedForClousure<<endl;
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
	else
	if((pom=strstr(rob,"AUTO="))!=NULL) //Nie NULL czyli jest
    {
    AUTOSTART=atol(pom+5);
	cerr<<"AUTO="<<AUTOSTART<<endl;
	if(AUTOSTART)
		{
		iWychodzenie=1;
		cerr<<"STOP = "<<(iWychodzenie?"Yes":"No")<<endl;
		}
    }
	else
	if((pom=strstr(rob,"STOP="))!=NULL) //Nie NULL czyli jest
    {
    iWychodzenie=(toupper(pom[5])=='Y');
	cerr<<"STOP = "<<(iWychodzenie?"Yes":"No")<<endl;
    }
    else  //SYNC
	if((pom=strstr(rob,"SYNC="))!=NULL) //Nie NULL czyli jest
    {
    TypSymulacji=!(toupper(pom[5])=='Y');
	cerr<<"SYNC = "<<(TypSymulacji==0?"Yes":"No")<<endl;
    }
    else
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
    //cerr<<"\tRSPC=N/Y or 1..WIDTH - Random calculation of spatial correlation ("<<(spatial_correlation_mode==0?"N":"Y")<<")\n";
    if((pom=strstr(rob,"RSPC="))!=NULL) 
    {
    const char* lpom=pom+5;
    if(toupper(*lpom)=='N')
        spatial_correlation_mode=0;
    else
    if(toupper(*lpom)=='Y')
        spatial_correlation_mode=16;
    else
        spatial_correlation_mode=atoi(lpom);
    cerr<<"Random calculation of spatial correlation is "<<(spatial_correlation_mode==0?"d i s a b l e d":"e n a b l e d")<<". Multiplication="<<spatial_correlation_mode<<"\n";
    }
	else
	if((pom=strstr(rob,"LOGF="))!=NULL) //Nie NULL czyli jest
    {
    strcpy(LogName,pom+5);
    }
    else
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
		cout<<"Unknown parameter \""<<argv[i]<<"\"\n";
		cout<<"YOU CAN USE:\n";
		cout<<"\tREPL=hist.otx - not symulate but replay symulation history file.\n";
		cout<<"\tMAPL=initL.gif (or BMP)- file with initialization map of attitudes (RANDOM)\n";
		cout<<"\tMAPP=initP.gif (or BMP)- file with initialization map of powers (RANDOM)\n";
		cout<<"\tMASK=mask.gif	(or BMP)- mask file for alive (not black) agents (ALL ALIVE)\n";	

		cout<<"\tSYNC=Y/N - synchronic (Y) or Monte-Carlo symulation mode ("<<(TypSymulacji==0?"Yes":"No")<<")\n";
		cout<<"\tNFOC=0..inf - need for closure  [meaning depend on model]("<<NeedForClousure<<")\n";
		cout<<"\tSELF=0..1 - use self for calculations ("<<(WagaSiebie)<<")\n";
		
        cout<<"\tWIDTH=NN - matrix size ("<<iWidth<<")\n";
		cout<<"\tCLSS=NN - number of class. Must be power of 2. ("<<IloscKlas<<")\n";	
        cout<<"\tFILL=0.001..1 - ratio of live agents ("<<Fill<<")\n";	
        cout<<"\tBALA=0.001..0.999 - part of society with \"white\" attitude ("<<MajorMinor<<")\n"; 
        cout<<"\tPMIG=0.001..1 - probability of migration under pressure ("<<ProbMig<<")\n";	
		cout<<"\tMPOW=NN - max strength for initilization ("<<MaksymalnaSila<<")\n"	;
		cout<<"\tWPOW=N	- walking step of strenght	("<<RuchomaSila<<")\n";
		cout<<"\tTRSP=N - % of treshold of strenght ("<<TresProcent<<")\n";
		cout<<"\tPRTR=2..WIDTH^2-1 - number of interaction partners (-1 = all in neighbourhood) ("<<IleSasiadow<<")\n";
		cout<<"\tINDI=1..WIDTH/2-1 - interaction distance ("<<RozmiarSasiedztwa<<")\n";
		
		cout<<"\tNOIP=NN - percent of noise ("<<ProcentSzumu<<")\n";
		cout<<"\tSPCH=NN - percent of spontanic change of attitudes ("<<MutacjeSpon*100<<")\n";
		cout<<"\tMAX=NNNN - max simulation step ("<<iMaxIterations<<")\n";
		
        cout<<"\tRSPC=N/Y or 1..WIDTH - Random calculation of spatial correlation ("<<(spatial_correlation_mode?"N":"Y")<<")\n";
        cout<<"\tILOG=NNNN - lenght of internal statistic logs ("<<internal_log<<")\n";
		cout<<"\tSTOP=N/Y - exit after MAX steps ("<<(iWychodzenie?"Yes":"No")<<")\n";
		cout<<"\tVIEV=N - visualisation frequency ("<<iViewRatio<<")\n";
		cout<<"\tLOGC=N - log file saving frequency ("<<iLogRatio<<")\n";
		cout<<"\tLOGF=name.log - file for simulation log ("<<LogName<<")\n";
		cout<<"\tHIST=hist.otx - file for full history of symulation.\n";
		cout<<"\tWIDTHWIN=YYY,HEIGHTWIN=XXX - initial window size.("<<SWIDTH<<'x'<<SHEIGHT<<"\n";
		cout<<"\nAUTO=XXX - number of auto-repetition of symulation.("<<AUTOSTART<<")\n";
        cout<<flush;
	return 0;
	}
    }
return 1;
}


/*  OGOLNA FUNKCJA MAIN */
/************************/

int main(const int argc,const char* argv[])
{
cout<<endl<<WINDOW_HEADER<<", compilation: "<<__DATE__<<' '<<__TIME__<<endl;
cout<<"Programmed by W.Borkowski from Warsaw University"<<endl;
cout<<"==================================================================="<<endl;
cout.flush();

if(!parse_options(argc,argv))
        exit(1);

main_area_menager Lufciki(24,SWIDTH,SHEIGHT,28);
if(!Lufciki.start(WINDOW_HEADER,argc,argv,1))
	{
	cerr<<"Can't initialize graphics"<<endl;
	exit(1);
	}

//Utworzenie sensownej nazwy pliku(-ów) do zrzutow ekranu
{
wb_pchar buf(strlen(SYMULATION_NAME)+20);
buf.prn("%s_%ld",SYMULATION_NAME,time(NULL));
Lufciki.set_dump_name(buf.get());
}


//INICJALIZACJA SYMULACJI
aworld& tenSwiat=*new aworld(iWidth,
						   LogName,
						   MapLName,
						   MapPName,
						   MaskName,
						   ProcentSzumu/100.0,//Szum od 0-1
						   MaksymalnaSila,//Zeby byla w przedziale
						   IloscKlas,
						   RozmiarSasiedztwa,
						   IleSasiadow,
						   WagaSiebie,
						   NeedForClousure,
						   (TypSymulacji==0?true:false),//Synchroniczna czy nie
						   RuchomaSila,
						   MaksymalnaSila*TresProcent/100.0,
						   MutacjeSpon,
                           Fill,
                           ProbMig,
                           MajorMinor
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

