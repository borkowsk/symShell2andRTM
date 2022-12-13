// world.cpp: implementation of the world class.
//  	Virtual output methods and operator <<
// ----------------------------------------------
// Its is rather simply implementation. You can replace it if you need.
//*////////////////////////////////////////////////////////////////////
#include "world.hpp"

char world::separator='\t';

int world::implement_output(ostream& o) const
{
	o<<Licznik<<separator;
	o<<MaxIterations<<separator;
	o<<LogRatio<<separator;
//	assert("world::implement_output() not implemented");
	return 1;
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


