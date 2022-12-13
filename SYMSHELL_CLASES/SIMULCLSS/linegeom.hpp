///////////////////////////////////////////////////////////////
//	LINEAR GEOMETRY 
///////////////////////////////////////////////////////////////
#error NOT IMPLEMENTED YET!!! //W trakcie przerobki z rectangular geom

#ifndef __LINEAR_GEOM_HPP__
#define __LINEAR_GEOM_HPP__
#include "geombase.hpp"
//#include "wb_ptr.hpp"
//#include "wbminmax.hpp"
#include "wb_rand.h"


class linear_geometry:public geometry_base
//---------------------------------------------
{
protected:
size_t N;			//ile elementow
int		torus;

//Parametry aktualnego wycinka uzywanego dla wizualizacji
size_t start,len;

public:
//Struktora do globalnej iteracji po tablicy
	struct iterator:public geometry_base::iterator_base
	{
	size_t i;//Iteracja 
	size_t sub_width;//Parametry zaznaczonego wycinka
	long horiz_start;
	
	//Konstruktor - sSZER,sWYS,lSZER,lWYS
	iterator(long hors,size_t subw):
				i(0),
				geometry_base::iterator_base(subh*subw),
				horiz_start(hors),sub_width(subw)
					{}
	virtual 
	~iterator()
	{
	//	cerr<<"linear_geometry::~iterator() ";
	}

	//Implementacja pobrania nastepnego elementu
	void _next(const geometry_base& geo,size_t& ret,size_t& end)
		{
		linear_geometry* MyGeo=(linear_geometry*)&geo;//Dostep do pol		
		size_t I_S=horiz_start+i;
		size_t J_W=vert_start+j;

		ret=MyGeo->get(I_S,J_W);//FULL gdy nie w tablicy

		if((i=(i+1)%sub_width)==0)//Inkrementacja kolumn
			if((j=j+1)==sub_height)//i czasami wierszy
				{//Jesli j==lb to jestesmy za oknem
				end=1;//Konczymy z tym iteratorem
				}
		}
	};//koniec klasy iteratora globalnego

//Struktura do losowej iteracji po tablicy
	struct monte_carlo_iterator:public geometry_base::iterator_base
	{
	size_t	ile;//Do zliczania w dól
	size_t	sub_width;//Parametry zaznaczonego wycinka
	long	horiz_start;
	monte_carlo_iterator(size_t ii,long hors,size_t subw):
				ile(ii),
				geometry_base::iterator_base(ii),
				horiz_start(hors),sub_width(subw)				
			{}
	virtual 
	~monte_carlo_iterator()
	{
	//	cerr<<"~monte_carlo_iterator() ";
	}
	
	//Implementacja pobrania nastepnego elementu
	void _next(const geometry_base& geo,size_t& ret,size_t& end)
		{		
		linear_geometry* MyGeo=(linear_geometry*)&geo;//Dostep do pol		
		size_t i=_wb_random(sub_width); //Jesli wycinek wybiega za tablice to moze 
		size_t j=_wb_random(sub_height);// nie byc wewnatrz tablicy elementow
		size_t I_S=horiz_start+i;
		size_t J_W=vert_start+j;

		ret=MyGeo->get(I_S,J_W);//FULL gdy nie w tablicy
			
		ile--;//Juz zaliczony
		if(ile==0)
			end=1;//Konczymy z tym iteratorem
		}
	};//koniec klasy iteratora monte-carlo
	

//METODY IMPLEMENTUJACE OGOLNE WLASCIWOSCI GEOMETRII
///////////////////////////////////////////////////////
int  compare(geometry_base& bsec)
{
if( _compare_geometry_base(&bsec)==0 )//Czy jest tego samego typu i wymiaru
	{
	linear_geometry& sec=*(linear_geometry*)&bsec;//Mozna zrzutowac
	if(		sec.N==N && //Dlugosc zrodla
			sec.start==start &&	
			sec.len==len )	//Parametry zaznaczonego wycinka
			return 0;
	}
return -1;
}

//Informacja o rozmiarze uzytecznej przestrzeni                 
MD_info* get_info(MD_info* Info=NULL) const
{
if(Info==NULL) Info=new MD_info;
assert(Info!=NULL);
if(Info==NULL) return NULL;
//set information
Info->max.X=N-1;
return Info;
}

//Informacja o polozeniu i zasiegu kamery dla view_iteratora
view_info* get_view_info(view_info* Info=NULL) const
{
if(Info==NULL) Info=new view_info;
assert(Info!=NULL);
if(Info==NULL) return NULL;

//set information
Info->pos.X=(2*start+len)/2.0;//Czy na pewno
Info->dia.X=len/2.0;
Info->sst.X=1;

return Info;
}

//Ustawianie polozeniu i zasiegu kamery dla view_iteratora
//sst jest ignorowane.
int set_view_info(const view_info* Info)
{
double dstart=0,dlen=0;
if(Info==NULL) goto RESET;
//Przeliczanie na wewnetrzny sposob reprezentacji wycinka
dsSZER=round(Info->pos.X-Info->dia.X);
dlSZER=round(2*Info->dia.X);

//Sprawdzanie poprawnosci parametrow
if(dlSZER<1 || dlSZER>Szerokosc)
			  goto ERROR;
if(torus)
	{	
	if(dsSZER<-double(Szerokosc) || dsSZER>2*Szerokosc-1)
		goto ERROR;	
	}
	else
	{
	if(dsSZER<0 || dsSZER>Szerokosc-dlSZER)
		goto ERROR;
	}

//Ostateczne przypisanie - UWAGA OBCINANIE:  DOUBLE->INT !!!
lSZER=long(dlSZER);

if( torus )
	{			
	sSZER=(Szerokosc+long(dsSZER))%Szerokosc;										
	}
	else
	{
	sSZER=long(dsSZER);
	}			
return 0;
RESET:
start=0;
len=N;
return 0;
ERROR:
return -1;
}

//Tworzy iterator po calosci. Umozliwia nastepnie czytanie od poczatku tablicy lub wycinka
iteratorh make_global_iterator() const
	{ 
	return new iterator(0,0,Szerokosc,Wysokosc); 
	}

//Tworzy iterator po obszarze wizualizacji. Umozliwia nastepnie czytanie wycinka wybranego do wizualizacji
virtual iteratorh make_view_iterator() const
	{ 
	return new iterator(sSZER,sWYS,lSZER,lWYS); 
	}

//Tworzy globalny iterator monte-carlo
iteratorh make_random_global_iterator(size_t how_many=-1) const
	{
	if(how_many==-1)
		how_many=Szerokosc*Wysokosc;//Dla pelnego kroku monte-carlo
	return new monte_carlo_iterator(how_many,0,0,Szerokosc,Wysokosc);
	}

//Tworzy iterator po sasiadach
iteratorh make_neighbour_iterator(size_t center,size_t dist=1)  const
	{
	long x=center%Szerokosc-dist;
	long lenx=1+2*dist;
	long y=center/Szerokosc-dist;
	long leny=1+2*dist;
	return new iterator(x,y,lenx,leny);
	}

//Tworzy losowy iterator po sasiadach
iteratorh make_random_neighbour_iterator(size_t center,size_t dist=1,size_t how_many=-1)  const
	{
	long x=center%Szerokosc-dist;
	long lenx=1+2*dist;
	long y=center/Szerokosc-dist;
	long leny=1+2*dist;
	if(how_many==-1)
		how_many=lenx*leny;//Dla pelnego kroku monte-carlo	sasiedztwa
	return new monte_carlo_iterator(how_many,x,y,lenx,leny);
	}

//METODY SPECYFICZNE TYLKO DLA GEOMETRI PROSTOKATNEJ
///////////////////////////////////////////////////////////////////
// bezposrednie akcesory rozmiarowe
size_t get_size() const { return N;}
size_t get_width() const { return Szerokosc;}
size_t get_sub_width() const { return lSZER;}
size_t get_sub_horiz_start() const { return sSZER;}
int	   is_torus() const { return torus;}	

//Specyficzna dla tej geometrii transformacja do indeksu liniowego
long get(long x,long y) const
// Gwarantuje poprawna prace dla zakresu 
//			x in < -Szerokosc, 2*Szerokosc )
//			y in < -Wysokosc, 2*Wysokosc )
// o ile Wysokosc lub Szerokosc nie jest w zakresie (0,LONG_MAX/2)
{
long ret=FULL;//Sygnal ze nie w tablicy

if( x>=0 && x<Szerokosc && y>=0 && y<Wysokosc )//Czy wewnatrz danych
		ret=y*Szerokosc+x;//Mozna obliczyc index
		else	//Jesli nie to jak torus
		if(torus)
			{			
			if(x<0) x=Szerokosc+x;
			if(y<0) y=Wysokosc+y;
			assert(x>=0 && y>=0); //Sprawdzanie czy od dolu jest w tablicy
			ret=(y%Wysokosc)*Szerokosc+(x%Szerokosc);//mozna obliczyc index
			}

//Zwrot wyniku			
return ret;
}

//  KONSTRUKTORY/DESTRUKTORY
//////////////////////////////
~linear_geometry(){}

linear_geometry(	size_t iN,			//Szerokosc pelnego obszaru
					int  itorus=1		//Czy wlaczyc geometrie torusa.				
				)
			:geometry_base(2),
			 N(iN),
			 torus(itorus),
			 start(0),
			 len(N),
		{
		assert(Szerokosc>0);
		}
};



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

