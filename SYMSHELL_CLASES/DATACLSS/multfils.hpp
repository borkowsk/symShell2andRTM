/// @file
/// @brief __The base type for filters containing subsources__ /<br>
///         _Podstawowa klasa dla filtrów zawierających pod-źródła_
/// @date 2026-05-18 (modified)
//==============================================================================
//
#ifndef SYMSHELL2_MULTI_FILTER_SOUR_HPP_INCLUDED_
#define SYMSHELL2_MULTI_FILTER_SOUR_HPP_INCLUDED_

#include <cmath> /*DLA FILTRÓW POTOMNYCH? */
#include "datasour.hpp"
#include "simpsour.hpp"
#include "sourmngr.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

/// @brief
///   \PL{ Szablon klasy bazowej dla klas liczących podstawowe parametry statystyczne innego źródła. }
///   \EN{ Base template for data sources calculating basic statistical parameters from another source. }
/// @details
///   \PL{ Wyliczone parametry są podawane w arbitralnej kolejności lub poprzez skalarne źródła pośrednie. }
///   \EN{ The calculated parameters are given in an arbitrary order or through scalar intermediate sources. }
template<class DATA_SOURCE>
class multi_filter_source_base : public template_filter_source_base<DATA_SOURCE>
//------------------------------------------------------------------------------
{
protected:
    /// @brief \PL{ Źródło skalarne na wyniki. } \EN{ Scalar source for results. }
    /// @if ENGLISH
    /// @details
    /// A somewhat artificial single-element source type with notification to the appropriate "basic statistics filter" `father`.
    /// Informs that current data will be needed.
    /// In descendant types, it's certainly safe to substitute a virtual `get` method.
    /// @elseif POLISH
    /// @details
    /// Nieco sztuczna klasa źródła jednoelementowego z powiadamianiem właściwej klasy "filtra podstawowych statystyk".
    /// Informuje, że będą potrzebne aktualne dane.
    /// W klasach potomnych na pewno bezpiecznie można podstawić wirtualną metodę `get`.
    /// @endif
    class local_scalar_source : public scalar_source<double>
    {
        multi_filter_source_base *Father;

    public:
        /// Konieczny konstruktor.
        local_scalar_source(multi_filter_source_base *father, const char *title)
        : scalar_source<double>(0, title), Father(father)
        {
            assert(local_scalar_source::Father != NULL);
        }

        /// @name Metody, które powiadamiają, że może być potrzebna aktualizacja.
        /// @details
        ///      Każda sprawdza/uaktualnia wersje "ojca", potem wywołuje metoda `_calculate`.
        /// @{
        void bounds(size_t &N, double &min, double &max) override
        {
            Father->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
            Father->_calculate(); //Sprawdza, czynie trzeba policzyć u ojca i ewentualnie liczy
            scalar_source<double>::bounds(N, min, max);
        }

        iterator_h reset() override
        {
            Father->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
            Father->_calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
            return scalar_source<double>::reset();
        }

        double get() override
        {
            assert(Father != NULL);
            Father->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
            Father->_calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
            return scalar_source<double>::get();
        }

        double get(size_t index_from_geometry) override
        {
            assert(Father != NULL);
            //Przetwarza index uzyskany z geometrii, choć tu zawsze na tę sama wartość, czyli na wartość z serii.
            Father->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
            Father->_calculate(); //Sprawdza, czy nie trzeba policzyć i ewentualnie liczy
            return scalar_source<double>::get();
        }
        /// @}
    };

//	friend class basic_statistics_source<DATA_SOURCE>::local_scalar_source;
    friend class local_scalar_source;

private:
    sources_manager_base *sources_manager; ///< Wskaźnik do ZEWNĘTRZNEGO zarządcy danych.
    int need_calculation; ///< Flaga konieczności ponownego wyliczenia danych pochodnych.

protected:
    wb_dynarray<scalar_source<double> *> table; ///< Tablica źródeł jednowartościowych z wynikami obliczeń.

    /// Implementacja sprawdzania.
    /// Nadpisuje wirtualną metodę klasy macierzystej:
    /// - Sprawdza, czy zmieniły się dane w źródle.
    /// - Uaktualnia wersję danych w pod-źródłach.
    /// - Znakuje konieczność liczenia aktualnej wartości w metodzie _calculate()
    int check_version_() override
    {
        int ret = template_filter_source_base<DATA_SOURCE>::check_version_();
        if(ret == 1)	//Są nowe dane
        {
            need_calculation = 1; //Znacznik konieczności liczenia
            //Znakuje zmianę w pod-źródłach, na wypadek, gdyby nie obsługiwał ich zarządca danych.
            size_t table_size = table.get_size();
            for(size_t i = 0; i < table_size; i++)
                if(table[i] != NULL)
                    table[i]->update_version_from(this);
        }
        return ret;
    }

    /// Wersja domyślna dla `multi_filter_source_base` tylko zeruje flagę "need calculation".
    /// @return 1, jeśli uznał, że trzeba przeliczyć.
    virtual int _calculate() //
    {
        if(!need_calculation) return 0;
        need_calculation = 0;
        return 1;
    }

    /// @name Constructor and destructor
    /// @{

    /// Konstruktor 1.
    explicit multi_filter_source_base( DATA_SOURCE                 *ini = NULL,
                                       sources_manager_base *my_manager = NULL,
                                       size_t                table_size = 0,
                                       const char               *format = "FILTER_OF(%s)")
    : template_filter_source_base<DATA_SOURCE>(ini, format),
      table(table_size), sources_manager(my_manager), need_calculation(1)
    {
        for(size_t i = 0; i < table_size; i++)
            table[i] = NULL;
    }

    /// Destructor.
    ~multi_filter_source_base() override
    {
        size_t table_size = table.get_size();

        if(sources_manager != NULL)
        {
            for(size_t i = 0; i < table_size; i++)
                if(table[i] != NULL)
                    //Są insertowane jako "not managed" więc tu tylko...
                    sources_manager->replace(table[i]->name(), NULL); //wywala z zarządcy
        }

        for(size_t i = 0; i < table_size; i++)
            if(table[i] != NULL)
                delete table[i]; //Sam sobie wywala z pamięci
    }

public:
    /// Wymienia dołączonego zarządcę danych i linkuje mu pod-serie.
    /// Jeśli "usun == 1" to usuwa je najpierw z poprzedniego zarządcy.
    void link_sources_manager(sources_manager_base *new_manager, int usun = 1)
    {
        size_t table_size = table.get_size();

        if(usun && sources_manager != NULL)
            for(size_t i = 0; i < table_size; i++)
                if(table[i] != NULL)
                    //Są insertowane jako "not managed" więc tu tylko...
                    sources_manager->replace(table[i]->name(), NULL); //wywala z zarządcy

        sources_manager = new_manager;
        if(sources_manager != NULL)	//Jeśli nie NULL to umieść
            for(size_t i = 0; i < table_size; i++)
                if(table[i] != NULL)
                    sources_manager->insert(table[i], 1/*NOT MANAGED*/);
    }

    /// Zwraca minimum i maksimum z danych źródłowych, ale N zwraca 0.
    /// Chodzi chyba o to, żeby zablokować czytanie, które dla klasy bazowej jest bez sensu.
    void bounds(size_t &N, double &min, double &max)
    {
        check_version_(); //Tu może być update, bo zakres musi być zawsze aktualny
        template_filter_source_base<DATA_SOURCE>::bounds(N, min, max);
        N = 0; //Nie można sięgać do tych danych bezpośrednio jako do seri.
    }

    data_source_base::iterator_h reset()	//Nie ma możliwości bezpośredniego czytania
    {
        check_version_();
        return NULL;
    }

    scalar_source<double> *GetMonoSource(size_t select, const char *format = NULL)
    //Alokuje źródła jednowartościowe dla parametrów statystycznych
    {
        if(table[select] == NULL)	//Jeśli nie ma to alokuj
        {
            //Przygotuj title z formatu
            const char *sour_name = this->Source->name();
            wb_pchar title(strlen(format) + strlen(sour_name) + 10);
            sprintf(title.get_ptr_val(), format, sour_name);
            scalar_source<double> *ptr = new local_scalar_source(this, title.get_ptr_val());
            table[select] = ptr;

            if( //ptr != NULL && //Już nie może się nie zaalokować, więc nie trzeba zareagować na NULL.
                sources_manager != NULL)
                //Jeśli OK to trzeba zgłosić zarządcy danych!
                sources_manager->insert(ptr, 1/*NOT MANAGE MEMORY*/);
        }
        return table[select];
    }

    virtual void all_subseries_required() = 0; //Alokuje i ewentualnie rejestruje w zarządcy wszystkie serie
};

typedef multi_filter_source_base<data_source_base> generic_multi_filter_source_base;

}} // end-of-namespaces sym2::data

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
#endif

