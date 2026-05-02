/// @file
/// @brief INTERFACE-y najbardziej podstawowych klas źródeł.
/// @date 2026-05-02 (modified)
// *********************************************************************************************************************
//
#ifndef SYMSHELL2_DATA_SOURCES_HPP_INCLUDED_
#define SYMSHELL2_DATA_SOURCES_HPP_INCLUDED_

#ifndef __cplusplus
#error C++ required
#endif

#include "sourbase.hpp" //Podstawowy interface
using wbrtm::wb_dynarray;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

/// Klasa źródła przekazującego pojedynczą daną.
class scalar_source_base:public data_source_base,public title_util
//-----------------------------------------------------------------
{
#if __GNUC__ >= 3 //Problemy z widocznością poniższej zmiennej, ale i tak nie pomaga!!!
public:
#else
protected:
#endif
int CheckMinMax;

public:
/// Constructor.
explicit scalar_source_base(const char* nam,double min=0,double max=0):
      title_util(nam)
    {
     ymin=min;ymax=max;
     CheckMinMax=!(ymin==0 && ymax==0);
    }

const char* name() override //Musi zwracac nazwe serii albo "" - NIE NULL!!!
    { return title_util::name();}

void  bounds(size_t& N,double& min,double& max) override
    //Ile elementow,wartosc minimalna i maksymalna
    //sczytane z wewnetrznych pol.UWAGA! Moze byc min==max==0
    {
     N=1;
     min=ymin;
     max=ymax;
    }

/// Umożliwia czytanie od początku — `iteratorh` jest uchwytem iterator-a.
/// Domyślnie z obiektu Source, ale czasem nie.
iteratorh   reset() override
    { return (iteratorh)1;}

virtual double get(iteratorh& I)=0;
    //Daje nastepna czyli jedyna liczby!!!
    //Po czym obiekt zrodlowy zwalnia iterator!
    //Ta metoda do podstawienia

void  close(iteratorh& I) override
    //Obiekt źródłowy zwalnia iterator, jeśli nie został zwolniony przez `get(N)`
    { I=0; }

};

/// Klasa podstawowa dla utypowionych źródeł jedno-elementowych.
template<class T>
class template_scalar_source_base:public scalar_source_base
//---------------------------------------------------------
{
public:
//Constructor
explicit template_scalar_source_base(const char* nam,double min=0,double max=0):
      scalar_source_base(nam,min,max)
      { miss=default_missing<T>(); }

double get(iteratorh& I) override //"Zwalnia"(?) iterator i wywołuje wirtualne `get()`
        {               assert(I!=nullptr);//Jak juz zwolniony to nie powinien byc wywołany.
            I=nullptr;
            return get();
        }

//const T&  - - - TU TRZEBA ZMIENIĆ GDY `source_base` stanie szablonem
virtual const double get()=0; //Ma zawsze jedna wartość, więc można tędy uprościć dostęp.

};

/// Zrodlo filtrujace dane z innego zrodla.
class filter_source_base:public data_source_base,public title_util
//-----------------------------------------------------------------
{
protected:
data_source_base* Source;
double		 source_miss;
wb_dynarray<char>  _name;

virtual int check_version()
    //Sprawdza czy i jak zmienily sie dane w zrodle
    //W filtrach cache'ujacych moze powodowac oproznienie
    //lub ponowne napelnienie. Zwraca 1 jesli zmienilo wersje.
    //0 jesli wersje sie zgadzaja.
    {
    return update_version_from(Source);
    }

public:
/// Constructor.
explicit filter_source_base(data_source_base* ini=NULL,const char* format="F(%s)"):
      title_util(format)
      {
         set_source(ini);
      }

void set_source(data_source_base* ini)
    {
        Source=ini;
        source_miss=Source->get_missing();
    }
const data_source_base* get_source()//Zwraca wskaznik do seri zrodlowej
    { return Source;}

//Virtual accessors
long data_version() override
    //numer wersji danych
    { check_version();return data_source_base::data_version();}

long how_old_data() override
    //od ilu wersji dane sie nie zmieniły
    { check_version();return data_source_base::how_old_data();}

const char* name() override;//Musi zwracac nazwe serii albo "" - NIE NULL!!!


geometry_base* getgeometry() override//Zwraca wskaznik do obowiazujacej geometri danych
    { return Source->getgeometry(); }//domyslnie taka jak w zrodle.


//DOSTEP DO DANYCH
void  bounds(size_t& N,double& min,double& max) override
    //Ile elementow,wartosc minimalna i maksymalna
    //Byc moze wartosci te trzeba przekonwertowac
    {
    check_version();
    Source->bounds(N,min,max);
    if(ymin<ymax) //Jesli ustawiono to "overwrite"
        {min=ymin;max=ymax;}
    }

iteratorh   reset() override
    //Umozliwia czytanie od poczatku - iteratorh jest uchwytem iteratora
    // domyslnie z obiektu Source, ale czasem nie
    {
       check_version();  //Zeby zrodlo mialo szanse na update
       iteratorh pom=Source->reset();
       source_miss=Source->get_missing();//Dla pewnosci - moze sie zmienilo
       return pom;
    }

virtual double get(iteratorh& I)
    //Daje nastepna z N liczb!!! Po N-tej obiekt zrodlowy zwalnia iterator!
    //Ta metoda zapewne najczesciej do podstawienia
    {
    assert(!"Linear access get() not implemented");
    return Source->get(I);
    }

virtual double get(size_t index_from_geometry)
    //Przetwarza index uzyskany z geometri
    //na wartosc z serii, o ile jest mozliwe czytanie losowe
    //Ta metoda tez najczesciej do podstawienia
    {
    assert(!"Random access get() not implemented");
    return Source->get(index_from_geometry);
    }

void  close(iteratorh& I) override
    //Obiekt zrodlowy zwalnia iterator jesli nie zostal zwolniony przez get(N)
    { Source->close(I);}

/// Uzupełnienie o sprawdzanie, czy Source->get nie dało missing.
virtual int FromSourceIsMissing(double val)
    {
    if(val==source_miss)
        return 1;
    else
        return 0;
    }
};

/// Szablon zrodla filtrujace dane z innego zrodla - dla wiekszej efektywnosci.
template< class SOURCE_TYPE  >
class template_filter_source_base:public filter_source_base
//---------------------------------------------------
{
public:
/// Constructor.
explicit template_filter_source_base(SOURCE_TYPE* ini=NULL,const char* format="F(%s)"):
      filter_source_base(ini,format)
      { }

void  bounds(size_t& N,double& min,double& max) override
    //Ile elementow,wartosc minimalna i maksymalna
    //Byc moze wartosci te trzeba przekonwertowac
    {
    check_version();
    ((SOURCE_TYPE*)Source)->bounds(N,min,max);
    if(ymin<ymax)//Jesli sa ustawione to "overwrite"
        {min=ymin;max=ymax;}
    }

iteratorh   reset() override
    //Umozliwia czytanie od poczatku - iteratorh jest uchwytem iteratora
    // domyslnie z obiektu Source, ale czasem nie
    { check_version();return ((SOURCE_TYPE*)Source)->reset();}

double get(iteratorh& I)
    //Daje nastepna z N liczb!!! Po N-tej obiekt zrodlowy zwalnia iterator!
    //Ta metoda zapewne najczesciej do podstawienia
    {
    assert(!"Linear access get() not implemented");
    return ((SOURCE_TYPE*)Source)->get(I);
    }

double get(size_t index_from_geometry)
    //Przetwarza index uzyskany z geometri
    //na wartosc z serii, o ile jest mozliwe czytanie losowe
    //Ta metoda tez najczesciej do podstawienia
    {
    assert(!"Random access get() not implemented");//DEBUG
    return ((SOURCE_TYPE*)Source)->get(index_from_geometry);
    }

void  close(iteratorh& I) override
    //Obiekt źródłowy zwalnia iterator jesli nie zostal zwolniony przez get(N)
    { ((SOURCE_TYPE*)Source)->close(I);}

};

/// Klasa bazowa dla zrodel przekazujących dane liniowo.
class linear_source_base:public data_source_base,public title_util
//--------------------------------------------------------
{
protected:
size_t N; ///< Ile elementów.

// Constructor
linear_source_base(size_t iN,const char* itit):
    N(iN),title_util(itit)
    {}

// Przemieszcza iterator o jednostkę. Zeruje jesli koniec tablicy.
size_t _next(iteratorh& p)
{
assert(p!=NULL); //Nie wolno wywołać dla NULL
size_t pom=((size_t)p)-1;
if(pom+1>=N)
    p=NULL;
    else
    p=(iteratorh)(pom+2);
return pom;
}

public:
// Methods:
//=========

virtual void _change_size(size_t NewN) //Be carefull!!!
    { //WYMUSZA ZMIANĘ ROZMIARU SERII. PRZYDAJE SIĘ TYLKO GDY SERIA
      //JEST UCHWYTEM DO ZEWNĘTRZNYCH DANYCH!
        N=NewN;
    }

size_t get_size()
    { return N;}

const char* name() override	//Zwraca nazwe serii
    { return title_util::name();}

iteratorh  reset() override
//Umozliwia czytanie od poczatku
    { return (iteratorh)1;}

void close(iteratorh& p) override
    {
      p=NULL;
    }

};

/// Klasa bazowa dla zrodel udostepniajacych dane prostokatne wraz z wycinkami.
class rectangle_source_base:public data_source_base,public title_util
//--------------------------------------------------------------
{
private:
    rectangle_geometry* my_geometry;
    bool             local_geometry;

protected:

/// Constructor with private my_geometry.
rectangle_source_base(
              const char* itit,
              size_t iA,size_t iB,
              int		itorus,	 //Czy wlaczyc geometrie torusa. Default, bo wtedy nie trzeba "miss"
              int*		subs=NULL,	 //Ustala wycinek tablicy. startX,lenX,startY,lenY
              double	imiss=default_missing<double>()//Wartosc podawana przy
                                         //skanowaniu wycinka wychodzacego poza maciez
              ):
    title_util(itit),
    my_geometry(NULL),
    local_geometry(false)
    {
    set_missing(imiss);
    my_geometry=new rectangle_geometry(iA,iB,itorus);   assert(my_geometry!=NULL);
    local_geometry=true;
    //my_geometry.set_view_info(NULL);//Reset an default
    }

/// Constructor with borrowed geometry.
rectangle_source_base(
              const char* itit,
              rectangle_geometry& geom,  //Geometria z zewnatrz - dealokacja nie bedzie zarządzana //obsolete: int*		subs=NULL,	 //Ustala wycinek tablicy. startX,lenX,startY,lenY
              double	imiss=default_missing<double>()//Wartosc podawana przy
                                         //skanowaniu wycinka wychodzacego poza maciez
              ):
    title_util(itit),
    my_geometry(NULL),
    local_geometry(false) //Nie bedzie zarzadzal dealokacja
    {
    set_missing(imiss);
    my_geometry=&geom;            assert(my_geometry!=NULL);
    //my_geometry.set_view_info(NULL);//Reset an default
    }

~rectangle_source_base()
{
    if(local_geometry)
        delete my_geometry;
}

//Zwraca indeks do aktualnego i przesuwa iterator.
//Zeruje iterator jesli koniec danych. Zwraca ULONG_MAX jesli missing...
size_t _next(iteratorh& p)
{
return my_geometry->get_next(p);
}

public:
//Zwraca wskaznik do obowiazujacej geometrii danych. NULL oznaczalby dane nie-zgeometryzowane
geometry_base* getgeometry()
    {
                            assert(my_geometry!=NULL);
    return my_geometry;
    }

rectangle_geometry* getrectgeometry() //Non virtual (!!!) shortcut
    {
                            assert(my_geometry!=NULL);
    return my_geometry;
    }

const char* name()	//Zwraca nazwe serii
    { return title_util::name();}

iteratorh  reset()
//Umozliwia czytanie od poczatku
//tablicy lub wycinka
    { return my_geometry->make_global_iterator();}

void close(iteratorh& p)
    { my_geometry->destroy_iterator(p);}


//Stare podawanie parametrów geometrii zrodla na outtab i liczby wymiarow
/*
int box(int* outtab)
    {
    if(outtab)
        {
        outtab[0]=my_geometry.get_width();
        outtab[1]=my_geometry.get_height();
        }
    return 2;
    };
*/
};



/// Podstawa zrodla funkcyjnego - zeby zminimalizowac ilosc powtarzajacych sie metod.
class function_source_base:public data_source_base,public title_util
//-----------------------------------------------------------------------------------
{
protected:
double xmin,xmax;//dany z gory zakres X
double step;//rozmiar kroku probkujacego
size_t N;

//Constructor
function_source_base(size_t iN,
                double ixmin,double ixmax,//Zakres po X-ach
                const char* itit,		//Nazwa funkcji, do podpisu na wykresie
                double iymin,double iymax)://Zakres na y-kach. Oszczedza liczenia
    N(iN),title_util(itit),
    xmin(ixmin),xmax(ixmax)//pola wlasne
    {
    ymin=iymin;ymax=iymax;//pola dziedziczone
    assert(xmin<xmax);
    assert(ymin<=ymax);//Jesli rowne to klasa potomna musi liczyc
    step=(xmax-xmin)/(N-1);
    }


public:

const char* name()	//Zwraca nazwe serii
    { return title_util::name();}

void  bounds(size_t& num,double& min,double& max)
//Ile elementow
//wartosc minimalna i maksymalna jest czasto nietrywialna
//ale tu w sposob uproszczony
    {
    num=N;
    min=ymin;
    max=ymax;
    }

iteratorh  reset()
//Umozliwia czytanie od poczatku
    { return 0;}

void close(iteratorh& p)
    {
    p=NULL;
    }

};

//Szablon zrodla funcyjnego. Sparametryzowane typem funkcyjnym.
//Parametr F musi byc klasa z bezparametrowym konstruktorem i
//metoda: double operator () (double). Moze miec dowolne pola pomocnicze.

//template<class F>
//class function_source:function_source_base  --- #include "funcsour.hpp"

//		IMPLEMENTATION
// //////////////////////////////////////

inline
const char* filter_source_base::name()
//Musi zwracać nazwę serii albo "" - NIE NULL!!!
    {
    const char* pom=Source->name();
    if(!_name.IsOK() || strstr(_name.get_ptr_val(),pom)==NULL)
    //Jeśli jeszcze nie ma albo zmieniło się w obiekcie źródła.
        {
        _name.alloc(strlen(title_util::name())+strlen(pom)+1);
        sprintf(_name.get_ptr_val(),title_util::name(),pom);
        }
    return _name.get_ptr_val();
    }

#pragma clang diagnostic pop
/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
#endif //SYMSHELL2_DATA_SOURCES_HPP_INCLUDED_




