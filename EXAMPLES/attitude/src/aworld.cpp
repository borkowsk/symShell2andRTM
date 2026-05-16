/// @file
/// @brief IMPLEMENTATION OF   W O R L D  FOR "attitudes" SIMULATION.
/// @date 2026-05-16 (modification)
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <limits.h>
//#include <assert.h>
//#include <string.h>
//#include <math.h>
#include <cstring>
#include <cmath>

#include "arand.h"
#include "aworld.h"
#include "histosou.hpp"
#include "clstsour.hpp" //Jest też "statsour" jakby co...
#include "coincsou.hpp"
#include "gadgets.hpp"
#include "wb_swap.hpp"
#include "wb_ptrio.h"

using namespace sym2;

const int RAMKA=4;
extern const char* SIMULATION_NAME;

// Konstrukcja agentów:
// ////////////////////

aagent::aagent(const aagent& ini)
    {
        if(&ini!=NULL) //TODO: to już nieeleganckie.
        {
            First=ini.First;
            Second=ini.Second;
            Power=RANDOM(max_str + 1);
        }
        else
            _clean();
    }

aagent::aagent()
    {
        _clean();
        First=RANDOM(n_of_cate);
        Second=0;
        Power=RANDOM(max_str + 1);
    }

// Statyczne pola aAgent-ów dla inicjalizacji:
// ///////////////////////////////////////////

short	aagent::str_grow=1;  //Maksymalny skok siły
short	aagent::max_str=256; //Maksymalna siła agenta
short	aagent::n_of_cate=256; //Liczba kategorii w mapach
short	aagent::cate_shift=0; //Przesuniecie dla wczytywania gifa
double	aagent::mutation_prob=0; //Prawd. spontanicznej zmiany poglądów (0..1)

// KONSTRUKCJA	ŚWIATA:
// ////////////////////

extern unsigned InternalLogLen;

aworld::aworld(size_t Width,		//Szerokość torusa macierzy agentów
      char* log_name,	//Nazwa pliku do zapisywania historii
      char* mapl_name,	//Nazwa (bit)mapy inicjującej "składowe"
      char* mapp_name,	//Nazwa (bit)mapy inicjującej "sily"
      char* live_mask,	//Czarne w tej mapie są kasowane
      double noise_p,	//Szum informacyjny
      short	max_str,	//Maksymalna siła agenta
      short	n_of_cate,	//Liczba kategorii w mapach
      short	neigh_radius,	//Rozmiar sąsiedztwa
      short	n_of_neigh, //8 == Gęstość sąsiedztwa
      short need_use_self, //Czy ma używać siebie
      bool	sync_update,
      short walk_str,
      short str_thres,
      double spon_prob
        ):
        world(log_name,50),
        MaplName(clone_str(mapl_name)), //Nazwa (bit)mapy 1. inicjującej agentów
        MappName(clone_str(mapp_name)), //Nazwa (bit)mapy 2. inicjującej agentów
        MaskName(clone_str(live_mask)), //Nazwa bitmapy maskującej (kasującej agentów)
    //Sub-obiekty właściwe dla tej symulacji
        MyWidth(Width),
        Agents(Width, Width, NULL), //Initer jest NULL, więc tworzone są konstruktorem, a nie klonowanie
        MaxSila(max_str),	//Maksymalna siła agenta
        ThrsStr(str_thres), //Siła dająca odporność na zmiany
        NofCateg(n_of_cate),	//Liczba kategorii w mapach
        NofNeigh(n_of_neigh),	//8 == Gęstość sąsiedztwa
        NeighRadius(neigh_radius),	//Rozmiar sąsiedztwa
        Noise(noise_p),
        UseSelf(need_use_self),
        SyncChange(sync_update),
        TakeAll(0), //Sąsiedztwo bez losowania
        //Wskaźniki do podstawowych seri danych
        Firsts(NULL),
        Seconds(NULL),
        Powers(NULL)
    { // Niewiele można zrobić, bo nie można tu jeszcze polegać na wirtualnych metodach klasy aWorld.
        aagent::str_grow=walk_str;
        //set_simulation_name("attitudes_v02");
        world::set_simulation_name(SIMULATION_NAME);
        aagent::mutation_prob=spon_prob;
        if(NofNeigh == -1)
            TakeAll=1;
    }

// Generuje podstawowe źródła dla wbudowanego manager-a danych lub innego:
// ///////////////////////////////////////////////////////////////////////

void aworld::make_basic_sources()
{
    sources_manager& WhatSourMen=this->Sources;
    world::make_basic_sources(); //Odziedziczone

    //Główne serie danych:
    Firsts=Agents.make_source("Attitude", &aagent::First);
    if(Firsts)
        Firsts->set_min_max(0, NofCateg - 1);
    Seconds=Agents.make_source("Prev. attitude", &aagent::Second);
    if(Seconds)
        Seconds->set_min_max(0, NofCateg - 1);

    Powers=Agents.make_source("Power", &aagent::Power);

    //Umieszczenie głównych serii w zarządcy serii:
    WhatSourMen.insert(Firsts);
    WhatSourMen.insert(Seconds);
    WhatSourMen.insert(Powers);
}


// Współpraca z zarządcą wyświetlania, a także logiem:
// ///////////////////////////////////////////////////

void aworld::make_default_visualisation()
//Rejestruje pochodne serie, tworzy domyślne "lufciki" i wkłada w "Manager"
{
    area_manager_base& Manager=this->MyAreaMenager();
    int iFirst=0,iSecond=0,iPower=0,iClassif=0;
    //Uzyskanie indeksów podstawowych serii z zarządcy
    {
    if(Firsts) iFirst=Sources.search(Firsts->name());
        else  goto ERROR;

    if(Seconds) iSecond=Sources.search(Seconds->name());
        else  goto ERROR;

    if(Powers) iPower=Sources.search(Powers->name());
        else  goto ERROR;

    if(Firsts)  iClassif=Sources.search(Firsts->name());
        else  goto ERROR;


    //Oraz utworzenie pochodnych serii statystycznych:
    generic_clustering_source*	FirstStat=new generic_clustering_source(Firsts);
    if(!FirstStat) goto ERROR;
        else	Sources.insert(FirstStat);

    generic_clustering_source*	SecondStat=new generic_clustering_source(Seconds);
    if(!SecondStat) goto ERROR;
        else	Sources.insert(SecondStat);

    //Źródło liczące statystykę i histogram z klasyfikacji:
    generic_histogram_source*  ClassStat=new generic_histogram_source(Firsts);
    if(!ClassStat) goto ERROR;
        else	Sources.insert(ClassStat);

    //A także utworzenie seri liczących ich wzajemne ko-statystyki:
    coincidence_source* CorrFS=new coincidence_source(Firsts, Seconds);
    if(!CorrFS) goto ERROR;
    Sources.insert(CorrFS); //Żeby została na końcu zwolniona, a poza tym może ktoś kiedyś użyje.

    fifo_source<double>* EntropyFSLog=new fifo_source<double>(CorrFS->Entropy(), InternalLogLen);
    if(!EntropyFSLog) goto ERROR;
    int iEntropyFS=Sources.insert(EntropyFSLog);

    fifo_source<double>* CorrFSLogR=new fifo_source<double>(CorrFS->Tau_a_Goodman_Kruskal(), InternalLogLen); //Kolejka "Fifo" dla korelacji pierwszych z drugimi
    if(!CorrFSLogR) goto ERROR;
    int iCorrFSR=Sources.insert(CorrFSLogR);


    //I utworzenie seri liczących ich statystyki:

    fifo_source<double>* StressFirstLog=new fifo_source<double>(FirstStat->Stress(), InternalLogLen); //Fifo, ze stresu
    if(!StressFirstLog) goto ERROR;
    int iSFirst=Sources.insert(StressFirstLog);

    fifo_source<double>* StressSecondLog=new fifo_source<double>(SecondStat->Stress(), InternalLogLen); //Fifo, ze stresu
    if(!StressSecondLog) goto ERROR;
    int iSSecond=Sources.insert(StressSecondLog);

    //iMainClassF,iWhichMainF,iNumClassF,
    fifo_source<double>* NumClassLog=new fifo_source<double>(ClassStat->NumOfClass(), InternalLogLen);
    if(!NumClassLog) goto ERROR;
    int iNumClassF=Sources.insert(NumClassLog);

    fifo_source<double>* ClassEntropyLog=new fifo_source<double>(ClassStat->Entropy(), InternalLogLen);
    if(!ClassEntropyLog) goto ERROR;
    int iClassEntropy=Sources.insert(ClassEntropyLog);

    fifo_source<double>* MainClassLog=new fifo_source<double>(ClassStat->MainClass(), InternalLogLen);
    if(!MainClassLog) goto ERROR;
    int iMainClassF=Sources.insert(MainClassLog);


    fifo_source<double>* WhichMainLog=new fifo_source<double>(ClassStat->WhichMain(), InternalLogLen);
    if(!WhichMainLog) goto ERROR;
    int iWhichMainF=Sources.insert(WhichMainLog);


    //I umieszczanie w logu tego, co trzeba:
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
    //WYMIARY DOMYŚLNEGO OKNA
    unsigned szer= Manager.get_width();
    unsigned wyso= Manager.get_height();
    assert(szer>50 && wyso>40); //Najmniejsze sensowne okno

    //Obszary domyślne — np. obszar STATUSU
    world::make_default_visualisation();
    if(OutArea)
    {
        OutArea->set(1,1,szer/2-1,wyso/2-1);
        Manager.as_original(Manager.search(OutArea->name()));
    }

    //WŁAŚCIWE LUFCIKI:
    graph* pom1=new sequence_graph(szer/2-1,wyso/4,szer-50,wyso/2-1,
                                    3,Sources.make_series_info(
                                            iNumClassF,iMainClassF,iWhichMainF,
                                                -1
                                            ).get_ptr_val(),
                                    0//* Z reskalowaniem
                                   );
    if(!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF CLASSIFICATION");
    Manager.insert(pom1);

    //inne mniej potrzebne
    graph* pom=new sequence_graph(szer/2-1,1,szer-50,wyso/4-1,	//domyślne współrzędne
                                    1,Sources.make_series_info(
                                            iSFirst,
                                                -1
                                            ).get_ptr_val(),
                                    //0// Z reskalowaniem
                                   1); //Wspolne minimum/maximum
    if(!pom) goto ERROR;
        pom->set_frame(128);
        pom->set_title("HISTORY OF STRESS");
    Manager.insert(pom);

    pom=new carpet_graph(1,wyso/2,szer/3,wyso-1,	//domyślne współrzędne
                            Firsts); //I źródlo danych
        pom->set_data_colors(0, 255);
        pom->set_title("Map of current attitude");
    Manager.insert(pom);

    pom=new bars_graph(szer/3+1,wyso/2,szer/3*2,wyso-1,	//domyślne współrzędne
                            ClassStat);
        pom->set_data_colors(0, 255);
        pom->set_title("Histogram of attitude");
    Manager.insert(pom);

    pom=new manhattan_graph(szer/3*2+1,wyso/2,szer,wyso-1,	//domyślne współrzędne
                                CorrFS,0,	//I źródło danych
                                CorrFS,0,
                                1,
                                0.22,		//Ułamek szerokości przeznaczony na perspektywę
                                0.77);		//Ułamek wysokości  przeznaczony na perspektywe
        pom->set_data_colors(0, 255);
        pom->set_text_colors(0);
        pom->set_title("Dynamism: curr. attit. vers. prev. attitude");
    Manager.insert(pom);

    //PRZYCISKI
    pom=new carpet_graph(szer-49,5*(char_height('X')+RAMKA),szer,6*(char_height('X')+RAMKA),	//domyślne współrzędne
                            Seconds); //I źródlo danych
        pom->set_data_colors(0, 255);
        pom->set_frame(0);
        pom->set_title("Map of previous attitude");
    Manager.insert(pom);


    pom=new carpet_graph(szer-49,6*(char_height('X')+RAMKA),szer,7*(char_height('X')+RAMKA),	//domyślne współrzędne
                            Powers); //I źródło danych
        pom->set_data_colors(0, 255);
        pom->set_frame(0);
        pom->set_title("Map of power");
    Manager.insert(pom);

    pom=new manhattan_graph(szer-49, 7*(char_height('X')+RAMKA),szer,8*(char_height('X')+RAMKA), 	//domyślne współrzędne
                            Powers,0, //I źródlo danych o wysokościach, niezarządzane
                            Firsts,0, //Zrodlo danych o kolorach — niezarządzane
                            1,		//Słupki zaczynają się co najmniej od 0!
                                        //Jesli 0 to zaczynają się od min>0
                            0.22,		//Ułamek szerokości przeznaczony na perspektywę
                            0.77		//Ułamek wysokości  przeznaczony na perspektywę
                            ); //I źródlo danych
        pom->set_data_colors(0, 255);
        pom->set_frame(0);
        pom->set_title("A composed map of strength & attitude of agents");
    Manager.insert(pom);

    pom1=new sequence_graph(szer-49, 9*(char_height('X')+RAMKA),szer,10*(char_height('X')+RAMKA),

                                    1,Sources.make_series_info(
                                            iClassEntropy,
                                                -1
                                            ).get_ptr_val(),
                                   1/*Wspólne minimum/maximum*/);
    if(!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF ENTROPY OF CLASSIFICATION");
    Manager.insert(pom1);


    pom1=new sequence_graph(szer-49, 10*(char_height('X')+RAMKA),szer,11*(char_height('X')+RAMKA),
                                    1,Sources.make_series_info(
                                            iEntropyFS,
                                                -1
                                            ).get_ptr_val(),
                                   1/*Wspolne minimum/maximum*/);
    if(!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF ENTROPY OF CHANGE");
    Manager.insert(pom1);


    pom=new sequence_graph(szer-49, 11*(char_height('X')+RAMKA),szer,12*(char_height('X')+RAMKA),
                                    1,Sources.make_series_info(
                                            iCorrFSR, //iCorrFS,
                                                -1
                                            ).get_ptr_val(),
                                    1
                                   );
    if(!pom) goto ERROR;
        pom->set_frame(128);
        pom->set_title("HISTORY OF Prev.TO Curr. CORRELATION");
    Manager.insert(pom);

    //Tworzenie obszaru sterującego:
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
        pom->set_background(10);
    Manager.insert(pom);
    }

    }
    Sources.new_data_version(1,1); //Oznajmia seriom, że dane się uaktualniły	(po inicjacji)

    ERROR://... tu akcja na niepogodę
        ; //error_message(...)
}



// AKCJE SYMULACYJNE:
// //////////////////

void aworld::after_read_from_image()
//actions after read state from file. Aktualizacja pol static aAgent'a!!!
{
    aagent::max_str=MaxSila; //Maksymalna siła agenta
    aagent::n_of_cate=NofCateg; //Liczba kategorii w mapach

    switch(NofCateg)
    {
    case   2:aagent::cate_shift=7;break;
    case   4:aagent::cate_shift=6;break;
    case   8:aagent::cate_shift=5;break;
    case  16:aagent::cate_shift=4;break;
    case  32:aagent::cate_shift=3;break;
    case  64:aagent::cate_shift=2;break;
    case 128:aagent::cate_shift=1;break;
    case 256:aagent::cate_shift=0;break;
    default:
        aagent::n_of_cate= NofCateg=256;
        aagent::cate_shift=0;
        cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
        Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
        break;
    }
}

// stan startowy symulacji
void aworld::initialize_layers()
//-------------------------------------
{
    static int first=1; //TYMCZASOWE WYŁĄCZENIE NADMIARU WYDRUKÓW!!!
    if(first)
        Log.GetStream()<<"attitude SIMULATION:";

    aagent::max_str=MaxSila; //Maksymalna siła agenta
    aagent::n_of_cate=NofCateg; //Liczba kategorii w mapach

    switch(NofCateg)
    {
    case   2:aagent::cate_shift=7;break;
    case   4:aagent::cate_shift=6;break;
    case   8:aagent::cate_shift=5;break;
    case  16:aagent::cate_shift=4;break;
    case  32:aagent::cate_shift=3;break;
    case  64:aagent::cate_shift=2;break;
    case 128:aagent::cate_shift=1;break;
    case 256:aagent::cate_shift=0;break;
    default:
        aagent::n_of_cate= NofCateg=256;
        aagent::cate_shift=0;
        cerr<<"Invalid number of class (not power of 2 less than 256). Using default.\n";
        Log.GetStream()<<"Invalid number of class (not power of 2). Using default.\n";
        break;
    }

    //...wydruk wartości parametrów symulacji
    if(first)
      Log.GetStream()
              << "\nMax Power=" << Log.separator() << MaxSila
              << "\nTresh of Power=" << Log.separator() << ThrsStr
              << "\nNum of Kl=" << Log.separator() << NofCateg
              << "\nNoise %=" << Log.separator() <<Noise*100
              << "\nSelf=" << Log.separator() << UseSelf
              << "\nNaighborhood=" << Log.separator() << NofNeigh << "/(" << (1 + 2 * NeighRadius) << "*" << (1 + 2 * NeighRadius) << ")\n";

    //			USTALANIE STANÓW AGENTÓW:
    // Wczytuje używając konstruktora lub klonowania, gdy go niema, wiec inicjuje resztę pól.
    int from1= Agents.init_from_bitmap(MappName.get_ptr_val(),&aagent::assignPow);
    int from2= Agents.init_from_bitmap(MaplName.get_ptr_val(),&aagent::assign123);

    //Gdy nie zainicjowane, to prowizoryczna inicjacja przez konstruktory lub klonowanie
    if(from1!=1 && from2!=1)
        Agents.reallocate_all();

    //Zabija agenta, gdy w masce jest czarny kolor
    if(Agents.init_from_bitmap(MaskName.get_ptr_val(), &aagent::killBlack) == 1 )
        Agents.deallocate_not_OK();

    first=0; //Koniec pierwszego wywołania. Potem wydruki już nie są potrzebne.
}

//Pojedynczy krok symulacji
void aworld::simulate_one_step()
//---------------------------------------
{
    const geometry_base* MyGeom=Agents.get_geometry();
    assert(MyGeom);

    if(SyncChange)
    {
        //Idziemy po agentach pełnym iterator-em, a stan agentów zmieniamy dopiero potem
        iterator_h Full=MyGeom->make_global_iterator();
        while(Full)
        {
            size_t index=MyGeom->get_next(Full); //Uzyskujemy index  agenta

            assert(index!=any_layer_base::FULL);				//... tutaj nie powinno się zdarzyć

            aagent& CenterAgent=*(Agents.get_ptr(index).get_ptr_val()); // Uzyskujemy referencje do agenta omijając asercje na NULL

            if(Agents.is_empty(CenterAgent))	// Sprawdzamy, czy nie jest to pusta kom�rka (NULL)
                continue;						// bo wtedy robić dalej byłoby bez sensu.

            if(CenterAgent.Power <= ThrsStr)		// Czy nie ma już immunitetu na zmiany
                CheckChange(MyGeom,index,CenterAgent); //Sprawdzamy zmianę stanu

        }
        // upewniamy się, że iterator zostanie usunięty
        MyGeom->destroy_iterator(Full);


        Full=MyGeom->make_global_iterator(); //Tworzymy nowy iterator i iterujemy od początku
        while(Full)
        {
            size_t index=MyGeom->get_next(Full); //Uzyskujemy index  agenta

            assert(index!=any_layer_base::FULL);				//... tutaj nie powinno się zdarzyć

            aagent& CenterAgent=*(Agents.get_ptr(index).get_ptr_val()); // Uzyskujemy referencje do agenta omijając asercje na NULL

            if(Agents.is_empty(CenterAgent))	// Sprawdzamy, czy nie jest to pusta kom�rka (NULL)
                continue;

            wb_swap(CenterAgent.First,CenterAgent.Second);  //Ma nowy stan
            CenterAgent.make_older();						//Robi się starszy
        }

        // upewniamy się, że iterator zostanie usunięty
        MyGeom->destroy_iterator(Full);

    }
    else
    {
        iterator_h Monte=MyGeom->make_random_global_iterator();	//Alokujemy iterator Monte-Carlo

        while(Monte) //Idziemy po agentach iterator-em Monte-Carlo. Niektórzy mogą się powtórzyć
        {
            size_t index=MyGeom->get_next(Monte); //Uzyskujemy index losowo wybranego agenta

            assert(index!=any_layer_base::FULL);				//... tutaj nie powinno się zdarzyć

            aagent& CenterAgent=*(Agents.get_ptr(index).get_ptr_val()); // Uzyskujemy referencje do agenta omijając asercje na NULL
            if(Agents.is_empty(CenterAgent))	// Sprawdzamy, czy nie jest to pusta kom�rka (NULL)
                continue;						// bo wtedy robić dalej byłoby bez sensu.

            if(CenterAgent.Power <= ThrsStr)		// Czy nie ma już immunitetu na zmiany
                if(CheckChange(MyGeom,index,CenterAgent)==1) //Czy zaszła zmiana stanu
                {
                    wb_swap(CenterAgent.First,CenterAgent.Second);
                }

            CenterAgent.make_older();			//Robi się starszy
        }

        // upewniamy się, że iterator zostanie usunięty
        MyGeom->destroy_iterator(Monte);
    }
}



int aworld::CheckChange(const geometry_base* MyGeom,
                        size_t index,
                        aagent& CenterAgent
                        ) //KOD NA SZUKANIE ZMIAN
{ 
    int testowanie=0;

    if(DRAND()<=aagent::mutation_prob) //Rzadka, spontaniczna zmiana poglądu
    {
        int atti=RANDOM(NofCateg);       	assert(0 <= atti && atti < NofCateg);
        CenterAgent.Second=atti;			//zmieniamy w agencie centralnym
        return 1;
    }


    //TABLICA POMOCNICZA
    wb_dynarray<int> Firsts(NofCateg);               assert(Firsts.IsOK());
    //Czyszczenie licznika
    memset(Firsts.get_ptr_val(),0, sizeof(int) * NofCateg);

    // Alokujemy iterator sąsiedztwa
    ::iterator_h Neigh=NULL;

    if(TakeAll)
    {
        Neigh=MyGeom->make_neighbour_iterator(index, NeighRadius);
    }
    else
    {
        Neigh=MyGeom->make_random_neighbour_iterator(index, NeighRadius, NofNeigh);
    }

    //`iterator_h Neigh=MyGeom->make_neighbour_iterator(index,NeighRadius);`
    unsigned zliczanie=0; //Zliczanie sasiad�w

    while(Neigh)
    {
        size_t index2=MyGeom->get_next(Neigh); //Uzyskujemy index sąsiada
        if(index2==any_layer_base::FULL || index2==index)	//Gdy poza obszarem symulacji lub w
            continue;				//centrum obszaru to dalej jest bez sensu.

        aagent& PeryfAgent=*(Agents.get_ptr(index2).get_ptr_val()); //Uzyskujemy referencje do sąsiada omijając asercje na NULL
        if(Agents.is_empty(PeryfAgent))		//Sprawdzamy, czy nie jest to pusta kom�rka (NULL)
            continue;					   // bo wtedy robić dalej byłoby bez sensu.

        zliczanie++;
        //Dodawanie sił sąsiadów do liczników w tablicach
        Firsts[PeryfAgent.First]+=PeryfAgent.Power;
    }

    MyGeom->destroy_iterator(Neigh);	// upewniamy się, że iterator zostanie usunięty
    //Zlicza wylosowanych agentów
    testowanie++;

    //Dodawanie własnych sił do liczników w tablicach
    if(UseSelf)
    {
        Firsts[CenterAgent.First]+=CenterAgent.Power;
    }

    //Szukanie maksimów
    int maxF=0,indF=-1;

    int offset=RANDOM(NofCateg);
    assert(0<=offset && offset < NofCateg); //Jak NofCateg==2 to 0 albo 1 itd..

    for(int g=0; g < NofCateg; g++)
    {
        int h= (g+offset) % NofCateg;
        assert(h>=0 && h < NofCateg);
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
        CenterAgent.Second=CenterAgent.First; //Albo nic nie zmieniamy
        return 0;
    }

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

