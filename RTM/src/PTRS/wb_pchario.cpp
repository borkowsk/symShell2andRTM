//	Special implementation of I/O for wb_sptr<char> and wb_pchar
// /////////////////////////////////////////////////////////////////////
#define		ANSI            /* Comment out for UNIX V version  (var_arg)   */

#include "excpbase.hpp"

#include <stdio.h>			/* vsprintf */
#include <ctype.h>

#ifdef ANSI             /* ANSI compatible version          */
#include <stdarg.h>
#else                   /* UNIX V compatible version          */
#include <varargs.h>
#error NOT TESTED
#endif

#include "wb_ptr.hpp"
#include "wb_ptrio.h"
#include "wb_pchario.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY


class _pchar_Raiser
{
public:
//#ifdef USE_WB_EXCPTIONS
	int Raise(WB_Exception_base const& E) const
	{
		E.PrintTo(cerr);
		cerr<<endl;
		return 0;
	}
//#endif
} Raiser;


ostream& operator<<(ostream& s,const wb_sptr<char>& p)
{
    assert("Not implemented"==nullptr);
	return s;
}

istream& operator>>(istream& s,wb_sptr<char>& p)
{
    assert("Not implemented"==nullptr);
	return s;
}
 
void escaped_pchar_write(ostream& s,const char* pch,char enclos)
{
	if(!pch)
	{
		s<<'@';
	}
	else
        if( *pch=='\0' || ::strchr(pch,enclos) || strpbrk(pch,wbrtm::WB_PCHAR_VERBOTEN_CHARS)!=nullptr ) //Pusty lancuch albo sa separatory itp
		{//Zapis bezpieczny

			WriteEnclosedString(s,pch,Raiser,enclos);
		}
		else
			s<<pch;//Zapis jako jedno slowo
}

ostream& operator<<(ostream& s,const wb_pchar& p)
{
	escaped_pchar_write(s,p.get());
	return s;
}


istream& operator>>(istream& s,wb_pchar& p)
{
	char pom;
	s>>pom;

	if(pom=='@')
	{
        p=nullptr;
	}
	else
		if(pom=='\"')
		{	
			s.putback(pom);
			p.take( ReadEnclosedString(s,Raiser,'\"') );
		}
		else
		{		
			char poms[1024]; //Wielki zaps
			s.putback(pom);
			s>>poms;
			assert(::strlen(poms)<1024);
			p.take(	clone_str(poms) );//Tylko wczytane
		}
		return s;
}


} // end of namespace wbrtm

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
