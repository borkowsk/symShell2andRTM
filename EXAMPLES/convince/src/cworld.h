/// @file
/// @brief
///  @EN{ DECLARATION OF 'convWorld' FOR "Convince" SIMULATION. }
///  @PL{ DEKLARACJA 'convWorld' DLA SYMULACJI "Convince". }
/// @date 2026-05-29 (modified)
///       =========================================================
/// @details
///  @EN{ A fairly simple simulation of a change of opinion implementing Professor D. Stauffer's model. }
///  @PL{ Dość prosta symulacja zmiany opinii wykorzystująca model profesora D. Stauffer-a. }
//======================================================================================================================

#include <climits>
#include "world.hpp"
#include "layer.hpp"
#include "cagent.h" //A definition of an agent is here.

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nullptr"

using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

/// @brief
///     @EN{ The Whole World of "Convince" Simulations. }
///     @PL{ Cały świat symulacji "Convince". }
class convWorld: public sym2::shell::world
//--------------------------------------------------
{
    // Single-value parameters:
    // ////////////////////////

    size_t				MyWidth;	//!< @brief @EN{ Circumference of a torus. } @PL{ ... }

    wb_pchar			MappName;	//!< @brief @EN{ initialization bitmap file name (powers). } @PL{ ... }
    wb_pchar			MapLName;	//!< @brief @EN{ initialization bitmap file name (attitudes). } @PL{ ... }
    wb_pchar			MaskName;	//!< @brief @EN{ initialization bitmap file name (mask of habitable areas). } @PL{ ... }

    // Simulation layer(s):
    // ////////////////////

    rectangle_layer_of_ptr_to_agents<convAgent>	Agents;	//!< @brief @EN{ The agent layer. It is a torus. } @PL{ ... }

    /// @name @EN{ Main data series - it's more convenient to have pointers than to search from `Sources` by name: }
    ///       @PL{ ... }
    // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @{
    sym2::data::ptr_to_struct_matrix_source<convAgent,short>		*Firsts;	//!< '=Agents.make_source("First mem",&convAgent::First);'
    sym2::data::ptr_to_struct_matrix_source<convAgent,short>		*Seconds;	//!< '=Agents.make_source("Second mem",&convAgent::Second);'
    sym2::data::ptr_to_struct_matrix_source<convAgent,short>		*Powers;	//!< '=Agents.make_source("Power",&convAgent::Power);'
    /// @}
public:
    // CONSTRUCTION AND DESTRUCTION:
    // /////////////////////////////
    /// @brief @EN{Constructor.  } @PL{ ... }
    explicit convWorld(
            unsigned		iWidth,			//!< @EN{ Width of the torus of the agent matrix. } @PL{ ... }
            double	 iToBeNewProb=0.1,		//!< @EN{ Likelihood of a loner spontaneously changing his views on a new type of entertainment. } @PL{ ... }
            double	 iReverseProb=0.9,		//!< @EN{ Probability of reversal of views to 0 (means no idea for entertainment again). } @PL{ ... }
            double	iSupportLevel=0.5,		//!< @EN{ The power of support when the agent has some company for a given game. } @PL{ ... }
            const char*	 iLog_name="convince.log",	//!< @EN{ File name for saving history. } @PL{ ... }
            const char*	iMapL_name=NULL,	//!< @EN{ The name of the "components" initialization map file. } @PL{ ... }
            const char*	iMapP_name=NULL,	//!< @EN{ The filename of the "forces" initialization map. } @PL{ ... }
            const char*	iLive_mask=NULL,	//!< @EN{ The name of the habitable areas mask file. The black areas in this map are forbidden areas. } @PL{ ... }
            short	 iMax_strength=100,		//!< @EN{ Maximum agent strength. } @PL{ ... }
            short	 iMin_strength=100		//!< @EN{ Minimum agent strength. } @PL{ ... }
            );

    /// @brief @EN{Not-so-trivial destructor. } @PL{ ... }
    ~convWorld() override = default;

protected:
    // TYPICAL ACTIONS:
    // /////////////////
    void	initialize_layers() override;		//!< @brief @EN{ Prepares the starting state of the simulation. } @PL{ ... }
    void	after_read_from_image() override;	//!< @brief @EN{ Actions after read state from map files. } @PL{ ... }
    void	simulate_one_step() override;		//!< @brief @EN{ Implementation of a single simulation step. } @PL{ ... }

    // Collaboration with display manager and data manager:
    //-----------------------------------------------------
    void	make_default_visualisation() override;	//!< @brief @EN{ Creates default display areas and places them. } @PL{ ... }
    //void actualize_out_area() override;			//!< @brief @EN{ Updates OutArea contents every `n` simulation steps. } @PL{ ... }
    void	make_basic_sources() override;			//!< @brief @EN{ Generates basic data sources for the built-in data manager. } @PL{ ... }

    // I/O implementation:
    //--------------------
    int		implement_output(ostream& o) const override;	//!< @brief @EN{ Serialization. @returns 1 when success! } @PL{ ... }
    int		implement_input(istream& i) override;			//!< @brief @EN{ Deserialization. @returns 1 when success! } @PL{ ... }
};

#pragma clang diagnostic pop
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



