/** @file
*  \brief    Dynamic arrays - some resizable
*====================================================================================================================
*  \details Obsolete and not improved part of the wb_rtm library.
*           It was established before 2000.
*  \copyright Wojciech T. Borkowski
*  \date 2022-10-12 (last modification)
*  @ingroup OBSOLETE
*/
#ifndef _MSC_VER //# warning still not work under Microsoft C++
#warning  "This code is OBSOLETE and not tested in C++11 standard"
#endif

#ifndef _ARRAY_HPP_
#define _ARRAY_HPP_

#include <iostream>
#include <cstring>

#include "arr_base.hpp"
#include "excpothr.hpp"
#include "ioexcep.hpp"

#include "_voidnew.hpp"
#include "iosuppor.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \brief Obsolete class
template<class T>
class array_constsize:public array_base<T>
	// Not RESIZEABLE. User must set size in construction.
    // For scalar & class with << & >> operators;
{
    using vobject::Raise;
    using int_key_container_base::NewMaxIndex;
    using int_key_container_base::CurrSize;
    using int_key_container_base::GetSize;

    VIRTUAL_NECESSARY_AND_IO( array_constsize<T> )
public:
    array_constsize(const array_base<T>& a):array_base<T>(a){}
    array_constsize(size_t N=1):array_base<T>(N)//1 zeby byl jakis bezparametrowy
    { // Alloc array of N items
                                                assert(N>0);
          NewMaxIndex(N-1); // save index for iterators & High function
    }
    ~array_constsize(){}                  // destroy array
    T&   operator  () (size_t i);		 // raise exception if "i" not into
    int Expand(size_t nsize);// Tu faktycznie nie powieksza, ale sprawdza czy jest jeszcze miejsce w ramach alokacji
};

/// \brief Obsolete class
template<class T>
class array_template:public array_base<T>
	// Define index operator and basic realloc
	// for derived array types
{
public:
	~array_template() {}
	array_template(size_t isize=0):array_base<T>(isize) {}
	array_template(const array_base<T>& a):array_base<T>(a){}
	T&   operator  () (size_t i); // Automatic array expansion if index not in allocated block
	int Expand(size_t nsize);// Realloc array with coping data
	int Insert(size_t posit);// Insert empty element at posit(ion). Move the rest of table.
};

/// \brief For no class types - construction/destruction maybe not handled properly
template<class T>
class array_of:public array_template<T>
{
	VIRTUAL_NECESSARY_AND_IO( array_of<T> )
public:
	~array_of(){}
	array_of(size_t isize=0):array_template<T>(isize) {}
	array_of(const array_base<T>& a):array_template<T>(a){}
	//int Expand(size_t nsize);
};

/// \brief Obsolete class
template<class T>
class array_of_class:public array_template<T>
	// For class types - with construction/destruction handling
	// NOTE: Objects must be position independent! Array can move
	// any from its non active components to new position in memory
	// without construction/destruction sequention
{
	VIRTUAL_NECESSARY_AND_IO( array_of_class<T> )
		void force_init_table(void* mem,size_t items);
public:
	~array_of_class(); // special handling destruction!
	// Special handling construction because special behavior of expand
	array_of_class(size_t isize=0):array_template<T>(0) {array_of_class::Expand(isize);}
	array_of_class(const array_base<T>& a);//Use Expand for allocation
	int Expand(size_t nsize);// Realloc with construction & destruction
};

/// \brief only for pointers, rather pointing to heap allocated blocks.
template<class T>
class array_of_ptr:public array_template< T* >
{
	VIRTUAL_NECESSARY_AND_IO( array_of_ptr< T > )
	container_base::memmode g_mem_mode;
	void _Force_Pointers_Item_Only_(){ *this->tab[0]; }
protected:
	virtual
	int KickOf(size_t pos);	//Poprawione ze wzgledu na ewentualnie zaalokowany obiekt.
public:
	int  DataAreStatic(){ return g_mem_mode!=container_base::DYNAMIC_VAL;}
	void ReallocData();//Move data to current heap
	~array_of_ptr();
	array_of_ptr(const array_base< T* >& a):array_template<T*>(a){}
	array_of_ptr(size_t isize=0,
			    container_base::memmode m=container_base::DYNAMIC_VAL):
		array_template< T* >(isize),g_mem_mode(m)
	{
		if(this->tab)
			memset(this->tab,0x0,array_template< T* >::GetSize()*sizeof(T*));
	}
	// Very safe indexing. Doing neccesary dealocation if used as lvalue
	lvptr<T> lv(size_t index)
	{
		return lvptr<T>((*this)(index),g_mem_mode);
	}
	int Expand(size_t nsize);// Realloc with clear pointers
};

#include "arrays.imp"

} //namespace

/* ******************************************************************/
/*			        WBRTM  version 2013-09-26                       */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://borkowski.iss.uw.edu.pl/                     */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif
