// Klasa bazowa filtra zależnego od dwu źródeł
//*////////////////////////////////////////////////////////////////////////////
#ifndef __bifilter_hpp__
#define __bifilter_hpp__
#include "datasour.hpp"

//Zrodlo filtrujace dane z innego zrodla
class bi_filter_source_base:public filter_source_base
//-----------------------------------------------------------------
{

protected:
data_source_base* Source2;
double		 source2_miss;

virtual int check_version()
	//Sprawdza czy i jak zmienily sie dane w zrodlach
	//W filtrach cache'ujacych moze powodowac oproznienie
	//lub ponowne napelnienie. Zwraca 1 jesli zmienilo wersje.
	//0 jesli wersje sie zgadzaja.
	{
	int update2=update_version_from(Source2);//Tu ewentualnie moze zmienic wersje
	int update1=update_version_from(Source);//Tylko zeby dac zrodlu szanse updatu
	assert(update1==0);//NIe powinno juz zmienic wersji
    
    //Aktualizacja "missing values"
	source_miss=Source->get_missing();
	source2_miss=Source2->get_missing();
	return update2;
	}

//Wewnetrzna struktura dla indeksowania
struct indexes
	{ 
	iteratorh first;
	iteratorh second;
	indexes(iteratorh ifirst,iteratorh isecond):
				first(ifirst),second(isecond)
				{}
	};

public:
//Constructor
bi_filter_source_base(data_source_base* ini1=NULL,
					  data_source_base* ini2=NULL,
					  const char* format="F(%s,%s)"):
	  filter_source_base(ini1,format)
	  {		 
		 set_second_source(ini2);		 
	  }

void set_second_source(data_source_base* ini)
	{ 
		Source2=ini;
		source2_miss=Source2->get_missing();
	}

void set_first_source(data_source_base* ini)
	{ 
		Source=ini;
		source_miss=Source->get_missing();
	}

data_source_base* get_second_source()//Zwraca wskaznik do drugiej seri zrodlowej
	{ 
		source2_miss=Source2->get_missing();
		return Source2;
	}

data_source_base* get_first_source()//Zwraca wskaznik do pierwszej seri zrodlowej
	{ 
		source_miss=Source->get_missing();
		return Source;
	}

//Sposob tworzenia nazwy musi byc przedefiniowany
virtual const char* name();//Musi zwracac nazwe serii albo "" - NIE NULL!!!

virtual geometry_base* getgeometry()
//Powinien zwracac wskaznik do obowiazujacej geometri danych
//domyslnie nie mozna ustalic geometri bo sa dwie alternatywne,
//ale klasy potomne moga zadecydowac ktora wziac.
	{ 
	return NULL;
	}

//DOSTEP DO DANYCH:
//UWAGA: Zlozona iteracja wykonuje sie do konca krotszego ze zrodel.
//--------------------------------------------------------------------
virtual void  bounds(size_t& N,double& min,double& max)
	//Ile elementow,wartosc minimalna i maksymalna
	//Byc moze wartosci te trzeba przekonwertowac
	{ 
		check_version();

		size_t N1=0,N2=0;
		double min1=0,min2=0,max1=0,max2=0;
		Source->bounds(N1,min1,max1);
		Source2->bounds(N2,min2,max2);
		N=(N1<N2?N1:N2);
		_bounds(min1,max1,min2,max2,min,max);
	}

virtual iteratorh   reset()
	//Umozliwia czytanie od poczatku - iteratorh jest uchwytem iteratora zlozonego
	{  
	   check_version();  //Zeby zrodla mialo szanse na update
	   
	   //Tworzenie zlozonego indeksu:
	   indexes* pom=new indexes(Source->reset(),Source2->reset());	  

	   return pom;
	}


virtual double get(iteratorh& I)
	//Daje nastepna z N liczb!!! Po N-tej obiekt zrodlowy zwalnia iterator!
	{ 
	indexes* pom=(indexes*)I;
	double val1=Source->get(pom->first);
	double val2=Source2->get(pom->second);
	if(pom->first==NULL || pom->second==NULL)//Ktores zrodlo sie wyczerpalo
		{
		Source->close(pom->first);
		Source2->close(pom->second);
		delete pom;
		I=NULL;//Informacja zwrotna	
		}
	return _get(val1,val2);
	}


virtual double get(size_t index_from_geometry)
	//Przetwarza index uzyskany z geometri
	//na wartosc z serii, o ile jest mozliwe czytanie losowe
    //(domyslnie nie jest, bo nie wiadomo czy ta sama geometria)
	//i uzywamy _get do interpretacji pary wartosci
	{ 
    double val1=Source->get(index_from_geometry);
	double val2=Source2->get(index_from_geometry);
    return _get(val1,val2);
	}		

virtual void  close(iteratorh& I)
	//Obiekt  zwalnia iteratory wewnetrzne i caly iterator jesli nie zostal 
	//zwolniony przez get(N)
	{
	indexes* pom=(indexes*)I;
	if(pom==NULL) return;
	Source->close(pom->first);
	Source2->close(pom->second);
	delete pom;
	I=NULL;//Informacja zwrotna
	}

//Implementacja konkretnej operacji - musi sprawdzac czy 
//ktoras z wartosci lub obie nie sa "missing"
protected:
virtual double _get(double val1,double val2)=0;

//implementacja decyzji o wartosci minimalnej i maksymalnej
virtual void  _bounds(double& min1,double& max1,
					  double& min2,double& max2,
					  double& min,double& max)=0;

//Uzupelnienie o sprawdzanie czy ktores get nie dalo missing
virtual int FromFirstIsMissing(double val)
	{ 
	if(val==source_miss) return 1;
	   else return 0; 
	}

virtual int FromSecondIsMissing(double val)
	{ 
	if(val==source2_miss) return 1;
	   else return 0; 
	}

};


inline
const char* bi_filter_source_base::name()
//Musi zwracac nazwe serii albo "" - NIE NULL!!!
	{
	const char* pom1=Source->name();
	const char* pom2=Source2->name();
	if( 
		!_name.IsOK() || 
	   strstr(_name.get_ptr_val(),pom1)==NULL ||
	   strstr(_name.get_ptr_val(),pom2)==NULL
	   )	//Jesli jeszcze nie ma albo zmienilo sie ktoryms obiekcie zrodla
		{
		_name.alloc(strlen(title_util::name())+strlen(pom1)+strlen(pom2)+1);
		sprintf(_name.get_ptr_val(),title_util::name(),pom1,pom2);
		}
	return _name.get_ptr_val();	
	}


#endif




/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/



