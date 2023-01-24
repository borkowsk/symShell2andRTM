#ifndef __MATTRIX_SOUR_HPP__
#define __MATTRIX_SOUR_HPP__
// Konkretne klasy zrodel - dostep do danych w tablicach i zmiennych
//--------------------------------------------------------------------
#include "datasour.hpp"

//Klasa udostepniajaca dowolna tablice prostokatna oraz jej wycinki.
//Jesli zdefiniowany wycinek wykracza poza tablice zrodlowa to funkcja
//get zwraca wartosc "miss" podawana w konstruktorze.Alternatywnie
//wycinek moze realizowac geometrie torusa i wtedy miss nie jest
//potrzebne.
template<class T>
class matrix_source:public rectangle_source_base
{
T* arra;		//Wskaznik do tablicy
public:
// Constructors
matrix_source(const char* itit,
			  size_t iA,size_t iB,
			  void* iarray,
			  int  itorus=1,	 //Czy wlaczyc geometrie torusa. Default, bo wtedy nie trzeba "miss"
			  int* subs=NULL, //Ustala wycinek tablicy. startX,lenX,startY,lenY
			  double imiss=default_missing<double>()//Wartosc podawana przy
							 //skanowaniu wycinka wychodzacego poza maciez
			  ):
	rectangle_source_base(itit,iA,iB,itorus,subs,imiss),
	arra((T*)iarray)
	{}


matrix_source(size_t iA,size_t iB,
			  void* iarray,
			  const char* itit,
			  int  itorus
			 ):

	rectangle_source_base(itit,iA,iB,itorus,NULL,default_missing<double>()),

	arra((T*)iarray)
	{}

matrix_source(const char* itit,
              rectangle_geometry& geom,
			  void* iarray,
			  int* subs=NULL, //Ustala wycinek tablicy. startX,lenX,startY,lenY
			  double imiss=default_missing<double>() //Wartość podawana przy
							 //skanowaniu wycinka wychodzącego poza macierz
			  ):
	rectangle_source_base(itit,geom,subs,imiss),
	arra(iarray)
		{}

~matrix_source()
	{
#ifndef NDEBUG
	cerr<<"~matrix_source():"<<name()<<'\n'; //                ???
#endif
	}

void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
	{
	num=getrectgeometry()->get_width()*getrectgeometry()->get_height();
	if(ymin<ymax)//Sa dane
		{
		min=ymin;max=ymax;
		return;
		}

	//Nie sa dane wiec probkujemy - co troche kosztuje
    min=wbrtm::limit<T>::Max();
	max=wbrtm::limit<T>::Min();
	iteratorh iterator=getrectgeometry()->make_global_iterator();
	while(iterator)
		{
		size_t i=getrectgeometry()->get_next(iterator);
		if(min>arra[i]) min=arra[i];
		if(max<arra[i]) max=arra[i];
		}
	getrectgeometry()->destroy_iterator(iterator);
	}

double get(iteratorh& p)
//Daje nastepna z la*lb liczb!!!
	{
	//assert(p!=NULL);
	if(p==NULL) return miss;
	size_t pom=_next( p );
	if(pom!=ULONG_MAX)
		return arra[ pom ];
		else
		return miss;
	}

double get(size_t index)//Przetwarza index uzyskany z geometri
	{ //na wartosc z serii, o ile jest mozliwe czytanie losowe
	assert(index<getrectgeometry()->get_size());
	return arra[ index ];
	}

};

//Klasa czytajaca z dowolnej prostokatnej tablicy struktor
//za pomoca wskaznikow do skladowych
template<class STRUCT_T,class FIELD_T>
class struct_matrix_source:	public rectangle_source_base
//------------------------------------------------
{
typedef FIELD_T STRUCT_T::* TYP_POLA;
STRUCT_T* arra; //Tablica indywiduow
TYP_POLA  skladowa; //Wskaznik do skladowej indywiduum
public:
// Constructor
struct_matrix_source(const char* itit,
              size_t iA,size_t iB,
			  STRUCT_T* iarray,
			  TYP_POLA iskladowa,
              int  itorus=1,  //Czy wlaczyc geometrie torusa. Default, bo wtedy nie trzeba "miss"
			  int* subs=NULL, //Ustala wycinek tablicy. startX,lenX,startY,lenY
			  double imiss=default_missing<double>()//Wartosc podawana przy
							 //skanowaniu wycinka wychodzacego poza maciez
			  ):
    //rectangle_source_base(iA,iB,itit,itorus,subs,imiss),
    rectangle_source_base(itit,iA,iB,itorus,subs,imiss),
    arra(iarray),
    skladowa(iskladowa)
    {
       assert(skladowa!=NULL);
    }

struct_matrix_source(const char* itit,
              rectangle_geometry& geom,
			  STRUCT_T* iarray,
			  TYP_POLA iskladowa,
			  int* subs=NULL, //Ustala wycinek tablicy. startX,lenX,startY,lenY
			  double imiss=default_missing<double>() //Wartość podawana przy
							 //skanowaniu wycinka wychodzącego poza macierz
			  ):
	rectangle_source_base(itit,geom,imiss),
	arra(iarray),
	skladowa(iskladowa)
		{}

~struct_matrix_source()
	{
	//cerr<<"~struct_matrix_source(): "<<name()<<'\n';
	}


double get(iteratorh& p)
//Daje następną z la*lb liczb!!!
{                                                                                                       assert(p!=NULL);
    size_t pom=_next( p );
    if( pom< this->getrectgeometry()->get_size() )
    {                                                                               // cerr<<this->name()<<' '<<pom<<endl; // DEBUG get()
        double val=arra[ pom ].*skladowa;
        return val;
    }
    else
        return miss;
}

double get(size_t index) //Przetwarza index uzyskany z geometrii
	{ //na wartość z serii, o ile jest możliwe czytanie losowe
                                                                                                           assert(arra);
                                                                                                 assert(skladowa!=NULL);
                                                                            assert(index<getrectgeometry()->get_size());
        return arra[ index ].*skladowa;
	}

void  bounds(size_t& num,double& min,double& max)
//Ile elementów, wartość minimalna i maksymalna
	{
	num=getrectgeometry()->get_width()*getrectgeometry()->get_height();
	if(ymin<ymax)//Sa dane
		{
		min=ymin;max=ymax;
		return;
		}

	//Nie są dane, wiec próbkujemy, co trochę kosztuje.
    min=wbrtm::limit<FIELD_T>::Max();
	max=wbrtm::limit<FIELD_T>::Min();
	iteratorh iterator=getrectgeometry()->make_global_iterator();
	while(iterator)
		{
		size_t i=getrectgeometry()->get_next(iterator);
        double pom=arra[i].*skladowa;
		if(min>pom) min=pom;
		if(max<pom) max=pom;
		}
	getrectgeometry()->destroy_iterator(iterator);
	}

};

//Klasa czytająca z dowolnej prostokątnej tablicy wskaźników do struktur
//za pomocą wskaźników do składowych
template<class STRUCT_T,class FIELD_T>
class ptr_to_struct_matrix_source:	public rectangle_source_base
//------------------------------------------------
{
typedef FIELD_T STRUCT_T::* TYP_POLA;
STRUCT_T** arra; //Tablica wskaźników do indywiduów
TYP_POLA  skladowa; //Wskaźnik do składowej indywiduum
public:
// Constructor
ptr_to_struct_matrix_source(const char* itit,
                            size_t iA,size_t iB,
			  STRUCT_T** iarray,
			  TYP_POLA iskladowa,
			  int  itorus=1,	 //Czy włączyć geometrie torusa. Default, bo wtedy nie trzeba "miss"
			  int* subs=NULL,    //Ustala wycinek tablicy. startX,lenX,startY,lenY
			  double imiss=default_missing<double>() //Wartość podawana przy
                                 //skanowaniu wycinka wychodzącego poza macierz, lub NULL w miejscu indywiduum
			  ):
rectangle_source_base(itit,iA,iB,itorus,subs,imiss),
arra(iarray),
skladowa(iskladowa)
{}

ptr_to_struct_matrix_source(const char* itit,
              rectangle_geometry& geom,
			  STRUCT_T** iarray,
			  TYP_POLA iskladowa,
			  int* subs=NULL, //Ustala wycinek tablicy. startX,lenX,startY,lenY
			  double imiss=default_missing<double>() //Wartość podawana przy
							 //skanowaniu wycinka wychodzącego poza macierz
			  ):
	rectangle_source_base(itit,geom,imiss),
	arra(iarray),
	skladowa(iskladowa)
		{}

~ptr_to_struct_matrix_source()
	{
	//cerr<<"~ptr_to_struct_matrix_source(): "<<name()<<'\n';
	}


double get(iteratorh& p)
//Daje następną z la*lb liczb!!!
	{
	assert(p!=NULL);
	size_t pom1=_next( p );
	if( pom1< this->getrectgeometry()->get_size() )
		{
		STRUCT_T* pom2=arra[ pom1 ];
		if(pom2)
			return pom2->*skladowa;
		}
	//Nie udalo sie bo costam.
	return miss;
	}

double get(size_t index)//Przetwarza index uzyskany z geometri
	{ //na wartosc z serii, o ile jest mozliwe czytanie losowe
    size_t s=getrectgeometry()->get_size();        assert(index<getrectgeometry()->get_size());
	if(index<s /*&& index!=ULONG_MAX*/)
		{
		STRUCT_T* pom2=arra[ index ];
		if(pom2)
			return pom2->*skladowa;
		}
	//Nie udalo sie bo costam.
	return miss;
	}

void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
	{
	num=getrectgeometry()->get_width()*getrectgeometry()->get_height();
	if(ymin<ymax)//Sa dane
		{
		min=ymin;max=ymax;
		return;
		}

	//Nie sa dane wiec probkujemy - co troche kosztuje
    min=wbrtm::limit<FIELD_T>::Max();
	max=wbrtm::limit<FIELD_T>::Min();
	iteratorh iterator=getrectgeometry()->make_global_iterator();
	while(iterator)
		{
		size_t i=getrectgeometry()->get_next(iterator);
		STRUCT_T* pom1=arra[i];
		if(pom1==NULL)
			continue;//Nie ma szans na wartosc, wiec ignorujemy
		double pom=pom1->*skladowa;
		if(min>pom) min=pom;
		if(max<pom) max=pom;
		}
	getrectgeometry()->destroy_iterator(iterator);
	}

};


//Klasa czytajaca z dowolnej prostokatnej tablicy struktor
//za pomoca wskaznikow do skladowych
template<class STRUCT_T,class METHOD_T>
class method_matrix_source:	public rectangle_source_base
//------------------------------------------------
{
typedef METHOD_T (STRUCT_T::* TYP_METODY)(void);
STRUCT_T* arra;//Tablica indywiduow
TYP_METODY  skladowa;//Wskaznik do skladowej indywiduum
wb_dynarray<METHOD_T> cache;
int					  cache_ok;
public:
// Constructor
method_matrix_source(const char* itit,
              size_t iA,size_t iB,
			  STRUCT_T* iarray,
			  TYP_METODY iskladowa,
			  int  itorus=1,	 //Czy wlaczyc geometrie torusa. Default, bo wtedy nie trzeba "miss"
			  int* subs=NULL, //Ustala wycinek tablicy. startX,lenX,startY,lenY
			  double imiss=default_missing<double>()//DEFAULT_MISSING//Wartosc podawana przy
							 //skanowaniu wycinka wychodzacego poza maciez
			  ):
	rectangle_source_base(iA,iB,itit,itorus,subs,imiss),
	arra(iarray),
	skladowa(iskladowa),
	cache(iA*iB),
	cache_ok(0)
		{}

// Constructor II
method_matrix_source(const char* itit,
              rectangle_geometry& geom,
			  STRUCT_T* iarray,
			  TYP_METODY iskladowa,
			  int* subs=NULL, //Ustala wycinek tablicy. startX,lenX,startY,lenY
			  double imiss=default_missing<double>()//Wartosc podawana przy
							 //skanowaniu wycinka wychodzacego poza maciez
			  ):
	rectangle_source_base(itit,geom,imiss),
	arra(iarray),
	skladowa(iskladowa),
	cache(geom.get_height()*geom.get_width()),
	cache_ok(0)
		{}


~method_matrix_source()
		{
#ifndef NDEBUG
		cerr<<"~method_matrix_source(): "<<name()<<'\n';
#endif
		}

double _get(size_t i)
//Sieganie do wyniku funkcji - tu bezparametrowej ale w klasach potomnych niekoniecznie
	{
	double pom;
	if(cache_ok)
		pom=cache[i];
		else
		pom=cache[i]=(arra[i].*skladowa)();
	return pom;
	}

double get(iteratorh& p)
//Daje nastepna z la*lb liczb!!!
	{
	double ret=0;
	assert(p!=NULL);
	size_t pom=_next( p );

	if(pom!=ULONG_MAX)
		ret=_get(pom);
		else
		ret=miss;

	if(p==NULL) //Koniec danych
		cache_ok=1;//Wiec juz wszystkie w cachu

	return ret;
	}

double get(size_t index)//Przetwarza index uzyskany z geometri
	{ //na wartosc z serii, o ile jest mozliwe czytanie losowe
	assert(index<getrectgeometry()->get_size());
	return _get( index );
	}

void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
	{
	num=getrectgeometry()->get_width()*getrectgeometry()->get_height();
	if(ymin<ymax)//Sa dane
		{
		min=ymin;max=ymax;
		return;
		}

	//Nie sa dane wiec probkujemy - co troche kosztuje
    min=wbrtm::limit<METHOD_T>::Max();
	max=wbrtm::limit<METHOD_T>::Min();
	iteratorh iterator=getrectgeometry()->make_global_iterator();

	//assert(miss!=???);
	//missing();//Aktualizacja wartosci "miss" ???? DEBUG THIS

	while(iterator)
	{
		size_t i=getrectgeometry()->get_next(iterator);
		double pom=_get(i);//Dostep liniowy
		if(!is_missing(pom))
		{
			if(min>pom) min=pom;
			if(max<pom) max=pom;
		}
	}

	cache_ok=1;//Wszystkie przeszly wiec cache pelny
	getrectgeometry()->destroy_iterator(iterator);
	}

void  new_data_version(int change=1,unsigned increment=1)//Ustalanie informacji o wersji danych
	{
	rectangle_source_base::new_data_version(change,increment);
	cache_ok=0;//Cache juz nieaktualny
	}


};

//Klasa czytajaca z dowolnej prostokatnej tablicy struktor
//za pomoca wskaznikow do skladowych
template<class STRUCT_T,class METHOD_T>
class method_by_ptr_matrix_source:	public rectangle_source_base
//------------------------------------------------
{
typedef METHOD_T (STRUCT_T::* TYP_METODY)(void);
STRUCT_T** arra;//Tablica indywiduow
TYP_METODY  skladowa;//Wskaznik do skladowej indywiduum
wb_dynarray<METHOD_T> cache;
int					  cache_ok;
public:
// Constructor
method_by_ptr_matrix_source(const char* itit,
              size_t iA,size_t iB,
			  STRUCT_T** iarray,
			  TYP_METODY iskladowa,
			  int  itorus=1,	 //Czy wlaczyc geometrie torusa. Default, bo wtedy nie trzeba "miss"
			  int* subs=NULL, //Ustala wycinek tablicy. startX,lenX,startY,lenY
			  double imiss=default_missing<double>()//Wartosc podawana przy
							 //skanowaniu wycinka wychodzacego poza maciez
			  ):
	rectangle_source_base(itit,iA,iB,itorus,subs,imiss),
	arra(iarray),
	skladowa(iskladowa),
	cache(iA*iB),
	cache_ok(0)
		{}

method_by_ptr_matrix_source(const char* itit,
              rectangle_geometry& geom,
			  STRUCT_T** iarray,
			  TYP_METODY iskladowa,
			  int* subs=NULL, //Ustala wycinek tablicy. startX,lenX,startY,lenY
			  double imiss=default_missing<double>()//Wartosc podawana przy
							 //skanowaniu wycinka wychodzacego poza maciez
			  ):
	rectangle_source_base(itit,geom,imiss),
	arra(iarray),
	skladowa(iskladowa),
	cache(geom.get_height()*geom.get_width()),
	cache_ok(0)
		{}

~method_by_ptr_matrix_source()
		{
#ifndef NDEBUG
		//cerr<<"~method_by_ptr_matrix_source(): "<<name()<<'\n';
#endif
		}

double _get(size_t i)
//Sieganie do wyniku funkcji - tu bezparametrowej ale w klasach potomnych niekoniecznie
{
	double pom;
	if(cache_ok)
	{
		if(arra[i]!=NULL)//Troche to zmniejsza wartosc cache'u
			pom=cache[i];
			else
			pom=miss;
	}
	else
	{
		if(arra[i]!=NULL)
			pom=cache[i]=(arra[i]->*skladowa)();
			else
			pom=miss;
	}

	return pom;
}

double get(size_t index)//Przetwarza index uzyskany z geometri
	{ //na wartosc z serii, o ile jest mozliwe czytanie losowe
	                            assert(index<getrectgeometry()->get_size());
	return _get( index );
	}

double get(iteratorh& p)
//Daje nastepna z la*lb liczb!!!
	{
	double ret=0;
	assert(p!=NULL);
	size_t pom=_next( p );

	if(pom!=ULONG_MAX)
		ret=_get(pom);
		else
		ret=miss;

	if(p==NULL) //Koniec danych
		cache_ok=1;//Wiec juz wszystkie w cachu

	return ret;
	}


void  bounds(size_t& num,double& min,double& max)
//Ile elementow,wartosc minimalna i maksymalna
	{
	num=getrectgeometry()->get_width()*getrectgeometry()->get_height();
	if(ymin<ymax)//Sa dane
		{
		min=ymin;max=ymax;
		return;
		}

	//Nie sa dane wiec probkujemy - co troche kosztuje
    min=wbrtm::limit<METHOD_T>::Max();
	max=wbrtm::limit<METHOD_T>::Min();
	iteratorh iterator=getrectgeometry()->make_global_iterator();

	get_missing();//Aktualizacja wartosci "miss"
	while(iterator)
	{
		size_t i=getrectgeometry()->get_next(iterator);
		double pom=_get(i);//Dostep liniowy
		if(!is_missing(pom))
		{
			if(min>pom) min=pom;
			if(max<pom) max=pom;
		}
	}
	cache_ok=1;//Wszystkie przeszly wiec cache pelny
	getrectgeometry()->destroy_iterator(iterator);
	}

void  new_data_version(int change=1,unsigned increment=1)//Ustalanie informacji o wersji danych
	{
	rectangle_source_base::new_data_version(change,increment);
	cache_ok=0;//Cache juz nieaktualny
	}


};


/*
class file_source:public data_source_base;
*/

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



