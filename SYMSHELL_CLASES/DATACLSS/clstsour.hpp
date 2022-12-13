///////////////////////////////////////////////////////////////////////////
// Filtr liczacy rozne rodzaje klasteringu dla serii  g e o m e t r i a!!!
///////////////////////////////////////////////////////////////////////////
#ifndef __CLSTSOUR_HPP__
#define __CLSTSOUR_HPP__
#include "statsour.hpp"

template<class DATA_SOURCE>
class clustering_source:public basic_statistics_source<DATA_SOURCE>
//------------------------------------------------------------------------------
{
protected:

int _calculate() //Zwraca 1 jesli musial przeliczyc
{
if(!basic_statistics_source<DATA_SOURCE>::_calculate()) 
	return 0;

unsigned testowanie=0;//Licznik wartosci centralnych
double Stress=miss;	  //Suma stresow
geometry_base* MyGeom=Source->getgeometry();//Wskaznik do geometri

if(MyGeom!=NULL)
{//Jest znana geometria - da sie policzyc
	double suma=0;
	
	//Alokujemy iterator 
	iteratorh Glob=MyGeom->make_global_iterator();
	
	//Idziemy po agentach iteratorem.
	while(Glob)
	{	
		size_t index=MyGeom->get_next(Glob);//Uzyskujemy index agenta	
		                                    assert(index!=any_layer_base::FULL);//... tutaj nie powinno sie zdarzyc
		double CenterVal=Source->get(index);// Uzyskujemy referencje do agenta
		if(Source->is_missing(CenterVal))	// Sprawdzamy czy nie jest miss.
			continue;					// bo wtedy robic dalej by³oby bez sensu.
		
		// Alokujemy iterator sasiedztwa
		iteratorh Neigh=MyGeom->make_neighbour_iterator(index,1);
		unsigned zliczanie=0;//Zliczanie sasiadów
		unsigned tacysami=0; //Zliczanie takich samych sasiadow
		unsigned inni=0; //Zliczanie innych samych sasiadow

		while(Neigh)
		{
			size_t index2=MyGeom->get_next(Neigh);//Uzyskujemy index sasiada		
			if(index2==any_layer_base::FULL || index2==index)	//Jesli poza obszarem symulacji lub w 
				continue;				//centrum obszaru to dalej byloby bez sensu.
		
			double PeryfVal=Source->get(index2);//Uzyskujemy referencje do sasiada
			if(Source->is_missing(PeryfVal))		//Sprawdzamy czy nie jest miss.
				continue;					// bo wtedy robic dalej by³oby bez sensu.
			
			zliczanie++;
			if(CenterVal==PeryfVal)
				tacysami++;
				else
				inni++;
			
		}	
        
		MyGeom->destroy_iterator(Neigh);	// upewniamy sie ze iterator zostanie usuniety
		
        if(zliczanie>0)
        {
            suma+=double(inni)/double(zliczanie);
            //Zlicza ile by³o wartosci
            testowanie++;					
        }
	}
	// upewniamy sie ze iterator zostanie usuniety
	MyGeom->destroy_iterator(Glob);
	Stress=suma/testowanie;
}

if(table[6]!=NULL)
		{
		table[6]->change_val(Stress);		
		}

return 1;//Musial przeliczyc
}

public:
clustering_source(DATA_SOURCE* ini=NULL,
				  sources_menager_base* MyMenager=NULL,
				  size_t table_size=7+1/*ZAPAS*/,
				  const char* format="CLUSTERING_STATS(%s)"):
	 basic_statistics_source<DATA_SOURCE>(ini,MyMenager,table_size,format) 
	{}

~clustering_source(){}
	
scalar_source<double>*      Stress(const char* format="Stress(%s)")	
{
return GetMonoSource(6,format);
}

void all_subseries_required()//Alokuje i ewentualnie rejestruje w menagerze wszystkie serie
{
	basic_statistics_source<DATA_SOURCE>::all_subseries_required();
	Stress();
}

};

typedef clustering_source<data_source_base> generic_clustering_source;


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