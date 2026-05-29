/// @file
/// @brief
///  @EN{ IMPLEMENTATION OF THE WORLD FOR "KattitudeS" SIMULATION. }
///  @PL{ IMPLEMENTACJA ŚWIATA DO SYMULACJI "Kattitudes". }
/// @date 2026-05-29 (modified)
///       ==========================================================
/// @details (attitudeS old example for SymShell)
//======================================================================================================================

#include <cstring>

#include "ka_rand.h"
#include "ka_world.h"
#include "histosou.hpp" //Histogram w starej wersji
#include "dhistosou.hpp"//I w nowszej
#include "clstsour.hpp"
#include "spatcors.hpp"
#include "coincsou.hpp"
#include "funcsour.hpp" 
#include "gadgets.hpp" 
#include "wb_ptrio.h"
#include "toitoutoll.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

using namespace sym2;

const int RAMKA=4;
extern const char* SIMULATION_NAME;


// Konstrukcja agentów:
//=====================
short ka_agent::DrawAttitude()
    {
        if(0<Majority && Majority<1) //Uwaga! Może być faktycznie w mniejszości!!!
        {
            if(DRAND()<Majority)
                return tos(Kate_num - 1); //Zawsze białe
            else
                return RANDOM(Kate_num - 1);
        }
        else
            return RANDOM(Kate_num);
    }

ka_agent::ka_agent(const ka_agent& ini)
    {
        First=ini.First;
        Second=ini.Second;
        Power=tos(1+RANDOM(Max_power)); //Siła jest przydzielana z rozkładu
        Press=-1; //Bo to "statystyka", więc nie ma sensu kopiować
        DurCh=false;
    }

ka_agent::ka_agent(const ka_agent* ini)
    {
        if(ini!=nullptr)
        {
            First=ini->First;
            Second=ini->Second;
            Power=tos(1+RANDOM(Max_power)); //Sila jest przydzielana z rozkładu
            Press=-1; //Bo to "statystyka", więc nie ma sensu kopiować
            DurCh=false;
        }
        else
            _clean();
    }

ka_agent::ka_agent()
    {
        _clean();
        First=DrawAttitude();
        Second=DrawAttitude();
        Power=tos(1+RANDOM(Max_power));
        Press=-1;
        DurCh=false;
    }

// Statyczne pola aagent-ów dla inicjalizacji:
//============================================

short	ka_agent::Power_change=1;	//Maksymalny skok siły
short	ka_agent::Max_power=256;	//Maksymalna siła agenta
short	ka_agent::Kate_num=256;		//Liczba kategorii w mapach
short	ka_agent::Kate_shift=0;		//Przesuniecie dla wczytywania gifa
double  ka_agent::Majority=-1;		//Domyślnie nie ma znaczącej większości!!!
double	ka_agent::MutationLevel=0;

ostream &operator<<(ostream &o, const ka_agent &a)
{
    o<<'{';
    o<<' '<<a.Power<<' '<<a.First<<' '<<a.Second<<' '<<a.Press<<' ';
    o<<'}';
    return o;
}

istream &operator>>(istream &i, ka_agent &a)
{
    char pom;
    i>>pom;		//ignoruje {
    i>>a.Power>>a.First>>a.Second>>a.Press;
    i>>pom;		//ignoruje }
    return i;
}
//Prawd. spontanicznej zmiany poglądów (0..1)

// KONSTRUKCJA	ŚWIATA:
//=====================
extern unsigned iInternalLogsLen;
extern unsigned iSpatialCorrelationMode;

ka_world::ka_world(
                    size_t	Width,	//Szerokość torusa macierzy agentów
                    char*	log_name,			//Nazwa pliku do zapisywania historii
                    char*	map_l_name,			//Nazwa pliku mapy inicjującej "składowe"
                    char*	mapp_name,			//Nazwa pliku mapy inicjującej "siły"
                    char*	live_mask,			//Czarne w tej mapie są kasowane
                    double	noise,				//Szum informacyjny
                    short	max_sila,			//Maksymalna siła agenta
                    short	ile_kate,			//Liczba kategorii w mapach
                    short	nei_radius,			//Rozmiar/promień sąsiedztwa
                    short	nei_density,		//8 == gęstość sąsiedztwa
                    double	need_use_self,		//Z jaką waga ma brać siebie
                    double	need_for_closure,	//Z jaką waga brać innych
                    bool	synchronously,
                    short	walk_power,
                    short	thr_power,
                    double	spontaneously,
                    double	fill,
                    double	migration_prob,
                    double	majority
        )
 : world(log_name,50),
   MapLName(clone_str(map_l_name)), MappName(clone_str(mapp_name)), MaskName(clone_str(live_mask)),
  //Sub-obiekty właściwe dla tej symulacji
    MyWidth(Width),
   Agents(Width, Width, NULL), //Initializer == nullptr, więc tworzeni przez konstruktory, a nie klonowanie
    MaxPower(max_sila),
   ThrPower(thr_power), //Siła dająca odporność na zmiany
    IleKate(ile_kate),
   NeiDens(nei_density),
   NeiSize(nei_radius),
   Noise(noise),
   LifeFill(fill),
   MigrationProb(migration_prob),
   WeightOfSelf(need_use_self),     //Z jaką wagą brać siebie pod uwagę (0..1).
    NeedForClosure(need_for_closure), //Z jaką wagą brani są inni. Domyślnie 1.
    Synchronic(synchronously),
   TakeAll(0), //Sąsiedztwo bez losowania
    //Wskaźniki do podstawowych seri danych
    Firsts(NULL), Seconds(NULL),
   Powers(NULL), //,Classify(NULL)
    ptrStres(NULL), ptrClsSize(NULL),
   ptrLastChanged(NULL), ptrLastMigration(NULL),
   CountCh(0), CountMig(0),
   Pressure(NULL), MaxPressure(0)
{// Niewiele można zrobić, gdy nie można tu jeszcze polegać na wirtualnych metodach klasy świat.
    ka_agent::Power_change=walk_power;
    ka_agent::Majority=majority;
    world::set_simulation_name(SIMULATION_NAME);
    ka_agent::MutationLevel=spontaneously;
    if(NeiDens == -1)
        TakeAll=1;
}

// Generuje podstawowe źródła dla wbudowanego zarządcy danych:
void ka_world::make_basic_sources()
{
    world::make_basic_sources(); //Odziedziczone
    sources_manager& WhatSourMen=this->Sources;

    //Główne serie:
    Firsts=Agents.make_source("Attitude", &ka_agent::First);
    if(Firsts)
        Firsts->set_min_max(0, IleKate - 1);
    Seconds=Agents.make_source("Prev. attitude", &ka_agent::Second);
    if(Seconds)
        Seconds->set_min_max(0, IleKate - 1);

    Powers=Agents.make_source("Power", &ka_agent::Power);
    Pressure=Agents.make_source("Pressure", &ka_agent::Press);

    MaxPressure= (MaxPower) * WeightOfSelf;
    MaxPressure+= (MaxPower) * NeedForClosure * (NeiDens > 0 ? NeiDens : sqr(NeiSize * 2 + 1) - 1   );
    Pressure->set_missing(-1);
    Pressure->set_min_max(0, MaxPressure);

    ptrLastChanged=new ptr_to_scalar_source<int>(nullptr,"Change cnt.");
    ptrLastMigration=new ptr_to_scalar_source<int>(nullptr,"Migration cnt.");

    //NIE DZIAŁA PRAWIDŁOWO?
    ptrLastChanged->set_missing(-1);
    ptrLastMigration->set_missing(-1);
    CountCh=default_missing< decltype(CountCh) >();
    ptrLastChanged->set_missing(CountCh);
    CountMig=default_missing< decltype(CountMig) >();
    ptrLastMigration->set_missing(CountMig);

    //Classify=Agents.make_source("Classification",&aagent::Classify); //Z PIERWOWZORU "LANGUAGES"
    //if(Classify)
    //	Classify->set_min_max(0,NofCat*NofCat*NofCat-1); //Max class ==NofCat^3 bo trzy niezależne płaszczyzny

    //Umieszczenie głównych serii w zarządcy serii.
    WhatSourMen.insert(Firsts);
    WhatSourMen.insert(Seconds);
    WhatSourMen.insert(Powers);
    WhatSourMen.insert(Pressure);
    WhatSourMen.insert(ptrLastChanged);
    WhatSourMen.insert(ptrLastMigration);

//WhatSourMen.insert(Classify);
}


//Wypisywanie/dopisywanie na konsole statusu.
void    ka_world::actualize_out_area()
{
    world::actualize_out_area();

    if(OutArea)
    {
        wb_pchar bufor(1024); //ze sporym zapasem.
        //assert(ptrStres->);
        double Stres=ptrStres->get();      //Zakładamy, że to źródła jednowartościowe.
        double ClsSiz=ptrClsSize->get();   //Oba są do przekazywania aktualnie najważniejszych danych na okno statusu.
        bufor.prn("Stress: %g \nApproximated cluster size: %g",Stres,ClsSiz);
        OutArea->add_text(bufor.get_ptr_val());
    }
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
//Rejestruje pochodne serie, tworzy domyślne "lufciki" i wkłada w zarządcę obszarów.
void ka_world::make_default_visualisation()
{
    area_manager_base& manager=this->MyAreaManager();
    int iFirst=0,iSecond=0,iPower=0,iPressure=0,iChangeCnt,iMigrationCnt;
    //Uzyskanie indeksów podstawowych serii z menadżera danych.
    {
    if(Firsts) iFirst=Sources.search(Firsts->name());
    else  goto ERROR;

    if(Seconds) iSecond=Sources.search(Seconds->name());
    else  goto ERROR;

    if(Powers)   iPower=Sources.search(Powers->name());
    else  goto ERROR;

    if(Pressure)  iPressure=Sources.search(Pressure->name());

    //Oraz utworzenie pochodnych serii statystycznych
    fifo_source<int>* LastChangedLog=new fifo_source<int>(ptrLastChanged, iInternalLogsLen);
    //if(!LastChangedLog) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    iChangeCnt=Sources.insert(LastChangedLog);

    fifo_source<int>* LastMigrationLog=new fifo_source<int>(ptrLastMigration, iInternalLogsLen);
    //if(!LastMigrationLog) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    iMigrationCnt=Sources.insert(LastMigrationLog);

    generic_clustering_source*	FirstStat=new generic_clustering_source(Firsts);
    //if(!FirstStat) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    Sources.insert(FirstStat);

    generic_clustering_source*	SecondStat=new generic_clustering_source(Seconds);
    //if(!SecondStat) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    Sources.insert(SecondStat);

    generic_basic_statistics_source* PressureStat=new generic_basic_statistics_source(Pressure);
    //if(!PressureStat) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    Sources.insert(PressureStat);


    //Źródło liczące statystykę i histogram z klasyfikacji
                                      //= new  generic_histogram_source(Firsts);
    generic_discrete_histogram_source*  ClassStat=new generic_discrete_histogram_source(0,this->IleKate,Firsts);
    //if(!ClassStat) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    Sources.insert(ClassStat);  //cerr<<ClassStat->name();

    generic_spatial_correlation_source* SpatialCorr=new generic_spatial_correlation_source(Firsts, -1, toi(iSpatialCorrelationMode) );
    //if(!SpatialCorr) goto ERROR;  // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    //int i Spatial Corr=
    Sources.insert(SpatialCorr);

    //A także utworzenie seri liczących ich wzajemne ko-statystyki
    coincidence_source* CorrFS=new coincidence_source(Firsts, Seconds);
    //if(!CorrFS) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    Sources.insert(CorrFS); //Żeby została kiedyś zwolniona, a poza tym może ktoś kiedyś skorzysta 🙂

    fifo_source<double>* EntropyFSLog=new fifo_source<double>(CorrFS->Entropy(), iInternalLogsLen);
    //if(!EntropyFSLog) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    int iEntropyFS=Sources.insert(EntropyFSLog);

    fifo_source<double>* CorrFSLogR=new fifo_source<double>(CorrFS->Tau_a_Goodman_Kruskal(), iInternalLogsLen); //Kolejka typu FIFO dla korelacji pierwszych z drugimi
    //if(!CorrFSLogR) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    int iCorrFSR=Sources.insert(CorrFSLogR);


    //I utworzenie seri liczących ich statystyki

    fifo_source<double>* MeanPressLog=new fifo_source<double>(PressureStat->Mean(), iInternalLogsLen); //Kolejka ze średniego chwilowego stresu
    //if(!MeanPressLog) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    int iMeanPress=Sources.insert(MeanPressLog);

    /*
    fifo_source<double>* StressFirstLog=new fifo_source<double>(FirstStat->Stress(),iInternalLogsLen);  //FIFO ze stresu klasycznego.
    if(!StressFirstLog) goto ERROR;
    int iSFirst=Sources.insert(StressFirstLog);

    fifo_source<double>* StressSecondLog=new fifo_source<double>(SecondStat->Stress(),iInternalLogsLen); //FIFO ze starego stresu.
    if(!StressSecondLog) goto ERROR;
    int iSSecond=Sources.insert(StressSecondLog);
    */

    fifo_source<double>* ClusterSizeLog=new fifo_source<double>(SpatialCorr->ApproximatedClusterSize(), iInternalLogsLen); //FIFO z rozmiaru klastra
    //if(!ClusterSizeLog) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    int iClusterSize=Sources.insert(ClusterSizeLog);

    //iMainClassF,iWhichMainF,iNumClassF,
    fifo_source<double>* NumClassLog=new fifo_source<double>(ClassStat->NumOfClass(), iInternalLogsLen);
    //if(!NumClassLog) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    int iNumClassF=Sources.insert(NumClassLog);

    fifo_source<double>* ClassEntropyLog=new fifo_source<double>(ClassStat->Entropy(), iInternalLogsLen);
    //if(!ClassEntropyLog) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    int iClassEntropy=Sources.insert(ClassEntropyLog);

    fifo_source<double>* MainClassLog=new fifo_source<double>(ClassStat->MainClass(), iInternalLogsLen);
    //if(!MainClassLog) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    int iMainClassF=Sources.insert(MainClassLog);

    fifo_source<double>* WhichMainLog=new fifo_source<double>(ClassStat->WhichMain(), iInternalLogsLen);
    //if(!WhichMainLog) goto ERROR; // Od C++11 new rzuca wyjątkiem, zamiast zwracać `nullptr`!
    int iWhichMainF=Sources.insert(WhichMainLog);

    ptrStres=FirstStat->Stress();
    ptrClsSize=SpatialCorr->ApproximatedClusterSize();

    // I umieszczanie w logu tych źródeł co trzeba:
    //---------------------------------------------
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
    for(int k=0;k<this->IleKate;k++) //Źródła histogramu muszą być na końcu, bo jest zmienna liczba.
    {
        Log.insert(ClassStat->Categories(k));
    }

    //PODSTAWOWA WIZUALIZACJA SERII DANYCH
    //WYMIARY DOMYŚLNEGO OKNA
    int w_width= manager.get_width();
    int w_height= manager.get_height();
    assert(w_width > 50 && w_height > 40); //Najmniejsze sensowne okno

    //Obszary domyślne. Np. obszar STATUSU
    world::make_default_visualisation();
    if(OutArea)
    {
        OutArea->set(1, 1, w_width / 2. - 1, w_height / 2. - 1);
        manager.as_original(manager.search(OutArea->name()));
    }

    //WŁAŚCIWE LUFCIKI DANYCH
    graph* pom1=new sequence_graph(w_width / 2 - 1, w_height / 4, w_width - 50, w_height / 2 - 1,
                                   3, Sources.make_series_info(
                                            iNumClassF,iMainClassF,iWhichMainF,
                                                -1
                                            ).get_ptr_val(),
                                   0//* Z reskalowaniem
                                   );
    //if(!pom1) goto ERROR; //Zbędne!
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF CLASSIFICATION");
    manager.insert(pom1);

    //inne mniej potrzebne
    graph* pom=new sequence_graph(w_width / 2 - 1, 1, w_width - 50, w_height / 4 - 1,	//domyślne współrzędne
                                   4, Sources.make_series_info(
                                            //iSSecond,
                                            iClusterSize,
                                            iChangeCnt,
                                            iMigrationCnt,
                                            iMeanPress,
                                            -1
                                            ).get_ptr_val(),
                                  0// Z reskalowaniem
                                   //1//Wspólne minimum/maximum
                                   );
    //if(!pom) goto ERROR; //Zbędne
        pom->set_frame(128);
        pom->set_title("HISTORY OF CLUSTERIZATION");
    manager.insert(pom);

    pom=new carpet_graph(1, w_height / 2, w_width / 3, w_height - 1, //domyślne współrzędne
                            Firsts); //I źródło danych...

        pom->set_data_colors(0, 255);
        pom->set_title("Map of current attitude");
    manager.insert(pom);

    pom=new bars_graph(w_width / 3 + 1, w_height / 2, w_width / 3 * 2, w_height - 1, //domyślne współrzędne  w_width-49,7*char_height('X')+7,w_width,8*char_height('X')+9
                            ClassStat);
        pom->set_data_colors(0, 255);
        pom->set_title("Histogram of attitude");
    manager.insert(pom);


    pom=new carpet_graph(w_width / 3 * 2 + 1, w_height / 2, w_width, w_height - 1, //domyślne współrzędne:  w_width-49,7*char_height('X')+7,w_width,8*char_height('X')+9
                            Pressure);
        pom->set_data_colors(0, 255);
        pom->set_title("Map of instantaneous social pressure");
    manager.insert(pom);

    //PRZYCISKI
    pom=new carpet_graph(w_width - 49,toi(5 * (char_height('X') + RAMKA)), w_width,toi(6 * (char_height('X') + RAMKA)), //domyślne współrzędne
                            Seconds); //I źródło danych...
        pom->set_data_colors(0, 255);
        pom->set_frame(0);
        pom->set_title("Map of previous attitude");
    manager.insert(pom);


    pom=new carpet_graph(w_width - 49,toi(6 * (char_height('X') + RAMKA)),w_width,toi(7 * (char_height('X') + RAMKA)), //domyślne współrzędne
                            Powers); //I źródło danych...
        pom->set_data_colors(0, 255);
        pom->set_frame(0);
        pom->set_title("Map of power");
    manager.insert(pom);

    pom=new manhattan_graph(w_width - 49,toi(7 * (char_height('X') + RAMKA)),w_width,toi(8 * (char_height('X') + RAMKA)), //domyślne współrzędne
                            Powers, 0, //I źródło danych o wysokościach (niezarządzane)
                            Firsts, 0, //źródło danych o kolorach (niezarządzane)
                            1,		//Słupki zaczynają się co najmniej od 0!
                                            //Jeśli 0 to zaczynają się od min>0
                            0.22,		//Ułamek szerokości przeznaczony na perspektywę
                            0.77		//Ułamek wysokości  przeznaczony na perspektywę
                            );
        pom->set_data_colors(0, 255);
        pom->set_frame(0);
        pom->set_title("A composed map of strength and attitude of agents");
    manager.insert(pom);

    pom=new manhattan_graph(w_width - 49,toi(8 * (char_height('X') + RAMKA)), w_width,toi(9 * (char_height('X') + RAMKA)), //domyślne współrzędne
                                CorrFS, 0,	//I dane
                                CorrFS, 0,
                            1,
                            0.22,		//Ułamek szerokości przeznaczony na perspektywę
                                0.77);	//Ułamek wysokości  przeznaczony na perspektywę
        pom->set_data_colors(0, 255);
        pom->set_text_colors(0);
        pom->set_title("Dynamism: curr. attitude vs. prev. attitude");
    manager.insert(pom);


    pom1=new sequence_graph(w_width - 49,toi(9 * (char_height('X') + RAMKA)), w_width,toi(10 * (char_height('X') + RAMKA)),
                            1, Sources.make_series_info(
                                            iClassEntropy,
                                                -1
                                            ).get_ptr_val(),
                            1/*Wspólne minimum/maximum*/);
    //if(!pom1) goto ERROR; //Zbędne sprawdzenie.
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF ENTROPY OF CLASSIFICATION");
    manager.insert(pom1);


    pom1=new sequence_graph(w_width - 49,toi(10 * (char_height('X') + RAMKA)), w_width,toi(11 * (char_height('X') + RAMKA)),
                            1, Sources.make_series_info(
                                            iEntropyFS,
                                                -1
                                            ).get_ptr_val(),
                            1/*Wspólne minimum/maximum*/);
    //if(!pom1) goto ERROR; //Po co? Od C++11 już niepotrzebne.
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF ENTROPY OF CHANGE");
    manager.insert(pom1);

    pom=new sequence_graph(w_width - 49,toi(11 * (char_height('X') + RAMKA)), w_width,toi(12 * (char_height('X') + RAMKA)),
                           1, Sources.make_series_info(
                                            iCorrFSR, //iCorrFS,
                                                -1
                                            ).get_ptr_val(),
                           1/*Wspólne minimum/maximum*/
                                   );
    //if(!pom) goto ERROR; //Zbędne!
        pom->set_frame(128);
        pom->set_title("HISTORY OF Prev.TO Curr. CORRELATION");
    manager.insert(pom);

    /*
    pom1=new sequence_graph(w_width-49, 12*(char_height('X')+RAMKA),w_width,13*(char_height('X')+RAMKA),
                                    1,Sources.make_series_info(
                                            iSpatialCorr,
                                                -1
                                            ).get_ptr_val(),
                                   0);

    */
    function_source_base* Linear=new function_source<y_eq_x>(SpatialCorr->get_size(), 0, (double)SpatialCorr->get_size(), "length");
    Sources.insert(Linear);
    pom1=new scatter_graph(w_width - 49,toi(12 * (char_height('X') + RAMKA)), w_width,toi(13 * (char_height('X') + RAMKA)),
                           Linear, 0,
                           SpatialCorr, 0);

    //if(!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("SPATIAL CORRELATION");
    manager.insert(pom1);

    //Tworzenie obszaru sterującego
    {
    wb_dynarray<rectangle_source_base*> tmp(4,(rectangle_source_base*)Sources.get(iFirst),
                                              (rectangle_source_base*)Sources.get(iSecond),
                                              //(rectangle_source_base*)Sources.get(iThird),
                                              (rectangle_source_base*)Sources.get(iPower),
                                              (rectangle_source_base*)Sources.get(iPressure),
                                              -1
                                              );
    drawable_base* pom_lok=new steering_wheel(w_width - 49, 0, w_width,toi(5 * char_height('X') + RAMKA), tmp);
                                                                                           //assert(pom_lok != nullptr);
    pom_lok->set_background(10);
    manager.insert(pom_lok);
    }

    }
    Sources.new_data_version(1,1); //Oznajmia seriom, że dane się uaktualniły (po inicjacji)

    ERROR://Tu "akcja na niepogodę"!
      cerr<<"Failed to initialize all visualization areas!"<<endl; //error_message(...)
}
#pragma clang diagnostic pop



// AKCJE SYMULACYJNE
//=====================

//Actions after read state from a file. Aktualizacja pól static aagent-a!!!
void ka_world::after_read_from_image()
{
    ka_agent::Max_power=MaxPower; //Maksymalna siła agenta
    ka_agent::Kate_num=IleKate; //Liczba kategorii w mapach

    switch(IleKate)
    {
    case   2:ka_agent::Kate_shift=7;break;
    case   4:ka_agent::Kate_shift=6;break;
    case   8:ka_agent::Kate_shift=5;break;
    case  16:ka_agent::Kate_shift=4;break;
    case  32:ka_agent::Kate_shift=3;break;
    case  64:ka_agent::Kate_shift=2;break;
    case 128:ka_agent::Kate_shift=1;break;
    case 256:ka_agent::Kate_shift=0;break;
    default:
        ka_agent::Kate_num= IleKate=256;
            ka_agent::Kate_shift=0;
        cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
        Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
        break;
    }
}

// stan startowy symulacji
void ka_world::initialize_layers()
//-------------------------------------
{
    static int first_call=1; //TYMCZASOWE WYŁĄCZENIE NADMIARU WYDRUKÓW!!!???

    if(first_call)
        Log.GetStream()<<"attitude SIMULATION:";

    ka_agent::Max_power=MaxPower; //Maksymalna siła agenta
    ka_agent::Kate_num=IleKate; //Liczba kategorii w mapach

    switch(IleKate)
    {
    case   2:ka_agent::Kate_shift=7;break;
    case   4:ka_agent::Kate_shift=6;break;
    case   8:ka_agent::Kate_shift=5;break;
    case  16:ka_agent::Kate_shift=4;break;
    case  32:ka_agent::Kate_shift=3;break;
    case  64:ka_agent::Kate_shift=2;break;
    case 128:ka_agent::Kate_shift=1;break;
    case 256:ka_agent::Kate_shift=0;break;
    default:
        ka_agent::Kate_num= IleKate=256;
            ka_agent::Kate_shift=0;
        cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
        Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
        break;
    }

    //Wydruk wartości parametrów symulacji
    if(first_call)
      Log.GetStream()
              << "\nMax. Power=" << Log.separator() << MaxPower
              << "\nThresh. of Power=" << Log.separator() << ThrPower
              << "\nNumber of Cat.=" << Log.separator() << IleKate
              << "\nNoise %=" << Log.separator() <<Noise*100
              << "\nSelf=" << Log.separator() << WeightOfSelf
              << "\nNeed for Cl.=" << Log.separator() << NeedForClosure
              << "\nNeighborhood=" << Log.separator() << NeiDens << "/(" << (1 + 2 * NeiSize) << "*" << (1 + 2 * NeiSize) << ")"
        <<"\n";

    //USTALANIE STANÓW AGENTÓW:
    //-------------------------

    //Wczytuje, używając konstruktora lub klonowania, gdy nie ma. Inicjuje resztę pól.
    int from1= Agents.init_from_bitmap(MappName.get_ptr_val(), &ka_agent::assignPow);
    int from2= Agents.init_from_bitmap(MapLName.get_ptr_val(), &ka_agent::assign_curr);
 //   int from3= Agents.init_from_bitmap(MapLName.get_ptr_val(),aagent::assign_prev);

    //Gdy nie zainicjowane, to prowizoryczna inicjacja przez konstruktory lub klonowanie
    if(from1!=1 && from2!=1)
        Agents.reallocate_all();

    //Zabija, gdy w masce jest czarny kolor
    if(Agents.init_from_bitmap(MaskName.get_ptr_val(), &ka_agent::killBlack) == 1 )
        Agents.deallocate_not_OK();

    if(LifeFill < 1) //Dealokacja nadmiarów
    {
        int how_many= toi( (1 - LifeFill) * (double)sqr(MyWidth) );
        Agents.clean_randomly(how_many);
    }

    first_call=0; //Koniec pierwszego wywołania //TYMCZASOWO!!!???
}

//Pojedynczy krok symulacji
void ka_world::simulate_one_step()
//---------------------------------------
{   
    CountCh=CountMig=0; //Zerowanie liczników dynamizmu
    const rectangle_geometry* MyGeom=dynamic_cast<const rectangle_geometry*>(Agents.get_geometry());
                                                                                                   assert(MyGeom!=nullptr);

    if(Synchronic)
    {
        //Idziemy po agentach pełnym iterator-em, a stan agentów zmieniamy dopiero potem
        iterator_h IGlobal=MyGeom->make_global_iterator();
        while(IGlobal)
        {
            size_t index=MyGeom->get_next(IGlobal); //Uzyskujemy index  agenta

            assert(index!=MyGeom->FULL);				//Tutaj nie powinno się zdarzyć

            ka_agent& CenterAgent=*(Agents.get_ptr(index).get_ptr_val()); // Uzyskujemy referencje do agenta omijając asercje na nullptr (?)

            if(Agents.is_empty(CenterAgent))	// Sprawdzamy, czy nie jest to pusta komórka (nullptr)
                continue;						// bo wtedy robić dalej byłoby bez sensu.

            if(CenterAgent.DurCh)
                continue;                       //Ten już był sprawdzany, tylko ze się przeniósł

            if(CenterAgent.Power <= ThrPower)		// Czy nie ma już immunitetu na zmiany
                CheckChange(MyGeom,index,CenterAgent); //Sprawdzamy zmianę stanu

        }
        // upewniamy się, że iterator zostanie usunięty
        MyGeom->destroy_iterator(IGlobal);

        IGlobal=MyGeom->make_global_iterator(); //Tworzymy nowy iterator i iterujemy od początku
        while(IGlobal)
        {
            size_t index=MyGeom->get_next(IGlobal); //Uzyskujemy index  agenta

            assert(index!=MyGeom->FULL);				//Tutaj nie powinno się zdarzyć

            ka_agent& CenterAgent=*(Agents.get_ptr(index).get_ptr_val()); // Uzyskujemy referencje do agenta omijając asercje na nullptr (?)

            if(Agents.is_empty(CenterAgent))	// Sprawdzamy, czy nie jest to pusta komórka (nullptr)
                continue;

            CenterAgent.update();  //Ma nowy stan

            CenterAgent.MakeOlder();						//Robi się starszy
        }

        // upewniamy się, że iterator zostanie usunięty
        MyGeom->destroy_iterator(IGlobal);

    }
    else
    {  
        //int ret=-1;
        iterator_h Monte=MyGeom->make_random_global_iterator(-1);	//Alokujemy iterator Monte-Carlo dla wszystkich (bo -1)

        while(Monte) //Idziemy po agentach iterator-em Monte-Carlo. Niektórzy mogą się powtórzyć
        {
            size_t index=MyGeom->get_next(Monte); //Uzyskujemy index losowo wybranego agenta
                                                            assert(index!=MyGeom->FULL); //Tutaj nie powinno się zdarzyć

            ka_agent& CenterAgent=*(Agents.get_ptr(index).get_ptr_val()); // Uzyskujemy referencje do agenta omijając asercje na nullptr (?)

            if(Agents.is_empty(CenterAgent))	// Sprawdzamy, czy nie jest to pusta komórka (nullptr)
                continue;						// bo wtedy robić dalej byłoby bez sensu.

            if(CenterAgent.Power <= ThrPower)		// Czy nie ma już immunitetu na zmiany
            {
                //ret=
                     CheckChange(MyGeom,index,CenterAgent); //Czy zaszła zmiana stanu
                                                       assert(CenterAgent.DurCh==true); //Czy flaga ustawiona prawidłowo
            }

            CenterAgent.update();
            CenterAgent.MakeOlder();			//Robi się starszy
        }

        // upewniamy się ze iterator zostanie usunięty
        MyGeom->destroy_iterator(Monte);
    }

    ptrLastChanged->change_ptr(&CountCh); //Alternatywna metoda dla oznaczenia braku/obecności policzonych danych
    ptrLastMigration->change_ptr(&CountMig); //Alternatywna metoda dla oznaczenia braku policzonych danych
}


long ka_world::DoMigration( const rectangle_geometry* MyGeom, //Ta procedura jest napisana nie-ogólnie, tj. w uzależnieniu od prostokątnego typu geometrii
                           size_t index,
                           ka_agent& /*CenterAgent*/
                         )
{
    size_t SouX,SouY;
    int    TarX,TarY;
    MyGeom->WhatCoordinates(index,SouX,SouY); //Nie ma co sprawdzać, czy dobrze, bo przecież było dobrze 😁
    
    do{
    TarX=RANDOM(MyGeom->get_width());
    TarY=RANDOM(MyGeom->get_height());
    }while(Agents.filled(TarX, TarY)); //Dopóki nie znajdzie pustego

    Agents.swap(TarX, TarY, SouX, SouY); //Zamienia miejsce
    return MyGeom->get(TarX,TarY);    //Nowa pozycja w postaci liniowej
}

int ka_world::CheckChange(const rectangle_geometry* MyGeom,
                          size_t index,
                          ka_agent& CenterAgent
                        ) //KOD NA SZUKANIE ZMIAN
{ 
    //int testowanie = 0; //DEBUG

    if(DRAND() <= ka_agent::MutationLevel) //Rzadka, spontaniczna zmiana poglądu
    {
        short attitude=RANDOM(IleKate);                                     assert(0 <= attitude && attitude < IleKate);
        CenterAgent.new_attitude(attitude); //Przygotowujemy zmianę w agencie centralnym
        return 1;
    }


    //TABLICA POMOCNICZA
    wb_dynarray<int> firsts(IleKate);                              assert(firsts.IsOK());
    //Czyszczenie licznika
    memset(firsts.get_ptr_val(), 0, sizeof(int) * IleKate);

    // Alokujemy iterator sąsiedztwa
    ::iterator_h Neigh=nullptr;

    if(TakeAll)
    {
        Neigh=MyGeom->make_neighbour_iterator(index, NeiSize);
    }
    else
    {
        Neigh=MyGeom->make_random_neighbour_iterator(index, NeiSize, NeiDens);
    }

    //iterator_h Neigh=MyGeom->make_neighbour_iterator(index,NeiSize);
    unsigned zliczanie=0; //Zliczanie sąsiadów

    while(Neigh)
    {
        size_t index2=MyGeom->get_next(Neigh); //Uzyskujemy index sąsiada
        if(index2==geometry::FULL || index2==index)	//Gdy poza obszarem symulacji lub w
            continue;				//centrum obszaru to dalej byłoby bez sensu.

        ka_agent& NeighAgent=*(Agents.get_ptr(index2).get_ptr_val()); //Uzyskujemy referencje do sąsiada omijając asercje na nullptr (???)
        if(Agents.is_empty(NeighAgent))		//Sprawdzamy, czy nie jest to pusta komórka (nullptr)
            continue;								// bo wtedy robić dalej byłoby bez sensu.

        zliczanie++;
        //Dodawanie sił sąsiadów do liczników w tablicach
        firsts[NeighAgent.First]+=toi(NeighAgent.Power * NeedForClosure); //W założeniu to nie są duże liczby
    }

    MyGeom->destroy_iterator(Neigh);	// upewniamy się ze iterator zostanie usunięty

    //Zlicza wylosowanych agentów
    //testowanie++; //DEBUG

    //Dodawanie własnych sił do liczników w tablicach, o ile `Weight Of Self > 0`
    if(WeightOfSelf>0)
    {
        firsts[CenterAgent.First]+=toi(CenterAgent.Power * WeightOfSelf); //W założeniu to nie są duże liczby.
    }

    //Szukanie maksimów
    int    maxF=0; ///< Jakie jest maksimum.
    short indF=-1; ///< Przy jakim poglądzie jest maksimum.

    int offset=RANDOM(IleKate);                     assert(0<=offset && offset<IleKate); //Jak NofCat==2 to 0 albo 1 itd..

    for(int g=0;g<IleKate;g++)
    {
        int h=(g+offset)%IleKate;                   assert(h>=0 && h<IleKate);
        //Dodawanie szumu
        if(firsts[h] > 0)
            firsts[h]+=toi(DRAND() * Noise * (4.5 * MaxPower));

        //Testowanie
        if(firsts[h] > maxF)
        {
            maxF=firsts[h];
            indF=tos(h);
        }

    }

    assert(indF!=-1); //Czy jest maksimum

    //Trzeba coś zrobić, ale co?
    if(MigrationProb != 0 && LifeFill < 1 && (MigrationProb == 1 || DRAND() < MigrationProb)  ) //Decyzja
    { //Migracja
        if(CenterAgent.First!=indF) //Jednak tylko, gdy trzeba zmienić pogląd
        {                                                   assert( maxF <= limit<decltype(CenterAgent.Press)>::Max() );
            CenterAgent.Press=tos(maxF); //Specjalny znacznik
            CenterAgent.new_attitude(CenterAgent.First); //więc nic nie zmieniamy w jego poglądach
            DoMigration(MyGeom,index,CenterAgent); //za to zmieniamy pozycje w świecie
            CountMig++;                      
            return 0; //Trzeba wyjść, bo "index" jest nieaktualny i zasygnalizować to wyżej
        }
        else
        {
            CenterAgent.Press=0;
            CenterAgent.new_attitude(CenterAgent.First); //nie potrzeba migrować więc nic nie zmieniamy
            return 0;
        }
    }
    else 
    { //Konformizm
        if(CenterAgent.First!=indF) //Gdy znaleziono coś nowego
        {                                                   assert( maxF <= limit<decltype(CenterAgent.Press)>::Max() );
            CenterAgent.Press=tos(maxF);
            CenterAgent.new_attitude(indF);		//zmieniamy w agencie centralnym            
            CountCh++;                             
            return 1;
        }
        else                                    //nie znaleziono
        {
            CenterAgent.Press=0;
            CenterAgent.new_attitude(CenterAgent.First); //więc nic nie zmieniamy
            return 0;
        }
    }
}

#pragma clang diagnostic pop
/* ****************************************************************** */
/*         SYMSHELL2 EXAMPLES version 2006/2022/2026                  */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/              */
/*        MAIL:  wborkowski@uw.edu.pl                                 */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */

