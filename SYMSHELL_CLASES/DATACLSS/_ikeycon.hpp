#ifndef _IKEYCON_HPP_
#define _IKEYCON_HPP_
///  Private base for array class family.
///  Implement size_t key support

//#include "pix.hpp"
//#include "tnames.h"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

    typedef void* 	                pix; ///< Po co tak to nie wiem, jakaś stara historia.
    typedef unsigned long        size_t; ///< ulong - ten sam rozmiar co pix?
    typedef unsigned long object_size_t; ///< powinien wystarczyć 32 bitowy, ale są rzuty.

class int_key_container_base
{
/* PIX SUPPORT - pix == size_t in this class */
/* & RANGE & SIZE CHECKING		*/
static size_t index;
size_t rsize;         // current size
size_t first_unused;  // max already used index + 1

protected:
int_key_container_base(void)
{
first_unused=0;rsize=0;
}

void NewMaxIndex(size_t i)
{
if(i+1>first_unused)
	first_unused=i+1;
}

void ClearLastItem()
{
if(first_unused>0)
	first_unused--;
}

void SetSize(size_t s)
// set size of array. After succesfull allocation
{
rsize=s;
if(s<first_unused) // if size decreased
	first_unused=s;
}

void registering_and_align_size(size_t& s)
{
const int ALIGN=4;
NewMaxIndex(s-1); //save before changing. It's important for iterators.
s=((s+ALIGN-1)/ALIGN)*ALIGN;
}

public:
size_t GetSize() const
{
return rsize;
}

size_t CurrSize() const
{
return first_unused;
}


protected:
/* IMPLEMENTATION OF SEARCHING SUPPORT */
pix 	_first() const
	// return pix to first object in collection, or NULL if empty
	{
	if(CurrSize()>0)
		return (pix)1;
		else
		return (pix)0;
	}
pix     _previous(pix p) const
	// return pix to previous object from collection
	{
	long pom=(size_t)p;
	pom--;
	if(pom<1)
      {
	  if(CurrSize()>0)
		pom=1;
		else
		pom=0;
	  }
	return (pix)pom;
	}
pix     _next(pix p) const
	// return next object from collection,or NULL if no more
	{
	size_t pom=(size_t)p;
	pom++;
	if(pom==0 || pom>first_unused)
		return (pix)0;
		else
		return (pix)pom;
	}

void 	_destroy(pix& p) const
	// destroy pix after using
	{
	p=(pix)0;
	}

const  size_t&  _key(pix p) const
	// return key for current object setting by  First,Next or Search
	{
	index=((size_t)p-1);  //! UWAGA NA KOLEJNE WYWOLANIA
	return index;
	}
/* DELETE SUPPORT */
int  _remove(pix)
	// deleting current object, return 1 if success
	{ return 0; }
public:
int Truncate(size_t i)
	// truncate table for iterators and index checking
	{
	if(i<=CurrSize())//Jesli zadany rozmiar jest taki jak wymagany to OK
	     {
	     first_unused=i;
	     return 1;
	     }
	     else return 0;
	}

};

class long_key_container_base
{
/* PIX SUPPORT - pix == object_size_t in this class */
/* & RANGE & SIZE CHECKING		*/
static object_size_t index;
object_size_t rsize;   // current size
object_size_t first_unused;  // max already used index + 1

public:
long_key_container_base(void)
{
first_unused=0;rsize=0;
}

void NewMaxIndex(object_size_t i)
{
if(i+1>first_unused)
	first_unused=i+1;
}

void ClearLastItem()
{
if(first_unused>0)
	first_unused--;
}

void SetSize(object_size_t s)
// set size of array. After succesfull allocation
{
rsize=s;
if(s<first_unused) // if size decreased
	first_unused=s;
}

void registering_and_align_size(object_size_t& s)
{
const int ALIGN=8;
NewMaxIndex(s-1); //save before changing. It's important for iterators.
s=((s+ALIGN-1)/ALIGN)*ALIGN;
}

object_size_t GetSize() const
{
return rsize;
}

object_size_t CurrSize() const
{
return first_unused;
}

/* IMPLEMENTATION OF SEARCHING SUPPORT */
pix 	_first() const
	// return pix to first object in collection, or NULL if empty
	{
	if(CurrSize()>0)
		return (pix)1;
		else
		return (pix)0;
	}
pix     _previous(pix p) const
	// return pix to previous object from collection
	{
    object_size_t pom=(object_size_t)p;
	pom--;
	if(pom<1)
	  {
	  if(CurrSize()>0)
		pom=1;
		else
		pom=0;
	  }
	return (pix)pom;
	}
pix     _next(pix p) const
	// return next object from collection,or NULL if no more
	{
	object_size_t pom=(object_size_t)p;
	pom++;
	if(pom==0 || pom>first_unused)
		return (pix)0;
		else
		return (pix)pom;
	}

void 	_destroy(pix& p) const
	// destroy pix after using
	{
	p=(pix)0;
	}

const  object_size_t&  _key(pix p) const
	// return key for current object setting by  First,Next or Search
	{
	index=((object_size_t)p-1);  //! UWAGA NA KOLEJNE WYWOLANIA
	return index;
	}

/* DELETE SUPPORT */
int  _remove(pix)
	// deleting current object, return 1 if success
	{ return 0; }

int Truncate(object_size_t i)
	// truncate table for iterators and index checking
	{
	if(i<CurrSize())
	     {
	     first_unused=i;
	     return 1;
	     }
	     else return 0;
	}

};


} //namespace

/* *******************************************************************/
/*                    WBRTM  version 2006/2022                       */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
#endif


