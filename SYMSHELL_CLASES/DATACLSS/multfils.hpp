/// @file
/// @brief Podstawowa klasa dla filtrow zawierających podzrodla
/// @date 2026-05-07 (modified)
// /////////////////////////////////////////////////////////////////////////////
//
#ifndef __MULTIFILTSOUR_HPP__
#define __MULTIFILTSOUR_HPP__

#include "math.h" /*DLA FILTROW */
#include "datasour.hpp"
#include "simpsour.hpp"
#include "sourmngr.hpp"

//KLasa liczaca podstawowe parametry statystyczne innego źródła.
//Parametry są podawne w arbitralnej kolejnosci lub poprzez jednowartosciowe
//zrodła pośrednie
template<class DATA_SOURCE>
class multi_filter_source_base : public template_filter_source_base<DATA_SOURCE>
//------------------------------------------------------------------------------
{
protected:

    class local_scalar_source : public scalar_source<double>
        //Nieco sztuczna klasa źródła jednoelementowego z powiadamianiem
        //właściwej klasy "filtra podstawowych statystyk" ze beda potrzebne
        //aktualne dane. W klasach potomnych na pewno bezpiecznie można podstawic
        //virtualna metodę get().
    {
        multi_filter_source_base *Father;

    public:
        local_scalar_source(multi_filter_source_base *father, const char *title) :
                Father(father),
                scalar_source<double>(0, title)
        {
            assert(local_scalar_source::Father != NULL);
        }

        //Metody ktore powiadamiaja ze może być potrzebna aktualizacja
        //-----------------------------------------------------------------
        void bounds(size_t &N, double &min, double &max)
        {
            Father->check_version_(); //Uaktualnia tez wersje podzrodla, jeśli trzeba
            Father->_calculate(); //Sprawdza, czynie trzeba policzyc u ojca i ewentualnie liczy
            scalar_source<double>::bounds(N, min, max);
        }

        iteratorh reset()
        {
            Father->check_version_(); //Uaktualnia tez wersje podzrodla, jeśli trzeba
            Father->_calculate(); //Sprawdza, czynie trzeba policzyc i ewentualnie liczy
            return scalar_source<double>::reset();
        }

        double get()
        {
            assert(Father != NULL);
            Father->check_version_(); //Uaktualnia tez wersje podzrodla, jeśli trzeba
            Father->_calculate(); //Sprawdza, czynie trzeba policzyc i ewentualnie liczy
            return scalar_source<double>::get();
        }

        double get(size_t index_from_geometry)
        {
            assert(Father != NULL);
            //Przetwarza index uzyskany z geometrii - tu zawsze na ta sama wartość
            //na wartość z serii, o ile jest możliwe czytanie losowe
            Father->check_version_(); //Uaktualnia tez wersje podzrodla, jeśli trzeba
            Father->_calculate(); //Sprawdza, czynie trzeba policzyc i ewentualnie liczy
            return scalar_source<double>::get();
        }

    };

//	friend class basic_statistics_source<DATA_SOURCE>::local_scalar_source;
    friend class local_scalar_source;

private:
    sources_menager_base *sour_menager;
    int need_calculation;

protected:
    wb_dynarray<scalar_source<double> *> table; //Tablica źródeł jednowartosciowych
    int check_version_()
    //Nadpisuje virtualna metodę klasy macierzystej:
    //Sprawdza, czyzmienily się dane w zrodle
    //i znakuje koniecznosc liczenia aktualnej wartości w metodzie _calculate()
    {
        int ret = template_filter_source_base<DATA_SOURCE>::check_version_();
        if(ret == 1)	//Nowe dane
        {
            need_calculation = 1; //Znacznik liczenia
            //Znakuje zmiane w podzrodlach na wypadek gdyby
            //nie obslugiwal ich menager danych.
            size_t table_size = table.get_size();
            for(size_t i = 0; i < table_size; i++)
                if(table[i] != NULL)
                    table[i]->update_version_from(this);
        }
        return ret;
    }

    virtual int _calculate() //Zwraca 1, jeśli musial przeliczyc
    {
        if(!need_calculation) return 0;
        need_calculation = 0;
        return 1;
    }

public:
    multi_filter_source_base(DATA_SOURCE *ini = NULL, sources_menager_base *MyMenager = NULL, size_t table_size = 0,
                             const char *format = "FILTER_OF(%s)") :
            template_filter_source_base<DATA_SOURCE>(ini, format),
            table(table_size),
            sour_menager(MyMenager),
            need_calculation(1)
    {
        for(size_t i = 0; i < table_size; i++)
            table[i] = NULL;
    }

    ~multi_filter_source_base()
    {
        size_t table_size = table.get_size();

        if(sour_menager != NULL)
        {
            for(size_t i = 0; i < table_size; i++)
                if(table[i] != NULL)
                    //Sa insertowane jako "not menaged" wiec tu tylko
                    sour_menager->replace(table[i]->name(), NULL); //...wywala z zarządcy
        }

        for(size_t i = 0; i < table_size; i++)
            if(table[i] != NULL)
                delete table[i]; //Sam sobie wywala z pamięci
    }

    void link_sources_menager(sources_menager_base *NewMenager, int usun = 1)
    //Wymienia dolaczonego zarządcy i linkuje mu podserie
    //Jeśli "usun==1" to usuwa je najpierw z poprzedniego
    {
        size_t table_size = table.get_size();

        if(usun && sour_menager != NULL)
            for(size_t i = 0; i < table_size; i++)
                if(table[i] != NULL)
                    //Sa insertowane jako "not menaged" wiec tu tylko
                    sour_menager->replace(table[i]->name(), NULL); //...wywala z zarządcy

        sour_menager = NewMenager;
        if(sour_menager != NULL)	//Jeśli nie NULL to umiesc
            for(size_t i = 0; i < table_size; i++)
                if(table[i] != NULL)
                    sour_menager->insert(table[i], 1/*NOT MENAGED*/);
    }


    void bounds(size_t &N, double &min, double &max)
    {
        check_version_(); //Tu może być update, bo zakres musi być zawsze aktualny
        template_filter_source_base<DATA_SOURCE>::bounds(N, min, max);
        N = 0; //Nie można siegac do tych danych bezposrednio jako do seri
    }

    data_source_base::iteratorh reset()	//Nie ma możliwości bezposredniego czytania
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
            //Może się nie zaalokowac - trzeba zareagowac na NULL
            //----------------------------------------------------
            if(ptr != NULL && sour_menager != NULL)
                //Jeśli OK to trzeba zgłosic menagerowi danych!
                sour_menager->insert(ptr, 1/*NOT MENAGE MEMORY*/);
        }
        return table[select];
    }

    virtual void all_subseries_required() = 0;//Alokuje i ewentualnie rejestruje w menagerze wszystkie serie
};

typedef multi_filter_source_base<data_source_base> generic_multi_filter_source_base;

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

