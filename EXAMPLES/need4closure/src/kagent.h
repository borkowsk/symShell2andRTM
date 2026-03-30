//DECLARATION OF    A G E N T   FOR "need 4 closure" SIMULATION
/////////////////////////////////////////////////////////////
#include "layer.hpp"
#include "krand.h"

inline void wb_swap(short& a,short& b)
{
	short c=a;
	a=b;
	b=c;
}

class kagent:public agent_base
{
	friend class kworld;//Zeby uproscic dostep do skladowych.
	
	// STATYCZNE SKLADOWE - PARAMETRY INICJOWANIA I ZMIANY AGENTÓW
	static short max_sila;//Maksymalna sila agenta
    static int   treshold;//Granica domkniecia pogladu
	//static short ile_kate;//Ilosc kategori w mapach	
	//static short kate_shift;//Przesuniecie dla wczytywania gifa
    static double Majority; //Udzial w calosci przekonanych do wiekszej klasy 
    static double Minority; //Udzial w calosci przekonanych do mniejszej klasy 
	static double NoiseLevel;//Prawd. spontanicznej zmiany
	
    static short DrawAttitude();//Funkcja do losowania przekonania - uzywa Majority i Minority

	// SKLADOWE DLA SYMULACJI
	short Power;	//Sila agenta
	short First;	//Aktualne przekonanie -1,0,1 (Left,Neutral,Right)
	short Second;	//Nowe przekonanie lub poprzednie 

    unsigned int   ForRight;
    unsigned int   ForLeft;

    //short Press;    //Nacisk spoleczny - sumaryczna sila za zwyciezajacym pogladem, o ile agent go nie wyznaje, albo 0
    bool  DurCh:1;  //Czy jest w trakcie zmieniania (do zarzadzania zmianami stanow)
    
	void _clean()
	{		
        Power=-1;
		
        First=0;Second=0;

        ForRight=0;ForLeft=0;
        //Press=0;
        DurCh=false;
	}
	
	// TO CO MUSI byc zdefiniowane
	///////////////////////////////////
public:
	int IsOK()
	{
		return Power!=-1;
	}
	
	kagent(const kagent& ini);	//Konkretna implementacja w kworld!
	
	kagent();					//Konkretna implementacja w kworld!

	kagent* clone() const
	{ return new kagent(*this);}
		
	~kagent()
	{_clean();}
	
	void clean()
	{_clean();}

    void new_attitude(short a)
    {
        Second=a;//Takie ma byc nowe przekonanie
        DurCh=true;//Sygnal ze juz jest "w trakcie" zmiany - np. zeby zapobiec powtorce
    }

    void update() //Kontrola zmiany stanu
    {
        assert(DurCh);//Powinien byc w trakcie zmiany
        wb_swap(First,Second);
        DurCh=false; //Teraz jest juz zmieniony
    }
	
	void assign_curr(unsigned char Red,unsigned char Green,unsigned char Blue)
	{
        ForLeft=Red;
        ForRight=Blue;
		First=0;//Na razie bez zdecydowania
	}
	
    void assign_prev(unsigned char Red,unsigned char Green,unsigned char Blue)
	{
		Second=0;	
	}

	void assignPow(unsigned char Red,unsigned char Green,unsigned char Blue)
	{
		Power=short((int(Red)+int(Green)+int(Blue))/(3.*255)*max_sila);
	}
	
	void killBlack(unsigned char Red,unsigned char Green,unsigned char Blue)
	{
		if(Red==0 && Green==0 && Blue==0)
			_clean();
	}

	long Classif()
	{
		return First;
	}
	
	long RGB()
	{
		return (unsigned long) ( (unsigned char) (First) );
	}

    friend
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
	
	friend
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

//Konstrukcja agentow
///////////////////////////////////
inline
kagent::kagent(const kagent& ini)
	{
		if(&ini!=NULL)
		{
			First=ini.First;
			Second=ini.Second;
			Power=1+RANDOM(max_sila);//Sila jest przydzielana z rozkladu
            ForLeft=RANDOM(treshold);//Licznik przekonan za "Lewymi"
            ForRight=RANDOM(treshold);//Licznik przekonan za "Prawymi"
		}
		else
			_clean();
	}

inline
kagent::kagent()
	{
		_clean();		
        Power=1+RANDOM(max_sila);
        ForLeft=RANDOM(treshold);//Licznik przekonan za "Lewymi"
        ForRight=RANDOM(treshold);//Licznik przekonan za "Prawymi"
		First=DrawAttitude();
		Second=First;
	}

inline
short kagent::DrawAttitude()
    {
        assert(Majority+Minority<=1);

        double pom=DRAND();
        if(pom<Majority)
        {
            return -1;  //Czarni=Lewi
        }
        else 
            if(pom<Majority+Minority)
            {
                return 1;//Biali=Prawi
            }
            else
                return 0;//Niezdecydowani

    }




/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*                                                                  */
/*      Instytut Studiow Spolecznych Uniwersytetu Warszawskiego     */
/*                                                                  */
/*        WWW:  http://www.iss.uw.edu.pl/~borkowsk                  */
/*        MAIL: borkowsk@samba.iss.uw.edu.pl                        */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
