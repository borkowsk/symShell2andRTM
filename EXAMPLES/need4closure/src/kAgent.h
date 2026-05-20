/// @file
/// @brief
/// @EN{ DECLARATION OF THE AGENT FOR "need 4 closure" SIMULATION. }
/// @PL{  }
/// @date 2026-05-20 (modified)
/// =========================================================
/// @details(old example for SymShell implementing a Kruglanski's like model)
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

class kagent:public sym2::shell::agent_base
{
    friend class kWorld;		///< To simplify access to the components of the world class.

    // STATIC CLASS MEMBERS - INITIALIZATION AND AGENT CHANGE PARAMETERS:
    // //////////////////////////////////////////////////////////////////
    static short Max_power;		///< Maksymalna siła agenta.
    static int   Threshold;		///< Granica domknięcia poglądu.
    //static short ile_kate;		///< Liczba kategorii w mapach.
    //static short kate_shift;		///< Przesuniecie dla wczytywania gifa.
    static double Majority;		///< Udział w całości przekonanych do większej klasy.
    static double Minority;		///< Udział w całości przekonanych do mniejszej klasy.
    static double NoiseLevel;	///< Prawd. spontanicznej zmiany (niezaimplementowane chyba).

    static short DrawAttitude();	///< Funkcja do losowania przekonania. Używa `Majority` i `Minority`.

    // AGENT ATTRIBUTES IMPORTANT FOR SIMULATION:
    short Power;			///< Siła agenta.
    short First;			///< Aktualne przekonanie -1,0,1 (Left,Neutral,Right).
    short Second;			///< Nowe przekonanie lub poprzednie.

    unsigned int   ForRight;
    unsigned int   ForLeft;

    //short Pressure;		///< Social pressure. The aggregate force behind a prevailing view, unless the agent holds it, or 0.
    bool  DurCh:1;		///< Information whether the state is being changed (for managing state changes).
    
    void _clean()		///< Implementacja czyszczenia stanu agenta.
    {
        Power=-1;

        First=0;Second=0;

        ForRight=0;ForLeft=0;
        //Press=0;
        DurCh=false;
    }

    // EVERYTHING THAT MUST BE DEFINED:
    // ////////////////////////////////
public:
    int IsOK() const
    {
        return Power!=-1;
    }

    kagent(const kagent& ini);			///< Konstruktor typowy.

    explicit kagent(const kagent* ini);	///< Konstruktor nietypowy.

    kagent();							///< Konstruktor bezparametrowy.

    kagent* clone() const				///< Dynamiczna kopia agenta.
    { return new kagent(*this);}

    ~kagent() override					///< Wirtualny destruktor.
    {_clean();}

    void clean() override				///< Wirtualne czyszczenia stanu agenta.
    {_clean();}

    void new_attitude(short a)
    {
        Second=a; //Takie ma być nowe przekonanie
        DurCh=true; //Sygnał, że juź jest "w trakcie" zmiany. Np. żeby zapobiec powtórce.
    }

    void update() 			///< Kontrola zmiany stanu
    {                                                                     assert(DurCh); //Powinien być w trakcie zmiany
        wb_swap(First,Second);
        DurCh=false; //Teraz jest już zmieniony
    }

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
        return (unsigned long) ( (unsigned char) (First) ); //TODO Jakaś dziwna kombinacja...
    }

    friend /// Zapisywanie. Serializacja.
    ostream& operator << (ostream& o,const kagent& a)
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

    friend /// Wczytywanie. Deserializacja.
    istream& operator >> (istream& i,kagent& a)
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

};

// Implementation of agent construction:
// /////////////////////////////////////

inline kagent::kagent(const kagent& ini)
    {
        DurCh= false;
        First=ini.First;
        Second=ini.Second;
        Power=1+RANDOM(Max_power); //Siła jest przydzielana z rozkładu
        ForLeft=RANDOM(Threshold); //StepCounter przekonań za "Lewymi"
        ForRight=RANDOM(Threshold); //StepCounter przekonań za "Prawymi"
    }

inline kagent::kagent(const kagent *ini)
{
    if(ini!=nullptr)
    {
        DurCh= false;
        First=ini->First;
        Second=ini->Second;
        Power=1+RANDOM(Max_power); //Siła jest przydzielana z rozkładu
        ForLeft=RANDOM(Threshold); //StepCounter przekonań za "Lewymi"
        ForRight=RANDOM(Threshold); //StepCounter przekonań za "Prawymi"
    }
    else
        _clean();
}

inline kagent::kagent()
    {
        _clean();
        Power=1+RANDOM(Max_power);
        ForLeft=RANDOM(Threshold); //StepCounter przekonań za "Lewymi"
        ForRight=RANDOM(Threshold); //StepCounter przekonań za "Prawymi"
        First=DrawAttitude();
        Second=First;
        DurCh= false;
    }

inline short kagent::DrawAttitude()
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



