/// @file
/// @brief
///  @EN{ DECLARATION OF 'life_agent' FOR "life" SIMULATION. }
///  @PL{  }
/// @date 2026-05-21 (modified)
///       =========================================================
/// @details ...
//======================================================================================================================

#include "layer.hpp"

using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

/// @PL{ Klasa agenta do implementacji "Life". }
/// @EN{ Agent class for implementing "Life". }
class life_agent: public sym2::shell::agent_base
{
    friend class life_world; //!< Na razie tak, żeby uprościć dostęp do składowych.

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
    bool IsOK() override
    {
        return true;
    }

    /// Określenie, czy agent jest żywy, tj. bierze udział w symulacji.
    int is_alive()
    {
        return 1; //Chyba zawsze tak...
    }

    life_agent(const life_agent& ini); //!< Konkretna implementacja tego konstruktora w "lworld.cpp".

    life_agent(); //!< Konkretna implementacja tego konstruktora w "lworld.cpp".

    /// Tworzenie klonu agenta na stercie.
    life_agent* clone() const
    { return new life_agent(*this);}

    /// Destruktor wirtualny.
    ~life_agent() override
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
    ostream& operator << (ostream& o,const life_agent& a)
    {
        o<<'{';
        o<<' '<<a.First<<' ';
        o<<'}';
        return o;
    }

    ///Odczyt ze strumienia.
    friend
    istream& operator >> (istream& i, life_agent& a)
    {
        char pom;
        i>>pom;		//ignoruje {
        i>>a.First;
        i>>pom;		//ignoruje }
        return i;
    }

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



