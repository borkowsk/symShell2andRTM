// IMPLEMENTACJA PODSTAWOWYCH KLAS GRAFÓW
//*/////////////////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

//#include "INCLUDE/platform.hpp"
#include <cstdlib>
#include <cstdio> //sprintf!!!
#include <iostream>

//#include "platform.hpp"
#include "symshell.h"
#include "graphs.hpp"

void config_point::plot(int /*x*/,int /*y*/,unsigned short /*s*/,wb_color /*c*/)//KLASA BAZOWA?
{
	//::plot(x,y,0);
}

void simple_point::plot(int x, int y, unsigned short /*s*/, wb_color c)
{
	::plot(x,y,c);
}

void hash_point::plot(int x,int y,unsigned short s,wb_color c)
{
	s/=2;
	fill_rect(x-s,y,x+s+1,y+1,c);
	fill_rect(x,y-s,x+1,y+s+1,c);
}

void circle_point::plot(int x,int y,unsigned short s,wb_color c)
{
	s/=2;
	if(s<=0) s=1;
	fill_circle(x,y,s,c);
}

void rhomb_point::plot(int x,int y,unsigned short s,wb_color c)
{
	s/=2;
	if(s<=1)
	{
		::plot(x,y,c);
	}
	else
	{
		ssh_point pts[4]={
            0,-int(s),  //gora
            s, 0,       //prawo
            0, s,       //dol
            -int(s), 0} //lewo
        ;

			fill_poly(x,y,pts,4,c);
	}
}



/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*          WWW:  http://www.iss.uw.edu.pl/borkowski                */
/*          MAIL: wborkowski@uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/



