/*
Base class for all class with virtual members
Force VTBL as first member & destructor as virtual method.
*/

#ifndef _VOBJECT_HPP_
#define _VOBJECT_HPP_

#include "wb_tnames.h"
#include "excpbase.hpp"
#include "errorhan.hpp"

#if defined( __BCPLUSPLUS__ )
#define _CPPRTTI
#endif

#if defined( __GXX_RTTI ) //g++
#include <typeinfo>
#endif

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

class vobject;
//class IO_type_info_base;
class base_encoder;
class base_decoder;

//class ostream; &  class istream;
#include <iostream>
using namespace std;

class IO_type_info_base
// Wystarczy po jednym obiekcie na typ
// Potomstwo tej klasy tworzy listę informacji o typach
// w aplikacji.
{
friend class _io_database;
static  IO_type_info_base*	    top;
        IO_type_info_base*		next;
protected:
        IO_type_info_base(); ///< Linkuje obiekt do listy
virtual ~IO_type_info_base(){}; ///< Skoro jest wirtualny interfejs to i musi być wirtualny destruktor

public:
// Wymuszony interface
//*///////////////////////
virtual const char* Name() const=0 ;  ///< Nazwa klasy - jakaś! Format nie jest zdefiniowany
virtual size_t		SizeOf() const=0; ///< Wirtualnie dostępny rozmiar obiektu
virtual vobject*	Create() const=0; ///< Tworzy obiekt tego typu na bazie domyślnego konstruktora
};

template<class T>
class IO_type_info:public IO_type_info_base
//Defaultowa implementacja
{
protected:
#if !defined( _CPPRTTI ) && !defined( __GXX_RTTI )
static const char* name;//User musi dostarczyc linkerowi
public:
const char* Name() const { return name; }
#else
//"Class Name" dostarcza kompilator przez RTTI
public:
const char* Name() const { return typeid(T).name();}
#endif
vobject*  Create() const { return new T;}
size_t    SizeOf() const { return sizeof(T);}
};

template<>//New syntax - explicit specialization of class template
class IO_type_info<vobject>:public IO_type_info_base
//Implementacja dla `vobject` wyłapuje błędy użycia
//io_type_info klasach potomnych `vobject`.
{
public:
const char* Name() const;
vobject*  Create() const;
size_t    SizeOf() const;
};

template<class T>
class IO_type_info_alias:public IO_type_info<T>
//Implementacja aliasów
{
const char* loc_name; //User musi dostarczyć w inicjalizacji
				      //jako łańcuch trwały przez cały czas istnienia tego obiektu
public:
const char* Name()const{ return loc_name;}
IO_type_info_alias(const char* alias_name) //alias_name nie może zniknąć wcześniej niż ten obiekt!
			:loc_name(alias_name)
{}
};

// RAISE AS MACRO IS MORE FLEXIBLE.
// IT CAN (???) SIMULATE EXCEPTION WITHOUT SUPPORT
#define RAISE( _EXC_ ) 	\
	{if( Raise(_EXC_) ) return ;/* will call destructors */}
#define RAISEV( _EXC_ , _RV_ ) \
	{if( Raise(_EXC_) ) return (_RV_) ;/* will call destructors */}

// Same deklaracje operacji wejścia/wyjścia
#define  IO_PUBLIC_DECLARE						\
protected:										\
/*virtual void implement_encode(base_encoder&) const;  */   \
/*virtual void implement_decode(base_decoder&) const;  */   \
virtual void implement_output(ostream&)const;	\
virtual void implement_input(istream&);			\
private:										\

// Pełnia niezbędnych informacji koniecznych dla wejścia/wyjścia
// oraz klonowani obiektu w kontenerach heterogenicznych
#define  VIRTUAL_NECESSARY( _P_ )		    \
private:								    \
static IO_type_info< _P_ > _io_info;	    \
virtual									    \
vobject*	_clone() const				    \
	{ return new _P_(*this); }			    \
public:									    \
virtual									    \
const IO_type_info_base& _IO_info() const	\
	{ return _io_info;}	                    \
static                  				    \
const IO_type_info_base& _IO_info_static()  \
    { return _io_info;}					    \
friend									    \
_P_*    clone(const _P_ * par)			    \
	{ return (_P_ *)(par->_clone()); }	    \
private:								    \

//Wersja deklaracji odtwarzająca przecinek niekompatybilny z preprocesorem
#define  VIRTUAL_NECESSARY2( _P1_ , _P2_ )		\
private:										\
static IO_type_info<  _P2_,_P2_  > _io_info;	\
virtual											\
vobject*	_clone() const						\
	{ return new _P1_,_P2_   (*this); }			\
public:											\
virtual											\
const IO_type_info_base& _IO_info() const		\
	{ return _io_info;}							\
static                  				        \
const IO_type_info_base& _IO_info_static()      \
    { return _io_info;}					        \
friend											\
_P_*    clone(const _P1_,_P2_ * par)			\
	{ return (  _P1_,_P2_  *)(par->_clone()); }	\
private:										\

//Razem z IO
#define VIRTUAL_NECESSARY_AND_IO(  _PP_ )	\
VIRTUAL_NECESSARY( _PP_ )					\
IO_PUBLIC_DECLARE							\


// Definicje zmiennych niezbędnych do wejścia/wyjścia
#define DEFINE_VIRTUAL_NECESSARY_FOR( _T_ )	\
IO_type_info< _T_ > _T_::_io_info;

// Definiuje alias do czytania strumieni klas produkcji innych kompilatorów
#define DEFINE_ALIAS_RTIME_TYPE( _T_ , _ident_ )	\
IO_type_info_alias< _T_ > _io_info_alias_##_ident_   ( (#_ident_) );

// Wersja pozwalająca na zupełnie dowolny łańcuch w ciapkach
#define DEFINE_ALIAS_RTIME_TYPE2( _T_ , _STR_ , _ident_ )	\
IO_type_info_alias< _T_ > _io_info_alias_##_ident_   ( (#_STR_) );

// Definicje zmiennych niezbędnych do wejścia/wyjścia
// dla szablonów
#define DEFINE_VIRTUAL_NECESSARY_FOR_TEMPLATE( _list_ , _KLASA_ )	\
template _list_														\
IO_type_info< _KLASA_ > _KLASA_::_io_info;

/*
//#ifndef __DJGPP__
//#else
#define DEFINE_VIRTUAL_NECESSARY_FOR_TEMPLATE( _list_ , _KLASA_ ) \
// W DJGPP nie zaimplementowane niestety, trzeba osobno definiować
#endif
*/

//Definicja niezbędna, żeby maskować przecinki w powyższych definicjach
//używanych dla szablonów wieloparametrowych.
// Konieczna tylko dla starego (?) MSVC++
#define _COMA_  ,


class vobject  //: virtual public error_handling
{
/* SUPPORT FOR RUN TIME TYPE CONTROL */
VIRTUAL_NECESSARY_AND_IO(vobject)
private:
//Prevent for generate default operator!
// Also prevent define "operator =" by compiler for derived class (GNU?)
void operator=(const vobject&);

protected:
/* CONSTRUCTION/DESTRUCTION ONLY FOR DERIVED CLASS*/
vobject(){}             // Empty constructor
virtual ~vobject(){}	// Force destructoruction virtual
public:
virtual void finalise() const;	// run destructor if T::~T access is unaccessible

/* DYNAMIC - EXCEPTION MODE ERROR SUPPORT */
virtual /* Derived class can exchange this method for catch */
int Raise(const WB_Exception_base& e) const
	{ return error_handling::Error(e);/*Call exception handling or terminate*/ }

/* SUPPORT FOR STREAM OPERATIONS */
//IO_PUBLIC_DECLARE   // virtual i/o implementation is in  VIRTUAL_NECESSARY
public:
virtual int Save(const char* name) const;
virtual int Load(const char* name);
friend // Only this one declaration for all derived class!!!
ostream& operator << (ostream& o,const vobject& vo); // stream output function
friend // Only this one declaration for all derived class!!!
istream& operator >> (istream& i,vobject& vo); // stream input function
};

} //namespace

/* *******************************************************************/
/*			          WBRTM  version 2006-2022                       */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego        */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                 */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
#endif
