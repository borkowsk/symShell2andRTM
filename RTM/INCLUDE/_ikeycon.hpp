/// @file
/// @brief Private base for an array class family.
/// @date 2026-05-04 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_INT_KEY_CON_HPP_INCLUDED_
#define SYMSHELL2_INT_KEY_CON_HPP_INCLUDED_
///  @ingroup OBSOLETE
//  Implement size_t key support
//#include "pix.hpp"
//#include "tnames.h"
#include <cstdint>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"

/// WOJCIECH BORKOWSKI RUN TIME LIBRARY.
namespace wbrtm {

  typedef void*                   pix; ///< @brief Do ukrywania realnej implementacji przed użytkownikiem klas.
  typedef uintptr_t            size_t; ///< @brief `uintptr_t` gwarantuje ten sam rozmiar co `void*` czyli `pix`?
  typedef uintptr_t     object_size_t; ///< @brief Powinien wystarczyć 32-bitowy, ale są rzuty (na `pix`?).
  static_assert(sizeof(pix)==sizeof(size_t),"intptr_t not properly set!");

/// Private base for an array class family.
class int_key_container_base
{
/* PIX SUPPORT — pix == size_t in this class & RANGE & SIZE CHECKING */
static size_t index; //!< ??????
size_t        rsize; //!< current size.
size_t first_unused; //!< max already used index + 1.

protected:
int_key_container_base()
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

/// Set the current size of an array. After successful allocation of a longer buffer.
void SetSize(size_t s)
{
rsize=s;
if(s<first_unused) // if size decreased
    first_unused=s;
}

void registering_and_align_size(size_t& s)
{
const int ALIGN=4;
NewMaxIndex(s-1); //Save before changing! It's important for iterators.
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

/// @return `pix` to first object in the collection, or @c NULL if empty.
/// @note `pix` is declared as `void*` but more often it plays the role of an `integer`
///       (e.g., index) hidden from the user of the class.
pix 	_first() const
    {
    if(CurrSize()>0)
        return reinterpret_cast<pix>(1);
    else
        return reinterpret_cast<pix>(0);
    }

/// @return `pix` to a previous object from the collection.
pix     _previous(pix p) const
    {
    size_t pom=(size_t)p;
    pom--;
    if(pom<1)
      {
      if(CurrSize()>0)
        pom=1;
      else
        pom=0;
      }
    return reinterpret_cast<pix>(pom);
    }

/// @return next object from the collection, or @c NULL if no more.
pix     _next(pix p) const
    {
    size_t pom=(size_t)p;
    pom++;
    if(pom==0 || pom>first_unused)
        return reinterpret_cast<pix>(0);
    else
        return reinterpret_cast<pix>(pom);
    }

/// It destroys `pix` after usage.
void 	_destroy(pix& p) const
    {
    p=reinterpret_cast<pix>(0);
    }

/// @return key for the current object previously set by First, Next or Search.
/// @note Why is this a referential return phrase? It's probably unnecessary and dangerous.
/// TODO: Dlaczego to jest zwrot z referencją? Niepotrzebne chyba i niebezpieczne.
const  size_t&  _key(pix p) const
    {
    index=((size_t)p-1);  //! UWAGA NA KOLEJNE WYWOŁANIA.
    return index;
    }

/* DELETE SUPPORT */

/// It deletes the current object. @return 1 if success.
int  _remove(pix)
    { return 0; }

public:

/// It virtually truncates the table for iterators and index checking.
int Truncate(size_t i)
    {
    if(i<=CurrSize()) //Jeśli zadany rozmiar jest taki jak wymagany to jeszcze OK.
         {
         first_unused=i;
         return 1;
         }
    else return 0;
    }

};

/* PIX SUPPORT — pix == object_size_t in this class */
/* & RANGE & SIZE CHECKING		*/
class long_key_container_base
{
static object_size_t  index;  ///< DO ZWRACANIA PRZEZ REFERENCJĘ. Tylko po co ten chwyt?
object_size_t         rsize;  ///< current size.
object_size_t  first_unused;  ///< max already used index + 1.

public:
long_key_container_base()
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

/// It set current size of an array. After successful allocation of a longer buffer (?).
void SetSize(object_size_t s)
{
rsize=s;
if(s<first_unused) // if size decreased
    first_unused=s;
}

void registering_and_align_size(object_size_t& s)
{
const int ALIGN=8;
NewMaxIndex(s-1); //Save before changing! It's important for iterators.
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

/// @return pix to first object in the collection, or NULL if empty.
pix 	_first() const
    {
    if(CurrSize()>0)
        return reinterpret_cast<pix>(1);
    else
        return reinterpret_cast<pix>(0);
    }

/// @return pix to a previous object from the collection.
pix     _previous(pix p) const
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
    return reinterpret_cast<pix>(pom);
    }

/// @return next object from the collection, or NULL if no more.
pix     _next(pix p) const
    {
    object_size_t pom=(object_size_t)p;
    pom++;
    if(pom==0 || pom>first_unused)
        return reinterpret_cast<pix>(0);
    else
        return reinterpret_cast<pix>(pom);
    }

/// It destroys pix after usage.
void 	_destroy(pix& p) const
    {
    p=reinterpret_cast<pix>(0);
    }

/// @return key for the current object which set previously by First, Next or Search.
const  object_size_t&  _key(pix p) const
    {
    index=((object_size_t)p-1);  //! UWAGA NA KOLEJNE WYWOŁANIA
    return index;
    }

/* DELETE SUPPORT */
/// It deletes the current object, return 1 if success.
int  _remove(pix)
    { return 0; }

/// It virtually truncates the table for iterators and index checking.
int Truncate(object_size_t i)
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

#pragma clang diagnostic pop
/* ****************************************************************** */
/*                 RTM version 2006/2022/2026                         */
/* ****************************************************************** */
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                   */
/*            W O J C I E C H   B O R K O W S K I                     */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*                                                                    */
/*                                (Don't change or remove this note)  */
/* ****************************************************************** */
#endif //SYMSHELL2_INT_KEY_CON_HPP_INCLUDED_



