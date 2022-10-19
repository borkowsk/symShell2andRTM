//
// Przechwytywanie błędów programowych WBRTM i inne sztuczki - stare więc  nie wiadomo czy bezpieczne
// Zobacz: https://www.thegeekstuff.com/2010/10/linux-error-codes/
//*///////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sstream>
#include <cstdio> //scanf? TODO
#include "INCLUDE/errorhan.hpp"
#include "INCLUDE/excpbase.hpp"
#include "INCLUDE/wb_ptr.hpp"


// ABSOLETE!
#if defined( __WIN32__ ) || defined( __WIN16__ )
#include <windows.h> //#include <Winuser.h>  ?
#endif

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

static int juz_bylo=0;
static int in_error_flag=0;

/// \brief Internal function
void _high_endln_forcer()
{
if(cerr.bad() || cerr.fail()) //Jeśli coś nie tak
		return;

if(juz_bylo)
		return;

if(WB_error_enter_before_clean)
	{
	cerr<<"(Press ENTER)\n";
	cin.get();
	WB_error_enter_before_clean=0;
	}

cerr<<'\n';
juz_bylo=1;
}

// C++ EXCEPTION MODE ERROR SUPPORT
/// \brief Internal class
static class _premain_force_endl
{
public:
_premain_force_endl()
{
#if !defined( NDEBUG ) && !defined( WBRTM_NOLOGO )
#if defined( __WIN32__ ) || defined( __WIN16__ )
   const char* msg="compilation: "__DATE__" "__TIME__;
   MessageBox(nullptr,msg,"WBRTM - DEBUG VERSION ",MB_OK);
#else
if( cerr.flags() & unitbuf !=0 ) //Może go jeszcze nie być, ale to nie jest sposób na sprawdzenie. :(
	cerr<<"WBRTM - DEBUG VERSION "<<__DATE__<<' '<<__TIME__<<'\n';
#endif
#endif
//atexit(high_endln_forcer);//Nie ma sposobu sprawdzenia, ze std::cerr jeszcze nie wywołało destruktora!!!
}

~_premain_force_endl()
{
//high_endln_forcer();//Nie ma sposobu sprawdzenia, ze std::cerr jeszcze nie wywołało destruktora!!!
}

} _preamain_forcer; ///< singleton

/// \details Very simple version for compiler without exception support
int error_handling::Error(const WB_Exception_base& e)
{
cerr<<e<<'\n';

if(!e.Recoverable())
	{
	if(in_error_flag==0)
		{
		in_error_flag=1;
#if defined( __WIN32__ ) || defined( __WIN16__ )
        ostringstream out;
        out<<e<<'\0';
        char* bufor=out.str(); //TODO???
        wb_sptr<char> Zwalniacz(bufor);
        MessageBox(nullptr,bufor,"Application error",MB_ICONHAND);
#else
		if(WB_error_enter_before_clean)
		  {
		  cerr<<"\nApplication error""\n(Press ENTER to exit)\n";
		  cin.get();
		  WB_error_enter_before_clean=0;// Juz zrobione
		  }
#endif
		exit(e.ExitCode());
		}
		else
		{
		cerr<<"Nested error! ABORTED\n";
		abort();
		}
		//return 1; //I tak nigdy nie powinien tu wejść
	}
	else
	{//Caller may try to recovery
	// Return 1 if try to cleanup
	// Return 0 if try to resume
	//OK zwraca 1, CANCEL zwraca 2
	#if defined( __WIN32__ ) || defined( __WIN16__ )
    ostringstream out;
    out<<e<<'\0';
    char* bufor=out.str(); //TODO???
    wb_sptr<char> Zwalniacz(bufor);
    return 	MessageBox(nullptr,bufor,"Warning or recoverable error",MB_ICONWARNING+MB_OKCANCEL)-1;
	#else
	int ret=0; //PROWIZORKA {????????????}
	cerr<<"Warning or recoverable error.\b\b"<<endl;
	cerr<<"Chose action:\n\t0\t:\tOK(resume)\n\t1\t:\tCANCEL(cleanup)\n\t2\t:\tzzzzz\n ??? > "<<flush;
	cin>>ret;
	return ret;
	#endif
	}
}

static const char* ecomm[]={
        "OK=0",
        "nullptr USE","ALLOC ERROR","NOT FOUND","INVALID KEY","INTERNAL FAULT",
        "RANGE ERROR","I/O ERROR",
        "OTHER ERROR"};

void RunTimeErrorExcp::PrintTo(ostream& o) const
{
if(Code>0)
   {
      if(Code < err_handl::OTHER_ERROR)
		o<<"\n\""<<(Code<sizeof(ecomm)/sizeof(ecomm[0])?ecomm[Code]:ecomm[sizeof(ecomm)/sizeof(ecomm[0])-1])
		<<"\" ";
      else
        o<<" UNKNOWN ERROR ";
   }

ExcpRaisePosition::PrintTo(o);
}

} //namespace

/* *******************************************************************/
/*            WBRTM  version 2006 - renovation 2022                  */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  ...                                                  */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
