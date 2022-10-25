/** \file excpprin.cpp
 *  \author borkowsk
 *  \brief Print functions for wbrtm exceptions.
 *         ------------------------------------
 *  \date 2022-10-11 (last modification)
 */
#include "excpbase.hpp"
#include "excpmem.hpp"
#include "excpothr.hpp"
//#include <cstdlib>
#include <cstring> //strerror()
#include <iosfwd>
#include <iostream>
//#include <iomanip>
using std::ostream;

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

ostream& operator << (ostream& o,const WB_Exception_base& e)
{
    e.PrintTo(o); return o;
}

// C++ EXCEPTION MODE ERROR SUPPORT

/// \details Printing usable data from  ExcpRaisePosition.
void ExcpRaisePosition::PrintTo(ostream& o) const
{
if(file!=nullptr)
	o<<" in "<< file;
if(line!=0)
	o<<" at line "<<line;
}

/// \details Printing usable data from TextException.
void TextException::PrintTo(ostream& o) const
{
o<<comm;
RunTimeErrorExcp::PrintTo(o);
if(Code!=0)
	o<<" ["<<Code<<']';
}

/// \details Printing usable data from SystemExcp.
void SystemExcp::PrintTo(ostream& o) const
{
#ifdef _MSC_VER
	char bufor[1024];
	auto ret = strerror_s(bufor, 1024, Errno);
	o << "SYSTEM ERROR:" << ret << ": " << bufor;
#else
	o<<"SYSTEM ERROR:<<Errno<<": "<< ::strerror(Errno);
#endif
	ExcpRaisePosition::PrintTo(o);
}

/// \details Printing usable data from MemoryExcp.
void MemoryExcp::PrintTo(ostream& o) const
{
o<<"MEMORY EXCEPTION RAISED";
ExcpRaisePosition::PrintTo(o);
}

/// \details Printing usable data from NumericExcp.
void NumericExcp::PrintTo(ostream& o) const
{
o<<"NUMERIC EXCEPTION RAISED";
ExcpRaisePosition::PrintTo(o);
}

/// \details Printing usable data from RangCheckExcp.
void RangCheckExcp::PrintTo(ostream& o) const
{
o<<"RANGE CHECK EXCEPTION RAISED";
ExcpRaisePosition::PrintTo(o);
}

/// \details Printing usable data from SearchingExcp.
void SearchingExcp::PrintTo(ostream& o) const
{
o<<"SARCHING EXCEPTION RAISED";
ExcpRaisePosition::PrintTo(o);
}

/// \details Printing usable data from OutOfMemoryExcp.
void OutOfMemoryExcp::PrintTo(ostream& o) const
{
o<<"CAN'T ALLOCATE:"<< size <<"B. RAISED";
ExcpRaisePosition::PrintTo(o);
}

/// \details Printing usable data from InvalidPtrUseExcp.
void InvalidPtrUseExcp::PrintTo(ostream& o) const
{
o<<"INVALID POINTER IN USE:"<<std::hex<< ptr <<" RAISED";
ExcpRaisePosition::PrintTo(o);
}

/// \details Printing usable data from InvalidIndexExcp.
void InvalidIndexExcp::PrintTo(ostream& o) const
{
o<<"Invalid index "<< I <<" RAISED";
ExcpRaisePosition::PrintTo(o);
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

