/// @file
/// @brief
///  @EN{ DECLARATION OF 'convAgent' FOR "Convince" SIMULATION. }
///  @PL{ DEKLARACJA 'convAgent' DLA SYMULACJI "Convince". }
/// @date 2026-05-29 (modified)
///       =========================================================
/// @details ...
//======================================================================================================================

#include "layer.hpp"

using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

/// @brief
///     @PL{ Agent symulacji "Convince". }
///     @EN{ Simulation agent for "Convince". }
class convAgent: public sym2::shell::agent_base
{
    friend class convWorld;	//!< @brief @EN{ To simplify access to the agent's components from the universe side. }
                            //!< @PL{ ... }

    /// @name @EN{ STATIC COMPONENTS - MAINLY AGENT INITIALISATION PARAMETERS: }
    ///       @PL{ KOMPONENTY STATYCZNE - GŁÓWNIE PARAMETRY INICJALIZACJI AGENTÓW: }
    // /////////////////////////////////////////////////////////////////////////////
    /// @{

    static short	MinStrength;	//!< @brief @EN{ Minimum agent strength at initialization. } @PL{ ... }
    static short	MaxStrength;	//!< @brief @EN{ Maximum agent strength at initialization. } @PL{ ... }
    static short	NumOfCate;		//!< @brief @EN{ Number of categories (states/beliefs?). } @PL{ ... }
    static short	CateShift;		//!< @brief @EN{ Bit shift when loading category/state from a GIF file. } @PL{ ... }

    static double	ToBeNewProb;	//!< @brief @EN{ How often a loner spontaneously changing his views on a new type of entertainment. }
                                    //!< @PL{ ... }
    static double	NewInfectProb;	//!< @brief @EN{ Probability of infection based on a view from a pair of infected individuals. }
                                    //!< @PL{ ... }
    static double	ReverseProb;	//!< @brief @EN{ Probability of reversal of views to 0, i.e., again lack of ideas for entertainment. }
                                    //!< @PL{ ... }
    static double	SupportLevel;	//!< @brief @EN{ The power of support when you have some friends with the same belief. }
                                    //!< @PL{ ... }
    /// @}

    /// @name @EN{ INDIVIDUAL AGENT ATTRIBUTES: }
    ///       @PL{ INDYWIDUALNE WŁAŚCIWOŚCI AGENTA: }
    // //////////////////////////////////////////////
    /// @{
    short	Power;		//!< @brief @EN{ The agent's power of persuasion. } @PL{ ... }
    short	First;		//!< @brief @EN{ First/previous belief. } @PL{ ... }
    short	Second;		//!< @brief @EN{ New belief. } @PL{ ... }
    /// @}

    /// @brief @EN{ Implementation of agent state clearing. } @PL{ Implementacja czyszczenia agenta. }
    void	_clean()
    {
        First=-1;
        Second=-1;
        Power=-1;
    }

public:
    /// @name @EN{ WHAT MUST be defined for each agent type: }
    ///       @PL{ CO MUSI zostać zdefiniowane dla każdego typu agenta: }
    // //////////////////////////////////////////////////////////////////
    /// @{

    int	IsOK() const 					//!< @brief @EN{ Integrity test. } @PL{ Sprawdza, czy z agentem jest "wporzo". }
    {
        return First!=-1 && Second!=-1 && Power!=-1;
    }

    convAgent(const convAgent& ini);	///< @brief @EN{ Copy constructor. Concrete implementation for by the world class! }
                                        ///< @PL{ ... }
    convAgent();						///< @brief @EN{ Default constructor.Concrete implementation for by the world class! }
                                        ///< @PL{ ... }

    convAgent*	clone() const			///< @brief @EN{ Creating a dynamic copy on the heap. } @PL{ ... }
    { return new convAgent(*this);}

    ~convAgent() override				///< @brief @EN{ Virtual destructor. } @PL{ ... }
    {_clean();}

    void	clean() override			///< @brief @EN{ Virtual cleaning of agent attributes. } @PL{ ... }
    {_clean();}

    /// @brief @PL{Przydział do kategorii.} @EN{Assignment to a category.}
    unsigned long classify() override { return First; }

    /// @brief @EN{ Determines beliefs based on a bitmap pixel. } @PL{ ... }
    void	assign123(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        First=short( Red >> CateShift );
        Second=short( Blue >> CateShift );
    }

    /// @brief @EN{ Determines the strength based on a bitmap pixel. } @PL{ ... }
    void	assignPow(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        Power=short((int(Red)+int(Green)+int(Blue)) / (3.*255) * MaxStrength);
    }

    /// @brief @EN{ Resets beliefs based on pixel color (when black). } @PL{ ... }
    void	killBlack(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        if(Red==0 && Green==0 && Blue==0)
            _clean();
    }

    /// @brief @EN{ Specifies the display color of the agent based on its status. } @PL{ ... }
    long	RGB() const
    {
        return ( (unsigned char) (First) );
    }
    /// @}

    /// @name I/O operator(s/y).
    /// @{
    friend
    ostream&	operator << (ostream& o,const convAgent& a);

    friend
    istream&	operator >> (istream& i, convAgent& a);
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



