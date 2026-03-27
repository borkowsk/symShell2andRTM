/// @file
/// DECLARATION OF   W O R L D  FOR "Conways Life" SIMULATION.
// ///////////////////////////////////////////////////////////
/// @date 2026-03-26 (modified)

#include <limits.h> //SHRT_MAX
#include "world.hpp"
#include "layer.hpp"
#include "lagent.h" //Definicja agenta

/// Cały świat symulacji Life.
class lifeworld:public world
//--------------------------------------------------
{
    // Parametry jednowartościowe:
    // ///////////////////////////

    size_t				MyWidth;	//!< Obwód torusa
    short				IleKate;	//!< Liczba kategorii w mapach
    short				IleSasiad;	//!< 8 == gęstość sąsiedztwa
    short				OdlSasiad;	//!< Rozmiar sąsiedztwa
    short				BierzWszystko; //!< Czy ma brać wszystko z sąsiedztwa

    double				Noise;		//!< Szum informacyjny
    bool				Synchronic; //!< Synchroniczna zmiana "poglądów"
    wb_pchar			MaplName;	//!< Nazwa pliku inicjującej bitmapy/grafiki


    // Warstwy symulacji (są torusami):
    // ////////////////////////////////

    //rectangle_unilayer<unsigned char> zdatność; //!< Warstwa definiująca zdatność do zasiedlenia
    rectangle_layer_of_agents<lifeagent> Agenci;  //!< Właściwa warstwa agentów zasiedlających

    // Główne serie — wygodniej miec wskaźniki niż odszukiwać z Sources po nazwach:
    // //////////////////////////////////////////////////////////////////////////////

    struct_matrix_source<lifeagent,short>		*Firsts; //=Agenci.make_source("First mem",&lifeagent::First);
    struct_matrix_source<lifeagent,short>		*Seconds; //=Agenci.make_source("Second mem",&lifeagent::Second);


    int CheckChange(const geometry_base* MyGeom,size_t index,lifeagent& CenterAgent);

    public:
    //KONSTRUKCJA DESTRUKCJA
    lifeworld(
          size_t Width,	//Szerokość torusa / macierzy agentów
          char* log_name,	//Nazwa pliku do zapisywania historii
          char* mapl_name,	//Nazwa (bit)mapy inicjującej "składowe"
          double Noise=0,
          short	ile_kate=2,		//Liczba kategorii w mapach
          short	odl_sasiad=1,	//Rozmiar sąsiedztwa
          short	ile_sasiad=8,	//8 == Gęstość sąsiedztwa — jeśli -1 to wszystko po kolei
          bool	synchronicly=true,
          double spontanic=0	//Prawdopodobieństwo spontanicznej zmiany "stanu"
          );

    ~lifeworld(){}

    protected:
    //AKCJE
    void	initialize_layers();     //Stan startowy symulacji
    void	after_read_from_image(); //Actions after read state from a file. Aktualizacja pol static `lifeagent`-a!!!
    void	simulate_one_step();     //Właściwa implementacja kroku symulacji

    //Wspolpraca z menagerem wyswietlania
    //---------------------------------------------
    void	make_default_visualisation() override; //Tworzy domyślne "lufciki" i umieszcza w zarządcy lufcików ustawionym w world::initialise(...)
    //void actualize_out_area(); // aktualizacja zawartości `OutArea` po `n` krokach symulacji

    //Generuje podstawowe źródła dla wbudowanego manager-a danych lub innego
    void	make_basic_sources() override;

    //Implementacja wejścia/wyjścia. Zwracaj 1, jeśli sukces!
    virtual
    int		implement_output(ostream& o) const;
    virtual
    int		implement_input(istream& i);
};

// /////////////////////////////////////////////
// Example for SYMSHELL CLASSES library.
/// @author Wojciech Borkowski, iss.uw.edu.pl
// https://github.com/borkowsk/symShell2andRTM
/// @copyright 2000 - 2026
// /////////////////////////////////////////////
