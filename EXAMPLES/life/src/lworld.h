/// @file
/// @brief
///  @EN{ DECLARATION OF 'life_world' FOR "Conways Life" SIMULATION. }
///  @PL{  }
/// @date 2026-05-29 (modified)
///       =========================================================
/// @details ...
// =====================================================================================================================

#include <climits> //SHRT_MAX
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
    /// @name @PL{ Parametry modelu: } @EN{  }
    /// @details Ustawienia modelu i inicjalizacji agentów.
    //=====================================================
    /// @{
    size_t				MyWidth;		//!< Obwód torusa.
    short				NeighDens;		//!< 8 == gęstość sąsiedztwa.
    short				NeighRadius;	//!< Rozmiar sąsiedztwa.
    short				ConsiderAll;	//!< Określa, czy ma brać wszystko z sąsiedztwa.

    double				Noise;			//!< Szum informacyjny.
    bool				Synchronic;		//!< Synchroniczna zmiana "poglądów".
    wb_pchar			MapLName;		//!< Nazwa pliku inicjującej bitmapy/grafiki.
    /// @}

    /// @name  @PL{ Warstwy symulacji (tutaj tylko jedna): } @EN{  }
    //================================================================================================================
    /// @{
    rectangle_layer_of_agents<life_agent> Agents;  //!< @brief Właściwa warstwa agentów zasiedlających.
    /// @}
    
    /// @name @PL{ Główne serie danych. } @EN{  }
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
          size_t	Width,				///< Szerokość torusa / macierzy agentów.
          char*		log_name,			///< Nazwa pliku do zapisywania historii.
          char*		map_l_name,			///< Nazwa (bit-) mapy inicjującej "składowe".
          short		neigh_radius=1,		///< Rozmiar sąsiedztwa.
          short		neigh_dens=8,		///< 8 == Gęstość sąsiedztwa — jeśli -1 to wszystko po kolei.
          bool		synchronously=true,	///< Określa czy update jest synchroniczny.
          double	spontaneously=0		///< Prawdopodobieństwo spontanicznej zmiany "stanu".
          );

    /// Destruktor udający, że nic nie robi.
    ~life_world() override= default;

protected:
    /// @name @PL{ GŁÓWNE AKCJE: } @EN{  }
    //====================================================================================
    /// @{
    void	initialize_layers() override;     //!< Stan startowy symulacji.
    void	after_read_from_image() override; //!< Actions after read state from a file. Aktualizacja pol static `life_agent`-a!!!
    void	simulate_one_step() override;     //!< Właściwa implementacja kroku symulacji.
    /// @}
    
    /// @name @PL{ Współpraca z managerem wyświetlania i zarządcą danych: }
    ///       @EN{ Collaboration with the display manager and data manager: }
    //--------------------------------------------------------------
    /// @{

    /// Tworzy domyślne "lufciki" i umieszcza w zarządcy lufcików ustawionym w `world::initialise`.
    void	make_default_visualisation() override;

    /// Generuje podstawowe źródła dla wbudowanego manager-a danych.
    void	make_basic_sources() override;

    //void actualize_out_area(); //TODO
    /// @}
    
    /// @name @PL{ Implementacja wejścia/wyjścia: } @EN{  }
    //------------------------------------------------------------------------------------------------------
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


