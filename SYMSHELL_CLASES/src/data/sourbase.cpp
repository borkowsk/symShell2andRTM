/// @file
/// @brief **DEPRECATED "COUNTING" of IEEE tags: INF and NAN** /<br>
///         _PRZESTARZAŁE "LICZENIE" znaczników IEEE: INF i NAN._
/// @date 2026-05-17 (modification)
//-----------------------------------------------------------
/// DAWNO NIE TESTOWANE, CHYBA NIE DZIAŁA, I NIE JEST NAPRAWDĘ POTRZEBNE
//========================================================================
#if 0 //CLOSED!

#include <cfloat>
#include "sourbase.hpp"

//int _finite( double x );int _isnan( double x );int _fpclass( double x );


//#ifdef __MSVC__
#define _CRTIMP2	/* EMPTY*/
#include <..\crt\src\xmath.h>

    /// @name Makra do tworzenia stałych typu `wb_Dconst`.
    /// @{
    #if _D0
     #define INIT(w0)		{0, 0, 0, w0}
     #define INIT2(w0, w1)	{w1, 0, 0, w0}
    #else
     #define INIT(w0)		{w0, 0, 0, 0}
     #define INIT2(w0, w1)	{w0, 0, 0, w1}
    #endif
    /// @}

    /// Typ będący unią typów zmiennoprzcinkowych i ich pól specjalnych.
    /// @note Prawdopodobnie nie jest już potrzebna taka komplikacja bo NaN i INF można wziąć od kompilatora.
    typedef union {
        unsigned short _W[5];
        float _F;
        double _D;
        long double _L;
        } wb_Dconst;

    /// @name Stałe dl INF i NaN
    /// @{
    const wb_Dconst _wb_Inf = {INIT(_DMAX << _DOFF)};
    const wb_Dconst _wb_NaN = {INIT(_DSIGN | (_DMAX << _DOFF) | (1 << _DOFF - 1))};
    /// @}

    /// @name Funkcje static udostępniające INF i NaN
    /// @{
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
    /// @}

#else
    //double data_source_base::inf()
    //Liczy INF wg IEEE - może generować SIGFPE na części platform
    //{
    //	double zero=0;
    //	return 1./zero; //-INF. jako znacznik braku
    //}
    //double data_source_base::nan(); //IS IMPLEMENTED?
#endif
/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
