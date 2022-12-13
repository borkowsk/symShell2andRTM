#error - ZA SKOMPLIKOWANE W IMPLEMENTACJI
//////////////////////////////////////////////////////////////////////////////
// Klasa  filtru korelacji - dwuzrodlowego, jednowartosciowego filtru danych 
//////////////////////////////////////////////////////////////////////////////
#ifndef	__CORRSOUR_HPP__
#define __CORRSOUR_HPP__
#include "bifilter.hpp"

class correlation_source:public template_scalar_source_base<double>,
						 public bi_filter_source_base						  
//---------------------------------------------------------------------
{
double CorrVal;//Cache na wartosc
protected:
virtual	void _calculate();//Liczy korelacje i zapisuje w CorrVal
public:
 correlation_source(data_source_base* ini1=NULL,
			data_source_base* ini2=NULL,
			const char* format="CORR(%s , %s)"):
	  bi_filter_source_base(ini1,ini2,format),
	  template_scalar_source_base<double>("",/*min*/-1,/*max*/1)
	  {}

//Raczej nieuzywana implementacja decyzji o wartosci minimalnej i maksymalnej
virtual void  _bounds(double& min1,double& max1,
					  double& min2,double& max2,
					  double& min,double& max)
	{
	//Jesli nie ustawione to przyjmujemu typowy zakres korelacji
	min=-1;
	max=1;
	}

//Racej nieuzywane. Zawsze  zwraca tylko raz policzona korelacje, chyba ze dane sie zmienily
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


#endif