/** \file ioexcep.cpp
 *  \author borkowsk
 *  \brief Catching 'wbrtm' errors and other tricks.
 *         Old so, it is not known if it is safe yet.
 *  \date 2022-10-11 (last modification)
 */

#include <iostream>
#include <cstring>

#include "ioexcep.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \details Implementacja metody PrintTo klasy ExcpIO
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

/* *******************************************************************/
/*	       WBRTM  version 2006 - renovated in 2022                   */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  htt...                                               */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/