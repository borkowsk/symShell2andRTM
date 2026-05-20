/// @file
/// @brief __Base type for statistical co-filters - two-series statistics.__ /<br>
///         _Podstawowa klasa dla ko-filtrów statystycznych — statystyk z dwu serii.
/// @date 2026-05-20 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_CO_STAT_SOUR_HPP_INCLUDED_
#define SYMSHELL2_CO_STAT_SOUR_HPP_INCLUDED_

#include "datasour.hpp"
#include "scalsour.hpp"
#include "bifilter.hpp"
#include "sourmngr.hpp"
#include <cmath> /*DLA FILTRÓW */
#include <limits>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

/// @brief @EN{ A class that calculates basic statistical parameters for two sources—e.g., correlations. }
///        @PL{ Klasa licząca podstawowe parametry statystyczne dla dwu źródeł — np. korelacje. }
/// @details
///     Wyniki są dostępne poprzez jednowartościowe źródła pośrednie.
///     `template<class DATA_SOURCE1,class DATA_SOURCE2>` //NA RAZIE NIE JEST SZABLONEM.
class co_statistics_source : public bi_filter_source_base //public template_bi_filter<DATA_SOURCE1,DATA_SOURCE2>
//--------------------------------------------------------
{
protected:
    /// @brief @PL{ Pod-źródło statystyczne prezentujące wynik jako pojedynczy skalar. }
    ///        @EN{ A statistical sub-source that presents the result as a single scalar. }
    /// @details
    /// Nieco sztuczna klasa źródła jednoelementowego z powiadamianiem właściwej klasy "filtra podstawowych statystyk".
    /// Powiadamia, że będą potrzebne aktualne dane.
    /// W klasach potomnych na pewno bezpiecznie można podstawić wirtualną metodę `get`.
    class local_scalar_source : public scalar_source<double>
    {
        co_statistics_source *Father; ///< Klasa nadrzędna dla pod-źródła.

    public:
        local_scalar_source(co_statistics_source *father, const char *title) :
                Father(father),
                scalar_source<double>(0, title)
        {
            assert(local_scalar_source::Father != NULL);
        }

        /// @name Metody, które powiadamiają, że może być potrzebna aktualizacja.
        /// @details
        ///     Każda sprawdza wersję "ojca" i uaktualnia też swoją wersję, jeśli trzeba.
        ///     Po czym sprawdza, czy nie trzeba policzyć u ojca i ewentualnie liczy za pomocą
        ///     `father->_calculate`.
        ///     A potem zwraca wynik wywołania tej samej funkcji w klasie bazowej.
        /// @{

        /// Minimum, maksimum i liczba danych (czyli 1).
        void bounds(size_t &N, double &min, double &max) override
        {
            Father->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
            Father->_calculate(); //Sprawdza, czy nie trzeba policzyć u ojca i ewentualnie liczy
            scalar_source<double>::bounds(N, min, max);
        }

        /// W tym przypadku iterator może zawierać tylko `1' albo NULL.
        iterator_h reset() override
        {
            Father->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
            Father->_calculate(); //Sprawdza, czy nie trzeba policzyć i ewentualnie liczy
            return scalar_source<double>::reset();
        }

        double get() override
        {
            Father->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
            Father->_calculate(); //Sprawdza, czy nie trzeba policzyć i ewentualnie liczy
            return scalar_source<double>::get();
        }
        /// @}
    };

    friend class co_statistics_source::local_scalar_source;

private:
    sources_manager_base *my_sour_manager; ///< Zarządca danych, w którym są pod-źródła.
    int need_calculation; ///< Flaga, czy wymagane jest przeliczenie.

//double Sigma(double sum,double sumSqr,unsigned N)
//	{return sqrt(   ( sumSqr- (sum*sum)/N )  /  (N-1)  );}

protected:
    wb_dynarray<scalar_source<double> *> table; ///< Tablica źródeł jednowartościowych (wynikowych).

    /// Własne sprawdzanie wersji danych.
    /// Nadpisuje wirtualną metodę klasy macierzystej:
    /// - Sprawdza, czy zmieniły się dane w serii źródłowej dla statystyki.
    /// - Znakuje konieczność liczenia bardziej aktualnych wartości w metodzie `_calculate`.
    /// - Znakuje też zmianę w pod-źródłach, na wypadek, gdyby nie obsługiwał ich żaden manager danych.
    int check_version_() override
    {
        int ret = bi_filter_source_base::check_version_();
        if(ret == 1)	//Nowe dane
        {
            need_calculation = 1; //Znacznik liczenia
            //Znakuje zmianę w pod-źródłach, na wypadek, gdyby nie obsługiwał ich żaden manager danych.
            size_t table_size = table.get_size();
            for(size_t i = 0; i < table_size; i++)
                if(table[i] != NULL)
                    table[i]->update_version_from(this);
        }
        return ret;
    }

    /// Leniwe obliczanie statystyk wynikowych. @return 1, jeśli trzeba przeliczyć.
    /// Jeśli nie ma w `dane` swoich źródeł wynikowych, to sam nie liczy, ale może klasa potomna zechce?
    /// Dlatego wtedy też zwraca 1.
    virtual int _calculate()
    {
        if(!need_calculation)
            return 0;

        need_calculation = 0;

        if(table[0] == NULL && table[1] == NULL) //Jeśli nie ma swoich źródeł wynikowych,
            return 1;  // Sam nie liczy, ale udaje, bo może klasa potomna zechce...

        data_source_base *source1 = get_first_source();
        data_source_base *source2 = get_second_source();

        double min, max, Correlation = get_missing();
        size_t N1, N2;
        unsigned Pairs = 0;

        source1->bounds(N1, min, max);
        source2->bounds(N2, min, max);

        if(N1 <= 0 || N2 <= 0) goto ERROR; //Nie da się dalej liczyć

        {//LICZENIE PAR i ŚREDNICH
            double X_s = 0, Y_s = 0, summ1 = 0, summ2 = 0, summ3 = 0;

            iterator_h Ind1 = source1->reset();
            iterator_h Ind2 = source2->reset();
            while(Ind1 && Ind2) //Puki oba iteratory
            {
                double pom1 = source1->get(Ind1);
                double pom2 = source2->get(Ind2);
                if((!from_first_is_missing(pom1)) &&
                   (!from_second_is_missing(pom2))
                        )
                {
                    Pairs++;
                    X_s += pom1;
                    Y_s += pom2;
                }
            }
            source1->close(Ind1);
            source2->close(Ind2);

            if(table[1] == NULL || Pairs == 0) goto ERROR; //Nie da się albo nie trzeba dalej liczyć

            /// Konwersja z unsigned na double jest zawsze bezpieczna, ale jak user zmieni typy tych zmiennych...
            static_assert(std::numeric_limits<decltype(Pairs)>::digits<=std::numeric_limits<decltype(X_s)>::digits,
                          "There may be a loss of precision.");
            X_s /= Pairs;
            Y_s /= Pairs;

            //liczenie składowych korelacji
            Ind1 = source1->reset();
            Ind2 = source2->reset();
            while(Ind1 && Ind2) //Puki oba iteratory
            {
                double pom1 = source1->get(Ind1);
                double pom2 = source2->get(Ind2);
                if((!from_first_is_missing(pom1)) &&
                   (!from_second_is_missing(pom2))
                        )
                {
                    summ1 += (X_s - pom1) * (Y_s - pom2);
                    summ2 += (X_s - pom1) * (X_s - pom1);
                    summ3 += (Y_s - pom2) * (Y_s - pom2);
                }
            }
            source1->close(Ind1);
            source2->close(Ind2);

            if(summ2 <= 0 || summ3 <= 0) goto ERROR; // Bo pierwiastek i dzielenie
            Correlation = summ1 / (sqrt(summ2) * sqrt(summ3));
        } //KONIEC LICZENIA

        ERROR: //Tu ląduje przeskok obliczeń
        if(table[0] != NULL)
        {
            table[0]->change_val(Pairs);
        }

        if(table[1] != NULL)
        {
            table[1]->change_val(Correlation);
        }

        return 1; //Musial przeliczyć
    }

    /// Alokuje źródła jednowartościowe dla parametrów statystycznych.
    scalar_source<double> *GetMonoSource(size_t select, const char *format = NULL)
    {
        if(table[select] == NULL)	//jeśli nie ma to alokuj
        {
            //Przygotuj title z formatu
            const char *sour_name1 = get_first_source()->name();
            const char *sour_name2 = get_second_source()->name();

            wb_pchar title(strlen(format) + strlen(sour_name1) + strlen(sour_name2) + 10);
            sprintf(title.get_ptr_val(), format, sour_name1, sour_name2);

            scalar_source<double> *ptr = new local_scalar_source(this, title.get_ptr_val());
            table[select] = ptr;

            if(//ptr != NULL && // Już nie trzeba reagować na NULL.
                my_sour_manager != NULL)
                //jeśli OK to trzeba zgłosić zarządcy danych!
                my_sour_manager->insert(ptr, 1/*MEMORY IS NOT MANAGED!*/);
        }
        return table[select];
    }

    /// Raczej nieużywana (Bo `bounds` zwraca N == 0!) implementacja decyzji o wartości minimalnej i maksymalnej
    void _bounds( double &min1, double &max1,
                  double &min2, double &max2,
                  double &min, double &max) override
    {
        //Jeśli nie ustawione to przyjmujemy typowy zakres korelacji
        min = -1;
        max = 1;
    }

    /// Raczej nieużywane w tej klasie.
    double _get(double val1, double val2) override
    {
        return get_missing();
    }

public:
    /// Ma dwa pod-źródła, ale klasy potomne mogą mieć więcej.
    virtual size_t number_of_subseries()
    {
        return 2;
    }

    virtual void all_subseries_required()	//Alokuje i ewentualnie rejestruje w zarządcy wszystkie serie
    {
        Pair();
        Corr();
    }

    /// Liczba wspólnych par w obu źródłach (tzn. gdy oba nie są "missing").
    scalar_source<double> *Pair(const char *format = "Pair(%s,%s)")
    { return GetMonoSource(0, format); }

    /// Korelacja między dwoma źródłami.
    scalar_source<double> *Corr(const char *format = "Corr(%s,%s)")
    { return GetMonoSource(1, format); }

    ///CONSTRUCTION.
    /// \param ini1 pierwsze źródło do skorelowania
    /// \param ini2 drugie źródło do skorelowania.
    /// \param data_manager zarządca danych
    /// \param table_size dodatkowej rozmiar
    /// \param format to sposób tworzenia nazwy tego filtru z nazw jego źródeł danych.
    explicit co_statistics_source( data_source_base *ini1 = NULL,
                                   data_source_base *ini2 = NULL,
                                   sources_manager_base *data_manager = NULL,
                                   size_t table_size = 0,
                                   const char *format = "CO_STATS(%s,%s)")
    : bi_filter_source_base(ini1, ini2, format),
      table(2 + table_size),		//Dwa obowiązkowe, reszta z klas potomnych
      my_sour_manager(data_manager),
      need_calculation(1)
    {
        for(size_t i = 0; i < table.get_size(); i++)
            table[i] = NULL;
    }

    /// Wirtualny destructor.
    ~co_statistics_source() override
    {
        size_t table_size = table.get_size();

        if(my_sour_manager != NULL)
        {
            for(size_t i = 0; i < table_size; i++)
                if(table[i] != NULL)
                    //Są insertowane jako "not managed" więc tu tylko...
                    my_sour_manager->replace(table[i]->name(), NULL); //wywala z zarządcy
        }

        for(size_t i = 0; i < table_size; i++)
            if(table[i] != NULL)
                delete table[i]; //Sam sobie wywala z pamięci

    }

    /// Wymiana dołączonego wcześniej zarządcy.
    /// Linkuje nowemu zarządcy pod-serie, wygenerowane przez siebie.
    /// @param usun == 1, to pod-serie usuwa najpierw z poprzedniego.
    [[maybe_unused]]
    void link_sources_manager(sources_manager_base *new_manager, int usun = 1)
    {
        size_t table_size = table.get_size();

        if(usun && my_sour_manager != NULL)
            for(size_t i = 0; i < table_size; i++)
                if(table[i] != NULL)
                    //Są insertowane jako "not managed" więc tu tylko...
                    my_sour_manager->replace(table[i]->name(), NULL); //wywala z zarządcy

        my_sour_manager = new_manager;
        if(my_sour_manager != NULL)	//Jeśli nie NULL to umieść
            for(size_t i = 0; i < table_size; i++)
                if(table[i] != NULL)
                    my_sour_manager->insert(table[i], 1/*NOT MANAGED*/);
    }

    /// Dlaczego podaje, że nie ma danych?
    /// Bo nie ma! Wszystkie wyniki są podawane za pośrednictwem pod-źródeł skalarnych.
    void bounds(size_t &N, double &min, double &max) override
    {
        check_version_(); //Tu może być update
        //bi_filter_source_base::bounds(N,min,max);
        min = max = 0;
        N = 0; //Nie można sięgać do tych danych bezpośrednio jako do seri
    }

    /// Nie ma w tym źródle możliwości bezpośredniego czytania danych.
    iterator_h reset() override
    {
        check_version_();
        return NULL;
    }

};

typedef co_statistics_source generic_co_statistics_source;

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
#endif //SYMSHELL2_CO_STAT_SOUR_HPP_INCLUDED_

