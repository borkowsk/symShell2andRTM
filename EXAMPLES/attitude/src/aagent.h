/// @file
/// @brief
///  @EN{ DECLARATION OF A TYPE 'aagent' FOR "attitudes" SIMULATION. }
///  @PL{ DEKLARACJA TYPU 'aagent' DO SYMULACJI postaw. }
/// @date 2026-05-20 (modified)
///       ============================================================
/// @PL{ BARDZO OSZCZĘDNIE SKOMENTOWANE.} @EN{ VERY SPARINGLY COMMENTED.}
/// @PL{ DOXYGENIZACJA WYŁĄCZNIE PO POLSKU. }
/// @EN{ DOXYGENIZATION IN POLISH ONLY. }
//======================================================================================================================

#include "layer.hpp"

using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

/// @brief @PL{ Agent symulacji "attitudes". } @EN{ "Attitudes" simulation agent. }
class aagent:public agent_base
{
    friend class aworld; ///< Przyjacielska deklaracja klasy świata. Na razie tak — żeby uprościć dostęp do składowych.

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
        if(aagent::str_grow)
        {
            Power+=aagent::str_grow;
            Power%=aagent::max_str; //Nigdy nie przekracza siły maksymalnej
        }
    }

    /// Konstruktor 1.
    aagent(const aagent& ini);	//Konkretna implementacja w aworld!

    /// Konstruktor 2.
    aagent();					//Konkretna implementacja w aworld!

    /// Wymagane klonowanie.
    aagent* clone() const
    { return new aagent(*this);}

    /// Destruktor najpierw czyści agenta, a potem dopiero wykonuje właściwą destrukcję.
    ~aagent() override
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
    ostream& operator << (ostream& o,const aagent& a)
    {
        o<<'{';
        o<<' '<<a.Power<<' '<<a.First<<' '<<a.Second<<' ';
        o<<'}';
        return o;
    }

    friend
    istream& operator >> (istream& i,aagent& a)
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
