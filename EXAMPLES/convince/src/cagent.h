//DECLARATION OF    A G E N T   FOR this SIMULATION
/////////////////////////////////////////////////////////////
#include "layer.hpp"

class anAgent:public agent_base
{
	friend class aWorld;//Na razie tak - zeby uproscic dostep do skladowych.
	
	// STATYCZNE SKLADOWE - PARAMETRY INICJOWANIA AGENTÓW
	static short ruchsily;//Czy sila sie zmienia (rosnie) z wiekiem
	static short min_sila;//Minimalna sila agenta
	static short max_sila;//Maksymalna sila agenta
	static short ile_kate;//Ilosc kategori w mapach	
	static short kate_shift;//Przesuniecie dla wczytywania gifa
	
	static double ToBeNewProb;//Prawd. spontanicznej zmiany pogladow samotnika na nowy typ rozrywki/sportu
	static double NewInfectProb;//Prawd. zainfekowania od pary zainfekowanych
	static double ReverseProb;//Prawd. reversji pogladow na 0 - brak pomyslu na rozrywke
	static double SupportLevel;//Sila wsparcia gdy ma jakies towarzystwo
	
	// SKLADOWE DLA SYMULACJI
	short Power;	//Sila agenta
	short First;	//Pierwsze przekonanie
	short Second;	//Nowe przekonanie
	
	
	void _clean()
	{
		First=-1;
		Second=-1;
		Power=-1;
	}
	
	// TO CO MUSI byc zdefiniowane
	///////////////////////////////////
public:
	int IsOK()
	{
		return First!=-1 && Second!=-1 && Power!=-1;
	}
	/*
	void MakeOlder()//Sila jako wiek
	{
		if(anAgent::ruchsily)
		{
			Power+=anAgent::ruchsily;
			Power%=anAgent::max_sila;//Nigdy nie przekracza sily maksymalnej
		}
	}
	*/
	anAgent(const anAgent& ini);	//Konkretna implementacja w aWorld!
	
	anAgent();					//Konkretna implementacja w aWorld!

	anAgent* clone() const
	{ return new anAgent(*this);}
		
	~anAgent()
	{_clean();}
	
	void clean()
	{_clean();}
	
	void assign123(unsigned char Red,unsigned char Green,unsigned char Blue)
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
	ostream& operator << (ostream& o,const anAgent& a)
	{
		o<<'{';
		o<<' '<<a.Power<<' '<<a.First<<' '<<a.Second<<' ';	
		o<<'}';
		return o;
	}
	
	friend
	istream& operator >> (istream& i,anAgent& a)
	{
		char pom;
		i>>pom;		//ignoruje {
		i>>a.Power>>a.First>>a.Second;
		i>>pom;		//ignoruje }
		return i;
	}

};


