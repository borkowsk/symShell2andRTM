/// @file
/// DECLARATION OF    A G E N T   FOR "life" SIMULATION.
//---------------------------------------------------------------
/// Its is rather simply implementation. You can replace it if you need.
// ////////////////////////////////////////////////////
/// @date 2026-03-27 (modified)

#include "layer.hpp"

/// Klasa agenta do implementacji Life.
class lifeagent:public agent_base
{
    friend class lifeworld; //!< Na razie tak, żeby uprościć dostęp do składowych.

    // STATYCZNE SKŁADOWE - PARAMETRY INICJOWANIA AGENTÓW:
    static short  ile_kate;   //!< Liczba kategorii. Dla Life zawsze 2!!! WIĘC TO POLE NIEUŻYWANE — TYLKO DLA INFORMACJI.
    static short  kate_shift; //!< Przesunięcie do wczytywania (z grafik?).
    static double MutationLevel; //!< Prawd. spontanicznej zmiany stanu (0..1) - chyba tu nieużywane.
    static double InitProp;  //!< Proporcje inicjowania losowego.

    // SKŁADOWE DLA SYMULACJI:
    short First;		//!< Pierwszy, czyli aktualny stan.
    short Second;		//!< Nowy albo stary stan (zależnie od modelu).

    /// Statyczne czyszczenie stanu.
    void _clean()
    {
        First=0;
        Second=0;
    }

    // TO CO MUSI byc zdefiniowane:
    // ////////////////////////////
    public:
    /// Określenie, czy stan poprawny formalnie.
    int IsOK()
    {
        return 1;
    }

    /// Określenie, czy agent jest żywy, tj. bierze udział w symulacji.
    int is_alive()
    {
        return 1; //Chyba tak...
    }

    lifeagent(const lifeagent& ini); //!< Konkretna implementacja tego konstruktora w "lworld.cpp".

    lifeagent(); //!< Konkretna implementacja tego konstruktora w "lworld.cpp".

    /// Tworzenie klonu agenta na stercie.
    lifeagent* clone() const
    { return new lifeagent(*this);}

    /// Destruktor wirtualny.
    ~lifeagent() override
    {_clean();}

    /// Wirtualne czyszczenie.
    void clean() override
    {_clean();}

    ///Do przypisywania stanu z obrazków (RGB)
    void assign123(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        First=unsigned( (long(Red)+long(Green)+Blue)/3 ) >> kate_shift;
        Second=0;
    }

    ///Do odczytywania stanu na obrazek.
    long RGB()
    {
        return (unsigned long) ( (unsigned char) (First) );
    }

    ///Zapis na strumień.
    friend
    ostream& operator << (ostream& o,const lifeagent& a)
    {
        o<<'{';
        o<<' '<<a.First<<' ';
        o<<'}';
        return o;
    }

    ///Odczyt ze strumienia.
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

