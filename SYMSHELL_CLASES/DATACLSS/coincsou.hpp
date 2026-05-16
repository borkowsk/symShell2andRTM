/// @file
/// @brief **A filter that counts the coincidences of classes of two series and derived statistics (Hi^2 etc.).** /<br>
///         _Filtr liczący koincydencje klas dwu serii i pochodne statystyki (Hi^2 itp.)._
/// @date 2026-05-16 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_COINCIDENCE_SOUR_HPP_INCLUDED_
#define SYMSHELL2_COINCIDENCE_SOUR_HPP_INCLUDED_

#include "costatso.hpp"
#include <limits>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

#ifndef SIZE_MAX
#define SIZE_MAX (std::numeric_limits<size_t>::max())
#endif

/// Filtr liczący koincydencje klas dwu serii i pochodne statystyki (Hi^2 itp.).
class coincidence_source : public co_statistics_source
//------------------------------------------------------------------------------
{
public:
    typedef sym2::rectangle_geometry my_geometry_t;

protected:
    size_t N; ///< Required number of Class of First
    size_t M; ///< Required number of Class of Second
    wb_ptr<my_geometry_t> my_geometry; ///< Geometria macierzy koincydencji (prostokątna).
    wb_dynmatrix<unsigned long>  arra; ///< Macierz koincydencji (aż 64-bitowe liczniki to chyba trochę za dużo?)
    //wb_dynmatrix<unsigned>  arra; ///< Macierz koincydencji. Ale tak wywoływała pad przy alokacji... DZIWNE.
    unsigned iHi; ///< Indeks dla pozycji Hi-kwadrat (`size_t`, czyli u64 to chyba trochę za dużo)?

    /// Bezpośrednio siega do tablicy `arra`.
    double _get(size_t index)
    {
        assert(arra.IsOK());
        size_t NNHeight = my_geometry->get_width();
        size_t i = index / NNHeight; //Który wiersz?
        assert(arra[i].IsOK());
        size_t j = index % NNHeight; //Która kolumna?
        return arra[i][j]; //Musi przekonwertować na `double`, bo tak wygląda zapotrzebowanie. Jednak ryzyko przekłamań minimalne.
    }

    /// Leniwe obliczanie. Zwraca 1, jeśli musiał przeliczyć.
    /// DUŻO LICZENIA W TEJ METODZIE.
    int _calculate() override;

public:
    /// Ma sporo pod-źródeł. Te odziedziczone i 8 własnych.
    size_t number_of_subseries() override
    {
        return co_statistics_source::number_of_subseries() +
               8; //Ma osiem pod-źródeł
    }

    /// Alokuje i ewentualnie rejestruje w zarządcy wszystkie serie.
    void all_subseries_required() override
    {
        co_statistics_source::all_subseries_required();
        Chi2(); //HiKwadrat
        LevelOfFreedom(); //Liczba stopni swobody układu
        T2Czupurow();     //T Czupurowa
        V2Cramer();       //V^2 Cramera — miernik zależności dla zredukowanej (min) liczby stopni swobody.
        Entropy();
        NormEntropy();
        Tau_b_Goodman_Kruskal(); //Tau_b zależność B od A wg Goodmana i Kruskala
        Tau_a_Goodman_Kruskal(); //Tau_a zależność A od B wg Goodmana i Kruskala
    }

    /// @name Access to "children" scalar sub-sources.
    /// @{
    scalar_source<double> *Chi2(const char *format = "Chi^2(%s,%s)")
    {
        iHi = co_statistics_source::number_of_subseries();
        return GetMonoSource(iHi, format);
    }

    scalar_source<double> *T2Czupurow(const char *format = "T^2(%s,%s)")
    {
        if(iHi == -1)
            iHi = co_statistics_source::number_of_subseries();
        return GetMonoSource(iHi + 1, format);
    }

    scalar_source<double> *V2Cramer(const char *format = "V^2(%s,%s)")
    {
        if(iHi == -1)
            iHi = co_statistics_source::number_of_subseries();
        return GetMonoSource(iHi + 2, format);
    }

    scalar_source<double> *LevelOfFreedom(const char *format = "L.of.f(%s,%s)")	//Ilość stopni swobody układu
    {
        if(iHi == -1)
            iHi = co_statistics_source::number_of_subseries();
        return GetMonoSource(iHi + 3, format);
    }

    scalar_source<double> *Entropy(const char *format = "S(%s,%s)")
    {
        if(iHi == -1)
            iHi = co_statistics_source::number_of_subseries();
        return GetMonoSource(iHi + 4, format);
    }

    scalar_source<double> *NormEntropy(const char *format = "nS(%s,%s)")
    {
        if(iHi == -1)
            iHi = co_statistics_source::number_of_subseries();
        return GetMonoSource(iHi + 5, format);
    }

    scalar_source<double> *Tau_b_Goodman_Kruskal(const char *format = "Tau_b(%s,%s)")
    {
        if(iHi == -1)
            iHi = co_statistics_source::number_of_subseries();
        return GetMonoSource(iHi + 6, format);
    }

    scalar_source<double> *Tau_a_Goodman_Kruskal(const char *format = "Tau_a(%s,%s)")
    {
        if(iHi == -1)
            iHi = co_statistics_source::number_of_subseries();
        return GetMonoSource(iHi + 7, format);
    }
    /// @}

    /// Construction.
    /// \param ini1 pierwsze źródło danych.
    /// \param ini2 drugie źródło danych.
    /// \param NumberOfClass1 arbitralna liczba klas dla `ini1` albo -1 oznaczające klasy całkowitoliczbowe.
    /// \param NumberOfClass2 arbitralna liczba klas dla `ini2` albo -1 oznaczające klasy całkowitoliczbowe.
    /// \param data_manager zarządca danych, do którego trafią skalarne pod-źródła wynikowe.
    /// \param table_size zapas na liczbę pod-źródeł klas potomnych.
    /// \param format sposób tworzenia nazwy filtru z nazw źródeł danych.
    explicit coincidence_source( data_source_base *ini1 = NULL,
                                 data_source_base *ini2 = NULL,
                                 size_t NumberOfClass1 = -1,		//-1 oznacza tryb całkowitoliczbowy
                                 size_t NumberOfClass2 = -1,		//-1 oznacza tryb całkowitoliczbowy
                                 sources_manager_base *data_manager = NULL,
                                 size_t table_size = 1/*ZAPAS*/,
                                 const char *format = "COINCIDENT(%s,%s)")
    : N(NumberOfClass1),M(NumberOfClass2),iHi(-1),
      co_statistics_source(ini1, ini2,
                           data_manager,
                           8 + table_size,		//4 własne + z klas potomnych
                           format)
    {}

    /// Destruktor jest niby pusty.
    ~coincidence_source() override = default;

// Accession Methods:
//===================

    /// Ile elementów w tablicy koincydencji, wartość minimalna i maksymalna.
    void bounds(size_t &num, double &min, double &max) override
    {
        check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        num = get_size();
        min = y_min;
        max = y_max;
    }

    /// Ile elementów w tablicy koincydencji.
    size_t get_size()
    {
        check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        if(!my_geometry)
            return 0;
        return my_geometry->get_size(); //Prawdziwy rozmiar tablicy koincydencji
    }

    /// Zwraca wskaźnik do obowiązującej geometrii danych.
    /// W tym przypadku do geometrii tablicy koincydencji.
    geometry_base *get_geometry() override
    {
        check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        return my_geometry.get_ptr_val();
    }

    /// Umożliwia czytanie od początku tablicy koincydencji.
    iterator_h reset() override
    {
        check_version_(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
        if(my_geometry)
            return my_geometry->make_global_iterator();
        else
            return NULL;
    }

    /// Wymuszony koniec iteracji.
    void close(iterator_h &p) override
    {
        if(my_geometry)
            my_geometry->destroy_iterator(p);
    }

    /// Daje następną z liczb tablicy koincydencji.
    double get(iterator_h &p) override
    {
        double ret = 0;
        assert(p != NULL);
        size_t pom = my_geometry->get_next(p);

        if(pom != ULONG_MAX)
            ret = _get(pom);
        else
            ret = miss;

        return ret;
    }

    /// Przetwarza `index` uzyskany z geometrii na jedną z liczb tablicy koincydencji.
    double get(size_t index) override
    {
#ifdef CAREFULLY_GET //Raczej niepotrzebne, bo robi to już i get_geometry() i bounds() i get_size();
        check_version(); //Uaktualnia też wersje pod-źródła, jeśli trzeba
        _calculate(); //Sprawdza, czynie trzeba policzyć i ewentualnie liczy
#endif
        assert(index < get_size());
        return _get(index);
    }

};

// Implementacja największej funkcji:
//====================================

inline //Oczywiście naprawdę to nie będzie inline, ale dzięki temu nie musimy tworzyć pliku cpp.
int coincidence_source::_calculate()
{
    if(!co_statistics_source::_calculate())
        return 0;

    double Hi = 0;      ///< Suma składowych Chi^2
    double Entropy = 0; ///< Suma składowych entropii
    double Za = 0;      ///< Ilość błędów dla Tau_b
    double Zb = 0;      ///< Ilość błędów dla Tau_a
    size_t NNHeight = 0, MMWidth = 0, columns = 0, rows = 0, Pairs = 0;

    wb_dynarray<unsigned long> SummColumns;
    wb_dynarray<unsigned long> SummRows;

    {//OBLICZANIE HISTOGRAMU 2D KOINCYDENCJI ZDARZEŃ
        assert(N == -1); //Tylko tryb integer-owy jest zaimplementowany.
        assert(M == -1);
        data_source_base *source1 = get_first_source();
        data_source_base *source2 = get_second_source();
        double min1, max1, min2, max2;
        size_t N1, N2; ///< Numbers of pairs, items and real numbers of classes
        size_t i, j;

        source1->bounds(N1, min1, max1);
        source2->bounds(N2, min2, max2);

        if(N1 <= 0 || N2 <= 0) goto ERROR;		//Nie da się dalej liczyć

        if(max1 - min1 <= double(size_t(-1)))	//Czy w zakresie size_t
            NNHeight = size_t(max1 - min1) + 1;	//Ile jednostek całkowitych zakresu
        else
            goto ERROR;
        if(max2 - min2 <= double(size_t(-1)))	//Czy w zakresie size_t
            MMWidth = size_t(max2 - min2) + 1;	//Ile jednostek całkowitych zakresu
        else
            goto ERROR;

        //Alokacja tablicy, jeśli potrzeba
        if(!my_geometry ||
           my_geometry->get_height() != NNHeight ||
           my_geometry->get_width() != MMWidth)
        {
            if(arra.alloc(NNHeight, MMWidth) == 0)	//błąd alokacji — za malo/za dużo?
                goto ERROR;

            my_geometry = new my_geometry_t(MMWidth, NNHeight, 0);
        }

        for(i = 0; i < NNHeight; i++)
        {
            assert(arra[i].get_size() == MMWidth);
            memset(arra[i].get_ptr_val(), 0,		//Wypełnia za pomocą 0
                   sizeof(unsigned long) * arra[i].get_size());
        }


        //PĘTLA ZLICZANIA:
        SummColumns.alloc(NNHeight);
        SummRows.alloc(MMWidth);

        memset(SummColumns.get_ptr_val(), 0,		//Wypełnia za pomocą 0
               sizeof(unsigned long) * NNHeight);

        memset(SummRows.get_ptr_val(), 0,			//Wypełnia za pomocą 0
               sizeof(unsigned long) * MMWidth);

        iterator_h Ind1 = source1->reset();
        iterator_h Ind2 = source2->reset();
        while(Ind1 && Ind2) //Puki oba iteratory...
        {
            double pom1 = source1->get(Ind1);
            double pom2 = source2->get(Ind2);
            if((!from_first_is_missing(pom1)) &&
               (!from_second_is_missing(pom2))
                    )
            {
                Pairs++;   //Suma zdarzeń!!!
                // Wyjście poza SIZE_MAX bardzo mało prawdopodobne.
                pom1 -= min1; /*NN*/ //assert(pom1 < SIZE_MAX);
                pom2 -= min2; /*MM*/ //assert(pom2 < SIZE_MAX);

                SummColumns[size_t(pom1)]++;		//To niespecjalnie czysta metoda. WARNING!!!
                SummRows[size_t(pom2)]++;			//To niespecjalnie czysta metoda. WARNING!!!

                arra[size_t(pom1)][size_t(pom2)]++;	//To niespecjalnie czysta metoda. WARNING!!!
            }
        }
        source1->close(Ind1);
        source2->close(Ind2);

        //PĘTLA	MIN/MAX, Hi^2
        if(Pairs == 0) //Nie ma współwystępowania np. z powodu missing values
            goto ERROR;

        y_min = DBL_MAX;
        y_max = 0;
        //size_t count = 0, min_p = 0, max_p = 0;

        for(i = 0; i < NNHeight; i++)
            if(SummColumns[i] != 0)
                rows++;

        for(j = 0; j < MMWidth; j++)
            if(SummRows[j] != 0)
                columns++;

        if(rows <= 1 || columns <= 1) //jeśli któraś ze zmiennych ma tylko jedną klasę to nie da się ustalić związku
            goto ERROR;

        for(i = 0; i < NNHeight; i++)
            for(j = 0; j < MMWidth; j++)
            {
                double pom = arra[i][j]; //Aktualna wartość

                //if(pom>0) count++;

                if(pom > y_max)
                {
                    y_max = pom;
                    //max_p = i;
                }

                if(pom < y_min)
                {
                    y_min = pom;
                    //min_p = i;
                }

                //SKŁADOWE Chi^2
                //Liczebność oczekiwana
                double fe = ( double(SummColumns[i]) * double(SummRows[j]) ) / double(Pairs);

                //Powiększenie sumy, gdy nie jest to puste skrzyżowanie
                if(fe > 0)
                    Hi += sqr(pom - fe) / fe;

                //SKŁADOWE ENTROPI
                double qij = pom / double(Pairs);
                //Powiększenie sumy, gdy nie jest to puste skrzyżowanie
                if(qij > 0)
                    Entropy += qij * log(qij);

                //SKŁADOWE Tau_b — liczba błędów przy znajomości cechy A
                double SumaWiersza = double( SummColumns[i] ); //Dla gigantycznych zliczeń może być przybliżone!
                if(SumaWiersza > 0) //W niepustym wierszu
                    Za += pom * ((SumaWiersza - pom) / SumaWiersza);

                //SKŁADOWE Tau_a — liczba błędów przy znajomości cechy B
                double SumaKolumn = double( SummRows[j] ); //Dla gigantycznych zliczeń może być przybliżone!
                if(SumaKolumn > 0) //W niepustym wierszu
                    Zb += pom * ((SumaKolumn - pom) / SumaKolumn);
            }

    }//Koniec obliczeń składowych

    if(table[iHi] != NULL)
        table[iHi]->change_val(Hi);

    if(table[iHi + 1] != NULL)
        table[iHi + 1]->change_val(Hi / (double(Pairs) * sqrt(double((rows - 1) * (columns - 1)))));

    if(table[iHi + 2] != NULL)
        table[iHi + 2]->change_val(Hi / (double(Pairs) * min(double(rows - 1),double(columns - 1))));

    if(table[iHi + 3] != NULL)	//Level of freedom
        table[iHi + 3]->change_val(double(rows - 1) * double(columns - 1));

    if(table[iHi + 4] != NULL)	//Entropy
    {
        Entropy = -Entropy; //Z sumowania wychodzi ujemna, bo ln z ułamków
        table[iHi + 4]->change_val(Entropy);
    }

    if(table[iHi + 5] != NULL)	//"Normalised Entropy"
    {
        if(Entropy < 0)
            Entropy = -Entropy; //Z sumowania wychodzi ujemna, bo ln z ułamków
        /*
        double q=1/double(rows*columns); //Ilość możliwych słupków sprawiedliwie obdzielona
        assert(Pairs>0 && q>0);
        double MaxS=-(double(rows*columns)*(q*log(q)));
        */
        table[iHi + 5]->change_val(
                Entropy / log( rows * columns )  ); //Dzielone przez maksymalna możliwa dla tej liczby słupków
    }

    if(table[iHi + 6] != NULL)
    {
        double Na = 0;
        for(unsigned j = 0; j < MMWidth; j++)
            if(SummRows[j] != 0)
            {
                auto pom = SummRows[j]; // Przy wielkich licznikach może być bardzo przybliżone.
                Na += double(pom) * ( double(Pairs - pom) / double( Pairs) );
            }
        assert(Na > 0);
        table[iHi + 6]->change_val((Na - Za) / Na);
    }

    if(table[iHi + 7] != NULL)
    {
        double Nb = 0;
        for(unsigned i = 0; i < NNHeight; i++)
            if(SummColumns[i] != 0)
            {
                auto pom = SummColumns[i];
                Nb += double(pom) * ( double(Pairs - pom) / double(Pairs) );
            }
        assert(Nb > 0);
        table[iHi + 7]->change_val((Nb - Zb) / Nb);
    }

    return 1;
    ERROR:
    if(table[iHi] != NULL)
        table[iHi]->change_val(table[iHi]->get_missing());
    if(table[iHi + 1] != NULL)
        table[iHi + 1]->change_val(table[iHi + 1]->get_missing());
    if(table[iHi + 2] != NULL)
        table[iHi + 2]->change_val(table[iHi + 2]->get_missing());
    if(table[iHi + 3] != NULL)
        table[iHi + 3]->change_val(table[iHi + 3]->get_missing());
    if(table[iHi + 4] != NULL)
        table[iHi + 4]->change_val(table[iHi + 4]->get_missing());
    if(table[iHi + 5] != NULL)
        table[iHi + 5]->change_val(table[iHi + 5]->get_missing());
    if(table[iHi + 6] != NULL)
        table[iHi + 6]->change_val(table[iHi + 7]->get_missing());
    if(table[iHi + 7] != NULL)
        table[iHi + 7]->change_val(table[iHi + 7]->get_missing());
    arra.dispose();
    my_geometry = NULL;
    y_min = y_max = 0;
    return 1;
}

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
#endif //SYMSHELL2_COINCIDENCE_SOUR_HPP_INCLUDED_

