#ifndef __FIFOSOUR_HPP__
#define __FIFOSOUR_HPP__
//Klasa kolejkujaca wartosci ze zrod�a skalarnego
//------------------------------------------------
#include "datasour.hpp"


template<class T> //T - typ wartosci w buforze. Nie koniecznie double !!!
class fifo_source:public linear_source_base
//----------------------------------------------
{
typedef template_scalar_source_base<T> SOURCE;//Typ zrodlowy
wb_dynarray<char>  _name;
SOURCE*				Source;
wb_dynarray<T>		bufor;
int					Nastepny;//Granica wpisania gdy idzie cyklicznie

//Metoda obslugujaca FIFO
//Laduje wartosc do bufora
//i ewentualnie zmienia N 
//zalezne id sprawdzania MISSING VALUE wiec musi byc z double
//const T& - TO DO ZMIANY GDY data_source_base bedzie SZABLONEM
void add(const double& val);

int check_version()
	//Sprawdza czy i jak zmienily sie dane w zrodle
	//i laduje nastepna wartosc
	{
	if(Source->data_version()==-1)
		return 0;//Przed inicjalizacja nic nie zapisuje
	int ret=update_version_from(Source);
	if(ret==1)//Nowe dane
    {
        double pom=Source->get();
		add(pom);
    }
	return ret;
	}

public:
	SOURCE* _get_source() { return Source;}//Mo�na odnale�� �r�d�o danych i np. jako� delikatnie wp�yn�� 

// KONSTRUKTOR
	fifo_source(SOURCE* isource,size_t buffsize,
				const char* format="%s(last %0.3g steps)",
				double imin=0,double imax=0
				):
			linear_source_base(0,format),Source(isource),
			bufor(buffsize),Nastepny(0)//,Pierwszy(-1)
	{
												assert(Source!=NULL);
	size_t dummy;//Nie istotne
	linear_source_base::set_missing(Source->get_missing());//Tu przenosimy "na zywca"
	Source->bounds(dummy,ymin,ymax);
	if(ymin==ymax)//Nieustalone
		{
		ymin=imin;
		ymax=imax;
		}
		else
		{
		if(ymin>imin) ymin=imin;
		if(ymax<imax) ymax=imax;
		}
	check_version();
	}

//DESTRUKTOR
	~fifo_source(){}

//Restartuje versioning zrodel. W wypadku podzrodel powinna byc reimplementacja!
void	restart_counting()	
{
    linear_source_base::restart_counting();
    Nastepny=0;// Jak w konstruktorze
    linear_source_base::N=0; //Nie ma juz aktualnych wartosci
    //linear_source_base::
    //Pierwszy=-1;//
}

//Przedefiniowana obsluga "missing values" bo wartosci w buforze moga  nie byc typu double!!!
void set_missing(const T& imiss)
{
	linear_source_base::set_missing(double(imiss));
}

//A stara obsluga zaslonieta - widoczna tylko jako wirtualna
private:   void set_missing(double imiss)
{                                                assert(wbrtm::limit<T>::Min()<=imiss && imiss<=wbrtm::limit<T>::Max());
    linear_source_base::set_missing(double(imiss));
}

public:
T get_missing();  //Musi byc specyficznie zdefiniowana dla float, long , int, unsigned

int is_missing(const T& val)
{
	double vald=double(val);//Zakladamy ze w ta strone zawsze jest OK
	return linear_source_base::is_missing(vald);
}

//Przedefiniowane akcesory wirtualne
const char* name();//KOmbinowana nazwa seri FIFO
	
long data_version()//numer wersji danych
	{check_version();return data_source_base::data_version();}

long how_old_data()//od ilu wersji dane sie nie zmienily
	{check_version();return data_source_base::how_old_data();}

//Na wypadek gdy monitorowana zmienna mo�e si� zmieni� na skutek dzia�a� uzytkownika
void  force_last(double val);

void  bounds(size_t& num,double& min,double& max)
//Sprawdza zrodlo i podaje ile elementow,wartosc minimalna i maksymalna
	{
	check_version();
	Source->bounds(num,min,max);
	if(min<max)//Ustawione przez kogos
		{
		if(ymin>min) ymin=min;
		if(ymax<max) ymax=max;
		}
	num=N;
	min=ymin;
	max=ymax;
	}

iteratorh  reset()//Rozpoczecie czytania bufora
	{ check_version();return linear_source_base::reset();}

double  get(iteratorh& ptr_to_iterator);//Daje nastepna z N liczb!!!

double	get(size_t index_from_geometry);//Daje konkretna z N liczb	
};

template<>
inline double fifo_source<double>::get_missing()
{
	double pom=linear_source_base::get_missing();
	double	   pomT(pom);                   //NOTE: missing should be inside T
	//assert(double(pomT)==double(pom));  //check this constrain!
	return pomT;
}

template<>
inline float fifo_source<float>::get_missing()
{
	double pom=linear_source_base::get_missing();
	float   pomT=float(pom);                   //NOTE: missing should be inside T 	
	if(double(pomT)!=double(pom))  //check this constrain!
        pomT=default_missing<float>();
	return pomT;
}

template<>
inline long fifo_source<long>::get_missing()  
{
	double pom=linear_source_base::get_missing();
	long    pomT=long(pom);                   //NOTE: missing should be inside T 	
	if(double(pomT)!=double(pom))  //check this constrain!
        pomT=default_missing<long>();
	return pomT;
}

template<>
inline int fifo_source<int>::get_missing()  
{
	double pom=linear_source_base::get_missing();
	int	   pomT=int(pom);                   //NOTE: missing should be inside T 	
	if(double(pomT)!=double(pom))  //check this constrain!
        pomT=default_missing<int>();
	return pomT;
}

template<>
inline unsigned long fifo_source<unsigned long>::get_missing()  
{
	double pom=linear_source_base::get_missing();
	unsigned long pomT((unsigned long)(pom));                   //NOTE: missing should be inside T 	
	if(double(pomT)!=double(pom))  //check this constrain!
        pomT=default_missing<unsigned long>();
	return pomT;
}

template<>
inline unsigned int fifo_source<unsigned int>::get_missing()  
{
	double pom=linear_source_base::get_missing();
	unsigned int pomT((unsigned int)(pom));                   //NOTE: missing should be inside T 	
	if(double(pomT)!=double(pom))  //check this constrain!
        pomT=default_missing<unsigned int>();
	return pomT;
}

template<class T>
inline T fifo_source<T>::get_missing()  
{
	double pom=linear_source_base::get_missing();
	T	   pomT(default_missing<T>());  //NOTE: missing value should be inside type T 	
	assert(double(pomT)==double(pom));  //check this constrain!
	return pomT;
}

template<class T>
inline void fifo_source<T>::add(const double& val)
//Metoda obslugujaca FIFO,laduje wartosc do bufora i ewentualnie zmienia N 
	{
	size_t TN=bufor.get_size();//Ile sie miesci w tablicy

	//Implementacja cyklicznosci bufora
	Nastepny%=TN;//Zeby nie wyszlo z tablicy. CYKL!

	//if(Nastepny==Pierwszy)//Pelny bufor
	//	{
	//	Pierwszy++; //Zapominamy jednego z bufora
	//	Pierwszy%=TN;//Gdyby wylazlo za bufor to zawija do przodu
	//	}

	if(!Source->is_missing(val))
		{
		if(ymax<val)
			ymax=val;
		if(ymin>val)
			ymin=val;
		//Zapamientanie
		bufor[Nastepny]=T(val);//Uwaga - bufor typu T,  a val typu double
		}
		else
		bufor[Nastepny]=T(get_missing());

	//Przemiesczenie indeksu pustego miejsca
	Nastepny++;

	//Gospodarka miejscem
	if(N<TN) 
		{
		N++;//Wypelnianie pustego miejsca w buforze
		//if(Pierwszy==-1) 
		//	Pierwszy++;
		}

	}

template<class T>
inline void  fifo_source<T>::force_last(double val)
//Zmienia zawarto�� ostatniej kom�rki - np. gdy u�ytkownik zmieni warto�� zmiennej
{
	size_t TN=bufor.get_size();//Ile si� miesci w tablicy

	//Implementacja cyklicznosci bufora
	size_t ind=(Nastepny>0?Nastepny-1:0);
	ind%=TN;//Zeby nie wyszlo z tablicy. CYKL! (niepotrzebne tutaj?)

	if(!Source->is_missing(val))
		{
		if(ymax<val)
			ymax=val;
		if(ymin>val)
			ymin=val;
		//Zapamientanie
		bufor[ind]=T(val);//Uwaga - bufor typu T,  a val typu double
		}
		else
		bufor[ind]=T(get_missing());
}

template<class T>
inline double fifo_source<T>::get(data_source_base::iteratorh& ptr_to_iterator)
//Daje nastepna z N liczb!!!
	{ 
	size_t TN=bufor.get_size();
	size_t poz=_next(ptr_to_iterator);
	if(N==TN)
		{
		poz+=Nastepny;
		//poz+=(Pierwszy);//-1? Bo to pierwszy,ale dla nastepnego kroku!!!
		poz%=TN;//Przewiniecie przez koniec bufora w razie co
		}
	return bufor[poz]; 
	}

template<class T>
inline double	fifo_source<T>::get(size_t poz)
//Daje konkretna z N liczb - ale po przewinieciu dzial do D...
    { 	
    if(poz<N)
	    return bufor[poz]; 
    else
        return miss;
	}

template<class T>
inline const char* fifo_source<T>::name()
//Musi zwracac zawsze aktualna kombinowana nazwe serii
	{
	const char* pom=Source->name();
	if(!_name.IsOK() || strstr(_name.get_ptr_val(),pom)==NULL)
	//Jesli jeszcze nie ma albo zmienilo sie w obiekcie zrodla
		{
		_name.alloc(strlen(title_util::name())+strlen(pom)+ZAPAS_NA_CYFRY+1);
		sprintf(_name.get_ptr_val(),title_util::name(),pom,double(bufor.get_size()));
		}
	return _name.get_ptr_val();
	}


#endif

/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://www.iss.uw.edu.pl/borkowski                  */
/*        MAIL: wborkowsk@uw.edu.pl                                 */
/*                               (Don't change or remove this note) */
/********************************************************************/



