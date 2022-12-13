//			Definitions of basic data_sources class
//------------------------------------------------------------------------
//*////////////////////////////////////////////////////////////////////////
#ifndef __DATA_SOURCE_BASE_HPP__
#define __DATA_SOURCE_BASE_HPP__

#include <cassert>
#include <climits>
#include <cfloat>
#include <cstdio>

#include "wb_limits.hpp"
#include "wb_ptr.hpp"
#include "wb_clone.hpp"

#include "titleuti.hpp"
#include "rectgeom.hpp" //GEOMETRY INTERFACE

const unsigned ZAPAS_NA_CYFRY=(DBL_DIG*2); //Do wyświetlania: DBL_DIG+zapas na znaki i wykładnik

//Definicja INTERFACE'u źródła danych. Każde źródło musi mieć
//zaimplementowane takie metody, ale może mieć też inne.
class data_source_base
//-----------------------------------------------------------
{
long cur_step;	//Numer kolejnej wersji danych
long no_change;	//Od ilu kroków nie było zmiany
#if __GNUC__ >= 3
public:
#else
protected:
#endif
double miss;	//Missing value
double ymin;	//dany lub wydedukowany zakres Y.
double ymax;	//Jeśli jest dany, to nie należy go dedukować.

public:
typedef ::iteratorh iteratorh; //Skrót dla typu uchwytu iteratora.

//Liczy -INF wg IEEE -jako znacznik braku (może generować SIGPFE na części platform)
//static double inf();
//Liczy NAN wg IEEE -jako znacznik braku (użycie może generować SIGPFE na części platform)
//static double nan();

// accesors
//---------------------------------------

//Ustala missing value takie jakie chce user
void	set_missing(double imiss);

// Sprawdzanie czy Source->get nie dało missing
int		is_missing(double val);

// Zapewnia właściwa inicjacje i obsługę wartości "miss"
// Wystarczy wywołać raz, przed iteracją, a potem używać tylko
// is_missing() lub po prostu miss.
double	get_missing();

// Ustala min i max, żeby uniknął próbkowania danych.
// Podanie równych może ponownie włączać próbkowanie.
void	setminmax(double imin,double imax);

// OBSŁUGA WERSJI DANYCH
//----------------------------------

// Ustalanie informacji o wersji danych
virtual
void	new_data_version(int change=1,unsigned increment=1);

// Uaktualnia wersje wg podanego źródła i wtedy zwraca 1.
// Jeśli wersje są zgodne, to zwraca 0.
virtual
int		update_version_from(data_source_base* Source);

// numer wersji danych
virtual /* Czy virtual potrzebne ? */
long	data_version()		{ return cur_step; }

// Podaje, odd ilu wersji dane się nie zmieniły.
virtual /* Czy potrzebne ? */
long	how_old_data()		{ return no_change;}

// Restartuje versioning źródeł. W wypadku pod-źródeł powinna byc reimplementacja!
virtual
void	restart_counting()	{ cur_step=-1;no_change=0;}

// OBSŁUGA GEOMETRII SERII
//----------------------------------------

// Zwraca wskaźnik do obowiązującej geometrii danych.
// NULL oznacza dane nie-zgeometryzowane, wyłącznie z dostępem sukcesywnym.
virtual
geometry_base* getgeometry()	{ return NULL; }

//Przetwarza index uzyskany z geometrii
//na wartość z serii, o ile jest możliwe czytanie losowe.
//Domyślnie NIE JEST MOŻLIWE! - powoduje błąd wykonania.
virtual
double	get(size_t index_from_geometry);

//PURE virtual INTERFACE - need defined!!!
//-------------------------------------------

//Musi zwracać nazwę serii albo "" - NIE NULL!
virtual
const char* name()=0;

// Ile elementów, wartość minimalna i maksymalna
virtual
void	bounds(size_t& N,double& min,double& max)=0;

// Umożliwia czytanie sukcesywne od początku -
// 'iteratorh' jest uchwytem dla 'iterator'
virtual
iteratorh   reset()=0;

// Daje następną z N liczb!!! Po N-tej zwalnia iterator!
virtual
double	get(iteratorh&)=0;

// Zwalnia iterator, jeśli nie został zwolniony przez get(N)
virtual
void	close(iteratorh&)=0;

//CONSTRUCTION/DESTRUCTION
//---------------------------------

//Constructor
data_source_base():
		cur_step(-1),no_change(0),
		ymin(0),ymax(0),
		miss(default_missing<double>())
		{}

//Destructor
virtual ~data_source_base()
		{}

};

//ACCESSORS
//---------------------------------------

// Ustala missing value takie jakie chce user.
inline
void  data_source_base::set_missing(double imiss)
{
	miss=imiss;
}

// Sprawdzanie, czy get() nie dało missing.
inline
int data_source_base::is_missing(double val)
{
    if(val==miss)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// Zapewnia właściwą inicjację i obsługę wartości "miss"
// A le czy to "really" potrzebne?
inline
double data_source_base::get_missing()
{
    double tmp=default_missing<double>(); //Klasy szablonowe muszą to reimplementować
	if(memcmp(&miss,&tmp,sizeof(miss))!=0) // Jeśli ustawione lub już domyślne
		return miss;
		else
		return miss=tmp; //Zapewnia, że będzie ustawione na default_missing<...>()
}

// Ustala arbitralne min i max, żeby uniknąć próbkowania (dedukowania).
// Podanie równych włącza znowu próbkowanie.
inline
void  data_source_base::setminmax(double imin,double imax)
{
	assert( imin <= imax ); //Podanie równych włącza znowu próbkowanie (dedukcję)!!!
	ymin=imin;
	ymax=imax;
}


// OBSŁUGA WERSJI DANYCH
// ///////////////////////////////////////////////

//Ustalanie informacji o wersji danych
inline
void  data_source_base::new_data_version(int change,unsigned increment)
{
	cur_step+=increment;
	if(change) no_change=0;
		  else no_change+=increment;
	assert(cur_step>=no_change);
}

//Uaktualnia wersje wg podanego źródła i wtedy zwraca 1.
//Jeśli wersje są zgodne, to zwraca 0.
inline
int   data_source_base::update_version_from(data_source_base* Source)
{
	if(Source->data_version()>data_source_base::data_version()) //Tu może startować ewentualna propagacja
	{
		new_data_version(
			Source->how_old_data()==0?1:0,
			Source->data_version()-data_source_base::data_version()	//Żeby nie było rekurencji nie wprost
			);
		return 1;
	}
	else
		return 0;
}


// OBSŁUGA GEOMETRYZOWANYCH SERII
// ///////////////////////////////////

//Przetwarza index uzyskany z geometrii
//na wartość z serii, o ile jest możliwe czytanie losowe
inline
double data_source_base::get(size_t index_from_geometry)
{
	assert(!"Random access get() not implemented");
	return miss;
}

#endif
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego        */
/*        WWW:  http://www.iss.uw.edu.pl/~borkowsk/                  */
/*        MAIL: borkowsk@iss.uw.edu.pl                               */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/

