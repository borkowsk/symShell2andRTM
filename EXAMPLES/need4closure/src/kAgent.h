/// @file
/// @brief
///  @EN{ DECLARATION OF THE AGENT FOR "need 4 closure" SIMULATION. }
///  @PL{ DEKLARACJA AGENTA W SYMULACJI „potrzeba zamknięcia”. }
/// @date 2026-05-29 (modified)
/// =========================================================
/// @details (old example for SymShell implementing a Kruglanski's like model)
// =====================================================================================================================

#pragma once
#include "layer.hpp"
#include "krand.h"

inline void wb_swap(short& a,short& b)
{
    short c=a;
    a=b;
    b=c;
}

/// @brief
///     @EN{ THE AGENT FOR "need 4 closure" SIMULATION. }
///     @PL{ AGENT DO SYMULACJI „potrzeba zamknięcia”. }
class kAgent: public sym2::shell::agent_base
{
    friend class kWorld;		///< @EN{ To simplify access to the components of the world class. }
                                ///< @PL{ Aby ułatwić dostęp do komponentów klasy świata.  }

    /// @name @EN{ STATIC CLASS MEMBERS - INITIALIZATION AND AGENT CHANGE PARAMETERS: }
    ///       @PL{ ATRYBUTY STATYCZNE KLASY – PARAMETRY INICJALIZACJI I ZMIANY AGENTA: }
    // /////////////////////////////////////////////////////////////////////////////////
    /// @{
    static short    MaxPower;		///< @brief @PL{ Maksymalna siła agenta. } @EN{  }
    static int     Threshold;		///< @brief @PL{ Granica domknięcia poglądu. } @EN{  }
    //static short    NofCat;		///< @brief @PL{ Liczba kategorii w mapach. } @EN{  }
    //static short CateShift;		///< @brief @PL{ Przesuniecie dla wczytywania gifa. } @EN{  }
    static double   Majority;		///< @brief @PL{ Udział w całości przekonanych do większej klasy. } @EN{  }
    static double   Minority;		///< @brief @PL{ Udział w całości przekonanych do mniejszej klasy. } @EN{  }
    static double NoiseLevel;		///< @brief @PL{ Prawd. spontanicznej zmiany (nieużywane tu raczej). } @EN{  }

    static short DrawAttitude();	///< @brief @PL{ Funkcja do losowania przekonania. Używa `Majority` i `Minority`. }
                                    ///<        @EN{  }
    /// @}

    /// @name @EN{ AGENT ATTRIBUTES IMPORTANT FOR SIMULATION: }
    //=========================================================
    /// @{
    short      Power;			///< @brief @PL{ Siła agenta. } @EN{  }
    short      First;			///< @brief @PL{ Aktualne przekonanie -1,0,1 (Left,Neutral,Right). } @EN{  }
    short      Second;			///< @brief @PL{ Nowe przekonanie lub poprzednie. } @EN{  }
    unsigned   ForRight;		///< @brief @PL{ Informacja "za prawicą". } @EN{  }
    unsigned   ForLeft;			///< @brief @PL{ Informacja "za lewicą". } @EN{  }
    //short Pressure;			///< Social pressure. The aggregate force behind a prevailing view, unless the agent holds it, or 0.
    bool       DurCh:1;			///< @brief @PL{ Informacja, czy stan jest zmieniany (w celu zarządzania zmianami stanu). }
                                ///< @EN{ Information whether the state is being changed (for managing state changes). }
    /// @}

    void _clean()				///< @brief @PL{ Implementacja czyszczenia stanu agenta. } @EN{  }
    {
        Power=-1;

        First=0;Second=0;

        ForRight=0;ForLeft=0;
        //Pressure=0;
        DurCh=false;
    }

    void new_attitude(short a)			///< @brief @PL{ Propozycja zmiany stanu. } @EN{  }
    {
        Second=a; //Takie ma być nowe przekonanie
        DurCh=true; //Sygnał, że juź jest "w trakcie" zmiany. Np. żeby zapobiec powtórce.
    }

    void update() 						///< @brief @PL{ Wprowadzenie zmiany stanu. } @EN{  }
    {                                                                     assert(DurCh); //Powinien być w trakcie zmiany
        wb_swap(First,Second);
        DurCh=false; //Teraz jest już zmieniony, więc nie jest w trakcie zmiany.
    }

public:
    /// @name @EN{ EVERYTHING THAT MUST BE DEFINED: }
    ///       @PL{ WSZYSTKO CO NAJWAŻNIEJSZE DO ZDEFINIOWANIA: }
    // /////////////////////////////////////////////////////////
    /// @{

    bool IsOK() override 				///< @brief @PL{ Informuje czy wszystko formalnie OK z agentem. } @EN{  }
    {
        return Power!=-1;
    }

    kAgent(const kAgent& ini);			///< @brief @PL{ Konstruktor typowy, kopiujący. } @EN{  }

    explicit kAgent(const kAgent* ini);	///< @brief @PL{ Konstruktor nietypowy, klonujący. } @EN{  }

    kAgent();							///< @brief @PL{ Konstruktor bezparametrowy. } @EN{  }

    kAgent* clone() const				///< @brief @PL{ Dynamiczna kopia agenta. } @EN{  }
    { return new kAgent(*this); }

    ~kAgent() override					///< @brief @PL{ Wirtualny destruktor. } @EN{  }
    {_clean();}

    void clean() override				///< @brief @PL{ Wirtualne czyszczenia stanu agenta. } @EN{  }
    {_clean();}

    void assign_curr(unsigned char Red,unsigned char /*Green*/,unsigned char Blue)
    {
        ForLeft=Red;
        ForRight=Blue;
        First=0; //Na razie bez zdecydowania
    }

    void assign_prev(unsigned char /*Red*/,unsigned char /*Green*/,unsigned char /*Blue*/)
    {
        Second=0;
    }

    void assignPow(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        Power=short((int(Red)+int(Green)+int(Blue)) / (3.*255) * MaxPower);
    }

    void killBlack(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        if(Red==0 && Green==0 && Blue==0)
            _clean();
    }

    unsigned long classify() override
    {
        return First;
    }

    long RGB() const
    {
        return ( (unsigned char) (First) );
    }
    /// @}

    /// @name I/O operator(y/s)
    /// @{
    friend /// Zapisywanie. Serializacja.
    ostream& operator << (ostream& o,const kAgent& a);

    friend /// Wczytywanie. Deserializacja.
    istream& operator >> (istream& i, kAgent& a);
    /// @}
};

// Implementation of agent construction:
// /////////////////////////////////////

inline kAgent::kAgent(const kAgent& ini)
{
    DurCh= false;
    First=ini.First;
    Second=ini.Second;
    Power=short(1+RANDOM(MaxPower)); //Siła jest przydzielana z rozkładu
    ForLeft=RANDOM(Threshold); //Licznik przekonań za "Lewymi"
    ForRight=RANDOM(Threshold); //Licznik przekonań za "Prawymi"
}

inline kAgent::kAgent(const kAgent *ini)
{
    if(ini!=nullptr)
    {
        DurCh= false;
        First=ini->First;
        Second=ini->Second;
        Power=short(1+RANDOM(MaxPower)); //Siła jest przydzielana z rozkładu
        ForLeft=RANDOM(Threshold); //Licznik przekonań za "Lewymi"
        ForRight=RANDOM(Threshold); //Licznik przekonań za "Prawymi"
    }
    else
        _clean();
}

inline kAgent::kAgent()
{
    _clean();
    Power=short(1+RANDOM(MaxPower));
    ForLeft=RANDOM(Threshold); //Licznik przekonań za "Lewymi"
    ForRight=RANDOM(Threshold); //Licznik przekonań za "Prawymi"
    First=DrawAttitude();
    Second=First;
    DurCh= false;
}

inline short kAgent::DrawAttitude()
{
    assert(Majority+Minority<=1);

    double pom=DRAND();
    if(pom<Majority)
    {
        return -1;  //Czarni = Lewi
    }
    else
        if(pom<Majority+Minority)
        {
            return 1; //Biali = Prawi
        }
        else
            return 0; //Niezdecydowani

}

inline istream &operator>>(istream &i, kAgent &a)
{
    char pom;
    i>>pom;		//ignoruje {
    i   >>a.Power
        >>a.First
        >>a.Second
        //>>a.Press
        >>a.ForRight
        >>a.ForLeft
            ;
    i>>pom;		//ignoruje }
    return i;
}

inline ostream &operator<<(ostream &o, const kAgent &a)
{
    o<<'{';
    o<<' '<<a.Power<<' '
     <<a.First<<' '
     <<a.Second<<' '
     //  <<a.Press<<' '
     <<a.ForRight<<' '
     <<a.ForLeft<<' '
            ;
    o<<'}';
    return o;
}

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



