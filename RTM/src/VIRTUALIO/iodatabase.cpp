// Bardzo prosta implementacja bazy klas inteligentnego I/O w postaci
// listy liniowej
//*//////////////////////////////////////////////////////////////////////////
#include "iosuppor.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

IO_type_info_base::IO_type_info_base(void)
//Konstruktor odpowiada za dołączenie do listy informacyjnej
{
	next=top;
	top=this;
}

const IO_type_info_base& _io_database::GetInfoFor(const char* name)const
//generuje wyjątek, gdy brak w bazie.
{
	const IO_type_info_base* pom=IO_database.GetInfoPtr(name);
	if(pom==NULL)
	    error_handling::Error(
	    		TextException(name,error_handling::NOT_FOUND,"\"IO DATABASE\""));
	return *pom;
}

//Szczyt listy informacyjnej dla inteligentnego i/o
IO_type_info_base*	IO_type_info_base::top=NULL;

//Pomocnicza, zeby nie przeszukiwac ciagle od nowa jak sie powtarza
static IO_type_info_base*	last=NULL;

const IO_type_info_base* _io_database::GetInfoPtr(const char* name)const
//NULL jeśli brak w bazie
{
IO_type_info_base*	current=IO_type_info_base::top;

//Zanim wchodzi w petle to sprawdza czy juz aby ostatnio nie znalazl takiego
if(last!=NULL && ::strcmp(last->Name(),name)==0)
	return last;

while(current!=NULL)
	{
    const char* ptr_to_type_name=current->Name();                               assert(ptr_to_type_name!=NULL);
   // cerr<<ptr_to_type_name<<endl;
	if(::strcmp(ptr_to_type_name,name)==0)
		{
		last=current;
		return current;
		}
	current=current->next;
	}
return NULL;
}

//Jedyny obiekt typu _io_database
_io_database IO_database;

} //namespace

/********************************************************************/
/*                    WBRTM  version 2006                           */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
