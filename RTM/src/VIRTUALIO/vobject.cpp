#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

#include "vobject.hpp"
#include "wb_ptr.hpp"
#include "errorhan.hpp"
#include "ioexcep.hpp"
#include "excpmem.hpp"

#include "compatyb.hpp" // użyteczne funkcje nie wszędzie zaimplementowane

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

DEFINE_VIRTUAL_NECESSARY_FOR(vobject)

const char* IO_type_info<vobject>::Name()const
{
return "derived of vobject pure type";
}

vobject*  IO_type_info<vobject>::Create()const
{
assert("IO_type_info<vobject> used");
error_handling::Error(
TextException("IO_type_info<vobject> used", error_handling::INTERNAL_FAULT,
						__FILE__,__LINE__));
return nullptr;
}

size_t    IO_type_info<vobject>::SizeOf()const
{
assert("IO_type_info<vobject> used");
error_handling::Error(
TextException("IO_type_info<vobject> used", error_handling::INTERNAL_FAULT,
						__FILE__,__LINE__));
return sizeof(vobject);
}


void vobject::finalise() const	// run destructor if T::~T construction
{ 	                // is unaccessible
WARNING("vobject::finalise() called");
//#if defined( __BORLANDC__ ) || defined( __MSVC__ )
//(*(vobject*)this).~vobject();
//#else
this->~vobject();
//#endif
}

ostream& operator << (ostream& o,const vobject& vo) // stream output function
{
o<<'{';//A spacja oddzielajaca ???
assert(o. good());
vo.implement_output(o);
assert(o.good());
o<<'}';
assert(o.good());
return o;
}

istream& operator >> (istream& i,vobject& vo) // stream input function
{
assert((!i.fail()) && i.good());
char zn='\0';

//Musi zaczynac sie od { conajwyzej poprzedzonego blank'ami
i>>zn;

if(zn!='{')
    if(vo.Raise(ExcpIO(nullptr,i.tellg(),"expected { for begin of vobject data."))==1)
	return i;

assert(i.good() && !i.eof());
vo.implement_input(i);

assert(i.good() && !i.eof());

i>>zn;//Musi konczyc sie na }
if(zn!='}')
{
    ostringstream pom;
	i.putback(zn);
	pom<<"At the end of reading vobject::, '}' expected but '"<<zn<<"'(#"<<unsigned(zn)<<") found.\n----->";
	for(int j=0;j<20;j++)	//Kopiowanie fragmentu za
		if(!i.eof())
			pom<<char(i.get());
	pom<<"<------"<<char(0);

    if( vo.Raise( ExcpIO(nullptr,i.tellg(),pom.str().c_str(),-1,&i) )==1)
		return i;
}

assert(!i.bad());
return i;
}

int vobject::Load(const char* name)
{
ifstream i(name);
//ifstream i(name,ios::in|ios::_Nocreate); //NIE DZIAŁAJĄ
//ifstream i(name,ios::in|ios::nocreate);

if(i.fail())  // blad otwarcia pliku
    if(Raise(ExcpIO(name,MAXOBJECTSIZE,"open read stream for vobject"))==1)
	return 0;

assert(i.good());
i>>*this;// Uzycie standardowego operatora

assert(i.good());

return 1;
}

int vobject::Save(const char* name) const
{
fstream o(name);
//ofstream o(name,ios::out,ios_base::_Openprot); //NIE DZIAŁAJĄ :-(
//ofstream o(name,ios::out,filebuf::openprot);

if(o.fail())  // blad otwarcia pliku
	if(Raise(ExcpIO(name,MAXOBJECTSIZE,"open write stream for vobject"))==1)
		return 0;

assert(o.good());

o<<*this;// Uzycie standardowego operatora

if(o.bad())  // blad zapisu do pliku
	if(Raise(ExcpIO(name,o.tellp(),"write"))==1)
		return 0;
return 1;
}



//STREAM & BINARY IO IMPLEMENTATION
/* Those methods must be defined in all derived class */
/* but not declared as pure for simple code developing   */
void vobject::implement_output(ostream&)      const
	{ Raise(FATAL("Method not implemented"));}

void vobject::implement_input(istream&)
	{ Raise(FATAL("Method not implemented"));}

/*void vobject::implement_encode(base_encoder&) const
	{ Raise(FATAL("Method not implemented"));}
void vobject::implement_decode(base_decoder&) const
	{ Raise(FATAL("Method not implemented"));}*/

} //namespace

/********************************************************************/
/*                   WBRTM  version 2006/2020                       */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
