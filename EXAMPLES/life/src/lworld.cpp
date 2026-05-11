/// @file
/// IMPLEMENTATION OF   W O R L D  FOR "Conways Life" SIMULATION.
/// @date 2026-05-11 (modified)
// //////////////////////////////////////////////////////////////

//#include <limits.h>
//#include <assert.h>
//#include <string.h>
//#include <math.h>
#include <cstring>
#include <cmath>

#include "lrand.h"
#include "lworld.h"
#include "histosou.hpp"
#include "clstsour.hpp" //Jest tez statsour (?)
#include "coincsou.hpp"
#include "gadgets.hpp" 
#include "wb_ptrio.h"
#include "wb_swap.hpp"

using namespace symshell2;

const int RAMKA=4;
extern const char* SIMULATION_NAME;

/*
template <class T>
inline void wb_swap(T& a,T& b)
{
    T c=a;
    a=b;
    b=c;
}
*/

// Konstrukcja agentów:
// ////////////////////

lifeagent::lifeagent(const lifeagent& ini)
    {
        if(&ini!=nullptr)
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

// Statyczne pola `lifeagent`-ów dla inicjalizacji:
// ////////////////////////////////////////////////

short	lifeagent::ile_kate=2;      //!< Liczba kategorii w mapach
short	lifeagent::kate_shift=7;

double	lifeagent::MutationLevel=0; //!< Prawd. spontanicznej zmiany poglądów (0..1)
double  lifeagent::InitProp=0.5;    //!< Proporcje inicjowania losowego

// KONSTRUKCJA	ŚWIATA:
// ////////////////////
extern unsigned internal_log;

lifeworld::lifeworld(
       size_t Width,	//Szerokość torusa macierzy agentów
      char* log_name,	//Nazwa pliku do zapisywania historii
      char* mapl_name,	//Nazwa pliku mapy inicjującej "składowe"
      double noise,		//= 0,
      short	ile_kate,	//= 2,		//Liczba kategorii w mapach
      short	odl_sasiad,	//= 1,	//Rozmiar sąsiedztwa
      short	ile_sasiad,	//= 8,	//8 == gęstość sąsiedztwa — jeśli -1 to wszystko po kolei
      bool	synchronicly,	//=true,
      double spontanic	//= 0	//Prawdopodobieństwo spontanicznej zmiany poglądu
        ):
        world(log_name,50),
        MaplName(clone_str(mapl_name)), //Nazwa mapy 1. inicjującej agentów
    //Sub-obiekty właściwe dla tej symulacji:

        MyWidth(Width),
        //Agenci(Width,Width,false,nullptr), //Initer == nullptr wiec tworzony przez konstruktor, a nie klonowanie
        Agenci(Width,Width),       //Zakładamy, że wystarcza to, co robi bezparametrowy konstruktor agenta
        IleKate(ile_kate),                //Liczba kategorii w mapach
        IleSasiad(ile_sasiad),            //8 == gęstość sąsiedztwa
        OdlSasiad(odl_sasiad),            //Rozmiar sąsiedztwa
        Noise(noise),
        Synchronic(synchronicly),
        BierzWszystko(0),                 //Sąsiedztwo bez losowania
        //Wskaźniki do podstawowych seri danych
        Firsts(nullptr),
        Seconds(nullptr)
        {// Niewiele można zrobić, bo nie można tu jeszcze polegać na wirtualnych metodach klasy świat !!!
            assert(ile_kate==2); //Na razie nie może być nic innego
            lifeworld::set_simulation_name(SIMULATION_NAME);

            if(lifeagent::InitProp!=Noise)
            {
                lifeagent::InitProp=Noise; //Inne proporcje inicjowania losowego
                Agenci.Reinitialise(); //Niestety powtórna robota
            }

            lifeagent::MutationLevel=spontanic;
            if(IleSasiad==-1)
                BierzWszystko=1;

        }

// Generujemy podstawowe źródła dla wbudowanego manager-a danych:
// //////////////////////////////////////////////////////////////
void lifeworld::make_basic_sources()
{
    world::make_basic_sources(); //TWORZY SERIĘ "STEP"
    sources_menager& WhatSourMen=this->Sources;

    //Główne serie
    Firsts=Agenci.make_source("State",&lifeagent::First);
    if(Firsts)
        Firsts->set_min_max(0, IleKate - 1);

    Seconds=Agenci.make_source("Prev. state",&lifeagent::Second);
    if(Seconds)
        Seconds->set_min_max(0, IleKate - 1);

    //Umieszczenie głównych serii w managerze serii
    WhatSourMen.insert(Firsts);
    WhatSourMen.insert(Seconds);
}


// Współpraca z managerem wyświetlania, a także logiem:
// ////////////////////////////////////////////////////

void lifeworld::make_default_visualisation()
//Współpraca z zarządcą wyświetlania.
//Rejestruje pochodne serie, tworzy domyślne "lufciki" i wkłada w "Manager"
{
    area_manager_base& Menager=this->MyAreaMenager(); //ustawiane w this->initialise()

    if(!Firsts) {
        cerr<<"`Firsts` obligatory data serie was not found!"<<endl;
        goto ERROR;
    }

    if(!Seconds) {
        cerr<<"`Seconds` obligatory data serie was not found!"<<endl;
        goto ERROR;
    }

    {  //Uzyskanie indeksów podstawowych serii z zarządcy
        const char *FiName = Firsts->name();
        int iFirst = Sources.search(FiName);

        const char *SeName = Seconds->name();
        int iSecond = Sources.search(SeName);

        //Oraz utworzenie pochodnych serii statystycznych
        generic_clustering_source *FirstStat = new generic_clustering_source(Firsts);
        // TODO Współczesny C++ (od standardu C++11 wzwyż) ma w tej kwestii bardzo jasne zasady.
        //      Krótka odpowiedź brzmi: standardowy operator new nie zwraca `nullptr`!!!
        if (!FirstStat) goto ERROR; //TODO!!!
        else Sources.insert(FirstStat);

        generic_clustering_source *SecondStat = new generic_clustering_source(Seconds);
        if (!SecondStat) goto ERROR; //TODO!!!
        else Sources.insert(SecondStat);

        //Źródło liczące statystykę i histogram z klasyfikacji
        generic_histogram_source *ClassStat = new generic_histogram_source(Firsts);
        if (!ClassStat) goto ERROR; //TODO itd...
        else Sources.insert(ClassStat);

        //A także utworzenie seri liczących ich wzajemne ko-statystyki
        coincidention_source *CorrFS = new coincidention_source(Firsts, Seconds);
        if (!CorrFS) goto ERROR;
        Sources.insert(CorrFS); //Żeby została kiedyś zwolniona, a poza tym może ktoś kiedyś...

        fifo_source<double> *EntropyFSLog = new fifo_source<double>(CorrFS->Entropy(), internal_log);
        if (!EntropyFSLog) goto ERROR;
        int iEntropyFS = Sources.insert(EntropyFSLog);

        fifo_source<double> *CorrFSLogR = new fifo_source<double>(CorrFS->Tau_a_Goodman_Kruskal(),
                                                                  internal_log); //Kolejka dla korelacji pierwszych z drugimi
        if (!CorrFSLogR) goto ERROR;
        int iCorrFSR = Sources.insert(CorrFSLogR);


        //I utworzenie seri liczących ich statystyki

        fifo_source<double> *StressFirstLog = new fifo_source<double>(FirstStat->Stress(),
                                                                      internal_log); //Fifo ze stresu
        if (!StressFirstLog) goto ERROR;
        int iSFirst = Sources.insert(StressFirstLog);

        fifo_source<double> *StressSecondLog = new fifo_source<double>(SecondStat->Stress(),
                                                                       internal_log); //Fifo ze stresu
        if (!StressSecondLog) goto ERROR;
        int iSSecond = Sources.insert(StressSecondLog);

        //iMainClassF,iWhichMainF,iNumClassF,
        fifo_source<double> *NumClassLog = new fifo_source<double>(ClassStat->NumOfClass(), internal_log);
        if (!NumClassLog) goto ERROR;
        int iNumClassF = Sources.insert(NumClassLog);

        fifo_source<double> *ClassEntropyLog = new fifo_source<double>(ClassStat->Entropy(), internal_log);
        if (!ClassEntropyLog) goto ERROR;
        int iClassEntropy = Sources.insert(ClassEntropyLog);

        fifo_source<double> *MainClassLog = new fifo_source<double>(ClassStat->MainClass(), internal_log);
        if (!MainClassLog) goto ERROR;
        int iMainClassF = Sources.insert(MainClassLog);

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
        //WYMIARY DOMYŚLNEGO OKNA
        unsigned szer = Menager.get_width();
        unsigned wyso = Menager.get_height();
        assert(szer > 50 && wyso > 40); //Najmniejsze sensowne okno

        //Obszary domyślne — np. obszar STATUSU
        world::make_default_visualisation(); // this->initialize(Manager);
        if (OutArea) {
            OutArea->set(1, 1, szer / 2 - 1, wyso / 2 - 1);
            Menager.as_original(Menager.search(OutArea->name()));
        }

        // WŁAŚCIWE LUFCIKI:
        graph *pom1 = new sequence_graph(szer / 2 - 1, wyso / 4, szer - 50, wyso / 2 - 1,
                                         3, Sources.make_series_info(
                        iClassEntropy, iNumClassF, iMainClassF,
                        -1
                ).get_ptr_val(),
                                         0 // Z reskalowaniem
        );
        if (!pom1)
            goto ERROR;

        pom1->set_frame(128);
        pom1->set_title("HISTORY OF CLASSIFICATION");
        Menager.insert(pom1);

        //inne mniej potrzebne
        graph *pom = new sequence_graph(szer / 2 - 1, 1, szer - 50, wyso / 4 - 1,  //domyślne współrzędne
                                        1, Sources.make_series_info(
                        iSFirst,
                        -1
                ).get_ptr_val(),
                //0// Z reskalowaniem
                                        1); //Wspólne minimum/maximum
        if (!pom) goto ERROR;
        pom->set_frame(128);
        pom->set_title("HISTORY OF STRESS");
        Menager.insert(pom);

        pom = new carpet_graph(1, wyso / 2, szer / 3, wyso - 1,//domyślne współrzędne
                               Firsts); //I  //TODO!!! danych
        pom->set_data_colors(0, 255);
        pom->set_title("Map of current state");
        Menager.insert(pom);

        pom = new bars_graph(szer / 3 + 1, wyso / 2, szer / 3 * 2,
                             wyso - 1,//domyślne współrzędne  szer-49,7*char_height('X')+7,szer,8*char_height('X')+9
                             ClassStat);
        pom->set_data_colors(0, 255);
        pom->set_title("Histogram of state");
        Menager.insert(pom);

        pom = new manhattan_graph(szer / 3 * 2 + 1, wyso / 2, szer, wyso - 1,//domyślne współrzędne
                                  CorrFS, 0,    //I źródło danych
                                  CorrFS, 0,
                                  1,
                                  0.22,        //Ułamek szerokości przeznaczony na perspektywę
                                  0.77);    //Ułamek wysokości  przeznaczony na perspektywę
        pom->set_data_colors(0, 255);
        pom->set_text_colors(0);
        pom->set_title("Determination of curr. state by prev. state");
        Menager.insert(pom);

        //PRZYCISKI
        pom = new carpet_graph(szer - 49, 5 * (char_height('X') + RAMKA), szer,
                               6 * (char_height('X') + RAMKA),//domyślne współrzędne
                               Seconds); //I źródło danych
        pom->set_data_colors(0, 255);
        pom->set_frame(0);
        pom->set_title("Map of previous state");
        Menager.insert(pom);


        pom1 = new sequence_graph(szer - 49, 9 * (char_height('X') + RAMKA), szer, 10 * (char_height('X') + RAMKA),

                                  1, Sources.make_series_info(
                        iEntropyFS,
                        -1
                ).get_ptr_val(),
                                  1 /*Wspólne minimum/maximum*/);
        if (!pom1) goto ERROR;
        pom1->set_frame(128);
        pom1->set_title("HISTORY OF ENTROPY OF DETERMINATION");
        Menager.insert(pom1);


        pom = new sequence_graph(szer - 49, 11 * (char_height('X') + RAMKA), szer, 12 * (char_height('X') + RAMKA),
                                 1, Sources.make_series_info(
                        iCorrFSR,//iCorrFS,
                        -1
                ).get_ptr_val(),
                                 1
        );
        if (!pom) goto ERROR;
        pom->set_frame(128);
        pom->set_title("HISTORY OF Prev. TO Curr. CORRELATION");
        Menager.insert(pom);

        //Tworzenie obszaru sterującego:
        {
            wb_dynarray < rectangle_source_base * > tmp(2, (rectangle_source_base *) Sources.get(iFirst),
                                                        (rectangle_source_base *) Sources.get(iSecond),
                                                        -1
            );
            drawable_base *pom = new steering_wheel(szer - 49, 0, szer, 5 * (char_height('X') + RAMKA), tmp);
            assert(pom != nullptr);
            pom->set_background(10);
            Menager.insert(pom);
        }

        Sources.new_data_version(1, 1); //Oznajmia seriom, że dane się uaktualniły	(po inicjacji)
    }
    ERROR: //Tu akcja na niepogodę
    std::cerr<<"Incorrect initialization of the default visualizations!"<<std::endl;
        ;  //`error_message(...)`???
}



// AKCJE SYMULACYJNE:
// //////////////////

void lifeworld::after_read_from_image()
//Actions after read state from a file. Aktualizacja pól statycznych `lifeagent`-a!!!
{                   assert(IleKate==2); //Dla life tylko dwie możliwe kategorie.
    lifeagent::ile_kate=IleKate; //Liczba kategorii w mapach
    lifeagent::kate_shift=7;
    /*
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
     */
}

// stan startowy symulacji
void lifeworld::initialize_layers()
//-------------------------------------
{
    static int first=1; //TYMCZASOWE WYŁĄCZENIE NADMIARU WYDRUKÓW!!!
    if(first)
        Log.GetStream()<<"attitude SIMULATION:";
    //odl_sasiad = 1,//Rozmiar sąsiedztwa
    //ile_sasiad = 8 //8 == gęstość sąsiedztwa

    lifeagent::ile_kate=IleKate; //Liczba kategorii w mapach

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

    //...wydruk wartości parametrów symulacji
    if(first)
      Log.GetStream()
        <<"\nNum of Kl="<<Log.separator()<<IleKate
        <<"\nNoise %="<<Log.separator()<<Noise*100
        <<"\nNeighborhood="<<Log.separator()<<IleSasiad<<"/("<<(1+2*OdlSasiad)<<"*"<<(1+2*OdlSasiad)<<")\n";

    //			USTALANIE STANÓW AGENTÓW
    //Wczytuje, używając konstruktora lub klonowania, gdy nie ma, wiec inicjuje resztę pól.
    rectangle_layer_of_agents<lifeagent>::assign_rgb_fun tmp=&lifeagent::assign123;
    char* fname=MaplName.get_ptr_val();
    //Jeśli nie zainicjowane z bitmapy to zostaje to z konstruktorów!
    if( fname != nullptr && strlen(fname)>0 )
    {
        int from = Agenci.init_from_bitmap(MaplName.get_ptr_val(), tmp);

        if (from != 1) {
            cerr << "Agents initialization from the bitmap " << MaplName << " failed!" <<endl;
            //Agenci.clean(); // reallocate_all();
            exit(-10);
        }
    }
    else
    {
        cerr << "Agents default initialization because of empty bitmap filename." <<endl;
    }

    first=0; //Koniec pierwszego wywołania //TYMCZASOWO!!! Ha Ha !!!
}

//Pojedynczy krok symulacji
void lifeworld::simulate_one_step()
//---------------------------------------
{
    const geometry_base* MyGeom=Agenci.get_geometry();
    assert(MyGeom);

    if(Synchronic)
    {
        //Idziemy po agentach pełnym iteratorem, a stan agentów zmieniamy dopiero potem.
        iteratorh IGlob=MyGeom->make_global_iterator();
        while(IGlob)
        {   //Uzyskujemy index  agenta
            size_t index=MyGeom->get_next(IGlob);    assert(index!=MyGeom->FULL); //Tutaj nie powinno się zdarzyć

            lifeagent& CenterAgent=Agenci.get(index); // Uzyskujemy referencje do agenta

            CheckChange(MyGeom,index,CenterAgent); //Sprawdzamy zmianę stanu
        }
        // upewniamy się ze iterator zostanie usunięty
        MyGeom->destroy_iterator(IGlob);


        IGlob=MyGeom->make_global_iterator(); //Tworzymy nowy iterator i iterujemy od początku
        while(IGlob)
        {
            size_t index=MyGeom->get_next(IGlob); //Uzyskujemy index  agenta

            assert(index!=MyGeom->FULL);				//... tutaj nie powinno się zdarzyć

            lifeagent& CenterAgent=Agenci.get(index); // Uzyskujemy referencje do agenta

            wb_swap(CenterAgent.First,CenterAgent.Second);  //Ma nowy stan
        }

        // upewniamy się ze iterator zostanie usunięty
        MyGeom->destroy_iterator(IGlob);

    }
    else
    {
        iteratorh Monte=MyGeom->make_random_global_iterator();	//Alokujemy iterator Monte-Carlo

        while(Monte) //Idziemy po agentach iteratorem Monte-Carlo. Niektórzy mogą się powtórzyć
        {
            //Uzyskujemy index losowo wybranego agenta
            size_t index=MyGeom->get_next(Monte);    assert(index!=MyGeom->FULL); //... tutaj nie powinno się zdarzyć

            lifeagent& CenterAgent=Agenci.get(index); // Uzyskujemy referencje do agenta

            if(CheckChange(MyGeom,index,CenterAgent)==1) //Czy zaszła zmiana stanu
                {
                    wb_swap(CenterAgent.First,CenterAgent.Second);
                }
        }

        // upewniamy się, że iterator zostanie usunięty
        MyGeom->destroy_iterator(Monte);
    }

}


int lifeworld::CheckChange(const geometry_base* MyGeom,
                        size_t index,
                        lifeagent& CenterAgent
                        )//KOD NA SZUKANIE ZMIAN
{ 
    int testowanie=0;

    if(DRAND()<=lifeagent::MutationLevel) //Rzadka, spontaniczna zmiana stanu
    {
        int atti=RANDOM(IleKate);
        assert(0<=atti && atti<IleKate);

        CenterAgent.Second=atti; //zmieniamy w agencie centralnym
        return 1;
    }

    // Alokujemy iterator sąsiedztwa
    ::iteratorh Neigh=nullptr;

    if(BierzWszystko)
    {
        Neigh=MyGeom->make_neighbour_iterator(index,OdlSasiad);
    }
    else
    {
        Neigh=MyGeom->make_random_neighbour_iterator(index,OdlSasiad,IleSasiad);
    }

    int zliczanie=0; //Zliczanie sąsiadów
    double alive=0;  //LICZNIK ŻYWYCH

    while(Neigh)
    {
        size_t index2=MyGeom->get_next(Neigh); //Uzyskujemy index sąsiada
        if(index2==MyGeom->FULL || index2==index) //Jeśli poza obszarem symulacji lub w
            continue; //centrum obszaru to dalej byłoby bez sensu.

        lifeagent& PeryfAgent=Agenci.get(index2); //Uzyskujemy referencje do sąsiada

        zliczanie++;
        alive+=PeryfAgent.First; // `double(lifeagent::ile_kate);`
    }

    MyGeom->destroy_iterator(Neigh); // upewniamy się, że iterator zostanie usunięty

    if(alive<=1 || alive>=4)
    {
        CenterAgent.Second=0; //zmieniamy w agencie centralnym
        return 1;
    }
    else
    if(alive==3 ) //&& CenterAgent.First!=0)
    {
        CenterAgent.Second=1; // + `CenterAgent.First;` //zmieniamy w agencie centralnym
        return 1;
    }
    else //Nic się nie zmienia
    {
        CenterAgent.Second=CenterAgent.First; //Albo nic nie zmieniamy
        return 0;
    }
}

// /////////////////////////////////////////////
// Example for SYMSHELL CLASSES library.
/// @author Wojciech Borkowski, iss.uw.edu.pl
// https://github.com/borkowsk/symShell2andRTM
/// @copyright 2000 - 2026
// /////////////////////////////////////////////
