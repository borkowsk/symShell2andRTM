//#include <limits.h>
//#include <assert.h>
//#include <string.h>
//#include <math.h>
#include <string.h>
#include <math.h>

#include "arand.h"
#include "aworld.h"
#include "histosou.hpp" //Histogram w starej wersji
#include "dhistosou.hpp"//Nowsza wersja - poprawiona
#include "clstsour.hpp" //Jest tez statsour
#include "spatcors.hpp"
#include "coincsou.hpp"
#include "funcsour.hpp" 
#include "gadgets.hpp" 
#include "wb_ptrio.h"

const RAMKA=4;
extern const char* SYMULATION_NAME;


//Konstrukcja agentow
///////////////////////////////////
short aagent::DrawAttitude()
    {
        if(0<Majority && Majority<1) //Uwaga - moze byc faktycznie w mniejszosci!!!
        {
            if(DRAND()<Majority)
                return ile_kate-1; //Zawsze biale
            else
                return RANDOM(ile_kate-1);
        }
        else
            return RANDOM(ile_kate);
    }

aagent::aagent(const aagent& ini)
	{
		if(&ini!=NULL)
		{
			First=ini.First;
			Second=ini.Second;
			Power=1+RANDOM(max_sila);//Sila jest przydzielana z rozkladu
            Press=-1; //Bo to "statystyka" - nie ma sensu kopiowac
		}
		else
			_clean();
	}

aagent::aagent()
	{
		_clean();
		First=DrawAttitude();
		Second=DrawAttitude();
		Power=1+RANDOM(max_sila);
        Press=-1;
	}

//Statyczne pola aagentow dla inicjalizacji
////////////////////////////////////////////////////////////////
short	aagent::ruchsily=1;  //Maksymalny skok sily
short	aagent::max_sila=256;//Maksymalna sila agenta
short	aagent::ile_kate=256;//Ilosc kategori w mapach	
short	aagent::kate_shift=0;//Przesuniecie dla wczytywania gifa
double  aagent::Majority=-1; //Domyslnie nie ma znaczacej wiekszosci!!!
double	aagent::MutationLevel=0;//Prawd. spontanicznej zmiany pogladow (0..1)

//KONSTRUKCJA	SWIATA
////////////////////////////////////
extern unsigned internal_log;
extern unsigned spatial_correlation_mode;

aworld::aworld(size_t Width,		//Szerokosc torusa macierzy agentow
	  char* log_name,	//Nazwa pliku do zapisywania histori
	  char* mapl_name,	//Nazwa (bit)mapy inicjujacej "skladowe"
	  char* mapp_name,	//Nazwa (bit)mapy inicjujacej "sily"
	  char* live_mask,	//Czarne w tej mapie sa kasowane
	  double noise,		//Szum informacyjny
	  short	max_sila,	//Maksymalna sila agenta
	  short	ile_kate,	//Ilosc kategori w mapach	
	  short	odl_sasiad,	//Rozmiar sasiedztwa
	  short	ile_sasiad, //8==Gestosc sasiedztwa
	  double need_use_self,//Z jaka waga ma brac siebie
	  double need_for_closure,//Z jaka waga brac innych
	  bool	synchronicly,
	  short walkpower,
	  short trespower,
	  double spontanic,
      double fill,
      double migrprob,
      double majority
		):	
		world(log_name,50),		
		MaplName(clone_str(mapl_name)),//Nazwa (bit)mapy 1. inicjujacej agentow					
		MappName(clone_str(mapp_name)),//Nazwa (bit)mapy 2. inicjujacej agentow					
		MaskName(clone_str(live_mask)),//Nazwa bitmapy maskujacej (kasujacej agentow)
	//Sub-obiekty wlasciwe dla tej symulacji
		MyWidth(Width),
		Agenci(Width,Width,NULL),//Initer == NULL wiec tworzone przez konstruktor a nie klonowanie
		MaxSila(max_sila),	//Maksymalna sila agenta
		TrsSila(trespower), //Sila dajaca odporosc na zmiany
		IleKate(ile_kate),	//Ilosc kategori w mapach
		IleSasiad(ile_sasiad),	//8==Gestosc sasiedztwa
		OdlSasiad(odl_sasiad),	//Rozmiar sasiedztwa
		Noise(noise),
        Fill(fill),
        Migr(migrprob),
		WeightOfSelf(need_use_self),//Z jaka waga brac siebie pod uwage (0..1
		NeedForClosure(need_for_closure),//Z jaka waga brani s¹ inni domyslnie 1
		Synchronic(synchronicly),
		BierzWszystko(0), //Sasiedztwo bez losowania
		//Wskazniki do podstawowych seri danych
		Firsts(NULL),
		Seconds(NULL),
		Powers(NULL),//,Classif(NULL)
        ptrStres(NULL),
        ptrClsSize(NULL),
        ptrLastChanged(NULL),
        ptrLastMigration(NULL),
        CountCh(0),
        CountMig(0)
		{//!!!Niewiele mozna zrobic bo nie mozna tu jeszcze polegac na wirtualnych metodach klasy swiat
			aagent::ruchsily=walkpower;
            aagent::Majority=majority;
			//set_simulation_name("attitudes_v02");
			set_simulation_name(SYMULATION_NAME);
			aagent::MutationLevel=spontanic;
			if(IleSasiad==-1)
				BierzWszystko=1;
		}

//Generuje podstawowe zrodla dla wbudowanego menagera danych lub innego
////////////////////////////////////////////////////////////////////////////
void aworld::make_basic_sources(sources_menager& WhatSourMen)
{
world::make_basic_sources(WhatSourMen);//Odziedziczone

//Glowne serie 
Firsts=Agenci.make_source("Attitude",&aagent::First);	
if(Firsts)
	Firsts->setminmax(0,IleKate-1);	
Seconds=Agenci.make_source("Prev. attitude",&aagent::Second);
if(Seconds)
	Seconds->setminmax(0,IleKate-1);

Powers=Agenci.make_source("Power",&aagent::Power);
Pressure=Agenci.make_source("Pressure",&aagent::Press);

MaxPressure=long(MaxSila)*WeightOfSelf;
MaxPressure+=long(MaxSila)*NeedForClosure * (IleSasiad>0 ? IleSasiad : sqr(OdlSasiad*2+1)-1   );
Pressure->set_missing(-1);
Pressure->setminmax(0, MaxPressure  );

ptrLastChanged=new ptr_to_scalar_source<int>(NULL,"Change cnt.");
ptrLastMigration=new ptr_to_scalar_source<int>(NULL,"Migration cnt.");

//NIE DZIALA PRAWIDLOWO
ptrLastChanged->set_missing(-1);
ptrLastMigration->set_missing(-1);
CountCh=ptrLastChanged->get_missing();
CountMig=ptrLastMigration->get_missing();

//Classif=Agenci.make_source("Classification",&aagent::Classif); //Z PIERWOWZORU "LANGUAGES"
//if(Classif)
//	Classif->setminmax(0,IleKate*IleKate*IleKate-1);//Max class ==IleKate^3 bo trzy niezalezne plaszczyzny

//Umieszczenie glownych serii w menagerze serii
WhatSourMen.insert(Firsts);
WhatSourMen.insert(Seconds);
WhatSourMen.insert(Powers);
WhatSourMen.insert(Pressure);
WhatSourMen.insert(ptrLastChanged);
WhatSourMen.insert(ptrLastMigration);

//WhatSourMen.insert(Classif);
}


//Wspolpraca z menagerem wyswietlania a takze logiem
//------------------------------------------------------------------
//Wypisywanie/dopisywanie na konsole statusu
void    aworld::actualize_out_area()
{
    world::actualize_out_area();
//ptrStres; ptrClsSize; - Do przekazywania aktualnie najwazniejszych danych na okno statusu
    if(OutArea)
	{
        wb_pchar bufor(1024);//ze sporym zapsem
        //assert(ptrStres->);
        double Stres=ptrStres->get();      //Zakladamy ze to zrodla jednowartosciowe
        double ClsSiz=ptrClsSize->get();
        bufor.prn("Stress: %g \nAproximated cluster size: %g",Stres,ClsSiz);
        OutArea->add_text(bufor.get_ptr_val());
    }
}


void aworld::make_default_visualisation(area_menager_base& Menager)
//Rejestruje pochodne serie, tworzy domyslne "lufciki" i wklada w "Menager"
{
int iFirst=0,iSecond=0,iPower=0,iPressure=0,iChangeCnt,iMigratCnt;
//Uzyskanie indeksow podstawowych serii z menagera
{
if(Firsts) iFirst=Sources.search(Firsts->name());
	else  goto ERROR;

if(Seconds) iSecond=Sources.search(Seconds->name());
	else  goto ERROR;

if(Powers)   iPower=Sources.search(Powers->name());
	else  goto ERROR;

if(Pressure)  iPressure=Sources.search(Pressure->name());
	

//Oraz utworzenie pochodnych serii statystycznych
fifo_source<int>* LastChangedLog=new fifo_source<int>(ptrLastChanged,internal_log);
if(!LastChangedLog) goto ERROR;
iChangeCnt=Sources.insert(LastChangedLog);

fifo_source<int>* LastMigrationLog=new fifo_source<int>(ptrLastMigration,internal_log);
if(!LastMigrationLog) goto ERROR;
iMigratCnt=Sources.insert(LastMigrationLog);

generic_clustering_source*	FirstStat=new generic_clustering_source(Firsts);
if(!FirstStat) goto ERROR;
	else	Sources.insert(FirstStat);

generic_clustering_source*	SecondStat=new generic_clustering_source(Seconds);
if(!SecondStat) goto ERROR;
	else	Sources.insert(SecondStat);

generic_basic_statistics_source* PressureStat=new generic_basic_statistics_source(Pressure);
if(!PressureStat) goto ERROR;
	else	Sources.insert(PressureStat);


//Zrodlo liczace statystyke i histogram z klasyfikacji
                                  //=new  generic_histogram_source(Firsts);   
generic_discrete_histogram_source*  ClassStat=new generic_discrete_histogram_source(0,this->IleKate,Firsts);
if(!ClassStat) goto ERROR;
	else	Sources.insert(ClassStat);  //cerr<<ClassStat->name();

generic_spatial_correlation_source* SpatialCorr=new generic_spatial_correlation_source(Firsts,-1,spatial_correlation_mode);
if(!SpatialCorr) goto ERROR;
int iSpatialCorr=Sources.insert(SpatialCorr);

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

fifo_source<double>* MeanPressLog=new fifo_source<double>(PressureStat->Mean(),internal_log);//Fifo ze sredniego chwilowego stresu
if(!MeanPressLog) goto ERROR;
int iMeanPress=Sources.insert(MeanPressLog);
/*
fifo_source<double>* StressFirstLog=new fifo_source<double>(FirstStat->Stress(),internal_log);//Fifo ze stresu klasycznego
if(!StressFirstLog) goto ERROR;
int iSFirst=Sources.insert(StressFirstLog);

fifo_source<double>* StressSecondLog=new fifo_source<double>(SecondStat->Stress(),internal_log);//Fifo ze starego stresu
if(!StressSecondLog) goto ERROR;
int iSSecond=Sources.insert(StressSecondLog);
*/

fifo_source<double>* ClusterSizeLog=new fifo_source<double>(SpatialCorr->ApproximatedClusterSize(),internal_log);//Fifo z rozmiaru klastra
if(!ClusterSizeLog) goto ERROR;
int iClusterSize=Sources.insert(ClusterSizeLog);

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

ptrStres=FirstStat->Stress();
ptrClsSize=SpatialCorr->ApproximatedClusterSize();

//I umieszczanie w logu tego co trzeba
Log.insert(ClassStat->NumOfClass());
Log.insert(ClassStat->Entropy());
Log.insert(ClassStat->NormEntropy());
Log.insert(ClassStat->MainClass());
Log.insert(ClassStat->WhichMain());
//Log.insert(FirstStat->Stress());
Log.insert(PressureStat->Mean());
Log.insert(ptrLastChanged);
Log.insert(ptrLastMigration);
Log.insert(CorrFS->Entropy());
Log.insert(CorrFS->NormEntropy());
Log.insert(CorrFS->Chi2());
Log.insert(CorrFS->LevelOfFreedom());
Log.insert(CorrFS->V2Cramer());
Log.insert(CorrFS->T2Czupurow());
Log.insert(CorrFS->Tau_b_Goodman_Kruskal());
Log.insert(CorrFS->Tau_a_Goodman_Kruskal());
Log.insert(SpatialCorr->ApproximatedClusterSize());
for(int k=0;k<this->IleKate;k++) //Zrodla histogramu - musza byc na koncu bo zmien na liczba
{
    Log.insert(ClassStat->Class(k));
}

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
							   4,Sources.make_series_info(
                                        //iSSecond,
                                        iClusterSize,
                                        iChangeCnt,
                                        iMigratCnt,
										iMeanPress,                                        
											-1
										).get_ptr_val(),
								0// Z reskalowaniem 
							   //1//Wspolne minimum/maximum
                               );
if(!pom) goto ERROR;
pom->setframe(128);
pom->settitle("HISTORY OF CLUSTERISATION");
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


pom=new carpet_graph(szer/3*2+1,wyso/2,szer,wyso-1,//domyslne wspolrzedne,//domyslne wspolrzedne  szer-49,7*char_height('X')+7,szer,8*char_height('X')+9
						Pressure);
pom->setdatacolors(0,255);
pom->settitle("Map of instantaneous social pressure");
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

pom=new manhattan_graph(szer-49, 8*(char_height('X')+RAMKA),szer,9*(char_height('X')+RAMKA),//domyslne wspolrzedne
						    CorrFS,0,	//I zrodlo danych
							CorrFS,0,
							1,
							0.22,		//Ulamek szerokosci przeznaczony na perspektywe
							0.77);		//Ulamek wysokosci  przeznaczony na perspektywe
pom->setdatacolors(0,255);
pom->settextcolors(0);
pom->settitle("Dynamism: curr. attit. vers. prev. attitude");
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
								1/*Wspolne minimum/maximum*/
							   );
if(!pom) goto ERROR;
pom->setframe(128);
pom->settitle("HISTORY OF Prev.TO Curr. CORRELATION");
Menager.insert(pom);

/*
pom1=new sequence_graph(szer-49, 12*(char_height('X')+RAMKA),szer,13*(char_height('X')+RAMKA),						
							    1,Sources.make_series_info(
										iSpatialCorr,
											-1
										).get_ptr_val(),
							   0);

*/ 
function_source_base* Linear=new function_source<yeqx>(SpatialCorr->get_size(),0,SpatialCorr->get_size(),"lenght");
Sources.insert(Linear);
pom1=new scatter_graph(szer-49, 12*(char_height('X')+RAMKA),szer,13*(char_height('X')+RAMKA),
                       Linear,0,
                       SpatialCorr,0);	
                           
if(!pom1) goto ERROR;
pom1->setframe(128);
pom1->settitle("SPATIAL CORRELATION");
Menager.insert(pom1);

//Tworzenie obszaru sterujacego
{
wb_dynarray<rectangle_source_base*> tmp(4,(rectangle_source_base*)Sources.get(iFirst),
										  (rectangle_source_base*)Sources.get(iSecond),
										  //(rectangle_source_base*)Sources.get(iThird),
										  (rectangle_source_base*)Sources.get(iPower),
										  (rectangle_source_base*)Sources.get(iPressure),
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
void aworld::after_read_from_image()
//actions after read state from file. Aktualizacja pol static aagent'a!!!
{
	aagent::max_sila=MaxSila;//Maksymalna sila agenta
	aagent::ile_kate=IleKate;//Ilosc kategori w mapach	
	
	switch(IleKate)
	{
	case   2:aagent::kate_shift=7;break;
	case   4:aagent::kate_shift=6;break;
	case   8:aagent::kate_shift=5;break;
	case  16:aagent::kate_shift=4;break;
	case  32:aagent::kate_shift=3;break;
	case  64:aagent::kate_shift=2;break;
	case 128:aagent::kate_shift=1;break;
	case 256:aagent::kate_shift=0;break;
	default:
		aagent::ile_kate=IleKate=256;
		aagent::kate_shift=0;
		cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
		Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
		break;
	}
}

// stan startowy symulacji
void aworld::initialize_layers()
//-------------------------------------
{
	static first=1;//TYMCZASOWE WYLACZENIE NADMIARU WYDRUKOW!!!
	if(first)
		Log.GetStream()<<"attitude SIMULATION:";
	//odl_sasiad=1,//Rozmiar sasiedztwa
	//ile_sasiad=8 //8==Gestosc sasiedztwa
	aagent::max_sila=MaxSila;//Maksymalna sila agenta
	aagent::ile_kate=IleKate;//Ilosc kategori w mapach	
	
	switch(IleKate)
	{
	case   2:aagent::kate_shift=7;break;
	case   4:aagent::kate_shift=6;break;
	case   8:aagent::kate_shift=5;break;
	case  16:aagent::kate_shift=4;break;
	case  32:aagent::kate_shift=3;break;
	case  64:aagent::kate_shift=2;break;
	case 128:aagent::kate_shift=1;break;
	case 256:aagent::kate_shift=0;break;
	default:
		aagent::ile_kate=IleKate=256;
		aagent::kate_shift=0;
		cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
		Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
		break;
	}
	
	//...wydruk wartosci parametrow symulacji
	if(first)
	  Log.GetStream()
		<<"\nMax Power="<<Log.separator()<<MaxSila
		<<"\nTresh of Power="<<Log.separator()<<TrsSila
		<<"\nNum of Kl="<<Log.separator()<<IleKate
		<<"\nNoise %="<<Log.separator()<<Noise*100
		<<"\nSelf="<<Log.separator()<<WeightOfSelf
		<<"\nNforC="<<Log.separator()<<NeedForClosure
		<<"\nNeighborhood="<<Log.separator()<<IleSasiad<<"/("<<(1+2*OdlSasiad)<<"*"<<(1+2*OdlSasiad)<<")"
        <<"\n";// //bylo Naighborhood
	
	//			USTALANIE STANÓW AGENTÓW
	//Wczytuje uzywajac konstruktora lub klonowania gdy niema, wiec inicjuje reszte pól.
	int from1= Agenci.init_from_bitmap(MappName.get_ptr_val(),aagent::assignPow);
	int from2= Agenci.init_from_bitmap(MaplName.get_ptr_val(),aagent::assign_curr);
 //   int from3= Agenci.init_from_bitmap(MaplName.get_ptr_val(),aagent::assign_prev);
	
	//Jesli nie zainicjowane to prowizoryczna inicjacja przez konstruktory lub klonowanie
	if(from1!=1 && from2!=1)
		Agenci.reallocate_all();
	
	//Zabija m jesli w masce jesc czarny kolor
	if(Agenci.init_from_bitmap(MaskName.get_ptr_val(),aagent::killBlack)==1 )
		Agenci.deallocate_not_OK();

    if(Fill<1)//Dealokacja nadmiarow
    {
        size_t how_many=(1-Fill)*sqr(MyWidth);
        Agenci.clean_randomly(how_many);
    }
	first=0;//Koniec pierwszego wywolania //TYMCZASOWO!!!
}

//Pojedynczy krok symulacji
void aworld::simulate_one_step()
//---------------------------------------
{   
    CountCh=CountMig=0; //Zerowanie licznikow dynamizmu
	const rectangle_geometry* MyGeom=dynamic_cast<const rectangle_geometry*>(Agenci.get_geometry());
	                                                                                               assert(MyGeom!=NULL);
	
	if(Synchronic)
	{
		//Idziemy po agentach pelnym iteratorem a stan agentow zmieniamy dopiero potem
		iteratorh IGlobal=MyGeom->make_global_iterator();
		while(IGlobal)
		{
			size_t index=MyGeom->get_next(IGlobal);//Uzyskujemy index  agenta	
					 
			assert(index!=MyGeom->FULL);				//... tutaj nie powinno sie zdarzyc

			aagent& CenterAgent=*(Agenci.get_ptr(index).get_ptr_val());// Uzyskujemy referencje do agenta omijajac asercje na NULL
			
			if(Agenci.is_empty(CenterAgent))	// Sprawdzamy czy nie jest to pusta komórka (NULL)
				continue;						// bo wtedy robic dalej by³oby bez sensu.
			
            if(CenterAgent.DurCh)
                continue;                       //Ten juz byl sprawdzany, tylko ze sie przeniosl

			if(CenterAgent.Power<=TrsSila)		// Czy nie ma juz immunitetu na zmiany
				CheckChange(MyGeom,index,CenterAgent); //Sprawdzamy zmiane stanu
				
		}
		// upewniamy sie ze iterator zostanie usuniety
		MyGeom->destroy_iterator(IGlobal);

		
		IGlobal=MyGeom->make_global_iterator();//Tworzymy nowy iterator i iterujemy od poczatku
		while(IGlobal)
		{
			size_t index=MyGeom->get_next(IGlobal);//Uzyskujemy index  agenta	
					 
			assert(index!=MyGeom->FULL);				//... tutaj nie powinno sie zdarzyc

			aagent& CenterAgent=*(Agenci.get_ptr(index).get_ptr_val());// Uzyskujemy referencje do agenta omijajac asercje na NULL
			
			if(Agenci.is_empty(CenterAgent))	// Sprawdzamy czy nie jest to pusta komórka (NULL)
				continue;

			CenterAgent.update();  //Ma nowy stan
			
            CenterAgent.MakeOlder();						//Robi sie starszy
		}	
		
		// upewniamy sie ze iterator zostanie usuniety
		MyGeom->destroy_iterator(IGlobal);

	}
	else
    {  
        int ret=-1;
		iteratorh Monte=MyGeom->make_random_global_iterator();	//Alokujemy iterator Monte-Carlo
		
		while(Monte)//Idziemy po agentach iteratorem Monte-Carlo. Niektórzy moga sie powtórzyc
		{	
			size_t index=MyGeom->get_next(Monte);//Uzyskujemy index losowo wybranego agenta	
		
			assert(index!=MyGeom->FULL);				//... tutaj nie powinno sie zdarzyc
			
			aagent& CenterAgent=*(Agenci.get_ptr(index).get_ptr_val());// Uzyskujemy referencje do agenta omijajac asercje na NULL
			
            if(Agenci.is_empty(CenterAgent))	// Sprawdzamy czy nie jest to pusta komórka (NULL)
				continue;						// bo wtedy robic dalej by³oby bez sensu.
			
			if(CenterAgent.Power<=TrsSila)		// Czy nie ma juz immunitedu na zmiany
            {
				ret=CheckChange(MyGeom,index,CenterAgent);//Czy zaszla zmiana stanu
                                             assert(CenterAgent.DurCh==true);//Czy flaga ustawiona prawidlowo
            }
				
			CenterAgent.update();				
			CenterAgent.MakeOlder();			//Robi sie starszy
		}
		
		// upewniamy sie ze iterator zostanie usuniety
		MyGeom->destroy_iterator(Monte);
	}
	
    ptrLastChanged->change_ptr(&CountCh); //Alternatywna metoda dla oznacznia braku/obecnosci policzonych danych
    ptrLastMigration->change_ptr(&CountMig);//Alternatywna metoda dla oznacznia braku policzonych danych
}


int aworld::DoMigration(const rectangle_geometry* MyGeom, //Ta procedura jest napisana nie-ogolnie, tj. w uzaleznieniu od prostokatnego typu geometrii
						size_t index,
						aagent& CenterAgent
						)
{
    size_t SouX,SouY,TarX,TarY;
    MyGeom->WhatCoordinates(index,SouX,SouY);//Nie ma co sprawdzac czy dobrze bo przeciez bylo dobrze
    
    do{
    TarX=RANDOM(MyGeom->get_width());
    TarY=RANDOM(MyGeom->get_height());
    }while(Agenci.filled(TarX,TarY));//Dopuki nie znajdzie pustego

    Agenci.swap(TarX,TarY,SouX,SouY);//Zamienia miejsce
    return MyGeom->get(TarX,TarY);//Nowa pozycja w postaci liniowej 
}

int aworld::CheckChange(const rectangle_geometry* MyGeom,
						size_t index,
						aagent& CenterAgent
						)//KOD NA SZUKANIE ZMIAN
{ 
	int testowanie=0;
	
	if(DRAND()<=aagent::MutationLevel)//Rzadka, spontaniczna zmiana pogladu
	{
		int atti=RANDOM(IleKate);
		assert(0<=atti && atti<IleKate);
		
        CenterAgent.new_attitude(atti);         //zmieniamy w agencie centralnym

		return 1;
	}
	
	
	//TABLICA POMOCNICZA
	wb_dynarray<int> Firsts(IleKate);
	assert(Firsts.IsOK());
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
		if(index2==MyGeom->FULL || index2==index)	//Jesli poza obszarem symulacji lub w 
			continue;				//centrum obszaru to dalej byloby bez sensu.
		
		aagent& PeryfAgent=*(Agenci.get_ptr(index2).get_ptr_val());//Uzyskujemy referencje do sasiada omijajac asercje na NULL
		if(Agenci.is_empty(PeryfAgent))		//Sprawdzamy czy nie jest to pusta komórka (NULL)
			continue;					   // bo wtedy robic dalej by³oby bez sensu.
		
		zliczanie++;
		//Dodawanie sil sasiadow do licznikow w tablicach
		Firsts[PeryfAgent.First]+=unsigned(PeryfAgent.Power*NeedForClosure);//W za³o¿eniu to nie sa duze liczby
	}
	
	MyGeom->destroy_iterator(Neigh);	// upewniamy sie ze iterator zostanie usuniety
	//Zlicza wylosowanych agentow
	testowanie++;	
	
	//Dodawanie wlasnych sil do licznikow w tablicach o ile WeightOfSelf>0
	if(WeightOfSelf)
	{
		Firsts[CenterAgent.First]+=unsigned(CenterAgent.Power*WeightOfSelf);//W za³o¿eniu to nie sa duze liczby
	}
	
	//Szukanie maksimow
	int maxF=0,indF=-1;
	
	int offset=RANDOM(IleKate);                     assert(0<=offset && offset<IleKate);//Jak IleKate==2 to 0 albo 1 itd..
	
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

    assert(indF!=-1);//Czy jest maksimum 

    //Trzeba cos zrobic, ale co?
    if(Migr!=0 &&  Fill<1 && (Migr==1 || DRAND()<Migr)  )//Decyzja 
    { //Migracja
        if(CenterAgent.First!=indF) //Ale tylko gdy trzebaby zmienic poglad
        {
            CenterAgent.Press=maxF;//Specjalny znacznik 
            CenterAgent.new_attitude(CenterAgent.First); //wiec nic nie zmieniamy w jego pogladach
            DoMigration(MyGeom,index,CenterAgent);//za to zmieniamy pozycje w swiecie
            CountMig++;                      
            return 0;//Trzeba wyjsc bo "index" jest nieaktualny i zasygnalizowac to wyzej
        }
        else
        {
            CenterAgent.Press=0;
            CenterAgent.new_attitude(CenterAgent.First);//nie potrzeba migrowac wiec nic nie zmieniamy                                 
            return 0;
        }
    }
    else 
    { //Konformizm
        if(CenterAgent.First!=indF) //Jesli znaleziono cos nowego
        {
            CenterAgent.Press=maxF;
            CenterAgent.new_attitude(indF);		//zmieniamy w agencie centralnym            
            CountCh++;                             
            return 1;
        }
        else                                    //nie znaleziono
        {
            CenterAgent.Press=0;
            CenterAgent.new_attitude(CenterAgent.First);//wiec nic nie zmieniamy     
            return 0;
        }
    }
}
