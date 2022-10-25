/** \file ioexcep.cpp
 *  \author borkowsk
 *  \brief Catching 'wbrtm' errors and other tricks.
 *         Old so, it is not known if it is safe yet.
 *  \date 2022-10-25 (last modification)
 */

#include <iostream>
#include <cstring>
#include "compatyb.h"
#include "ioexcep.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \details Implementacja metody PrintTo klasy ExcpIO
	void ExcpIO::PrintTo(ostream& er) const
	{
		er << "IO error";

		if (comm)
			er << ": \"" << comm << '\"';

		if (stream_name)
			er << " in stream " << stream_name;

		if (position != MAXOBJECTSIZE)
		{
			er << " near position " << position;
			if (stream_ptr)
			{

				int pos = 0;
				int len = 80;

				if (position - len > 0) //TODO size_t vs. int
					pos = position - len;
				else
					pos = 0;

				stream_ptr->seekg(pos);
				if (!stream_ptr->fail())
				{
					er << "\nLook after following text (starting 80 characters before error):\n\"";
					for (; pos < position; pos++)
					{
						int pom = stream_ptr->get();
						if (pom != -1/*EOF*/)
							er << char(pom);
					}
					er << "\"" << endl << flush;
				}
			}
		}

		if (my_errno != 0)
		{
#ifndef __MSVC__
			er << " ERRNO:" << my_errno << ": " << ::strerror(my_errno) << '\n';
#else
			char bufor[1024];
			er << " ERRNO:" << ::strerror_s(bufor,1024,my_errno) << ": "<< bufor << '\n';
#endif
		}

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