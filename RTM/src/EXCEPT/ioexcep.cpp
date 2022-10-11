//
// Przechwytywanie błędów programowych WBRTM i inne sztuczki - stare więc  nie wiadomo czy bezpieczne
//
// Implemetacja metod klasy ExcpIO
//-----------------------------------------------------------------------------
#include "INCLUDE/platform.hpp"

#include <iostream>
#include <cstring>
//#include <cassert>
#include "INCLUDE/ioexcep.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

void ExcpIO::PrintTo(ostream& er) const
{
er<<"IO error";
if(comm)
	er<<": \""<<comm<<'\"';
if(stream_name)
	er<<" in stream "<<stream_name;
if(position!=MAXOBJECTSIZE)
{
	er<<" near position "<<position;
	if(stream_ptr)
	{

		int pos=0;
		int len=80;

		if(position-len>0)
			pos=position-len;
		else
			pos=0;

		stream_ptr->seekg(pos);
		if(!stream_ptr->fail())
		{
			er<<"\nLook after following text (starting 80 characters before error):\n\"";
			for(;pos<position;pos++)
			{
				int pom=stream_ptr->get();
				if(pom!=-1/*EOF*/)
					er<<char(pom);
			}
			er<<"\""<<endl<<flush;
		}
	}
}
if(my_errno!=0)
	er<<" ERRNO:"<<strerror(my_errno)<<'\n';
}

} //namespace

/********************************************************************/
/*			          WBRTM  version 2006                           */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
