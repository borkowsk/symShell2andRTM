#ifndef __ARRAY_SOUR_HPP__
#define __ARRAY_SOUR_HPP__
// Konkretne klasy zrodel - dostep do danych w tablicach liniowych
//--------------------------------------------------------------------
//--------------------------------------------------------------------
#include "datasour.hpp"

//Klasa przekazujaca kolejne dane z liniowej tablicy. 
//Dane w tablicy moga sie zmieniac, ale raczej nie w trakcie iteracji ;)
template<class T>
class array_source:public linear_source_base
//--------------------------------------------------------
{
T* arra;
public:
// Constructor 
array_source(size_t iN,T* iarray,const char* itit):
	linear_source_base(iN,itit),arra(iarray)
	{
        miss=default_missing<T>();//!!! BARDZO WAZNE DLA SZABLON�W
    }
//Others method
void set_source(size_t NewN,T* iarray)
    {
        arra=iarray;
        linear_source_base::N=NewN;
    }
void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
	{
	num=N;	
	if(ymin<ymax)//Sa dane
		{
		min=ymin;
        max=ymax;
		return;
		}

	//Nie sa dane wiec probkujemy
    min=wbrtm::limit<T>::Max();
	max=wbrtm::limit<T>::Min();
	for(size_t i=0;i<N;i++)
		{
		if(min>arra[i]) min=arra[i];
		if(max<arra[i]) max=arra[i];
		}
	}

double get(iteratorh& ptr_to_iterator)
//Daje nastepna z N liczb!!! 
	{
	assert(ptr_to_iterator!=NULL);
	return arra[ _next(ptr_to_iterator) ];
	}

double get(size_t index)//Przetwarza index uzyskany z geometri
	{ //na wartosc z serii, o ile jest mozliwe czytanie losowe	
	assert(index<get_size());
	return arra[ index ];
	}	

};


//Klasa przekazujaca kolejne dane z liniowej tablicy struktur
//za pomoca wskaznikow do skladowych
template<class STRUCT_T,class FIELD_T>
class struct_array_source:public linear_source_base
//--------------------------------------------------------
{
typedef FIELD_T STRUCT_T::* TYP_POLA;
STRUCT_T* arra;//Tablica indywiduow
TYP_POLA  skladowa;//Wskaznik do skladowej indywiduum
public:

// Constructor 
struct_array_source(size_t iN,
                    STRUCT_T* iarray,
                    TYP_POLA iskladowa,
                    const char* itit):
	linear_source_base(iN,itit),arra(iarray),skladowa(iskladowa)
	{
        miss=default_missing<FIELD_T>();//!!! BARDZO WAZNE DLA SZABLON�W
    }
//Others methods    
void set_source(size_t NewN,STRUCT_T* iarray)
    {
        arra=iarray;
        linear_source_base::N=NewN;
    }
void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
	{
	num=N;	
    //cerr<<this->name()<<" ?\n";
	if(ymin<ymax)//Sa dane
		{//cerr<<"???\n";        
		min=ymin;max=ymax;
		return;
		}    
	//Nie sa dane wiec probkujemy
    min=wbrtm::limit<FIELD_T>::Max();
	max=wbrtm::limit<FIELD_T>::Min();
	for(size_t i=0;i<N;i++)
		{
        FIELD_T pom=arra[i].*skladowa;
        if(pom==miss) continue; //Skip missing values!!!
		if(min>pom) min=pom;
		if(max<pom) max=pom;
		}
	}

double get(iteratorh& ptr_to_iterator)
//Daje nastepna z N liczb!!! 
	{
	assert(ptr_to_iterator!=NULL);
    size_t pom=_next( ptr_to_iterator );
    if(pom!=ULONG_MAX)
    {
        double val=arra[ pom ].*skladowa; 
        return val;
    }
    else
        return miss;
	}

double get(size_t index)//Przetwarza index liniowy
	{ //na wartosc z serii, o ile jest mozliwe czytanie losowe	
                           //assert(index<get_size());
    if(index<get_size())
        return arra[ index ].*skladowa;
    else
        return miss;
	}	

};

//Klasa przekazujaca kolejne dane z liniowej tablicy struktur
//za pomoca wskaznikow do skladowych
template<class STRUCT_T,class FIELD_T>
class ptr_to_struct_array_source:public linear_source_base
//--------------------------------------------------------
{
typedef FIELD_T STRUCT_T::* TYP_POLA;
STRUCT_T** arra;//Tablica wskaznikow do indywiduow
TYP_POLA  skladowa;//Wskaznik do skladowej indywiduum
public:
// Constructor 
ptr_to_struct_array_source(size_t iN,
                           STRUCT_T** iarray,
                           TYP_POLA iskladowa,
                           const char* itit):
	linear_source_base(iN,itit),arra(iarray),skladowa(iskladowa)
	{
        miss=default_missing<FIELD_T>();//!!! BARDZO WAZNE DLA SZABLON�W
    }
//Others method
void set_source(size_t NewN,STRUCT_T** iarray)
    {
        arra=iarray;
        linear_source_base::N=NewN;
    }
void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
	{
	num=N;	
	if(ymin<ymax)//Sa dane
		{
		min=ymin;max=ymax;
		return;
		}
	//Nie sa dane wiec probkujemy
    min=wbrtm::limit<FIELD_T>::Max();
	max=wbrtm::limit<FIELD_T>::Min();
	for(size_t i=0;i<N;i++)
		{
		if(min>arra[i]->*skladowa) min=arra[i]->*skladowa;
		if(max<arra[i]->*skladowa) max=arra[i]->*skladowa;
		}
	}

double get(iteratorh& ptr_to_iterator)
//Daje nastepna z N liczb!!! 
	{
	assert(ptr_to_iterator!=NULL);
    size_t pom=_next( ptr_to_iterator );
    if(pom!=ULONG_MAX)
    {
        double val=arra[ pom ]->*skladowa; 
        return val;
    }
    else
        return miss;
	}

double get(size_t index)//Przetwarza index liniowy
	{ //na wartosc z serii, o ile jest mozliwe czytanie losowe	
	assert(index<get_size());
	return arra[ index ]->*skladowa;
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



