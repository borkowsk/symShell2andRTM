#include "vobject.hpp"
#include "iosuppor.hpp"

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

ReadDynamicVal<char>::ReadDynamicVal(istream& file,const vobject* user)
{
//cerr<<"ReadDynamicVal<char> construction\n";
ptr=ReadEnclosedString(file,'"',*user);
}

WriteDynamicVal<char>::WriteDynamicVal(ostream& file,const char* ptrval,const vobject* user)
{
//cerr<<"WriteDynamicVal<char>\n";
code=WriteEnclosedString(file,ptrval,*user,'"');
}

WriteDynamicVal<vobject>::WriteDynamicVal(ostream& file,const vobject* ptrval,const vobject* user)
{
//cerr<<"WriteDynamicVal<vobject>\n";
WriteEnclosedString(file,IO_database.NameOf(*ptrval),*user,'\"');
file<<' '<<*ptrval;
code = file.bad()?-1:0;
}

ReadDynamicVal<vobject>::ReadDynamicVal(istream& file,const vobject* user)
{
//cerr<<"ReadDynamicVal<vobject> construction\n";
char* name=ReadEnclosedString(file,'"',*user);
const IO_type_info_base* pom=IO_database.GetInfoPtr(name);
if(pom==NULL)
	{
	char bufor[256];
	sprintf(bufor,"%s is undefined for I/O",name);
	user->Raise(ExcpIO(NULL,file.tellg(),bufor));
	}
ptr=pom->Create();//tworzenie
file>>*ptr;//Wczytywanie
}

//Obiekty do uruchamiania Raise jesli uzywa sie
//funkcji iosupportu z poziomu bez dostepu do
//jakiegos obiektu pochodnego od vobject
_io_default_raiser IO_default_raiser;
_io_fail_raiser    IO_fail_raiser;

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