/**
* \file
* \brief   Generic "input/output" exception.
*        ----------------------------------------
* \details
*          Contents:
*          - class wbrtm::ExcpIO;
*
* \date    2022-10-12 (last modification)
* \author  borkowsk
*/

#ifndef __IOEXCEPTION_HPP__
#define __IOEXCEPTION_HPP__

#include <cerrno>
#include <iostream>
#include "wb_ptr.hpp"
#include "excpbase.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \brief Typ wyjątku do zastosowania w operacjach wejścia/wyjścia na strumieniach.
/// \note  Zakładamy ze można spokojnie alokować pamięć dla łańcuchów z treścią komunikatu.
class ExcpIO:public WB_Exception_base
{
	EXCP_NECESSARY(ExcpIO)
protected:
	wb_pchar	 	stream_name;
	istream*		stream_ptr;
	object_size_t		position;
	int			my_errno;
	wb_pchar		comm;
public:
	//ZWYKLY KONSTRUKTOR
	ExcpIO(const char* sname,
		const object_size_t posit=MAXOBJECTSIZE,
		const char*			icomm=NULL,
		const int			e=-1,
		istream*			iptr=NULL
		):
		stream_name(sname),stream_ptr(iptr),
		position(posit),my_errno(e),comm(icomm)
	{
		if(my_errno==-1)
			my_errno= errno ;
	}

	//KONSTRUKTOR KOPIUJACY
	ExcpIO(const ExcpIO& w):
		stream_name(w.stream_name),	//Kopiowanie wb_pchar�w!!!
		stream_ptr(w.stream_ptr),position(w.position),
		my_errno(w.my_errno),comm(w.comm)
	{}

	//DESTRUKTOR VIRTUALNY
	virtual ~ExcpIO()
	{}

	//Wypisywanie na strumien (virtualne)
	void PrintTo(ostream&) const;
};


} //namespace

/* *******************************************************************/
/*			           WBRTM  version 2022                           */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*         WWW:  ...                                                 */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
#endif
