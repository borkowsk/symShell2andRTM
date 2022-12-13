///////////////////////////////////////////////////////////////////////////
// Rozne klasy dwuzrodlowych filtrow danych
///////////////////////////////////////////////////////////////////////////
#ifndef __BIFILTSOUR_HPP__
#define __BIFILTSOUR_HPP__

//#include "math.h" /*DLA FILTROW */
#include "bifilter.hpp"

//Prosta klasa SUMUJACA dwa zrodla bez uzycia cache'a
class summ_source:public bi_filter_source_base
//----------------------------------------------------
{
public:
summ_source(data_source_base* ini1=NULL,
			data_source_base* ini2=NULL,
			const char* format="SUMM(%s , %s)"):
	  bi_filter_source_base(ini1,ini2,format)
	  {}
	  
//DOSTEP DO DANYCH:
//UWAGA: Zlozona iteracja wykonuje sie do konca krotszego ze zrodel.
//--------------------------------------------------------------------

//implementacja decyzji o wartosci minimalnej i maksymalnej
virtual void  _bounds(double& min1,double& max1,
					  double& min2,double& max2,
					  double& min,double& max)
	{
	//Jesli jest ustawione to zostawiamy
	if(ymin<ymax)
		{
		min=ymin;
		max=ymax;
		return;
		}
	//Jesli nie ustawione to liczymy
	//Wersja prowizoryczna.
	//Naprawde powinien byc cache wartosci 
	min=min1<min2?min1:min2;
	max=max1+max2;
	}

//Implementacja konkretnej operacji - musi sprawdzac czy 
//ktoras z wartosci lub obie nie sa "missing"
virtual double _get(double val1,double val2)
	{
	if(FromFirstIsMissing(val1) || FromSecondIsMissing(val2))
			return miss;
			else
			return val1+val2;
	}

};

//Klasa zwracajaca wartosci z drugiego zrodla jesli pierwsze,
//traktowane jako warunek zwraca wartosc nie "missing"
class if_then_source:public bi_filter_source_base
//---------------------------------------------------
{
    geometry_base* the_geom;
public:
if_then_source(data_source_base* ini1=NULL,
			data_source_base* ini2=NULL,
			const char* format="IF %s THEN %s ;"):
	  bi_filter_source_base(ini1,ini2,format),the_geom(reinterpret_cast<geometry_base*>(ULONG_MAX))
	  {}
	  
//DOSTEP DO DANYCH:
//UWAGA: Liniowa iteracja wykonuje sie do konca krotszego ze zrodel.
//--------------------------------------------------------------------

//implementacja decyzji o wartosci minimalnej i maksymalnej
virtual void  _bounds(double& min1,double& max1,
					  double& min2,double& max2,
					  double& min,double& max)
	{
	//Jesli jest ustawione to zostawiamy
	if(ymin<ymax)
		{
		min=ymin;
		max=ymax;
		return;
		}
	
	//Jesli nie ustawione to bierzemy z drugiego zrodla bo jego
	//wartosci faktycznie trafiaja na wyjscie
	min=min2;
	max=max2;
    the_geom=reinterpret_cast<geometry_base*>(ULONG_MAX);//Zeby znowu sprawdzic jak bedzie potrzebne
	}

//Implementacja konkretnej operacji - musi sprawdzac czy 
//ktoras z wartosci lub obie nie sa "missing"
virtual double _get(double val1,double val2)
	{
	//Jesli pierwsza seria ma "missing" to zwracamy missing
	//==Realizacja warunku
	if(FromFirstIsMissing(val1) ) 
            return miss;
	
	//Jesli druga seria ma i tak missing to konwertujemy
	//wartosc na lokalne "miss", a jesli wartosc jest wlasciwa
	//to ja zwracamy
	if(FromSecondIsMissing(val2))
			return miss;
			else
			return val2;
	}

virtual geometry_base* getgeometry()
//Powinien zwracac wskaznik do obowiazujacej geometri danych
//Oba zrodla musz miec ta sama geometrie
{ 
    if(the_geom!=reinterpret_cast<geometry_base*>(ULONG_MAX))
    {
        return the_geom;
    }
    else
    {
        if((the_geom=Source2->getgeometry())==Source->getgeometry())
            return the_geom;
        else
            return the_geom=NULL;
    }
}

};


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


