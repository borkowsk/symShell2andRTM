//                 "LICZENIE" znacznik�w IEEE: INF NAN 
//-----------------------------------------------------------
//- CHYBA NIE DZIALA, I ZDAJE SIE NIE JEST NAPRAWDE POTRZEBNE
//////////////////////////////////////////////////////////////////
#include <float.h>
//#include "INCLUDE/platform.h"
#include "sourbase.hpp"

//int _finite( double x );int _isnan( double x );int _fpclass( double x );

#if 0 //CLOSED!
//#ifdef __MSVC__
#define _CRTIMP2	/* EMPTY*/
#include <..\crt\src\xmath.h>

#if _D0
 #define INIT(w0)		{0, 0, 0, w0}
 #define INIT2(w0, w1)	{w1, 0, 0, w0}
#else
 #define INIT(w0)		{w0, 0, 0, 0}
 #define INIT2(w0, w1)	{w0, 0, 0, w1}
#endif

typedef union {
	unsigned short _W[5];
	float _F;
	double _D;
	long double _L;
	} wb_Dconst;

const wb_Dconst _wb_Inf = {INIT(_DMAX << _DOFF)};
const wb_Dconst _wb_NaN = {INIT(_DSIGN | (_DMAX << _DOFF) | (1 << _DOFF - 1))};

double data_source_base::inf()
//zwraca INF wg IEEE 
{
	return _wb_Inf._D;
}

double data_source_base::nan()
//zwraca NaN wg IEEE 
{
	return _wb_NaN._D;
}

#else
//double data_source_base::inf()
//Liczy INF wg IEEE - moze generowac SIGPFE na czesci platform
//{
//	double zero=0;
//	return 1./zero;//-INF jako znacznik braku
//}
//double data_source_base::nan(); //IS IMPLEMENTED?
#endif
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/
