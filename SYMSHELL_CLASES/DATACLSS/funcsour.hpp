/// @file
/// @brief Klasy zrodel funkcyjnych i ich parametrow
/// @date 2026-04-27 (modified)
// ********************************************************************************************************************
//                       Zmodyfikowana znacząco 05.04.2008
//
#ifndef __FUNCTION_SOURCES_HPP__
#define __FUNCTION_SOURCES_HPP__
#ifndef __cplusplus
#error C++ required
#endif

#include "datasour.hpp" //Podstawowy interface

/// @name Przykładowe funkcje dla zrodla funkcyjnego.
/// @{

/// Klasa funkcyjna definiująca funkcję stałą czyli zwracajaca zawsze te sama wartosc.
template<int VDef>
class constans
{
double Val;
public:
    constans(double IniVal):Val(IniVal)
    {}
    constans():Val(VDef)
    {}
double operator () (double x)
    { return Val;}
};

/// Klasa funkcyjna opakowujaca funkcje liniowa y=x - potrzebna np do zrobienia scaterplot'a dla jednej serii.
class yeqx
{
public:
double operator () (double x)
    { return x;}
};

/// Klasa funkcyjna opakowujaca cosinus.
class cosinus
{
public:
double operator () (double x)
    { return cos(x);}
};

/// Klasa funkcyjna opakowujaca sinus.
class sinus
{
public:
double operator () (double x)
    { return sin(x);}
};
/// @}

/// Szablon źródła funkcyjnego sparametryzowany typem funkcyjnym.
/// @tparam F musi byc klasą z bezparametrowym konstruktorem i główną metodą o definicji `double operator () (double)`.
template<class F>
class function_source:public function_source_base
//----------------------------------------------------------------------
{
protected:
F f;//Obiekt klasy funkcyjnej

public:
// Constructor 
function_source(size_t iN=10,                  //Gestosc próbkowania zakresu X-ow
                double ixmin=0,double ixmax=1, //Zakres po X-ach
                const char* itit="f(x)",	   //Nazwa funkcji, do podpisu na wykresie
                double iymin=0,double iymax=0)://Zakres na y-kach. Oszczedza liczenia minimum i maksimum
    function_source_base(iN,ixmin,ixmax,itit,iymin,iymax)
    {
    //Jesli nie podano ograniczenia po Y-kach albo podano zle
    if(ymin==ymax)//to jest szukane
        {
        ymin=DBL_MAX;
        ymax=-DBL_MAX;
        for(size_t i=0;i<N;i++)
            {
            double pom=f(xmin+i*step);
            if(pom<ymin) ymin=pom;
            if(pom>ymax) ymax=pom;
            }
        }
    assert(ymin<ymax);
    }

double get(iteratorh& p)
//Daje nastepna z N liczb!!! 
    {
    size_t pom=(size_t)p;
    if(pom+1>=N)
        p=NULL;
        else
        p=(iteratorh)(pom+1);
    return f(xmin+step*pom);
    }

double get(size_t i)
//Daje któras z N liczb!!! 
    {
    if(i>N)
        return miss;
        else
        return f(xmin+step*i);
    }
};


/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*  Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk              */
/*        MAIL: borkowsk@iss.uw.edu.pl                               */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
#endif




