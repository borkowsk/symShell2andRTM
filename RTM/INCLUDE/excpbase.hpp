/*
Base exception class, and derived simple multipurpose exception classes:
------------------------------------------------------------------------

class WB_Exception_base;
class 	ExcpRaisePosition;
class		SystemExcp;
class 		MemoryExcp;
class 		NumericExcp;
class 		RangCheckExcp;
class		SearchingExcp;
class		RunTimeErrorExcp;
class 		TextException;

*/

#ifndef _EXCEPTION_BASE_HPP_
#define _EXCEPTION_BASE_HPP_
//#include "tnames.h"

#ifdef __DJGPP__
#pragma interface
#endif
#if _MSC_VER > 1000
#pragma warning( disable : 4512 )		//assignment operator could not be generated
#endif // _MSC_VER > 1000

#include <iostream>
//using namespace std;

// Najwiekszy rozmiar obszaru danych dla wyjatku - mozna przedefiniowywac dalej
#define EXCEPTION_MAX_SIZE 1024

// Metody niezbedne dla ogolnego stosowania
#define EXCP_NECESSARY(_P_)   \
public: \
virtual const char* TypeName() const		{ return #_P_ ; } \
virtual size_t SizeOf() const				{ return sizeof(_P_);}\
virtual WB_Exception_base* clone() const	{ return new _P_ (*this);}\
private:

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY
    // Używane dalej aliasy do typów i rozmiarów.
    // Dziwne czasem, z przyczyn historycznych.
    typedef void*  pointer;
    typedef size_t object_size_t;
    const   auto MAXOBJECTSIZE=SIZE_MAX;

// Pure base for all exceptions in WB-library
class WB_Exception_base
{
// VMT - exist ! Virtual function exist.
EXCP_NECESSARY(WB_Exception_base)
int not_recoverable;//0 gdy program moze byc kontynuowany.
protected:
	WB_Exception_base():not_recoverable(1){}
public:
virtual int SetRecoverable(int v) const
	{
	int pom=not_recoverable;
	(const_cast<WB_Exception_base*>(this))->not_recoverable=!v;
	return !pom;
	}
virtual ~WB_Exception_base() {}
virtual void PrintTo(std::ostream&) const {}
virtual int  Recoverable() const
	{ return !not_recoverable; }
virtual int  ExitCode() const
	{ return -1; }
friend
std::ostream& operator << (std::ostream& o,const WB_Exception_base& e);
};

class ExcpRaisePosition:public WB_Exception_base
{
EXCP_NECESSARY(ExcpRaisePosition)
protected:
const char* file;
const int   line;
ExcpRaisePosition(const char* fname,const int fline):
	file(fname),line(fline) {}
virtual void PrintTo(std::ostream&) const;
};

// Base for all exception dependend from OS
class SystemExcp:public ExcpRaisePosition
{
EXCP_NECESSARY(SystemExcp)
protected:
int Errno;
public:
SystemExcp(int cur_errno,const char* fname,const int fline):
	ExcpRaisePosition(fname,fline),
	Errno(cur_errno)
	{}
int  ExitCode() const    { return Errno; }
void PrintTo(std::ostream&) const;
};



// Base for all memory menaging exception
class MemoryExcp:public ExcpRaisePosition
{
EXCP_NECESSARY(MemoryExcp)
public:
MemoryExcp(const char* fname=0,const int fline=0):ExcpRaisePosition(fname,fline){}
int  ExitCode() const    { return 1; }
void PrintTo(std::ostream&) const;
};


// Base for all numeric & math exception
class NumericExcp:public ExcpRaisePosition
{
EXCP_NECESSARY(NumericExcp)
public:
NumericExcp(const char* fname=0,const int fline=0):ExcpRaisePosition(fname,fline){}
int  ExitCode() const    { return 1; }
void PrintTo(std::ostream&) const;
};


// Base for all ranging & indexing exception
class RangCheckExcp:public ExcpRaisePosition
{
EXCP_NECESSARY(RangCheckExcp)
public:
RangCheckExcp(const char* fname=0,const int fline=0):ExcpRaisePosition(fname,fline){}
int  ExitCode() const    { return 1; }
void PrintTo(std::ostream&) const;
};

// Base for all search & (not)found exception
class SearchingExcp:public ExcpRaisePosition
{
EXCP_NECESSARY(SearchingExcp)
public:
SearchingExcp(const char* fname=0,const int fline=0):ExcpRaisePosition(fname,fline){}
int  ExitCode() const    { return 1; }
void PrintTo(std::ostream&) const;
};

// Base for old style Runtime Errors
class RunTimeErrorExcp:public ExcpRaisePosition
{
EXCP_NECESSARY(RunTimeErrorExcp)
protected:
int Code;
public:
RunTimeErrorExcp(int icode,const char* fname,const int fline):
	ExcpRaisePosition(fname,fline),
	Code(icode)
	{}
int  ExitCode() const    { return Code; }
virtual void PrintTo(std::ostream&) const;
};


// Generic=text exception rather for code developing
class TextException:public RunTimeErrorExcp
{
EXCP_NECESSARY(TextException)
protected:
const char*		 comm;
public:
TextException(const char* icomm,const int icode=-1,const char* fname=0,const int fline=0):
	RunTimeErrorExcp(icode,fname,fline),comm(icomm)
	{
	}
~TextException()
	{
	}
int  Recoverable() const
	{ return Code==0; }
int  ExitCode() const
	{ return Code; }
virtual void PrintTo(std::ostream&) const;
};


#define EXCEPTION( _CM_ , _cd_ ) wbrtm::TextException((_CM_),(_cd_),__FILE__,__LINE__)
#define WARNING( _CM_ ) 	 wbrtm::TextException((_CM_),0,__FILE__,__LINE__)
#define FATAL( _CM_ )		 wbrtm::TextException((_CM_),5,__FILE__,__LINE__)
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
