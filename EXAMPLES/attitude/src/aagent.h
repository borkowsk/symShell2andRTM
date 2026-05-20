/// @file
/// @brief
///  @EN{ DECLARATION OF A TYPE 'aagent' FOR "attitudes" SIMULATION. }
///  @PL{ DEKLARACJA TYPU 'aagent' DO SYMULACJI postaw. }
/// @date 2026-05-21 (modified)
///       ============================================================
/// @PL{ BARDZO OSZCZĘDNIE SKOMENTOWANE.} @EN{ VERY SPARINGLY COMMENTED.}
/// @PL{ DOXYGENIZACJA WYŁĄCZNIE PO POLSKU. }
/// @EN{ DOXYGENIZATION IN POLISH ONLY. }
//======================================================================================================================

#include "layer.hpp"

using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

/// @brief @PL{ Agent symulacji "attitudes". }
///        @EN{ "Attitudes" simulation agent. }
class attitude_agent: public sym2::shell::agent_base
{
    friend class attitude_world; ///< Przyjacielska deklaracja klasy świata. Na razie tak — żeby uprościć dostęp do składowych.

    /// @name STATYCZNE SKŁADOWE - PARAMETRY INICJOWANIA AGENTÓW.
    /// @{
    static short	str_grow;		///< Określa, czy siła się zmienia (rośnie) z wiekiem.
    static short	max_str;		///< Maksymalna siła agenta.
    static short	n_of_cate;		///< Liczba kategorii w mapach.
    static short	cate_shift;		///< Przesuniecie dla wczytywania gifa.
    static double	mutation_prob;	///< Prawd. spontanicznej zmiany poglądów (0..1).
    /// @}

    /// @name SKŁADOWE AGENTA DEFINIUJĄCE JEGO STAN W TRAKCIE SYMULACJI
    /// @{
    short	Power;	///< Siła agenta.
    short	First;	///< Pierwsze przekonanie.
    short	Second;	///< Nowe przekonanie.
    /// @}

    void _clean()
    {
        First=-1;
        Second=-1;
        Power=-1;
    }


public:
    // TO CO MUSI byc zdefiniowane:
    // ////////////////////////////

    bool IsOK() override ///< Jest "wporzo" gdy ma stan zdefiniowany.
    {
        return First!=-1 && Second!=-1 && Power!=-1;
    }

    void make_older() ///< Starzenie się agenta. Sila jako wiek. Nigdy nie przekracza siły maksymalnej.
    {
        if(attitude_agent::str_grow)
        {
            Power+=attitude_agent::str_grow;
            Power%=attitude_agent::max_str; //Nigdy nie przekracza siły maksymalnej
        }
    }

    /// Konstruktor 1.
    attitude_agent(const attitude_agent& ini);	//Konkretna implementacja w aworld!

    /// Konstruktor 2.
    attitude_agent();					//Konkretna implementacja w aworld!

    /// Wymagane klonowanie.
    attitude_agent* clone() const
    { return new attitude_agent(*this);}

    /// Destruktor najpierw czyści agenta, a potem dopiero wykonuje właściwą destrukcję.
    ~attitude_agent() override
    {_clean();}

    void clean() override
    {_clean();}

    void assign123(unsigned char Red,unsigned char /*Green*/,unsigned char Blue)
    {
        First  = Red >> cate_shift;
        Second = Blue >> cate_shift;
    }

    void assignPow(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        Power=short((int(Red)+int(Green)+int(Blue)) / (3.*255) * max_str);
    }

    void killBlack(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        if(Red==0 && Green==0 && Blue==0)
            _clean();
    }

    long classif() const
    {
        return First;
    }

    long RGB() const
    {
        return ( (unsigned char) (First) );
    }

    friend
    ostream& operator << (ostream& o,const attitude_agent& a)
    {
        o<<'{';
        o<<' '<<a.Power<<' '<<a.First<<' '<<a.Second<<' ';
        o<<'}';
        return o;
    }

    friend
    istream& operator >> (istream& i, attitude_agent& a)
    {
        char pom;
        i>>pom;		//ignoruje {
        i>>a.Power>>a.First>>a.Second;
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
