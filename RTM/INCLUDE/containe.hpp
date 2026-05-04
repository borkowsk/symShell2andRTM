// Klasy definiujace interface do wszelkiego rodzaju kontenerow
// zawierajacych obiekty typu T
// ///////////////////////////////////////////////////////////////////////
#ifndef _CONTAINER_HPP_
#define _CONTAINER_HPP_
//#include "pix.hpp"
#include "vobject.hpp"

#ifdef __DJGPP__
#pragma interface
#endif

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

// Szablon zapewniajacy wlasciwa semantyke przypisania na wskaznik
// zamkniety w typie kontenerowym. Uzywaja funkcje lv klas tablicowych.
template<class TT>
class lvptr
////////////////////////////////////
{
TT*& reftoptr;
unsigned flags;
public:	
lvptr(TT*& ini,unsigned ifl):reftoptr(ini),flags(ifl){}
lvptr(const lvptr& ini):reftoptr(ini.reftoptr),flags(ini.flags){}

operator TT* () 
	{
		return reftoptr;//rvalue (konversja do TT*)
	}  

TT* operator = (TT* asi)//Symuluje lvptr
	{ 
	if((flags&0x1) && reftoptr!=NULL)
		delete reftoptr;//oczyszczenie wskaznika
	reftoptr=asi;		//przypisanie na wlasciwy wskaznik
	return reftoptr;	//konwersja do TT*
	}	
};

//Baza kontenerow - glownie definiuje wyliczenia
class container_base:public wbrtm::vobject
//////////////////////////////////////////////////////
{
public:
container_base(void){}
~container_base(void){}
enum memmode   { STATIC_VAL=0,STACK_VAL=0,HEAP_VAL=1, DYNAMIC_VAL=1};
enum ordermode { SORTED=1,UNSORTED=0,REVERSE_ORDER=2,NORMAL_ORDER=0};
};

typedef container_base cbase;//Istotny skrot bo trzeba naduzywac w odniesieniach do enum's

template<class T>
class container: public container_base
{
public:
container(void)	{}
~container(void){}
/* ITERATION SUPPORT */
virtual pix 	     first() const=0;
	// return pix to first object in collection, or NULL if empty
virtual pix 	     next(pix) const=0;
	// return next object from collection,or NULL if no more
virtual void         destroy(pix&) const=0;
	// destroy pix after using
virtual T&           value(pix)=0;
	// return key for current object setting by  First,Next or Search
/* DELETE SUPPORT */
virtual int    	     remove(pix)=0;
	// deleting current object, return 1 if success
};

template<class K,class V>
class key_container: public container<V>
{
protected:
key_container(void) {}
public:
// Both "key" method return key for current object coded in pix
virtual const K& key(pix) const=0; //const reference - for class keys
virtual K        copyOfKey(pix) const=0;//Copy value - for scalars & pointers
// Both []&() method return lvalue associated with particular key
//V& operator [] (K)=0;// the most efficient indexing only for already existing items
virtual V& operator () (K)=0;// the most smart indexing. Bulding item if possible
};

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
