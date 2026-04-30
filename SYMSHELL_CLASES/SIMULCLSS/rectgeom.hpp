/// @file
/// @brief GEOMETRIA PROSTOKĄTNA. / RECTANGULAR GEOMETRY.
/// @date 2026-04-30 (modified)
///      Pierwsza i główna specjalizacja. Heksagonalnej i sieciowej nigdy nie napisałem, choć były w planie.
// ********************************************************************************************************************
//
#ifndef RECT_GEOM_HPP_DEFINED_
#define RECT_GEOM_HPP_DEFINED_

#include <cmath>
#include "wb_ptr.hpp"
#include "wb_rand.hpp"
#include "geombase.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "modernize-use-auto"

using namespace wbrtm;
//using wbrtm::wb_dynmatrix;
//using wbrtm::round;

/// Klasa geometrii komórkowej 2D.
class rectangle_geometry:public geometry_base
//---------------------------------------------
{
    wb_dynmatrix<float> distances;	//!< Macierz odległości. Oczywiście nie zawsze potrzebna.

    long	columns;			//!< Ile ogólnie kolumn obszaru.
    long	rows;				//!< Ile ogólnie wierszy obszaru.
    unsigned long _currSize=0;	//!< Liczba itemów do leniwego obliczania rozmiaru (?).
    int		torus;				//!< flaga geometrii torusa.

protected:
    //Parametry aktualnego wycinka używanego dla wizualizacji:
    long	sSZER;	//!< ...
    long	sWYS;	//!< ...
    long	lSZER;	//!< ...
    long	lWYS;	//!< ...

    RandomGenerator& RndSel;	//!< Maszyna losująca pozycje.

public:
    RandomGenerator& 	get_rnd() { return RndSel; }	//!< Dostęp do odpowiedniego generatora liczb pseudolosowych.

    /// Struktura do globalnej iteracji po tablicy.
    struct iterator:public geometry_base::iterator_base
    {
        size_t i,j;	//!< Iteracja po wierszach i kolumnach
        size_t sub_width,sub_height;	//!< Parametry zaznaczonego wycinka
        long horiz_start,vert_start;	//!< ...

        /// Konstruktor inicjujący wycinek danych.
        iterator(long hors,long vers,size_t subw,size_t subh):
                    i(0),j(0),
                    geometry_base::iterator_base(subh*subw),
                    horiz_start(hors),sub_width(subw),
                    vert_start(vers),sub_height(subh)
                        {}

        ~iterator() override
        {
            //	cerr<<"rectangle_geometry::~iterator() ";
        }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
        /// Implementacja pobrania następnego elementu.
        void _next(const geometry_base& geo,size_t& ret,size_t& end) override
        {
            rectangle_geometry* MyGeo=(rectangle_geometry*)&geo; //Dostęp do pól
            long I_S=horiz_start+i;
            long J_W=vert_start+j;

            ret=MyGeo->get(I_S,J_W); //FULL, gdy nie w tablicy

            if((i=(i+1)%sub_width)==0) //Inkrementacja kolumn
                if((j=j+1)==sub_height) //i czasami wierszy
                    { //Gdy j == lb to jesteśmy za oknem
                    end=1; //Kończymy z tym iterator-em
                    }
        }
#pragma clang diagnostic pop

    }; //koniec definicji iterator-a globalnego

    /// Struktura do losowej iteracji po tablicy.
    struct monte_carlo_iterator:public geometry_base::iterator_base
    {
        size_t ile;	//!< Do zliczania w dół
        size_t sub_width,sub_height;	//!< Parametry zaznaczonego wycinka
        long horiz_start,vert_start;	//!< ...

        /// Konstruktor.
        monte_carlo_iterator(size_t ii,long hors,long vers,size_t subw,size_t subh):
                    ile(ii),
                    geometry_base::iterator_base(ii),
                    horiz_start(hors),sub_width(subw),
                    vert_start(vers),sub_height(subh)
        {
            // cerr<<"monte_carlo_iterator:"<<ile<<"["<<horiz_start<<"+"<<sub_width<<"|"<<vert_start<<"+"<<sub_width<<"]"<<endl;
        }

        ~monte_carlo_iterator() override
        {
            // cerr<<"~monte_carlo_iterator() ";
        }

        /// Implementacja pobrania następnego elementu.
        void _next(const geometry_base& geo,size_t& ret,size_t& end) override
        {
            rectangle_geometry* MyGeo=(rectangle_geometry*)&geo;
                                                            assert(MyGeo!=NULL); //Dostęp do pól
            RandomGenerator& rnd=MyGeo->get_rnd();
            size_t i=rnd.Random(sub_width);
                                                            assert(i<sub_width); // Gdy wycinek wybiega za tablice, to może...
            size_t j=rnd.Random(sub_height);
                                                            assert(j<sub_height); // ...nie być wewnątrz tablicy elementów.
                                                                                            //    assert(horiz_start>=0);
            long I_S=horiz_start+i;                                                         //            assert(I_S>=0);
                                                                                            //     assert(vert_start>=0);
            long J_W=vert_start+j;                                                          //            assert(J_W>=0);

            ret=MyGeo->get(I_S,J_W); //FULL, gdy nie w tablicy

            ile--; //Już zaliczony
            if(ile==0)
                end=1; //Kończymy z tym iterator-em.
        }
    }; //koniec klasy iterator-a monte-carlo

    // METODY IMPLEMENTUJĄCE OGÓLNE WŁAŚCIWOŚCI GEOMETRII:
    //*///////////////////////////////////////////////////

    int  compare(geometry_base& bsec) override
    {
        if( _compare_geometry_base(&bsec)==0 ) //Czy jest tego samego typu i wymiaru
            {
            rectangle_geometry& sec=*(rectangle_geometry*)&bsec; //Można zrzutować.
            if(sec.columns == columns && //Wiersze i kolumny
                    sec.rows == rows &&
                    sec.sSZER==sSZER &&	//Parametry zaznaczonego wycinka
                    sec.sWYS==sWYS &&
                    sec.lSZER==lSZER &&
                    sec.lWYS==lWYS) return 0;
            }
        return -1;
    }

    /// Informacja o rozmiarze użytecznej przestrzeni.
    /// @note W wersji bezparametrowej zwraca coś zaalokowanego, co trzeba potem zdealokować!
    MD_info* get_info(MD_info* Info/* =nullptr */) const override
    {
        if(Info==nullptr)
            Info=new MD_info; //Teraz już nie może być pusty.
        //Set information:
        Info->max.X()= columns - 1;
        Info->max.Y()= rows - 1;
        return Info;
    }

    /// Informacja o maksymalnej możliwej odległości. Potrzebna np. dla "Spatial correlation".
    double      get_max_distance() const override
    {
        double ret=(columns * columns + rows * rows);
        ret=sqrt(ret);
        if(torus)
            ret/=2;
        return ret;
    }

    /// Informacja o położeniu i zasięgu kamery dla `view_iterator`-a.
    /// @note W wersji bezparametrowej zwraca coś zaalokowanego, co trzeba potem zdealokować!
    view_info* get_view_info(view_info* Info) const override
    {
        if(Info==nullptr)
            Info=new view_info;  //Teraz już nie może być pusty.
        //Set information
        Info->pos.X()=(2*sSZER+lSZER)/2.0;
        Info->pos.Y()=(2*sWYS+lWYS)/2.0;
        Info->dia.X()=lSZER/2.0;
        Info->dia.Y()=lWYS/2.0;
        Info->sst.X()=1;
        Info->sst.Y()=1;
        return Info;
    }

    /// Ustawianie położenia i zasięgu kamery dla `view_iterator`.
    /// @note sst jest ignorowane (???)
    int set_view_info(const view_info* Info) override
    {
        double dsSZER=0,dlSZER=0,dsWYS=0,dlWYS=0;

        if(Info==nullptr) goto RESET;

        //Przeliczanie na wewnętrzny sposób reprezentacji wycinka:
        //-------------------------------------------------------
        dsSZER=round((double)Info->pos.X()-Info->dia.X());
        dlSZER=round(2*Info->dia.X());
        dsWYS=round(Info->pos.Y()-Info->dia.Y());
        dlWYS=round(2*Info->dia.Y());

        //Sprawdzanie poprawności parametrów:
        //-----------------------------------
        if(dlSZER<1 || dlSZER > columns)
                      goto ERROR;
        if(dlWYS<1 || dlWYS > columns)
                      goto ERROR;
        if(torus)
            {
            if(dsSZER<-double(columns) || dsSZER > 2 * columns - 1)
                goto ERROR;
            if(dsWYS<-double(rows) || dsWYS > 2 * rows - 1)
                goto ERROR;
            }
        else
            {
            if(dsSZER<0 || dsSZER > columns - dlSZER)
                goto ERROR;
            if(dsWYS<0 || dsWYS > rows - dlWYS)
                goto ERROR;
            }

        //Ostateczne przypisanie. UWAGA OBCINANIE:  DOUBLE->INT !!!
        lSZER=long(dlSZER);
        lWYS=long(dlWYS);
        if( torus )
            {
            sSZER= (columns + long(dsSZER)) % columns;
            sWYS= (rows + long(dsWYS)) % rows;
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
        lSZER=columns;
        lWYS=rows;
        return 0;
    ERROR:
        return -1;
    }

    /// Tworzy iterator po całości. Umożliwia następnie czytanie od początku całej tablicy lub wycinka.
    iteratorh make_global_iterator() const override
        {
            return new iterator(0, 0, columns, rows);
        }

    /// Tworzy iterator po obszarze wizualizacji. Umożliwia następnie czytanie wycinka wybranego do wizualizacji.
    iteratorh make_view_iterator() const override
        {
            return new iterator(sSZER,sWYS,lSZER,lWYS);
        }

    /// Tworzy globalny iterator monte-carlo.
    iteratorh make_random_global_iterator(size_t how_many/*=-1*/) const override
        {
         if(how_many==size_t(-1))
            how_many= columns * rows; //Dla pełnego kroku monte-carlo
         return new monte_carlo_iterator(how_many, 0, 0, columns, rows);
        }

    /// Tworzy iterator po sąsiadach.
    iteratorh make_neighbour_iterator(size_t center,size_t dist/*=1*/)  const override
        {
        long x= center % columns - dist;
        long lenX= 1 + 2 * dist;
        long y= center / columns - dist;
        long lenY= 1 + 2 * dist;
        return new iterator(x, y, lenX, lenY);
        }

    /// Tworzy losowy iterator po sąsiadach.
    iteratorh make_random_neighbour_iterator(size_t center,size_t dist/*=1*/,size_t how_many/*=-1*/)  const override
        {
        long x= center % columns - dist;
        long lenX= 1 + 2 * dist;
        long y= center / columns - dist;
        long lenY= 1 + 2 * dist;
        if(how_many==size_t(-1)) //-1 jako marker domyślnego rozmiaru
            how_many= lenX * lenY; //Dla pełnego kroku monte-carlo	sąsiedztwa
        return new monte_carlo_iterator(how_many, x, y, lenX, lenY);
        }

    // METODY SPECYFICZNE TYLKO DLA GEOMETRII PROSTOKĄTNEJ:
    //*////////////////////////////////////////////////////

    // bezpośrednie akcesory rozmiarowe
    size_t get_size() const { return _currSize;}
    size_t get_height() const { return rows;}
    size_t get_width() const { return columns;}
    size_t get_sub_height() const { return lWYS;}
    size_t get_sub_width() const { return lSZER;}
    size_t get_sub_vert_start() const { return sWYS;}
    size_t get_sub_horiz_start() const { return sSZER;}
    int	   is_torus() const { return torus;}

    /// Specyficzna dla tej geometrii transformacja do indeksu liniowego.
    /// Gwarantuje poprawna prace dla zakresu:
    ///```
    ///			x in < -columns, 2*columns
    ///			y in < -rows, 2*rows
    ///```
    /// O ile `rows` lub `columns` nie jest w zakresie (0,LONG_MAX/2).
    long get(long x,long y) const
    {
        long ret = FULL; //Sygnał, że nie tego w tablicy

        if (x >= 0 && x < columns && y >= 0 && y < rows) //Czy wewnątrz danych?
        {
            ret = y * columns + x; //Można obliczyć index
        }
        else if(torus)	//Gdy nie to jak torus
                {
                    if(x<0) x= columns + x;
                    if(y<0) y= rows + y;
                                                             assert(x>=0 && y>=0); //Sprawdzanie, czy od dołu jest w tablicy
                    ret= (y % rows) * columns + (x % columns); //da się obliczyć index
                }

        //Zwrot wyniku
        return ret;
    }

    //  KONSTRUKTORY/DESTRUKTORY
    //*////////////////////////////
    ~rectangle_geometry() override = default;

    rectangle_geometry(	size_t iA,				//!< Szerokość pełnego obszaru.
                        size_t iB,				//!< Wysokość pełnego obszaru.
                        int  iTorus=1,			//!< Ustala, czy włączyć geometrie torusa.
                        bool eDistMat=false,	//!< Informuje, czy będzie potrzebna macierz odległości (która jest duża!).
                        RandomGenerator& RndIni	//!< Generator do losowania elementów.
                                        =TheRandG	//!< Domyślnie z całości i sąsiedztwa! RÓWNOMIERNIE!
                        )
                : geometry_base(2),
                  columns(iA), rows(iB),
                  torus(iTorus),
                  sSZER(0), sWYS(0),
                  lSZER(iA), lWYS(iB),
                  RndSel(RndIni)
        {
            assert(columns > 0);
            assert(rows > 0);
            if(eDistMat)
                enable_distance_matrix();
            _currSize=iA*iB;
        }

    void enable_distance_matrix(bool yes=true)
    {
        if(yes)
        {
            distances.alloc(columns * rows, columns * rows); //Bardzo duża tablica, do tego w połowie nieużywana!!!
            distances.fill(-1.0); //Nie obliczone odległości
        }
        else
        {
            distances.dispose();
        }
    }

    void  set(	size_t	iA,				//!< Szerokość pełnego obszaru.
                size_t	iB,				//!< Wysokość pełnego obszaru.
                int		iTorus=1,			//!< Określa, czy włączyć geometrie torusa.
                RandomGenerator& RndIni 	//!< Generator do losowania elementów.
                                        =TheRandG 	//!< Domyślnie z całości i sąsiedztwa! RÓWNOMIERNIE!
            )
        {
            columns=iA;
            rows=iB;
            torus=iTorus;
            sSZER=0;
            sWYS=0;
            lSZER=iA;
            lWYS=iB;
            RndSel=RndIni;
            _currSize=iA*iB;
        }


    /// Informacja o odległości dwóch obiektów o określonych indeksach. Dla "Spatial correlation" i ważenia oddziaływań.
    /// @note Mogłoby mieć "cache-owanie" obliczonych odległości, ale aktualnie wykomentowane.
    double      get_distance(size_t first,size_t second) const override
    {/*
        if(distances.IsOK()) //To sprawdzenie niestety dodaje pewien procent roboty gdy nie używa się wcale tablicy
        {
            if(distances[first][second]!=-1)
            {
                return distances[first][second];
            }
            else
            {
                //Odzyskiwanie współrzędnych z indeksu
                size_t fX=first%columns;
                size_t fY=first/columns;
                size_t sX=second%columns;
                size_t sY=second/columns;

                //Liczenie składowych
                long dX=abs(fX-sX); //Pierwsza składowa
                long dY=abs(fY-sY); //Druga składowa

                if(torus)
                {
                    if(dX>columns/2)
                        dX=columns-dX;
                    if(dY>rows/2)
                        dY=rows-dY;
                }

                return distances[first][second]=sqrt(dX*dX+dY*dY); //Zwraca z zapamiętaniem
            }
        }
        else */
        {
            //Odzyskiwanie współrzędnych z indeksu
            size_t fX= first % columns;
            size_t fY= first / columns;
            size_t sX= second % columns;
            size_t sY= second / columns;

            //Liczenie składowych
            long dX=abs(long(fX-sX)); //Pierwsza składowa
                                      //	assert(fX>=sX); //Dlaczego - nie pamiętam idei...
            long dY=abs(long(fY-sY)); //Druga składowa
                                      //	assert(fY>=sY); //Dlaczego

            if(torus) //Z dwu odległości po torusie ważna jest ta mniejsza
            {
                if(dX > columns / 2)
                    dX= columns - dX;
                if(dY > rows / 2)
                    dY= rows - dY;
            }

            return sqrt(double(dX*dX)+double(dY*dY)); //Zwraca wynik, ale nie ma zapamiętywania, bo nie ma gdzie
        }
    }

    /// Przekształca indeks na koordynaty w warstwie.
    /// @return `true` jak jest to wykonalne, `false` w przeciwnym razie.
    bool WhatCoordinates(const size_t index,size_t& x,size_t& y) const
    {
        if(index< (unsigned long)(columns) * rows)
        {
            x= index % columns;
            y= index / columns;
            return true;
        }
        else
            return false;
    }

};

#pragma clang diagnostic pop
/* ***************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/* Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk              */
/*        MAIL: borkowsk@iss.uw.edu.pl                               */
/*                               (Don't change or remove this note)  */
/* ***************************************************************** */
#endif

