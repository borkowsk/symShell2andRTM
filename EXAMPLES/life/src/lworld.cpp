//#include <limits.h>
//#include <assert.h>
//#include <string.h>
//#include <math.h>
#include <string.h>
#include <math.h>

#include "lrand.h"
#include "lworld.h"
#include "histosou.hpp"
#include "clstsour.hpp" //Jest tez statsour
#include "coincsou.hpp"
#include "gadgets.hpp" 
#include "wb_ptrio.h"

const RAMKA=4;
extern const char* SYMULATION_NAME;

/*
template <class T>
inline void wb_swap(T& a,T& b)
{
	T c=a;
	a=b;
	b=c;
}
*/

//Konstrukcja agentow
///////////////////////////////////
lifeagent::lifeagent(const lifeagent& ini)
	{
		if(&ini!=NULL)
		{
			First=ini.First;
			Second=ini.Second;
		}
		else
			_clean();
	}

lifeagent::lifeagent()
	{
		_clean();
		if(DRAND()<=InitProp)
			First=1;
		else
			First=0;
		//First=RANDOM(ile_kate);
		Second=0;
	}

//Statyczne pola lifeagentow dla inicjalizacji
////////////////////////////////////////////////////////////////
short	lifeagent::ile_kate=2;//Ilosc kategori w mapach	
double	lifeagent::MutationLevel=0;//Prawd. spontanicznej zmiany pogladow (0..1)
short	lifeagent::kate_shift=7;
double  lifeagent::InitProp=0.5;//Proporcje inicjowania losowego

//KONSTRUKCJA	SWIATA
////////////////////////////////////
extern unsigned internal_log;

lifeworld::lifeworld(
	   size_t Width,	//Szerokosc torusa macierzy agentow
	  char* log_name,	//Nazwa pliku do zapisywania histori
	  char* mapl_name,	//Nazwa (bit)mapy inicjujacej "skladowe"
	  double noise,		//=0,
	  short	ile_kate,	//=2,		//Ilosc kategori w mapach	
	  short	odl_sasiad,	//=1,	//Rozmiar sasiedztwa
	  short	ile_sasiad,	//=8,	//8==Gestosc sasiedztwa	- jesli -1 to wszystko po kolei
	  bool	synchronicly,	//=true,
	  double spontanic	//=0	//Prawdopodobienstwo spontanicznej zmiany pogladu
		):	
		world(log_name,50),		
		MaplName(clone_str(mapl_name)),//Nazwa (bit)mapy 1. inicjujacej agentow					
	//Sub-obiekty wlasciwe dla tej symulacji
		
		MyWidth(Width),
		//Agenci(Width,Width,false,NULL),//Initer == NULL wiec tworzone przez konstruktor a nie klonowanie
		Agenci(Width,Width),//Zakladamy ze wystarcza to co robi bezparametrowy konstruktor agenta
        IleKate(ile_kate),	//Ilosc kategori w mapach
		IleSasiad(ile_sasiad),	//8==Gestosc sasiedztwa
		OdlSasiad(odl_sasiad),	//Rozmiar sasiedztwa
		Noise(noise),
		Synchronic(synchronicly),
		BierzWszystko(0), //Sasiedztwo bez losowania
		//Wskazniki do podstawowych seri danych
		Firsts(NULL),
		Seconds(NULL)
		{//!!!Niewiele mozna zrobic bo nie mozna tu jeszcze polegac na wirtualnych metodach klasy swiat
			
			set_simulation_name(SYMULATION_NAME);
			
			assert(ile_kate==2);//Na razie nie moze byc nic innego

			if(lifeagent::InitProp!=Noise)
			{
				lifeagent::InitProp=Noise;//Inne proporcje inicjowania losowego
				Agenci.Reinitialise();//Niestety powtorna robota
			}

			lifeagent::MutationLevel=spontanic;
			if(IleSasiad==-1)
				BierzWszystko=1;

		}

//Generuje podstawowe zrodla dla wbudowanego menagera danych lub innego
////////////////////////////////////////////////////////////////////////////
void lifeworld::make_basic_sources(sources_menager& WhatSourMen)
{
world::make_basic_sources(WhatSourMen);//Odziedziczone

//Glowne serie 
Firsts=Agenci.make_source("State",&lifeagent::First);	
if(Firsts)
	Firsts->setminmax(0,IleKate-1);	

Seconds=Agenci.make_source("Prev. state",&lifeagent::Second);
if(Seconds)
	Seconds->setminmax(0,IleKate-1);

//Umieszczenie glownych serii w menagerze serii
WhatSourMen.insert(Firsts);
WhatSourMen.insert(Seconds);
}


//Wspolpraca z menagerem wyswietlania a takze logiem
//------------------------------------------------------------------
void lifeworld::make_default_visualisation(area_menager_base& Menager)
//Rejestruje pochodne serie, tworzy domyslne "lufciki" i wklada w "Menager"
{
int iFirst=0,iSecond=0;
//Uzyskanie indeksow podstawowych serii z menagera
{
if(Firsts) iFirst=Sources.search(Firsts->name());
	else  goto ERROR;

if(Seconds) iSecond=Sources.search(Seconds->name());
	else  goto ERROR;


//Oraz utworzenie pochodnych serii statystycznych
generic_clustering_source*	FirstStat=new generic_clustering_source(Firsts);
if(!FirstStat) goto ERROR;
	else	Sources.insert(FirstStat);

generic_clustering_source*	SecondStat=new generic_clustering_source(Seconds);
if(!SecondStat) goto ERROR;
	else	Sources.insert(SecondStat);

//Zrodlo liczace statystyke i histogram z klasyfikacji
generic_histogram_source*  ClassStat=new generic_histogram_source(Firsts);
if(!ClassStat) goto ERROR;
	else	Sources.insert(ClassStat);

//A takze utworzenie seri liczacych ich wzajemne ko-statystyki
coincidention_source* CorrFS=new coincidention_source(Firsts,Seconds);
if(!CorrFS) goto ERROR;
Sources.insert(CorrFS);//Zeby zostala kiedys zwolniona, a poza tym moze ktos kiedys...

fifo_source<double>* EntropyFSLog=new fifo_source<double>(CorrFS->Entropy(),internal_log);
if(!EntropyFSLog) goto ERROR;
int iEntropyFS=Sources.insert(EntropyFSLog);

fifo_source<double>* CorrFSLogR=new fifo_source<double>(CorrFS->Tau_a_Goodman_Kruskal(),internal_log);//Fifo korelacji pierwszych z drugimi
if(!CorrFSLogR) goto ERROR;
int iCorrFSR=Sources.insert(CorrFSLogR);
 
 
//I utworzenie seri liczacych ich statystyki
 
fifo_source<double>* StressFirstLog=new fifo_source<double>(FirstStat->Stress(),internal_log);//Fifo ze stresu
if(!StressFirstLog) goto ERROR;
int iSFirst=Sources.insert(StressFirstLog);

fifo_source<double>* StressSecondLog=new fifo_source<double>(SecondStat->Stress(),internal_log);//Fifo ze stresu
if(!StressSecondLog) goto ERROR;
int iSSecond=Sources.insert(StressSecondLog);

//iMainClassF,iWhichMainF,iNumClassF,			
fifo_source<double>* NumClassLog=new fifo_source<double>(ClassStat->NumOfClass(),internal_log);
if(!NumClassLog) goto ERROR;
int iNumClassF=Sources.insert(NumClassLog);

fifo_source<double>* ClassEntropyLog=new fifo_source<double>(ClassStat->Entropy(),internal_log);
if(!ClassEntropyLog) goto ERROR;
int iClassEntropy=Sources.insert(ClassEntropyLog);

fifo_source<double>* MainClassLog=new fifo_source<double>(ClassStat->MainClass(),internal_log);
if(!MainClassLog) goto ERROR;
int iMainClassF=Sources.insert(MainClassLog);

//I umieszczanie w logu tego co trzeba
Log.insert(ClassStat->NumOfClass());
Log.insert(ClassStat->Entropy());
Log.insert(ClassStat->NormEntropy());
Log.insert(ClassStat->MainClass());
Log.insert(ClassStat->WhichMain());
Log.insert(FirstStat->Stress());
Log.insert(SecondStat->Stress());
Log.insert(CorrFS->Entropy());
Log.insert(CorrFS->NormEntropy());
Log.insert(CorrFS->Chi2());
Log.insert(CorrFS->LevelOfFreedom());
Log.insert(CorrFS->V2Cramer());
Log.insert(CorrFS->T2Czupurow());
Log.insert(CorrFS->Tau_b_Goodman_Kruskal());
Log.insert(CorrFS->Tau_a_Goodman_Kruskal());

//PODSTAWOWA WIZUALIZACJA SERII DANYCH
//WYMIARY DOMYSLNEGO OKNA
unsigned szer=Menager.getwidth();
unsigned wyso=Menager.getheight();
assert(szer>50 && wyso>40);//Najmniejsze sensowne okno

//Obszary domyœlne - np obszar STATUSU
world::make_default_visualisation(Menager);
if(OutArea) 
{
	OutArea->set(1,1,szer/2-1,wyso/2-1);
	Menager.as_orginal(Menager.search(OutArea->name()));
}

//WLASCIWE LUFCIKI
graph* pom1=new sequence_graph(szer/2-1,wyso/4,szer-50,wyso/2-1,
							    3,Sources.make_series_info(
										iClassEntropy,iNumClassF,iMainClassF,									
											-1
										).get_ptr_val(),
								0//* Z reskalowaniem 
							   );
if(!pom1) goto ERROR;
pom1->setframe(128);
pom1->settitle("HISTORY OF CLASSIFICATION");
Menager.insert(pom1);

//inne mniej potrzebne
graph* pom=new sequence_graph(szer/2-1,1,szer-50,wyso/4-1,	//domyslne wspolrzedne
							    1,Sources.make_series_info(
										iSFirst,
											-1
										).get_ptr_val(),
								//0// Z reskalowaniem 
							   1);//Wspolne minimum/maximum
if(!pom) goto ERROR;
pom->setframe(128);
pom->settitle("HISTORY OF STRESS");
Menager.insert(pom);

pom=new carpet_graph(1,wyso/2,szer/3,wyso-1,//domyslne wspolrzedne
						Firsts);//I zrodlo danych
pom->setdatacolors(0,255);
pom->settitle("Map of current state");
Menager.insert(pom);

pom=new bars_graph(szer/3+1,wyso/2,szer/3*2,wyso-1,//domyslne wspolrzedne  szer-49,7*char_height('X')+7,szer,8*char_height('X')+9
						ClassStat);
pom->setdatacolors(0,255);
pom->settitle("Histogram of state");
Menager.insert(pom);

pom=new manhattan_graph(szer/3*2+1,wyso/2,szer,wyso-1,//domyslne wspolrzedne
						    CorrFS,0,	//I zrodlo danych
							CorrFS,0,
							1,
							0.22,		//Ulamek szerokosci przeznaczony na perspektywe
							0.77);		//Ulamek wysokosci  przeznaczony na perspektywe
pom->setdatacolors(0,255);
pom->settextcolors(0);
pom->settitle("Determination of curr. state by prev. state");
Menager.insert(pom);

//PRZYCISKI
pom=new carpet_graph(szer-49,5*(char_height('X')+RAMKA),szer,6*(char_height('X')+RAMKA),//domyslne wspolrzedne 
						Seconds);//I zrodlo danych
pom->setdatacolors(0,255);
pom->setframe(0);
pom->settitle("Map of previous state");
Menager.insert(pom);

					
pom1=new sequence_graph(szer-49, 9*(char_height('X')+RAMKA),szer,10*(char_height('X')+RAMKA),
						
							    1,Sources.make_series_info(
										iEntropyFS,
											-1
										).get_ptr_val(),
							   1/*Wspolne minimum/maximum*/);
if(!pom1) goto ERROR;
pom1->setframe(128);
pom1->settitle("HISTORY OF ENTROPY OF DETERMINATION");
Menager.insert(pom1);


pom=new sequence_graph(szer-49, 11*(char_height('X')+RAMKA),szer,12*(char_height('X')+RAMKA),
							    1,Sources.make_series_info(
										iCorrFSR,//iCorrFS,						
											-1
										).get_ptr_val(),
								1
							   );
if(!pom) goto ERROR;
pom->setframe(128);
pom->settitle("HISTORY OF Prev. TO Curr. CORRELATION");
Menager.insert(pom);

//Tworzenie obszaru sterujacego
{
wb_dynarray<rectangle_source_base*> tmp(2,(rectangle_source_base*)Sources.get(iFirst),
										  (rectangle_source_base*)Sources.get(iSecond),
										  -1
										  );
drawable_base* pom=new steering_wheel(szer-49,0,szer,5*(char_height('X')+RAMKA),tmp);			
assert(pom!=NULL);
pom->setbackground(10);
Menager.insert(pom);
}

}
Sources.new_data_version(1,1);//Oznajmia seriom ze dane sie uaktualnily	(po inicjacji)

ERROR://... tu akcja na niepogode
	;//error_message(...)
}
	


//AKCJE SYMULACYJNE
//////////////////////
//////////////////////
void lifeworld::after_read_from_image()
//actions after read state from file. Aktualizacja pol static lifeagent'a!!!
{
	lifeagent::ile_kate=IleKate;//Ilosc kategori w mapach	
	
	switch(IleKate)
	{
	case   2:lifeagent::kate_shift=7;break;
	case   4:lifeagent::kate_shift=6;break;
	case   8:lifeagent::kate_shift=5;break;
	case  16:lifeagent::kate_shift=4;break;
	case  32:lifeagent::kate_shift=3;break;
	case  64:lifeagent::kate_shift=2;break;
	case 128:lifeagent::kate_shift=1;break;
	case 256:lifeagent::kate_shift=0;break;
	default:
		lifeagent::ile_kate=IleKate=256;
		lifeagent::kate_shift=0;
		cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
		Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
		break;
	}
}

// stan startowy symulacji
void lifeworld::initialize_layers()
//-------------------------------------
{
	static first=1;//TYMCZASOWE WYLACZENIE NADMIARU WYDRUKOW!!!
	if(first)
		Log.GetStream()<<"attitude SIMULATION:";
	//odl_sasiad=1,//Rozmiar sasiedztwa
	//ile_sasiad=8 //8==Gestosc sasiedztwa

	lifeagent::ile_kate=IleKate;//Ilosc kategori w mapach	
	
	switch(IleKate)
	{
	case   2:lifeagent::kate_shift=7;break;
	case   4:lifeagent::kate_shift=6;break;
	case   8:lifeagent::kate_shift=5;break;
	case  16:lifeagent::kate_shift=4;break;
	case  32:lifeagent::kate_shift=3;break;
	case  64:lifeagent::kate_shift=2;break;
	case 128:lifeagent::kate_shift=1;break;
	case 256:lifeagent::kate_shift=0;break;
	default:
		lifeagent::ile_kate=IleKate=256;
		lifeagent::kate_shift=0;
		cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
		Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
		break;
	}
	
	//...wydruk wartosci parametrow symulacji
	if(first)
	  Log.GetStream()
		<<"\nNum of Kl="<<Log.separator()<<IleKate
		<<"\nNoise %="<<Log.separator()<<Noise*100
		<<"\nNaighborhood="<<Log.separator()<<IleSasiad<<"/("<<(1+2*OdlSasiad)<<"*"<<(1+2*OdlSasiad)<<")\n";
	
	//			USTALANIE STANÓW AGENTÓW
	//Wczytuje uzywajac konstruktora lub klonowania gdy niema, wiec inicjuje reszte pól.
	int from= Agenci.init_from_bitmap(MaplName.get_ptr_val(),lifeagent::assign123);
	
	//Jesli nie zainicjowane z bitmapy to zostaje to z konstruktorow
	//if(from!=1)	Agenci.clean();//		reallocate_all();
	
	first=0;//Koniec pierwszego wywolania //TYMCZASOWO!!!
}

//Pojedynczy krok symulacji
void lifeworld::simulate_one_step()
//---------------------------------------
{
	const geometry_base* MyGeom=Agenci.get_geometry();
	assert(MyGeom);
	
	if(Synchronic)
	{
		//Idziemy po agentach pelnym iteratorem a stan agentow zmieniamy dopiero potem
		iteratorh IGlob=MyGeom->make_global_iterator();
		while(IGlob)
		{
			size_t index=MyGeom->get_next(IGlob);//Uzyskujemy index  agenta	
					 
			assert(index!=MyGeom->FULL);				//... tutaj nie powinno sie zdarzyc

			lifeagent& CenterAgent=Agenci.get(index);// Uzyskujemy referencje do agenta 
			
			CheckChange(MyGeom,index,CenterAgent); //Sprawdzamy zmiane stanu	
		}
		// upewniamy sie ze iterator zostanie usuniety
		MyGeom->destroy_iterator(IGlob);

		
		IGlob=MyGeom->make_global_iterator();//Tworzymy nowy iterator i iterujemy od poczatku
		while(IGlob)
		{
			size_t index=MyGeom->get_next(IGlob);//Uzyskujemy index  agenta	
					 
			assert(index!=MyGeom->FULL);				//... tutaj nie powinno sie zdarzyc

			lifeagent& CenterAgent=Agenci.get(index);// Uzyskujemy referencje do agenta
			
			wb_swap(CenterAgent.First,CenterAgent.Second);  //Ma nowy stan
		}	
		
		// upewniamy sie ze iterator zostanie usuniety
		MyGeom->destroy_iterator(IGlob);

	}
	else
	{
		iteratorh Monte=MyGeom->make_random_global_iterator();	//Alokujemy iterator Monte-Carlo
		
		while(Monte)//Idziemy po agentach iteratorem Monte-Carlo. Niektórzy moga sie powtórzyc
		{	
			size_t index=MyGeom->get_next(Monte);//Uzyskujemy index losowo wybranego agenta	
		
			assert(index!=MyGeom->FULL);				//... tutaj nie powinno sie zdarzyc
			
			lifeagent& CenterAgent=Agenci.get(index);// Uzyskujemy referencje do agenta
			
			if(CheckChange(MyGeom,index,CenterAgent)==1)//Czy zaszla zmiana stanu
				{
					wb_swap(CenterAgent.First,CenterAgent.Second);
				}
		}
		
		// upewniamy sie ze iterator zostanie usuniety
		MyGeom->destroy_iterator(Monte);
	}
	
	
	
}


int lifeworld::CheckChange(const geometry_base* MyGeom,
						size_t index,
						lifeagent& CenterAgent
						)//KOD NA SZUKANIE ZMIAN
{ 
	int testowanie=0;
	
	if(DRAND()<=lifeagent::MutationLevel)//Rzadka, spontaniczna zmiana stanu
	{
		int atti=RANDOM(IleKate);
		assert(0<=atti && atti<IleKate);
		
		CenterAgent.Second=atti;			//zmieniamy w agencie centralnym
		return 1;
	}

	// Alokujemy iterator sasiedztwa
	::iteratorh Neigh=NULL;
	
	if(BierzWszystko)
	{
		Neigh=MyGeom->make_neighbour_iterator(index,OdlSasiad);
	}
	else
	{
		Neigh=MyGeom->make_random_neighbour_iterator(index,OdlSasiad,IleSasiad);
	}

	int zliczanie=0;//Zliczanie sasiadów
	double alive=0;//LICZNIK ZYWYCH

	while(Neigh)
	{
		size_t index2=MyGeom->get_next(Neigh);//Uzyskujemy index sasiada		
		if(index2==MyGeom->FULL || index2==index)	//Jesli poza obszarem symulacji lub w 
			continue;				//centrum obszaru to dalej byloby bez sensu.
		
		lifeagent& PeryfAgent=Agenci.get(index2);//Uzyskujemy referencje do sasiada
	
		zliczanie++;
		alive+=PeryfAgent.First;// /double(lifeagent::ile_kate);
	}
	
	MyGeom->destroy_iterator(Neigh);	// upewniamy sie ze iterator zostanie usuniety
	
	if(alive<=1 || alive>=4)
	{
		CenterAgent.Second=0;			//zmieniamy w agencie centralnym
		return 1;
	}
	else
	if(alive==3 )//&& CenterAgent.First!=0)
	{
		CenterAgent.Second=1;//+CenterAgent.First;			//zmieniamy w agencie centralnym
		return 1;
	}
	else //Nic sie nie zmienia
	{
		CenterAgent.Second=CenterAgent.First;//Albo nic nie zmieniamy
		return 0;
	}
}
