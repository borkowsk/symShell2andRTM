/// @file
/// DECLARATION OF    A G E N T   FOR "life" SIMULATION.
//---------------------------------------------------------------
/// Its is rather simply implementation. You can replace it if you need.
// ////////////////////////////////////////////////////
/// @date 2026-03-27 (modified)

#include "layer.hpp"

class lifeagent:public agent_base
{
    friend class lifeworld; //Na razie tak, żeby uprościć dostęp do składowych.

    // STATYCZNE SKŁADOWE - PARAMETRY INICJOWANIA AGENTÓW:
    static short  ile_kate;   //Liczba kategorii. Dla Life zawsze 2!!! WIĘC TO POLE NIEUŻYWANE — TYLKO DLA INFORMACJI.
    static short  kate_shift; //Do wczytywania
    static double MutationLevel; //Prawd. spontanicznej zmiany stanu (0..1) - chyba nieużywane
    static double InitProp; //Proporcje inicjowania losowego

    // SKŁADOWE DLA SYMULACJI:
    short First;		//Pierwsze stan
    short Second;		//Nowy/stary stan


    void _clean()
    {
        First=0;
        Second=0;
    }

    // TO CO MUSI byc zdefiniowane:
    // ////////////////////////////
    public:
    int IsOK()
    {
        return 1;
    }

    int is_alive()
    {
        return 1; //Chyba tak...
    }

    lifeagent(const lifeagent& ini); //!< Konkretna implementacja tego konstruktora w "lworld.cpp".

    lifeagent(); //!< Konkretna implementacja tego konstruktora w "lworld.cpp".

    lifeagent* clone() const
    { return new lifeagent(*this);}

    ~lifeagent() override
    {_clean();}

    void clean() override
    {_clean();}

    void assign123(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        First=unsigned( (long(Red)+long(Green)+Blue)/3 ) >> kate_shift;
        Second=0;
    }


    long RGB()
    {
        return (unsigned long) ( (unsigned char) (First) );
    }

    friend
    ostream& operator << (ostream& o,const lifeagent& a)
    {
        o<<'{';
        o<<' '<<a.First<<' ';
        o<<'}';
        return o;
    }

    friend
    istream& operator >> (istream& i,lifeagent& a)
    {
        char pom;
        i>>pom;		//ignoruje {
        i>>a.First;
        i>>pom;		//ignoruje }
        return i;
    }

};

// /////////////////////////////////////////////
// Example for SYMSHELL CLASSES library.
/// @author Wojciech Borkowski, iss.uw.edu.pl
// https://github.com/borkowsk/symShell2andRTM
/// @copyright 2000 - 2026
// /////////////////////////////////////////////

