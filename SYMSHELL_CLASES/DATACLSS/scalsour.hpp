/// @file
/// @brief  Konkretne klasy źródeł — dostęp do danych w tablicach i zmiennych
/// @date 2026-05-04 (modified)
// ********************************************************************************************************************
//
#ifndef __SCALAR_SOUR_HPP__
#define __SCALAR_SOUR_HPP__

#include "datasour.hpp"

/// Klasa źródła przechowujacego i dajacego pojedyncza wartość.
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

//const T&  - - - TU ZMIENIC GDY source_base stanie szablonem - mogą być klopoty z MISSING VALUE
double   get()  //Uproszczona wersja get() bo i tak jest tylko jedna wartość
    { return val;}

};

template<class Typek>
void  scalar_source<Typek>::change_val(const Typek& next)
    {
      val=next;
      //Uaktualnienie zakresu min-max
      if(this->CheckMinMax) //this-> dodane ze względu na problemy z GCC (07.2011)
        {
        if(val>this->y_max) this->y_max=val;
        if(val<this->y_min) this->y_min=val;
        }
    }

//Klasa źródła dajacego pojedyncza wartość czytana przez wskaźnik
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
    //Ile elementów 0, czy1,wartość minimalna i maksymalna
    //sczytane z wewnetrznych pol, w razie koniecznosci rozszerzone
    //o wartość aktualna. Jeśli pominie się wywołanie bounds to
    //zakres min-max może nie odzwierciedlac prawdziwej zmiennosci
    {
      if(ptr)
        {
        N=1;
        if(this->CheckMinMax)	//this-> dodane ze względu na problemy z GCC (07.2011)
            {
            if(*ptr>this->y_max) this->y_max=*ptr;
            if(*ptr<this->y_min) this->y_min=*ptr;
            }
        }
        else N=0;
      min=this->y_min;
      max=this->y_max;
    }

virtual iteratorh   reset()
    //Umozliwia czytanie od poczatku - iteratorh jest uchwytem iteratora
    //domyślnie z obiektu Source, ale czasem nie
    { return (ptr?(iteratorh)1:(iteratorh)0);}

void  change_ptr(T* next)
    //Zmienia wskaźnik a wartości mogą się zmieniać niezaleznie
    { ptr=next; }

//const T&  - - - TU ZMIENIC GDY source_base stanie szablonem
double   get()  //Uproszczona wersja get() bo i tak jest tylko jedna wartość. Musi być double a nie T bo miss jest typu double!!!
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

//Klasa źródła dajacego pojedyncza wartość czytana przez wywołanie
//bezparametrowej funkcji lub statycznej metody
template<class RET>
class ptr_to_fuction_source:public  template_scalar_source_base<RET>
//-----------------------------------------------------------------
{
typedef RET (*F)(void);
protected:
F ptr; //Jest wskaźnikiem do funkcji bezparametrowej
public:
//Constructor
ptr_to_fuction_source(F ini,const char* title,const RET& min=0,const RET& max=0):
       ptr(ini),
       template_scalar_source_base<RET>(title,min,max)
    {}

//Accesors
virtual void  bounds(size_t& N,double& min,double& max)
    //Ile elementów 0, czy1,wartość minimalna i maksymalna
    //sczytane z wewnetrznych pol, w razie koniecznosci rozszerzone
    //o wartość aktualna. Jeśli pominie się wywołanie bounds to
    //zakres min-max może nie odzwierciedlac prawdziwej zmiennosci
    {
      if(ptr)
        {
        N=1;
        RET r=ptr();
        if(this->CheckMinMax)
            {
            if(r>this->y_max) this->y_max=r;
            if(r<this->y_min) this->y_min=r;
            }
        }
        else N=0;
      min=this->y_min;
      max=this->y_max;
    }

virtual iteratorh   reset()
    //Umozliwia czytanie od poczatku - iteratorh jest uchwytem iteratora
    //domyślnie z obiektu Source, ale czasem nie
    { return (ptr?(iteratorh)1:(iteratorh)0);}

void  change_ptr(F next)
    //Zmienia wskaźnik a wartości mogą się zmieniać niezaleznie
    { ptr=next; }

//const T&  - - - TU ZMIENIC GDY source_base stanie szablonem
const double   get()  //Uproszczona wersja get() bo i tak jest tylko jedna wartość. Musi być double a nie T bo miss jest typu double!!!
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

/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*  Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk              */
/*        MAIL: borkowsk@iss.uw.edu.pl                               */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
#endif





