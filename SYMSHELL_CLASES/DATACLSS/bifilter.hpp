/// @file
/// @brief Base class of a two-source dependent filter/
///        Klasa bazowa filtra zależnego od dwu źródeł.
/// @date 2026-05-06 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_bi_filter_hpp_INCLUDED_
#define SYMSHELL2_bi_filter_hpp_INCLUDED_

#include "datasour.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

/// Baza źródła danych filtrującego dane łączone z dwóch innych źródeł.
class bi_filter_source_base : public filter_source_base
//-----------------------------------------------------------------
{
protected:
    data_source_base *source2; //!< Drugie źródło danych (pierwsze jest dziedziczone).
    double       source2_miss; //!< Wartość dla braku danych w drugim źródle.

    int check_version_() override;

    /// Wewnętrzna struktura dla indeksowania.
    struct indexes
    {
        iteratorh first;
        iteratorh second;

        indexes(iteratorh i_first, iteratorh i_second) :
                first(i_first), second(i_second)
        {}
    };

    /// Constructor.
    /// \param ini1 - wskaźnik do pierwszego źródła danych.
    /// \param ini2 - wskaźnik do drugiego źródła danych.
    /// \param format - sposób utworzenia nazwy filtra z nazw źródeł.
    explicit bi_filter_source_base(data_source_base *ini1 = NULL,
                                   data_source_base *ini2 = NULL,
                                   const char *format = "F(%s,%s)")
    : filter_source_base(ini1, format), source2(NULL), source2_miss(INFINITY)
    {
        set_second_source(ini2);
    }

public:
    /// Przestawia drugie źródło.
    void set_second_source(data_source_base *ini);

    /// Zwraca wskaźnik do drugiej seri źródłowej.
    data_source_base *get_second_source();

    ///  Przestawia pierwsze źródło.
    void set_first_source(data_source_base *ini);

    /// Zwraca wskaźnik do pierwszej seri źródłowej.
    data_source_base *get_first_source();

// Metody reimplementowane:
//=========================

    /// Tworzenie nazwy na podstawie formatu i naz obu źródeł danych wejściowych.
    const char *name() override;

    /// Powinien zwracać wskaźnik do obowiązującej geometrii danych.
    /// @returns NULL w tej implementacji, bo domyślnie nie można ustalić geometrii.
    /// Skoro możliwe są dwie alternatywne geometrie, to która jest prawdziwa?
    /// Jednak mogą być tożsame albo klasy potomne mogą zadecydować, jaką wezmą.
    geometry_base *get_geometry() override;

    /// N jest brane mniejsze, ale minimum i maksimum zależą od obu źródeł.
    /// Taka prosta implementacja, jednak w klasach potomnych może być nietrywialnie.
    /// Za implementacje tej decyzji odpowiada wirtualna funkcja wewnętrzna `_bounds`
    void bounds(size_t &N, double &min, double &max) override;

    /// METODA OTWIERAJĄCA DOSTĘP DO DANYCH.
    /// @returns wskaźnik do ZAALOKOWANEGO iteratora typu `indexes` (aktualnie tak, ale...)
    /// @note Domyślnie zaimplementowana złożona iteracja wykonuje się do końca krótszego ze źródeł.
    iteratorh reset() override
    {
        check_version_();  //Żeby źródła miało szanse na update

        //Tworzenie złożonego indeksu:
        indexes *pom = new indexes(Source->reset(), source2->reset());

        return pom;
    }

    /// Daje kolejną z N liczb!!! Po N-tej obiekt źródłowy zwalnia iterator!
    /// Ostateczny wynik opiera się na wywołaniu wewnętrznej wirtualnej metodzie `_get`.
    /// @note Domyślnie zaimplementowana złożona iteracja wykonuje się do końca krótszego ze źródeł.
    double get(iteratorh &I) override
    {
        indexes *pom = (indexes *) I;
        double val1 = Source->get(pom->first);
        double val2 = source2->get(pom->second);
        if(pom->first == NULL || pom->second == NULL)	//Czy któreś źródło się wyczerpało?
        {
            Source->close(pom->first);
            source2->close(pom->second);
            delete pom;
            I = NULL; //Informacja zwrotna o braku danych.
        }

        return _get(val1, val2);
    }

    /// Przetwarza indeks uzyskany z geometrii na wartość z serii.
    /// Znowu używamy wewnętrznego `_get` do interpretacji pary wartości.
    double get(geometry::index_t index_from_geometry) override
    {
        double val1 = Source->get(index_from_geometry);
        double val2 = source2->get(index_from_geometry);
        return _get(val1, val2);
    }

    /// Metoda zwalnia iteratory wewnętrzne i cały iterator.
    /// Jeśli nie został zwolniony przez `get`.
    /// Podwójne zwolnienie jest bezpieczne, bo zwolniony już `iteratorh` zawiera NULL.
    void close(iteratorh &I) override
    {
        indexes *pom = (indexes *) I;
        if(pom == NULL) return;
        Source->close(pom->first);
        source2->close(pom->second);
        delete pom;
        I = NULL; //Informacja zwrotna
    }

protected:
    /// WYMAGANA implementacja w klasach potomnych konkretnej decyzji o wartości minimalnej i maksymalnej.
    virtual void _bounds(double &min1, double &max1,
                         double &min2, double &max2,
                         double &min, double &max) = 0;

    /// WYMAGANA implementacja konkretnej operacji logiczno-matematycznej w klasie potomnej.
    /// Musi też sprawdzać, czy któraś z wartości lub obie nie są "missing"!
    virtual double _get(double val1, double val2) = 0;

    /// @name Uzupełnienie o sprawdzanie, czy z którejś z serii przyszło missing.
    /// TODO Czy one muszą być wirtualne?
    /// @{
    virtual int from_first_is_missing(double val)
    { return (val == source_miss? 1:0); }

    virtual int from_second_is_missing(double val)
    { return(val == source2_miss? 1:0); }
    /// @}
};

// INLINE IMPLEMENTATIONS:
//========================

inline
int bi_filter_source_base::check_version_()
//Sprawdza, czy i jak zmieniły się dane w źródłach.
//W filtrach cache-ujących może powodować opróżnienie lub ponowne napełnienie.
//Zwraca 1, jeśli zmieniło wersje, a 0, jeśli wersje się zgadzają.
{
    int update2 = update_version_from(source2); //Tu ewentualnie może zmienić wersje.
    int update1 = update_version_from(Source); //Tylko żeby dać źródłu szanse aktualizacji.
    assert(update1 == 0); //Nie powinno już zmienić wersji

    //Aktualizacja "missing values"
    source_miss = Source->get_missing();
    source2_miss = source2->get_missing();
    return update2;
}

inline
const char *bi_filter_source_base::name()
//Musi zwracać nazwę serii albo "" - NIE NULL!!!
{
    const char *pom1 = Source->name();
    const char *pom2 = source2->name();
    if(
            !_name.IsOK() ||
            strstr(_name.get_ptr_val(), pom1) == NULL ||
            strstr(_name.get_ptr_val(), pom2) == NULL
            )    //jeśli jeszcze nie ma albo zmieniło się którymś obiekcie źródła
    {
        _name.alloc(strlen(title_util::name()) + strlen(pom1) + strlen(pom2) + 1);
        sprintf(_name.get_ptr_val(), title_util::name(), pom1, pom2);
    }
    return _name.get_ptr_val();
}

inline
void bi_filter_source_base::set_second_source(data_source_base *ini)
{
    source2 = ini;
    source2_miss = source2->get_missing();
}

inline
data_source_base *bi_filter_source_base::get_second_source()
{
    source2_miss = source2->get_missing();
    return source2;
}

inline
void bi_filter_source_base::set_first_source(data_source_base *ini)
{
    Source = ini;
    source_miss = Source->get_missing();
}

inline
data_source_base *bi_filter_source_base::get_first_source()
{
    source_miss = Source->get_missing();
    return Source;
}

inline
geometry_base *bi_filter_source_base::get_geometry()
{
    return NULL;
}

inline
void bi_filter_source_base::bounds(size_t &N, double &min, double &max)
//Ile elementów oraz wartość minimalna i maksymalna.
{
    check_version_();

    size_t N1 = 0, N2 = 0;
    double min1 = 0, min2 = 0, max1 = 0, max2 = 0;

    Source->bounds(N1, min1, max1);
    source2->bounds(N2, min2, max2);

    N = (N1 < N2?N1:N2);

    _bounds(min1, max1, min2, max2, min, max);
}


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
#endif //SYMSHELL2_bi_filter_hpp_INCLUDED_




