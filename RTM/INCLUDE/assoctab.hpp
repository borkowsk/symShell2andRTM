/** @file
*  \brief    The simplest associative arrays.
*====================================================================================================================
*  \details Obsolete and not improved part of the wb_rtm library.
*           It was established before 2000.
*           <br>
*         Zarzadza powiazaniami wartosci skalarnych i wskaznikow.
*         Zarowno dla kluczy(K) jak i wartosci(V) rezerwowane jest miesjce o
*         rozmiarze = uniontype (slowo conajmniej 32 bitowe) i typ K i V musi byc
*         rzutowalny na unitype. A K posiadac tez wcielenie funkcji "compare".
*         Wartosci dla K sa kopiowane w razie potrzeby, a dla V mozna je przypisywac.
*
*         "assoc_template" moze byc uzyta do dowolnego typu, ale nie ma operacji I/O.
*         "assoc_table" nie powinna byc uzyta do wskaznikow ze wzgledu na
*         implementacje I/O.
*         "assoc_table_of_ptr" ma mozliwosc wyboru trybu pracy - wskazniki statyczne
*         lub dynamiczne. Przy wczytywaniu jednak wszystkie dane sa realokowane na
*         sterte poniewaz operacja ta moze alokowac nowe dane. Jesli dane sa dynamiczne
*         to sa dealokowane w destruktorze.
*         Dla typu char* mozna uzywac jedynie assoc_template albo typow z rodziny
*         "dictionary*" ze wzgledu na niestandardowe traktowanie w jezyku C++
*
*  \copyright Wojciech T. Borkowski
*  \date 2022-10-12 (last modification)
*  @ingroup OBSOLETE
*/
#ifndef _MSC_VER //# warning still not work under Microsoft C++
#warning  "This code is OBSOLETE and not tested in C++11 standard"
#endif

#ifndef _ASSOC_TABLE_HPP_
#define _ASSOC_TABLE_HPP_

#include "assobase.hpp"
#include "containe.hpp"
#include "compare.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \brief Obsolete class
template<class K,class V>
class assoc_template : public key_container<K,V> ,protected assoc_base
// Typ K i V musi zmiescic sie w unii "unitype" !!!
{
protected:
// Reeksport metody z klasy bazowej ktora moze sie przydac
// znajduje item lub go tworzy (make==1), zwraca NULL
// ALbo wskaznik do znalezionego assoitem
assoitem* _base_search(const unitype key,int make)
	{ return assoc_base::_Search(key,make);}

//Porownuje dwa klucze. Zwraca 0 gdy ==, 1 gdy first>second
virtual
int 	  Compare(const unitype first,const unitype second);
//Wywolywana zawsze przed usunieciem i-temu z tab.
//UWAGA .ptr o wartosci NULL lub FULL oznacza oczyszczony unitype!
void      BeforeDeletion( assoitem& ){}//W tym wypadku nic nie robia
//Uzywany do wyprowadzania
int       AssoOutput(ostream&,const assoitem& /*what*/) const {return 0;} //KLASA BEZ I/O
//Uzywany do wprowadzania
int       AssoInput(istream&,assoitem& /*what*/) {return 0;} //KLASA BEZ I/O

public:
/* CONTAINER SEARCHING SUPPORT & DELETE SUPPORT */
virtual pix 	     first() const ;
virtual pix 	     next(pix p) const ;
virtual void         destroy(pix& p) const ;
virtual int    	     remove(pix p) ;
virtual V&           value(pix p) ;
virtual const K&     key(pix p) const ;
virtual K            copyOfKey(pix p) const ;

/* OTHER PROPERTIES */
virtual V* 	    Search( K key,int make=0 );
virtual void   	Remove( K key );	// usuwa item.
virtual void	RemoveAll();		// usuwa wszystko, mozna zapisywac od nowa
virtual V& 	operator [] ( K key );
virtual V& 	operator () ( K key );

virtual int 	Raise(const WB_Exception_base& e) const;

size_t CurrSize() const { return assoc_base::CurrSize();}
/* CONSTRUCTORS */
assoc_template(int sorted=1,int rev=0):
	assoc_base(sorted,rev)	    {}

assoc_template(size_t lenght,int sorted,int rev)://With prealocation
	assoc_base(lenght,sorted,rev)	    {}

~assoc_template(){}  //NOTHING TO DO THERE, BUT...

IO_PUBLIC_DECLARE
};

/// \brief Obsolete class
template<class K,class V>
class assoc_table: public assoc_template<K,V>
// only scalars in val and key
{
VIRTUAL_NECESSARY( assoc_table<K _COMA_ V> )
protected:
//Uzywany do wyprowadzania
int		  AssoOutput(ostream&,const assoitem& what) const;
//Uzywany do wprowadzania
int       AssoInput(istream&,assoitem& what);
public:
assoc_table(int sorted=container_base::SORTED,
	    int rev=container_base::NORMAL_ORDER):
	assoc_template<K,V>(sorted,rev)
			{}
~assoc_table(){} //NOTHING TO DO!
//IO_PUBLIC_DECLARE dziedziczony po bazowej
};

/// \brief Obsolete class
template<class K,class V>
class assoc_table_of_ptr: public assoc_template<K,V*>
// only scalars in key & only pointers in val, rather heap allocated
{
IO_PUBLIC_DECLARE
VIRTUAL_NECESSARY( assoc_table_of_ptr<K _COMA_ V> )
cbase::memmode v_mem_mode;
protected:
//Wywolywana zawsze przed usunieciem i-temu z tab.
//UWAGA .ptr o wartosci NULL lub FULL oznacza oczyszczony unitype!
void      BeforeDeletion( assoitem& );
//Uzywany do wyprowadzania
int       AssoOutput(ostream&,const assoitem& what) const;
//Uzywany do wprowadzania
int       AssoInput(istream&,assoitem& what);
public:
int		DataAreStatic(){ return v_mem_mode!=cbase::DYNAMIC_VAL;}
void	ReallocData();	//move user data to current heap
virtual //Dla pewnosci
void	RemoveAll();	//clean assoc_table. May be calling "delete".
assoc_table_of_ptr(	int sorted=cbase::SORTED,
					int rev=cbase::NORMAL_ORDER,
					cbase::memmode m=cbase::DYNAMIC_VAL):
	assoc_template<K,V*>(sorted,rev)
				{v_mem_mode=m;}
~assoc_table_of_ptr(); //call BeforeDeletion for all items & delete input_pom
// Very safe indexing. Doing neccesary dealocation if used as lvalue
lvptr<V> lv(K index)
		{
		V** pom=Search(index,1);
		if(pom==FULL) pom=NULL;//lvptr nie osluguje FULL
		return lvptr<V>(*pom,v_mem_mode);
		}
};

// IMPLEMENTACJA METOD DLA TABLIC ASOCJACYJNYCH
// //////////////////////////////////////////////////////////////////////////////////////////
/* GLOBAL TEMPLATES FOR CONVERSION  */

/// \brief Obsolete class
template<class T>
inline
unitype _to_unitype_for_assoc_table(T& key)
{
unitype pom;
assert(sizeof(T)<=sizeof(unitype));

// Operacja umieszczenia na obiekcie unitype
// /////////////////////////////////////////
new (&pom)T(key);//Klasa T musi miec operacje inicjacji z obiektu tego samego typu
// /////////////////

return pom;//Tu juz przepisanie binarne
}

/// \brief Obsolete class
template<class T>
inline
T _from_unitype_for_assoc_table(T&,unitype u)//
{
return  *(T*)&(u);
}

/* ASSOC_TEMPLATE CONTAINER SEARCHING SUPPORT & DELETE SUPPORT */
/// \brief Obsolete class
template<class K,class V>
inline
int       assoc_template<K,V>::Raise(const WB_Exception_base& e) const
	{ return tab.Raise(e); }

    /// \brief Obsolete class
template<class K,class V>
inline  V* assoc_template<K,V>::Search(K key,int make)
{
// Typ K musi byc rzutowalny na gkey_t
assoitem* pom=assoc_base::_Search(_to_unitype_for_assoc_table(key),make);
if(pom==NULL) return NULL;
   else return (V*)&(pom->val);
}

/// \brief Obsolete class
template<class K,class V>
//Porownuje dwa klucze. Zwraca 0 gdy ==, 1 gdy first>second
int 	  assoc_template<K,V>::Compare(const unitype first,const unitype second)
{
K f=*(K*)&(first);
K s=*(K*)&(second);// Nie mozna zwyczajnie bo K moze byc wskaznikiem do klasy
return compare(f,s);
}

/// \brief Obsolete class
template<class K,class V>
inline pix 	     assoc_template<K,V>::first() const { return _first();}

/// \brief Obsolete class
template<class K,class V>
inline pix 	     assoc_template<K,V>::next(pix p) const { return _next(p);}

/// \brief Obsolete class
template<class K,class V>
inline void        assoc_template<K,V>::destroy(pix& p) const { _destroy(p); }

/// \brief Obsolete class
template<class K,class V>
inline int         assoc_template<K,V>::remove(pix p)  { return _remove(p); }

/// \brief Obsolete class
template<class K,class V>
inline V&          assoc_template<K,V>::value(pix p) { return *(V*)_value(p); }

/// \brief Obsolete class
template<class K,class V>
inline const K&    assoc_template<K,V>::key(pix p) const { return *(K*)_key(p);}

/// \brief Obsolete class
template<class K,class V>
inline K           assoc_template<K,V>::copyOfKey(pix p) const { return *(K*)_key(p);}

/* OTHER PROPERTIES */
/// \brief Obsolete class
template<class K,class V>
void	assoc_template<K,V>::RemoveAll()
//clean assoc_table. May be calling "delete".
{
	assoc_base::Truncate();//Zapomina ze mial juz elementy, ale nie dealokuje
}

/// \brief Obsolete class
template<class K,class V>	// usuwa item.
inline void	     assoc_template<K,V>::Remove(K key)
				{assoc_base::_Remove(_to_unitype_for_assoc_table(key));}

/// \brief Obsolete class
template<class K,class V>
inline
V& 	  assoc_template<K,V>::operator [] (K key)
{
V* pom=Search(key,0/*Don't make*/);
if(pom==NULL)
	Raise(ASSO_KEY_NOT_FOUND);
return *pom;
}

/// \brief Obsolete class
template<class K,class V>
inline
V&        assoc_template<K,V>::operator () (K key)
{
V* pom=Search(key,1/*Make*/);
if(pom==NULL) 
	Raise(ASSO_KEY_NOT_FOUND);//Can't find/make item
return *pom;
}

/* I/O SUPPORT */
/// \brief Obsolete class
template<class K,class V>
void assoc_template<K,V>::implement_input(istream& inp)
{
assoc_base::implement_input(inp);
}

/// \brief Obsolete class
template<class K,class V>
void assoc_template<K,V>::implement_output(ostream& o)      const
{
assoc_base::implement_output(o);
}

/* ASSOC_TABLE I/O IMPLEMENTATION */
/// \brief Obsolete class
template<class K,class V>
int   assoc_table<K,V>::AssoOutput(ostream& o,const assoitem& what) const
//Uzywany do wyprowadzania
{
o<<*(K*)&(what.key)<<' ';
o<<*(V*)&(what.val);
return !o.fail();
}

/// \brief Obsolete class
template<class K,class V>
int   assoc_table<K,V>::AssoInput(istream& inp,assoitem& what)
//Uzywany do wprowadzania
{
K k;V v;
inp>>k;
if(inp.fail()) return 0;
inp>>v;
if(inp.fail()) return 0;
what.key=_to_unitype_for_assoc_table(k);
what.val=_to_unitype_for_assoc_table(v);
return 1;
}


/* ASSOC TABLE OF PTR IMPLEMENTATION */
/// \brief Obsolete class
template<class K,class V>
void assoc_table_of_ptr<K,V>::ReallocData()
{
object_size_t size=this->tab.CurrSize();
if(size>0)// Przealokowac statycznie alokowane wartosci
   for(size_t i=0;i<size;i++)
		{
		V* v1=_from_unitype_for_assoc_table(v1,(this->tab)[i].val);//V should be a ptr!!!
		if(v1!=NULL && v1!=FULL)
			{
			V* v2=clone(v1);
			if(v2==NULL)
                if(this->Raise(OutOfMemoryExcp(sizeof(v2[0]),__FILE__,__LINE__))==1)
					return;
			(this->tab)[i].val.ptr=v2;
			if(v_mem_mode==container_base::DYNAMIC_VAL)
				delete v1;
			}
		}
v_mem_mode=container_base::DYNAMIC_VAL;//Zrobil z niego dynamic.
}

/// \brief Obsolete class
template<class K,class V>
int   assoc_table_of_ptr<K,V>::AssoOutput(ostream& o,const assoitem& what) const
//Uzywany do wyprowadzania
{
o<<*(K*)&(what.key)<<' ';
V* pom=(V*)(what.val.ptr);
if(pom==NULL || pom==FULL)
	o<<"@ ";
	else
	WriteDynamicVal<V>(o,pom,this);
return !o.fail();
}

/// \brief Obsolete class
template<class K,class V>
void      assoc_table_of_ptr<K,V>::BeforeDeletion( assoitem& a )
//Wywolywana zawsze przed usunieciem i-temu z tab.
//UWAGA .ptr o wartosci NULL lub FULL oznacza oczyszczony unitype!
//Tu dealokuje zmienna wskazywana przez val jesli nie jest juz pusta.
{
if(!DataAreStatic() && a.val.ptr!=NULL && a.val.ptr!=FULL)
	{
	delete (V*)(a.val.ptr);
	a.val.ptr=NULL;
	}
}

/// \brief Obsolete class
template<class K,class V>
int   assoc_table_of_ptr<K,V>::AssoInput(istream& inp,assoitem& what)
//Uzywany do wprowadzania
{
K k;
V* v=NULL;
inp>>k;
if(inp.fail()) return 0;
char c;//Do sprawdzanie @
inp>>c;
if(c!='@')
  {
  v=ReadDynamicVal<V>(inp,this);
  if(v==NULL)
    if(this->Raise(OutOfMemoryExcp(sizeof(v[0]),__FILE__,__LINE__))==1)
				return 0;
  }
  else v=NULL;
what.key=_to_unitype_for_assoc_table(k);
what.val=_to_unitype_for_assoc_table(v);
return 1;
}

/// \brief Obsolete class
template<class K,class V>
void assoc_table_of_ptr<K,V>::implement_input(istream& inp)
{
if(v_mem_mode!=container_base::DYNAMIC_VAL)
	ReallocData();
assoc_template<K,V*>::implement_input(inp);
}

/// \brief Obsolete class
template<class K,class V>
void assoc_table_of_ptr<K,V>::implement_output(ostream& o)      const
{
assoc_template<K,V*>::implement_output(o);
}

/// \brief Obsolete class
template<class K,class V>
assoc_table_of_ptr<K,V>::~assoc_table_of_ptr() //call BeforeDeletion for all items
{
object_size_t size=this->tab.CurrSize();
if(!DataAreStatic())
  for(size_t i=0;i<size;i++)
	BeforeDeletion(this->tab[i]);
}

/// \brief Obsolete class
template<class K,class V>
void	assoc_table_of_ptr<K,V>::RemoveAll()
//clean assoc_table. May be calling "delete".
{
object_size_t size=this->tab.CurrSize();

if(!DataAreStatic())
  for(size_t i=0;i<size;i++)
  {
	BeforeDeletion(this->tab[i]);//Tu powinien zdealokowac to co wskazywane
  }

assoc_template<K,V*>::RemoveAll();//A teraz po prostu usuwa
}

DEFINE_VIRTUAL_NECESSARY_FOR_TEMPLATE( <class K _COMA_ class V> , assoc_table<K _COMA_ V> )
DEFINE_VIRTUAL_NECESSARY_FOR_TEMPLATE( <class K _COMA_ class V> , assoc_table_of_ptr<K _COMA_ V> )

} //namespace

/* ******************************************************************/
/*                WBRTM  version 2022 for GuestXR                   */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif
