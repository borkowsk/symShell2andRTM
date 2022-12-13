//	RECTANGULAR GEOMETRY
//*/////////////////////////////////////////////////////////////
#ifndef __RECTGEOM_HPP__
#define __RECTGEOM_HPP__

#include <cmath>
//#include "wbminmax.hpp"
#include "wb_ptr.hpp"
#include "wb_rand.hpp"
#include "geombase.hpp"

using namespace wbrtm;
//using wbrtm::wb_dynmatrix;
//using wbrtm::round;

class rectangle_geometry:public geometry_base
//---------------------------------------------
{
wb_dynmatrix<float> distances;

protected:
RandomGenerator& RndSel;//Maszyna losujaca pozycje
long	Szerokosc;		//Ile kolumn
long	Wysokosc;		//Ile wierszy
int		torus;			//flaga geometrii torusa

//Parametry aktualnego wycinka uzywanego dla wizualizacji
long	sSZER,sWYS;
long	lSZER,lWYS;  

public:
    RandomGenerator& 	get_rnd() { return RndSel; } //Interface do odpowiedniego generatora liczb pseudo-losowych
    
    //Struktora do globalnej iteracji po tablicy
	struct iterator:public geometry_base::iterator_base
	{
	size_t i,j;//Iteracja po wierszach i kolumnach
	size_t sub_width,sub_height;//Parametry zaznaczonego wycinka
	long horiz_start,vert_start;
	
	//Konstruktor - sSZER,sWYS,lSZER,lWYS
	iterator(long hors,long vers,size_t subw,size_t subh):
				i(0),j(0),
				geometry_base::iterator_base(subh*subw),
				horiz_start(hors),sub_width(subw),
				vert_start(vers),sub_height(subh)
					{}
	virtual 
	~iterator()
	{
	//	cerr<<"rectangle_geometry::~iterator() ";
	}

	//Implementacja pobrania nastepnego elementu
	void _next(const geometry_base& geo,size_t& ret,size_t& end)
		{
		rectangle_geometry* MyGeo=(rectangle_geometry*)&geo;//Dostep do pol		
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
	size_t ile;//Do zliczania w dól
	size_t sub_width,sub_height;//Parametry zaznaczonego wycinka
	long horiz_start,vert_start;
	monte_carlo_iterator(size_t ii,long hors,long vers,size_t subw,size_t subh):
				ile(ii),
				geometry_base::iterator_base(ii),
				horiz_start(hors),sub_width(subw),
				vert_start(vers),sub_height(subh)
			{}
	virtual 
	~monte_carlo_iterator()
	{
	//	cerr<<"~monte_carlo_iterator() ";
	}
	
	//Implementacja pobrania nastepnego elementu
	void _next(const geometry_base& geo,size_t& ret,size_t& end)
		{		
		rectangle_geometry* MyGeo=(rectangle_geometry*)&geo;//Dostep do pol		
		size_t i=MyGeo->get_rnd().Random(sub_width); //Jesli wycinek wybiega za tablice to moze 
		size_t j=MyGeo->get_rnd().Random(sub_height);// nie byc wewnatrz tablicy elementow
		assert(i<sub_width);
		assert(i<sub_height);
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
	rectangle_geometry& sec=*(rectangle_geometry*)&bsec;//Mozna zrzutowac
	if(sec.Szerokosc==Szerokosc && //Wiersze i kolumny
			sec.Wysokosc==Wysokosc &&	
			sec.sSZER==sSZER &&	//Parametry zaznaczonego wycinka
			sec.sWYS==sWYS &&	 
			sec.lSZER==lSZER && 
			sec.lWYS==lWYS) return 0;
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
Info->max.X()=Szerokosc-1;
Info->max.Y()=Wysokosc-1;
return Info;
}

virtual //Informacja o maksymalnej mozliwej odleglosci - potrzebna np dla "Spatial correlation"
double      get_max_distance() const
{
    double ret=(Szerokosc*Szerokosc+Wysokosc*Wysokosc);
    ret=sqrt(ret);
    if(torus)
        ret/=2;
    return ret;
}

//Informacja o polozeniu i zasiegu kamery dla view_iteratora
view_info* get_view_info(view_info* Info=NULL) const
{
if(Info==NULL) Info=new view_info;
assert(Info!=NULL);
if(Info==NULL) return NULL;
//set information
Info->pos.X()=(2*sSZER+lSZER)/2.0;
Info->pos.Y()=(2*sWYS+lWYS)/2.0;
Info->dia.X()=lSZER/2.0;
Info->dia.Y()=lWYS/2.0;
Info->sst.X()=1;
Info->sst.Y()=1;
return Info;
}

//Ustawianie polozeniu i zasiegu kamery dla view_iteratora
//sst jest ignorowane.
int set_view_info(const view_info* Info)
{
double dsSZER=0,dlSZER=0,dsWYS=0,dlWYS=0;
if(Info==NULL) goto RESET;
//Przeliczanie na wewnetrzny sposob reprezentacji wycinka
dsSZER=round((double)Info->pos.X()-Info->dia.X());
dlSZER=round(2*Info->dia.X());
dsWYS=round(Info->pos.Y()-Info->dia.Y());
dlWYS=round(2*Info->dia.Y());
//Sprawdzanie poprawnosci parametrow
if(dlSZER<1 || dlSZER>Szerokosc)
			  goto ERROR;
if(dlWYS<1 || dlWYS>Szerokosc)
			  goto ERROR;
if(torus)
	{	
	if(dsSZER<-double(Szerokosc) || dsSZER>2*Szerokosc-1)
		goto ERROR;	
	if(dsWYS<-double(Wysokosc) || dsWYS>2*Wysokosc-1)
		goto ERROR;			
	}
	else
	{
	if(dsSZER<0 || dsSZER>Szerokosc-dlSZER)
		goto ERROR;
	if(dsWYS<0 || dsWYS>Wysokosc-dlWYS)
		goto ERROR;
	}

//Ostateczne przypisanie - UWAGA OBCINANIE:  DOUBLE->INT !!!
lSZER=long(dlSZER);
lWYS=long(dlWYS);
if( torus )
	{			
	sSZER=(Szerokosc+long(dsSZER))%Szerokosc;										
	sWYS=(Wysokosc+long(dsWYS))%Wysokosc;
	}
	else
	{
	sSZER=long(dsSZER);
	sWYS=long(dsWYS);
	}			
return 0;
RESET:
sSZER=0;
sWYS=0;
lSZER=Szerokosc;
lWYS=Wysokosc;
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
	 if(how_many==size_t(-1))
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
	if(how_many==size_t(-1))//-1 jako marker domyślnego rozmiaru
		how_many=lenx*leny;//Dla pelnego kroku monte-carlo	sasiedztwa
	return new monte_carlo_iterator(how_many,x,y,lenx,leny);
	}

//METODY SPECYFICZNE TYLKO DLA GEOMETRII PROSTOKATNEJ
///////////////////////////////////////////////////////////////////
// bezposrednie akcesory rozmiarowe
size_t get_size() const { return size_t(Wysokosc)*size_t(Szerokosc);}
size_t get_height() const { return Wysokosc;}
size_t get_width() const { return Szerokosc;}
size_t get_sub_height() const { return lWYS;}
size_t get_sub_width() const { return lSZER;}
size_t get_sub_vert_start() const { return sWYS;}
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
~rectangle_geometry(){}

rectangle_geometry(	size_t iA,			//Szerokosc pelnego obszaru
					size_t iB,			//Wysokosc pelnego obszaru
					int  itorus=1,		//Czy wlaczyc geometrie torusa.	
                    bool distmat=false,  
					RandomGenerator& RndIni // Generator do losowania elementow
									=TheRandG // z calosci i sasiedztwa! ROWNOMIERNIE!
					)
			:geometry_base(2),
			 Szerokosc(iA),Wysokosc(iB),
			 torus(itorus),
			 sSZER(0),sWYS(0),
			 lSZER(iA),lWYS(iB),
			 RndSel(RndIni) 
	{
		assert(Szerokosc>0);
		assert(Wysokosc>0);
        if(distmat)
            enable_distance_matrix();
    }

void enable_distance_matrix(bool yes=true)
{
    if(yes)
    {
        distances.alloc(Szerokosc*Wysokosc,Szerokosc*Wysokosc);//Bardzo duza tablica, do tego w polowie nieuzywana!!!
        distances.fill(-1.0); //Nie obliczone odleglosci
    }
	else
    {
        distances.dispose();
    }
}

void  set(	size_t	iA,			//Szerokosc pelnego obszaru
			size_t	iB,			//Wysokosc pelnego obszaru
			int		itorus=1,		//Czy wlaczyc geometrie torusa.	
			RandomGenerator& RndIni // Generator do losowania elementow
									=TheRandG // z calosci i sasiedztwa! ROWNOMIERNIE!
		)
	{
		Szerokosc=iA;
		Wysokosc=iB;
		torus=itorus;
		sSZER=0;
		sWYS=0;
		lSZER=iA;
		lWYS=iB;
		RndSel=RndIni;
	}


virtual //Informacja o odleglosci dwuch obiektow o okreslonych indeksach - dla "Spatial correlation" i wazenia oddzialywan
double      get_distance(size_t first,size_t second) const
{/*
    if(distances.IsOK()) //To sprawdzenie niestety dodaje pewien procent roboty gdy nie uzywa sie wcale tablicy
    {
        if(distances[first][second]!=-1)
        {
            return distances[first][second];
        }
        else
        {            
            //Odzyskiwanie wspolrzednych z indeksu
            size_t fX=first%Szerokosc; 
            size_t fY=first/Szerokosc;
            size_t sX=second%Szerokosc;
            size_t sY=second/Szerokosc;
            
            //Liczenie skladowych
            long dX=abs(fX-sX); //Pierwsza skladowa
            long dY=abs(fY-sY); //Druga skladowa
            
            if(torus)
            {
                if(dX>Szerokosc/2)
                    dX=Szerokosc-dX;
                if(dY>Wysokosc/2)
                    dY=Wysokosc-dY;
            }
            
            return distances[first][second]=sqrt(dX*dX+dY*dY);//Zwraca z zapamietaniem
        }
    }
    else */
    {
        //Odzyskiwanie wspolrzednych z indeksu
        size_t fX=first%Szerokosc; 
        size_t fY=first/Szerokosc;
        size_t sX=second%Szerokosc;
        size_t sY=second/Szerokosc;
        
        //Liczenie skladowych
		long dX=abs(long(fX-sX)); //Pierwsza skladowa
								  //	assert(fX>=sX); //Dlaczego - nie pamiętam idei...
		long dY=abs(long(fY-sY)); //Druga skladowa
								  //	assert(fY>=sY); //Dlaczego
		
		if(torus) //Z dwu odległości po torusie ważna jest ta mniejsza
		{
            if(dX>Szerokosc/2)
                dX=Szerokosc-dX;
            if(dY>Wysokosc/2)
                dY=Wysokosc-dY;
        }
        
        return sqrt(double(dX*dX)+double(dY*dY));//Zwraca - nie ma zapamientywania bo nie ma gdzie
    }
}

bool WhatCoordinates(const size_t index,size_t& x,size_t& y) const
{
    if(index<(unsigned long)(Szerokosc)*Wysokosc)
    {
        x=index%Szerokosc; 
        y=index/Szerokosc;
        return true;        
    }
    else
        return false;
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

