/// @file
/// @brief __A filter that calculates the number of series classes and derived spatial statistics, including spatial correlations.__ /<br>
///         _Filtr liczący liczebność klas serii i pochodne statystyki przestrzenne, w tym korelacje przestrzenne._
/// @date 2026-05-18 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_SPATIAL_CORRELATION_SOUR_HPP_INCLUDED_
#define SYMSHELL2_SPATIAL_CORRELATION_SOUR_HPP_INCLUDED_

#include <cmath>
#include "multfils.hpp"
#include "wb_rand.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

/// @brief Filtr liczący  pochodne statystyki przestrzenne, w tym korelacje przestrzenne, ale też liczebność klas serii(?).
template<class DATA_SOURCE>
class spatial_correlation_source : public multi_filter_source_base<DATA_SOURCE>
//------------------------------------------------------------------------------
{
protected:
    size_t                           N;  ///< Number of distance categories/bins;
    int                      rand_mult;  ///< How many times for random counting. If 0 the _count_all() is used instead _count_randomly()
    wb_dynarray<double>           arra;  ///< Data. Musi być `double`, bo inaczej źle przechowuje "missing value"
    wb_dynarray<unsigned>     the_same;
    wb_dynarray<unsigned> not_the_same;

    /// Przemieszcza iterator o jednostkę(?). Zeruje, jeśli koniec tablicy.
    size_t _next(data_source_base::iterator_h &p)
    {
        assert(p != NULL); //Nie wolno wywołać dla NULL
        size_t pom = ((size_t) p) - 1;
        if(pom + 1 >= N)
            p = NULL;
        else
            p = (data_source_base::iterator_h) (pom + 2); //     DLACZEGO 2?
        return pom;
    }

    /// Wykonuje obliczenia statystyk przestrzennych, przechodząc wszystkie możliwości.
    bool _count_all();

    /// Wykonuje obliczenia statystyk przestrzennych, przechodząc losową próbkę możliwości.
    bool _count_randomly();

    /// Wirtualne, leniwe obliczanie. Zwraca 1, jeśli musiał przeliczyć.
    int _calculate();

public:
    /// Konstruktor.
    /// \param ini to wskaźnik do źródła danych, które ma być analizowane.
    /// \param number_of_categories to liczba klas histogramu(?), gdzie `-1` oznacza tryb całkowitoliczbowy.
    /// \param count_mode to multiplikator liczby losowań. Jednak 0 oznacza przeliczenie wszystkich możliwości.
    /// \param my_manager to wskaźnik do zarządcy danych.
    /// \param table_size to rozmiar tablicy pod-źródeł. Ta klasa potrzebuje tylko jednego.
    /// \param format to sposób tworzenia nazwy tego obiektu z nazwy obiektu źródłowego.
    explicit spatial_correlation_source( DATA_SOURCE *ini = NULL,
                                         size_t number_of_categories = -1,
                                         int count_mode = 2,
                                         sources_manager_base *my_manager = NULL,
                                         size_t table_size = 1 /*DAJEMY JEDNO POD_ŹRÓDŁO, BEZ ZAPASU*/,
                                         const char *format = "SPATIAL CORRELATION(%s)") 
    : multi_filter_source_base<DATA_SOURCE>(ini, my_manager, table_size, format),
      rand_mult(count_mode), N(number_of_categories)
    {}

    ~spatial_correlation_source() = default;

    /// @name Metody związane z wynikowym źródłem skalarnym.
    /// @{

    /// @brief Udostępnia jedyne dodatkowe pod-źródło danych: przybliżony rozmiar klastra.
    scalar_source<double> *ApproximatedClusterSize(const char *format = "ClustSize(%s)")
    {
        return this->GetMonoSource(0, format);
    }

    /// Alokuje i rejestruje w zarządcy jedyne swoje pod-źródło.
    void all_subseries_required()
    {
        //multi_filter_source_base<DATA_SOURCE>::all_subseries_required(); - pure virtual!
        ApproximatedClusterSize();
    }
    /// @}

// Methods required for a basic interface of data_source
//------------------------------------------------------
    /// @name Metody wymagające leniwej kalkulacji.
    /// @{

    /// Rozmiar danych tego źródła to długość tablicy "arra".
    size_t get_size()
    {
        this->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        return arra.get_size();
    }

    /// Początek iteracji po tablicy "arra".
    data_source_base::iterator_h reset()
    {
        this->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        return (data_source_base::iterator_h) 1;
    }

    /// Koniec iteracji po tablicy "arra".
    void close(data_source_base::iterator_h &p)
    {
        p = NULL;
    }

    /// Ile elementów w tablicy "arra" i jakie minimum oraz maksimum.
    void bounds(size_t &num, double &min, double &max)
    {
        this->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        num = get_size();
        min = this->y_min;
        max = this->y_max;
    }

    /// Daje następną z N liczb iteracji po tablicy "arra".
    double get(data_source_base::iterator_h &ptr_to_iterator)
    {
        assert(ptr_to_iterator != NULL);
        return arra[_next(ptr_to_iterator)];
    }

    /// Przekształca index do tablicy "arra" w konkretną wartość.
    double get(size_t index)
    {
        this->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        assert(index < get_size());
        return arra[index];
    }
    /// @}

};

// IMPLEMENTACJA OBLICZEŃ:
//========================

template<class DATA_SOURCE>
int spatial_correlation_source<DATA_SOURCE>::_calculate()
{
    if(!multi_filter_source_base<DATA_SOURCE>::_calculate())
        return 0;

    //OBLICZANIE KORELACJI DLA RÓŻNYCH ODLEGŁOŚCI
    assert(N == -1);  //Tylko tryb "integer-owy" - tyle klas ile liczb całkowitych

    // Ustalenie, czy mieści się w maksymalnej odległości:
    sym2::geometry *MyGeom = this->Source->get_geometry(); ///< Wskaźnik do geometrii

    //Musi być dostępna realna i co najmniej jednowymiarowa geometria symulacji, inaczej dupa blada.
    if(MyGeom != NULL && MyGeom->get_dimension() > 0)
    {
        //PRZYGOTOWANIE
        size_t NN = size_t(ceil(MyGeom->get_max_distance())); //TODO A dlaczego `size_t`???
        assert(ceil(MyGeom->get_max_distance()) <= UINT_MAX);

        if(arra.get_size() != NN)
        {
            arra.alloc(NN);
            arra.fill(float(1.0));
            the_same.alloc(NN);
            the_same.fill(0);
            not_the_same.alloc(NN);
            not_the_same.fill(0);
        }
        else
        {
            arra.fill(float(1.0));
            the_same.fill(0);
            not_the_same.fill(0);
        }

        assert(arra.get_size() == the_same.get_size());

        //ZLICZANIE SKŁADOWYCH
        if(rand_mult <= 0)
        {
            if(!_count_all()) goto ERROR;
        }
        else
        {
            if(!_count_randomly()) goto ERROR;
        }

        //OBLICZANIE FUNKCJI KORELACJI I SZUKANIE PIERWSZEGO PIERWIASTKA FUNKCJI KORELACJI
        /// Poszukiwana wartość rozmiaru klastra.
        /// Przy inicjalizacji ignorujemy raczej mało prawdopodobną utratę precyzji: `NN` musiałoby być ogromne.
        double pierwiastek = double(NN);

        this->y_min = DBL_MAX;
        this->y_max = -DBL_MAX;
        for(unsigned int i = 0; i < NN; i++)
        {
            //double pom=arra[i]=the_same[i]+not_the_same[i]; //"DEBUGGING" ROZKŁADU TRAFIEŃ

            if(the_same[i] + not_the_same[i] > 0)
            {
                double pom = (double(the_same[i]) - not_the_same[i]);
                pom /= double(the_same[i] + not_the_same[i]); //Prosta korelacja taki-sam versus inny

                if(pierwiastek == double(NN) && pom <= 0) // `NN` musiałoby być ogromne!
                    pierwiastek = i; //Pierwsza odległość z korelacją poniżej 0.

                if(pom < this->y_min)
                    this->y_min = pom;

                if(pom > this->y_max)
                    this->y_max = pom;

                arra[i] = pom;
            } else
            {
                arra[i] = this->get_missing();
            }

        }

        //if()
        this->y_min = -1;
        this->y_max = 1; //Jeśli nie interesuje nas maksimum i minimum, to...

        //ZAPAMIĘTANIE WYNIKU
        if(this->table[0] != NULL)
        {
            this->table[0]->change_val(pierwiastek);
        }

        return 1;
    } //Musial przeliczyć

    ERROR:
    if(this->table[0] != NULL)
        this->table[0]->change_val(this->table[0]->get_missing());

    arra.dispose();
    the_same.dispose();
    not_the_same.dispose();

    this->y_min = this->y_max = 0;
    return 1;
}

template<class DATA_SOURCE>
bool spatial_correlation_source<DATA_SOURCE>::_count_all()
{
    size_t NN = arra.get_size();
    size_t HowManyCells=0;
    //size_t zliczaj = 0; ///< Kontrola liczby sprawdzonych par.

    double Min, Max; //Parametry serii źródłowej
    this->Source->bounds(HowManyCells, Min, Max);
    if(Min == Max || HowManyCells < 2)
        return false; //W każdym razie nie ma czego liczyć

    data_source_base::geometry *MyGeom = this->Source->get_geometry(); //Wskaźnik do geometrii
    for(unsigned int i = 0; i < HowManyCells; i++) //Dla każdej z komórek
    {
        double CenterVal = this->Source->get(i);         //Uzyskujemy wartość dla centralnego
        if(this->Source->is_missing(CenterVal))          //Sprawdzamy, czynie jest miss.
            continue;                                    //bo wtedy robić dalej byłoby bez sensu.

        for(unsigned int j = i + 1; j < HowManyCells; j++)
        {
            assert(i != j);
            double neighbor_val = this->Source->get(j);      //Uzyskujemy wartość dla sąsiada
            if(this->Source->is_missing(neighbor_val))       //Sprawdzamy, czynie jest miss.
                continue;                                //bo wtedy robić dalej byłoby bez sensu.

            double dist = MyGeom->get_distance(i, j);
            size_t d = size_t(dist);             assert(d < NN); //Index kategorii odległości — w sposób uproszczony

            if(CenterVal == neighbor_val)
                the_same[d]++;
            else
                not_the_same[d]++;

            //zliczaj++; //Kontrola liczby sprawdzonych par.
        }
    }

    //assert(zliczaj==(HowManyCells*HowManyCells-HowManyCells)/2);

    return true;
}

template<class DATA_SOURCE>
bool spatial_correlation_source<DATA_SOURCE>::_count_randomly()
{
    size_t NN = arra.get_size();
    size_t HowManyDrawings;
    double Min, Max; //Parametry serii źródłowej
    this->Source->bounds(HowManyDrawings, Min, Max);
    if(Min == Max || HowManyDrawings < 2)
        return false; //W każdym razie nie ma czego liczyć

    if(HowManyDrawings * rand_mult > ((HowManyDrawings * HowManyDrawings) - HowManyDrawings) / 2 *
                                     2)	//2 za wzór na liczbę odległości i 2 za większy koszt algorytmu tej procedury.
    {
        cerr << "SPATIAL CORRELATION WARNING: A random mode with the multiplier "
             << rand_mult << " is more expensive than full mode. Mode changed.\n";
        rand_mult = 0; //Jeśli liczba losowań byłaby większa niż liczba odległości, to nie ma sensu ta metoda.
        HowManyDrawings = (HowManyDrawings * HowManyDrawings) / 4; //Niezłe przybliżenie na ten pierwszy raz.
    } else
    {
        HowManyDrawings *= rand_mult; //Co najmniej tyle losowań co obiektów w serii, ale można też powielić liczbę losowań pare razy.
    }

    data_source_base::geometry *MyGeom = this->Source->get_geometry(); //Wskaźnik do geometrii
    data_source_base::iterator_h RndIter = MyGeom->make_random_global_iterator(HowManyDrawings); //Alokujemy iterator
    while(RndIter)
    {
        size_t index = MyGeom->get_next(RndIter); ///< Uzyskujemy index agenta.
        assert(index != MyGeom->FULL); //Tutaj nie powinno się zdarzyć coś takiego.
        double CenterVal = this->Source->get(index); ///< Uzyskujemy referencje do agenta.
        if(this->Source->is_missing(CenterVal))    // Sprawdzamy, czynie jest miss.
            continue;                              // bo wtedy robić dalej byłoby bez sensu.

        // Alokujemy iterator sąsiedztwa — o różnym rozmiarze, żeby wyrównać prawdopodobieństwa
        // poszczególnych odległości — ale to nietrywialne.
        // ------------------------------------------------
        //size_t radius=size_t(   (1-TheRandG.DRand()*TheRandG.DRand()*TheRandG.DRand() ) * NN   );       //Rozkład trafień wychodzi jednomodalny z maksimum w środku.
        //size_t radius=size_t(TheRandG.DRand()*NN/2)+((1-TheRandG.DRand()*TheRandG.DRand())*NN/2);       //Tu podobnie...
        //size_t radius=size_t(TheRandG.Random(NN);                                                       //A tu niemal liniowy spadek trafień wraz z odległością.
        //size_t radius=NN;                                                                               //Rozkład staje się jednomodalny z 0 przy odległości 1.
        size_t radius = (TheRandG.DRand() > 0.5?size_t(TheRandG.Random(NN))
                                               :NN);    //Troche lepiej, ale nie idealnie — mało dla odległości największych.

        data_source_base::iterator_h Neigh = MyGeom->make_random_neighbour_iterator(index, radius,
                                                                                    1);  //Po ilus (1,2,?) sąsiadów każdego wylosowanego
        auto LOC_FULL=MyGeom->FULL;  ///< Łatwiej dostać się do `FULL` z geometrii niż statycznie.
        while(Neigh)
        {
            size_t index2 = MyGeom->get_next(Neigh);         ///< Uzyskujemy index sąsiada

            if(index2 == LOC_FULL || index2 == index)        //Jeśli poza obszarem symulacji lub w
                continue;                                    //centrum obszaru to dalej byłoby bez sensu.

            double neighbor_val = this->Source->get(index2); ///< Uzyskujemy referencję do sąsiada
            if(this->Source->is_missing(neighbor_val))       //Sprawdzamy, czynie jest miss.
                continue;                                    //bo wtedy robić dalej byłoby bez sensu.

            double dist = MyGeom->get_distance(index, index2);
            size_t i = size_t(dist);            assert(i < NN); //Index kategorii odległości — w sposób uproszczony.

            if(CenterVal == neighbor_val)
                the_same[i]++;
            else
                not_the_same[i]++;

        }
        MyGeom->destroy_iterator(Neigh); // upewniamy się ze iterator zostanie usunięty.
    }

    MyGeom->destroy_iterator(RndIter);  // upewniamy się ze iterator zostanie usunięty.
    return true;
}

typedef spatial_correlation_source<data_source_base> generic_spatial_correlation_source;

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
#endif //SYMSHELL2_SPATIAL_CORRELATION_SOUR_HPP_INCLUDED_
