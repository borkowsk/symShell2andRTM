/// @file
/// @brief Klasa  filtru korelacji - dwuzrodlowego, jednowartosciowego filtru danych
/// @date 2026-04-27 (modified)
// ********************************************************************************************************************
//
#error - ZA SKOMPLIKOWANE W IMPLEMENTACJI

#ifndef	__CORRSOUR_HPP__
#define __CORRSOUR_HPP__
#include "bifilter.hpp"

/// Klasa  filtru korelacji — dwuźródłowego, jednowartosciowego filtru danych.
class correlation_source:public template_scalar_source_base<double>,
                         public bi_filter_source_base
//---------------------------------------------------------------------
{
double CorrVal; //Cache na wartosc
protected:
virtual	void _calculate();//Liczy korelacje i zapisuje w CorrVal
public:
 correlation_source(data_source_base* ini1=NULL,
            data_source_base* ini2=NULL,
            const char* format="CORR(%s , %s)"):
      bi_filter_source_base(ini1,ini2,format),
      template_scalar_source_base<double>("",/*min*/-1,/*max*/1)
      {}

//Raczej nieużywana implementacja decyzji o wartości minimalnej i maksymalnej
virtual void  _bounds(double& min1,double& max1,
                      double& min2,double& max2,
                      double& min,double& max)
    {
    //Jesli nie ustawione, to przyjmujemy typowy zakres korelacji
    min=-1;
    max=1;
    }

//Raczej nieużywane. Zawsze  zwraca tylko raz policzona korelacje, chyba że dane się zmieniły.
virtual double _get(double val1,double val2)
    {
    return CorrVal;
    }

virtual const double& get()
    {
    if(check_version())
            _calculate();
    return CorrVal;
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
