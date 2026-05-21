/// @file
/// @brief
///  @EN{ IMPLEMENTATION OF THE "KWORLD".  }
///  @PL{  }
/// @date 2026-05-21 (modified)
/// =========================================================
/// @details
///  (old example for SymShell implementing Kruglansky like model)
//======================================================================================================================

#include <cstring>
#include <cmath>
#include "krand.h"
#include "kWorld.h"
#include "wb_ptrio.h"
#include "toitoutoll.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

using namespace sym2;
using namespace sym2::data;

extern const int RAMKA=4;
extern const char* SIMULATION_NAME;

// Statyczne pola `kAgent`-ów:
//============================

short	kAgent::Max_power=256;		// Maksymalna siła agenta.
int     kAgent::Threshold=256;		// Granica domknięcia poglądu.
double  kAgent::Majority=0.10;		// Początkowa liczba czarnych, czyli lewych.
double  kAgent::Minority=0.05;		// Początkowa liczba białych, czyli prawych.
double	kAgent::NoiseLevel=0;       // Prawd. spontanicznej zmiany.

// IMPLEMENTACJA KONSTRUKCJI ŚWIATA:
//==================================

extern unsigned internal_log;
extern unsigned spatial_correlation_mode;

kWorld::kWorld(size_t	Width,				// Określa ile kolumn torusa macierzy agentów.
               char*	log_name,			// Nazwa pliku do zapisywania historii.
               char*	map_l_name,			// Nazwa pliku mapy inicjującej "składowe".
               char*	mapp_name,			// Nazwa pliku mapy inicjującej "siły".
               char*	live_mask,			// Czarne w tej mapie są kasowane.
               double	noise,				// Szum informacyjny.
               short	max_str,			// Maksymalna siła agenta.
               short	how_many_neigh,		// 8 == gęstość losowania sąsiedztwa
               double	how_use_self,		// Z jaka waga ma brać siebie.
               double	need_for_closure,	// Główny parametr modelu. Domyślnie 1.
               bool		synchronously,
               short	threshold,
               double	spontaneously,		// Prawdopodobieństwo spontanicznej zmiany stanu.
               double	fill,
               double	migration_prob,
               double	majority,
               double	minority
               )
  : world(log_name,50),
    MapLName(clone_str(map_l_name)),
    MappName(clone_str(mapp_name)),
    MaskName(clone_str(live_mask)),
    //Sub-obiekty właściwe dla tej symulacji:
    MyWidth(Width),
    Agenci(Width,Width,nullptr), //`Initer == nullptr`, więc tworzone przez konstruktor, a nie klonowanie
    MaxStrength(max_str),
    Threshold(threshold),
    NeighDens(how_many_neigh),
    Noise(noise),
    Fill(fill),
    MigrationP(migration_prob),
    WeightOfSelf(how_use_self),
    NeedForClosure(need_for_closure),
    Synchronic(synchronously),
    //Wskaźniki do podstawowych seri danych
    Firsts(nullptr),
    Seconds(nullptr),
    Powers(nullptr), //,classif(nullptr)
    ForRight(nullptr),
    ForLeft(nullptr),
    ptrStres(nullptr),
    ptrClsSize(nullptr),
    ptrLastChanged(nullptr),
    ptrLastMigration(nullptr),
    CountCh(0),CountMig(0),MaxPressure(0)
{// Nie można tu jeszcze polegać na wirtualnych metodach tej klasy!!!
    kAgent::Majority=majority;
    kAgent::Minority=minority;
    world::set_simulation_name(SIMULATION_NAME);
    kAgent::Max_power=MaxStrength; //Maksymalna siła agenta
    kAgent::NoiseLevel=spontaneously;
}


// AKCJE SYMULACYJNE:
//===================

void kWorld::after_read_from_image()
//Actions after read state from a file. Aktualizacja pol static kAgent-a!!!
{   
    //NIEZAIMPLEMENTOWANE...NIC DO ROBOTY!
}

// stan startowy symulacji
void kWorld::initialize_layers()
//-------------------------------------
{
    kAgent::Max_power=MaxStrength; //Maksymalna siła agenta
    
    static int first=1; //EWENTUALNE WYŁĄCZENIE WYDRUKÓW GDY SYMULACJA
    
    //Wydruk wartości parametrów symulacji
    if(first)
        Log.GetStream()
                << "SIMULATION:" << SIMULATION_NAME
                << "\nMax Power=" << Log.separator() << MaxStrength
        //		<<"\nThresh of Power="<<Log.separator()<<TrsSila
        //		<<"\nNum of Kl="<<Log.separator()<<NofCat
            <<"\nNoise %="<<Log.separator()<<Noise*100
        //		<<"\nSelf="<<Log.separator()<<WeightOfSelf
            <<"\nNforC="<<Log.separator()<<NeedForClosure
        //		<<"\nNeighborhood="<<Log.separator()<<NeighDens<<"/("<<(1+2*NeighRadius)<<"*"<<(1+2*NeighRadius)<<")\n"
            <<endl;
    
    //			USTALANIE STANÓW AGENTÓW 
    //Wczytuje, używając konstruktora lub klonowania, gdy niema, więc inicjuje resztę pól.
    int from1= Agenci.init_from_bitmap(MappName.get_ptr_val(),&kAgent::assignPow);
    int from2= Agenci.init_from_bitmap(MapLName.get_ptr_val(), &kAgent::assign_curr);
    //   int from3= Agents.init_from_bitmap(MapLName.get_ptr_val(),kAgent::assign_prev);
    
    //Gdy niezainicjowane z pliku to prowizoryczna inicjacja przez konstruktory lub klonowanie
    if(from1!=1 && from2!=1)
        Agenci.reallocate_all();
    
    //Zabija agenta, gdy w masce jest czarny kolor
    if(Agenci.init_from_bitmap(MaskName.get_ptr_val(),&kAgent::killBlack) == 1 )
        Agenci.deallocate_not_OK();
    
    if(Fill<1) //Dealokacja nadmiarów
    {
        int how_many=toi((1.0-Fill)*(double)sqr(MyWidth));
        Agenci.clean_randomly(how_many);
    }

    first=0; //Koniec pierwszego wywołania. WYŁĄCZA WYPISYWANIE PARAMETRÓW W KOLEJNYCH POWTÓRZENIACH SYMULACJI.
}

//Pojedynczy krok symulacji
void kWorld::simulate_one_step()
//---------------------------------------
{   
    CountCh=CountMig=0; //Zerowanie liczników dynamizmu
    const rectangle_geometry* MyGeom=dynamic_cast<const rectangle_geometry*>(Agenci.get_geometry());    assert(MyGeom!=nullptr);
    
    if(Synchronic) //Gdy synchronicznie to inna pętla niż przy monte-carlo.
    {//Idziemy po agentach pełnym iterator-em a stan agentów zmieniamy dopiero potem
        
        iterator_h ItGlobal=MyGeom->make_global_iterator(); //Alokujemy iterator po wszystkich agentach
        while(ItGlobal)
        {
            size_t index=MyGeom->get_next(ItGlobal); //Uzyskujemy index  agenta
            
            assert(index!=MyGeom->FULL);				//Tutaj nie powinno się zdarzyć.
            
            kAgent& CenterAgent=*(Agenci.get_ptr(index).get_ptr_val()); // Uzyskujemy referencje do agenta omijając asercje na nullptr
            
            if(Agenci.is_empty(CenterAgent))	// Sprawdzamy, czy nie jest to pusta komórka (nullptr)
                continue;						// bo wtedy robić dalej byłoby bez sensu.
            
            if(CenterAgent.DurCh)
                continue;                       //Ten już był sprawdzany, tylko ze się przeniósł.
            
            
            CheckChange(MyGeom,index,CenterAgent); //Sprawdzamy zmianę stanu
            
        }        
        MyGeom->destroy_iterator(ItGlobal); // upewniamy się ze iterator zostanie usunięty.
        
        
        ItGlobal=MyGeom->make_global_iterator(); //Tworzymy nowy iterator i iterujemy od początku
        while(ItGlobal)
        {
            size_t index=MyGeom->get_next(ItGlobal); //Uzyskujemy index  agenta.
                                                         assert(index!=MyGeom->FULL);	//Tutaj nie powinno się zdarzyć.
            
            kAgent& CenterAgent=*(Agenci.get_ptr(index).get_ptr_val()); // Uzyskujemy referencje do agenta omijając asercje na nullptr
            
            if(Agenci.is_empty(CenterAgent))	// Sprawdzamy, czy nie jest to pusta komórka (nullptr)
                continue;
            
            CenterAgent.update();  //Tu dopiero NADAJEMY nowy stan agentowi
        }	                
        MyGeom->destroy_iterator(ItGlobal); // Upewniamy się, że iterator zostanie usunięty
    }
    else
    {
        iterator_h Monte=MyGeom->make_random_global_iterator(-1);	//Alokujemy iterator Monte-Carlo dla wszystkich (-1)
        while(Monte) //Idziemy po agentach iterator-em Monte-Carlo. Niektórzy mogą się powtórzyć.
        {
            size_t index=MyGeom->get_next(Monte); //Uzyskujemy index losowo wybranego agenta	
                                                         assert(index!=MyGeom->FULL);	//Tutaj nie powinno się zdarzyć
            
            kAgent& CenterAgent=*(Agenci.get_ptr(index).get_ptr_val()); // Uzyskujemy referencje do agenta omijając asercje na nullptr
            if(Agenci.is_empty(CenterAgent))	// Sprawdzamy, czy nie jest to pusta komórka (nullptr)
                continue;						// bo wtedy robić dalej byłoby bez sensu.
            
            CheckChange(MyGeom,index,CenterAgent); //Czy zaszła zmiana stanu?
            
            CenterAgent.update(); //Tu NADAJEMY nowy stan agentowi				
        }
        MyGeom->destroy_iterator(Monte); // upewniamy się, że iterator zostanie usunięty
    }
    
    //Wpisujemy wartości liczników do źródeł eksportujących je dla wizualizacji
    ptrLastChanged->change_ptr(&CountCh); //Alternatywna metoda dla oznaczenia braku/obecności policzonych danych
    ptrLastMigration->change_ptr(&CountMig); //Alternatywna metoda dla oznaczenia braku policzonych danych
}

long kWorld::DoMigration(const rectangle_geometry *MyGeom, //Ta procedura jest napisana nie-ogólnie, tj. w uzależnieniu od prostokątnego typu geometrii
                         size_t index,
                         kAgent& /* CenterAgent */
                        )
{
    size_t SouX,SouY;
    int    TarX,TarY;
    MyGeom->WhatCoordinates(index,SouX,SouY); //Nie ma co sprawdzać, czy dobrze, bo przecież było dobrze :-)
    
    do{
        TarX=RANDOM(MyGeom->get_width());
        TarY=RANDOM(MyGeom->get_height());
    }while(Agenci.filled(TarX,TarY)); //Dopóki nie znajdzie pustego
    
    Agenci.swap(TarX,TarY,SouX,SouY); //Zamienia miejsce
    return MyGeom->get(TarX,TarY); //Nowa pozycja w postaci liniowej 
}

int kWorld::CheckChange(const rectangle_geometry* MyGeom,
                        size_t index,
                        kAgent& CenterAgent
                        ) //KOD NA SZUKANIE ZMIAN
{ 
    //int testowanie = 0;

    // Alokujemy iterator sąsiedztwa o boku 2*Need_For_Closure, zawierający "NeighDens" losowych sąsiadów.
    iterator_h Neigh=MyGeom->make_random_neighbour_iterator(index, dtou(NeedForClosure), NeighDens);
    
    while(Neigh)
    {
         size_t index2=MyGeom->get_next(Neigh); //Uzyskujemy index sąsiada
         if(index2==geometry::FULL || index2==index)	//Gdy poza obszarem symulacji lub w
                  continue;				//centrum obszaru to dalej byłoby bez sensu.
         
         kAgent& SecAgent=*(Agenci.get_ptr(index2).get_ptr_val()); //Uzyskujemy referencje do sąsiada omijając asercje na nullptr
         if(Agenci.is_empty(SecAgent))		//Sprawdzamy, czy nie jest to pusta komórka (nullptr)
                    continue;					   // bo wtedy robić dalej byłoby bez sensu.

         double distance=MyGeom->get_distance(index,index2);                            assert(distance>=1);
         
         double impact_fact= pow(SecAgent.Power, NeedForClosure) * 1 / sqrt(distance);
         double own_impact= pow(CenterAgent.Power, NeedForClosure) * WeightOfSelf;

         double left_impact=0,left_base=0;
         double right_impact=0,right_base=0;

         if(SecAgent.First<=0)
         {
             left_impact= impact_fact * SecAgent.ForLeft;
             left_base= own_impact * CenterAgent.ForLeft;
            CenterAgent.ForLeft=toi(  pow(left_base + left_impact, 1. / (2. * NeedForClosure))  );
         }
         
         if(SecAgent.First>=0)
         {
             right_impact= impact_fact * SecAgent.ForRight;
             right_base= own_impact * CenterAgent.ForRight;
            CenterAgent.ForRight=toi(  pow(right_base + right_impact, 1. / (2. * NeedForClosure))  );
         }         
    }


    //Tu z pewnym prawdopodobieństwem mogłaby zajść zmiana postawy
    if(std::abs((long)CenterAgent.ForRight-CenterAgent.ForLeft) > Threshold)
        {
            if(CenterAgent.ForRight>CenterAgent.ForLeft)
                CenterAgent.new_attitude(1);
            else
                CenterAgent.new_attitude(-1);
            CountCh++;
        }
    else
        {
            CenterAgent.new_attitude(CenterAgent.First); //nic nie trzeba zmieniać, więc nic nie zmieniamy (faktycznie)
        }

    return 0;
}

#pragma clang diagnostic pop
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


