/// @file
/// @brief
///  @EN{  }
///  @PL{ SYMULACJA KONFLIKTÓW BOCA 2005 (plik główny) }
/// @date 2026-05-19 (modified)
///       =========================================================
/// @details UZUPEŁNIONY 10-11.2005, 9-2010, 02-2014, 04-2026

//-=====================================================================================================================
//#include "platform.hpp" //????

#include "optParam.hpp"
#include "wb_ptr.hpp"
#include "wb_rand.hpp"

#include "simpsour.hpp"
#include "funcsour.hpp"
#include "statsour.hpp"
#include "fifosour.hpp"
#include "sourmngr.hpp"

//#include "gadgets.hpp"
#include "graphs.hpp"
#include "areamngr.hpp"
#include "mainmngr.hpp"
#include "world.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nullptr"

using namespace sym2;

/// ## HISTORIA
/// * 1.1 Dodano obsługę wczytywania sieci z pliku
/// * 1.2 Dodano obsługę parametrów wywołania
///       i symetryczne traktowanie połączeń wczytanych z pliku
/// * 1.21 Poprawiono obsługę parametrów — błąd dla stringów
///       Dodano parametry kontroli wydruku oraz Min-Max dla skali stanów
/// * 1.22 "Updajt" do poprawionej wersji bibliotek SYMSHELL i WBRTM.
/// * 1.23 Ponowny "updejt" do stanu bibliotek na 2026.04
const char* SIMULATION_NAME= "CONFLICTS 1.22b BOCA/WARSZAWA: " __DATE__;
const char* SCREEN_DUMP_NAME="CONFLICTS_v1.22_";

//#define POLISH_VERSION
unsigned    lang_selector=1;

const char* LogName="conf.log";  //Nazwa tego logu
const char* MetaExpFileName="metaconf.out"; //Nazwa logu meta-eksperymentu
const char* DefaultNetFileName="conflicts1.net"; //Nazwa pliku z definicją sieci

//ROZMIARY OKNA
unsigned SCR_WIDTH=750;	//1200;
unsigned SCR_HEIGHT=552;	//1024;

float  DIAM_SIZE=0.01f;
unsigned  ARROWS_DIV=7;	//Dzielnik skracający strzałki. Jak 0, to mają stała długość
double FixMinState=0;
double FixMaxState=0;

//Ogólne parametry modelowania:
int MultiExp=1; //Ile zrobić powtórzeń?
unsigned iMaxIterations=10000;    //Ile najwięcej kroków
unsigned iMaxIterRep=1000;
unsigned iViewRatio=1;           //Jak często wizualizować
unsigned iLogRatio=1;            //Jak często zapisywać do logu

//Parametry właściwego modelu:
int     def_mode=1; //0,1,2,3,4 —> 2 i 4 oznacza wczytanie sieci z pliku
unsigned def_num_of_nodes=25; //Ile jest węzłów, gdy sieć nie jest z pliku

//Stany początkowe i zmiany:
double def_m_init_st=0.005;  //Średni stan początkowy
double def_r_init_st=0.000001;   //Odchylenie od średniego stanu początkowego
double def_m_of_noise=-0.1;  //Średni poziom szumu/sygnału dodawanego do węzłów
double def_r_of_noise=0.1;	//Odchylenie od śred. szumu/sygnału dodawanego do węzłów

//Parametry zakresu zmian dla multi-eksperymentów:
double defm_start_mean_state=-1.1;
double defm_end_mean_state=1.1;
double defm_start_noise_mean=-0.5;
double defm_end_noise_mean=0.5;

//Te na razie zafiksowane:
double def_m_of_weight=0.1; //Średnia waga krawędzi
double def_r_of_weight=0.0001; //Odchylenie od śred. wagi krawędzi
double def_asymmetry=1;   //Asymetria przekazywania komunikatów dodatnich względem ujemnych

//Nazwy niektórych obszarów:
const char* HISTofSTATES=lang("HISTORIA STANÓW","HISTORY OF THE STATE");
const char* HISTofACCT=lang("HISTORIA AKTYWNOŚCI","HISTORY OF THE ACTIVITY");
const char* META_GRAPH=lang("SYGNAŁ x ŚREDNI STAN","INPUT vs. MEAN STATE");

#include <cassert>
//#include <ctype.h>
#include <iostream>
using namespace std;
#include <cmath>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

OptionalParameterBase* Parameters[]={ //sizeof(Parameters)/sizeof(Parameters[])
new OptionalParameter<const char*>(LogName,"*.log","e.g conflict1.log","LOGNAME","Single experiment outputfile name"),
new OptionalParameter<const char*>(MetaExpFileName,"*.out","e.g mult1.out","MOUTNAME","Multiple experiment outputfile name"),
new OptionalParameter<const char*>(DefaultNetFileName,"*.net","e.g mynet.net","NETNAME","Network definition input file name"),
new OptionalParameter<int>(def_mode,0,3,"IMODE","Network initialization mode: 0,1,3-random, 2-from a definition file"),

new OptionalParameter<unsigned>(iMaxIterations,10,1000000,"MAXITER","Limit for number of simulation steps"),
new OptionalParameter<unsigned>(def_num_of_nodes,2,10000,"NODEN","How many nodes in random networks"),
new OptionalParameter<double>(def_m_of_weight,-1,1,"MEWEIGHT","Mean edge weight"),
new OptionalParameter<double>(def_r_of_weight,0,1,"REWEIGHT","Difference from an average edge weight"),
new OptionalParameter<double>(def_m_init_st,-1,1,"MISTATE","Mean initial state"),
new OptionalParameter<double>(def_r_init_st,0,1,"RISTATE","Difference from mean init state"),
new OptionalParameter<double>(def_m_of_noise,-1,1,"MNOISE","Mean noise/signal for nodes"),
new OptionalParameter<double>(def_r_of_noise,0,1,"RNOISE","Difference from mean noise/signal"),
new OptionalParameter<double>(def_asymmetry,0,100,"ASSYM","Asymetry in positive/negative communication"),

new OptionalParameter<int>(MultiExp,2,1000,"MULTI","Multiplication factor for multiple experiments"),
new OptionalParameter<double>(defm_start_mean_state,-2,2,"SMISTATE","Start value for mean state"),
new OptionalParameter<double>(defm_end_mean_state,-2,2,"EMISTATE","End value for mean state"),
new OptionalParameter<double>(defm_start_noise_mean,-2,2,"SMNOISE","Start value for mean noise/signal"),
new OptionalParameter<double>(defm_end_noise_mean,-2,2,"EMNOISE","End value for mean noise/signal"),

new OptionalParameter<unsigned>(iMaxIterRep,10,1000000,"MMAXSTEP","Limit for number of steps during multi experiments"),

new OptionalParameter<unsigned>(iViewRatio,1,  1000000,"VIEWR","simulation step to visualisation ratio"),
new OptionalParameter<unsigned>(iLogRatio,1,   1000000,"LOGR","simulation step to log a writing ratio"),
new OptionalParameter<unsigned>(SCR_WIDTH, 100, 10000, "WINW", "Initial width of a program main window"),
new OptionalParameter<unsigned>(SCR_HEIGHT, 100, 10000, "WINH", "Initial height of a program main window"),
new OptionalParameter<double>(FixMinState,-100,100,"FIXMINS","Min for fixed range of state in graphs"),
new OptionalParameter<double>(FixMaxState,-100,100,"FIXMAXS","Max for fixed range of state in graphs"),
new OptionalParameter<float>(DIAM_SIZE,0.0000000001,1,"DIAMS","Diamond size in the meta exp. visualization"),
new OptionalParameter<unsigned>(ARROWS_DIV,1,100,"ARRDIV","Divider for arrows in the meta exp. visualization")
};


double niby_gauss_max(double mean,double max,unsigned steps=6)	//12
{
    double ret,A=0;             assert(mean<max);    
    double range=(max-mean);
    double min=mean-range;
    
    for(int i=steps;i>0;i--)
        A+=TheRandG.DRand(); 

    ret=A;                      assert(0.0<=ret && ret<=steps);
    ret/=steps;                 assert(0.0<=ret && ret<=1.0);
    ret*=range*2;               assert(0.0<=ret && ret<=2*range);
    ret+=min;                   assert(min<=ret && ret<=max);                  
    return ret;
}

// Świat - GŁÓWNA KLASA SYMULACYJNA i JEJ PODJEDNOSTKI Agent & Connection.
//==========================================================================
class LocalWorld: public world
//============================
{
public:
    class Agent
    {
        double x,y,r;   //Współrzędne wizualizacyjne agenta
        double state;   //Stan agenta -1..1 (?)
        double p_state; //Poprzedni stan agenta
        double delta;   //Ostatni wpływ od innych
        wb_pchar Name;  //Nazwa agenta — dla sieci wczytywanych
    public:
        Agent(): x(0), y(0), r(1), state(0), p_state(0)
            {}
        void _setstate(double i_state);        //Bezpośrednie nadawanie stanu — bez funkcji mapujących
        void setpos(double x,double y,double r=-1);	//Ustawianie pozycji i promienia. r = -1 —> pozostawia stary promień
        void prepare_to_step();
        void add_to_delta(double input);
        void add_the_delta();
        void add_directly(double noise);
    friend class LocalWorld; //Musi mieć bezpośredni dostęp do pól, żeby przypiąć źródła danych
    };

    class Connection
    {
        size_t start_node;   //Indeks węzła startowego
        size_t end_node;     //Indeks węzła końcowego
        double weight;       //Waga połączenia
        double last_act;      //Ostatnio rejestrowana aktywność połączenia
    public:
        Connection(): start_node(-1), end_node(0), weight(0), last_act(0)
            {}
        void set(size_t s,size_t e,double w=0);   
        void clean_act(){ last_act=0;}
    friend class LocalWorld; //Musi miec dostęp do pól, żeby przypisać źródła danych
    };

private:
    wb_dynarray<Agent> agents;  //Lista agentów
    wb_dynarray<Connection> connections; //Lista połączeń
    unsigned mode;	//0 - bez połączeń, 1 - symetrycznie 2-niesymetryczne połączenia
    unsigned logs_length; //długość buforów na statystyki symulacji
    unsigned how_many_agents;	//Zadana liczba agentów

    double m_init_st;   //Średni stan początkowy
    double r_init_st;   //Odchylenie od średniego stanu początkowego
    double m_of_noise;  //Średni poziom szumu/sygnału dodawanego do węzłów
    double r_of_noise;  //Odchylenie od śred. szumu/sygnału dodawanego do węzłów
    double m_of_weight; //Średnia waga krawędzi
    double r_of_weight; //Odchylenie od śred. wagi krawędzi
    double asymmetry;   //Asymetria przekazywania komunikatów dodatnich względem ujemnych
    
    struct_array_source<Agent,double>* pNodeX;      //Współrzędne węzłów w aranżacji
    struct_array_source<Agent,double>* pNodeY;      //------------//----------------
    struct_array_source<Agent,double>* pNodeR;      //Rozmiar węzła w aranżacji
    struct_array_source<Agent,double>* pNodeState;  //Aktywność węzłów
    struct_array_source<Agent,double>* pNodePreState;	//Poprzednia aktywność węzłów
    struct_array_source<Agent,double>* pNodeDelta;  //Ostatni wpływ od innych

    struct_array_source<Connection,size_t>* pConnStart;	//Indeksy początków linii łączących węzły sieci
    struct_array_source<Connection,size_t>* pConnEnd;	//Indeksy końców linii łączących węzły sieci
    struct_array_source<Connection,double>* pConnWeight;	//Waga połączenia
    struct_array_source<Connection,double>* pConnAcct;	//Aktywność połączenia
    
    generic_basic_statistics_source*	StateStat;
    fifo_source<double>*    MinStateLog;
    fifo_source<double>*    MeanStateLog;
    fifo_source<double>*    MaxStateLog;

    generic_basic_statistics_source*	AcctStat;
    fifo_source<double>*    MinAcctLog;
    fifo_source<double>*    MeanAcctLog;
    fifo_source<double>*    MaxAcctLog;

    //Funkcje pomocnicze do modelu
    void _MakeCircle(double cx=0,double cy=0,double r=1,unsigned start=0,unsigned end=-1);  //Układa agentów w kółko
    void _MakeStates(double mean,double max,unsigned start=0,unsigned end=-1); //Ustala agentom stany z rozkładu
    void _AddNoise(); //Dodaje "sobie" szum

    //Pomocnicze do statystyk itp
    void AllocSources(); //Tworzy źródła danych
public:
    virtual 
        ~LocalWorld() {}
    
    LocalWorld(unsigned  WhatMode,
               unsigned HowManyAgents,

               double im_init_st,  //Średni stan początkowy
        double ir_init_st,   //Odchylenie od średniego stanu początkowego
        double im_of_noise,  //Średni poziom szumu/sygnału dodawanego do węzłów
        double ir_of_noise,  //Odchylenie od śred. szumu/sygnału dodawanego do węzłów
        double im_of_weight=0.5, //Średnia waga krawędzi
        double ir_of_weight=0.1, //Odchylenie od śred. wagi krawędzi
        double i_asymmetry=1   //Asymetria przekazywania komunikatów dodatnich względem ujemnych
        ):
            logs_length(100000),
            mode(WhatMode),
            how_many_agents(HowManyAgents),
            m_init_st(im_init_st),   //Średni stan początkowy
        r_init_st(ir_init_st),   //Odchylenie od średniego stanu początkowego
        m_of_noise(im_of_noise),  //Średni poziom szumu/sygnału dodawanego do węzłów
        r_of_noise(ir_of_noise),  //Odchylenie od śred. szumu/sygnału dodawanego do węzłów
        m_of_weight(im_of_weight), //Średnia waga krawędzi
        r_of_weight(ir_of_weight), //Odchylenie od śred. wagi krawędzi
        asymmetry(i_asymmetry),      //Asymetria przekazywania komunikatów dodatnich względem ujemnych
        world(LogName,50)   
    {
        pNodeX=NULL;pNodeY=NULL;pNodeR=NULL;pNodeState=NULL;pNodePreState=NULL;pNodeDelta=NULL;
        pConnStart=NULL;pConnEnd=NULL;pConnWeight=NULL;pConnAcct=NULL;
        StateStat=NULL;MinStateLog=NULL;MeanStateLog=NULL;MaxStateLog=NULL;
        AcctStat=NULL;MinAcctLog=NULL;MeanAcctLog=NULL;MaxAcctLog=NULL;
    }
    
    //Do ustawiania nowych parametrów modelu, przed reset?reinitialise
    void SetModelParameters(
        double im_init_st,  //Średni stan początkowy
        double ir_init_st,   //Odchylenie od średniego stanu początkowego
        double im_of_noise,  //Średni poziom szumu/sygnału dodawanego do węzłów
        double ir_of_noise,  //Odchylenie od śred. szumu/sygnału dodawanego do węzłów
        double im_of_weight=0.5, //Średnia waga krawędzi
        double ir_of_weight=0.1, //Odchylenie od śred. wagi krawędzi
        double i_asymmetry=1   //Asymetria przekazywania komunikatów dodatnich względem ujemnych
        )
    {
        m_init_st=im_init_st;   //Średni stan początkowy
        r_init_st=ir_init_st;   //Odchylenie od średniego stanu początkowego
        m_of_noise=im_of_noise;  //Średni poziom szumu/sygnału dodawanego do węzłów
        r_of_noise=ir_of_noise;  //Odchylenie od śred. szumu/sygnału dodawanego do węzłów
        m_of_weight=im_of_weight; //Średnia waga krawędzi
        r_of_weight=ir_of_weight; //Odchylenie od śred. wagi krawędzi
        asymmetry=i_asymmetry;      //Asymetria przekazywania komunikatów dodatnich względem ujemnych
    }
    
    
    //Trzeba wybrać którąś z metod inicjalizacji sieci
    //int     /*def_*/mode = 1; //0-bez połączeń, 1-symetryczne połączenia 2-asymetryczne połączenia
    void InitialiseNotConnected(unsigned HowManyAgents);
    void InitialiseFullyConnected(unsigned HowManyAgents);
    void InitialiseRandomConnected(unsigned HowManyAgents);
    void InitialiseFromWiesiekFile(const char* FileName);

    //Dodatkowe akcesory do źródeł statystycznych
    /*const*/ fifo_source<double>&    MeanStates() //Log średnich stanów. TODO Bez const. bo za dużo do przerabiania
        {return *MeanStateLog;};
    //... i inne kiedyś
    //...
    
    //PROVIDED FOR COMPATIBILITY WITH BASE CLASS "world"
    // "user defined" actions for initialization.
    virtual void initialize_layers();
    //Właściwa implementacja kroku symulacji — do zaimplementowania
    virtual void simulate_one_step();
    // user defined actions after read simulation state from file
    virtual void after_read_from_image();
    //Generuje podstawowe źródła dla wbudowanego zarządcy danych
    virtual void make_basic_sources();
    //Współpraca z zarządcą wyświetlania
    virtual void make_default_visualisation(); //Tworzy domyślne "lufciki" i umieszcza w potomstwie `area_manager_base&`
    //Aktualizacja zawartości okna statusu po `n` krokach symulacji
    virtual void actualize_out_area();
    virtual int  implement_output(ostream& o) const { o<<__PRETTY_FUNCTION__<<":Unexpected usage of serialization!"<<endl; return 0; }
    virtual int  implement_input(istream& i)  { cerr<<__PRETTY_FUNCTION__<<"Unexpected usage of deserialization!"<<endl; return 0; }
};

inline void LocalWorld::Agent::prepare_to_step()
{
    this->p_state=this->state;
    this->delta=0;
}

inline void LocalWorld::Agent::add_to_delta(double input)
{
    delta+=input;
}

inline void LocalWorld::Agent::add_the_delta()
{   
    state=tanh(state+delta);	//???
}

inline void LocalWorld::Agent::add_directly(double noise)
{
    state=tanh(state+noise);
}

void LocalWorld::simulate_one_step()
{
    for(unsigned i=0;i<agents.get_size();i++)
        agents[i].prepare_to_step();
    
    if(mode==1 || mode==2)	//Fully connected or from file undirected
    {
        for(unsigned j=0;j<connections.get_size();j++)
        {
            double p1,p2,w=connections[j].weight;
            unsigned s=connections[j].start_node;
            unsigned e=connections[j].end_node;
            double St=agents[e].state;
            agents[s].add_to_delta(p1=(w*St*(St<0?asymmetry:1)));	//Konflikt to wartości ujemne — umownie oczywiście
            St=agents[s].state;
            agents[e].add_to_delta(p2=(w*St*(St<0?asymmetry:1)));
            connections[j].last_act=std::max(p1, p2);
        }
    }

    if(mode==3 || mode==4)	//Random directed or from file directed
    {
        for(unsigned j=0;j<connections.get_size();j++)
        {
            double p,w=connections[j].weight;
            unsigned s=connections[j].start_node;
            unsigned e=connections[j].end_node;
            double St=agents[s].state;
            agents[e].add_to_delta(p=(w*St*(St<0?asymmetry:1)));	//Konflikt to wartości ujemne — umownie oczywiście
            connections[j].last_act=p;
        }
    }
    
    if(mode>0)
    {
        for(unsigned i=0;i<agents.get_size();i++)   
            agents[i].add_the_delta();
    }
    
    _AddNoise();
}

void LocalWorld::_AddNoise()
{
    for(unsigned i=0;i<agents.get_size();i++)
    {
     double n=niby_gauss_max(m_of_noise,m_of_noise+r_of_noise);
     agents[i].add_directly(n);
    }
}

void LocalWorld::_MakeStates(double mean, double max, unsigned start, unsigned end/*=-1*/) //Ustala agentom stany z rozkładu
{
    if(end==unsigned(-1) )	//SPECJALNE ZNACZENIE -1 "nie znam rozmiaru, weź i sprawdź"
        end=agents.get_size();
 
    for(unsigned i=start;i<end;i++)
    {
        agents[i]._setstate(0);	//Bo mogą być stare wartości
        agents[i].add_directly(niby_gauss_max(mean,mean+max));
    }
}

void LocalWorld::_MakeCircle(double cx, double cy, double r, unsigned start, unsigned end/*=-1*/)
{
    if(end==unsigned(-1)) //SPECJALNE ZNACZENIE -1 "nie znam rozmiaru, weź i sprawdź"
        end=agents.get_size();
    int ile=end-start;
    double krok=(2*M_PI)/ile;
    double ar=1;	//domyślny promień agenta - trzeba `set_min_max()` dla seri "R" !!!
    
    for(double angle=0;angle<(2*M_PI) && start<end;angle+=krok)
    {
        double x=cx+sin(angle)*r;
        double y=cy+cos(angle)*r;
        agents[start].setpos(x,y,ar);
        start++;
    }

    assert(start<=end);
}

void LocalWorld::InitialiseNotConnected(unsigned HowManyAgents)
{
    if(agents.get_size()!=HowManyAgents)	//Jednakże, jak nie równe i reinicjalizacja to kicha!!!
        agents.alloc(HowManyAgents);
    //connections.alloc(1);
    //connections[0].set(0,0,0);	//Dummy connection
    _MakeCircle();	//Układa wszystkich w domyślne koło
    _MakeStates(m_init_st,r_init_st); //Ustala agentom stany z rozkładu
}

void LocalWorld::InitialiseFullyConnected(unsigned HowManyAgents)
{
    if(agents.get_size()!=HowManyAgents)	//Wszakże jak nie równe i reinicjalizacja to kicha!!!
        agents.alloc(HowManyAgents);    
    
    _MakeCircle();	//Układa wszystkich w domyślne koło
    _MakeStates(m_init_st,r_init_st); //Ustala agentom stany z rozkładu

    unsigned how_many_con=((HowManyAgents*HowManyAgents)-HowManyAgents)/2; //( n^2-n )/ 2
    if(how_many_con!=connections.get_size()) //Wszakże jak nie równe i reinicjalizacja to kicha!!!
        connections.alloc(how_many_con);

    unsigned count_conn=0; //zliczanie/przesuwanie
    for(unsigned i=1;i<HowManyAgents;i++)
    {
        for(unsigned j=0;j<i;j++)
        {
            double Weight=niby_gauss_max(m_of_weight,m_of_weight+r_of_weight);
            connections[count_conn++].set(i, j, Weight);
        }
    }                                                  assert(count_conn == how_many_con);
}

void LocalWorld::InitialiseRandomConnected(unsigned HowManyAgents)
{
    if(agents.get_size()!=HowManyAgents)	//Wszakże jak nie równe i reinicjalizacja to kicha!!!
        agents.alloc(HowManyAgents);    

    _MakeCircle();	//Układa wszystkich w domyślne koło
    _MakeStates(m_init_st,r_init_st); //Ustala agentom stany z rozkładu
    
    unsigned how_many_con=(HowManyAgents*HowManyAgents)-HowManyAgents; //n^2-n
    if(how_many_con!=connections.get_size()) //Wszakże jak nie równe i reinicjalizacja to kicha!!!
            connections.alloc(how_many_con);

    unsigned count_conn=0;	//ZLICZANIE
    for(unsigned i=0;i<HowManyAgents;i++)
    {
        for(unsigned j=0;j<HowManyAgents;j++)
        if(j!=i)
        {
            double Weight=niby_gauss_max(m_of_weight,m_of_weight+r_of_weight);
            if(fabs(Weight)>0.000001)
                connections[count_conn++].set(i, j, Weight);
        }
    }                                                  assert(count_conn == how_many_con);
}


void LocalWorld::InitialiseFromWiesiekFile(const char* FileName)
{
    ifstream Input(FileName/*,ios::in | ios::nocreate*/); //W przypadku ifstream flaga nocreate jest... zbędna.
    // Domyślne zachowanie ifstream (strumienia wejściowego) jest dokładnie takie, jakiego oczekujesz od nocreate:
    // jeśli plik nie istnieje, strumień nie utworzy go, lecz ustawi flagę błędu (failbit).
    // A flaga "in" wynika z tego, że to strumień wejściowy :-D
    int HowManyAgents=INT_MAX;

    Input>>HowManyAgents;
                                                        assert(HowManyAgents>=0);

    if(HowManyAgents>0 && HowManyAgents<1000) //DLACZEGO 1000 ??? TODO
        if(agents.get_size()==unsigned(HowManyAgents)) //Rzut bo warning
        { //Już było czytane
            _MakeStates(m_init_st,r_init_st); //Ustala agentom stany z rozkładu
            for(unsigned c=0;c<connections.get_size();c++)
                connections[c].clean_act();
            cerr<<"Network already read before. Only states were cleaned."<<endl;
        }
        else
        {
            agents.alloc(HowManyAgents); 
            Input.ignore(INT_MAX,'\n');
            for(int n=0;n<HowManyAgents;n++)
            {
                double X=-DBL_MAX;
                double Y=-DBL_MAX;
                double S=0;
                double R=1; //Można by uzależnić od tekstu albo od wartości za tekstem (na razie ignorowanej)
                wb_pchar TmpStr;
                Input>>X>>Y>>S>>TmpStr;
                cerr<<X<<' '<<Y<<' '<<S<<' '<<TmpStr<<endl;
                Input.ignore(INT_MAX,'\n');
                agents[n].setpos(X,Y,R);
                agents[n]._setstate(S);	//Bo mogą być stare wartości
                if(X==-DBL_MAX || Y==-DBL_MAX || Input.fail())
                {
                    cerr<<"AT NODE #"<<n<<" AN ERROR OCCURED DURING DATA FILE READING"<<endl;
                    return;
                }

                if(1)	//Tylko wtedy gdy stany nie są dane w pliku. Na razie nie ma takiej możliwości
                    _MakeStates(m_init_st,r_init_st); //Ustala agentom stany z rozkładu
            }

            Input >> ws;	//old Input.eatwhite();

            unsigned countcon=0;
            connections.alloc(HowManyAgents*HowManyAgents);	//Początkowo z zapasem
            while(!(Input.fail()) && !(Input.eof()))
            {
                int S=-INT_MAX;
                int E=-INT_MAX;
                double W=-DBL_MAX;        
                Input>>S>>E>>W;
                cerr<<S<<"-->"<<E<<' '<<W<<endl;
                Input.ignore(INT_MAX,'\n');
                Input>> ws ;	//.eatwhite();	//Jest nadzieja :) , że będzie EOF, a nie \r na końcu czytania   TODO CHECK because changed
                connections[countcon++].set(S,E,W);
            }
            connections.trunc(countcon);
            if(Input.fail())
            {
                cerr<<"AT LINK #"<<countcon<<" AN ERROR OCCURED DURING DATA FILE READING"<<endl;
            }
        }
}

void LocalWorld::initialize_layers()
{  
   static int first=1;	//TYMCZASOWE WYŁĄCZENIE NADMIARU WYDRUKÓW!!!
   if(first)
        Log.GetStream()<<"CONFLICTS SIMULATION:";
   
   switch(mode)
   {
   case 0: this->InitialiseNotConnected(how_many_agents);
       Log.GetStream()<<"\nMode 0: Not connected"<<endl;
       cout<<"\nMode 0: Not connected"<<endl;
       break;
   case 1: this->InitialiseFullyConnected(how_many_agents);
       Log.GetStream()<<"\nMode 1: Undirected random weight connected"<<endl;
       cout<<"\nMode 1: Undirected random weight connected"<<endl;
       break;   
   case 2: this->InitialiseFromWiesiekFile(DefaultNetFileName);
       Log.GetStream()<<"\nMode 2: Undirected initialization from file "<<DefaultNetFileName<<endl;
       cout<<"\nMode 2: Initialisation from file "<<DefaultNetFileName<<"(Undirected)"<<endl;
       //Symmetrically, but from file
       break;
   case 4: this->InitialiseFromWiesiekFile(DefaultNetFileName);
       Log.GetStream()<<"\nMode 4: Directed initialization from file "<<DefaultNetFileName<<endl;
       cout<<"\nMode 4: Initialisation from file "<<DefaultNetFileName<<"(Directed)"<<endl;
       //Asymmetrically, but from file
       break; 
   default:
   case 3: this->InitialiseRandomConnected(how_many_agents);
       Log.GetStream()<<"\nMode 3: Directed random weight connected"<<endl;
       cout<<"\nMode 3: Directed random weight connected"<<endl;
       break;   
   }

 
    //...wydruk wartości parametrów symulacji
   if(first)
      Log.GetStream()
      <<"\nNumber of agents="<<Log.separator()<<agents.get_size()
      <<"\nNumber of connections="<<Log.separator()<<connections.get_size()
      <<"\n";
   first=0;	//Koniec pierwszego wywołania //TYMCZASOWO!!!
}

void LocalWorld::actualize_out_area()
{
    world::actualize_out_area();
    wb_pchar bufor(4000);
    bufor.prn("\nI=%g+-%g N=%g+-%g\n",m_init_st,r_init_st,m_of_noise,r_of_noise);
    OutArea->add_text( bufor.get_ptr_val() );
}


// user defined actions after read simulation state from a file
void LocalWorld::after_read_from_image()
{
    //...???
}

void LocalWorld::AllocSources() //Tworzy źródła danych
{
    pNodeX=new struct_array_source<Agent,double>(agents.get_size(),agents.get_ptr_val(),&Agent::x,"X" );      //Współrzędne węzłów w aranżacji
    pNodeY=new struct_array_source<Agent,double>(agents.get_size(),agents.get_ptr_val(),&Agent::y,"Y");      //------------//----------------
    pNodeR=new struct_array_source<Agent,double>(agents.get_size(),agents.get_ptr_val(),&Agent::r,"R");	//Rozmiar węzła w aranżacji
    pNodeR->set_min_max(0, 1);	//Dla DEBUG — promień jest stały na razie

    pNodeState=new struct_array_source<Agent,double>(agents.get_size(),agents.get_ptr_val(),&Agent::state,lang("Stan","State"));  //Aktywność węzłów
    pNodePreState=new struct_array_source<Agent,double>(agents.get_size(), agents.get_ptr_val(), &Agent::p_state, lang("Pop.Stan", "PrevSt"));  //Dawna aktywność węzłów
    if(FixMinState<FixMaxState)
    {
        pNodeState->set_min_max(FixMinState, FixMaxState);
        pNodePreState->set_min_max(FixMinState, FixMaxState);
    }
    pNodeDelta=new struct_array_source<Agent,double>(agents.get_size(),agents.get_ptr_val(),&Agent::delta,"Delta");  //Dawna aktywność węzłów
    
    pConnStart=new struct_array_source<Connection,size_t>(connections.get_size(),connections.get_ptr_val(),&Connection::start_node,lang("Początki","Starts"));	//Indeksy początków linii łączących węzły sieci
    pConnEnd=new struct_array_source<Connection,size_t>(connections.get_size(),connections.get_ptr_val(),&Connection::end_node,lang("Końce","Ends"));	//Indeksy końców linii łączących węzły sieci
    pConnWeight=new struct_array_source<Connection,double>(connections.get_size(),connections.get_ptr_val(),&Connection::weight,lang("Wagi","Weights"));	//Wagi połączeń
    pConnAcct=new struct_array_source<Connection,double>(connections.get_size(), connections.get_ptr_val(), &Connection::last_act, lang("Aktywność", "Activity"));	//Aktywności połączeń

    StateStat=new generic_basic_statistics_source(pNodeState);  assert(StateStat!=NULL);	//Statystyka stanów węzłów
    MeanStateLog=new fifo_source<double>(StateStat->Mean(), logs_length);assert(MeanStateLog != NULL);	//Kolejka fifo ze średnimi
    MinStateLog=new fifo_source<double>(StateStat->Min(), logs_length);assert(MinStateLog != NULL);	//Fifo z minimum
    MaxStateLog=new fifo_source<double>(StateStat->Max(), logs_length);assert(MaxStateLog != NULL);	//Fifo z maximum

    AcctStat=new generic_basic_statistics_source(pConnAcct);  assert(AcctStat!=NULL);	//Statystyka stanów węzłów
    MeanAcctLog=new fifo_source<double>(AcctStat->Mean(), logs_length);assert(MeanAcctLog != NULL);	//Kolejka ze średnimi
    MinAcctLog=new fifo_source<double>(AcctStat->Min(), logs_length);assert(MinAcctLog != NULL);	//Fifo z minimum
    MaxAcctLog=new fifo_source<double>(AcctStat->Max(), logs_length);assert(MaxAcctLog != NULL);	//Fifo z maximum
}

void LocalWorld::make_basic_sources()
{
    world::make_basic_sources();

    AllocSources();
    //Series ---> this->Sources
    Sources.insert(pNodeX); //1 oznacza, ze manager ma nie zwalniać
    Sources.insert(pNodeY);
    Sources.insert(pNodeR);
    Sources.insert(pNodeState);
    Sources.insert(pNodePreState);
    Sources.insert(pNodeDelta);

    Sources.insert(pConnStart);
    Sources.insert(pConnEnd);
    Sources.insert(pConnWeight);
    Sources.insert(pConnAcct);
    
    Sources.insert(StateStat);
    Sources.insert(MinStateLog);
    Sources.insert(MeanStateLog);
    Sources.insert(MaxStateLog);

    Sources.insert(AcctStat);
    Sources.insert(MinAcctLog);
    Sources.insert(MeanAcctLog);
    Sources.insert(MaxAcctLog);

    //I umieszczanie w logu tego co trzeba
    Log.insert(StateStat->Mean());
    Log.insert(StateStat->Min());
    Log.insert(StateStat->Max());
    Log.insert(StateStat->SD());

    Log.insert(AcctStat->Mean());
    Log.insert(AcctStat->Min());
    Log.insert(AcctStat->Max());
    Log.insert(AcctStat->SD());
}

void LocalWorld::make_default_visualisation() // area_manager_base& Lufciki     ?
{
                                            assert(this->HasAreaMenager());
    world::make_default_visualisation(); //Tworzy np. OutArea

    this->OutArea->set(0,0,250,250);

    graph* pom=NULL;
    
    float circle_max_size=(1.0f/agents.get_size())*0.5*M_PI;	//*1.5*M_PI;	// 2*Pi ??

    if(connections.get_size()>0)
    {
        net_graph* png;
        pom=png=new net_graph(250,0,500,250+3*char_height('X'),
            pNodeX,0,
            pNodeY,0,
            pConnStart,0,
            pConnEnd,0,
            
            pNodeState,0,	//KOLORY - STAN
            pNodeR,0, //Rozmiary — promień
            NULL,0, //Arrows,1,
            pConnAcct,0,
            new circle_point(0.01f,circle_max_size),1
            );

        png->set_background(default_dark_gray);
        //png->set_data_colors(256,511);
    }
    else
    {
        pom=new scatter_graph(250,0,500,250+3*char_height('X'),
            pNodeX,0,
            pNodeY,0,
            pNodeState,0,	//KOLORY - STAN
            pNodeR,0, //Rozmiary — promień
            new circle_point(0.01f,circle_max_size),1);
        pom->set_background(default_dark_gray);
    }

    pom->set_frame(128);
    pom->set_title(lang("MAPA SIECI", "NETWORK MAPP"));
    this->MyAreaManager().insert(pom);

    //Zależności stanów nowych od poprzednich dla każdego węzła
    pom=new scatter_graph(this->MyAreaManager().get_width() - 249,
                          this->MyAreaManager().get_height() - 250 - 3 * char_height('X'),
                          this->MyAreaManager().get_width() - 1,
                          this->MyAreaManager().get_height() - 1,
            pNodePreState,0,
            pNodeState,0,
            pNodeDelta,0
            );
    pom->set_frame(64);
    pom->set_background(default_light_gray);
    pom->set_title("Xn-i vs. Xn");
    //pom->series_info->setminmx();
    this->MyAreaManager().insert(pom);

    //STATYSTYKA STANÓW 
    {
    data_source_base* data[4]={MinStateLog,MeanStateLog,MaxStateLog,NULL};
    pom=new sequence_graph(1,250,250,550,	//domyślne współrzędne
                                3,data,
                                //0/* Z reskalowaniem */);
                                1/*Wspólne minimum/maximum*/);
    assert(pom);
        pom->set_frame(128);
        pom->set_title(lang("HISTORIA STAN�W", HISTofSTATES));
        this->MyAreaManager().insert(pom);
    }

    //Statystyka aktywności
    {
    data_source_base* data[4]={MinAcctLog,MeanAcctLog,MaxAcctLog,NULL};
    pom=new sequence_graph(250,250+3*char_height('X')+1,500,550,
                                3,data,
                                //0/* Z reskalowaniem */);
                                1/*Wspólne minimum/maximum*/);
    assert(pom);
        pom->set_frame(128);
        pom->set_title(lang("HISTORIA AKTYWNOŚCI", HISTofACCT));
        this->MyAreaManager().insert(pom);
    }

    Sources.new_data_version(1,1);	//Oznajmia seriom, że dane się uaktualniły	(po inicjacji)
}

inline void LocalWorld::Agent::_setstate(double i_state)
//Bezpośrednie nadawanie stanu — bez funkcji mapujących
{
    state=i_state;
}
inline void LocalWorld::Agent::setpos(double ix, double iy, double ir/*=-1*/)
{
    x=ix;y=iy;
    if(ir>=0) r=ir;
}
inline void LocalWorld::Connection::set(size_t s, size_t e, double w)
{
    start_node=s;end_node=e;weight=w;
}


//Klasa do przechowywania i wizualizowania danych spoza świata symulacji
//i/albo o poszczególnych światach symulacji (dla różnych parametrów)
//================================================================================
class MetaExperiment
//===================
{
    class tangens//Klasa funkcyjna opakowująca tangens
    {
    public:
        double operator () (double x) { return tan(x);}
    };
    
    class arcus_tangens//Klasa funkcyjna opakowująca arcustangens
    {
    public:
        double operator () (double x){ return atan(x);}
    };
    
    class hiperbolic_tangens//Klasa funkcyjna opakowująca tangens hiperboliczny
    {
    public:
        double operator () (double x){ return tanh(x);}
    };
    
    class sigmoida//Klasa funkcyjna opakowująca funkcję sigmoidalną
    {
    public:
        double operator () (double x) { return 1/(1+exp(-x));}
    };

    template<unsigned power>
    class pierwiastek_nieparzysty//Klasa funkcyjna opakowująca nieparzyste pierwiastki
    {
    public:
        double operator () (double x) 
        {
            if(x==0) return 0;
            else
            if(x>0)
                return pow(x,1.0/power);
            else
                return -pow(-x,1.0/power);

        }
    };

    class Point
    {
        double X,Y,Z;
    public:
        Point():X(-DBL_MAX),Y(-DBL_MAX),Z(-DBL_MAX)	//Muszą być "missing"
        {}
        
        void Set(double ix,double iy,double iz)
        { X=ix;Y=iy;Z=iz;}
    
        friend class MetaExperiment;	//Potrzebne do insertu do Sources
    };

    class Arrow
    {
        unsigned S,E;
        double W;
    public:
        Arrow():S(UINT_MAX),E(UINT_MAX),W(-DBL_MAX)	//Muszą być "missing"
        {}

        void Set(unsigned is,unsigned ie,double iw)
        { S=is;E=ie;W=iw;} 

        friend class MetaExperiment;	//Potrzebne do insertu do Sources
    };

    wb_dynarray<unsigned int> histogram_rnd;
    wb_dynarray<unsigned int> histogram_gau;   
    wb_dynarray<Arrow>  arrows;
    wb_dynarray<Point>  points;
    unsigned countexp;
    unsigned countpoints;
    //Zarządzanie źródłami danych
    sources_manager			Sources;	//Zarządca seri przekaźnikowych
    //Obiekt rządzący zapisem historii
    logfile					Log;		//plik z zapisem historii symulacji
    
    void fill_rands();

    public:
        MetaExperiment(): Sources(20), Log(100, MetaExpFileName),
                          histogram_rnd(202), histogram_gau(202),
                          countexp(0), countpoints(0)
        { fill_rands();}
        
        void AllocForExperiments(unsigned HowManyExperiments)
        {
            points.alloc(3*HowManyExperiments);
            arrows.alloc(HowManyExperiments);            
        }

        void AddExperiment(double X,    //Tutaj będzie to średni szum
                           double Y0,double Y1,double Y2,	//Początkowy średni stan, środkowy i końcowy
                           double Z0,double Z1,double Z2,	//0,0,1 - strzałki mają puste końce, docelowy jest pełny
                           double W=1 //Waga strzałki dowolna
                           );      

       void make_areas(area_manager& Lufciki); //Generowanie lufcików demonstracyjnych meta-świata
       void save_for_spreadsheet(const char* filename);
};	//end of META-ŚWIAT

void MetaExperiment::AddExperiment(double X,    //Tutaj będzie to średni szum
                                   double Y0, double Y1, double Y2,	//Początkowy średni stan, środkowy i końcowy
                                   double Z0, double Z1, double Z2,	//0,0,1 - strzałki mają puste końce, docelowy jest pełny
                                   double W //Waga strzałki dowolna
                                   )	//
{
    assert(arrows.get_size()>countexp);	//Tylko tu się zgadza
    
    points[countpoints++].Set(X,Y0,Z0);	//Punkt startowy
    
    double Y01=Y1;
    if(Y0>Y1)
        Y01=Y0-(ARROWS_DIV>0?(Y0-Y1)/ARROWS_DIV:0.05);
    else
        Y01=Y0+(ARROWS_DIV>0?(Y1-Y0)/ARROWS_DIV:0.05);
    //(Y1-Y0)/10;
    
    points[countpoints++].Set(X,Y01,Z1);	//Punkt startowy
    //points[countpoints++].Set(X,Y1,Z1);	//Punkt po kilku krokach
    
    arrows[countexp++].Set(countpoints-2,countpoints-1,W);
    
    points[countpoints++].Set(X,Y2,Z2);	//Końcowy punkt
    Sources.new_data_version(1,1);	//Oznajmia seriom, że dane się uaktualniły	(po inicjacji)
}

void MetaExperiment::save_for_spreadsheet(const char* filename)
{
    ofstream Out(filename);
    if(!Out.is_open())
    {
        cerr<<"Can't open "<<filename<<" for writing."<<endl;
        perror("SYSTEM");
    }

    Out<<'X'<<'\t'<<"Attractor"<<'\t'<<"Field"<<'\t'<<"Heads"<<'\t'<<"Bases"<<endl;
    
    for(unsigned i=0;i<arrows.get_size();i++)
    {
        unsigned s=arrows[i].S;
        unsigned e=arrows[i].E;
        Out<<points[e+1].X<<'\t'<<points[e+1].Y<<'\t'<<""/*MISS*/ <<'\t'<<""/*MISS*/ <<'\t'<<""/*MISS*/<<endl;
        Out<<  points[s].X<<'\t'<<""/*MISS*/   <<'\t'<<points[s].Y<<'\t'<<""/*MISS*/ <<'\t'<<points[s].Y<<endl;
        Out<<  points[e].X<<'\t'<<""/*MISS*/   <<'\t'<<points[e].Y<<'\t'<<points[e].Y<<'\t'<<""/*MISS*/<<endl;        
    }
}

void MetaExperiment::make_areas(area_manager& Lufciki) //Generowanie lufcików demonstracyjnych meta-świata
{
    int ret=0;    
    const int BUTWIDTH=90;
    
    if(points.get_size()>0)
    {
        //Serie do meta-grafu!
        data_source_base* _X,*_Y,*_Z,*_S,*_E,*_Ayes,*_AC=NULL;

        Sources.insert(_X=new struct_array_source<Point,double>(points.get_size(),points.get_ptr_val(),&Point::X,"X"));
        assert(_X->get_missing()==-DBL_MAX);
        _X->set_min_max(-1.2, 1.2);
        Sources.insert(_Y=new struct_array_source<Point,double>(points.get_size(),points.get_ptr_val(),&Point::Y,"Y"));
        assert(_Y->get_missing()==-DBL_MAX);
        _Y->set_min_max(-1.1, 1.1);
        Sources.insert(_Z=new struct_array_source<Point,double>(points.get_size(),points.get_ptr_val(),&Point::Z,"Z"));
        assert(_Z->get_missing()==-DBL_MAX);
        
        Sources.insert( _S=new struct_array_source<Arrow,unsigned int>(arrows.get_size(),
                                                                 arrows.get_ptr_val(),
                                                                 &MetaExperiment::Arrow::S, //< S jest typu unsigned int!
                                                                 lang("Początki","Starts")
                                                                 )
                       ); //Indeksy początków linii łączących węzły sieci
        assert(_S->get_missing()==UINT_MAX);
        Sources.insert(_E=new struct_array_source<Arrow,unsigned int>(arrows.get_size(),arrows.get_ptr_val(),&Arrow::E,lang("Końce","Ends"))); //Indeksy końców linii łączących węzły sieci
        assert(_E->get_missing()==UINT_MAX);
        Sources.insert(_Ayes=new function_source<constans<10> >(arrows.get_size(),0,1000000,"10",0,10));
        
        net_graph* png=new net_graph(501, 0, Lufciki.get_width() - BUTWIDTH - 1, 250 + 3 * char_height('X'),
                                     _X, 0,
                                     _Y, 0,
                                     _S, 0,
                                     _E, 0,

                                     NULL, 0,	//KOLORY - STAN
            _Z, 0, //Rozmiary — promień
            _Ayes, 0, //Arrows,1,
            _AC, 0,
            //new circle_point(0.00001f,0.01f),1
            new rhomb_point(0.00001f,DIAM_SIZE), 1
            );
        //png->set_background(default_light_gray);
        png->set_title(META_GRAPH);
        Lufciki.insert(png);
    }
    
    //Wprowadzenie dodatkowych seri danych
    //int _0=Sources.insert(	/*0*/new function_source<tangens>(225,-2*3.141595,2*3.141595,"tan(x)"));
    int _1=Sources.insert(	/*1*/new function_source<hiperbolic_tangens>(225,-2*3.141595,2*3.141595,"tanh(x)"));
    int _2=Sources.insert(	/*2*/new function_source<arcus_tangens>(225,-2*3.141595,2*3.141595,"atan(x)"));
    int _3=Sources.insert(	/*3*/new function_source<sinus>(225,-2*3.141595,2*3.141595,"Sin(x)"));
    int _4=Sources.insert(	/*4*/new function_source<cosinus>(225,-2*3.141595,2*3.141595,"Cos(x) ",-1,1));	//Podane dokładne min i max
    int _5=Sources.insert(	/*5*/new array_source<unsigned int>(202,histogram_rnd.get_ptr_val(),"rnd(-100,100)"));
    int _6=Sources.insert(	/*6*/new array_source<unsigned int>(202,histogram_gau.get_ptr_val(),"gaus()"));
    int _7=Sources.insert(	/*7*/new function_source<sigmoida>(225,-2*3.141595,2*3.141595,"1/(1+exp(-x))"));
    int _8=Sources.insert(	/*8*/new function_source<pierwiastek_nieparzysty<3> >(225,-2*3.141595,2*3.141595,"pow(x,1/3)"));
    int _9=Sources.insert(	/*9*/new function_source<pierwiastek_nieparzysty<9> >(225,-2*3.141595,2*3.141595,"pow(x,1/9)"));
    graph* pom;
    
    unsigned BUTHEIGHT=unsigned(char_height('X')*1.2);
    pom=new sequence_graph(SCR_WIDTH - BUTWIDTH, 0, SCR_WIDTH, BUTHEIGHT, 6,
                           Sources.make_series_info(/*_0,*/_1,_2,_7,_8,_9,-1).get_ptr_val(),
                           2,	//MODE 2 - MIN & MAX PROVIDED
        -3, 3
        );    
    
    assert(pom!=NULL);
    pom->set_title(lang("FUNKCJE", "FUNCTIONS"));
    pom->set_frame(20);
    Lufciki.insert(pom);
    
    pom=new bars_graph(SCR_WIDTH - BUTWIDTH, BUTHEIGHT, SCR_WIDTH, 2 * BUTHEIGHT, Sources.get(_5));            assert(pom != NULL);
    pom->set_title("TEST RND");
    pom->set_frame(20);
    Lufciki.insert(pom);
    
    pom=new bars_graph(SCR_WIDTH - BUTWIDTH, 2 * BUTHEIGHT, SCR_WIDTH, 3 * BUTHEIGHT, Sources.get(_6));     assert(pom != NULL);
    pom->set_title("TEST PSEUDO-GAUSS");
    pom->set_frame(20);
    Lufciki.insert(pom);
    
    Sources.new_data_version(1,1);	//Oznajmia seriom, że dane się uaktualniły	(po inicjacji)
}


void MetaExperiment::fill_rands()
{
    histogram_rnd.fill(0);    histogram_gau.fill(0);
    
    for(int i=0;i<100000;i++)
    {
        int x=TheRandG.Random(201);
        histogram_rnd[x]++;
        double y=niby_gauss_max(100,200);        
        histogram_gau[unsigned(y)]++;
    }
}

/*   GŁÓWNE OBIEKTY I OGÓLNA FUNKCJA MAIN   */
/* **************************************** */


int main(int argc,const char* argv[])
{
//int i=0;
//int cont=1;	//flaga kontynuacji
//int std=0;
cout << SIMULATION_NAME << endl;
cout<<"Use '-help' for graphics setup information,\nor 'HELP' for information about available parameters."<<endl;
if(OptionalParameterBase::parse_options(argc,argv,Parameters,sizeof(Parameters)/sizeof(Parameters[0])))
    return 1;
LocalWorld MyNetworkWorld( //Model symulacyjny z różnymi parametrami
               def_mode,	//= 2; //0-bez połączeń, 1-symetryczne połączenia 2-asymetryczne połączenia
               def_num_of_nodes,	//= 50; //Ile jest węzłów

               def_m_init_st,	//= 0,  //Średni stan początkowy
               def_r_init_st,	//= 0.5,   //Odchylenie od średniego stanu początkowego
               def_m_of_noise,	//= 0,  //Średni poziom szumu/sygnału dodawanego do węzłów
               def_r_of_noise,	//= 0.5,  //Odchylenie od śred. szumu/sygnału dodawanego do węzłów
               def_m_of_weight,	//= 0.5, //Średnia waga krawędzi
               def_r_of_weight,	//= 0.1 //Odchylenie od śred. wagi krawędzi
               def_asymmetry
               );
MetaExperiment MyMetaExp; //Obsługa meta-eksperymentu dla wielu ustawień symulacji
main_area_manager Lufciki(100, SCR_WIDTH, SCR_HEIGHT); //Zarządzanie ekranem
if(!Lufciki.start(SIMULATION_NAME, argc, argv, 1))
{
    printf("%s\n","Can't initialize graphics");
    exit(1);
}
else
//Utworzenie sensownej nazwy pliku(-ów) do zrzutów ekranu
{
    wb_pchar buf(strlen(SCREEN_DUMP_NAME) + 20);
    buf.prn("%s_%ld", SCREEN_DUMP_NAME, time(NULL));
    Lufciki.set_dump_name(buf.get());
}

// Przygotowanie danych i świata symulacji
//==========================================
//inicjalizacja globalnych randomizer-ów (TODO dlaczego obu na raz?)
TheRandG.Reset();
TheRandSTDC.Reset();


MyNetworkWorld.set_max_iteration(iMaxIterations);	//Ile najwięcej kroków
MyNetworkWorld.set_input_ratio(iViewRatio);   //Jak często wizualizować
MyNetworkWorld.set_log_ratio(iLogRatio);      //Jak często zapisywać do logu
//MyNetworkWorld.set_history_stream(HistName);  //Nazwa pliku z pełna historia symulacji
MyNetworkWorld.set_history_stream(NULL);      //Nie będzie historii symulacji

cout << SIMULATION_NAME << ": INITIALISED." << endl;
cout<<"Ctrl-I for HELP, Ctrl-Q to quit, Ctrl-A to start simulation."<<endl;

if(MultiExp<2)
{
    MyNetworkWorld.initialize(&Lufciki);
    MyMetaExp.make_areas(Lufciki);       //Serie i Lufciki testowe spoza właściwej symulacji
    Lufciki.replot();
    Lufciki.process_input();	//Pierwsze zdarzenia, które kończą się po ctrl-B
    //GŁÓWNA PĘTLA SYMULACJI
    cout << SIMULATION_NAME << ": STARTED." << endl;
    MyNetworkWorld.simulation_loop(0/*wyłączone automatyczne wychodzenie po zakończeniu symulacji*/);
    cout << SIMULATION_NAME << ": CLOSING." << endl;
}
else
{     
    wb_pchar dump_name(2048);
    dump_name.prn("con1-T%d-ri_%g-rn_%g-w_%g(%g)-Ax%g-%dnod#",
                         def_mode,
                               def_r_init_st,
                                     def_r_of_noise,
                                           def_m_of_weight,
                                              def_r_of_weight,
                                                    double(def_asymmetry),
                                                         def_num_of_nodes);  
    
    int symulacja=0;  //Aż tyle ich nie będzie, żeby potrzebne było `unsigned`

    for(double StartMeanState=defm_start_mean_state;StartMeanState<=defm_end_mean_state;StartMeanState+=(defm_end_mean_state-defm_start_mean_state)/MultiExp)	//Podwójna pętla
        for(double NoiseMean=defm_start_noise_mean;NoiseMean<=defm_end_noise_mean;NoiseMean+=(defm_end_noise_mean-defm_start_noise_mean)/MultiExp)	//Po stanie początkowym i szumie
        {  
            //Niestety reinicjalizacja, bo domyślne parametry to nie to!
            MyNetworkWorld.SetModelParameters(
                StartMeanState,	//Średni stan początkowy
                def_r_init_st,	//Odchylenie od średniego stanu początkowego
                NoiseMean,	//Średni poziom szumu/sygnału dodawanego do węzłów
                def_r_of_noise,	//Odchylenie od śred. szumu/sygnału dodawanego do węzłów
                def_m_of_weight,	//=0.5, //Średnia waga krawędzi
                def_r_of_weight,	//=0.1 //Odchylenie od śred. wagi krawędzi
                def_asymmetry
                );   
            
            if(symulacja==0) //Jak pierwsza z symulacji to musimy poustawiać wszystkie "pomoce naukowe"
            {
                MyMetaExp.AllocForExperiments((MultiExp + 1) * (MultiExp + 1));	//Z lekkim zapasem
                MyNetworkWorld.initialize(&Lufciki);
                MyMetaExp.make_areas(Lufciki);           //Serie i Lufciki testowe spoza właściwej symulacji
                
                int statusWin=Lufciki.search("STATUS");
                int histWin=Lufciki.search(HISTofSTATES);	// HISTofSTATES="HISTORY OF THE STATE"
                int histAcc=Lufciki.search(HISTofACCT);	// HISTofACCT="HISTORY OF THE ACTIVITY"
                int metaWin=Lufciki.search(META_GRAPH);
                wb_dynarray<int> co(size_t(4),statusWin,histWin,histAcc,metaWin);
                Lufciki.tile(co);
                MyNetworkWorld.set_max_iteration(iMaxIterRep);
                MyNetworkWorld.set_log_ratio(10);
                Lufciki.enable_background();
                Lufciki.maximize(Lufciki.search("STATUS"));
            }
            else
            {
                MyNetworkWorld.restart();	//A jak następne to już wystarczy tylko zrestartować i gotowe
            }
            
            //GŁÓWNA PĘTLA SYMULACJI
            symulacja++;
            Lufciki.set_dump_name(dump_name.get(),symulacja*100);
            MyNetworkWorld.MyLogStream() << "BEGIN OF REP #" << symulacja
                <<" M.of.INITIAL=\t"<<StartMeanState<<"\tM.of.NOISE=\t"<<NoiseMean<<endl;
            cout << SIMULATION_NAME << ": SIMULATION " << symulacja << " STARTED." << endl;
            
            MyNetworkWorld.simulation_loop(1);
            if(!Lufciki.should_continue()) goto USER_ABORT;

            cout << SIMULATION_NAME << ": SIMULATION " << symulacja << " DONE." << endl;
            
            //Dodanie wyników do listy eksperymentów
            double Y0=MyNetworkWorld.MeanStates().get(unsigned(0));
            double YT=MyNetworkWorld.MeanStates().get(unsigned(1));
            double YF=MyNetworkWorld.MeanStates().get(unsigned(iMaxIterRep - 1));
            MyMetaExp.AddExperiment(NoiseMean, Y0, YT, YF, 0, 0.1, iMaxIterRep);

            if(symulacja<MultiExp-1)
            {
                MyNetworkWorld.MyLogStream() << "END OF REP #" << symulacja << endl;
            }
        }
        
        cout << SIMULATION_NAME << ": MULTI-SIMULATION FINISHED ." << endl;
        //Lufciki.need_confirmation_before_clean();
        dump_name.add(".%s","out");
        MyMetaExp.save_for_spreadsheet(dump_name.get());
USER_ABORT:
        Lufciki.maximize(Lufciki.search(META_GRAPH));
        Lufciki.disable_background();
        Lufciki.run_input_loop();           
}

//I po zabawie
printf("Do widzenia!!!\n");
return 0;
//Gdzieś tu albo troche dalej destruktory...
}

/* ****************************************************************** */
/*        SYMSHELL2 EXAMPLE  version 2006/2022/2026                   */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
#pragma clang diagnostic pop
