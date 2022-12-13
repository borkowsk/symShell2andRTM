#ifndef __SCALAR_SOUR_HPP__
#define __SCALAR_SOUR_HPP__
// Konkretne klasy zrodel - dostep do danych w tablicach i zmiennych
//--------------------------------------------------------------------
//--------------------------------------------------------------------
#include "datasour.hpp"

//Klasa zrodla przechowujacego i dajacego pojedyncza wartosc
template<class Typek>
class scalar_source:public  template_scalar_source_base<Typek>
//-----------------------------------------------------------------
{
protected:
Typek val;
public:
//Constructor
scalar_source(const Typek& ini,const char* title,
					  const Typek& min=0,const Typek& max=0):
	  template_scalar_source_base<Typek>(title,min,max)
	{
            change_val(ini);
	}

//Accesors
void  change_val(const Typek& next);

//const T&  - - - TU ZMIENIC GDY source_base stanie szablonem - moga byc klopoty z MISSING VALUE
const double   get()  //Uproszczona wersja get() bo i tak jest tylko jedna wartosc
	{ return val;}

};

template<class Typek>
void  scalar_source<Typek>::change_val(const Typek& next)
	{
	  val=next;
	  //Uaktualnienie zakresu min-max
	  if(this->CheckMinMax) //this-> dodane ze wzglêdu na problemy z GCC (07.2011)
		{
		if(val>this->ymax) this->ymax=val;
		if(val<this->ymin) this->ymin=val;
		}
	}

//Klasa zrodla dajacego pojedyncza wartosc czytana przez wskaznik
template<class T>
class ptr_to_scalar_source:public  template_scalar_source_base<T>
//-----------------------------------------------------------------
{
protected:
const T* ptr;
public:
//Constructor
ptr_to_scalar_source(const T* ini,const char* title,const T& min=0,const T& max=0):
	   template_scalar_source_base<T>(title,min,max)
	{ ptr=ini;}

//Accesors
virtual void  bounds(size_t& N,double& min,double& max)
	//Ile elementow 0 czy 1,wartosc minimalna i maksymalna
	//sczytane z wewnetrznych pol, w razie koniecznosci rozszerzone
	//o wartosc aktualna. Jesli pominie sie wywolanie bounds to
	//zakres min-max moze nie odzwierciedlac prawdziwej zmiennosci
	{
	  if(ptr)
		{
		N=1;
		if(this->CheckMinMax)//this-> dodane ze wzglêdu na problemy z GCC (07.2011)
			{
			if(*ptr>this->ymax) this->ymax=*ptr;
			if(*ptr<this->ymin) this->ymin=*ptr;
			}
		}
		else N=0;
	  min=this->ymin;
	  max=this->ymax;
	}

virtual iteratorh   reset()
	//Umozliwia czytanie od poczatku - iteratorh jest uchwytem iteratora
	//domyslnie z obiektu Source, ale czasem nie
	{ return (ptr?(iteratorh)1:(iteratorh)0);}

void  change_ptr(T* next)
	//Zmienia wskaznik a wartosci moga sie zmieniac niezaleznie
	{ ptr=next; }

//const T&  - - - TU ZMIENIC GDY source_base stanie szablonem
const double   get()  //Uproszczona wersja get() bo i tak jest tylko jedna wartosc. Musi byc double a nie T bo miss jest typu double!!!
    {
    if(ptr==NULL)
        return data_source_base::get_missing();
    else
        if(*ptr==data_source_base::get_missing())
            return data_source_base::get_missing();
        else
            return *ptr;
    }

};

//Klasa zrodla dajacego pojedyncza wartosc czytana przez wywolanie
//bezparametrowej funkcji lub statycznej metody
template<class RET>
class ptr_to_fuction_source:public  template_scalar_source_base<RET>
//-----------------------------------------------------------------
{
typedef RET (*F)(void);
protected:
F ptr;//Jest wskaznikiem do funkcji bezparametrowej
public:
//Constructor
ptr_to_fuction_source(F ini,const char* title,const RET& min=0,const RET& max=0):
       ptr(ini),
	   template_scalar_source_base<RET>(title,min,max)
	{}

//Accesors
virtual void  bounds(size_t& N,double& min,double& max)
	//Ile elementow 0 czy 1,wartosc minimalna i maksymalna
	//sczytane z wewnetrznych pol, w razie koniecznosci rozszerzone
	//o wartosc aktualna. Jesli pominie sie wywolanie bounds to
	//zakres min-max moze nie odzwierciedlac prawdziwej zmiennosci
	{
	  if(ptr)
		{
		N=1;
        RET r=ptr();
		if(this->CheckMinMax)
			{
			if(r>this->ymax) this->ymax=r;
			if(r<this->ymin) this->ymin=r;
			}
		}
		else N=0;
	  min=this->ymin;
	  max=this->ymax;
	}

virtual iteratorh   reset()
	//Umozliwia czytanie od poczatku - iteratorh jest uchwytem iteratora
	//domyslnie z obiektu Source, ale czasem nie
	{ return (ptr?(iteratorh)1:(iteratorh)0);}

void  change_ptr(F next)
	//Zmienia wskaznik a wartosci moga sie zmieniac niezaleznie
	{ ptr=next; }

//const T&  - - - TU ZMIENIC GDY source_base stanie szablonem
const double   get()  //Uproszczona wersja get() bo i tak jest tylko jedna wartosc. Musi byc double a nie T bo miss jest typu double!!!
{
    if(ptr==NULL)
        return data_source_base::get_missing();
    else
    {
        RET r=ptr();
        if(r==data_source_base::get_missing())
            return data_source_base::get_missing();
        else
            return r;
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




