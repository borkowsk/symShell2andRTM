/// @file
/// @brief __A filter that counts the number of categories and derived statistics.__ /<br>
///         _Filtr liczący liczebność określonej liczby klas serii i pochodne statystyki._
/// @date 2026-05-31 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_FIXED_CLAS_HISTOGRAM_SOUR_HPP_INCLUDED_
#define SYMSHELL2_FIXED_CLAS_HISTOGRAM_SOUR_HPP_INCLUDED_

#include "statsour.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

/// @brief @EN{ A filter that counts the number of series classes and derived statistics. }
///        @PL{ Filtr liczący liczebność określonej liczby klas serii i pochodne statystyki. }
/// @details ...
/// @note UWAGA NA DZIEDZICZENIE! N jest zmienne więc liczba serii pochodnych też.
template<class DATA_SOURCE>
class fix_histogram_source : public basic_statistics_source<DATA_SOURCE>
//------------------------------------------------------------------------------------------------------
{
    typedef basic_statistics_source<DATA_SOURCE> base_type;
protected:
    size_t    Num;    ///< Number of categories.
    double FixMin;    ///< Ustalone z góry minimum.
    double FixMax;    ///< Ustalone z góry maksimum.
    bool SubRange: 1; ///< Jeżeli zakres realny wykracza poza zadany, to robi histogram z części danych

    wb_dynarray<unsigned long> arra; ///< Koszyki histogramu.

    /// Przemieszcza iterator o jednostkę. Zeruje, jeśli koniec tablicy.
    size_t _next(data_source_base::iterator_h &p);

    /// Leniwe obliczanie statystyk. Zwraca 1, jeśli musiał przeliczyć.
    int _calculate() override;

public:
    /// @name Pod-źródła skalarne z wynikami obliczanych statystyk.
    /// @{

    /// Liczebność najliczniejszej klasy (koszyka).
    scalar_source<double> *MainClass(const char *format = "MainClass(%s)")
    {
        return base_type::GetMonoSource(6, format);
    }

    scalar_source<double> *NumOfClass(const char *format = "NumOfClass(%s)")
    {
        return base_type::GetMonoSource(7, format);
    }

    scalar_source<double> *WhichMain(const char *format = "WhichMain(%s)")
    {
        return base_type::GetMonoSource(8, format);
    }

    scalar_source<double> *Entropy(const char *format = "S(%s)")
    {
        return base_type::GetMonoSource(9, format);
    }

    scalar_source<double> *NormEntropy(const char *format = "nS(%s)")
    {
        return base_type::GetMonoSource(10, format);
    }

    /// Źródło skalarne dla N-tego koszyka histogramu.
    /// @param index to określenie, który koszyk histogramu.
    scalar_source<double> *Categories(size_t index, const char *format = "C<%g,%g)(%s)")
    {
        char bufor[500]; //Z dużym zapasem
        double step = (FixMax - FixMin) / Num;
        double min = double(index) * step;
        double max = double(index + 1) * step;
        sprintf(bufor, format, min, max, "%s");
        return base_type::GetMonoSource(10 + 1 + index, bufor); //+1, bo "index" może być 0!!!
    }
    /// @}

    /// Konstruktor.
    fix_histogram_source(
            size_t iHowManyC,         ///< Number of categories.
            double   iFixMin,         ///< Ustalone z gory minimum.
            double   iFixMax,         ///< Ustalone z gory maximum.
            DATA_SOURCE *ini = NULL,  ///< Seria źródłowa.
            //Jeśli nie pokrywa się z `minX-maxX`, to faktycznie liczony jest wycinek.(?)
            const char *format = "DISTR_%d_CLASS(%s[%g..%g])",
            bool iSubRange = false, //Jeżeli zakres realny wykracza poza zadany, to robi histogram z części danych.
            sources_manager_base *my_manager = NULL,
            size_t table_size = 11/*BEZ ZAPASU*/
    )
    : Num(iHowManyC), FixMin(iFixMin), FixMax(iFixMax), SubRange(iSubRange),
      basic_statistics_source<DATA_SOURCE>( ini, my_manager,
                                            table_size + iHowManyC,		//Alokuje miejsce na pod-źródła skalarne.
                                            format)
    {
        wb_pchar bufor(strlen(format) + 2 * 100); //Z za dużym zapasem jak na dwa integer-y, ale...
        bufor.prn(format, Num, "%s", FixMin, FixMax);
        basic_statistics_source<DATA_SOURCE>::set_title(bufor.get());
        arra.alloc(Num); //Liczba klas zafiksowana
    }

    /// Destruktor.
    ~fix_histogram_source() = default;

// Methods
//========

    /// Ile koszyków histogramu.
    size_t get_size()
    {
        base_type::check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        return arra.get_size();
    }

    /// Umieszcza wszystkie skalarne pod-źródła w podanym w konstruktorze zarządcy danych.
    void all_subseries_required()	override //Alokuje i ewentualnie rejestruje w zarządcy wszystkie serie
    {
        basic_statistics_source<DATA_SOURCE>::all_subseries_required();
        MainClass();
        WhichMain();
        NumOfClass();
        Entropy();
        for(size_t i = 0; i < Num; i++)
            Categories(i); //Alokacja źródeł liczebności koszyków.
    }

    /// Ile koszyków, liczebności w najmniejszym i największym koszyku.
    void bounds(size_t &num, double &min, double &max) override
    {
        base_type::check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        num = get_size();
        min = base_type::y_min;
        max = base_type::y_max;
    }

    data_source_base::iterator_h reset() override; ///< Umożliwia czytanie wartości koszyków iteratorem od początku.
    void close(data_source_base::iterator_h &p) override; ///< Usuwa iterator
    double get(data_source_base::iterator_h &ptr_to_iterator) override; ///< Daje następną z N wartości koszyków.
    double get(size_t index) override; /// Przetwarza index koszyka na jego wartość.
};

// NAJBARDZIEJ KOSZTOWNE OBLICZENIOWE:
//====================================

template<class DATA_SOURCE>
int fix_histogram_source<DATA_SOURCE>::_calculate()
{
    if(!basic_statistics_source<DATA_SOURCE>::_calculate())
    {
        return 0; //NIC DO ROBOTY, BO NIE BYŁO ZMIAN
    } else
    {//OBLICZANIE HISTOGRAMU
        assert(Num > 1);      //Muszą być jakieś klasy
        assert(arra.IsOK()); //Musi być zaalokowana tablica

        size_t SN;
        double s_min, s_max;
        base_type::Source->bounds(SN, s_min, s_max);

        if((!SubRange) && (s_min < FixMin || FixMax < s_max))	//Czy w zakresie
            goto ERROR;

        arra.fill(0);

        //PĘTLA ZLICZANIA
        data_source_base::iterator_h Ind = base_type::Source->reset();
        base_type::source_miss = base_type::Source->get_missing(); //Trzeba to zrobić, żeby from_source_is_missing_ działało poprawnie!

        size_t Licz = 0, Poza = 0;
        for(size_t j = 0; j < SN; j++)
        {
            double pom = base_type::Source->get(Ind);
            if(!base_type::from_source_is_missing_(pom))
            {
                if(FixMin <= pom && pom < FixMax)
                {
                    Licz++; //Tylko te które, faktycznie weszły w histogram
                    pom = (pom - FixMin) / (FixMax - FixMin); //Wcześniej sprawdzono, że `pom` w zakresie
                    assert(0 <= pom && pom < 1);
                    arra[size_t(trunc(pom * Num))]++;
                } else if(pom == FixMax)	//Wyłapanie maksimum, jeśli jest.
                {
                    Licz++;
                    arra[Num - 1]++; //Arbitralnie do ostatniego koszyka
                } else
                {
                    Poza++; //Te, które nie weszły w histogram
                }
            }
        }
        base_type::Source->close(Ind);

        //PĘTLA	LICZENIA STATYSTYK
        double Entropy = 0;
        size_t licz_klasy = 0, max_p = 0;;

        if(Licz > 0)	//Jest cokolwiek do liczenia
        {
            //size_t min_p = 0;
            base_type::y_min = DBL_MAX;
            base_type::y_max = 0;

            for(size_t i = 0; i < Num; i++)
            {
                double pom = arra[i];

                if(pom > 0)
                    licz_klasy++;

                if(pom > base_type::y_max)
                {
                    base_type::y_max = pom;
                    max_p = i;
                }

                if(pom < base_type::y_min)
                {
                    base_type::y_min = pom;
                    //min_p = i;
                }

                //Liczenie składowych entropi
                double qi = pom / double(Licz);

                //Powiększenie sumy, gdy nie jest to "puste skrzyżowanie".
                if(qi > 0)
                    Entropy += qi * log(qi);

            }
        }

        //AKTUALIZACJA AKTYWNYCH ŹRÓDEŁ STATYSTYCZNYCH
        if(base_type::table[6] != NULL)
        {
            base_type::table[6]->change_val(base_type::y_max);
        }

        if(base_type::table[7] != NULL)
        {
            base_type::table[7]->change_val(licz_klasy);
        }

        if(base_type::table[8] != NULL)
        {
            base_type::table[8]->change_val(double(max_p) + s_min + 0.5); //1/2, bo środek przedziału całkowitego (TU TEŻ?)
        }

        if(base_type::table[9] != NULL)
        {
            if(Entropy != 0)	//Jeśli coś się zsumowało
                base_type::table[9]->change_val(-Entropy);
            else
                base_type::table[9]->change_val(base_type::table[9]->get_missing());
        }

        if(base_type::table[10] != NULL)
        {
            double KL = std::trunc(s_max - s_min + 1); //Ile jednostek całkowitych zakresu realnego
            assert(KL > 0);
            if(Entropy != 0)	//Jeśli coś się zsumowało
                base_type::table[10]->change_val(-Entropy / log(KL) ); //A może powinno być dla zadanego?
            else
                base_type::table[10]->change_val(base_type::table[10]->get_missing() );
        }

        //Jeśli zdefiniowana liczba klas
        for(size_t k = 0; k < Num; k++)
        {
            if(base_type::table[11 + k] != NULL) //"i" może być 0!
            {
                base_type::table[11 + k]->change_val(arra[k]); //Jeden do jednego,
            }
        }

        return 1;
    } //Musial przeliczyć

    ERROR:
    if(base_type::table[10] != NULL)
        base_type::table[10]->change_val(base_type::table[10]->get_missing());
    if(base_type::table[9] != NULL)
        base_type::table[9]->change_val(base_type::table[9]->get_missing());
    if(base_type::table[8] != NULL)
        base_type::table[8]->change_val(base_type::table[8]->get_missing());
    if(base_type::table[7] != NULL)
        base_type::table[7]->change_val(base_type::table[7]->get_missing());
    if(base_type::table[6] != NULL)
        base_type::table[6]->change_val(base_type::table[6]->get_missing());
    arra.dispose();
    base_type::y_min = base_type::y_max = 0;
    return 1;
}


template<class DATA_SOURCE>
size_t fix_histogram_source<DATA_SOURCE>::_next(data_source_base::iterator_h &p)
// Przemieszcza iterator o jednostkę. Zeruje, jeśli koniec tablicy.
{
    assert(p != NULL); //Nie wolno wywołać dla NULL
    size_t pom = ((size_t) p) - 1;

    if(pom + 1 >= Num)
        p = NULL;
    else
        p = (data_source_base::iterator_h) (pom + 2);
    return pom;
}



template<class DATA_SOURCE> inline
double fix_histogram_source<DATA_SOURCE>::get(size_t index)
//Przetwarza index uzyskany z pseudogeometrii na wartość z serii.
{
    base_type::check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
    _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
    assert(index < get_size());
    return arra[index];
}


template<class DATA_SOURCE> inline
double fix_histogram_source<DATA_SOURCE>::get(data_source_base::iterator_h &ptr_to_iterator)
//Daje następną z N liczb!!!
{
    assert(ptr_to_iterator != NULL);
    return arra[_next(ptr_to_iterator)];
}


template<class DATA_SOURCE> inline
void fix_histogram_source<DATA_SOURCE>::close(data_source_base::iterator_h &p)
{
    p = NULL;
}

template<class DATA_SOURCE> inline
data_source_base::iterator_h fix_histogram_source<DATA_SOURCE>::reset()
//Umożliwia czytanie po iteratorze od początku.
{
    base_type::check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
    _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
    return (data_source_base::iterator_h) 1;
}

typedef fix_histogram_source<data_source_base> generic_fix_histogram_source;

}} // end-of-namespaces sym2::data

#pragma clang diagnostic pop
/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/              */
/*        MAIL:  wborkowski@uw.edu.pl                                 */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
#endif

