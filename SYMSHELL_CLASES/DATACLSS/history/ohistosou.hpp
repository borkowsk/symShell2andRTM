/// @file
/// @brief Stary filtr liczący liczebność klas serii i pochodne statystyki. NIE DA SIĘ OBECNIE SKOMPILOWAĆ.
///        <br> __OBSOLETE and INVALID__
/// @date 2026-05-18 (modified)
// ********************************************************************************************************************
// Coś dziwnego Tidy robi z tym plikiem. Nie jest w stanie znaleźć systemowych nagłówków.
#ifndef __OLD_HISTOGRAM_SOUR_HPP__
#define __OLD_HISTOGRAM_SOUR_HPP__

#include <cstdio> //sprintf!!!
#include <stddef.h>
#include "../statsour.hpp"
#include "wb_ptr.hpp"

namespace sym2 { namespace data {

/// @brief Filtr liczący liczebność klas serii i pochodne statystyki.
/// @details
///     WERSJA PIERWOTNA — LICZY TYLKO HISTOGRAMY CAŁKOWITOLICZBOWE
///     Z RUCHOMĄ LICZBĄ KLAS, więc nie jest przystosowana do współpracy z plikiem logu symulacji.
template<class DATA_SOURCE>
class flex_histogram_source : public basic_statistics_source<DATA_SOURCE>
//---------------------------------------------------------------------
{
    typedef typename basic_statistics_source<DATA_SOURCE>::size_t size_t;
    typedef typename basic_statistics_source<DATA_SOURCE>::iterator_h iteratorh;
    using basic_statistics_source<DATA_SOURCE>::Source;
    using basic_statistics_source<DATA_SOURCE>::table;
    using basic_statistics_source<DATA_SOURCE>::y_min;
    using basic_statistics_source<DATA_SOURCE>::y_max;
    using basic_statistics_source<DATA_SOURCE>::source_miss;
    using basic_statistics_source<DATA_SOURCE>::check_version;
    using basic_statistics_source<DATA_SOURCE>::GetMonoSource;
    using basic_statistics_source<DATA_SOURCE>::from_source_is_missing_;

protected:
    size_t N; ///< Number of categories.
    wb_dynarray<unsigned long> arra;

    /// Przemieszcza iterator o jednostkę. Zeruje, jeśli koniec tablicy.
    size_t _next(iteratorh &p)
    {
        assert(p != NULL); //Nie wolno wywołać dla NULL
        size_t pom = ((size_t) p) - 1;
        if(pom + 1 >= N)
            p = NULL;
        else
            p = (iteratorh) (pom + 2);
        return pom;
    }

    /// @brief Ciężkie, acz leniwe, obliczenia. @returns 1, jeśli musiał przeliczyć.
    int _calculate()
    {
        if(!basic_statistics_source<DATA_SOURCE>::_calculate())
            return 0;

        double Entropy = 0;

        {//OBLICZANIE HISTOGRAMU
            assert(N == -1); //Tylko tryb integer-owy zaimplementowany

            size_t i;
            size_t SN, KL;
            double s_min, s_max;
            Source->bounds(SN, s_min, s_max);

            if(s_max - s_min <= double(size_t(-1)))	//Czy w zakresie size_t
                KL = size_t(s_max - s_min) + 1; //Ile jednostek całkowitych zakresu
            else
                goto ERROR;

            arra.alloc(KL);
            if(!arra.IsOK()) //błąd alokacji — za mało/za dużo?
                goto ERROR;

            for(i = 0; i < KL; i++)
                arra[i] = 0;
            /*//.....ATRAPA.....
            for(...
                arra[i]=s_min+i;
            y_min=s_min;
            y_max=smax;
            */

            //PĘTLA ZLICZANIA
            iteratorh Ind = Source->reset();
            source_miss = Source->get_missing();
            size_t Licz = 0;
            for(i = 0; i < SN; i++)
            {
                double pom = Source->get(Ind);
                if(!from_source_is_missing_(pom))
                {
                    Licz++;
                    pom -= s_min; //Przesuniecie
                    arra[pom]++; //Takie to sobie. TODO Trzeba przetrawić i poprawić. WARNING jak najbardziej.
                }
            }
            Source->close(Ind);

            //PĘTLA	MIN/MAX
            y_min = DBL_MAX;
            y_max = 0;
            size_t licz_klasy = 0;
            size_t min_p = 0, max_p = 0;

            for(i = 0; i < KL; i++)
            {
                double pom = arra[i];

                if(pom > 0)
                    licz_klasy++;

                if(pom > y_max)
                {
                    y_max = pom;
                    max_p = i;
                }

                if(pom < y_min)
                {
                    y_min = pom;
                    min_p = i;
                }

                //Liczenie składowych entropi
                double qi = pom / double(Licz);

                //Powiększenie sumy, gdy nie jest to "puste skrzyżowanie".
                if(qi > 0)
                    Entropy += qi * log(qi);
            }

            if(table[6] != NULL)
            {
                table[6]->change_val(y_max);
            }

            if(table[7] != NULL)
            {
                table[7]->change_val(licz_klasy);
            }

            if(table[8] != NULL)
            {
                table[8]->change_val(max_p + s_min + 0.5); //1/2, bo środek przedziału całkowitego
            }

            if(table[9] != NULL)
            {
                table[9]->change_val(-Entropy);
            }

            if(table[10] != NULL)
            {
                table[10]->change_val(-Entropy / log(KL));
            }

            return 1;
        } //Musial przeliczyć

        ERROR:
        if(table[10] != NULL)
            table[10]->change_val(table[10]->get_missing());
        if(table[9] != NULL)
            table[9]->change_val(table[9]->get_missing());
        if(table[8] != NULL)
            table[8]->change_val(table[8]->get_missing());
        if(table[7] != NULL)
            table[7]->change_val(table[7]->get_missing());
        if(table[6] != NULL)
            table[6]->change_val(table[6]->get_missing());
        arra.dispose();
        y_min = y_max = 0;
        return 1;
    }

public:
    scalar_source<double> *MainClass(const char *format = "MainClass(%s)")
    {
        return GetMonoSource(6, format);
    }

    scalar_source<double> *NumOfClass(const char *format = "NumOfClass(%s)")
    {
        return GetMonoSource(7, format);
    }

    scalar_source<double> *WhichMain(const char *format = "WhichMain(%s)")
    {
        return GetMonoSource(8, format);
    }

    scalar_source<double> *Entropy(const char *format = "S(%s)")
    {
        return GetMonoSource(9, format);
    }

    scalar_source<double> *NormEntropy(const char *format = "nS(%s)")
    {
        return GetMonoSource(10, format);
    }


    flex_histogram_source(DATA_SOURCE *ini = NULL,
                          size_t NumberOfClass = -1,		//-1 oznacza tryb całkowitoliczbowy
                          sources_manager_base *my_manager = NULL,
                          size_t table_size = 11/*BEZ ZAPASU*/,
                          const char *format = "HISTOGRAM(%s)") :
            N(NumberOfClass),
            basic_statistics_source<DATA_SOURCE>(ini, my_manager, table_size, format)
    {}

    /// Destructor.
    ~flex_histogram_source()
    {}

// Methods:
//=========

    size_t get_size()
    {
        check_version(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        return arra.get_size();
    }

    /// Alokuje i ewentualnie rejestruje wszystkie serie w zarządcy.
    void all_subseries_required()
    {
        basic_statistics_source<DATA_SOURCE>::all_subseries_required();
        MainClass();
        WhichMain();
        NumOfClass();
        Entropy();
    }

    /// Umożliwia czytanie od początku.
    iteratorh reset()
    {
        check_version(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        return (iteratorh) 1;
    }

    void close(iteratorh &p)
    {
        p = NULL;
    }

    void bounds(size_t &num, double &min, double &max)
//Ile elementów,wartość minimalna i maksymalna
    {
        check_version(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        num = get_size();
        min = y_min;
        max = y_max;
    }

    double get(iteratorh &ptr_to_iterator)
//Daje następną z N liczb!!!
    {
        assert(ptr_to_iterator != NULL);
        return arra[_next(ptr_to_iterator)];
    }

    double get(size_t index)	//Przetwarza index uzyskany z geometrii
    { //na wartość z serii, o ile jest możliwe czytanie losowe
        check_version(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        assert(index < get_size());
        return arra[index];
    }


};

typedef flex_histogram_source<data_source_base> generic_flex_histogram_source;

}} // end-of-namespaces sym2::data

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
#endif
