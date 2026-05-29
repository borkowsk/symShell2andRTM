/// @file
/// @brief
///  @EN{ IMPLEMENTATION OF AGENT FOR "KattitudeS" SIMULATION. }
///  @PL{ IMPLEMENTACJA AGENTA DO SYMULACJI POSTAW "KattitudeS". }
/// @date 2026-05-29 (modified)
/// =========================================================
/// @details (attitudeS old example for SymShell)
// =====================================================================================================================

#include "layer.hpp"

using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

static inline void wb_swap(short& a,short& b)
{
    short c=a;
    a=b;
    b=c;
}

/// @brief
///     @EN{ AGENT FOR "KattitudeS" SIMULATION. }
///     @PL{ AGENT DO SYMULACJI POSTAW "KattitudeS". }
class ka_agent: public sym2::shell::agent_base
{
    friend class ka_world; ///< Na razie tak. Żeby uprościć dostęp do składowych klasy zaprzyjaźnionej.

    /// @name STATYCZNE SKŁADOWE - PARAMETRY INICJOWANIA AGENTÓW:
    //===========================================================
    /// @{
    static short  Power_change;			//!< Określa, czy sila się zmienia (rośnie) z wiekiem.
    static short  Max_power;			//!< Maksymalna siła agenta.
    static short  Kate_num;				//!< Liczba kategorii w mapach.
    static short  Kate_shift;			//!< Przesuniecie dla wczytywania gifa.
    static double Majority;				//!< Udział największej klasy w całości.
    static double MutationLevel;		//!< Prawd. spontanicznej zmiany poglądów (0..1).
    /// @}


    /// @name SKŁADOWE KAŻDEGO AGENTA, ISTOTNE DLA MODELU LUB TECHNICZNE:
    //===================================================================
    /// @{
    short Power;				//!< Siła agenta.
    short First;				//!< Aktualne przekonanie.
    short Second;				//!< Nowe przekonanie lub poprzednie.
    short Press;				//!< Nacisk społeczny — sumaryczna siła za zwyciężającym poglądem, o ile agent go nie wyznaje, albo 0.
    bool  DurCh:1;				//!< Czy jest w trakcie zmieniania (do zarządzania zmianami stanów).
    /// @}

    static short DrawAttitude();				//!< Funkcja do losowania poglądu (???).

    void _clean()								//!< Implementacja czyszczenia.
    {
        First=-1;
        Second=-1;
        Power=-1;
        Press=0;
        DurCh=false;
    }

    void MakeOlder()							//!< Siła jako wiek.
    {
        if(ka_agent::Power_change)
        {
            Power+=ka_agent::Power_change;
            Power%=ka_agent::Max_power; //Nigdy nie przekracza siły maksymalnej
        }
    }

    void new_attitude(short a)					//!< Śledzona zmiana poglądu.
    {
        Second=a;   //Takie ma być nowe przekonanie
        DurCh=true; //Sygnał, że już jest "w trakcie" zmiany. Np. żeby zapobiec powtórce
    }

    void update()								//!< Kontrola ostatecznej zmiany stanu.
    {
        assert(DurCh);  //Powinien być w trakcie zmiany
        wb_swap(First,Second);
        DurCh=false;    //Teraz jest już zmieniony
    }

public:
    /// @name TO CO MUSI być zdefiniowane:
    //====================================
    /// @{

    bool IsOK() override						//!< Czy z agentem wszystko OK?
    {
        return First!=-1 && Second!=-1 && Power!=-1;
    }

    ka_agent();									//!< Konstruktor kopiujący.
    ka_agent(const ka_agent& ini);				//!< Konstruktor kopiujący.
    explicit ka_agent(const ka_agent *ini);		//!< Konstruktor ze wskaźnika.

    ka_agent* clone() const						//!< Dynamiczna kopia na stercie.
    { return new ka_agent(*this);}

    ~ka_agent() override						//!< Wirtualny destruktor. Wywołuje `_clean`.
    {_clean();}

    void clean() override						//!< Wirtualne czyszczenie. Wywołuje `_clean`.
    {_clean();}

    void assign_curr(unsigned char Red,unsigned char /*Green*/,unsigned char Blue)
    {
        First=Red >> Kate_shift;
        Second=Blue >> Kate_shift;
    }

    void assign_prev(unsigned char Red,unsigned char /*Green*/,unsigned char Blue)
    {
        First=Red >> Kate_shift;
        Second=Blue >> Kate_shift;
    }

    void assignPow(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        Power=short((int(Red)+int(Green)+int(Blue)) / (3.*255) * Max_power);
    }

    void killBlack(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        if(Red==0 && Green==0 && Blue==0)
            _clean();
    }

    /// @brief @PL{Przydział do kategorii.} @EN{Assignment to a category.}
    unsigned long classify() override { return First; }

    long RGB() const
    {
        return ( (unsigned char) (First) );
    }
    /// @}

    /// @name I/O operator(y/s):
    //--------------------------
    /// @{
    friend
    ostream& operator << (ostream& o,const ka_agent& a);

    friend
    istream& operator >> (istream& i, ka_agent& a);
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



