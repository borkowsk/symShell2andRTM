/// @file
/// @brief
///  @EN{ DECLARATION OF 'convAgent' FOR "Convince" SIMULATION. }
///  @PL{ DEKLARACJA 'convAgent' DLA SYMULACJI "Convince". }
/// @date 2026-05-29 (modified)
///       =========================================================
/// @details
///  @EN{ A fairly simple simulation of a change of opinion implementing Professor D. Stauffer's model. }
///  @PL{ Dość prosta symulacja zmiany opinii wykorzystująca model profesora D. Stauffer-a. }
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
                            //!<        @PL{ Aby uprościć dostęp do komponentów agenta od strony wszechświata. }

    /// @name @EN{ STATIC COMPONENTS - MAINLY AGENT INITIALISATION PARAMETERS: }
    ///       @PL{ KOMPONENTY STATYCZNE - GŁÓWNIE PARAMETRY INICJALIZACJI AGENTÓW: }
    // /////////////////////////////////////////////////////////////////////////////
    /// @{

    static short	MinStrength;	//!< @brief @EN{ Minimum agent strength after initialization. } @PL{ Minimalna siła agenta po inicjalizacji. }
    static short	MaxStrength;	//!< @brief @EN{ Maximum agent strength after initialization. } @PL{ Maksymalna siła agenta po inicjalizacji. }
    static short	NumOfCate;		//!< @brief @EN{ Number of possible categories (states/beliefs?). } @PL{ Liczba możliwych kategorii (stanów/przekonań?). }
    static short	CateShift;		//!< @brief @EN{ Bit shift when loading category/state from a bitmap file. } @PL{ Przesunięcie bitowe podczas ładowania kategorii/stanu z pliku bitmapowego. }

    static double	ToBeNewProb;	//!< @brief @EN{ How often a loner spontaneously changing his views on a new type of entertainment. }
                                    //!<        @PL{ ... }
    static double	NewInfectProb;	//!< @brief @EN{ Probability of infection based on a view from a pair of infected individuals. }
                                    //!< @PL{ ... }
    static double	ReverseProb;	//!< @brief @EN{ Probability of reversal of views to 0, i.e., again lack of ideas for entertainment. }
                                    //!< @PL{ ... }
    static double	SupportLevel;	//!< @brief @EN{ The power of support when you have some friends with the same belief. }
                                    //!< @PL{ Jaka jest siła wsparcia, gdy masz przyjaciół, którzy podzielają twoje przekonania. }
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

    explicit convAgent(const convAgent* ini);	///< @brief @EN{ Cloning constructor. Concrete implementation for by the world class! }
                                                ///<        @PL{ Konstruktor klonujący. Implementacja przy źródłach klasy word. }
    convAgent(const convAgent& ini);	///< @brief @EN{ Copy constructor. Concrete implementation for by the world class! }
                                        ///<        @PL{ Konstruktor kopiujący. Implementacja przy źródłach klasy word. }
    convAgent();						///< @brief @EN{ Default constructor.Concrete implementation for by the world class! }
                                        ///<        @PL{ Konstruktor domyślny. Implementacja przy źródłach klasy word. }

    convAgent*	clone() const			///< @brief @EN{ Creating a dynamic copy on the heap. } @PL{ Tworzenie dynamicznej kopii na stercie. }
    { return new convAgent(this);}

    ~convAgent() override				///< @brief @EN{ Virtual destructor. } @PL{ Wirtualny destruktor. }
    {_clean();}

    void	clean() override			///< @brief @EN{ Virtual cleaning of agent attributes. } @PL{ Wirtualne czyszczenie atrybutów agenta. }
    {_clean();}

    void save_state() 					///< @brief @EN{ Remembering the First state to Second. For dynamic statistics. } @PL{ Zapamiętanie stanu First na Second. Do statystyk dynamiki. }
    { Second=First; }


    /// @brief @PL{Przydział do kategorii.} @EN{Assignment to a category.}
    unsigned long classify() override { return First; }

    /// @brief @EN{ Determines beliefs based on a bitmap pixel. } @PL{ Określa przekonania na podstawie piksela mapy bitowej. }
    void	assign123(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        First=short( Red >> CateShift );
        Second=short( Blue >> CateShift );
    }

    /// @brief @EN{ Determines the strength based on a bitmap pixel. } @PL{ Określa siłę na podstawie piksela bitmapowego. }
    void	assignPow(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        Power=short((int(Red)+int(Green)+int(Blue)) / (3.*255) * MaxStrength);
    }

    /// @brief @EN{ Resets beliefs based on pixel color (when black). } @PL{ Resetuje przekonania na podstawie koloru piksela (gdy jest czarny). }
    void	killBlack(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        if(Red==0 && Green==0 && Blue==0)
            _clean();
    }

    /// @brief @EN{ Specifies the display color of the agent based on its state. } @PL{ Określa kolor wyświetlania agenta na podstawie jego stanu. }
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



