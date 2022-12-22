// Podstawowa klasa dla filtrow statystycznych
//*/////////////////////////////////////////////////////////////////////////
#ifndef __STATSOUR_HPP__
#define __STATSOUR_HPP__

#include <cmath> /*DLA FUNKCJI FILTRÓW */
//#include "datasour.hpp"
#include "simpsour.hpp"
#include "sourmngr.hpp"
#include "multfils.hpp"

//KLasa liczaca podstawowe parametry statystyczne innego zrodla.
//Parametry sa podawane w arbitralnej kolejnosci lub poprzez jednowartosciowe
//zrod�a po�rednie
template<class DATA_SOURCE>
class basic_statistics_source:public multi_filter_source_base<DATA_SOURCE>
//------------------------------------------------------------------------------
{
friend class local_scalar_source;

private:
double Sigma(double sum,double sumSqr,unsigned N)
	{
		if(N==0 || N==1)
			return this->miss;
		double pom=( sumSqr- (sum*sum)/N )  /  (N-1);
		if(pom>=0)
			return sqrt( pom  );
		else
		if(pom>-0.00000000001)
			return 0; //Na skutek b��d�w numerycznych mo�e czasem nie wyj�� 0
			else
			return this->miss;
	}

protected:

virtual int _calculate() //Zwraca 1 jesli musial przeliczyc
{
if(multi_filter_source_base<DATA_SOURCE>::_calculate()==0)
                    return 0;

double summ=0,sumkw=0;
double mean= data_source_base::get_missing();//Zby zaladowac "miss"
double stdev=this->miss;
double min=this->miss,max=this->miss;
size_t N=0,i,licz=0;

this->Source->bounds(N,min,max);

if(N>0)
	{
	iteratorh Ind=this->Source->reset();
	this->source_miss=this->Source->get_missing();
	for(i=0;i<N;i++)
		{
		double pom=this->Source->get(Ind);
                if(!filter_source_base::FromSourceIsMissing(pom))
			{
			licz++;
			if(pom>max)
                max=pom;
			if(pom<min)
                min=pom;
			summ+=pom;
			sumkw+=pom*pom;
			}
		}
	this->Source->close(Ind);
	}

if(this->table[0]!=NULL)
		{
		this->table[0]->change_val(min);
		}
if(this->table[1]!=NULL)
		{
		this->table[1]->change_val(max);
		}
if(this->table[2]!=NULL)
		{
		if(licz>0)
			mean=summ/licz;
			else
			mean=0;
		this->table[2]->change_val(mean);
		}
if(this->table[3]!=NULL && licz>=2)
		{
		stdev=Sigma(summ,sumkw,licz);
		this->table[3]->change_val(stdev);
		}
if(this->table[4]!=NULL)
		{
		this->table[4]->change_val(licz);
		}
if(this->table[5]!=NULL)
		{
		this->table[5]->change_val(N);
		}

return 1;//Musial przeliczyc
}

public:
basic_statistics_source(DATA_SOURCE* ini=NULL,sources_menager_base* MyMenager=NULL,size_t table_size=6,const char* format="BASIC_STATS(%s)"):
	multi_filter_source_base<DATA_SOURCE>(ini,MyMenager,table_size,format)
	{
	}

~basic_statistics_source()
	{
	}

scalar_source<double>*      LenN(const char* format="lenN(%s)")	{return this->GetMonoSource(5,format);}
scalar_source<double>*      RealN(const char* format="N(%s)")	{return this->GetMonoSource(4,format);}
scalar_source<double>*      Min(const char* format="Min(%s)")	{return this->GetMonoSource(0,format);}
scalar_source<double>*      Max(const char* format="Max(%s)")	{return this->GetMonoSource(1,format);}
scalar_source<double>*      Mean(const char* format="Mean(%s)")	{return this->GetMonoSource(2,format);}
scalar_source<double>*      SD(const char* format="SD(%s)")	{return this->GetMonoSource(3,format);}

virtual void all_subseries_required()//Alokuje i ewentualnie rejestruje w menagerze wszystkie serie
	{LenN();RealN();Min();Max();Mean();SD();}
};

typedef basic_statistics_source<data_source_base> generic_basic_statistics_source;
typedef basic_statistics_source<data_source_base> generic_statistics_source;

#endif
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*  Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk              */
/*        MAIL: borkowsk@iss.uw.edu.pl                               */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/

