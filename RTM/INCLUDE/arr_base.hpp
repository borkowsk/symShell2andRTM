/** @file
*  \brief    Base class for dynamic resizable arrays
*====================================================================================================================
*  \details Obsolete and not improved part of the wb_rtm library.
*           It was established before 2000.
*  \copyright Wojciech T. Borkowski
*  \date 2022-12-31 (last modification)
*  @ingroup OBSOLETE
*/
#ifndef _MSC_VER //# warning still not work under Microsoft C++
//#warning  "This code is OBSOLETE and not tested in C++11 standard"
#endif

#ifndef _ARRAY_BASE_HPP_
#define _ARRAY_BASE_HPP_

#include <assert.h>

#include "wb_ptr.hpp"
#include "excpmem.hpp"
#include "excpothr.hpp"
#include "_ikeycon.hpp"
#include "containe.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \brief Obsolete class
template<class T>
class array_base:public key_container<size_t,T>,public int_key_container_base
{
protected:
T* tab;
wb_pchar Separator;//Musi byc wypisywany jako const char* bo wiekszosc typowych separatorow klasa wb_pchar wypisuje w cudzyslowie
const char* separator() const   { return Separator.get(); }
public:
/* CONTAINER SEARCHING SUPPORT & DELETE SUPPORT */
virtual pix 	      first() const     { return _first();}
virtual pix 	      next(pix p) const { return _next(p);}
virtual void          destroy(pix& p) const { _destroy(p); }
virtual int    	      remove(pix p)     { return _remove(p); }
virtual T&            value(pix p)      { return tab[((size_t)p-1)];	}
virtual size_t        copyOfKey(pix p) const  { return _key(p); }
virtual const size_t& key(pix p) const  { return _key(p); }

/* IMPLEMENTATION OF OTHER PROPERTIES */
size_t                Low()             { return 0;}	   // lowerst index
long                  Height()          { return long(CurrSize())-1;} //hight useable index,-1 if size==0
int	                  Del(size_t pos)   { return KickOf(pos);}
wb_pchar              ChangeSeparator(const char* Sep);//Separator(s) for array printing. Must be only blank characters
/* Fast indexing without any controling if NDEBUG defined */
T&                    operator[] (size_t i){assert(i>=0 && i<CurrSize());return tab[i];} 
const T&              operator[] (size_t i) const    {assert(i>=0 && i<CurrSize());return tab[i];} 

/* Safe indexing with always range checking OR expansion */
virtual T&            operator  () (size_t i)=0;
virtual void          operator  =  (const array_base& a);//Uzywa () 

/* CONSTRUCTION/DESTRUCTION */
protected: //Use only as base class
array_base(const array_base& a);
array_base(size_t isize);
~array_base();

public:
virtual int Allocate(size_t items); //Alokuje odpowiednia tablice tak jak w konstruktorze, o ile tab==NULL i items>0
virtual int Deallocate();   //Usuwa wszystko, wywolujac destruktory podobiektow w ten sam sposob co destruktor array_base
virtual int Expand(size_t nsize)=0;// Tu faktycznie nie powieksza, ale sprawdza czy jest jeszcze miejsce w ramach alokacji
virtual int KickOf(size_t pos);//Bezpieczna implementacja przemieszczajaca, ale nie wywolujaca destruktorow

/* Conversion to pointer to T. */
const T* GetConstTabPtr() const {return tab;}//Read only static array of size=GetSize()
T*	 GetTabPtr()	{return tab;}			 //Use very carefully,as static array of size=GetSize()
T*	 GiveTabPtr()	{Truncate(0);T* pom=tab;tab=NULL;return pom;}//Caller is responsible for dealocation of ptr.
};

template<class T>
int array_base<T>::KickOf(size_t pos)
//Bezpieczna implementacja przemieszczajaca, ale nie wywolujaca destruktorow
{
size_t Siz=CurrSize();
if(Siz==0 || pos>=Siz) return 0;
size_t N=(Siz-pos)-1;
if(N>0)
{	
	assert(pos<Siz-1);//To nie jest ostatni element
	char bufor[sizeof(T)];
	//Przesuwa do przodu o 1. Usuwany element przesuwa na koniec
	memcpy(bufor,&tab[pos],sizeof(T));
	memmove(&tab[pos],&tab[pos+1],sizeof(T)*N);//memmove jest ostrozniejsze
	memcpy(&tab[Siz-1],bufor,sizeof(T));
}
Truncate(Siz-1);//Tu skraca tablice. Usuniety, teraz ostatni, element przestaje byc w tablicy.
assert(CurrSize()+1==Siz);
return 1;
}

template<class T>
int array_base<T>::Allocate(size_t N)
{
    if(tab!=NULL) return 0;
    if(N==0) return 0;
    
    registering_and_align_size(N);
    tab=new T[N];
    if(tab==NULL)
    {
        if( key_container<size_t,T>::Raise(OutOfMemoryExcp(N,__FILE__,__LINE__))==1 )
            SetSize(0);
        return 0;
    }
    else
    {  
        SetSize(N);
        return 1;
    }
    
}

template<class T>
int array_base<T>::Deallocate()  //Usuwa wszystko, wywolujac destruktory podobiektow w ten sam sposob co destruktor array_base
{
if(tab)
#ifdef USES_ALLOCATORS
     delete [GetSize()] tab;// User defined alocators may use size of table
#else
     delete [] tab;// default C++ alocator dont use size of table
#endif
   tab=NULL;
   int_key_container_base::Truncate(0);
   size_t s=0;
   int_key_container_base::SetSize(s);
   return 1;
}

template<class T>
int array_base<T>::Expand(size_t nsize)
// Tu faktycznie nie powieksza, tylko informuje ze sie nie udalo
{
return 0;
}

template<class T>
array_base<T>::array_base(const array_base<T>& a):tab(NULL),Separator(" ")
{
/*object_*/size_t isize=a.CurrSize();
if(isize>0)
  {
    if(Allocate(isize))           
        for(size_t i=0; i<a.CurrSize() ;i++)
            tab[i]=a.tab[i];  
  }
  else
  SetSize(0);
}

template<class T>
inline array_base<T>::array_base(size_t isize):tab(NULL),Separator(" ")
{
registering_and_align_size(isize);
SetSize(isize);
if(isize>0)
  {
  Allocate(isize);
  }
}

template<class T>
inline array_base<T>::~array_base()
{
if(tab)
#ifdef USES_ALLOCATORS
     delete [GetSize()] tab;// User defined alocators may use size of table
#else
     delete [] tab;// default C++ alocator dont use size of table
#endif
}

template<class T>
void array_base<T>::operator  =  (const array_base& a)
//Uzywa () 
{
size_t asiz=a.CurrSize();
(*this)(asiz-1);//Zaczyna od ostatniego zeby wymusic alokacje jesli trzeba
for(size_t i=asiz;i>0;i--)
	{
	(*this)[i-1]=a[i-1];
	}
Truncate(asiz);//Raczej nie dealokuje, ale nadmiar uznaje za "niebyly"
assert(CurrSize()==a.CurrSize());
}

template<class T>
inline wb_pchar array_base<T>::ChangeSeparator(const char* Sep)
//Setting Separator(s) for array printing. Must be only blank characters
{
	wb_pchar pom=Separator;
	Separator=clone_str(Sep);
	return pom;
}

} //namespace

/* *******************************************************************/
/*			          WBRTM  version 2006                            */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego        */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                 */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
#endif
