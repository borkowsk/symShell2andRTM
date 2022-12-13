///////////////////////////////////////////////////////////////////////////
// Podstawowa klasa dla ko-filtrow statystycznych - statystyk z dwu serii
///////////////////////////////////////////////////////////////////////////
#ifndef __CO_STATSOUR_HPP__
#define __CO_STATSOUR_HPP__

#include "math.h" /*DLA FILTROW */
#include "datasour.hpp"
#include "bifilter.hpp"
#include "sourmngr.hpp"

//KLasa liczaca podstawowe parametry statystyczne dla dwu zrodel - np korelacje
//Parametry sa podawne w arbitralnej kolejnosci lub poprzez jednowartosciowe
//zrod³a poœrednie.
//template<class DATA_SOURCE1,class DATA_SOURCE2>
class co_statistics_source:public bi_filter_source_base//public template_bi_filter<DATA_SOURCE1,DATA_SOURCE2>
//------------------------------------------------------------------------------
{
protected:
	class local_scalar_source:public scalar_source<double>
	//Nieco sztuczna klasa zrodla jednoelementowego z powiadamianiem
	//wlasciwej klasy "filtra podstawowych statystyk" ze beda potrzebne
	//aktualne dane. W klasach potomnych na pewno bezpiecznie mozna podstawic
	//virtualna metode get().
	{
	co_statistics_source* Father;
	
	public:
	local_scalar_source(co_statistics_source* father,const char* title):
		Father(father),
		scalar_source<double>(0,title)
		{
		  assert(local_scalar_source::Father!=NULL);	
		}
	
	//Metody ktore powiadamiaja ze moze byc potrzebna aktualizacja
    //-----------------------------------------------------------------
	void  bounds(size_t& N,double& min,double& max)
		{ 
		Father->check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
		Father->_calculate();//Sprawdza czy nie trzeba policzyc u ojca i ewentualnie liczy
		scalar_source<double>::bounds(N,min,max);
		}
	
	iteratorh   reset()
		{ 		
		Father->check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
		Father->_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy		
		return scalar_source<double>::reset();
		}
	
	const double   get()  
		{ 				
		Father->check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
		Father->_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy
		return scalar_source<double>::get();
		}

	};
	
friend class co_statistics_source::local_scalar_source;

private:
sources_menager_base*			sour_menager;
int need_calculation;

//double Sigma(double sum,double sumSqr,unsigned N)
//	{return sqrt(   ( sumSqr- (sum*sum)/N )  /  (N-1)  );}

protected:
wb_dynarray< scalar_source<double>* > table;//Tablica zrodel jednowartosciowych
int check_version() 
	//Nadpisuje virtualna metode klasy macierzystej:
	//Sprawdza czy zmienily sie dane w zrodle 
	//i znakuje koniecznosc liczenia aktualnej wartosci w metodzie _calculate()
	{
	int ret=bi_filter_source_base::check_version();
	if(ret==1)//Nowe dane
		{
		need_calculation=1;//Znacznik liczenia
		//Znakuje zmiane w podzrodlach na wypadek gdyby
		//nie obslugiwal ich menager danych.
		size_t table_size=table.get_size();		
		for(size_t i=0;i<table_size;i++)
				if(table[i]!=NULL)
					table[i]->update_version_from(this);				
		}
	return ret;
	}

virtual int _calculate() //Zwraca 1 jesli musial przeliczyc
{
if(!need_calculation) return 0;
need_calculation=0;

if(table[0]==NULL && table[1]==NULL) return 1;//Sam nie liczy ale moze klasa potomna

data_source_base* source1=get_first_source();
data_source_base* source2=get_second_source();
double min,max,Correlation=get_missing();
size_t N1,N2,Pairs=0;
source1->bounds(N1,min,max);
source2->bounds(N2,min,max);

if(N1<=0 || N2<=0) goto ERROR;//Nie da sie dalej liczyc

{//LICZENIE PAR i SREDNICH
double X_s=0,Y_s=0,summ1=0,summ2=0,summ3=0;
   
iteratorh Ind1=source1->reset(); 
iteratorh Ind2=source2->reset();
while(Ind1 && Ind2) //Puki oba iteratory
		{
		double pom1=source1->get(Ind1);
		double pom2=source2->get(Ind2);
		if((!FromFirstIsMissing(pom1)) &&
		  (!FromSecondIsMissing(pom2)) 
			)
			{	
			Pairs++;	
			X_s+=pom1;
   			Y_s+=pom2;
			}
		}
source1->close(Ind1);	
source2->close(Ind2);
   
if(table[1]==NULL || Pairs==0) goto ERROR;//Nie da sie albo nie trzeba dalej liczyc

X_s/=Pairs;	
Y_s/=Pairs;	
//liczenie skladowych korelacji   
Ind1=source1->reset(); 
Ind2=source2->reset();
while(Ind1 && Ind2) //Puki oba iteratory
		{
		double pom1=source1->get(Ind1);
		double pom2=source2->get(Ind2);
		if((!FromFirstIsMissing(pom1)) &&
		  (!FromSecondIsMissing(pom2)) 
			)
  			{
   			summ1+=(X_s-pom1)*(Y_s-pom2);
   			summ2+=(X_s-pom1)*(X_s-pom1);
   			summ3+=(Y_s-pom2)*(Y_s-pom2);
   			}
		}
source1->close(Ind1);	
source2->close(Ind2);
   
if(summ2<=0 || summ3<=0) goto ERROR;// Bo pierwiastek i dzielenie
Correlation=summ1/(sqrt(summ2)*sqrt(summ3));	
}//KONIEC LICZENIA

ERROR://Tu laduje przeskok obliczen
if(table[0]!=NULL)
		{
		table[0]->change_val(Pairs);		
		}

if(table[1]!=NULL)
		{
		table[1]->change_val(Correlation);		
		}

return 1;//Musial przeliczyc
}

scalar_source<double>*		GetMonoSource(size_t select,const char* format=NULL)
//Alokuje zrodla jednowartosciowe dla parametrow statystycznych
{
if(table[select]==NULL)//Jesli nie ma to alokuj
	{
	//Przygotuj title z formatu
	const char* sour_name1=get_first_source()->name();
	const char* sour_name2=get_second_source()->name();
	wb_pchar title(strlen(format)+strlen(sour_name1)+strlen(sour_name2)+10);
	sprintf(title.get_ptr_val(),format,sour_name1,sour_name2);
	scalar_source<double>* ptr=new local_scalar_source(this,title.get_ptr_val());	
	table[select]=ptr;
	//Moze sie nie zaalokowac - trzeba zareagowac na NULL
	//----------------------------------------------------
	if(ptr!=NULL && sour_menager!=NULL )
	//Jesli OK to trzeba zg³osic menagerowi danych!
		sour_menager->insert(ptr,1/*NOT MENAGE MEMORY*/);
	}
return table[select];
}

//Raczej nieuzywana (Bo bounds zwraca N=0) implementacja decyzji o wartosci minimalnej i maksymalnej
virtual void  _bounds(double& min1,double& max1,
					  double& min2,double& max2,
					  double& min,double& max)
	{
	//Jesli nie ustawione to przyjmujemu typowy zakres korelacji
	min=-1;
	max=1;
	}

//Raczej nieuzywane. 
virtual double _get(double val1,double val2)
	{
	return get_missing();
	}

public:
virtual size_t number_of_subseries()
	{
	return 2;//Ma dwa podzrodla
	}

virtual void all_subseries_required()//Alokuje i ewentualnie rejestruje w menagerze wszystkie serie
	{
	Pair();Corr();
	}

//Ilosc wspolnych par w obu zrodlach (tzn gdy oba nie sa "missing")
scalar_source<double>*      Pair(const char* format="Pair(%s,%s)")	{return GetMonoSource(0,format);}

//Korelacja miedzy dwoma zrodlami
scalar_source<double>*      Corr(const char* format="Corr(%s,%s)")	{return GetMonoSource(1,format);}

//CONSTRUCTION
co_statistics_source(data_source_base* ini1=NULL,
					 data_source_base* ini2=NULL,
					 sources_menager_base* MyMenager=NULL,
					 size_t table_size=0,
					 const char* format="CO_STATS(%s,%s)"):
	  bi_filter_source_base(ini1,ini2,format),
	  table(2+table_size),//Dwa obowiazkowe, reszta z klas potomnych
	  sour_menager(MyMenager),
	  need_calculation(1)
	{
	 for(size_t i=0;i<table.get_size();i++)
		table[i]=NULL;
	}

//destruction
~co_statistics_source()
	{
	size_t table_size=table.get_size();
	
	if(sour_menager!=NULL)
		{
		for(size_t i=0;i<table_size;i++)
			if(table[i]!=NULL)
				//Sa insertowane jako "not menaged" wiec tu tylko
				 sour_menager->replace(table[i]->name(),NULL);//...wywala z menagera
		}	

	for(size_t i=0;i<table_size;i++)
			if(table[i]!=NULL)
				delete table[i];//Sam sobie wywala z pamieci
	
	}

void link_sources_menager(sources_menager_base* NewMenager,int usun=1)
	//Wymienia dolaczonego menagera i linkuje mu podserie
	//Jesli "usun==1" to usuwa je najpierw z poprzedniego
	{
	size_t table_size=table.get_size();

	if(usun &&  sour_menager!=NULL)
		for(size_t i=0;i<table_size;i++)
			if(table[i]!=NULL)
				//Sa insertowane jako "not menaged" wiec tu tylko
				 sour_menager->replace(table[i]->name(),NULL);//...wywala z menagera	

	sour_menager=NewMenager;		
	if(sour_menager!=NULL)//Jesli nie NULL to umiesc
		for(size_t i=0;i<table_size;i++)
			if(table[i]!=NULL)
				sour_menager->insert(table[i],1/*NOT MENAGED*/);
	}


void  bounds(size_t& N,double& min,double& max)
	{
	check_version();//Tu moze byc update
	//bi_filter_source_base::bounds(N,min,max);
	min=max=0;
	N=0;//Nie mozna siegac do tych danych bezposrednio jako do seri
	}

iteratorh   reset()//Nie ma mozliwosci bezposredniego czytania
	{ check_version();return NULL;}

};

typedef co_statistics_source generic_co_statistics_source;


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