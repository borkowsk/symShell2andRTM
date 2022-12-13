///////////////////////////////////////////////////////////////////////////
// Rozne klasy jednozrodlowych filtrow danych
///////////////////////////////////////////////////////////////////////////
#ifndef __FILTSOUR_HPP__
#define __FILTSOUR_HPP__

#include "math.h" /*DLA FILTROW */
#include "datasour.hpp"

template<class SOURCE_TYPE>//Szablon filtru - log 10
class log_F_filter:public template_filter_source_base<SOURCE_TYPE>
//-----------------------------------------------------------------------
{
public:
log_F_filter(SOURCE_TYPE* ini=NULL,double imiss=default_missing<double>(),const char* format="log(%s)"):
	  template_filter_source_base<SOURCE_TYPE>(ini,format)
          {data_source_base::set_missing(imiss);}

void  bounds(size_t& N,double& min,double& max)
	//Ile elementow,wartosc minimalna i maksymalna,
	//ktore to wartosci trzeba przekonwertowac
	{
	template_filter_source_base<SOURCE_TYPE>::bounds(N,min,max);
	if(this->ymin<this->ymax)
		{
		    min=this->ymin;
		    max=this->ymax;
        }
		else
		{                       assert(min<max);
		if(min>0)
			min=log10(min);
			else
			min=0;
		if(max>0)
			max=log10(max);
			else
			max=this->miss;           assert(min<=max);
		}
	}

double get(iteratorh& I)
	//Daje nastepna z N liczb!!! Po N-tej obiekt zrodlowy zwalnia iterator!
	{
	double pom=((SOURCE_TYPE*)this->Source)->get(I);//template_filter_source_base<SOURCE_TYPE>::get(I);
	if(/*template_filter_source_base<SOURCE_TYPE>::*/this->is_missing(pom))
        return this->miss;
	if(pom>0)
		return log10(pom);
		else
		return this->miss;//Nie mozna obliczyc
	}

double get(size_t index_from_geometry)
	//Przetwarza index uzyskany z geometri
	//na wartosc z serii, o ile jest mozliwe czytanie losowe
    {
	double pom=((SOURCE_TYPE*)this->Source)->get(index_from_geometry);
    if(/*template_filter_source_base<SOURCE_TYPE>::*/this->is_missing(pom))
        return this->miss;
	if(pom>0)
		return log10(pom);
		else
		return this->miss;//Nie mozna obliczyc
	}

};

//WERSJA ZGENERALIZOWANA
typedef log_F_filter<data_source_base>  generic_log_F_filter;
//--------------------------------------------------------------------------

template<class SOURCE_TYPE>//Szablon filtru - 1+log
class log_1_plus_F_filter:public template_filter_source_base<SOURCE_TYPE>
//-----------------------------------------------------------------------
{
public:
//Constructor. Missing domyslnie jako 0
log_1_plus_F_filter(SOURCE_TYPE* ini=NULL,double imiss=default_missing<double>(),const char* format="log(1+%s)"):
	  template_filter_source_base<SOURCE_TYPE>(ini,format)
          {
            data_source_base::set_missing(imiss);
          }

void  bounds(size_t& N,double& min,double& max)
	//Ile elementow,wartosc minimalna i maksymalna,
	//ktore to wartosci trzeba przekonwertowac
	{
	template_filter_source_base<SOURCE_TYPE>::bounds(N,min,max);

	if(this->ymin<this->ymax)
		{
		    min=this->ymin;
		    max=this->ymax;
        }
		else
		{
		//assert(min<max);???Chyba niepotrzebne - po prostu "Invalid data"
        min+=1;
		if(min>0)
			min=log10(min);
			else
			min=0;
        max+=1;
		if(max>0)
			max=log10(max);
			else
			max=this->miss;
                            assert(min<=max);
		}
	}

double get(iteratorh& I)
	//Daje nastepna z N liczb!!! Po N-tej obiekt zrodlowy zwalnia iterator!
	{
	double pom=((SOURCE_TYPE*)this->Source)->get(I);//template_filter_source_base<SOURCE_TYPE>::get(I)+1;
	if(/*template_filter_source_base<SOURCE_TYPE>::*/this->is_missing(pom))
        return this->miss;
    pom+=1;//Unikamy wartosci nieobliczalnych
	if(pom>0)
		return log10(pom);
		else
		return this->miss;//Nie mozna obliczyc
	}

double get(size_t index_from_geometry)
	//Przetwarza index uzyskany z geometri
	//na wartosc z serii, o ile jest mozliwe czytanie losowe
    {
	double pom=((SOURCE_TYPE*)this->Source)->get(index_from_geometry);
    if(/*template_filter_source_base<SOURCE_TYPE>::*/this->is_missing(pom))
        return this->miss;
    pom+=1;//Unikamy wartosci nieobliczalnych
	if(pom>0)
		return log10(pom);
		else
		return this->miss;//Nie mozna obliczyc
	}

};

//WERSJA ZGENERALIZOWANA
typedef log_1_plus_F_filter<data_source_base>  generic_log_1_plus_F_filter;
//--------------------------------------------------------------------------

template<class SOURCE_TYPE>//Szablon filtru porownujacego ze skalarem
class treshold_filter_base:public template_filter_source_base<SOURCE_TYPE>
//---------------------------------------------------------------------------------
{
protected:
double tr_val;
public:
//Constructor. Missing domyslnie jako 0
treshold_filter_base(double ival,SOURCE_TYPE* ini=NULL,double imiss=default_missing<double>()/*DEFAULT_MISSING*/,const char* format="(%s) @ %g"):
	  template_filter_source_base<SOURCE_TYPE>(ini,format),
	  tr_val(ival)
	  {
            data_source_base::set_missing(imiss);
	  }
const char* name();
virtual double _get(const double& val)=0;//Funkcja sprawdzajaca warunek i ewentualnie zmieniajaca wartosc na miss

virtual double get(iteratorh& I)
	//Daje nastepna z N liczb!!! Po N-tej obiekt zrodlowy zwalnia iterator!
	{
    double val=((SOURCE_TYPE*)this->Source)->get(I);
	return _get(val);
	}

virtual double get(size_t index_from_geometry)
	//Przetwarza index uzyskany z geometri
	//na wartosc z serii, o ile jest mozliwe czytanie losowe
	//Ta metoda tez najczesciej do podstawienia
	{
	double val=this->Source->get(index_from_geometry);
	return _get(val);
	}
};

template<class SOURCE_TYPE>//Szablon filtru - Equal
class EQ_filter:public treshold_filter_base<SOURCE_TYPE>
//---------------------------------------------------------
{
public:
//Constructor. Missing domyslnie jako 0
EQ_filter(double ival,SOURCE_TYPE* ini=NULL,double imiss=default_missing<double>()/*DEFAULT_MISSING*/,const char* format="(%s)=%g"):
	  treshold_filter_base<SOURCE_TYPE>(ival,ini,imiss,format)
	  {}

double _get(const double& pom)
	{
	if(treshold_filter_base<SOURCE_TYPE>::IsMissing(pom))
        return this->miss;
	if(pom==this->tr_val)
		return pom;
		else
		return this->miss;//Usuwa wartosc ze "strumienia"
	}
};

//WERSJA ZGENERALIZOWANA
typedef EQ_filter<data_source_base>  generic_EQ_filter;
//--------------------------------------------------------------------------

template<class SOURCE_TYPE>//Szablon filtru - LessThan
class LT_filter:public treshold_filter_base<SOURCE_TYPE>
//---------------------------------------------------------
{
public:
//Constructor. Missing domyslnie jako 0
LT_filter(double ival,SOURCE_TYPE* ini=NULL,double imiss=default_missing<double>()/*DEFAULT_MISSING*/,const char* format="(%s)<%g"):
	  treshold_filter_base<SOURCE_TYPE>(ival,ini,imiss,format)
	  {}

double _get(const double& pom)
	{
	if(treshold_filter_base<SOURCE_TYPE>::IsMissing(pom))
        return this->miss;
	if(pom<this->tr_val)
		return pom;
		else
		return this->miss;//Usuwa wartosc ze "strumienia"
	}
};

//WERSJA ZGENERALIZOWANA
typedef LT_filter<data_source_base>  generic_LT_filter;
//--------------------------------------------------------------------------

template<class SOURCE_TYPE>//Szablon filtru - LessEqual
class LE_filter:public treshold_filter_base<SOURCE_TYPE>
//---------------------------------------------------------
{
public:
//Constructor. Missing domyslnie jako 0
LE_filter(double ival,SOURCE_TYPE* ini=NULL,double imiss=default_missing<double>()/*DEFAULT_MISSING*/,const char* format="(%s)<=%g"):
	  treshold_filter_base<SOURCE_TYPE>(ival,ini,imiss,format)
	  {}

double _get(const double& pom)
	{
	if(treshold_filter_base<SOURCE_TYPE>::IsMissing(pom))
        return this->miss;
	if(pom<=this->tr_val)
		return pom;
		else
		return this->miss;//Usuwa wartosc ze "strumienia"
	}
};

//WERSJA ZGENERALIZOWANA
typedef LE_filter<data_source_base>  generic_LE_filter;
//--------------------------------------------------------------------------

template<class SOURCE_TYPE>//Szablon filtru - MoreThan
class GT_filter:public treshold_filter_base<SOURCE_TYPE>
//---------------------------------------------------------
{
public:
//Constructor. Missing domyslnie jako 0
GT_filter(double ival,SOURCE_TYPE* ini=NULL,double imiss=default_missing<double>()/*DEFAULT_MISSING*/,const char* format="(%s)>%g"):
	  treshold_filter_base<SOURCE_TYPE>(ival,ini,imiss,format)
	  {}

double _get(const double& pom)
	{
	if(treshold_filter_base<SOURCE_TYPE>::is_missing(pom))
		return this->miss;
	if(pom>this->tr_val)
		return pom;
		else
		return this->miss;//Usuwa wartosc ze "strumienia"
	}
};

//WERSJA ZGENERALIZOWANA
typedef GT_filter<data_source_base>  generic_GT_filter;
//--------------------------------------------------------------------------

template<class SOURCE_TYPE>//Szablon filtru - MoreEqual
class GE_filter:public treshold_filter_base<SOURCE_TYPE>
//---------------------------------------------------------
{
public:
//Constructor. Missing domyslnie jako 0
GE_filter(double ival,SOURCE_TYPE* ini=NULL,double imiss=default_missing<double>()/*DEFAULT_MISSING*/,const char* format="(%s)>=%g"):
	  treshold_filter_base<SOURCE_TYPE>(ival,ini,imiss,format)
	  {}

double _get(const double& I) //Ale oryginalnie paramentr byl "pom"!
	{
	double pom=treshold_filter_base<SOURCE_TYPE>::get(I); //!?!?!?!?!?!?!!?!?!?!?!
                            assert("Strange and not tested code in use!!!"==NULL);
	if(treshold_filter_base<SOURCE_TYPE>::IsMissing(pom))
        return this->miss;
	if(pom>=this->tr_val)
		return pom;
		else
		return this->miss;//Usuwa wartosc ze "strumienia"
	}
};

//WERSJA ZGENERALIZOWANA
typedef GE_filter<data_source_base>  generic_GE_filter;
//--------------------------------------------------------------------------


//IMPLEMENTACJE METOD
//////////////////////////////////////////////////////
template<class SOURCE_TYPE>
const char* treshold_filter_base<SOURCE_TYPE>::name()
//Musi zwracac nazwe serii albo "" - NIE NULL!!!
	{
	const char* pom=this->Source->name();
	if(! this->_name.OK() || strstr(this->_name.get_ptr_val(),pom)==NULL)
	//Jesli jeszcze nie ma albo zmienilo sie w obiekcie zrodla
		{
		this->_name.alloc(strlen(title_util::name())+strlen(pom)+1+ZAPAS_NA_CYFRY/*Najdluzsza mozliwa liczba?*/);
		sprintf(this->_name.get_ptr_val(),title_util::name(),pom,tr_val);
		}
	return this->_name.get_ptr_val();
	}

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



