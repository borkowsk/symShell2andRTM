////////////////////////////////////////////////////////////////////////////////
//GEOMETRY - SPOSOB ORGANIZACJI AGENTOW W WARSTWIE
////////////////////////////////////////////////////////////////////////////////
//Geometria jest obiektem kt�ry potrafii opisac po�ozenie agent�w w warstwie i
// wzajemnie wzgledem siebie i przetworzyc je na liniowy indeks tablicy agent�w
// INTERFACE dla geometrii swiata symulacji
////////////////////////////////////////////////////////////////////////////////
#ifndef __GEOMBASE_HPP__
#define __GEOMBASE_HPP__

#include <stddef.h>
#include <assert.h>
#include <limits.h>
#include <float.h>
#include <string.h>

//#include "platform.hpp"
#include <iostream>

//#ifdef F U L L
//#define F U L L ((unsigned long)(-1)) //Moze byc w konflikcie z innymi definicjami FULL - np ((void*)0xffffffff)
//#endif

//Typ uchwytowy do iteratorow geometri
//---------------------------------------
typedef void* iteratorh; //Wersja stara ale jara :-)
/*
class geometry_base::iterator_base; //???Pomys�y na wersj� zabezpieczon�?
class iteratorh
//----------------
{
iterator_base* val;
public:
iteratorh():val(0){}
explicit iteratorh(unsigned long Init):val((iterator_base*)Init){}
void set(unsigned long Init){ val=(iterator_base*)Init;}
operator iterator_base* () {return val;}
operator void* () {return (void*)val;}
};
*/

class geometry_base 
//-----------------
{
public:
    enum my_full {FULL=((unsigned long)(-1))}; //Zamiast #define FULL

	//LOKALNE KLASY i STRUKTURY
	//-------------------------------------

	//Struktura dla wyrazania wspo�rzednych
	union coord
	//---------------
	{
		//struct{double X,Y,Z,T,U,V;};
		double C[6];
		coord()
		{	C[0]=C[1]=C[2]=C[3]=C[4]=C[5]=0;	} //initial values
		double& X(){ return C[0];}
		double& Y(){ return C[1];}
		double& Z(){ return C[2];}
		double& T(){ return C[3];}
		double& U(){ return C[4];}
		double& V(){ return C[5];}
		/* Czasem kompilator ma problem ktora wybrac - const czy nie const */
		double X()const { return C[0];}
		double Y()const { return C[1];}
		double Z()const { return C[2];}
		double T()const { return C[3];}
		double U()const { return C[4];}
		double V()const { return C[5];}
	};
		
	struct MD_info
	//-------------
	{	
		coord min;	//Najmniejsze wartosci dla kazdej wspolrzednej
		coord max;	//Najwieksze wartosci dla kazdej wspolrzednej	
	};
	
	struct view_info
	//---------------
	{
		coord pos;//Pozycja oka kamery
		coord dia;//Na jaka odleglosc chce widziec kamera	
		coord sst;//Sugerowany minimalny krok przesuwu
	};
	
	//Klasa bazowa dla wszystkich iteratorow geometrii
	class iterator_base
	//------------------
	{
	protected:
		size_t items;	//Ile elementow - trzeba ustawic!!!
		unsigned marker;//Dla sprawdzania ze to faktycznie iterator

	public:
		//Optymalizacja alokacji!!!
		  void* operator new (size_t s);
		  void  operator delete (void* p, size_t s);

		//Konstruktor ustawia marker
		iterator_base(size_t iite):
				marker(0xfedcba00),items(iite){}

	virtual	//Destruktor - na wszelki wypadek wirtualny
		~iterator_base()	{ marker=0;}

		  //Czy to na pewno iterator - np dla assercji
		  int is_iterator()	{ return marker==0xfedcba00;} 
		  
		  //Ile jest elementow w iteracji
		  size_t number_of_items(){ return items;}
		  
		  //Implementacja pobrania nastepnego elementu
		  virtual void _next(const geometry_base&,size_t& ret,size_t& end)=0;		  
	};

private:
int dimension;	//Ilosc wymiar�w geometri - zeby uzyc typu "coord"

protected:

//Sprawdza czy VMT i dimension sa takie same
int _compare_geometry_base(geometry_base* sec);
	
public:			  
//METODY INFORMACYJNE O
/////////////////////////////////////////////////////////////
//Informacja o ilosci wymiarow topologicznych geometrii - moze byc 0 jesli to pojecie nie ma sensu (np dla grafowych)
int			get_dimension() const {return dimension;}

virtual //Informacja o maksymalnej mozliwej odleglosci - potrzebna np dla "Spatial correlation"
double      get_max_distance() const =0; 

virtual //Informacja o odleglosci dwuch obiektow o okreslonych indeksach - dla "Spatial correlation" i wazenia oddzialywan
double      get_distance(size_t first,size_t second) const =0;
            
virtual //Informacja o rozmiarze uzytecznej przestrzeni     
MD_info*	get_info(MD_info* pom=NULL) const=0;

virtual //Informacja o polozeniu i zasiegu kamery dla view_iteratora
view_info*	get_view_info(view_info* pom=NULL) const=0;

virtual //Ustawianie polozeniu i zasiegu kamery dla view_iteratora. Zwraca 0 jesli OK
int			set_view_info(const view_info*)=0;

//POROWNANIA I OPERATORY
//----------------------------------------------

virtual //Mozna przedefiniowywac jesli sa powody!!!
int  compare(geometry_base& sec)
{
	return _compare_geometry_base(&sec);
}

int operator == (geometry_base& sec)
{
	return compare(sec)==0;
}

int operator != (geometry_base& sec)
{
	return compare(sec)!=0;
}

//METODY TWORZACE ITERATORY DO PRZECHODZENIA GEOMETRII - PURE VIRTUAL!!!
//-----------------------------------------------------------------------

//Tworzy iterator po obszarze wizualizacji 
virtual 
iteratorh make_view_iterator() const=0;

//Tworzy iterator po calosci (alokujac operatorem "new"), do likwidacji uzywa� destroy_iterator
virtual 
iteratorh make_global_iterator() const=0;

//Tworzy globalny iterator monte-carlo
virtual 
iteratorh make_random_global_iterator(size_t how_many=-1) const=0;

//Tworzy iterator po sasiadach - dist jako R we wszystkich kierunkach
virtual 
iteratorh make_neighbour_iterator(size_t center,size_t dist=1) const=0;

//Tworzy losowy iterator po sasiadach - dist jako R we wszystkich kierunkach
virtual 
iteratorh make_random_neighbour_iterator(size_t center,size_t dist=1,size_t how_many=-1) const=0;
        
//METODY UZYWAJACE ITERATOROW
//---------------------------------

//Zwraca indeks do aktualnego i przesuwa iterator.
//Zeruje iterator jesli koniec danych.Zwraca uFULL jesli missing...
size_t get_next(iteratorh& p) const;

//Likwiduje iterator, ktoremu nie dano dojsc do konca
virtual 
void destroy_iterator(iteratorh& p) const;

//CONSTRUCTION/DESTRUCTION
//--------------------------------
	geometry_base(int ini):
		dimension(ini){}

virtual
	~geometry_base(){}

};

//Tymczasem nazwa "geometry" zarezerwowana 
typedef geometry_base geometry;


//IMPLEMENTATION
////////////////////////

inline
int geometry_base::_compare_geometry_base(geometry_base* sec)
{	//Sprawdza czy VMT i dimension sa takie same
	return memcmp(this,sec,sizeof(geometry_base));
}

inline
size_t geometry_base::get_next(iteratorh& p) const
//Zwraca indeks do aktualnego i przesuwa iterator.
//Zeruje iterator jesli koniec danych.Zwraca uFULL jesli missing...
{
	size_t ret=FULL,end=0;
    
	iterator_base* pom=(iterator_base*)p;//NIELADNY CHWYT ALE DZIALA
    //iterator_base* pom1=(iterator_base*)(&p); assert(pom1==pom); --> A TAK NIE DZIALA
                                    assert(pom!=NULL);                                    
	                                assert(pom->is_iterator());
	
	//Wlasciwe odczytanie i przesuniecie iteratora
	pom->_next(*this,ret,end);
	if(end)
	{
		assert(pom->is_iterator());
		destroy_iterator(p);//Usuwany iterator 
	}
	
	return ret;
}


inline
void    geometry_base::destroy_iterator(iteratorh& p) const
//Likwiduje iterator, ktoremu nie dano dojsc do konca
{
	if(p==NULL) return;
	void* ptr=p;//Uwolnic sama warto��
	iterator_base* pom=(iterator_base*)ptr;//NIELADNY CHWYT ALE DZIALA
	assert(pom->is_iterator());            //choc dobrze sprawdzic czy faktycznie
	delete pom;//Usuwany iterator
	p=NULL;
}

inline //Informacja o maksymalnej mozliwej odleglosci - potrzebna np dla "Spatial correlation"
double      geometry_base::get_max_distance() const
{
    assert("Pure virtual method 'geometry_base::get_max_distance() const' used."==0);
    return 0;
}

inline //Informacja o odleglosci dwuch obiektow o okreslonych indeksach - dla "Spatial correlation" i wazenia oddzialywan
double     geometry_base::get_distance(size_t first,size_t second) const 
{
    assert("Pure virtual method 'geometry_base::get_distance() const' used."==0);
    return 0;
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

