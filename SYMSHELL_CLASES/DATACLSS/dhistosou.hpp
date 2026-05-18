/// @file
/// @brief __Filter counting the number of categories in discrete data serie and derived statistics.__ /<br>
///         _Filtr liczący liczebność dyskretnych klas serii i pochodne statystyki._
/// @date 2026-05-18 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_DISCR_HISTOGRAM_SOUR_HPP_INCLUDED_
#define SYMSHELL2_DISCR_HISTOGRAM_SOUR_HPP_INCLUDED_

#include "statsour.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

/// @brief Filtr liczący liczebność dyskretnych klas serii i pochodne statystyki.
/// @note UWAGA NA DZIEDZICZENIE! N jest zmienne więc liczba serii pochodnych też.
template<class DATA_SOURCE>
class discrete_histogram_source : public basic_statistics_source<DATA_SOURCE>
//------------------------------------------------------------------------------------------------------
{
protected:
    typedef unsigned int counter_type; /// Tablica liczników. 64 bity to chyba za dużo. Ale jak się uprzesz...
    size_t Num; ///< Number of categories;
    int    Sta; ///< Lowest integer category (offset?).

    //`int Real_Lowest; //Historycznie-aktualnie najniższa wartość ze źródła.`
    //`int Real_Highest; //Historycznie-aktualnie najwyższa wartość ze źródła.`
    wb_dynarray<counter_type> arra; /// Tablica liczników.


    /// Przemieszcza iterator o jednostkę. Zeruje, jeśli koniec tablicy.
    size_t _next(data_source_base::iterator_h &p)
    {
        assert(p != NULL); //Nie wolno wywołać dla NULL
        size_t pom = ((size_t) p) - 1;

        if(pom + 1 >= Num)
            p = NULL;
        else
            p = (data_source_base::iterator_h) (pom + 2);
        return pom;
    }

    /// Leniwie oblicza liczniki histogramu itp. Zwraca 1, jeśli musiał przeliczyć.
    int _calculate();

public:
    /// @name Pod-źródła skalarne z wynikami.
    /// @{

    scalar_source<double> *MainClass(const char *format = "MainClass(%s)")
    {
        return this->GetMonoSource(6, format);
    }

    scalar_source<double> *NumOfClass(const char *format = "NumOfClass(%s)")
    {
        return this->GetMonoSource(7, format);
    }

    scalar_source<double> *WhichMain(const char *format = "WhichMain(%s)")
    {
        return this->GetMonoSource(8, format);
    }

    scalar_source<double> *Entropy(const char *format = "S(%s)")
    {
        return this->GetMonoSource(9, format);
    }

    scalar_source<double> *NormEntropy(const char *format = "nS(%s)")
    {
        return this->GetMonoSource(10, format);
    }

    scalar_source<double> *Class(size_t number, const char *format = "N%d(%s)")
    {
        char bufor[200]; //Z dużym zapasem
        int pom = int(number) + Sta;
        sprintf(bufor, format, pom, "%s");
        return this->GetMonoSource(10 + 1 + number, bufor); //+1, bo "number" może być 0!!!
    }
    /// @}

    /// Konstruktor.
    /// \param the_lowest jaka jest najniższa klasa (dyskretna), która nas interesuje.
    /// \param how_many_categories ile klas nas interesuje powyżej klasy najniższej (dyskretnych w sensie `int`).
    /// \param ini wskaźnik do serii, której histogram dyskretny chcemy otrzymać.
    /// \param format sposób tworzenia nazwy histogramu z nazwy serii źródłowej.
    /// \param my_manager wskaźnik do zarządcy danych, w którym zostaną zarejestrowane wynikowe pod-źródła skalarne.
    /// \param table_size ile pod-źródeł jest potrzebnych (z uwzględnieniem potrzeb klasy potomnej).
    discrete_histogram_source(
            int            the_lowest,   //Najniższa klasa
            size_t how_many_categories,  //Ile klas od niej
            DATA_SOURCE    *ini = NULL,  //Klasa serii źródłowej.
            //Jeśli nie pokrywa się z minX-maxX to faktycznie liczony jest wycinek
            const char *format = "DISCR.DISTRIBUTION(%s[%d..%d])",
            sources_manager_base *my_manager = NULL,
            size_t table_size = 11/*BEZ ZAPASU*/
    )
    : Num(how_many_categories), Sta(the_lowest),
      basic_statistics_source<DATA_SOURCE>( ini, my_manager,
                                            table_size + how_many_categories,		//Alokuje miejsce na pod-źródła klasowe
                                            format)
    {
        wb_pchar bufor(strlen(format) + 2 * 100); //Z za dużym zapasem jak na dwa integer-y, ale...
        bufor.prn(format, "%s", Sta, Sta + Num - 1);
        basic_statistics_source<DATA_SOURCE>::set_title(bufor.get());
        arra.alloc(Num); //Liczba klas zafiksowana
    }

    /// Destructor.
    ~discrete_histogram_source() = default;

    // Methods:
    //=========

    /// Ile koszyków histogramu.
    size_t get_size()
    {
        this->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czy nie trzeba policzyć i ewentualnie liczy
        return arra.get_size();
    }

    /// Alokuje i ewentualnie rejestruje w zarządcy wszystkie pod-źródła.
    void all_subseries_required()
    {
        basic_statistics_source<DATA_SOURCE>::all_subseries_required();
        MainClass();
        WhichMain();
        NumOfClass();
        Entropy();
        for(size_t i = 0; i < Num; i++)
            Class(i); //Alokacja źródeł liczebności klas
    }

    /// Początek iteracji po koszykach.
    data_source_base::iterator_h reset()
    {
        this->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        return (data_source_base::iterator_h) 1;
    }

    /// Koniec iteracji po koszykach.
    void close(data_source_base::iterator_h &p)
    {
        p = NULL;
    }

    /// Ile koszyków histogramu, jaki najmniejszy i jaki największy koszyk.
    void bounds(size_t &num, double &min, double &max)
    {
        this->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        num = get_size();
        min = this->y_min;
        max = this->y_max;
    }

    /// Pobranie wartości kolejnego koszyka.
    double get(data_source_base::iterator_h &ptr_to_iterator)
    //Daje następną z N liczb!!!
    {
        assert(ptr_to_iterator != NULL);
        return arra[_next(ptr_to_iterator)];
    }

    /// Pobranie wartości z konkretnego koszyka określonego przez indeks.
    /// @param index pozycja koszyka liczona od 0.
    double get(size_t index)
    {
        this->check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        assert(index < get_size());
        return arra[index];
    }


};

// IMPLEMENTACJA NAJKOSZTOWNIEJSZEJ OBLICZENIOWO FUNKCJI:
//=======================================================

template<class DATA_SOURCE>
int discrete_histogram_source<DATA_SOURCE>::_calculate()
{
    if(!basic_statistics_source<DATA_SOURCE>::_calculate())
    {
        return 0; //NIC DO ROBOTY, BO NIE BYŁO ZMIAN
    }
    else
    {   //OBLICZANIE HISTOGRAMU
        assert(Num > 1);     //Muszą być jakieś klasy
        assert(arra.IsOK()); //Musi być zaalokowana tablica

        size_t SN; ///< Ile elementów w  serii źródłowej.
        double s_min, s_max; ///< Minimum i maksimum wartości w serii źródłowej.
        this->Source->bounds(SN, s_min, s_max);

        if(s_max - s_min > double(size_t(-1)))	//Czy w zakresie size_t
            goto ERROR;

        arra.fill(0);

        //PĘTLA ZLICZANIA
        data_source_base::iterator_h Ind = this->Source->reset();
        this->source_miss = this->Source->get_missing(); //Trzeba to zrobić, żeby from_source_is_missing_ działało poprawnie!

        size_t Licz = 0, Poza = 0;
        for(size_t j = 0; j < SN; j++)
        {
            double pom = this->Source->get(Ind);
            if(!filter_source_base::from_source_is_missing_(pom))
            {
                pom = std::trunc(pom); //A własne trunc?

                if(Sta <= pom && pom < Sta + Num)
                {
                    Licz++; //Tylko te, które faktycznie weszły w histogram
                    arra[size_t(pom - Sta)]++; //Wcześniej sprawdzono, że `pom` w zakresie `size_t` i że `>= Sta`.
                }
                else
                {
                    Poza++; //Te, które nie weszły w histogram
                }
            }
        }
        this->Source->close(Ind);

        //PĘTLA	LICZENIA STATYSTYK:
        double Entropy = 0;
        counter_type licz_klasy = 0, max_p = 0;

        if(Licz > 0)	//Jest cokolwiek do liczenia
        {
            //decltype(arra[0]) min_p = 0; //najmniejsza klasa.
            this->y_min = DBL_MAX;
            this->y_max = 0;

            for(size_t i = 0; i < Num; i++)
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

                //Powiększenie sumy, gdy nie jest to puste "skrzyżowanie"(?)
                if(qi > 0)
                    Entropy += qi * log(qi);

            }
        }

        //AKTUALIZACJA AKTYWNYCH ŹRÓDEŁ STATYSTYCZNYCH
        if(this->table[6] != NULL)
        {
            this->table[6]->change_val(this->y_max);
        }

        if(this->table[7] != NULL)
        {
            this->table[7]->change_val(licz_klasy);
        }

        if(this->table[8] != NULL)
        {
            this->table[8]->change_val( double(max_p) + s_min + 0.5); //1/2, bo środek przedziału całkowitego.
        }

        if(this->table[9] != NULL)
        {
            if(Entropy != 0)	//Jeśli coś się zsumowało.
                this->table[9]->change_val(-Entropy);
            else
                this->table[9]->change_val(this->table[9]->get_missing());
        }

        if(this->table[10] != NULL)
        {
            double KL = std::trunc(s_max - s_min + 1); //Ile jednostek całkowitych zakresu realnego.
            assert(KL > 0);
            if(Entropy != 0)	//Jeśli coś się zsumowało.
                this->table[10]->change_val(-Entropy / log( KL )); //A może powinno być dla zadanego?
            else
                this->table[10]->change_val(this->table[10]->get_missing());
        }

        //Jeśli zdefiniowana liczba klas
        for(size_t k = 0; k < Num; k++)
        {
            if(this->table[11 + k] != NULL) //"i" może być 0!
            {
                this->table[11 + k]->change_val(arra[k]); //Jeden do jednego,
            }
        }

        return 1;
    } //Musial przeliczyć

    ERROR:
    if(this->table[10] != NULL)
        this->table[10]->change_val(this->table[10]->get_missing());
    if(this->table[9] != NULL)
        this->table[9]->change_val(this->table[9]->get_missing());
    if(this->table[8] != NULL)
        this->table[8]->change_val(this->table[8]->get_missing());
    if(this->table[7] != NULL)
        this->table[7]->change_val(this->table[7]->get_missing());
    if(this->table[6] != NULL)
        this->table[6]->change_val(this->table[6]->get_missing());
    arra.dispose();
    this->y_min = this->y_max = 0;
    return 1;
}

   typedef discrete_histogram_source<data_source_base> generic_discrete_histogram_source;

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

