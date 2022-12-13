///////////////////////////////////////////////////////////////////////////
// Filtr liczacy liczebnosc klas serii i pochodne statystyki
///////////////////////////////////////////////////////////////////////////
#ifndef __SPATIAL_CORRELATION_SOUR_HPP__
#define __SPATIAL_CORRELATION_SOUR_HPP__
#include <math.h>
#include "SYMSHELL/multfils.hpp"
#include "INCLUDE/wb_rand.hpp"

template<class DATA_SOURCE>
class spatial_correlation_source:public multi_filter_source_base<DATA_SOURCE>
//------------------------------------------------------------------------------
{
protected:
size_t N;           //Number of distance class;
int RndMult;      //How many times for random counting. If 0 the _count_all() is used instead _count_randomly()
wb_dynarray<double> arra;   //Musi byc double bo inaczej zle przechowuje "missing value"
wb_dynarray<unsigned> zgodne;
wb_dynarray<unsigned> niezgodne;

// Przemieszcza iterator o jednostke. Zeruje jesli koniec tablicy
size_t _next(iteratorh& p)
{
	assert(p!=NULL);//Nie wolno wywolac dla NULL
	size_t pom=((size_t)p)-1;
	if(pom+1>=N)
		p=NULL;
	else
		p=(iteratorh)(pom+2);
	return pom;	
}

bool _count_all()
{
    size_t NN=arra.get_size();
    size_t HowManyCells,zliczaj=0; double Min,Max; //Parametry serii zrod這wej
    Source->bounds(HowManyCells,Min,Max);
    if(Min==Max || HowManyCells<2) 
        return false; //W kazdym razie nie ma czego liczyc

    geometry_base* MyGeom=Source->getgeometry();//Wskaznik do geometri
    for(unsigned int i=0;i<HowManyCells;i++) //Dla kazdej z komorek
    {
        double CenterVal=Source->get(i);         //Uzyskujemy wartosc dla centralnego
        if(Source->is_missing(CenterVal))		 //Sprawdzamy czy nie jest miss.
                continue;					     //bo wtedy robic dalej by這by bez sensu.

        for(unsigned int j=i+1;j<HowManyCells;j++)
        {
                                                    assert(i!=j);            
            double PeryfVal=Source->get(j);         //Uzyskujemy wartosc dla  sasiada
            if(Source->is_missing(PeryfVal))		//Sprawdzamy czy nie jest miss.
                continue;					        // bo wtedy robic dalej by這by bez sensu.
            
            double dist=MyGeom->get_distance(i,j);
            size_t d=size_t(dist);                  assert(d<NN);//Index kategori odleglosci - w sposob uproszczony
            
            if(CenterVal==PeryfVal)
                zgodne[d]++;
            else
                niezgodne[d]++;

            //zliczaj++;//Kontrola liczby sprawdzonych par
        }
    }

                                                    //assert(zliczaj==(HowManyCells*HowManyCells-HowManyCells)/2);

    return true;
}

bool _count_randomly()
{
    size_t NN=arra.get_size();
    size_t HowManyDrawings; double Min,Max; //Parametry serii zrod這wej
    Source->bounds(HowManyDrawings,Min,Max);
    if(Min==Max || HowManyDrawings<2) 
        return false; //W kazdym razie nie ma czego liczyc
    
    if(HowManyDrawings*RndMult>((HowManyDrawings*HowManyDrawings)-HowManyDrawings)/2*2)//2 za wzor na liczbe odleglosci i 2 za wiekszy koszt algorytmu tej procedury
    {        
        cerr<<"SPATIAL CORRELATION: Random mode with multiplier "<<RndMult<<" is more expensive than full mode. Mode changed.\n";
        RndMult=0; //Jesli liczba losowan bylaby wieksza niz odleglosci to nie ma sensu ta metoda
        HowManyDrawings=(HowManyDrawings*HowManyDrawings)/4; //Niezle przyblizenie na ten pierwszy raz
    }
    else
    {
        HowManyDrawings*=RndMult; //Conajmniej tyle losowan co obiektow w serii, ale mozna tez powielic liczbe losowan pare razy
    }

    geometry_base* MyGeom=Source->getgeometry();//Wskaznik do geometri
    iteratorh RndIter=MyGeom->make_random_global_iterator(HowManyDrawings);//Alokujemy iterator 
    while(RndIter)
    {
        size_t index=MyGeom->get_next(RndIter);//Uzyskujemy index agenta	
        assert(index!=MyGeom->FULL);//... tutaj nie powinno sie zdarzyc
        double CenterVal=Source->get(index);// Uzyskujemy referencje do agenta
        if(Source->is_missing(CenterVal))	// Sprawdzamy czy nie jest miss.
            continue;					    // bo wtedy robic dalej by這by bez sensu.
        
        // Alokujemy iterator sasiedztwa - o roznym rozmiarze, zeby wyrownac prawdopodobienstwa poszczegolnych odleglosci - ale to nietrywialne
        
        //size_t radius=size_t(   (1-TheRandG.DRand()*TheRandG.DRand()*TheRandG.DRand() ) * NN   );       //Rozklad trafien wychodzi jednomodalny z maksimum w srodku
        //size_t radius=size_t(TheRandG.DRand()*NN/2)+((1-TheRandG.DRand()*TheRandG.DRand())*NN/2);       //tu podobnie
        //size_t radius=size_t(TheRandG.Random(NN);                                                       //A tu niemal liniowy spadek trafien wraz z odlegloscia
        //size_t radius=NN;                                                                               //Jednomodalny z 0 przy odleglosci 1
        size_t radius=(TheRandG.DRand()>0.5 ? size_t(TheRandG.Random(NN)) : NN );                         //Troche lepiej, ale nie idealnie - ma這 dla odleglosci najwiekszych
        
        iteratorh Neigh=MyGeom->make_random_neighbour_iterator(index,radius,1);  //Po ilus (1,2,?) sasiadow kazdego wylosowanego
        while(Neigh)
        {
            size_t index2=MyGeom->get_next(Neigh);  //Uzyskujemy index sasiada		
            if(index2==MyGeom->FULL || index2==index)	    //Jesli poza obszarem symulacji lub w 
                continue;				            //centrum obszaru to dalej byloby bez sensu.
            
            double PeryfVal=Source->get(index2);    //Uzyskujemy referencje do sasiada
            if(Source->is_missing(PeryfVal))		//Sprawdzamy czy nie jest miss.
                continue;					        // bo wtedy robic dalej by這by bez sensu.
            
            double dist=MyGeom->get_distance(index,index2);
            size_t i=size_t(dist);      assert(i<NN);   //Index kategori odleglosci - w sposob uproszczony
            
            if(CenterVal==PeryfVal)
                zgodne[i]++;
            else
                niezgodne[i]++;
            
        }		
        MyGeom->destroy_iterator(Neigh);	    // upewniamy sie ze iterator zostanie usuniety
    }
    
    MyGeom->destroy_iterator(RndIter);  // upewniamy sie ze iterator zostanie usuniety
    return true;
}

int _calculate() //Zwraca 1 jesli musial przeliczyc
{   
    if(!multi_filter_source_base<DATA_SOURCE>::_calculate()) 
        return 0;
    
    //OBLICZANIE KORELACJI DLA ROZNYCH ODLEGLOSCI
                                                                    assert(N==-1);  //Tylko tryb "integerowy" - tyle klas ile liczb calkowitych 
                                                                                    // miesci sie w maksymalnej odleglosci
    geometry_base* MyGeom=Source->getgeometry();//Wskaznik do geometri  
    
    if(MyGeom!=NULL && MyGeom->get_dimension()>0) //Musi byc dostepna realna i conajmniej jednowymiarowa 
        //geometria symulacji - inaczej dupa blada
    {    
        //PRZYGOTOWANIE
        size_t NN=size_t(ceil(  MyGeom->get_max_distance() ));          assert(ceil(  MyGeom->get_max_distance() )<=UINT_MAX);
        double pierwiastek=NN;//Poszukiwana wartosc rozmiaru klastra

        if( arra.get_size() != NN)
        {
            arra.alloc(NN);arra.fill(float(1.0));
            zgodne.alloc(NN);zgodne.fill(0);
            niezgodne.alloc(NN);niezgodne.fill(0);
        }
        else
        {
            arra.fill(float(1.0));
            zgodne.fill(0);
            niezgodne.fill(0);
        }
        
                                                    assert(arra.get_size()==zgodne.get_size());

        //ZLICZANIE SKLADOWYCH        
        //if(!_count_randomly()) goto ERROR;
        if(RndMult<=0)
        {
            if(!_count_all()) goto ERROR;
        }
        else
        {
            if(!_count_randomly()) goto ERROR;
        }
        
        //OBLICZANIE FUNKCJI KORELACJI I SZUKANIE PIERWSZEGO PIERWIASTKA FUNKCJI KORELACJI
        ymin=DBL_MAX;ymax=-DBL_MAX;
        for(unsigned int i=0;i<NN;i++)
        {
            //double pom=arra[i]=zgodne[i]+niezgodne[i];//DEBUGING ROZKLADU TRAFIEN
           
            if(zgodne[i]+niezgodne[i]>0)
            {
                double pom=(double(zgodne[i])-niezgodne[i]);
                pom/=double(zgodne[i]+niezgodne[i]);//Prosta korelacja taki-sam versus inny
                
                if(pierwiastek==NN && pom<=0)
                    pierwiastek=i; //Pierwsza odleglosc z korelacja ponizej 0

                if(pom<ymin)
                    ymin=pom;
                
                if(pom>ymax)
                    ymax=pom;
                
                arra[i]=pom;
            }
            else
            {
                arra[i]=get_missing();
            }
           
        }
        
        //if()
        ymin=-1;ymax=1;//Jesli nie interesuje nas max i min

        //ZAPAMIETANIE WYNIKU
        if(table[0]!=NULL)
        {
            table[0]->change_val(pierwiastek);
        }
        
        return 1;
    }//Musial przeliczyc
    
ERROR:
    if(table[0]!=NULL)
        table[0]->change_val(table[0]->get_missing());	
    
    arra.dispose();zgodne.dispose();niezgodne.dispose();
    
    ymin=ymax=0;
    return 1;
}

public:
scalar_source<double>*      ApproximatedClusterSize(const char* format="ClustSize(%s)")	
{
	return GetMonoSource(0,format);
}
	
spatial_correlation_source(DATA_SOURCE* ini=NULL,
		size_t NumberOfClass=-1,//-1 oznacza tryb calkowitoliczbowy
        int  CountMode=2,
		sources_menager_base* MyMenager=NULL,
		size_t table_size=1 /*BEZ ZAPASU*/,
		const char* format="SPATIAL CORRELATION(%s)"):
        RndMult(CountMode),
	N(NumberOfClass),
		multi_filter_source_base<DATA_SOURCE>(ini,MyMenager,table_size,format) 
	{}
	
~spatial_correlation_source()
    {}

// Methods required for basic interface of data_source
//--------------------------------------------------------------------
size_t get_size()
{ 
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy	
	return arra.get_size();
}	

void all_subseries_required()//Alokuje i ewentualnie rejestruje w menagerze wszystkie serie
{
	//multi_filter_source_base<DATA_SOURCE>::all_subseries_required(); - pure virtual!
	//MAX CLASS
	ApproximatedClusterSize();
}

iteratorh  reset()
//Umozliwia czytanie od poczatku
{ 
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy
	return (iteratorh)1;
}

void close(iteratorh& p)
{
	p=NULL;
}

void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
{
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy
	num=get_size();	
	min=ymin;max=ymax;
}

double get(iteratorh& ptr_to_iterator)
//Daje nastepna z N liczb!!! 
{
	assert(ptr_to_iterator!=NULL);
	return arra[ _next(ptr_to_iterator) ];
}

double get(size_t index)//Przetwarza index uzyskany z geometri
{ //na wartosc z serii, o ile jest mozliwe czytanie losowe	
	check_version();//Uaktualnia tez wersje podzrodla jesli trzeba
	_calculate();//Sprawdza czy nie trzeba policzyc i ewentualnie liczy	
	assert(index<get_size());
	return arra[ index ];
}	


};

typedef spatial_correlation_source<data_source_base> generic_spatial_correlation_source;


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