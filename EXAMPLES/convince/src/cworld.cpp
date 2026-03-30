//#include <limits.h>
//#include <assert.h>
//#include <string.h>
//#include <math.h>
#include <string.h>
#include <math.h>

#include "crand.h"
#include "cworld.h"
#include "histosou.hpp"
#include "clstsour.hpp" //Jest tez statsour
#include "coincsou.hpp"
#include "gadgets.hpp" 
#include "wb_ptrio.h"

const int RAMKA=4;
extern const char* SYMULATION_NAME;

/* //jest w "wbminmax.hpp"
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
anAgent::anAgent(const anAgent& ini)
{
	if(&ini!=NULL)
	{
		First=ini.First;
		Second=ini.Second;
		if(min_sila<max_sila)	
			Power=min_sila+RANDOM(max_sila-min_sila+1);		
		else
			Power=ini.Power;
	}
	else
		_clean();
}

anAgent::anAgent()
{
	_clean();

	if(DRAND()<ToBeNewProb)
		First=1+RANDOM(ile_kate-1);
	else
		First=0;
	
	Second=0;
	if(min_sila<max_sila)	
		Power=min_sila+RANDOM(max_sila-min_sila+1);		
	else
		Power=max_sila;
}

//Statyczne pola Agentow dla inicjalizacji
////////////////////////////////////////////////////////////////
short	anAgent::ruchsily=1;  //Maksymalny skok sily
short   anAgent::min_sila=10;
short	anAgent::max_sila=100;//Maksymalna sila agenta
short	anAgent::ile_kate=2;//Ilosc kategori w mapach	
short	anAgent::kate_shift=0;//Przesuniecie dla wczytywania gifa

double	anAgent::ToBeNewProb=0;//Prawd. spontanicznej zmiany pogladow samotnika na nowy typ rozrywki/sportu
double  anAgent::NewInfectProb=0.01;//Prawd. zainfekowania od pary zainfekowanych
double  anAgent::ReverseProb=1;//Prawd. reversji pogladow na 0 - brak pomyslu na rozrywke
double  anAgent::SupportLevel=0.5;//Sila wsparcia gdy ma jakies towarzystwo

//KONSTRUKCJA	SWIATA
////////////////////////////////////
extern unsigned internal_log;

aWorld::aWorld(	
		unsigned iWidth,		//Szerokosc torusa macierzy agentow		
		double iToBeNewProb,//=0.1,//Prawd. spontanicznej zmiany pogladow samotnika na nowy typ rozrywki/sportu
		double iInfectProb,//=0.9,//Prawd. reversji pogladow na 0 - brak pomyslu na rozrywke
		double iSupportLevel,//=0.5,//Sila wsparcia gdy ma jakies towarzystwo	
		char* ilog_name,//="convince.log",		//Nazwa pliku do zapisywania histori
		char* imapl_name,//=NULL,	//Nazwa (bit)mapy inicjujacej "skladowe"
		char* imapp_name,//=NULL,	//Nazwa (bit)mapy inicjujacej "sily"
		char* ilive_mask,//=NULL,	//Czarne w tej mapie sa kasowane  		   			
		short imax_sila,//=100,	//Maksymalna sila agenta
		short imin_sila//,=10	//Minimalna sila 
			   /*,
			   double noise,		//Szum informacyjny
			   short ile_kate,		//Ilosc kategori w mapach	
			   short odl_sasiad,	//Rozmiar sasiedztwa
			   short ile_sasiad,	//8==Gestosc sasiedztwa			   
			   bool	synchronicly,
			   short walkpower,
			   short trespower,
			   */
			   ):	
		world(ilog_name,50),		
		MaplName(clone_str(imapl_name)),//Nazwa (bit)mapy 1. inicjujacej agentow					
		MappName(clone_str(imapp_name)),//Nazwa (bit)mapy 2. inicjujacej agentow					
		MaskName(clone_str(ilive_mask)),//Nazwa bitmapy maskujacej (kasujacej agentow)
	//Sub-obiekty wlasciwe dla tej symulacji
		MyWidth(iWidth),
		Agenci(iWidth,iWidth,NULL),//Initer == NULL wiec tworzone przez konstruktor a nie klonowanie
		//MaxSila(imax_sila),	//Maksymalna sila agenta
		//TrsSila(trespower), //Sila dajaca odporosc na zmiany
		//IleKate(ile_kate),	//Ilosc kategori w mapach
		//IleSasiad(ile_sasiad),	//8==Gestosc sasiedztwa
		//OdlSasiad(odl_sasiad),	//Rozmiar sasiedztwa
		//Noise(noise),
		//UseSelf(need_use_self),
		//Synchronic(synchronicly),
		//BierzWszystko(0), //Sasiedztwo bez losowania
		//Wskazniki do podstawowych seri danych
		Firsts(NULL),
		Seconds(NULL),
		Powers(NULL)//,Classif(NULL)
		{//!!!Niewiele mozna zrobic bo nie mozna tu jeszcze polegac na wirtualnych metodach klasy swiat
			anAgent::ruchsily=1;  //Maksymalny skok sily
			anAgent::min_sila=imin_sila;
			anAgent::max_sila=imax_sila;//Maksymalna sila agenta
			anAgent::ile_kate=2;//Ilosc kategori w mapach	
			anAgent::kate_shift=0;//Przesuniecie dla wczytywania gifa

			anAgent::ToBeNewProb=iToBeNewProb;//Prawd. spontanicznej zmiany pogladow samotnika na nowy typ rozrywki/sportu
			anAgent::NewInfectProb=iInfectProb;
			anAgent::ReverseProb=1-iToBeNewProb;//Prawd. reversji pogladow na 0 - brak pomyslu na rozrywke
			anAgent::SupportLevel=iSupportLevel;//Sila wsparcia gdy ma jakies towarzystwo

			set_simulation_name(SYMULATION_NAME);

			//if(IleSasiad==-1)BierzWszystko=1;
		}

//Generuje podstawowe zrodla dla wbudowanego menagera danych lub innego
////////////////////////////////////////////////////////////////////////////
void aWorld::make_basic_sources(sources_menager& WhatSourMen)
{
world::make_basic_sources(WhatSourMen);//Odziedziczone

//Glowne serie 
Firsts=Agenci.make_source("Attitude",&anAgent::First);	
if(Firsts)
Firsts->setminmax(0,anAgent::ile_kate-1);	
Seconds=Agenci.make_source("Prev. attitude",&anAgent::Second);
if(Seconds)
	Seconds->setminmax(0,anAgent::ile_kate-1);

Powers=Agenci.make_source("Power",&anAgent::Power);

//Classif=Agenci.make_source("Classification",&anAgent::Classif);
//if(Classif)
//	Classif->setminmax(0,IleKate*IleKate*IleKate-1);//Max class ==IleKate^3 bo trzy niezalezne plaszczyzny

//Umieszczenie glownych serii w menagerze serii
WhatSourMen.insert(Firsts);
WhatSourMen.insert(Seconds);
WhatSourMen.insert(Powers);

//WhatSourMen.insert(Classif);
}


//Wspolpraca z menagerem wyswietlania a takze logiem
//------------------------------------------------------------------
void aWorld::make_default_visualisation(area_menager_base& Menager)
//Rejestruje pochodne serie, tworzy domyslne "lufciki" i wklada w "Menager"
{
int iFirst=0,iSecond=0,iPower=0,iClassif=0;
//Uzyskanie indeksow podstawowych serii z menagera
{
if(Firsts) iFirst=Sources.search(Firsts->name());
	else  goto ERROR;

if(Seconds) iSecond=Sources.search(Seconds->name());
	else  goto ERROR;

if(Powers)   iPower=Sources.search(Powers->name());
	else  goto ERROR;

if(Firsts)  iClassif=Sources.search(Firsts->name());
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


fifo_source<double>* WhichMainLog=new fifo_source<double>(ClassStat->WhichMain(),internal_log);
if(!WhichMainLog) goto ERROR;
int iWhichMainF=Sources.insert(WhichMainLog);


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
										iNumClassF,iMainClassF,iWhichMainF,									
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
pom->settitle("Map of current attitude");
Menager.insert(pom);

pom=new bars_graph(szer/3+1,wyso/2,szer/3*2,wyso-1,//domyslne wspolrzedne  szer-49,7*char_height('X')+7,szer,8*char_height('X')+9
						ClassStat);
pom->setdatacolors(0,255);
pom->settitle("Histogram of attitude");
Menager.insert(pom);

pom=new manhattan_graph(szer/3*2+1,wyso/2,szer,wyso-1,//domyslne wspolrzedne
						    CorrFS,0,	//I zrodlo danych
							CorrFS,0,
							1,
							0.22,		//Ulamek szerokosci przeznaczony na perspektywe
							0.77);		//Ulamek wysokosci  przeznaczony na perspektywe
pom->setdatacolors(0,255);
pom->settextcolors(0);
pom->settitle("Dynamism: curr. attit. vers. prev. attitude");
Menager.insert(pom);

//PRZYCISKI
pom=new carpet_graph(szer-49,5*(char_height('X')+RAMKA),szer,6*(char_height('X')+RAMKA),//domyslne wspolrzedne 
						Seconds);//I zrodlo danych
pom->setdatacolors(0,255);
pom->setframe(0);
pom->settitle("Map of previous attitude");
Menager.insert(pom);


pom=new carpet_graph(szer-49,6*(char_height('X')+RAMKA),szer,7*(char_height('X')+RAMKA),//domyslne wspolrzedne
						Powers);//I zrodlo danych
pom->setdatacolors(0,255);
pom->setframe(0);
pom->settitle("Map of power");
Menager.insert(pom);

pom=new manhattan_graph(szer-49, 7*(char_height('X')+RAMKA),szer,8*(char_height('X')+RAMKA),//domyslne wspolrzedne
						Powers,0,//I zrodlo danych o wysokosciach, miezazadzane
						Firsts,0,//Zrodlo danych o kolorach - niezazadzane
						1,		//Slupki zaczynaja sie conajmniej od 0!
									//Jesli 0 to zaczynaja sie od min>0
						0.22,		//Ulamek szerokosci przeznaczony na perspektywe
						0.77		//Ulamek wysokosci  przeznaczony na perspektywe
						);//I zrodlo danych
pom->setdatacolors(0,255);
pom->setframe(0);
pom->settitle("Composed map of strength & attitude of agents");
Menager.insert(pom);
					
pom1=new sequence_graph(szer-49, 9*(char_height('X')+RAMKA),szer,10*(char_height('X')+RAMKA),
						
							    1,Sources.make_series_info(
										iClassEntropy,
											-1
										).get_ptr_val(),
							   1/*Wspolne minimum/maximum*/);
if(!pom1) goto ERROR;
pom1->setframe(128);
pom1->settitle("HISTORY OF ENTROPY OF CLASIFICATION");
Menager.insert(pom1);


pom1=new sequence_graph(szer-49, 10*(char_height('X')+RAMKA),szer,11*(char_height('X')+RAMKA),						
							    1,Sources.make_series_info(
										iEntropyFS,
											-1
										).get_ptr_val(),
							   1/*Wspolne minimum/maximum*/);
if(!pom1) goto ERROR;
pom1->setframe(128);
pom1->settitle("HISTORY OF ENTROPY OF CHANGE");
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
pom->settitle("HISTORY OF Prev.TO Curr. CORRELATION");
Menager.insert(pom);

//Tworzenie obszaru sterujacego
{
wb_dynarray<rectangle_source_base*> tmp(4,(rectangle_source_base*)Sources.get(iFirst),
										  (rectangle_source_base*)Sources.get(iSecond),
										  //(rectangle_source_base*)Sources.get(iThird),
										  (rectangle_source_base*)Sources.get(iPower),
										  (rectangle_source_base*)Sources.get(iClassif),
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
void aWorld::after_read_from_image()
//actions after read state from file. Aktualizacja pol static anAgent'a!!!
{	
	switch(anAgent::ile_kate)
	{
	case   2:anAgent::kate_shift=7;break;
	case   4:anAgent::kate_shift=6;break;
	case   8:anAgent::kate_shift=5;break;
	case  16:anAgent::kate_shift=4;break;
	case  32:anAgent::kate_shift=3;break;
	case  64:anAgent::kate_shift=2;break;
	case 128:anAgent::kate_shift=1;break;
	case 256:anAgent::kate_shift=0;break;
	default:
		anAgent::ile_kate=256;
		anAgent::kate_shift=0;
		cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
		Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
		break;
	}
}

// stan startowy symulacji
void aWorld::initialize_layers()
//-------------------------------------
{
	static int first=1;//TYMCZASOWE WYLACZENIE NADMIARU WYDRUKOW!!!
	if(first)
		Log.GetStream()<<"convince SIMULATION:";
	
	switch(anAgent::ile_kate)
	{
	case   2:anAgent::kate_shift=7;break;
	case   4:anAgent::kate_shift=6;break;
	case   8:anAgent::kate_shift=5;break;
	case  16:anAgent::kate_shift=4;break;
	case  32:anAgent::kate_shift=3;break;
	case  64:anAgent::kate_shift=2;break;
	case 128:anAgent::kate_shift=1;break;
	case 256:anAgent::kate_shift=0;break;
	default:
		anAgent::ile_kate=256;
		anAgent::kate_shift=0;
		cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
		Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
		break;
	}
	
	//...wydruk wartosci parametrow symulacji
	if(first)
	  Log.GetStream()
	    <<"\n Change to new Pn="<<Log.separator()<<anAgent::ToBeNewProb//Prawd. spontanicznej zmiany pogladow samotnika na nowy typ rozrywki/sportu
		<<"\n Reverse Pr="<<Log.separator()<<anAgent::ReverseProb//Prawd. reversji pogladow na 0 - brak pomyslu na rozrywke
		<<"\n Infection Pr="<<Log.separator()<<anAgent::NewInfectProb//Prawd. reversji pogladow na 0 - brak pomyslu na rozrywke
		<<"\n Nei. Support S="<<Log.separator()<<anAgent::SupportLevel//"Sila" wsparcia gdy ma jakies towarzystwo
	
		<<"\nMin Power="<<Log.separator()<<anAgent::min_sila
	    <<"\nMax Power="<<Log.separator()<<anAgent::max_sila
		<<"\nMov. Power="<<Log.separator()<<anAgent::ruchsily
		//<<"\nTresh of Power="<<Log.separator()<<anAgent::treshold
		<<"\nNum of Kl="<<Log.separator()<<anAgent::ile_kate;
		//<<"\nNoise %="<<Log.separator()<<Noise*100		
		//<<"\nNaighborhood="<<Log.separator()<<IleSasiad<<"/("<<(1+2*OdlSasiad)<<"*"<<(1+2*OdlSasiad)<<")\n";
	
	//			USTALANIE STANÓW AGENTÓW
	//Wczytuje uzywajac konstruktora lub klonowania gdy niema, wiec inicjuje reszte pól.
	int from1= Agenci.init_from_bitmap(MappName.get_ptr_val(),&anAgent::assignPow);
	int from2= Agenci.init_from_bitmap(MaplName.get_ptr_val(),&anAgent::assign123);
	
	//Jesli nie zainicjowane to prowizoryczna inicjacja przez konstruktory lub klonowanie
	if(from1!=1 && from2!=1)
		Agenci.reallocate_all();
	
	//Zabija m jesli w masce jesc czarny kolor
	if(Agenci.init_from_bitmap(MaskName.get_ptr_val(),&anAgent::killBlack)==1 )
		Agenci.deallocate_not_OK();

	first=0;//Koniec pierwszego wywolania //TYMCZASOWO!!!
}

//Pojedynczy krok symulacji
void aWorld::simulate_one_step()
//---------------------------------------
{
	const rectangle_geometry* MyGeom=dynamic_cast<const rectangle_geometry*>(Agenci.get_geometry());
												assert(MyGeom!=NULL);
	size_t MonteSteps=MyWidth*MyWidth;
	for(int m=0;m<MonteSteps;m++)
	{
		long x=RANDOM(MyWidth);				assert(x<MyWidth);
		long y=RANDOM(MyWidth);				assert(y<MyWidth);
		anAgent& CenterAgent=Agenci(x,y);

		if(CenterAgent.First==0) //pusty - bez pogladu na sport/rozrywke
		{
			if(DRAND()<anAgent::ToBeNewProb)
				CenterAgent.First=1+RANDOM(anAgent::ile_kate-1);
			continue;
		}
		else
		{
			//Trzeba sprawdzic czy nie jest samotny, a potem zmienic jego i okolice
			unsigned index=MyGeom->get(x,y);

			//Sprawdzamy jacy sa sasiadzi
			int koledzy[8][2],ilu=0;
			iteratorh Neigh=MyGeom->make_neighbour_iterator(index,1);
			while(Neigh)
			{
				size_t index2=MyGeom->get_next(Neigh);//Uzyskujemy index sasiada	
				assert(index2!=any_layer_base::FULL);//Zawsze ma byc torus

				if( index2==index)	//Jesli w centrum obszaru to on sam i dalej byloby bez sensu.
					continue;

				unsigned nx,ny;
				MyGeom->WhatCoordinates(index2,nx,ny);
				if(Agenci(nx,ny).First==Agenci(x,y).First)
				{
					koledzy[ilu][0]=nx;
					koledzy[ilu][1]=ny;
					ilu++;
				}				
			}
			MyGeom->destroy_iterator(Neigh);
			
			if(ilu==8)//Nie ma kogo zarazac
				continue;

			if(ilu>0)//Gdy ma choc jednego sasiada ze swoim pogladem
			{
				long nx,ny;
				if(ilu>1)
				{
					int pom=RANDOM(ilu);
					nx=koledzy[pom][0];
					ny=koledzy[pom][1];
				}
				else
				{
					nx=koledzy[0][0];
					ny=koledzy[0][1];
				}

				//Losowa utrata zainteresowania mimo supportu
				if(DRAND()<anAgent::ReverseProb-anAgent::SupportLevel)
				{
					CenterAgent.First=0;
					continue;
				}
				//A teraz najtrudniejsze - przekonywanie nieprzekonanych sasiadów

				int Inni[20][2],innych=0;
				for(int i=min(x,nx);i<=max(x,nx);i++)
				{
					for(int j=min(y,ny);j<=max(y,ny);j++)
					{
						if(i==x && j==y)
							continue; //Pierwszy z pary
						if(i==nx && j==ny)
							continue;//Drugi z pary
						if(abs(i-x)<2 && abs(i-nx)<2 &&
						   abs(j-y)<2 && abs(j-ny)<2)
						{
							size_t ConvertedIndex=MyGeom->get(i,j); assert(ConvertedIndex!=geometry_base::FULL);
																	assert(ConvertedIndex<MyWidth*MyWidth);
							anAgent& ForModify=Agenci.get(ConvertedIndex);
							if(ForModify.First==0 && DRAND()<anAgent::NewInfectProb)
								ForModify.First=CenterAgent.First;//TMP 
						}
						//....
					}
				}
				//MyGeom->WhatCoordinates
				
			}
			else
			{
				//Gdy nie ma ani sasiada ze swoim pogladem - szybka losowa utrata 
				if(DRAND()<anAgent::ReverseProb)
					CenterAgent.First=0;
				continue;
			}
		}
	}
	/*
	const geometry_base* MyGeom=Agenci.get_geometry();			assert(MyGeom);		
	if(Synchronic)
	{
		//Idziemy po agentach pelnym iteratorem a stan agentow zmieniamy dopiero potem
		iteratorh Full=MyGeom->make_global_iterator();
		while(Full)
		{
			size_t index=MyGeom->get_next(Full);//Uzyskujemy index  agenta	
					 
			assert(index!=any_layer_base::FULL);				//... tutaj nie powinno sie zdarzyc

			anAgent& CenterAgent=*(Agenci.get_ptr(index).get_ptr_val());// Uzyskujemy referencje do agenta omijajac asercje na NULL
			
			if(Agenci.is_empty(CenterAgent))	// Sprawdzamy czy nie jest to pusta komórka (NULL)
				continue;						// bo wtedy robic dalej by³oby bez sensu.
			
			if(CenterAgent.Power<=TrsSila)		// Czy nie ma juz immunitetu na zmiany
				CheckChange(MyGeom,index,CenterAgent); //Sprawdzamy zmiane stanu
				
		}
		// upewniamy sie ze iterator zostanie usuniety
		MyGeom->destroy_iterator(Full);

		
		Full=MyGeom->make_global_iterator();//Tworzymy nowy iterator i iterujemy od poczatku
		while(Full)
		{
			size_t index=MyGeom->get_next(Full);//Uzyskujemy index  agenta	
					 
			assert(index!=any_layer_base::FULL);				//... tutaj nie powinno sie zdarzyc

			anAgent& CenterAgent=*(Agenci.get_ptr(index).get_ptr_val());// Uzyskujemy referencje do agenta omijajac asercje na NULL
			
			if(Agenci.is_empty(CenterAgent))	// Sprawdzamy czy nie jest to pusta komórka (NULL)
				continue;

			wb_swap(CenterAgent.First,CenterAgent.Second);  //Ma nowy stan
			CenterAgent.MakeOlder();						//Robi sie starszy
		}	
		
		// upewniamy sie ze iterator zostanie usuniety
		MyGeom->destroy_iterator(Full);

	}
	else
	{
		iteratorh Monte=MyGeom->make_random_global_iterator();	//Alokujemy iterator Monte-Carlo
		
		while(Monte)//Idziemy po agentach iteratorem Monte-Carlo. Niektórzy moga sie powtórzyc
		{	
			size_t index=MyGeom->get_next(Monte);//Uzyskujemy index losowo wybranego agenta	
		
			assert(index!=any_layer_base::FULL);				//... tutaj nie powinno sie zdarzyc
			
			anAgent& CenterAgent=*(Agenci.get_ptr(index).get_ptr_val());// Uzyskujemy referencje do agenta omijajac asercje na NULL
			if(Agenci.is_empty(CenterAgent))	// Sprawdzamy czy nie jest to pusta komórka (NULL)
				continue;						// bo wtedy robic dalej by³oby bez sensu.
			
			if(CenterAgent.Power<=TrsSila)		// Czy nie ma juz immunitedu na zmiany
				if(CheckChange(MyGeom,index,CenterAgent)==1)//Czy zaszla zmiana stanu
				{
					wb_swap(CenterAgent.First,CenterAgent.Second);
				}
				
			CenterAgent.MakeOlder();			//Robi sie starszy
		}
		
		// upewniamy sie ze iterator zostanie usuniety
		MyGeom->destroy_iterator(Monte);
	}
	
	*/
}



int aWorld::CheckChange(const geometry_base* MyGeom,
						size_t index,
						anAgent& CenterAgent
						)//KOD NA SZUKANIE ZMIAN
{ 
	/*
	int testowanie=0;
	
	if(DRAND()<=anAgent::MutationLevel)//Rzadka, spontaniczna zmiana pogladu
	{
		int atti=RANDOM(IleKate);       	assert(0<=atti && atti<IleKate);
		CenterAgent.Second=atti;			//zmieniamy w agencie centralnym
		return 1;
	}
	
	
	//TABLICA POMOCNICZA
	wb_dynarray<int> Firsts(IleKate);               assert(Firsts.IsOK());
	//Czyszczenie licznika
	memset(Firsts.get_ptr_val(),0,sizeof(int)*IleKate);

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

	//iteratorh Neigh=MyGeom->make_neighbour_iterator(index,OdlSasiad);
	unsigned zliczanie=0;//Zliczanie sasiadów
	
	while(Neigh)
	{
		size_t index2=MyGeom->get_next(Neigh);//Uzyskujemy index sasiada		
		if(index2==any_layer_base::FULL || index2==index)	//Jesli poza obszarem symulacji lub w 
			continue;				//centrum obszaru to dalej byloby bez sensu.
		
		anAgent& PeryfAgent=*(Agenci.get_ptr(index2).get_ptr_val());//Uzyskujemy referencje do sasiada omijajac asercje na NULL
		if(Agenci.is_empty(PeryfAgent))		//Sprawdzamy czy nie jest to pusta komórka (NULL)
			continue;					   // bo wtedy robic dalej by³oby bez sensu.
		
		zliczanie++;
		//Dodawanie sil sasiadow do licznikow w tablicach
		Firsts[PeryfAgent.First]+=PeryfAgent.Power;
	}
	
	MyGeom->destroy_iterator(Neigh);	// upewniamy sie ze iterator zostanie usuniety
	//Zlicza wylosowanych agentow
	testowanie++;	
	
	//Dodawanie wlasnych sil do licznikow w tablicach
	if(UseSelf)
	{
		Firsts[CenterAgent.First]+=CenterAgent.Power;
	}
	
	//Szukanie maksimow
	int maxF=0,indF=-1;
	
	int offset=RANDOM(IleKate);
	assert(0<=offset && offset<IleKate);//Jak IleKate==2 to 0 albo 1 itd..
	
	for(int g=0;g<IleKate;g++)
	{
		int h=(g+offset)%IleKate;
		assert(h>=0 && h<IleKate);
		//Dodawanie szumu
		if(Firsts[h]>0)
			Firsts[h]+=long(DRAND()*Noise*(4.5*MaxSila));
		
		//Testowanie
		if(Firsts[h]>maxF)
		{
			maxF=Firsts[h];
			indF=h;
		}
		
	}
	
	if(indF!=-1)
	{
		CenterAgent.Second=indF;			//zmieniamy w agencie centralnym
		return 1;
	}
	else
	{
		CenterAgent.Second=CenterAgent.First;//Albo nic nie zmieniamy
		return 0;
	}
*/	
	return 0;
}
