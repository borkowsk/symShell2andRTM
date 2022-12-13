///////////////////////////////////////////////////////////////////////////
// Filtr liczacy liczebnosc klas serii i pochodne statystyki
// WERSJA PIERWOTNA - LICZY TYLKO HISTOGRAMY CALKOWITO-LICZBOWE 
// Z RUCHOM¥ LICZB¥ KLAS.
///////////////////////////////////////////////////////////////////////////
#ifndef __HISTOGRAM_SOUR_HPP__
#define __HISTOGRAM_SOUR_HPP__
#include "statsour.hpp"

template<class DATA_SOURCE>
class histogram_source:public basic_statistics_source<DATA_SOURCE>
//------------------------------------------------------------------------------
{
protected:
size_t N;//Number of Class;
wb_dynarray<unsigned long> arra;

// Przemieszcza iterator o jednostke. Zeruje jesli koniec tablicy
size_t _next(iteratorh& p)
{
	assert(p!=NULL);//Nie wolno wywolac dla NULL
	size_t pom=((size_t)p)-1;
	if(pom+1>=N)
		p=NULL;
	else
		p=(iteratorh)(pom+2);
	return pom;	
}


int _calculate() //Zwraca 1 jesli musial przeliczyc
{
	if(!basic_statistics_source<DATA_SOURCE>::_calculate()) 
		return 0;
	
	double Entropy=0;
	
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
	if(!arra.IsOK()) //blad alokacji - za malo/za duzo?
		goto ERROR;

	for(i=0;i<KL;i++)
		arra[i]=0;
	/*//.....ATRAPA.....
	for(...
		arra[i]=smin+i;
	ymin=smin;
	ymax=smax;
	*/

	//PETLA ZLICZANIA
	iteratorh Ind=Source->reset();
	source_miss=Source->get_missing();	
	size_t Licz=0;
	for(i=0;i<SN;i++)
		{
		double pom=Source->get(Ind);
		if(!FromSourceIsMissing(pom))
			{	
			Licz++;
			pom-=smin;//Przesuniecie
					 assert(pom<SIZE_MAX);
			arra[size_t(pom)]++;//Takie to sobie. Trzeba przetrawic i poprawic. WARNING jak najbardziej.
			}
		}
	Source->close(Ind);	

	//PETLA	MIN/MAX
	ymin=DBL_MAX;
	ymax=0;	
	size_t licz_klasy=0;
	size_t minp=0,maxp=0;

	for(i=0;i<KL;i++)
	{
		double pom=arra[i];
		
		if(pom>0)
			licz_klasy++;

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

		//Liczenie skladowych entropi
		double qi=pom/double(Licz);

		//Powiekrzenie sumy, tam gdzie nie jest to puste skrzyzowanie
		if(qi>0)
			Entropy+=qi*log(qi);
	}

	if(table[6]!=NULL)
		{
		table[6]->change_val(ymax);
		}

	if(table[7]!=NULL)
		{
		table[7]->change_val(licz_klasy);
		}

	if(table[8]!=NULL)
		{
		table[8]->change_val(maxp+smin+0.5);//0.5 bo srodek przedzialu calkowitego
		}

	if(table[9]!=NULL)
		{
		table[9]->change_val(-Entropy);
		}

	if(table[10]!=NULL)
		{
		table[10]->change_val(-Entropy/log(double(KL)));
		}

	return 1;
	}//Musial przeliczyc
	
ERROR:
	if(table[10]=NULL)
		table[10]->change_val(table[9]->get_missing());	
	if(table[9]=NULL)
		table[9]->change_val(table[9]->get_missing());
	if(table[8]!=NULL)
		table[8]->change_val(table[8]->get_missing());
	if(table[7]!=NULL)
		table[7]->change_val(table[7]->get_missing());
	if(table[6]!=NULL)
		table[6]->change_val(table[6]->get_missing());
	arra.dispose();
	ymin=ymax=0;
	return 1;
}

public:
scalar_source<double>*      MainClass(const char* format="MainClass(%s)")	
{
	return GetMonoSource(6,format);
}

scalar_source<double>*      NumOfClass(const char* format="NumOfClass(%s)")	
{
	return GetMonoSource(7,format);
}

scalar_source<double>*      WhichMain(const char* format="WhichMain(%s)")	
{
	return GetMonoSource(8,format);
}

scalar_source<double>*      Entropy(const char* format="S(%s)")	
{
	return GetMonoSource(9,format);
}

scalar_source<double>*      NormEntropy(const char* format="nS(%s)")	
{
	return GetMonoSource(10,format);
}

	
	histogram_source(DATA_SOURCE* ini=NULL,
		size_t NumberOfClass=-1,//-1 oznacza tryb calkowitoliczbowy
		sources_menager_base* MyMenager=NULL,
		size_t table_size=11/*BEZ ZAPASU*/,
		const char* format="HISTOGRAM(%s)"):
	N(NumberOfClass),
		basic_statistics_source<DATA_SOURCE>(ini,MyMenager,table_size,format) 
	{}
	
	~histogram_source(){}

// Methods
size_t get_size()
{ 
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy	
	return arra.get_size();
}	


void all_subseries_required()//Alokuje i ewentualnie rejestruje w menagerze wszystkie serie
{
	basic_statistics_source<DATA_SOURCE>::all_subseries_required();
	//MAX CLASS
	MainClass();
	WhichMain();
	NumOfClass();
	Entropy();
}

iteratorh  reset()
//Umozliwia czytanie od poczatku
{ 
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy
	return (iteratorh)1;
}

void close(iteratorh& p)
{
	p=NULL;
}

void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
{
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy
	num=get_size();	
	min=ymin;max=ymax;
}

double get(iteratorh& ptr_to_iterator)
//Daje nastepna z N liczb!!! 
{
	assert(ptr_to_iterator!=NULL);
	return arra[ _next(ptr_to_iterator) ];
}

double get(size_t index)//Przetwarza index uzyskany z geometri
{ //na wartosc z serii, o ile jest mozliwe czytanie losowe	
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy	
	assert(index<get_size());
	return arra[ index ];
}	


};

typedef histogram_source<data_source_base> generic_histogram_source;


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