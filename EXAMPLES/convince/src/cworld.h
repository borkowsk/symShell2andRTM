/// @file
/// DECLARATION OF W O R L D FOR "convince" SIMULATION.
/// @date 2026-05-07 (modified)
// ////////////////////////////////////////////////////////////////////

#include <climits>
#include "world.hpp"
#include "layer.hpp"
#include "cagent.h" //A definition of an agent is here.

/// //The Whole World of Simulations.
class aWorld:public symshell2::world
//--------------------------------------------------
{
    // Single-value parameters:
    // ////////////////////////

    size_t				MyWidth;	//!< Circumference of a torus

    wb_pchar			MappName;	//!< initialization bitmap file name (powers)
    wb_pchar			MaplName;	//!< initialization bitmap file name (attitudes)
    wb_pchar			MaskName;	//!< initialization bitmap file name (mask of habitable areas)

    // Simulation layers (only one this time):
    // ///////////////////////////////////////

    symshell2::rectangle_layer_of_ptr_to_agents<anAgent>	Agenci;	//!< The agent layer. It is a torus.

    // Main data series - it's more convenient to have pointers than to search from `Sources` by name:
    // ///////////////////////////////////////////////////////////////////////////////////////////////

    sym2::data::ptr_to_struct_matrix_source<anAgent,short>		*Firsts;	//!< '=Agenci.make_source("First mem",&anAgent::First);'
    sym2::data::ptr_to_struct_matrix_source<anAgent,short>		*Seconds;	//!< '=Agenci.make_source("Second mem",&anAgent::Second);'
    sym2::data::ptr_to_struct_matrix_source<anAgent,short>		*Powers;	//!< '=Agenci.make_source("Power",&anAgent::Power);'

public:
    // CONSTRUCTION AND DESTRUCTION:
    // /////////////////////////////
    explicit aWorld(
            unsigned		iWidth,			//!< Width of the torus of the agent matrix.
            double	 iToBeNewProb=0.1,		//!< Likelihood of a loner spontaneously changing his views on a new type of entertainment.
            double	 iReverseProb=0.9,		//!< Probability of reversal of views to 0 (means no idea for entertainment again).
            double	iSupportLevel=0.5,		//!< The power of support when the agent has some company for a given game.
            const char*	 iLog_name="convince.log",	//!< File name for saving history.
            const char*	iMapL_name=NULL,	//!< The name of the "components" initialization map file.
            const char*	iMapP_name=NULL,	//!< The filename of the "forces" initialization map.
            const char*	iLive_mask=NULL,	//!< The name of the habitable areas mask file. The black areas in this map are forbidden areas.
            short	 iMax_strength=100,		//!< Maximum agent strength.
            short	 iMin_strength=100		//!< Minimum agent strength.
            );

    ~aWorld() override {}

protected:
    // TYPICAL ACTIONS:
    // /////////////////
    void	initialize_layers() override;		//!< Prepares the starting state of the simulation.
    void	after_read_from_image() override;	//!< Actions after read state from map files.
    void	simulate_one_step() override;		//!< Implementation of a single simulation step.

    // Collaboration with display manager and data manager:
    //-----------------------------------------------------
    void	make_default_visualisation() override;	//!< Creates default display areas and places them.
    //void actualize_out_area() override;			//!< Updates OutArea contents every `n` simulation steps.
    void	make_basic_sources() override;			//!< Generates basic data sources for the built-in data manager.

    // I/O implementation:
    //--------------------
    int		implement_output(ostream& o) const override;	//!< Serialization. @returns 1 when success!
    int		implement_input(istream& i) override;			//!< Deserialization. @returns 1 when success!
};

/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */

