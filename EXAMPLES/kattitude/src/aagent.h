/// @file
/// @brief DECLARATION OF AGENT FOR "attitudeS" SIMULATION (kattitude old example for SymShell)
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @date 2026-04-03 (modified)
//
#include "layer.hpp"

inline void wb_swap(short& a,short& b)
{
    short c=a;
    a=b;
    b=c;
}

class aagent:public agent_base
{
    friend class aworld; //Na razie tak. Żeby uprościć dostęp do składowych klasy zaprzyjaźnionej.

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

    // TO CO MUSI byc zdefiniowane:
    // ////////////////////////////
public:
    int IsOK() const
    {
        return First!=-1 && Second!=-1 && Power!=-1;
    }

    void MakeOlder()				//!< Siła jako wiek.
    {
        if(aagent::Power_change)
        {
            Power+=aagent::Power_change;
            Power%=aagent::Max_power; //Nigdy nie przekracza siły maksymalnej
        }
    }

    aagent();								//!< Konstruktor kopiujący.
    aagent(const aagent& ini);				//!< Konstruktor kopiujący.
    explicit aagent(const aagent *ini);		//!< Konstruktor ze wskaźnika.

    aagent* clone() const					//!< Dynamiczna kopia na stercie.
    { return new aagent(*this);}

    ~aagent() override						//!< Wirtualny destruktor. Wywołuje `_clean`.
    {_clean();}

    void clean() override					//!< Wirtualne czyszczenie. Wywołuje `_clean`.
    {_clean();}

    void new_attitude(short a)
    {
        Second=a; //Takie ma być nowe przekonanie
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

    [[maybe_unused]]
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
    ostream& operator << (ostream& o,const aagent& a)
    {
        o<<'{';
        o<<' '<<a.Power<<' '<<a.First<<' '<<a.Second<<' '<<a.Press<<' ';
        o<<'}';
        return o;
    }

    friend
    istream& operator >> (istream& i,aagent& a)
    {
        char pom;
        i>>pom;		//ignoruje {
        i>>a.Power>>a.First>>a.Second>>a.Press;
        i>>pom;		//ignoruje }
        return i;
    }

};


/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */



