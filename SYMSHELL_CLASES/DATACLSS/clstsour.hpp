/// @file
/// @brief **Filter counting different types of clustering for series with geometry** /<br>
///         _Filtr liczący różne rodzaje klasteringu dla serii z  g e o m e t r i ą.__
/// @date 2026-05-19 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_CLUST_SOUR_HPP_INCLUDED_
#define SYMSHELL2_CLUST_SOUR_HPP_INCLUDED_

#include "statsour.hpp"
#include "layer.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

namespace sym2 { namespace data {

/// @brief @PL{ Filtr liczący różne rodzaje klasteringu dla serii z   g e o m e t r i ą!!! }
///        @EN{...}
template<class DATA_SOURCE>
class clustering_source : public basic_statistics_source<DATA_SOURCE>
//------------------------------------------------------------------------------
{
public:
    typedef basic_statistics_source<DATA_SOURCE> basics_; ///< Klasa bazowa.
    using basics_::table; ///< Skrócony dostęp do tablicy danych klasy bazowej

protected:
    /// Dosyć kosztowna obliczeniowo, jeśli liczy. Zwraca 1, jeśli musiał przeliczyć.
    int _calculate();

public:
    explicit clustering_source( DATA_SOURCE          *ini = NULL,
                                sources_manager_base *my_manager = NULL,
                                size_t                table_size = 7 + 1/*ZAPAS*/,
                                const char           *format = "CLUSTERING_STATS(%s)")
    : basic_statistics_source<DATA_SOURCE>(ini, my_manager, table_size, format)
    {}

    /// Destruktor.
    ~clustering_source() = default;

    /// Skalarne pod-źródło prezentujące.
    scalar_source<double> *Stress(const char *format = "Stress(%s)")
    {
        return basics_::GetMonoSource(6, format);
    }

    /// Alokuje i ewentualnie rejestruje w zarządcy wszystkie serie. W tym przypadku jedną.
    void all_subseries_required()
    {
        basic_statistics_source<DATA_SOURCE>::all_subseries_required();
        Stress();
    }

};

// IMPLEMENTACJA CIĘŻKIEJ FUNKCJI OBLICZENIOWEJ:
//==============================================

template<class DATA_SOURCE>
int clustering_source<DATA_SOURCE>::_calculate()
{
    if(!basic_statistics_source<DATA_SOURCE>::_calculate())
        return 0;

    unsigned testowanie = 0; //StepCounter wartości centralnych
    double Stress = basics_::miss;      //Suma stresów
    sym2::geometry *MyGeom = basics_::Source->get_geometry(); //Wskaźnik do geometrii

    if(MyGeom != NULL) //Jest znana geometria — da się policzyć
    {
        double suma = 0;

        //Alokujemy iterator
        data_source_base::iterator_h Glob = MyGeom->make_global_iterator();

        //Idziemy po agentach iteratorem.
        while(Glob)
        {
            size_t index = MyGeom->get_next(Glob); // Uzyskujemy index agenta...
            assert(index != sym2::any_layer_base::FULL); // Tutaj nie powinno się zdarzyć
            double CenterVal = basics_::Source->get(index);   // Uzyskujemy referencje do agenta
            if(basics_::Source->is_missing(CenterVal))        // Sprawdzamy, czynie jest miss.
                continue;                          // bo wtedy robić dalej byłoby bez sensu.

            // Alokujemy iterator sąsiedztwa
            data_source_base::iterator_h Neigh = MyGeom->make_neighbour_iterator(index, 1);
            unsigned zliczanie = 0; //Zliczanie sąsiadów
            unsigned tacy_sami = 0; //Zliczanie takich samych sąsiadów
            unsigned      inni = 0; //Zliczanie innych samych sąsiadów

            while(Neigh)
            {
                size_t index2 = MyGeom->get_next(Neigh); //Uzyskujemy index sąsiada
                if(index2 == sym2::any_layer_base::FULL || index2 == index)    //jeśli poza obszarem symulacji lub w
                    continue;                   //centrum obszaru to dalej byłoby bez sensu.

                double pery_val = basics_::Source->get(index2);  //Uzyskujemy referencje do sąsiada
                if(basics_::Source->is_missing(pery_val))        //Sprawdzamy, czynie jest miss.
                    continue;                    // bo wtedy robić dalej byłoby bez sensu.

                zliczanie++;
                if(CenterVal == pery_val)
                    tacy_sami++;
                else
                    inni++;

            }

            MyGeom->destroy_iterator(Neigh);    // upewniamy się ze iterator zostanie usunięty

            if(zliczanie > 0)
            {
                suma += double(inni) / double(zliczanie);
                //Zlicza ile było wartości
                testowanie++;
            }
        }

        // upewniamy się ze iterator zostanie usunięty
        MyGeom->destroy_iterator(Glob);
        Stress = suma / testowanie;
    }

    if(table[6] != NULL)
    {
        table[6]->change_val(Stress);
    }

    return 1; //Musial przeliczyć
}

    typedef clustering_source<data_source_base> generic_clustering_source;

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

