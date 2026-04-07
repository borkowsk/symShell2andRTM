// DECLARATION OF   , A G E N T   FOR "attitudes" SIMULATION.
// /////////////////////////////////////////////////////////
// @date 2026-04-07 (modification)
#include "layer.hpp"

class aagent:public agent_base
{
    friend class aworld; //Na razie tak — żeby uprościć dostęp do składowych.

    // STATYCZNE SKŁADOWE - PARAMETRY INICJOWANIA AGENTÓW
    static short str_grow; //Czy siła się zmienia (rośnie) z wiekiem
    static short max_str; //Maksymalna siła agenta
    static short n_of_cate; //Liczba kategorii w mapach
    static short cate_shift; //Przesuniecie dla wczytywania gifa
    static double mutation_prob; //Prawd. spontanicznej zmiany poglądów (0..1)

    // SKŁADOWE DLA SYMULACJI
    short Power;	//Sila agenta
    short First;	//Pierwsze przekonanie
    short Second;	//Nowe przekonanie


    void _clean()
    {
        First=-1;
        Second=-1;
        Power=-1;
    }

    // TO CO MUSI byc zdefiniowane:
    // ////////////////////////////
public:
    int IsOK()
    {
        return First!=-1 && Second!=-1 && Power!=-1;
    }

    void make_older() //Sila jako wiek
    {
        if(aagent::str_grow)
        {
            Power+=aagent::str_grow;
            Power%=aagent::max_str; //Nigdy nie przekracza siły maksymalnej
        }
    }

    aagent(const aagent& ini);	//Konkretna implementacja w aworld!

    aagent();					//Konkretna implementacja w aworld!

    aagent* clone() const
    { return new aagent(*this);}

    ~aagent()
    {_clean();}

    void clean()
    {_clean();}

    void assign123(unsigned char Red,unsigned char Green,unsigned char Blue)
    {
        First=Red >> cate_shift;
        Second=Blue >> cate_shift;
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

    long classif()
    {
        return First;
    }

    long RGB()
    {
        return (unsigned long) ( (unsigned char) (First) );
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

/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */


