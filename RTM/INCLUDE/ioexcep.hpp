/**
* \file
* \brief
*       @PL{  }
*       @EN{ Generic "input/output" exception. }
* @date 2026-05-19 (last modification)
*       ----------------------------------------
* \details
*          Contents:
*          - class wbrtm::ExcpIO;
*
* @note Cała biblioteka WB_RTM to jest nieco odremontowane muzeum kodu z wieku XX.!!!
*
* \author  borkowsk
* @ingroup ERRORHANDLING
*/

#ifndef __IOEXCEPTION_HPP__
#define __IOEXCEPTION_HPP__

#include <cerrno>
#include <iostream>
#include "wb_ptr.hpp"
#include "excpbase.hpp"

/**
 * @ingroup ERRORHANDLING Funkcje o klasy obsługi sytuacji błędnych i wyjątkowych
 */
///@{

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \brief Typ wyjątku do zastosowania w operacjach wejścia/wyjścia na strumieniach. \ingroup ERRORHANDLING
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
	//ZWYKŁY KONSTRUKTOR
	ExcpIO(const char* sname,
           const object_size_t posit=MAXOBJECTSIZE,
           wb_pchar            icomm= nullptr,
           const int			e=-1,
           istream*			iptr=NULL
		):
		stream_name(sname),stream_ptr(iptr),
		position(posit),my_errno(e),comm(icomm)
	{
		if(my_errno==-1)
			my_errno= errno ;
	}

	//KONSTRUKTOR KOPIUJĄCY
	ExcpIO(const ExcpIO& w):
		stream_name(w.stream_name),	//Kopiowanie wb_pchar-ów!!!
		stream_ptr(w.stream_ptr),position(w.position),
		my_errno(w.my_errno),comm(w.comm)
	{}

	//DESTRUKTOR WIRTUALNY
	virtual ~ExcpIO()
	{}

	//Wypisywanie na strumień (wirtualne)
	void PrintTo(ostream&) const;
};


} //namespace
///@}

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
