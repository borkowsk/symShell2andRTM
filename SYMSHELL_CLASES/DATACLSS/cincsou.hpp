///////////////////////////////////////////////////////////////////////////////
// Filtr liczacy koincydencje klas dwu serii i pochodne statystyki (Hi^2 itp)
///////////////////////////////////////////////////////////////////////////////
#ifndef __COINCIDENTION_SOUR_HPP__
#define __COINCIDENTION_SOUR_HPP__
#include "costatso.hpp"

template<class DATA_SOURCE>
class coincidention_source:public co_statistics_source<DATA_SOURCE>
//------------------------------------------------------------------------------
{
protected:
size_t N;//Required number of Class of First
size_t M;//Required number of Class of Second
wb_ptr<rectangle_geometry>   my_geometry;//Redundantna?
wb_dynmatrix<unsigned long> arra;
int iHi;//indeks dla Hi-kwadrat

/*
//Zwraca indeks do aktualnego i przesuwa iterator. 
//Zeruje iterator jesli koniec danych. Zwraca ULONG_MAX jesli missing...
size_t _next(iteratorh& p)
{
return my_geometry.get_next(p);
}
*/

double _get(size_t index)
//Bezposrednio siega do tablicy arra
{
	double pom;
	assert(arra);
	i=index/nn;//Który wiersz
	assert(arra[i]);
	j=index%nn;//Która kolumna
	return arra[i][j];
}


int _calculate() //Zwraca 1 jesli musial przeliczyc
{
	if(!basic_statistics_source<DATA_SOURCE>::_calculate()) 
		return 0;

	size_t nn,mm;//Real numbers of classes
/*	
	{//OBLICZANIE HISTOGRAMU
	assert(N==-1);//Tylko tryb integerowy zaimplementowany

	size_t i;
	size_t SN,KL;
	double smin,smax;	
	Source->bounds(SN,smin,smax);
	
	if(smax-smin<=double(size_t(-1)))//Czy w zakresie size_t
		KL=size_t(smax-smin)+1;//Ile jednostek calkowitych zakresu
		else
		goto ERROR;

	arra.alloc(KL);
	if(!arra) //blad alokacji - za malo/za duzo?
		goto ERROR;

	for(i=0;i<KL;i++)
		arra[i]=0;

	//PETLA ZLICZANIA
	iteratorh Ind=Source->reset();
	source_miss=Source->get_missing();	
	for(i=0;i<SN;i++)
		{
		double pom=Source->get(Ind);
		if(!FromSourceIsMissing(pom))
			{	
			pom-=smin;//Przesuniecie
			arra[pom]++;
			}
		}
	Source->close(Ind);	

	//PETLA	MIN/MAX
	ymin=DBL_MAX;
	ymax=0;
	size_t licz=0,minp=0,maxp=0;

	for(i=0;i<KL;i++)
	{
		double pom=arra[i];
		
		if(pom>0)
			licz++;

		if(pom>ymax) 
		{
			ymax=pom;
			maxp=i;
		}

		if(pom<ymin) 
		{
			ymin=pom;
			minp=i;
		}
	}

	if(table[6]!=NULL)
		{
		table[6]->change_val(ymax);
		}

	if(table[7]!=NULL)
		{
		table[7]->change_val(licz);
		}

	if(table[8]!=NULL)
		{
		table[8]->change_val(maxp+smin+0.5);//0.5 bo srodek przedzialu calkowitego
		}

	return 1;
	}//Musial przeliczyc
*/	
ERROR:
	arra.dispose();
	ymin=ymax=0;
	return 1;
}

public:
virtual size_t number_of_subseries()
	{
	return co_statistics_source::number_of_subseries()+
			4;//Ma cztery podzrodla
	}

void all_subseries_required()//Alokuje i ewentualnie rejestruje w menagerze wszystkie serie
{
	co_statistics_source<DATA_SOURCE>::all_subseries_required();
	//MAX CLASS
	Hi();
}

//Acces to "childrens"
scalar_source<double>*      Hi(const char* format="Hi(%s)")	
{
	iHi=co_statistics_source::number_of_subsseries();
	return GetMonoSource(iHi,format);
}


//Construction
	coincidention_source(DATA_SOURCE* ini1=NULL,
						 DATA_SOURCE* ini2=NULL,
						size_t NumberOfClass1=-1,//-1 oznacza tryb calkowitoliczbowy
						size_t NumberOfClass2=-1,//-1 oznacza tryb calkowitoliczbowy
						sources_menager_base* MyMenager=NULL,
						size_t table_size=1/*ZAPAS*/,
						const char* format="COINCIDENT(%s,%s)"):
	N(NumberOfClass1),
	M(NumberOfClass2),
	iHi(-1),
	co_statistics_source<DATA_SOURCE>(ini1,ini2,
						MyMenager,
						4+table_size,//4 wlasne + z klas potomych
						format) 
	{}
	
	~coincidention_source(){}

// Accession Methods
void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
{
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy
	num=get_size();	
	min=ymin;max=ymax;
}

size_t get_size()
//ile elementów
{ 
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy	
	if(!my_geometry)
			return 0;
	return my_geometry->get_size();//Prawdziwy rozmiar tablicy koincydencji
}	

//Zwraca wskaznik do obowiazujacej geometri danych. NULL oznacza dane nie-zgeometryzowane	
geometry_base* getgeometry()
	{ 
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy	
	return my_geometry.get_ptr_val(); 
	}		  

iteratorh  reset()
//Umozliwia czytanie od poczatku
//tablicy lub wycinka
	{ 
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy	
	if(my_geometry)
		return my_geometry->make_global_iterator();
		else
		return NULL;
	}

void close(iteratorh& p)
//Wymuszony koniec iteracji
	{ 
	if(my_geometry)
		my_geometry->destroy_iterator(p);
	}

double get(iteratorh& p)
//Daje nastepna z nn*mm liczb!!! 
	{
	double ret=0;
	assert(p!=NULL);
	size_t pom=my_geometry->get_next(p);

	if(pom!=ULONG_MAX)
		ret=_get(pom);
		else
		ret=miss;
	
	return ret;
	}

double get(size_t index)
//Przetwarza index uzyskany z geometri na jedna z nn*mm liczb
	{ 
#ifdef CAREFULLY_GET //Raczej niepotrzebne bo robi to juz i get_geometry() i bounds() i get_size();
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy			
#endif
	assert(index<get_size());
	return _get( index );
	}	

};

typedef coincidention_source<data_source_base> generic_coincidention_source;


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