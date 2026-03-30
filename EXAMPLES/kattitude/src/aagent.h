//DECLARATION OF    A G E N T   FOR "attitudeS" SIMULATION
/////////////////////////////////////////////////////////////
#include "layer.hpp"

inline void wb_swap(short& a,short& b)
{
	short c=a;
	a=b;
	b=c;
}

class aagent:public agent_base
{
	friend class aworld;//Na razie tak - zeby uproscic dostep do skladowych.
	
	// STATYCZNE SKLADOWE - PARAMETRY INICJOWANIA AGENTÓW
	static short ruchsily;//Czy sila sie zmienia (rosnie) z wiekiem
	static short max_sila;//Maksymalna sila agenta
	static short ile_kate;//Ilosc kategori w mapach	
	static short kate_shift;//Przesuniecie dla wczytywania gifa
    static double Majority; //Udzial najwiekszej klasy w calosci
	static double MutationLevel;//Prawd. spontanicznej zmiany pogladow (0..1)
	
    static short DrawAttitude();//Funkcja do losowania pogladu

	// SKLADOWE DLA SYMULACJI
	short Power;	//Sila agenta
	short First;	//Aktualne przekonanie
	short Second;	//Nowe przekonanie lub poprzednie 
    short Press;    //Nacisk spoleczny - sumaryczna sila za zwyciezajacym pogladem, o ile agent go nie wyznaje, albo 0
    bool  DurCh:1;  //Czy jest w trakcie zmieniania (do zarzadzania zmianami stanow)
    
	void _clean()
	{
		First=-1;
		Second=-1;
		Power=-1;
        Press=0;
        DurCh=false;
	}
	
	// TO CO MUSI byc zdefiniowane
	///////////////////////////////////
public:
	int IsOK()
	{
		return First!=-1 && Second!=-1 && Power!=-1;
	}
	
	void MakeOlder()//Sila jako wiek
	{
		if(aagent::ruchsily)
		{
			Power+=aagent::ruchsily;
			Power%=aagent::max_sila;//Nigdy nie przekracza sily maksymalnej
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

    void new_attitude(short a)
    {
        Second=a;//Takie ma byc nowe przekonanie
        DurCh=true;//Sygnal ze juz jest "w trakcie" zmiany - np. zeby zapobiec powtorce
    }

    void update() //Kontrola zmiany stanu
    {
        assert(DurCh);  //Powinien byc w trakcie zmiany
        wb_swap(First,Second);
        DurCh=false;    //Teraz jest juz zmieniony
    }
	
	void assign_curr(unsigned char Red,unsigned char Green,unsigned char Blue)
	{
		First=Red>>kate_shift;
		Second=Blue>>kate_shift;	
	}
	
    void assign_prev(unsigned char Red,unsigned char Green,unsigned char Blue)
	{
		First=Red>>kate_shift;
		Second=Blue>>kate_shift;	
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


