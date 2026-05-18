/// @file
/// @brief STARY WADLIWY (już niekompatybilny) filtr liczący koincydencje klas dwu serii i pochodne statystyki (Hi^2 itp.).
///        <br> __OBSOLETE__
/// @date 2026-05-18 (modified)
// ********************************************************************************************************************
//  Coś dziwnego Tidy robi z tym plikiem. Nie jest w stanie znaleźć systemowych nagłówków.
#ifndef SYMSHELL2_COINCIDENCE_SOUR_HPP_INCLUDED_
#define SYMSHELL2_COINCIDENCE_SOUR_HPP_INCLUDED_

#include "../costatso.hpp"
#include <cassert>

namespace sym2 { namespace data {

/// @brief Filtr liczący koincydencje klas dwu serii i pochodne statystyki (Hi^2 itp.)
template<class DATA_SOURCE>
class coincidention_source : public co_statistics_source//<DATA_SOURCE>
//------------------------------------------------------------------------------
{
protected:
    size_t N; ///< Required number of categories of First
    size_t M; ///< Required number of categories of Second

    wb_ptr<rect_geometry> my_geometry; //Redundantna?
    wb_dynmatrix<unsigned long> arra;

    int iHi; //indeks dla Hi-kwadrat

    /*
    //Zwraca indeks do aktualnego i przesuwa iterator.
    //Zeruje iterator, jeśli koniec danych. Zwraca ULONG_MAX, jeśli missing...
    size_t _next(iteratorh& p)
    {
    return my_geometry.get_next(p);
    }
    */

    /// Bezpośrednio sięga do swojej tablicy arra.
    double _get(size_t index)
    {
        double pom;     //assert(arra.);
        i = index / nn; //Który wiersz
                        assert(arra[i]);
        j = index % nn; //Która kolumna
        return arra[i][j];
    }


    int _calculate() //Zwraca 1, jeśli musial przeliczyć
    {
        if(!co_statistics_source/*<DATA_SOURCE>*/::_calculate())
            return 0;

        size_t nn, mm; //Real numbers of categories
/*	
    {//OBLICZANIE HISTOGRAMU
    assert(N==-1); //Tylko tryb integer-owy zaimplementowany

    size_t i;
    size_t SN,KL;
    double smin,smax;
    Source->bounds(SN,smin,smax);

    if(smax-smin<=double(size_t(-1)))	//Czy w zakresie size_t
        KL=size_t(smax-smin)+1; //Ile jednostek całkowitych zakresu
        else
        goto ERROR;

    arra.alloc(KL);
    if(!arra) //błąd alokacji — za mało/za dużo?
        goto ERROR;

    for(i=0;i<KL;i++)
        arra[i]=0;

    //PĘTLA ZLICZANIA
    iteratorh Ind=Source->reset();
    source_miss=Source->get_missing();
    for(i=0;i<SN;i++)
        {
        double pom=Source->get(Ind);
        if(!FromSourceIsMissing(pom))
            {
            pom-=smin; //Przesuniecie
            arra[pom]++;
            }
        }
    Source->close(Ind);

    //PĘTLA	MIN/MAX
    ymin=DBL_MAX;
    ymax=0;
    size_t licz=0,minp=0,maxp=0;

    for(i=0;i<KL;i++)
    {
        double pom=arra[i];

        if(pom>0)
            licz++;

        if(pom>ymax)
        {
            ymax=pom;
            maxp=i;
        }

        if(pom<ymin)
        {
            ymin=pom;
            minp=i;
        }
    }

    if(table[6]!=NULL)
        {
        table[6]->change_val(ymax);
        }

    if(table[7]!=NULL)
        {
        table[7]->change_val(licz);
        }

    if(table[8]!=NULL)
        {
        table[8]->change_val(maxp+smin+0.5); //0.5, bo środek przedziału całkowitego
        }

    return 1;
    } //Musial przeliczyć
*/
        ERROR:
        arra.dispose();
        y_min = y_max = 0;
        return 1;
    }

public:
    /// Ile jest pod-źródeł.
    virtual size_t number_of_subseries()
    {
        return co_statistics_source::number_of_subseries() +
               4; //Ma cztery pod-źródła.
    }

    /// Alokuje i ewentualnie rejestruje w zarządcy wszystkie pod-źródła.
    void all_subseries_required()
    {
        co_statistics_source<DATA_SOURCE>::all_subseries_required();
        Hi();
    }

    /// Access to "children".
    scalar_source<double> *Hi(const char *format = "Hi(%s)")
    {
        iHi = co_statistics_source::number_of_subseries();
        return GetMonoSource(iHi, format);
    }


//Construction
    coincidention_source(DATA_SOURCE *ini1 = NULL,
                         DATA_SOURCE *ini2 = NULL,
                         size_t NumberOfClass1 = -1,		//-1 oznacza tryb całkowitoliczbowy
                         size_t NumberOfClass2 = -1,		//-1 oznacza tryb całkowitoliczbowy
                         sources_manager_base *i_manager = NULL,
                         size_t table_size = 1/*ZAPAS NA POD-ŹRÓDŁA KLASY POTOMNEJ*/,
                         const char *format = "COINCIDENT(%s,%s)") :
            N(NumberOfClass1),
            M(NumberOfClass2),
            iHi(-1),
            co_statistics_source<DATA_SOURCE>(ini1, ini2,
                                              i_manager,
                                              4 + table_size,		//4 własne + z klas potomnych (zadeklarowany zapas).
                                              format)
    {}

    ~coincidention_source()
    {}

// Accession Methods:
//===================

    /// Ile elementów, wartość minimalna i maksymalna.
    /// Musi sprawdzić aktualność danych źródłowych i wywołać `_calculate`.
    void bounds(size_t &num, double &min, double &max)
    {
        check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czy nie trzeba policzyć i ewentualnie liczy
        num = get_size();
        min = y_min;
        max = y_max;
    }

    /// Ile elementów.
    size_t get_size()
    {
        check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        if(!my_geometry)
            return 0;
        return my_geometry->get_size(); //Prawdziwy rozmiar tablicy koincydencji
    }

    //Zwraca wskaźnik do obowiązującej geometrii danych. NULL oznacza dane nie-zgeometryzowane
    geometry_base *get_geometry()
    {
        check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        return my_geometry.get_ptr_val();
    }

    /// Początek iteracji. Używa geometrii.
    iteratorh reset()
    {
        check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        if(my_geometry)
            return my_geometry->make_global_iterator();
        else
            return NULL;
    }

    /// Wymuszony koniec iteracji.
    void close(iteratorh &p)
    {
        if(my_geometry)
            my_geometry->destroy_iterator(p);
    }

    /// Daje następną z `nn*mm` liczb.
    double get(iteratorh &p)
    {
        double ret = 0;
        assert(p != NULL);
        size_t pom = my_geometry->get_next(p);

        if(pom != ULONG_MAX)
            ret = _get(pom);
        else
            ret = miss;

        return ret;
    }

    /// Przetwarza index uzyskany z geometrii na jedną z `nn*mm` liczb.
    double get(size_t index)

    {
#ifdef CAREFULLY_GET //Raczej niepotrzebne, bo robi to już i get_geometry() i bounds() i get_size();
        check_version(); //Uaktualnia też wersje podźrodła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyc i ewentualnie liczy
#endif
        assert(index < get_size());
        return _get(index);
    }

};

typedef coincidention_source<data_source_base> generic_coincidention_source;

}} // end-of-namespaces sym2::data

/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
#endif //SYMSHELL2_COINCIDENCE_SOUR_HPP_INCLUDED_
