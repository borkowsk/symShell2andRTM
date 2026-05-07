/// @file
/// @brief Filtr liczący liczebność klas serii i pochodne statystyki przestrzenne, w tym korelacje przestrzenne.
/// @date 2026-05-07 (modified)
// ********************************************************************************************************************
//
#ifndef __SPATIAL_CORRELATION_SOUR_HPP__
#define __SPATIAL_CORRELATION_SOUR_HPP__

#include <cmath>
#include "multfils.hpp"
#include "wb_rand.hpp"

namespace sym2 { namespace data {

/// @brief Filtr liczący  pochodne statystyki przestrzenne, w tym korelacje przestrzenne, ale też liczebność klas serii(?).
template<class DATA_SOURCE>
class spatial_correlation_source : public multi_filter_source_base<DATA_SOURCE>
//------------------------------------------------------------------------------
{
protected:
    size_t N;           //Number of distance class;
    int RndMult;      //How many times for random counting. If 0 the _count_all() is used instead _count_randomly()
    wb_dynarray<double> arra;   //Musi być double bo inaczej zle przechowuje "missing value"
    wb_dynarray<unsigned> zgodne;
    wb_dynarray<unsigned> niezgodne;

// Przemieszcza iterator o jednostke. Zeruje, jeśli koniec tablicy
    size_t _next(data_source_base::iteratorh &p)
    {
        assert(p != NULL); //Nie wolno wywołać dla NULL
        size_t pom = ((size_t) p) - 1;
        if(pom + 1 >= N)
            p = NULL;
        else
            p = (data_source_base::iteratorh) (pom + 2);
        return pom;
    }

    bool _count_all()
    {
        size_t NN = arra.get_size();
        size_t HowManyCells, zliczaj = 0;
        double Min, Max; //Parametry serii źródłowej
        this->Source->bounds(HowManyCells, Min, Max);
        if(Min == Max || HowManyCells < 2)
            return false; //W każdym razie nie ma czego liczyć

        data_source_base::geometry *MyGeom = this->Source->get_geometry(); //Wskaźnik do geometrii
        for(unsigned int i = 0; i < HowManyCells; i++) //Dla kazdej z komorek
        {
            double CenterVal = this->Source->get(i);         //Uzyskujemy wartość dla centralnego
            if(this->Source->is_missing(CenterVal))         //Sprawdzamy, czynie jest miss.
                continue;                         //bo wtedy robic dalej by�oby bez sensu.

            for(unsigned int j = i + 1; j < HowManyCells; j++)
            {
                assert(i != j);
                double PeryfVal = this->Source->get(j);         //Uzyskujemy wartość dla  sasiada
                if(this->Source->is_missing(PeryfVal))        //Sprawdzamy, czynie jest miss.
                    continue;                            // bo wtedy robic dalej by�oby bez sensu.

                double dist = MyGeom->get_distance(i, j);
                size_t d = size_t(dist);
                assert(d < NN); //Index kategori odleglosci — w sposob uproszczony

                if(CenterVal == PeryfVal)
                    zgodne[d]++;
                else
                    niezgodne[d]++;

                //zliczaj++; //Kontrola liczby sprawdzonych par
            }
        }

        //assert(zliczaj==(HowManyCells*HowManyCells-HowManyCells)/2);

        return true;
    }

    bool _count_randomly()
    {
        size_t NN = arra.get_size();
        size_t HowManyDrawings;
        double Min, Max; //Parametry serii zrod�owej
        this->Source->bounds(HowManyDrawings, Min, Max);
        if(Min == Max || HowManyDrawings < 2)
            return false; //W kazdym razie nie ma czego liczyc

        if(HowManyDrawings * RndMult > ((HowManyDrawings * HowManyDrawings) - HowManyDrawings) / 2 *
                                       2)	//2 za wzor na liczbe odleglosci i 2 za wiekszy koszt algorytmu tej procedury
        {
            cerr << "SPATIAL CORRELATION: Random mode with multiplier " << RndMult
                 << " is more expensive than full mode. Mode changed.\n";
            RndMult = 0; //Jeśli liczba losowan bylaby wieksza niż odleglosci to nie ma sensu ta metoda
            HowManyDrawings = (HowManyDrawings * HowManyDrawings) / 4; //Niezle przyblizenie na ten pierwszy raz
        } else
        {
            HowManyDrawings *= RndMult; //Co Najmniej tyle losowan co obiektów w serii, ale można tez powielic liczbe losowan pare razy
        }

        data_source_base::geometry *MyGeom = this->Source->get_geometry(); //Wskaźnik do geometrii
        data_source_base::iteratorh RndIter = MyGeom->make_random_global_iterator(HowManyDrawings); //Alokujemy iterator
        while(RndIter)
        {
            size_t index = MyGeom->get_next(RndIter); //Uzyskujemy index agenta
            assert(index != MyGeom->FULL); //... tutaj nie powinno się zdarzyc
            double CenterVal = this->Source->get(index); // Uzyskujemy referencje do agenta
            if(this->Source->is_missing(CenterVal))    // Sprawdzamy, czynie jest miss.
                continue;                        // bo wtedy robic dalej by�oby bez sensu.

            // Alokujemy iterator sasiedztwa — o różnym rozmiarze, żeby wyrównac prawdopodobienstwa poszczególnych odleglosci — ale to nietrywialne

            //size_t radius=size_t(   (1-TheRandG.DRand()*TheRandG.DRand()*TheRandG.DRand() ) * NN   );       //Rozklad trafien wychodzi jednomodalny z maksimum w srodku
            //size_t radius=size_t(TheRandG.DRand()*NN/2)+((1-TheRandG.DRand()*TheRandG.DRand())*NN/2);       //tu podobnie
            //size_t radius=size_t(TheRandG.Random(NN);                                                       //A tu niemal liniowy spadek trafien wraz z odlegloscia
            //size_t radius=NN;                                                                               //Jednomodalny z 0 przy odleglosci 1
            size_t radius = (TheRandG.DRand() > 0.5?size_t(TheRandG.Random(NN))
                                                   :NN);                         //Troche lepiej, ale nie idealnie — ma�o dla odleglosci najwiekszych

            data_source_base::iteratorh Neigh = MyGeom->make_random_neighbour_iterator(index, radius,
                                                                     1);  //Po ilus (1,2,?) sasiadow każdego wylosowanego
            while(Neigh)
            {
                size_t index2 = MyGeom->get_next(Neigh);  //Uzyskujemy index sasiada
                if(index2 == MyGeom->FULL || index2 == index)        //Jeśli poza obszarem symulacji lub w
                    continue;                            //centrum obszaru to dalej byłoby bez sensu.

                double PeryfVal = this->Source->get(index2);    //Uzyskujemy referencje do sasiada
                if(this->Source->is_missing(PeryfVal))        //Sprawdzamy, czynie jest miss.
                    continue;                            // bo wtedy robic dalej by�oby bez sensu.

                double dist = MyGeom->get_distance(index, index2);
                size_t i = size_t(dist);
                assert(i < NN);   //Index kategori odleglosci — w sposob uproszczony

                if(CenterVal == PeryfVal)
                    zgodne[i]++;
                else
                    niezgodne[i]++;

            }
            MyGeom->destroy_iterator(Neigh);        // upewniamy się ze iterator zostanie usuniety
        }

        MyGeom->destroy_iterator(RndIter);  // upewniamy się ze iterator zostanie usuniety
        return true;
    }

    int _calculate() //Zwraca 1, jeśli musial przeliczyc
    {
        if(!multi_filter_source_base<DATA_SOURCE>::_calculate())
            return 0;

        //OBLICZANIE KORELACJI DLA RÓŻNYCH ODLEGLOSCI
        assert(N == -1);  //Tylko tryb "integerowy" - tyle klas ile liczb calkowitych
        // miesci się w maksymalnej odleglosci
        symshell2::geometry *MyGeom = this->Source->get_geometry(); //Wskaźnik do geometrii

        if(MyGeom != NULL && MyGeom->get_dimension() > 0) //Musi być dostępna realna i co najmniej jednowymiarowa
            //geometria symulacji, inaczej dupa blada
        {
            //PRZYGOTOWANIE
            size_t NN = size_t(ceil(MyGeom->get_max_distance()));
            assert(ceil(MyGeom->get_max_distance()) <= UINT_MAX);
            double pierwiastek = NN; //Poszukiwana wartość rozmiaru klastra

            if(arra.get_size() != NN)
            {
                arra.alloc(NN);
                arra.fill(float(1.0));
                zgodne.alloc(NN);
                zgodne.fill(0);
                niezgodne.alloc(NN);
                niezgodne.fill(0);
            } else
            {
                arra.fill(float(1.0));
                zgodne.fill(0);
                niezgodne.fill(0);
            }

            assert(arra.get_size() == zgodne.get_size());

            //ZLICZANIE SKŁADOWYCH
            //if(!_count_randomly()) goto ERROR;
            if(RndMult <= 0)
            {
                if(!_count_all()) goto ERROR;
            } else
            {
                if(!_count_randomly()) goto ERROR;
            }

            //OBLICZANIE FUNKCJI KORELACJI I SZUKANIE PIERWSZEGO PIERWIASTKA FUNKCJI KORELACJI
            this->y_min = DBL_MAX;
            this->y_max = -DBL_MAX;
            for(unsigned int i = 0; i < NN; i++)
            {
                //double pom=arra[i]=zgodne[i]+niezgodne[i]; //DEBUGING ROZKLADU TRAFIEN

                if(zgodne[i] + niezgodne[i] > 0)
                {
                    double pom = (double(zgodne[i]) - niezgodne[i]);
                    pom /= double(zgodne[i] + niezgodne[i]); //Prosta korelacja taki-sam versus inny

                    if(pierwiastek == NN && pom <= 0)
                        pierwiastek = i; //Pierwsza odleglosc z korelacja ponizej 0

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
            this->y_max = 1; //Jeśli nie interesuje nas max i min

            //ZAPAMIETANIE WYNIKU
            if(this->table[0] != NULL)
            {
                this->table[0]->change_val(pierwiastek);
            }

            return 1;
        } //Musial przeliczyc

        ERROR:
        if(this->table[0] != NULL)
            this->table[0]->change_val(this->table[0]->get_missing());

        arra.dispose();
        zgodne.dispose();
        niezgodne.dispose();

        this->y_min = this->y_max = 0;
        return 1;
    }

public:
    scalar_source<double> *ApproximatedClusterSize(const char *format = "ClustSize(%s)")
    {
        return this->GetMonoSource(0, format);
    }

    spatial_correlation_source(DATA_SOURCE *ini = NULL,
                               size_t NumberOfClass = -1,		//-1 oznacza tryb calkowitoliczbowy
                               int CountMode = 2,
                               sources_menager_base *MyMenager = NULL,
                               size_t table_size = 1 /*BEZ ZAPASU*/,
                               const char *format = "SPATIAL CORRELATION(%s)") :
            RndMult(CountMode),
            N(NumberOfClass),
            multi_filter_source_base<DATA_SOURCE>(ini, MyMenager, table_size, format)
    {}

    ~spatial_correlation_source()
    {}

// Methods required for basic interface of data_source
//--------------------------------------------------------------------
    size_t get_size()
    {
        this->check_version_(); //Uaktualnia tez wersje podzrodla, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyc i ewentualnie liczy
        return arra.get_size();
    }

    void all_subseries_required()	//Alokuje i ewentualnie rejestruje w menagerze wszystkie serie
    {
        //multi_filter_source_base<DATA_SOURCE>::all_subseries_required(); - pure virtual!
        //MAX CLASS
        ApproximatedClusterSize();
    }

    data_source_base::iteratorh reset()
//Umozliwia czytanie od poczatku
    {
        this->check_version_(); //Uaktualnia tez wersje podzrodla, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyc i ewentualnie liczy
        return (data_source_base::iteratorh) 1;
    }

    void close(data_source_base::iteratorh &p)
    {
        p = NULL;
    }

    void bounds(size_t &num, double &min, double &max)
//Ile elementów,wartość minimalna i maksymalna
    {
        this->check_version_(); //Uaktualnia tez wersje podzrodla, jeśli trzeba
        _calculate();//Sprawdza, czynie trzeba policzyc i ewentualnie liczy
        num = get_size();
        min = this->y_min;
        max = this->y_max;
    }

    double get(data_source_base::iteratorh &ptr_to_iterator)
//Daje następną z N liczb!!!
    {
        assert(ptr_to_iterator != NULL);
        return arra[_next(ptr_to_iterator)];
    }

    double get(size_t index)	//Przetwarza index uzyskany z geometrii
    { //na wartość z serii, o ile jest możliwe czytanie losowe
        this->check_version_();//Uaktualnia tez wersje podzrodla, jeśli trzeba
        _calculate();//Sprawdza, czynie trzeba policzyc i ewentualnie liczy
        assert(index < get_size());
        return arra[index];
    }


};

typedef spatial_correlation_source<data_source_base> generic_spatial_correlation_source;

}} // end of namespaces sym2::data

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
