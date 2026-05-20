/// @file
/// @brief
///  @EN{ IMPLEMENTATION OF AGENT FOR "attitudeS" SIMULATION. }
///  @PL{ WDROŻENIE AGENTA DO SYMULACJI POSTAW "attitudeS". }
/// @date 2026-05-21 (modified)
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
///     @EN{ AGENT FOR "attitudeS" SIMULATION. }
///     @PL{ AGENT DO SYMULACJI POSTAW "attitudeS". }
class ka_agent: public sym2::shell::agent_base
{
    friend class ka_world; ///< Na razie tak. Żeby uprościć dostęp do składowych klasy zaprzyjaźnionej.

    // STATYCZNE SKŁADOWE - PARAMETRY INICJOWANIA AGENTÓW:
    static short  Power_change;			//!< Określa, czy sila się zmienia (rośnie) z wiekiem.
    static short  Max_power;			//!< Maksymalna siła agenta.
    static short  Kate_num;				//!< Liczba kategorii w mapach.
    static short  Kate_shift;			//!< Przesuniecie dla wczytywania gifa.
    static double Majority;				//!< Udział największej klasy w całości.
    static double MutationLevel;		//!< Prawd. spontanicznej zmiany poglądów (0..1).

    static short DrawAttitude();		//!< Funkcja do losowania poglądu (???).

    // SKŁADOWE DLA SYMULACJI:
    short Power;				//!< Siła agenta.
    short First;				//!< Aktualne przekonanie.
    short Second;				//!< Nowe przekonanie lub poprzednie.
    short Press;				//!< Nacisk społeczny — sumaryczna siła za zwyciężającym poglądem, o ile agent go nie wyznaje, albo 0.
    bool  DurCh:1;				//!< Czy jest w trakcie zmieniania (do zarządzania zmianami stanów).
    
    void _clean()				//!< Implementacja czyszczenia.
    {
        First=-1;
        Second=-1;
        Power=-1;
        Press=0;
        DurCh=false;
    }

    // TO CO MUSI być zdefiniowane:
    // ////////////////////////////
public:
    bool IsOK() override
    {
        return First!=-1 && Second!=-1 && Power!=-1;
    }

    void MakeOlder()				//!< Siła jako wiek.
    {
        if(ka_agent::Power_change)
        {
            Power+=ka_agent::Power_change;
            Power%=ka_agent::Max_power; //Nigdy nie przekracza siły maksymalnej
        }
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

    void new_attitude(short a)
    {
        Second=a;   //Takie ma być nowe przekonanie
        DurCh=true; //Sygnał, że już jest "w trakcie" zmiany. Np. żeby zapobiec powtórce
    }

    void update()				//!< Kontrola zmiany stanu.
    {
        assert(DurCh);  //Powinien być w trakcie zmiany
        wb_swap(First,Second);
        DurCh=false;    //Teraz jest już zmieniony
    }

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

    [[maybe_unused]]
    long classif() const
    {
        return First;
    }

    long RGB() const
    {
        return (unsigned long) ( (unsigned char) (First) );
    }

    friend
    ostream& operator << (ostream& o,const ka_agent& a)
    {
        o<<'{';
        o<<' '<<a.Power<<' '<<a.First<<' '<<a.Second<<' '<<a.Press<<' ';
        o<<'}';
        return o;
    }

    friend
    istream& operator >> (istream& i, ka_agent& a)
    {
        char pom;
        i>>pom;		//ignoruje {
        i>>a.Power>>a.First>>a.Second>>a.Press;
        i>>pom;		//ignoruje }
        return i;
    }

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



