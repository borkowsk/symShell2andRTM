/// @file
/// @brief
///     @EN{ DECLARATION OF THE WORLD FOR "need 4 closure" SIMULATION. }
///     @PL{ DEKLARACJA ŚWIATA DLA SYMULACJI „need 4 closure”. }
/// @date 2026-05-29 (modified)
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
    size_t			MyWidth;		///< @brief @PL{ Obwód torusa świata. } @EN{  }
    short			MaxStrength;	///< @brief @PL{ Maksymalna siła agenta. } @EN{  }
    short			Threshold;		///< @brief @PL{ Próg siły, powyżej którego nie ma zmian. } @EN{  }
    short			NeighDens;		///< @brief @PL{ 8 == Gęstość sąsiedztwa. } @EN{  }
    double			WeightOfSelf;	///< @brief @PL{ Z jaka waga brać siebie pod uwagę (0..1). } @EN{  }
    double			NeedForClosure;	///< @brief @PL{ Znaczenie może byc różne, zależnie od implementacji. } @EN{  }
    double			Noise;			///< @brief @PL{ Szum informacyjny. } @EN{  }
    double			Fill;			///< @brief @PL{ Udział żywych na początku. } @EN{  }
    double			MigrationP;		///< @brief @PL{ Prawdopodobieństwo migracji (NIEUŻYWANE). } @EN{  }
    bool			Synchronic;		///< @brief @PL{ Synchroniczna zmiana poglądów. } @EN{  }
    wb_pchar		MappName;		///< @brief @PL{ Nazwa pliku inicjującej bitmapy. } @EN{  }
    wb_pchar		MapLName;		///< @brief @PL{ Nazwa pliku inicjującej bitmapy. } @EN{  }
    wb_pchar		MaskName;		///< @brief @PL{ Nazwa pliku inicjującego maskę zdatności (?). } @EN{  }
    /// @}

    /// @name @EN{ Simulation layers (both are tori): } @PL{ Warstwy symulacji (są torusami): }
    // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @{
    //rectangle_layer_of<unsigned char> zdatność; //Warstwa definiująca zdatność do zasiedlenia
    sym2::shell::rectangle_layer_of_ptr_to_agents<kAgent> Agents;	///< @brief @PL{ Właściwa warstwa agentów zasiedlających.}
                                                                    ///< @EN{  }
    /// @}

    /// @name @PL{ DORAŹNE STATYSTYKI. } @EN{ .... }
    //---------------------------------------------------------------------------------------------
    /// @{
    double MaxPressure; 	///< @brief @PL{ Do zapamiętania teoretycznie największej wartości "presji". } @EN{  }
    int    CountCh;			///< @brief @PL{ Ilu ostatnio zmieniło pogląd. Do celów statystyki. } @EN{  }
    int    CountMig;		///< @brief @PL{ Ilu ostatnio migrowało. Do celów statystyki, o ile jest zaimplementowana migracja. } @EN{  }
    /// @}

    /// @name @EN{ Main data series. @details It's convenient to have pointers rather than searching for them in Sources by name.}
    ///       @PL{ Główne serie danych. @details Wygodniej jest mieć wskaźniki niż szukać ich w źródłach po nazwie. }
    // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @{
    sym2::data::ptr_to_struct_matrix_source<kAgent,short>		*Firsts;	///< `=Agents.make_source("First mem",&kAgent::First);`
    sym2::data::ptr_to_struct_matrix_source<kAgent,short>		*Seconds;	///< `=Agents.make_source("Second mem",&kAgent::Second);`

    sym2::data::ptr_to_struct_matrix_source<kAgent,short>		*Powers;	///< `=Agents.make_source("Power",&kAgent::Power);`
    sym2::data::ptr_to_struct_matrix_source<kAgent,unsigned>    *ForLeft;	///< `=Agents.make_source("Power",&kAgent::ForLeft);`
    sym2::data::ptr_to_struct_matrix_source<kAgent,unsigned>    *ForRight;	///< `=Agents.make_source("Power",&kAgent::ForRight);`

    //ptr_to_struct_matrix_source<kAgent,short>		*Pressure;	///<  =Agents.make_source("Pressure",&kAgent::Press);
    //method_by_ptr_matrix_source<kAgent,long>		*Classify;	///< =Agents.make_source("Classification",&kAgent::classify);
    /// @}

    /// @name @PL{ Do przekazywania aktualnie najważniejszych danych na okno statusu. }
    ///       @EN{  }
    /// @{
    sym2::data::scalar_source<double>*           ptrStres;
    sym2::data::scalar_source<double>*           ptrClsSize;

    sym2::data::ptr_to_scalar_source<int>*       ptrLastChanged;	///< @brief @PL{ Do przekazywania liczników zmian. } @EN{  }
    sym2::data::ptr_to_scalar_source<int>*       ptrLastMigration;	///< @brief @PL{ Do przekazywania liczników migracji. } @EN{  }
    /// @}

    /// @name @EN{ Here is the actual simulation implementation: } @PL{ ..... }
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @{

    /// @brief @PL{ Zmiana stanów. } @EN{  }
    int CheckChange(const sym2::shell::rectangle_geometry* MyGeom, size_t index, kAgent& CenterAgent);

    /// @brief @PL{ Ewentualna migracja. } @EN{  }
    long DoMigration(const sym2::data::rectangle_geometry *MyGeom, size_t index, kAgent& CenterAgent);
    /// @}
public:
    // CONSTRUCTION AND DESTRUCTION:
    // /////////////////////////////

    /// @brief @PL{ Konstruktor. } @EN{ Constructor. }
    kWorld(
            size_t	Width,			///< @PL{ Szerokość torusa macierzy agentów. } @EN{  }
            char*	log_name,		///< @PL{ Nazwa pliku do zapisywania historii. } @EN{  }
            char*	map_l_name,		///< @PL{ Nazwa (bit-) mapy inicjującej "składowe". } @EN{  }
            char*	mapp_name,		///< @PL{ Nazwa (bit-) mapy inicjującej "siły". } @EN{  }
            char*	live_mask,		///< @PL{ Czarne w tej mapie są kasowane. } @EN{  }
            double	noise,			///< @PL{ Szum informacyjny. } @EN{  }
            short	max_str,		///< @PL{ Maksymalna sila agenta. } @EN{  }
            short	how_many_neigh,	///< @PL{ 8 == Gęstość sąsiedztwa. } @EN{  }
            double	need_use_self,	///< @PL{ Z jaką wagą ma brać siebie (0,1,2...?). } @EN{  }
            double	need_for_something,	///< @PL{ Z jaka wagą brać innych. } @EN{  }
            bool	synchronously,	///< @PL{ ... } @EN{ ... }
            short	threshold,		///< @PL{ ... } @EN{ ... }
            double	spontaneously,	///< @PL{ ... } @EN{ ... }
            double	fill,			///< @PL{ ... } @EN{ ... }
            double	migration_prob,	///< @PL{ ... } @EN{ ... }
            double	majority,		///< @PL{ ... } @EN{ ... }
            double	minority		///< @PL{ ... } @EN{ ... }
          );

    /// @PL{ Destruktor ukrywający to, co naprawdę robi. } @EN{ .... }
    ~kWorld() override = default;

protected:
    /// @name @EN{ TYPICAL ACTIONS OF EACH SIMULATION: } @PL{ .... }
    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @{
    void	initialize_layers() override;		///< @brief @PL{ Stan startowy symulacji.} @EN{ ... }
    void	after_read_from_image() override;	///< @brief @EN{ Actions after read state from a file.} @PL{ TAKŻE aktualizacja pól static kAgent-a! }
    void	simulate_one_step() override;		///< @brief @PL{ Właściwa implementacja kroku symulacji.} @EN{ ... }
    /// @}

    /// @name @EN{ Collaboration with the managers: } @PL{ Współpraca z oboma zarządcami: }
    //-------------------------------------------------------------------------------------
    /// @{

    /// @brief @PL{ Tworzy domyślne "lufciki" i umieszcza na liście zarządcy. } @EN{ ... }
    void	make_default_visualisation() override;
    /// @brief @PL{ Wypisywanie/dopisywanie na konsole statusu. Aktualizacja zawartości `OutArea` zwykle co `N` kroków symulacji. } @EN{ ... }
    void	actualize_out_area() override;

    /// @brief @PL{ Generuje podstawowe źródła dla wbudowanego manager-a danych lub innego} @EN{ ... }
    void	make_basic_sources() override;
    /// @}

    /// @name @EN{ Virtual input/output implementation: } @PL{ Implementacja i/o wirtualnego: }
    //-----------------------------------------------------------------------------------------
    /// @{
    int		implement_output(ostream& o) const override; ///< @returns 1, @PL{ gdy sukces. } @EN{ ... }
    int		implement_input(istream& i) override; ///< @returns 1, @PL{ gdy sukces. } @EN{ ... }
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


