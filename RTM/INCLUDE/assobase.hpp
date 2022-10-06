#ifndef _ASSOC_BASE_HPP_
#define _ASSOC_BASE_HPP_
// Klasa bazowa do wykonywania tablic asocjacyjnych.
// Zarzadza tablica rekordow asocjujacych dwie unie "unitype"
// W klasach potomnych wystarczy inteligentnie uzyc tych pol
// w najprostrzy sposob do przechowywania skalarow i wskaznikow.
#include "unitype.hpp"
#include "arrays.hpp"

#ifdef __DJGPP__
#pragma interface
#endif

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

class assoitem
{
public:
unitype key;
unitype val;

assoitem()
{ clear();}
~assoitem()
{ clear();}
void clear()
{ key.ptr=NULL;val.s=-1;}
// OBLIGATORY
void finalise()
{ this->~assoitem(); }

friend ostream& operator << (ostream& o, const assoitem& a)
{ 
  o	<<' '
	<<a.key
	<<' '
	<<a.val;
  return o;
}

friend istream& operator >> (istream& i, assoitem& a)
{
	i>>a.key
	 >>a.val;
  return i;
}

};


class AssocKeyNotFoundExcp:public SearchingExcp
{
EXCP_NECESSARY(AssocKeyNotFoundExcp)
public:
AssocKeyNotFoundExcp(const char* fname=0,const int fline=0):
	SearchingExcp(fname,fline)
	{}
void PrintTo( ostream& o ) const ;	
};

/*
//Szablon wyjatku dla dowolnego klucza - za kosztowny
template<class K>
class AnyKeyNotFoundExcp:public AssocKeyNotFoundExcp
/////////////////////////////////////////////////////
{
char* what;
public:
AnyKeyNotFoundExcp(const K* iw,const char* fname=0,const int fline=0):
	what(0),
	AssocKeyNotFoundExcp(fname,fline)
	{
	what=new char[1024];
	ostrstream pom(what,1024);
	pom<<iw;
	}
~AnyKeyNotFoundExcp()
	{
	if(what) delete what;
	}
void PrintTo( ostream& o ) const 
	{
	o<<"Key ";
	if(what) o<<'"'<<what<<'"';
	SearchingExcp::PrintTo(o);
	}
};
*/


class assoc_base//:pure virtual base implementation class for assoc_tables
{
private:
int      _remove( size_t i ); //Uzywana do usuwania i-temu z tab
int      _insert( size_t i ); //Uzywana do zrobienia miejsca w tab
int      _search( const unitype, size_t& index );// Ustawia index do assoitem z tab,ret=1 jesli jest faktycznie
protected:
/* IMPLEMENTATION OF SEARCHING SUPPORT -  */
/* DIRECTLY FROM ARRAY_OF. DON't CHANGE */
pix 	 	_first() const { return tab.first(); }
pix 	 	_next(pix p) const{ return tab.next(p); }
void     	_destroy(pix& p) const { tab.destroy(p); }
int    	      	_remove(pix); // Remove item. Return 1 on succes
const unitype* 	_key(pix p) const ;
// return key for current object setting by  First,Next or Search
unitype* _value(pix p)
// return key for current object setting by  First,Next or Search
{
assoitem* pom=&tab.value(p);
return   &(pom->val);
}

protected:
array_of_class< assoitem > tab;// Tablica lacznikow
int    sorted;		       // jesli ==1 to tablica zawsze posortowana
int    reverse;	       // jesli ==1 to malejaco (Reverse order)

// M E T O D Y
size_t	  CurrSize() const 
{ 
	return tab.CurrSize(); 
}

void	  Truncate() 
{
	tab.Truncate(0); //Ale pozostaje juz zaalokowana i nie wywoluje destruktorow
}

// znajduje item lub go tworzy (make==1), zwraca NULL
assoitem* _Search(/*const*/unitype,int make=1); // ALbo wskaznik do znalezionego asso
// usuwa item. Przedtem wywoluje "before_deletion" zeby klasa potomna
int       _Remove(/*const*/unitype); // mogla zareagowac, choc nie musi koniecznie

/* DO ZAIMPLEMENTOWANIA W KLASACH POTOMNYCH */
virtual	//Porownuje dwa klucze. Zwraca 0 gdy ==, 1 gdy first>second
int 	  Compare(const unitype first,const unitype second)=0;
virtual //Wywolywana zawsze przed usunieciem i-temu z tab.
	//UWAGA .ptr o wartosci NULL lub FULL oznacza oczyszczony unitype!
void      BeforeDeletion( assoitem& )=0;
virtual //Uzywany do wyprowadzania
int       AssoOutput(ostream& o,const assoitem& what) const=0;
virtual //Uzywany do wprowadzania
int       AssoInput(istream& o,assoitem& what)=0;

/* C O N S T R U C T O R (S) */
assoc_base(int isrt,int irev):
	tab(0),sorted(isrt),reverse(irev) {}

assoc_base(size_t Lenght,int isrt,int irev):
	tab(Lenght),sorted(isrt),reverse(irev) 
	{
		tab.Truncate(0); //Ale pozostaje juz zaalokowana
	}

virtual ~assoc_base();// Call before deletion for all items
IO_PUBLIC_DECLARE
};

#define OUT_OF_RANGE_ADR 	((char*)FULL)

#define ASSO_KEY_NOT_FOUND AssocKeyNotFoundExcp(__FILE__,__LINE__)
} //namespace

/********************************************************************/
/*			          WBRTM  version 2006                           */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif