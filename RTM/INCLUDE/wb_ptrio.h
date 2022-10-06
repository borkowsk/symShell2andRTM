//  Definicje I/O - dla inteligentnych wskaznikow
//  Uwaga!!! Nie sprawdzaja bledow.
// ///////////////////////////////////////////////
#ifndef _WB_PTRIO_H_
#define _WB_PTRIO_H_
#include "platform.hpp"
#include "wb_ptr.hpp"

#if	HIDE_WB_PTR_IO != 1
#define WB_PTRIO_DEFINED
#include <iostream>

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

template<class T>
ostream& operator << (ostream& ss,const wb_sptr<T>& pp)
{
	if(int(pp))
		ss<<*pp;
	else
		ss<<'@';
	return ss;
}

template<class T>
istream& operator>>(istream& s,wb_sptr<T>& p)
{
    char pom='@';
    s>>pom;
    if(pom=='@')
    {
        p=NULL;
    }
    else
    {
        s.putback(pom);
        p= new T;
        s>>(*p);
    }
    return s;
}


template<class T>
ostream& operator<<(ostream& s,const wb_dynarray<T>& p)
{
	s<<'{'<<' ';
	s<<p.get_size()<<' ';
	for(size_t i=0;i<p.get_size();i++)
		s<<p[i]<<' ';
	s<<'}'<<endl;
	return s;
}

template<class T>
istream& operator >> (istream& s, wb_dynarray<T>& p)
{
	char cpom;
	size_t i;

	//Wczytanie klamry otwierajacej
	s>>cpom;
	assert(cpom=='{');//if(pom!='{') goto ERROR;

	{
	//Wczytanie rozmiaru i alokacja
	size_t size;
	s>>size;
	p.alloc(size);
	//Wczytanie calej tablicy
	for(i=0;i<size;i++)
		s>>p[i];
	
	//Wczytanie zamykajacej klamry
	s>>cpom;
	assert(cpom=='}');//if(pom!='}') goto ERROR;
	}

	return s;
}

template<class T>
ostream& operator<<(ostream& s,const wb_dynmatrix<T>& p)
{
	return wb_dynarray<wb_dynarray<T> >::operator << (s,p);
}

template<class T>
istream& operator>>(istream& s,wb_dynmatrix<T>& p)
{
	return wb_dynarray<wb_dynarray<T> >::operator >> (s,p);
}

#endif
} //namespace

/* *******************************************************************/
/*			          WBRTM  version 2006                            */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego        */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                 */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
#endif
