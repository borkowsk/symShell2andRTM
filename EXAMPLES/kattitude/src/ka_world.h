/// @file
/// @brief
///  @EN{ DECLARATION OF THE 'ka_world' FOR "KattitudeS" SIMULATION. }
///  @PL{ DEKLARACJA 'ka_world' DLA SYMULACJI 'POSTAW'. }
/// @date 2026-05-29 (modified)
///       ============================================================
/// @details (attitudeS old example for SymShell)
// =====================================================================================================================

//#include <limits.h> //SHRT_MAX
#pragma once

#include "world.hpp"
#include "layer.hpp"
#include "ka_agent.h" //Definicja agenta

using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

/// @brief
///     @PL{ Cały świat symulacji `KattitudeS`. }
///     @EN{ The whole world of `KattitudeS` simulations. }
class ka_world: public sym2::shell::world
//---------------------------------------
{
    /// @name Parametry jednowartościowe:
    // //////////////////////////////////
    /// @{
    size_t				MyWidth;			//!< Obwód torusa.
    short				MaxPower;			//!< Maksymalna sila agenta.
    short				ThrPower;			//!< Threshold sily powyżej, którego nie ma zmian.
    short				IleKate;			//!< Liczba kategorii w mapach.
    short				NeiDens;			//!< 8 == Gęstość sąsiedztwa.
    short				NeiSize;			//!< Rozmiar sąsiedztwa (Neighborhood)
    short				TakeAll;			//!< Określa, czy ma brać wszystko z sąsiedztwa.
    double				WeightOfSelf;		//!< Z jaką wagą brać siebie pod uwagę (0..1).
    double				NeedForClosure;		//!< Znaczenie może być różne, zależnie od implementacji.
    double				Noise;				//!< Szum informacyjny.
    double				LifeFill;			//!< Udział żywych na początku.
    double				MigrationProb;		//!< Prawdopodobieństwo migracji.
    bool				Synchronic;			//!< Synchroniczna zmiana poglądów.
    wb_pchar			MappName;			//!< Nazwa pliku bitmapy inicjującej siły.
    wb_pchar			MapLName;			//!< Nazwa pliku bitmapy inicjującej stany.
    wb_pchar			MaskName;			//!< Nazwa pliku bitmapy inicjującej maskę obszarów zdatnych.
    /// @}

    // Warstwy symulacji (są torusami):
    // ////////////////////////////////

    //rectangle_layer_of<unsigned char> suitability;		//!< Warstwa definiująca zdatność do zasiedlenia (suitability or usefulness)
    rectangle_layer_of_ptr_to_agents<ka_agent> Agents;		//!< Właściwa warstwa agentów zasiedlających.

    /// @name Główne serie danych.
    /// @details Wygodniej i efektywniej mieć wskaźniki niż odszukiwać z Sources po nazwach:
    //======================================================================================
    /// @{
    sym2::data::ptr_to_struct_matrix_source<ka_agent,short>		*Firsts;		//!< =Agents.make_source("First mem",&aagent::First);
    sym2::data::ptr_to_struct_matrix_source<ka_agent,short>		*Seconds;		//!< =Agents.make_source("Second mem",&aagent::Second);

    sym2::data::ptr_to_struct_matrix_source<ka_agent,short>		*Powers;		//!< =Agents.make_source("Power",&aagent::Power);
    sym2::data::ptr_to_struct_matrix_source<ka_agent,short>		*Pressure;		//!< =Agents.make_source("Pressure",&aagent::Press);
    //sym2::data::method_by_ptr_matrix_source<aagent,long>		*Classify;		//!< =Agents.make_source("Classification",&aagent::classify);
    /// @}

    /// @name Do przekazywania aktualnie najważniejszych danych na okno statusu:
    //--------------------------------------------------------------------------
    /// @{
    sym2::data::scalar_source<double>*       ptrStres;			//!< Średni stress.
    sym2::data::scalar_source<double>*       ptrClsSize;		//!< Rozmiar klastra (średni czy maksymalny?)
    /// @}

    int  CountCh;			//!< Ilu ostatnio zmieniło pogląd (do celów statystycznych).
    int  CountMig;			//!< Ilu ostatnio migrowało (do celów statystycznych).

    sym2::data::ptr_to_scalar_source<int>*       ptrLastChanged;			//!< Do przekazywania liczników zmian.
    sym2::data::ptr_to_scalar_source<int>*       ptrLastMigration;			//!< Do przekazywania liczników zmian.

    double MaxPressure; ///< Do zapamiętania teoretycznie największej wartości "presji".

    /// @name Właściwa implementacja symulacji:
    //=========================================
    /// @{
    int  CheckChange(const rectangle_geometry* MyGeom, size_t index, ka_agent& CenterAgent);		//!< Sprawdzenie zmiany stanów.
    long DoMigration(const rectangle_geometry* MyGeom, size_t index, ka_agent& CenterAgent);		//!< Sprawdzenie możliwości migracji.
    /// @}
public:
    //KONSTRUKCJA DESTRUKCJA:
    // //////////////////////

    /// Konstruktor.
    ka_world(
            size_t	Width,				//!< Szerokość torusa macierzy agentów.
            char*	log_name,			//!< Nazwa pliku do zapisywania historii.
            char*	map_l_name,			//!< Nazwa bitmapy inicjującej "składowe".
            char*	mapp_name,			//!< Nazwa bitmapy inicjującej "siły".
            char*	live_mask,			//!< Czarne w tej mapie są kasowane.
            double	noise,				//!< Szum informacyjny.
            short	max_sila,			//!< Maksymalna sila agenta.
            short	ile_kate,			//!< Liczba kategorii w mapach.
            short	nei_radius,			//!< Rozmiar sąsiedztwa.
            short	nei_density,		//!< 8 == Gęstość sąsiedztwa.
            double	need_use_self,		//!< Z jaką wagą ma brać siebie.
            double	need_for_something,	//!< Z jaka waga brać innych.
            bool	synchronously,		//!< ...
            short	walk_power,			//!< ...
            short	thr_power,			//!< ...
            double	spontaneously,		//!< ...
            double	fill,				//!< ...
            double	migration_prob,		//!< ...
            double	majority			//!< ...
          );

    /// Destruktor.
    ~ka_world() override = default;

protected:
    /// @name WYMAGANE DUŻE AKCJE:
    //============================
    /// @{
    void	initialize_layers() override;		//!< Stan startowy symulacji
    void	after_read_from_image() override;	//!< Actions after read state from a file. Aktualizacja pól statycznych aagent-a!!!
    void	simulate_one_step() override;		//!< Właściwa implementacja kroku symulacji
    /// @}

    /// @name Współpraca z zarządcą wyświetlania i zarządcą danych:
    //-------------------------------------------------------------
    /// @{
    /// Tworzenie domyślnych "lufcików" i umieszczanie ich na liście zarządcy.
    void	make_default_visualisation() override;

    /// Wypisywanie/dopisywanie na konsole statusu.
    void    actualize_out_area() override; //!< Aktualizacja zawartości `OutArea` co ileś kroków symulacji.

    /// Generuje podstawowe źródła dla wbudowanego zarządcy danych.
    void	make_basic_sources() override;
    /// @}

    /// @name Implementacja wejścia/wyjścia:
    //--------------------------------------
    /// @{
    int		implement_output(ostream& o) const override;		//!< Serializacja. @returns 1, jeśli sukces!
    int		implement_input(istream& i) override;				//!< Deserializacja. @returns 1, jeśli sukces!
    /// @}
};

/* ****************************************************************** */
/*         SYMSHELL2 EXAMPLES version 2006/2022/2026                  */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */

