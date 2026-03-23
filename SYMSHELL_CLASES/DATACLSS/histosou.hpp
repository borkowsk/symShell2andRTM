// /////////////////////////////////////////////////////////////////////////
// Filtr licz¹cy liczebnoœci klas serii i pochodne statystyki
// WERSJA PIERWOTNA — LICZY TYLKO HISTOGRAMY CA£KOWITOLICZBOWE
// Z RUCHOM¥ LICZB¥ KLAS.
// /////////////////////////////////////////////////////////////////////////
#ifndef __HISTOGRAM_SOUR_HPP__
#define __HISTOGRAM_SOUR_HPP__
#include "statsour.hpp"

template<class DATA_SOURCE>
class histogram_source:public basic_statistics_source<DATA_SOURCE>
//------------------------------------------------------------------------------
{
public:
    typedef basic_statistics_source<DATA_SOURCE> basics_;
    using basics_::table; //skrócony dostêp do tablicy danych klasy bazowej
protected:
size_t N;//Number of Class;
wb_dynarray<unsigned long> arra;

// Przemieszcza iterator o jednostkê. Zeruje, jeœli koniec tablicy
size_t _next(iteratorh& p)
{                                        assert(p!=NULL); //Nie wolno wywo³aæ dla NULL
	size_t pom=((size_t)p)-1;
	if(pom+1>=N)
		p=NULL;
	else
		p=(iteratorh)(pom+2);
	return pom;	
}


int _calculate() //@returns 1, jeœli musia³ przeliczyæ
{
	if(!basic_statistics_source<DATA_SOURCE>::_calculate()) 
		return 0;
	
	double Entropy=0;
	
	{//OBLICZANIE HISTOGRAMU
	assert(N==-1); //Tylko tryb integer-owy zaimplementowany

	size_t i;
	size_t SN,KL;
	double smin,smax;
    this->Source->bounds(SN,smin,smax);

	if(smax-smin<=double(size_t(-1))) //Czy w zakresie size_t
		KL=size_t(smax-smin)+1; //Ile jednostek ca³kowitych zakresu
    else
		goto ERROR;

	arra.alloc(KL);
	if(!arra.IsOK()) //b³¹d alokacji — za ma³o/za du¿o?
		goto ERROR;

	for(i=0;i<KL;i++)
		arra[i]=0;
	/*//.....ATRAPA.....
	for(...
		arra[i]=smin+i;
	ymin=smin;
	ymax=smax;
	*/

	//PÊTLA ZLICZANIA
	iteratorh Ind=this->Source->reset();
	this->source_miss=this->Source->get_missing();
	size_t Licz=0;
	for(i=0;i<SN;i++)
		{
		double pom=this->Source->get(Ind);
		if(!this->FromSourceIsMissing(pom))
			{	
			Licz++;
			pom-=smin;//Przesuniecie
					            assert(pom<SIZE_MAX);
			arra[size_t(pom)]++; //Takie to sobie. Trzeba przetrawiæ i poprawiæ. WARNING jak najbardziej.
			}
		}

    this->Source->close(Ind);

	//PÊTLA MIN/MAX
    this->ymin=DBL_MAX;
    this->ymax=0;

	size_t licz_klasy=0;
	size_t minp=0,maxp=0;

	for(i=0;i<KL;i++)
	{
		double pom=arra[i];
		
		if(pom>0)
			licz_klasy++;

		if(pom>this->ymax)
		{
            this->ymax=pom;
			maxp=i;
		}

		if(pom<this->ymin)
		{
            this->ymin=pom;
			minp=i;
		}

		//Liczenie skladowych entropi
		double qi=pom/double(Licz);

		//Powiêkszenie sumy, tam, gdzie nie jest to puste skrzy¿owanie
		if(qi>0)
			Entropy+=qi*log(qi);
	}

	if(basics_::table[6]!=NULL)
		{
		table[6]->change_val(this->ymax);
		}

	if(table[7]!=NULL)
		{
		table[7]->change_val(licz_klasy);
		}

	if(table[8]!=NULL)
		{
		table[8]->change_val(maxp+smin+0.5); //0.5 bo œrodek przedzia³u ca³kowitego
		}

	if(table[9]!=NULL)
		{
		table[9]->change_val(-Entropy);
		}

	if(table[10]!=NULL)
		{
		table[10]->change_val(-Entropy/log(double(KL)));
		}

	return 1; //Musial przeliczyæ
	}
	
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
    basics_::ymin=basics_::ymax=0;
	return 1;
}

public:
scalar_source<double>*      MainClass(const char* format="MainClass(%s)")	
{
	return basics_::GetMonoSource(6,format);
}

scalar_source<double>*      NumOfClass(const char* format="NumOfClass(%s)")	
{
	return basics_::GetMonoSource(7,format);
}

scalar_source<double>*      WhichMain(const char* format="WhichMain(%s)")	
{
	return basics_::GetMonoSource(8,format);
}

scalar_source<double>*      Entropy(const char* format="S(%s)")	
{
	return basics_::GetMonoSource(9,format);
}

scalar_source<double>*      NormEntropy(const char* format="nS(%s)")	
{
	return basics_::GetMonoSource(10,format);
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
    basics_::check_version(); //Uaktualnia te¿ wersje pod-Ÿród³a, jeœli trzeba
	_calculate(); //Sprawdza, czy nie trzeba policzyæ i ewentualnie liczy
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
    basics_::check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
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
    basics_::check_version(); //Uaktualnia te¿ wersje podzrodla jesli trzeba
	_calculate(); //Sprawdza czy nie trzeba policzyc i ewentualnie liczy
	num=get_size();	
	min=basics_::ymin;
    max=basics_::ymax;
}

double get(iteratorh& ptr_to_iterator)
//Daje nastepna z N liczb!!! 
{
	assert(ptr_to_iterator!=NULL);
	return arra[ _next(ptr_to_iterator) ];
}

double get(size_t index) //Przetwarza index uzyskany z geometrii
{ //na wartosc z serii, o ile jest mo¿liwe czytanie losowe
    basics_::check_version(); //Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate(); //Sprawdza, czy nie trzeba policzyc i ewentualnie liczy
	assert(index<get_size());
	return arra[ index ];
}


};

typedef histogram_source<data_source_base> generic_histogram_source;


#endif
/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */
