// DECLARATION OF   W O R L D  FOR "attitudes" SIMULATION.
// ///////////////////////////////////////////////////////
// @date 2026-04-07 (modification)
#include <climits> //SHRT_MAX
#include "world.hpp"
#include "layer.hpp"
#include "aagent.h" //Definicja agenta

class aworld:public world	//Caly świat symulacji
//--------------------------------------------------
{
    // Parametry jednowartosciowe:
    // ///////////////////////////
    size_t				MyWidth;	//Obwod torusa
    short				MaxSila;	//Maksymalna sila agenta
    short				ThrsStr;	//Próg sily powyzej ktorego nie ma zmian
    short				NofCateg;	//Ilosc kategori w mapach
    short				NofNeigh;	//8==Gestosc sasiedztwa (ilu sąsiadów losujemy,albo bierzemy wszystkich gdy -1)
    short				NeighRadius;	//Rozmiar sasiedztwa
    short				TakeAll;	//Czy ma brac wszystko z sasiedztwa
    short				UseSelf;	//Czy ma brac siebie pod uwage
    double				Noise;		//Szum informacyjny
    bool				SyncChange; //Synchroniczna zmiana pogladow
    wb_pchar			MappName;	//nazwa pliku inicjujacej bitmapy
    wb_pchar			MaplName;	//nazwa pliku inicjujacej bitmapy
    wb_pchar			MaskName;	//nazwa pliku inicjujacej bitmapy

    //Warstwy symulacji (sa torusami)
    // ///////////////////////////////
    
    //rectangle_unilayer<unsigned char> zdatnosc; //Warstwa definiujaca zdatnosc do zasiedlenia
    rectangle_layer_of_ptr_to_agents<aagent> Agents;  //Wlaściwa warstwa agentow zasiedlajacych

    //Glowne serie - wygodniej miec wskazniki niz odszukiwac z Sources po nazwach
    // //////////////////////////////////////////////////////////////////////////////
    
    ptr_to_struct_matrix_source<aagent,short>		*Firsts; //=Agents.make_source("First mem",&aagent::First);
    ptr_to_struct_matrix_source<aagent,short>		*Seconds; //=Agents.make_source("Second mem",&aagent::Second);

    ptr_to_struct_matrix_source<aagent,short>		*Powers; //=Agents.make_source("Power",&aagent::Power);

    int CheckChange(const geometry_base* MyGeom,size_t index,aagent& CenterAgent);

public:
    // KONSTRUKCJA DESTRUKCJA:
    // ///////////////////////
    aworld(size_t Width,	//Szerokosc torusa macierzy agentow
          char* log_name,	//Nazwa pliku do zapisywania histori
          char* mapl_name,	//Nazwa (bit)mapy inicjujacej "skladowe"
          char* mapp_name,	//Nazwa (bit)mapy inicjujacej "sily"
          char* live_mask,	//Czarne w tej mapie sa kasowane
          double noise_p=0,
          short	max_str=255, //Maksymalna sila agenta
          short	n_of_cate=256, //Ilosc kategori w mapach
          short	neigh_radius=1, //Rozmiar sasiedztwa
          short	n_of_neigh=8, //8==Gestosc sasiedztwa	- jesli -1 to wszystko po kolei
          short	need_use_self=0,
          bool	sync_update=true,
          short	walk_str=0,	//Czy sila rosnie z wiekiem agenta
          short	str_thres=SHRT_MAX,	//Sila powyzej ktorej agent jest odporny na wplyw
          double	spon_prob=0	//Prawdopodobienstwo spontanicznej zmiany pogladu
          );

    ~aworld(){}

protected:
    // AKCJE:
    // //////
    void	initialize_layers();	//Stan startowy symulacji
    void	after_read_from_image(); //actions after read state from file. Aktualizacja pol static aagent'a!!!
    void	simulate_one_step();	//Wlasciwa implementacja kroku symulacji

    //Wspolpraca z menagerem wyswietlania
    //---------------------------------------------
    void	make_default_visualisation(); //Tworzy domyslne "lufciki" i umieszcza w
    //void actualize_out_area(); // aktualizacja zawartosci OutArea po n krokach symulacji

    //Generuje podstawowe zrodla dla wbudowanego menagera danych lub innego
    void	make_basic_sources();

    //Implementacja wejscia/wyjscia. Zwracaj 1 jesli sukces!
    virtual
    int		implement_output(ostream& o) const;
    virtual
    int		implement_input(istream& i);
};

/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */
