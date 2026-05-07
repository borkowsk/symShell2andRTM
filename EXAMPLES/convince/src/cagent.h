/// @file
/// @brief DECLARATION OF A G E N T FOR this SIMULATION.
/// @date 2026-05-07 (modified)
// /////////////////////////////////////////////////////////////////////
//
#include "layer.hpp"

class anAgent:public symshell2::agent_base
{
    friend class aWorld;	//!< To simplify access to the agent's components from the universe side.

    // STATIC COMPONENTS - MAINLY AGENT INITIALISATION PARAMETERS:
    // ///////////////////////////////////////////////////////////

    static short	MinStrength;	//!< Minimum agent strength at initialization.
    static short	MaxStrength;	//!< Maximum agent strength at initialization.
    static short	NumOfCate;		//!< Number of categories (states/beliefs?)
    static short	CateShift;		//!< Bit shift when loading category/state from a GIF file.

    static double	ToBeNewProb;	//!< How often a loner spontaneously changing his views on a new type of entertainment.
    static double	NewInfectProb;	//!< Probability of infection based on a view from a pair of infected individuals.
    static double	ReverseProb;	//!< Probability of reversal of views to 0, i.e: again lack of ideas for entertainment.
    static double	SupportLevel;	//!< The power of support when you have some friends with the same belief.

    // INDIVIDUAL AGENT ATTRIBUTES:
    // ////////////////////////////

    short	Power;		//!< The agent's power of persuasion.
    short	First;		//!< First/previous belief.
    short	Second;		//!< New belief.

    /// Implementation of agent state clearing.
    void	_clean()
    {
        First=-1;
        Second=-1;
        Power=-1;
    }

    // WHAT MUST be defined for each agent type:
    // /////////////////////////////////////////
public:
    int	IsOK() const 		//!< Integrity test.
    {
        return First!=-1 && Second!=-1 && Power!=-1;
    }

    anAgent(const anAgent& ini);	//!< Copy constructor. Concrete implementation for by the world class!
    anAgent();						//!< Default constructor.Concrete implementation for by the world class!

    anAgent*	clone() const		//!< Creating a dynamic copy on the heap.
    { return new anAgent(*this);}

    ~anAgent() override				//!< Virtual destructor.
    {_clean();}

    void	clean() override		//!< Virtual cleaning of agent attributes.
    {_clean();}

    void	assign123(unsigned char Red,unsigned char Green,unsigned char Blue)	//!< Determines beliefs based on a bitmap pixel.
    {
        First=Red >> CateShift;
        Second=Blue >> CateShift;
    }

    void	assignPow(unsigned char Red,unsigned char Green,unsigned char Blue)	//!< Determines the strength based on a bitmap pixel.
    {
        Power=short((int(Red)+int(Green)+int(Blue)) / (3.*255) * MaxStrength);
    }

    void	killBlack(unsigned char Red,unsigned char Green,unsigned char Blue)	//!< Resets beliefs based on pixel color (when black).
    {
        if(Red==0 && Green==0 && Blue==0)
            _clean();
    }

    long	RGB() const		//!< Specifies the display color of the agent based on its status.
    {
        return (unsigned long) ( (unsigned char) (First) );
    }

    friend
    ostream&	operator << (ostream& o,const anAgent& a)	//!< Serialization.
    {
        o<<'{';
        o<<' '<<a.Power<<' '<<a.First<<' '<<a.Second<<' ';
        o<<'}';
        return o;
    }

    friend
    istream&	operator >> (istream& i,anAgent& a)		//!< Deserialization.
    {
        char pom;
        i>>pom;		//ignoring `{`
        i>>a.Power>>a.First>>a.Second;
        i>>pom;		//ignoring `}`
        return i;
    }

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


