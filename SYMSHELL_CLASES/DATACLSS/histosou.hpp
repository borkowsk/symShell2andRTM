/// @file
/// @brief __Legacy filter for counting series categories and derived statistics.__ /<br>
///         _Starszy filtr liczący liczebności kategorii w serii i pochodne statystyki._
/// @date 2026-05-19 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_HISTOGRAM_SOUR_HPP_INCLUDED_
#define SYMSHELL2_HISTOGRAM_SOUR_HPP_INCLUDED_

#include "statsour.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

/// @brief @EN{ . }
///        @PL{ Starszy filtr liczący liczebności klas serii i pochodne statystyki.. }
/// @details ...
/// @PL{ WERSJA PIERWOTNA — LICZY TYLKO HISTOGRAMY CAŁKOWITOLICZBOWE Z RUCHOMĄ LICZBĄ KLAS. }
/// @EN{  }
template<class DATA_SOURCE>
class histogram_source : public basic_statistics_source<DATA_SOURCE>
//------------------------------------------------------------------------------
{
public:
    typedef basic_statistics_source<DATA_SOURCE> basics_; ///< Skrót do klasy bazowej.
    typedef data_source_base::iterator_h iter_handle; ///< Skrót do typu "uchwyt iteratora".

    using basics_::table; ///< Skrócony dostęp do tablicy danych klasy bazowej.

protected:
    size_t N; ///< Number of categories/histogram bins.
    wb_dynarray<unsigned long> arra; ///< Histogram bins/koszyki.

    /// Przemieszcza iterator o jednostkę. Zeruje, jeśli koniec tablicy koszyków (używa `N`).
    size_t _next(iter_handle &p)
    {
        assert(p != NULL); //Nie wolno wywołać dla NULL
        size_t pom = ((size_t) p) - 1;
        if(pom + 1 >= N)
            p = NULL;
        else
            p = (iter_handle) (pom + 2);
        return pom;
    }

    /// Leniwa funkcja obliczeniowa. @returns 1, jeśli musiał przeliczyć.
    int _calculate();

public:
    /// @name Skalarne pod-źródła z wynikami statystyk.
    /// @{

    /// Liczebność największego koszyka (klasy).
    scalar_source<double> *MainClass(const char *format = "MainClass(%s)")
    {
        return basics_::GetMonoSource(6, format);
    }

    scalar_source<double> *NumOfClass(const char *format = "NumOfClass(%s)")
    {
        return basics_::GetMonoSource(7, format);
    }

    scalar_source<double> *WhichMain(const char *format = "WhichMain(%s)")
    {
        return basics_::GetMonoSource(8, format);
    }

    scalar_source<double> *Entropy(const char *format = "S(%s)")
    {
        return basics_::GetMonoSource(9, format);
    }

    scalar_source<double> *NormEntropy(const char *format = "nS(%s)")
    {
        return basics_::GetMonoSource(10, format);
    }
    /// @}

    /// Konstruktor.
    /// \param ini to wskaźnik do serii źródłowej.
    /// \param number_of_categories to wymagana liczba klas histogramu, ale 1 oznacza tryb całkowitoliczbowy.
    /// \param my_manager to wskaźnik do zarządcy danych.
    /// \param table_size to wymagany rozmiar tablicy pod-źródeł.
    /// \param format to sposób tworzenia nazwy tego źródła pochodnego z nazwy źródła jego danych.
    explicit histogram_source(  DATA_SOURCE                 *ini = NULL,
                                size_t      number_of_categories = -1,		//-1 oznacza tryb całkowitoliczbowy
                                sources_manager_base *my_manager = NULL,
                                size_t                table_size = 11/*BEZ ZAPASU*/,
                                const char               *format = "HISTOGRAM(%s)")
    : basic_statistics_source<DATA_SOURCE>(ini, my_manager, table_size, format), N(number_of_categories)
    {}

    /// Destruktor.
    ~histogram_source() override = default;

// Methods:
//=========

    /// Zwraca liczbę koszyków, ale też liczy statystyki, jeśli jeszcze niebyły policzone dla tej wersji danych.
    size_t get_size()
    {
        basics_::check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czy nie trzeba policzyć i ewentualnie liczy.
        return arra.get_size();
    }

    /// Alokuje i ewentualnie rejestruje w zarządcy danych wszystkie skalarne "serie" statystyczne.
    void all_subseries_required() override
    {
        basic_statistics_source<DATA_SOURCE>::all_subseries_required();
        MainClass();
        WhichMain();
        NumOfClass();
        Entropy();
    }

    /// Umożliwia odczytanie wszystkich koszyków od początku.
    iter_handle reset() override
    {
        basics_::check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        return (iter_handle) 1;
    }

    /// Wymusza zakończenie iteracji.
    void close(iter_handle &p) override
    {
        p = NULL;
    }

    /// Ile koszyków oraz `basics_::y_min` i `basics_::y_max`.
    /// @note Podejrzane są te wartości minimalna i maksymalna. Raczej wcale nie dotyczą koszyków!
    void bounds(size_t &num, double &min, double &max) override;

    /// Daje wartość następnego z N koszyków!!!
    double get(iter_handle &ptr_to_iterator) override
    {
        assert(ptr_to_iterator != NULL);
        return arra[_next(ptr_to_iterator)];
    }

    /// Czyta wartość koszyka na podstawie jego indeksu.
    double get(size_t index) override
    {
        basics_::check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        assert(index < get_size());
        return arra[index];
    }

};


// IMPLEMENTACJE:
//==============

template<class DATA_SOURCE>
void histogram_source<DATA_SOURCE>::bounds(size_t &num, double &min, double &max)
{
    basics_::check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
    _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
    num = get_size();
    min = basics_::y_min;
    max = basics_::y_max;
}

template<class DATA_SOURCE>
int histogram_source<DATA_SOURCE>::_calculate()
{
    if(!basic_statistics_source<DATA_SOURCE>::_calculate())
        return 0;

    double Entropy = 0;

    {//OBLICZANIE HISTOGRAMU
        assert(N == -1); //Tylko tryb integer-owy zaimplementowany

        size_t i;
        size_t SN, KL;
        double s_min, s_max;
        this->Source->bounds(SN, s_min, s_max);

        if(s_max - s_min <= double(size_t(-1))) //Czy w zakresie size_t
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
        y_max=s_max;
        */

        //PĘTLA ZLICZANIA
        data_source_base::iterator_h Ind = this->Source->reset();
        this->source_miss = this->Source->get_missing();
        size_t Licz = 0;
        for(i = 0; i < SN; i++)
        {
            double pom = this->Source->get(Ind);
            if(!this->from_source_is_missing_(pom))
            {
                Licz++;
                pom -= s_min; //Przesuniecie
                assert(pom < SIZE_MAX);
                arra[size_t(pom)]++; //Takie to sobie. Trzeba przetrawić i poprawić. WARNING jak najbardziej.
            }
        }

        this->Source->close(Ind);

        //PĘTLA MIN/MAX.
        this->y_min = DBL_MAX;
        this->y_max = 0;

        size_t licz_klasy = 0;
        //size_t min_p = 0;
        size_t max_p = 0;

        for(i = 0; i < KL; i++)
        {
            double pom = arra[i];

            if(pom > 0)
                licz_klasy++;

            if(pom > this->y_max)
            {
                this->y_max = pom;
                max_p = i;
            }

            if(pom < this->y_min)
            {
                this->y_min = pom;
                //min_p = i;
            }

            //Liczenie składowych entropi
            double qi = pom / double(Licz);

            //Powiększenie sumy, tam, gdzie nie jest to puste skrzyżowanie
            if(qi > 0)
                Entropy += qi * log(qi);
        }

        if(basics_::table[6] != NULL)
        {
            table[6]->change_val(this->y_max);
        }

        if(table[7] != NULL)
        {
            table[7]->change_val(licz_klasy);
        }

        if(table[8] != NULL)
        {
            table[8]->change_val(double(max_p) + s_min + 0.5); //`1/2`, bo środek przedziału całkowitego
        }

        if(table[9] != NULL)
        {
            table[9]->change_val(-Entropy);
        }

        if(table[10] != NULL)
        {
            table[10]->change_val(-Entropy / log(double(KL)));
        }

        return 1; //Musial przeliczyć
    }

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
    basics_::y_min = basics_::y_max = 0;
    return 1;
}

typedef histogram_source<data_source_base> generic_histogram_source;

}} // end-of-namespaces `sym2::data`

#pragma clang diagnostic pop
/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/              */
/*        MAIL:  wborkowski@uw.edu.pl                                 */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
#endif

