/// @file
/// @EN{  }
/// @PL{  }
/// @date 2026-05-19 (modified)
/// =========================================================
///
/// @brief DECLARATION OF   W O R L D  FOR "attitudes" SIMULATION.
/// @date 2026-05-19 (modification)
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <climits> //SHRT_MAX
#include "world.hpp"
#include "layer.hpp"
#include "aagent.h" //Definicja agenta

class aworld:public sym2::world	//Caly świat symulacji
//--------------------------------------------------
{
    // Parametry jednowartosciowe:
    // ///////////////////////////
    size_t				MyWidth;	//Obwod torusa
    short				MaxSila;	//Maksymalna siła agenta
    short				ThrsStr;	//Próg siły powyżej ktorego nie ma zmian
    short				NofCateg;	//Liczba kategori w mapach
    short				NofNeigh;	//8 == Gęstość sąsiedztwa (ilu sąsiadów losujemy,albo bierzemy wszystkich gdy -1)
    short				NeighRadius;	//Rozmiar sąsiedztwa
    short				TakeAll;	//Czy ma brac wszystko z sąsiedztwa
    short				UseSelf;	//Czy ma brac siębie pod uwage
    double				Noise;		//Szum informacyjny
    bool				SyncChange; //Synchroniczna zmiana poglądow
    wb_pchar			MappName;	//nazwa pliku inicjującej bitmapy
    wb_pchar			MaplName;	//nazwa pliku inicjującej bitmapy
    wb_pchar			MaskName;	//nazwa pliku inicjującej bitmapy

    //Warstwy symulacji (sa torusami)
    // ///////////////////////////////
    
    //rectangle_unilayer<unsigned char> zdatnosc; //Warstwa definiujaca zdatnosc do zasiędlenia
    sym2::rectangle_layer_of_ptr_to_agents<aagent> Agents;  //Wlaściwa warstwa agentów zasiędlajacych

    //Glowne serie - wygodniej miec wskazniki niz odszukiwac z Sources po nazwach
    // //////////////////////////////////////////////////////////////////////////////
    
    sym2::data::ptr_to_struct_matrix_source<aagent,short>		*Firsts; //=Agents.make_source("First mem",&aagent::First);
    sym2::data::ptr_to_struct_matrix_source<aagent,short>		*Seconds; //=Agents.make_source("Second mem",&aagent::Second);

    sym2::data::ptr_to_struct_matrix_source<aagent,short>		*Powers; //=Agents.make_source("Power",&aagent::Power);

    int CheckChange(const sym2::geometry_base* MyGeom, size_t index, aagent& CenterAgent);

public:
    // KONSTRUKCJA DESTRUKCJA:
    // ///////////////////////
    aworld(size_t Width,	//Szerokość torusa macierzy agentów
          char* log_name,	//Nazwa pliku do zapisywania historii
          char* mapl_name,	//Nazwa (bit)mapy inicjującej "składowe"
          char* mapp_name,	//Nazwa (bit)mapy inicjującej "sily"
          char* live_mask,	//Czarne w tej mapie są kasowane
          double noise_p=0,
          short	max_str=255, //Maksymalna siła agenta
          short	n_of_cate=256, //Liczba kategori w mapach
          short	neigh_radius=1, //Rozmiar sąsiedztwa
          short	n_of_neigh=8, //8 == Gęstość sąsiedztwa	- jesli -1 to wszystko po kolei
          short	need_use_self=0,
          bool	sync_update=true,
          short	walk_str=0,	//Czy siła rosnie z wiekiem agenta
          short	str_thres=SHRT_MAX,	//Sila powyżej ktorej agent jest odporny na wplyw
          double	spon_prob=0	//Prawdopodobienstwo spontanicznej zmiany poglądu
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

    //Generuje podstawowe źródla dla wbudowanego menagera danych lub innego
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
