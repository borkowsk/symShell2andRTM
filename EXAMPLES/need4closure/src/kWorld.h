/// @file
/// @brief
///     @EN{ DECLARATION OF THE WORLD FOR "need 4 closure" SIMULATION. }
///     @PL{ DEKLARACJA ŚWIATA DLA SYMULACJI „need 4 closure”. }
/// @date 2026-05-21 (modified)
/// ====================================================================================================================
/// @details
///     (old example for SymShell implementing the Kruglanski's like model)
//
#pragma once
#include <climits> //SHRT_MAX
#include <rectgeom.hpp>
#include "world.hpp"
#include "layer.hpp"
#include "kAgent.h" //Definicja agenta

/// @brief
///     @PL{ Cały świat symulacji "need for closure". }
///     @EN{ A whole world of "need for closure" simulations. }
class kWorld: public sym2::shell::world
//--------------------------------------------------
{
    /// @name @EN{ Single-value model parameters: } @PL{ .... }
    // ////////////////////////////////////////////////////////////
    /// @{
    size_t			MyWidth;		///< Obwód torusa.
    short			MaxStrength;	///< Maksymalna siła agenta.
    short			Threshold;		///< Próg siły, powyżej którego nie ma zmian.
    short			NeighDens;		///< 8 == Gęstość sąsiedztwa.
    //short			NeighRadius;		///< Rozmiar sąsiedztwa. (NIEUŻYWANE?)
    double			WeightOfSelf;	///< Z jaka waga brać siebie pod uwagę (0..1).
    double			NeedForClosure;	///< Znaczenie może byc różne, zależnie od implementacji.
    double			Noise;			///< Szum informacyjny.
    double			Fill;			///< Udział żywych na początku.
    double			MigrationP;		///< Prawdopodobieństwo migracji (NIEUŻYWANE).
    bool			Synchronic;		///< Synchroniczna zmiana poglądów.
    wb_pchar		MappName;		///< Nazwa pliku inicjującej bitmapy.
    wb_pchar		MapLName;		///< Nazwa pliku inicjującej bitmapy.
    wb_pchar		MaskName;		///< Nazwa pliku inicjującego maskę zdatności (?).
    /// @}

    /// @name @EN{ Simulation layers (both are tori): } @PL{ Warstwy symulacji (są torusami): }
    // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @{
    //rectangle_unilayer<unsigned char> zdatność; //Warstwa definiująca zdatność do zasiedlenia
    sym2::shell::rectangle_layer_of_ptr_to_agents<kAgent> Agenci;		///< Właściwa warstwa agentów zasiedlających.
    /// @}

    /// @name @EN{ Main data series. It's convenient to have pointers rather than searching for them in Sources by name:}
    ///       @PL{ ...... }
    // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @{
    sym2::data::ptr_to_struct_matrix_source<kAgent,short>		*Firsts;	///< `=Agents.make_source("First mem",&kAgent::First);`
    sym2::data::ptr_to_struct_matrix_source<kAgent,short>		*Seconds;	///< `=Agents.make_source("Second mem",&kAgent::Second);`

    sym2::data::ptr_to_struct_matrix_source<kAgent,short>		*Powers;	///< `=Agents.make_source("Power",&kAgent::Power);`
    sym2::data::ptr_to_struct_matrix_source<kAgent,unsigned>    *ForLeft;	///< `=Agents.make_source("Power",&kAgent::ForLeft);`
    sym2::data::ptr_to_struct_matrix_source<kAgent,unsigned>    *ForRight;	///< `=Agents.make_source("Power",&kAgent::ForRight);`

    //ptr_to_struct_matrix_source<kAgent,short>		*Pressure;	///<  =Agents.make_source("Pressure",&kAgent::Press);
    //method_by_ptr_matrix_source<kAgent,long>		*Classify;	///< =Agents.make_source("Classification",&kAgent::classif);

    sym2::data::scalar_source<double>*       ptrStres;	///< Do przekazywania aktualnie najważniejszych danych na okno statusu.
    sym2::data::scalar_source<double>*       ptrClsSize;

    sym2::data::ptr_to_scalar_source<int>*       ptrLastChanged;	///< Do przekazywania liczników zmian.
    sym2::data::ptr_to_scalar_source<int>*       ptrLastMigration;	///< Do przekazywania liczników migracji.
    /// @}

    /// @name @PL{ DORAŹNE STATYSTYKI. } @EN{ .... }
    //---------------------------------------------------------------------------------------------
    /// @{
    double MaxPressure; 	///< Do zapamiętania teoretycznie największej wartości "presji".
    int    CountCh;			///< Ilu ostatnio zmieniło pogląd. Do celów statystyki.
    int    CountMig;		///< Ilu ostatnio migrowało. Do celów statystyki, o ile jest zaimplementowana migracja.
    /// @}

    /// @name @EN{ Here is the actual simulation implementation: } @PL{ .....}
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @{
    /// Zmiana stanów.
    int CheckChange(const sym2::shell::rectangle_geometry* MyGeom, size_t index, kAgent& CenterAgent);

    /// Ewentualna migracja.
    long DoMigration(const sym2::data::rectangle_geometry *MyGeom, size_t index, kAgent& CenterAgent);
    /// @}
public:
    // CONSTRUCTION AND DESTRUCTION:
    // /////////////////////////////

    /// @PL{ Konstruktor. } @EN{ Constructor. }
    kWorld(
            size_t	Width,			///< Szerokość torusa macierzy agentów.
            char*	log_name,		///< Nazwa pliku do zapisywania historii.
            char*	map_l_name,		///< Nazwa (bit-) mapy inicjującej "składowe".
            char*	mapp_name,		///< Nazwa (bit-) mapy inicjującej "siły".
            char*	live_mask,		///< Czarne w tej mapie są kasowane.
            double	noise,			///< Szum informacyjny.
            short	max_str,		///< Maksymalna sila agenta.
            short	how_many_neigh,	///< 8 == Gęstość sąsiedztwa.
            double	need_use_self,	///< Z jaką wagą ma brać siebie (0,1,2...?).
            double	need_for_something,	///< Z jaka wagą brać innych.
            bool	synchronously,
            short	threshold,
            double	spontaneously,
            double	fill,
            double	migration_prob,
            double	majority,
            double	minority
          );

    /// @PL{ Destruktor ukrywający to, co naprawdę robi. } @EN{ .... }
    ~kWorld() override = default;

protected:
    /// @name @EN{ TYPICAL ACTIONS OF EACH SIMULATION: } @PL{ .... }
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @{
    void	initialize_layers() override;	///< Stan startowy symulacji.
    void	after_read_from_image() override;	///< @brief @EN{ Actions after read state from a file.} @PL{ TAKŻE aktualizacja pól static kAgent-a! }
    void	simulate_one_step() override;	///< Właściwa implementacja kroku symulacji.
    /// @}

    /// @name @EN{ Collaboration with the managers: } @PL{ Współpraca z oboma zarządcami: }
    //-------------------------------------------------------------------------------------
    /// @{

    /// Tworzy domyślne "lufciki" i umieszcza na liście zarządcy.
    void	make_default_visualisation() override;
    /// Wypisywanie/dopisywanie na konsole statusu. Aktualizacja zawartości `OutArea` zwykle co `N` kroków symulacji.
    void	actualize_out_area() override;

    /// Generuje podstawowe źródła dla wbudowanego manager-a danych lub innego
    void	make_basic_sources() override;
    /// @}

    /// @name @EN{ Virtual input/output implementation: } @PL{ Implementacja i/o wirtualnego: }
    //-----------------------------------------------------------------------------------------
    /// @{
    int		implement_output(ostream& o) const override; ///< @returns 1, gdy sukces!
    int		implement_input(istream& i) override; ///< @returns 1, gdy sukces!
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


