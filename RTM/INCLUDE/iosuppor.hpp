#ifndef __IOSUPPORT_HPP__
#define __IOSUPPORT_HPP__

//#include "platform.hpp"
#include <errno.h>
#include <iostream>

#include "vobject.hpp"
#include "iocharp.hpp"
#include "ioexcep.hpp"
#include "excpmem.hpp"

#ifndef HIDE_WB_PTR_IO
#define HIDE_WB_PTR_IO 0
#endif

#include "wb_ptr.hpp" //Trzeba zaimplementowac dla nich I/O

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

inline
ostream& fill(ostream& o,char c,int n)//funkcja uzyteczna w formatowaniu wydrukow
{
	for(int i=0;i<n;i++) o<<c;
	return o;
}

class _io_default_raiser:public vobject
//Obiekt do uruchamiania Raise jesli uzywa sie
//funkcji iosupportu z poziomu bez dostepu do
//jakiegos obiektu pochodnego od vobject
{} /*IO_default_raiser*/;

class _io_fail_raiser:public vobject
//Obiekt do uruchamiania Raise zwracajacego 1 -> fail
//jesli uzywa sie funkcji iosupportu z poziomu bez dostepu do
//jakiegos obiektu pochodnego od vobject
{
public:
int Raise(const WB_Exception_base& e) const
	{
	e.SetRecoverable(1);//Aplikacja bedzie sie martwic
	error_handling::Error(e);//Call exception handling*/
	return 1;//Przerwij funkcje
	}
} /*IO_fail_raiser*/;

//Obiekty do uruchamiania Raise jesli uzywa sie
//funkcji iosupportu z poziomu bez dostepu do
//jakiegos obiektu pochodnego od vobject
extern _io_default_raiser IO_default_raiser;
extern _io_fail_raiser    IO_fail_raiser;

//Wczytanie wskaznika zawsze uwazane za bledne. C++ nie definiuje takich
//operatorow ale moga byc potrzebne w virtualnych funkcjach
//(nigdy nie uzywanych ale) generowanych z szablonow
template<class T>
istream& operator >> (istream& i,T*& ptr)
{
assert("Reading raw pointer is always an error");
IO_fail_raiser.Raise(
			ExcpIO(NULL,i.tellg(),"Reading raw pointer is always an error"));
ptr=NULL;
return i;
}

class _io_database
//Klasa zapewniajaca interface do dynamicznych danych
//o typach koniecznych dla zapisywania i odtwarzania
//obiektow heterogenicznych.
//Uzywa RTTI jesli jest dostepne. Jesli nie to jego namiastki z wbrtm
{
static void*	   assoctable;//Wypelniane przy pierszym uzyciu
public:
// Rejestracja dynamiczna.Potrzebna dla recznie laczonych bibliotek.
int   RegistryType(const char* name,const IO_type_info_base&);
// Przeszukiwanie
const IO_type_info_base* GetInfoPtr(const char* name)const;//NULL jesli brak w bazie
const IO_type_info_base& GetInfoFor(const char* name)const;//z wyjatkiem!
const IO_type_info_base& GetInfoOf(const vobject&  v)const //zwroci IO_info::vobject jesli brak!
	{   return v._IO_info(); }
// Nazwa typu dla zapisu
#ifdef _CPPRTTI
// na skroty poprzez RTTI
const char* NameOf(const vobject& v)
	{ 	return typeid(v).name(); }
#else
// niech sie martwi klasa IO_type_info<...>
const char* NameOf(const vobject& v)
	{ 	return GetInfoOf(v).Name(); }
#endif
};

extern _io_database IO_database; //Jedyny obiekt tej klasy

template<class T>
class WriteDynamicVal
// Klasa funkcyjna zapisuje obiekt wskazywany przez wskaznik, domyslnie
// uzywa operatora << zdefiniowanego dla obiektu wskazywanego
{
int code;
public:
WriteDynamicVal(ostream& file,const T* ptrval,const vobject* user=&IO_default_raiser)
	{
	file<<*ptrval;
	if(file.bad())//Bada sukces
		{
		user->Raise(ExcpIO(NULL,file.tellp(),"WriteDynamicVal failed"));
		code= -1;
		}
	code=0;
	}
operator int () { return code;}
};

template<class T>
class ReadDynamicVal
// Klasa funkcyjna.
// Wczytuje obiekt okreslonego typu alokujac w pamieci dynamicznej
// miejsce na niego i zwracajac wskaznik
{
T* ptr;
public:
ReadDynamicVal(istream& file,const vobject* user=&IO_default_raiser)
	{
	//cerr<<"template ReadDynamicVal construction\n";
	T* pom=new T;//tworzy nowy obiekt typu T

	if(pom==NULL)
		{
		// gcc_dummy( sizeof(T) , __FILE__ , __LINE__ );
		user->Raise(  OutOfMemoryExcp( sizeof(T) , __FILE__ , __LINE__ ) );
		goto END;
		}

	file>>*pom; //wczytuje zawartosc

	if(file.bad())
		{
		user->Raise( ExcpIO( NULL , file.tellg() , "ReadDynamicVal failed" ) );
		delete pom;pom=NULL;goto END;
		}

	END:
	ptr=pom;
	}
operator T* () { return ptr; }
};

// Warianty z domyslnym obiektem dla funkcji pomocniczych
//			wejscia/wyjscia kodowanych lancuchow.
//---------------------------------------------------------------
inline
int  IgnoreToEOF(istream& file,char delimiter='\n',const vobject& user=IO_fail_raiser)
// Funkcja ignoruje wszystko do znaku=delimiter lub konca pliku
// Zwraca delimiter lub EOF
{ return IgnoreToEOF(file,user,delimiter);}

/*
inline
char* ReadToEOF(istream& file,char delimiter='\n',const vobject& user=IO_fail_raiser,bool NULLIfEmpty=true)
//Czyta do konca lini, lub do konca pliku
{ return ReadToEOF(file,user,delimiter,NULLIfEmpty);}
*/

inline
char* ReadToEOF(istream& file,char delimiter='\n',bool NULLIfEmpty=true,const vobject& user=IO_fail_raiser)
//Czyta do konca lini, lub do konca pliku
{ return ReadToEOF(file,user,delimiter,NULLIfEmpty);}

/*
inline
char* ReadToEOF(istream& file)
//Czyta do konca lini, lub do konca pliku
{ return ReadToEOF(file,IO_fail_raiser,'\n',true);}
*/

inline
char* ReadEnclosedString(istream& file,char delimiter='"',const vobject& user=IO_fail_raiser)
// Czyta lancuch w cudzyslowach lub czyms zamiast,
// zwraca NULL jesli blad-np brak pamieci lub brak zamkniecia
{ return ReadEnclosedString(file,user,delimiter);}

inline
int WriteEnclosedString(ostream& file,const char* str,char delimiter='"',const vobject& user=IO_fail_raiser)
// Zapisuje lancuch w cudzyslowie, oznaczajac znaki specjlne
// Zwraca 0 lub -1 w wypadku zamaskowanego bledu
{ return 	WriteEnclosedString(file,str,user,delimiter);}

// Specjalizacja dla \typu char* ze wzgledu na jego nietypowosc
//------------------------------------------------------------------
template<> //explicit specialisation of class
class WriteDynamicVal<char>
{
int code;
public:
WriteDynamicVal(ostream& file,const char* ptrval,const vobject* user=&IO_default_raiser);
operator int () { return code;}
};

template<>	//New syntax - explicit specialization of class template
class ReadDynamicVal<char>
{
char* ptr;
public:
ReadDynamicVal<char>(istream& file,const vobject* user=&IO_fail_raiser);
operator char* () {return ptr;}
};

// Specjalizacja dla typow pochodnych od vobject
// implementujaca inteligentne zapisywanie/odczytywanie
// do uzycia jedynie jako baza dla klas potomnych
// K o n i e c z n y c h redefinicja dla klas potomnych od vobject
//-------------------------------------------------------------------------------------
template<>	//New syntax - explicit specialization of class template
class WriteDynamicVal<vobject>
{
int code;
public:
WriteDynamicVal(ostream& file,const vobject* ptrval,const vobject* user=&IO_default_raiser);
operator int () { return code;}
};

template<>	//New syntax - explicit specialization of class template
class ReadDynamicVal<vobject>
{
vobject* ptr;
public:
ReadDynamicVal<vobject>(istream& file,const vobject* user=&IO_fail_raiser);
operator vobject* () {return ptr;}
};

// Makro dla latwego definiowania ze klasa bazowa dla IO
// Deklaracje konieczne zeby typ byl traktowany jako typ bazowy
// rodziny klas zdatnej do uzycia w heterogenicznych kontenerach
// Wymusza wyjatek jesli typ jest niezutowalny
//----------------------------------------------------------------------

//WOGOLE PRZESTARZALE
//#if defined( _CPPRTTI  ) || defined( GCC )
//#define IO_DYNAMIC_CAST( _KLASAC_ ) dynamic_cast< _KLASAC_& >
//#else
//#define IO_DYNAMIC_CAST( _KLASAC_ ) (_KLASAC_&)
//#endif


#define DECLARE_AS_HETEROGENIC_ROOT( _KLASA_ )							\
template<>																\
class ReadDynamicVal< _KLASA_ >:public ReadDynamicVal<vobject>			\
{public:																\
ReadDynamicVal< _KLASA_ >(istream& file,								\
			const vobject* user=&IO_fail_raiser):						\
			ReadDynamicVal<vobject>(file,user){}						\
operator _KLASA_ * ()													\
{																		\
vobject* pom1=((ReadDynamicVal<vobject>*)this)->operator vobject*();	\
_KLASA_	&pom=dynamic_cast< _KLASA_& >(*pom1);							\
return &pom;															\
}																		\
};																		\
template<>																\
class WriteDynamicVal< _KLASA_ >:public WriteDynamicVal<vobject>		\
{public:																\
WriteDynamicVal(ostream& file,const  _KLASA_* ptrval,					\
				const vobject* user=&IO_default_raiser):				\
	WriteDynamicVal<vobject>(file,ptrval,user){}						\
};

//To samo dla szablonow
//----------------------------
#define DECLARE_TEMPLATE_AS_HETEROGENIC_ROOT( _list_ , _KLASASZ_ )		\
template _list_ 														\
DECLARE_AS_HETEROGENIC_ROOT( _KLASASZ_ )



//Definicje  I/O dla inteligentnych wskaznikow
////////////////////////////////////////////////

#ifndef WB_PTRIO_DEFINED  //Jak już wcześniej zdefiniowane to nie używamy tych
template<class T>
ostream& operator << (ostream& o,const wb_sptr<T>& p)
{
if(p)
        WriteDynamicVal<T>(o,p.get_ptr_val());
        else
        o<<'@';
return o;
}

template<class T>
istream& operator >> (istream& i,wb_sptr<T>& p)
{
char znak;
i>>znak;
if(znak!='@')
        {
        i.putback(znak);
        p=ReadDynamicVal<T>(i);
        }
        else
        p=NULL;
return i;
}

template<class T>
ostream& operator << (ostream& o,const wb_dynarray<T>&)
{
assert("Not implemented"==NULL);
return o;
}

template<class T>
istream& operator >> (istream& i,wb_dynarray<T>&)
{
assert("Not implemented"==NULL);
return i;
}
#endif //WB_PTRIO_DEFINED

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
