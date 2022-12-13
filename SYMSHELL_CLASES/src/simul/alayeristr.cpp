// any_layer class:
//		Input operator >>
//----------------------------------------------
// Its is rather simply implementation. You can replace it if you need.
//*////////////////////////////////////////////////////////////////////
#include "layer.hpp"

using namespace std;

istream& operator >> (istream& i,any_layer_base& w)
{
	char cpom;
	i>>cpom;
	if(cpom!='{')
	{
		cerr<<"Incorrect syntax: '{' expected."<<endl;
	}

	w.implement_input(i); //Call virtual input method.

	i>>cpom;
	if(cpom!='}')
	{
		cerr<<"Incorrect syntax: '}' expected."<<endl;
	}
	return i;
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

