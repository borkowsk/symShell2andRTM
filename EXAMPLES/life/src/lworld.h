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

/// @brief
///     @PL{ Cały świat symulacji "Life". }
///     @EN{ Whole simulation world for "Life". }
class life_world: public sym2::shell::world
//--------------------------------------------------
{
    /// @name Parametry jednowartościowe:
    /// @details Ustawienia modelu i inicjalizacji agentów.
    //=====================================================
    /// @{
    size_t				MyWidth;		//!< Obwód torusa.
    short				NofCat;			//!< Liczba kategorii w mapach.
    short				NeighDens;		//!< 8 == gęstość sąsiedztwa.
    short				NeighRadius;	//!< Rozmiar sąsiedztwa.
    short				ConsiderAll;	//!< Czy ma brać wszystko z sąsiedztwa.

    double				Noise;			//!< Szum informacyjny.
    bool				Synchronic;		//!< Synchroniczna zmiana "poglądów".
    wb_pchar			MapLName;		//!< Nazwa pliku inicjującej bitmapy/grafiki.
    /// @}

    /// @name Warstwy symulacji (są torusami):
    //========================================
    /// @{
    //rectangle_unilayer<unsigned char> zdatność; //!< Warstwa definiująca zdatność do zasiedlenia
    rectangle_layer_of_agents<life_agent> Agents;  //!< Właściwa warstwa agentów zasiedlających
    /// @}
    
    /// @name Główne serie danych.
    /// @details Wygodniej mieć wskaźniki niż odszukiwać z `Sources` po nazwach.
    // //////////////////////////////////////////////////////////////////////////////
    /// @{
    sym2::data::struct_matrix_source<life_agent,short>		*Firsts; //=Agents.make_source("First mem",&life_agent::First);
    sym2::data::struct_matrix_source<life_agent,short>		*Seconds; //=Agents.make_source("Second mem",&life_agent::Second);
    /// @}

    /// Sprawdza zmianę stanu komórki.
    int CheckChange(const geometry_base* MyGeom, size_t index, life_agent& CenterAgent);

public:
    //KONSTRUKCJA DESTRUKCJA:
    // //////////////////////
    
    /// Konstruktor.
    life_world(
          size_t	Width,				///< Szerokość torusa / macierzy agentów
          char*		log_name,			///< Nazwa pliku do zapisywania historii
          char*		map_l_name,			///< Nazwa (bit)mapy inicjującej "składowe"
          double	Noise=0,			///<  ???
          short		n_of_cat=2,			///< Liczba kategorii w mapach
          short		neigh_radius=1,		///< Rozmiar sąsiedztwa
          short		neigh_dens=8,		///< 8 == Gęstość sąsiedztwa — jeśli -1 to wszystko po kolei
          bool		synchronously=true,	///< ???
          double	spontaneously=0		///< Prawdopodobieństwo spontanicznej zmiany "stanu"
          );

    /// Destruktor udający, że nic nie robi.
    ~life_world() override= default;

protected:
    /// @name GŁÓWNE AKCJE:
    //=====================
    /// @{
    void	initialize_layers() override;     //!< Stan startowy symulacji.
    void	after_read_from_image() override; //!< Actions after read state from a file. Aktualizacja pol static `life_agent`-a!!!
    void	simulate_one_step() override;     //!< Właściwa implementacja kroku symulacji.
    /// @}
    
    /// @name Współpraca z managerem wyświetlania i zarządcą danych:
    //--------------------------------------------------------------
    /// @{
    void	make_default_visualisation() override; //!< Tworzy domyślne "lufciki". I umieszcza w zarządcy lufcików ustawionym w world::initialise(...)
    //void actualize_out_area(); // aktualizacja zawartości `OutArea` po `N` krokach symulacji

    /// Generuje podstawowe źródła dla wbudowanego manager-a danych.
    void	make_basic_sources() override;
    /// @}
    
    /// @name Implementacja wejścia/wyjścia:
    //--------------------------------------
    /// @{
    int		implement_output(ostream& o) const override;
    int		implement_input(istream& i) override;
    /// @}
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


