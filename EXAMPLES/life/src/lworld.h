/// @file
/// @brief
///  @EN{ DECLARATION OF 'life_world' FOR "Conways Life" SIMULATION. }
///  @PL{  }
/// @date 2026-05-21 (modified)
///       =========================================================
/// @details ...
// =====================================================================================================================

#include <limits.h> //SHRT_MAX
#include "world.hpp"
#include "layer.hpp"
#include "lagent.h" //Definicja agenta

using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;


/// @PL{ Cały świat symulacji "Life". }
/// @EN{ Whole simulation world for "Life". }
class life_world: public sym2::shell::world
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
    wb_pchar			MapLName;	//!< Nazwa pliku inicjującej bitmapy/grafiki


    // Warstwy symulacji (są torusami):
    // ////////////////////////////////

    //rectangle_unilayer<unsigned char> zdatność; //!< Warstwa definiująca zdatność do zasiedlenia
    rectangle_layer_of_agents<life_agent> Agenci;  //!< Właściwa warstwa agentów zasiedlających

    // Główne serie — wygodniej miec wskaźniki niż odszukiwać z Sources po nazwach:
    // //////////////////////////////////////////////////////////////////////////////

    sym2::data::struct_matrix_source<life_agent,short>		*Firsts; //=Agenci.make_source("First mem",&life_agent::First);
    sym2::data::struct_matrix_source<life_agent,short>		*Seconds; //=Agenci.make_source("Second mem",&life_agent::Second);


    int CheckChange(const geometry_base* MyGeom, size_t index, life_agent& CenterAgent);

    public:
    //KONSTRUKCJA DESTRUKCJA
    life_world(
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

    ~life_world() override= default;

    protected:
    //GŁÓWNE AKCJE
    void	initialize_layers() override;     //!< Stan startowy symulacji.
    void	after_read_from_image() override; //!< Actions after read state from a file. Aktualizacja pol static `life_agent`-a!!!
    void	simulate_one_step() override;     //!< Właściwa implementacja kroku symulacji.

    //Współpraca z menagerem wyświetlania:
    //------------------------------------
    void	make_default_visualisation() override; //!< Tworzy domyślne "lufciki". I umieszcza w zarządcy lufcików ustawionym w world::initialise(...)
    //void actualize_out_area(); // aktualizacja zawartości `OutArea` po `n` krokach symulacji

    /// Generuje podstawowe źródła dla wbudowanego manager-a danych.
    void	make_basic_sources() override;

    //Implementacja wejścia/wyjścia:
    //------------------------------
    int		implement_output(ostream& o) const override;
    int		implement_input(istream& i) override;
};

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


