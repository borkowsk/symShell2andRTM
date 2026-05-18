/// @file
/// @brief __Base type for ALL statistical filters.__ /<br>
///         _Podstawowa klasa dla filtrów statystycznych._
/// @date 2026-05-18 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_STAT_SOUR_HPP_INCLUDED_
#define SYMSHELL2_STAT_SOUR_HPP_INCLUDED_

#include <cmath> /*DLA FUNKCJI FILTRÓW */
#include "simpsour.hpp"
#include "sourmngr.hpp"
#include "multfils.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

/// Bazowy szablon klasy liczącej podstawowe parametry statystyczne innego źródła.
/// Parametry są podawane w arbitralnej kolejności lub poprzez jednowartościowe źródła pośrednie.
template<class DATA_SOURCE>
class basic_statistics_source : public multi_filter_source_base<DATA_SOURCE>
//------------------------------------------------------------------------------
{
public:
    friend class local_scalar_source;
    typedef ::size_t size_t;

private:
    /// Prywatna pomocnicza funkcja Sigma.
    double Sigma(double sum, double sumSqr, unsigned N)
    {
        if(N == 0 || N == 1)
            return this->miss;
        double pom = (sumSqr - (sum * sum) / N) / (N - 1);
        if(pom >= 0)
            return sqrt(pom);
        else if(pom > -0.00000000001)
            return 0; //Na skutek błędów numerycznych może czasem nie wyjść 0.
        else
            return this->miss;
    }

protected:
    /// Leniwe obliczanie. Zwraca 1, jeśli musial przeliczyć.
    int _calculate() override;

public:
    /// Konstruktor.
    /// \param ini to wskaźnik do źródła danych, które ma być analizowane.
    /// \param my_manager to wskaźnik do zarządcy danych.
    /// \param table_size to rozmiar tablicy pod-źródeł. Ta klasa potrzebuje aż 6.
    /// \param format to sposób tworzenia nazwy tego obiektu z nazwy obiektu źródłowego.
    explicit basic_statistics_source( DATA_SOURCE *ini = NULL,
                                      sources_manager_base *my_manager = NULL,
                                      size_t table_size = 6,
                                      const char *format = "BASIC_STATS(%s)")
    : multi_filter_source_base<DATA_SOURCE>(ini, my_manager, table_size, format)
    {}

    /// Destruktor.
    ~basic_statistics_source() override = default;

    /// @name Wynikowe pod-źródła skalarne.
    /// @{

    /// Deklarowane N.
    scalar_source<double> *LenN(const char *format = "lenN(%s)")
    { return this->GetMonoSource(5, format); }

    /// Rzeczywiste N, po odjęciu "missing values".
    scalar_source<double> *RealN(const char *format = "N(%s)")
    { return this->GetMonoSource(4, format); }

    scalar_source<double> *Min(const char *format = "Min(%s)")
    { return this->GetMonoSource(0, format); }

    scalar_source<double> *Max(const char *format = "Max(%s)")
    { return this->GetMonoSource(1, format); }

    scalar_source<double> *Mean(const char *format = "Mean(%s)")
    { return this->GetMonoSource(2, format); }

    /// Standard deviation.
    scalar_source<double> *SD(const char *format = "SD(%s)")
    { return this->GetMonoSource(3, format); }

    /// Alokuje i ewentualnie rejestruje w zarządcy wszystkie pod-źródła.
    void all_subseries_required() override
    {
        LenN();
        RealN();
        Min();
        Max();
        Mean();
        SD();
    }
    /// @}
};

// IMPLEMENTACJA OBLICZEŃ:
//========================

template<class DATA_SOURCE>
int basic_statistics_source<DATA_SOURCE>::_calculate()
{
    if(multi_filter_source_base<DATA_SOURCE>::_calculate() == 0)
        return 0;

    double mean = data_source_base::get_missing(); ///< Efekt uboczny. Żeby załadować "miss" do cache-a.
    double min = this->miss;                       ///< Pobieramy "miss" z cache-u.
    double max = mean;                             ///< W `mean` jest też na początku "miss".
    double values_sum = 0, squares_sum = 0;        ///< Sumu muszą być na początku zerowane.

    size_t N = 0;        ///< Pobierane przez `bounds`.
    size_t counter = 0;  ///< Liczba wartości "nie-missing".

    this->Source->bounds(N, min, max);

    if(N > 0)
    {
        data_source_base::iterator_h Ind = this->Source->reset();
        this->source_miss = this->Source->get_missing();
        for(decltype(N) i = 0; i < N; i++)
        {
            double pom = this->Source->get(Ind);
            if(!filter_source_base::from_source_is_missing_(pom))
            {
                counter++;
                if(pom > max)
                    max = pom;
                if(pom < min)
                    min = pom;
                values_sum += pom;
                squares_sum += pom * pom;
            }
        }
        this->Source->close(Ind);
    }

    if(this->table[0] != NULL)
    {
        this->table[0]->change_val(min);
    }
    if(this->table[1] != NULL)
    {
        this->table[1]->change_val(max);
    }
    if(this->table[2] != NULL) //Mean
    {
        if(counter > 0)
            mean = values_sum / double(counter); // Raczej możemy zignorować "utratę precyzji". Musiałoby być ogromnie dużo danych.
        else
            mean = 0; //TODO A dlaczego nie "missing"?
        this->table[2]->change_val(mean);
    }
    if(this->table[3] != NULL ) //SD
    {
        double std_dev = this->miss;
        if(counter >= 2) //Zostanie "missing" jak nie policzy.
            std_dev = Sigma(values_sum, squares_sum, counter);
        this->table[3]->change_val(std_dev);
    }
    if(this->table[4] != NULL) //RealN
    {
        this->table[4]->change_val(counter);
    }
    if(this->table[5] != NULL)
    {
        this->table[5]->change_val(N);
    }

    return 1; //Musial przeliczyć
}

typedef basic_statistics_source<data_source_base> generic_basic_statistics_source;
typedef basic_statistics_source<data_source_base> generic_statistics_source;

}} // end-of-namespaces sym2::data

#pragma clang diagnostic pop
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
#endif


