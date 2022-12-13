// world.cpp: implementation of the world class.
//	output operator <<
//----------------------------------------------
// Its is rather simply implementation. You can replace it if you need.
//*////////////////////////////////////////////////////////////////////
#include "world.hpp"

ostream& operator << (ostream& o,const world& w)
{
	if(!o || o.bad())
	{
		cerr<<"Sorry! Invalid or bad stream."<<endl;
		exit(2);
	}

	o<<w.SimulName<<w.separator;
	if(o.fail())
	{
		cerr<<"Sorry! Write failed."<<endl;
		exit(2);
	}

	o<<'{'<<endl;
	w.implement_output(o); //Call virtual output method.
	o<<'}'<<endl;

	if(o.fail())
	{
		cerr<<"Sorry! Write failed."<<endl;
		exit(2);
	}
	return o;
}


/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/


