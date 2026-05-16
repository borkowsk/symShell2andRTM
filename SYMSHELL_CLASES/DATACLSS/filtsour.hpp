/// @file
/// @brief __Different classes of single-source data filters.__ /<br>
///         _Różne klasy jednoźródłowych filtrów danych._
/// @date 2026-05-16 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_FILTERS_SOUR_HPP_INCLUDED_
#define SYMSHELL2_FILTERS_SOUR_HPP_INCLUDED_

#include "cmath" /*DLA IMPLEMENTACJI FILTRÓW log */
#include "datasour.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

/// Szablon filtru "log 10".
template<class SOURCE_TYPE>
class log_F_filter : public template_filter_source_base<SOURCE_TYPE>
//-----------------------------------------------------------------------
{
public:
    /// Konstruktor.
    /// \param    ini to seria źródłowa.
    /// \param i_miss to wartość dla "missing data".
    /// \param format to sposób tworzenia nazwy filtra z nazwy źródła.
    explicit log_F_filter(SOURCE_TYPE *ini = NULL,
                          double i_miss = symshell2::default_missing<double>(),
                          const char *format = "log(%s)")
    : template_filter_source_base<SOURCE_TYPE>(ini, format)
    { data_source_base::set_missing(i_miss); }

    /// Liczba danych jak w źródle, ale minimum i maksimum to logarytm tych wartości ze źródła.
    void bounds(size_t &N, double &min, double &max) override
    {
        template_filter_source_base<SOURCE_TYPE>::bounds(N, min, max);
        if(this->y_min < this->y_max)
        {
            min = this->y_min;
            max = this->y_max;
        }
        else
        {
            assert(min < max);
            if(min > 0)
                min = log10(min);
            else
                min = 0;
            if(max > 0)
                max = log10(max);
            else
                max = this->miss;
            assert(min <= max);
        }
    }

    /// Daje następną z N liczb. Po N-tej zwalnia iterator poprzez obiekt źródłowy.
    /// @return wartość ze źródła przetworzoną funkcją `log10`
    double get(data_source_base::iterator_h &I) override
    {
        double pom = ((SOURCE_TYPE *) this->Source)->get(I); //template_filter_source_base<SOURCE_TYPE>::get(I);
        if(/*template_filter_source_base<SOURCE_TYPE>::*/this->is_missing(pom))
            return this->miss;
        if(pom > 0)
            return log10(pom);
        else
            return this->miss; //Nie można obliczyć
    }

    /// Przetwarza index uzyskany z geometrii ŹRÓDŁA DANYCH na wartość zlogarytmowaną.
    /// Geometrię uzyskuje się normalnie przed wirtualne `get_geometry` zaimplementowane w klasie bazowej filtrów.
    double get(size_t index_from_geometry) override
    {
        double pom = ((SOURCE_TYPE *) this->Source)->get(index_from_geometry);
        if(/*template_filter_source_base<SOURCE_TYPE>::*/this->is_missing(pom))
            return this->miss;
        if(pom > 0)
            return log10(pom);
        else
            return this->miss; //Nie można obliczyć
    }

};

/// WERSJA ZGENERALIZOWANA SZABLONU `log_F_filter`.
typedef log_F_filter<data_source_base> generic_log_F_filter;
//--------------------------------------------------------------------------

/// Szablon filtru "1+log10".
/// Przydatne w wizualizacji. Dodanie jedynki gwarantuje, że wartość logarytmu nigdy nie będzie ujemna.
template<class SOURCE_TYPE>
class log_1_plus_F_filter : public template_filter_source_base<SOURCE_TYPE>
//-----------------------------------------------------------------------
{
public:
    /// Constructor.
    /// \param    ini to seria źródłowa.
    /// \param i_miss to wartość dla "missing data".
    /// \param format to sposób tworzenia nazwy filtra z nazwy źródła.
    explicit log_1_plus_F_filter(SOURCE_TYPE *ini = NULL,
                                 double i_miss = symshell2::default_missing<double>(),
                                 const char *format = "log(1+%s)")
    : template_filter_source_base<SOURCE_TYPE>(ini, format)
    {
        data_source_base::set_missing(i_miss);
    }

    /// Liczba danych jak w źródle, ale minimum i maksimum to logarytm powiększonych o 1 wartości ze źródła.
    void bounds(size_t &N, double &min, double &max) override
    {
        template_filter_source_base<SOURCE_TYPE>::bounds(N, min, max);

        if(this->y_min < this->y_max)
        {
            min = this->y_min;
            max = this->y_max;
        }
        else
        {
            //assert(min<max);???Chyba niepotrzebne - po prostu "Invalid data"
            min += 1; ///+1 !!!
            if(min > 0)
                min = log10(min);
            else
                min = 0;
            max += 1; ///+1 !!!
            if(max > 0)
                max = log10(max);
            else
                max = this->miss;
            assert(min <= max);
        }
    }

    /// Daje następną z N liczb. Po N-tej zwalnia iterator poprzez obiekt źródłowy.
    /// @return wartość ze źródła +1 i przetworzoną funkcją `log10`.
    double get(data_source_base::iterator_h &I) override
    {
        double pom = ((SOURCE_TYPE *) this->Source)->get(I); //template_filter_source_base<SOURCE_TYPE>::get(I)+1;
        if(/*template_filter_source_base<SOURCE_TYPE>::*/this->is_missing(pom))
            return this->miss;
        pom += 1; //Unikamy wartości nieobliczalnych
        if(pom > 0)
            return log10(pom);
        else
            return this->miss; //Nie można obliczyć
    }

    /// Przetwarza index uzyskany z geometrii ŹRÓDŁA DANYCH na zlogarytmowaną wartość+1.
    /// Geometrię uzyskuje się normalnie przed wirtualne `get_geometry` zaimplementowane w klasie bazowej filtrów.
    double get(size_t index_from_geometry) override
    //Przetwarza index uzyskany z geometrii
    //na wartość z serii, o ile jest możliwe czytanie losowe
    {
        double pom = ((SOURCE_TYPE *) this->Source)->get(index_from_geometry);
        if(/*template_filter_source_base<SOURCE_TYPE>::*/this->is_missing(pom))
            return this->miss;
        pom += 1; //Unikamy wartości nieobliczalnych
        if(pom > 0)
            return log10(pom);
        else
            return this->miss; //Nie można obliczyć
    }

};

/// WERSJA ZGENERALIZOWANA SZABLONU `log_1_plus_F_filter`.
typedef log_1_plus_F_filter<data_source_base> generic_log_1_plus_F_filter;
//--------------------------------------------------------------------------

/// Bazowy szablon filtru porównującego wartości ze źródła ze skalarem.
template<class SOURCE_TYPE>
class threshold_filter_base : public template_filter_source_base<SOURCE_TYPE>
//---------------------------------------------------------------------------------
{
protected:
    double thr_val; ///< Wartość progu.

public:
    /// Constructor.
    /// \param  i_thr to próg
    /// \param    ini to seria źródłowa.
    /// \param i_miss to wartość dla "missing data".
    /// \param format to sposób tworzenia nazwy filtra z nazwy źródła i progu(?).
    explicit threshold_filter_base(double i_thr,
                                   SOURCE_TYPE *ini = NULL,
                                   double i_miss = symshell2::default_missing<double>()/*DEFAULT_MISSING*/,
                                   const char *format = "(%s) @ %g")

    : template_filter_source_base<SOURCE_TYPE>(ini, format), thr_val(i_thr)
    {
        data_source_base::set_missing(i_miss);
    }

    /// Nazwa filtra utworzona za pomocą wzorca "format" z konstruktora.
    const char *name() override;

    /// WYMAGANA: Funkcja sprawdzająca warunek i ewentualnie zmieniająca wartość na miss
    virtual double _get(const double &val) = 0;

    /// Pobieranie danych z iteratora korzysta z implementacji filtrowania w `_get`.
    double get(data_source_base::iterator_h &I) override
    {
        double val = ((SOURCE_TYPE *) this->Source)->get(I);
        return _get(val);
    }

    /// Pobieranie danych wg indeksu geometrii ŹRÓDŁA DANYCH korzysta z implementacji filtrowania w `_get`.
    double get(size_t index_from_geometry) override
    {
        double val = this->Source->get(index_from_geometry);
        return _get(val);
    }
};

/// Szablon filtru porównującego "Equal".
template<class SOURCE_TYPE>
class EQ_filter : public threshold_filter_base<SOURCE_TYPE>
//---------------------------------------------------------
{
public:
    /// Konstruktor.
    /// \param  i_val to próg?
    /// \param    ini to seria źródłowa.
    /// \param i_miss to wartość dla "missing data".
    /// \param format to sposób tworzenia nazwy filtra z nazwy źródła i progu(?).
    explicit EQ_filter(double i_val,
                       SOURCE_TYPE *ini = NULL,
                       double i_miss = symshell2::default_missing<double>()/*DEFAULT_MISSING*/,
                       const char *format = "(%s)=%g")
    : threshold_filter_base<SOURCE_TYPE>(i_val, ini, i_miss, format)
    {}

    /// Funkcja dostarczająca wartość zgodnie z warunkiem.
    double _get(const double &pom) override
    {
        if(threshold_filter_base<SOURCE_TYPE>::IsMissing(pom))
            return this->miss;
        if(pom == this->thr_val)
            return pom;
        else
            return this->miss; //Usuwa wartość ze "strumienia"
    }
};

/// WERSJA ZGENERALIZOWANA SZABLONU `EQ_filter`.
typedef EQ_filter<data_source_base> generic_EQ_filter;
//--------------------------------------------------------------------------

template<class SOURCE_TYPE>	//Szablon filtru - LessThan
class LT_filter : public threshold_filter_base<SOURCE_TYPE>
//---------------------------------------------------------
{
public:
    /// Konstruktor.
    /// \param  i_val to próg?
    /// \param    ini to seria źródłowa.
    /// \param i_miss to wartość dla "missing data".
    /// \param format to sposób tworzenia nazwy filtra z nazwy źródła i progu(?).
    explicit LT_filter(double i_val,
                       SOURCE_TYPE *ini = NULL,
                       double i_miss = symshell2::default_missing<double>()/*DEFAULT_MISSING*/,
                       const char *format = "(%s)<%g")
    : threshold_filter_base<SOURCE_TYPE>(i_val, ini, i_miss, format)
    {}

    /// Funkcja dostarczająca wartość zgodnie z warunkiem.
    double _get(const double &pom) override
    {
        if(threshold_filter_base<SOURCE_TYPE>::IsMissing(pom))
            return this->miss;
        if(pom < this->thr_val)
            return pom;
        else
            return this->miss; //Usuwa wartość ze "strumienia"
    }
};

/// WERSJA ZGENERALIZOWANA SZABLONU `LT_filter`.
typedef LT_filter<data_source_base> generic_LT_filter;
//--------------------------------------------------------------------------

template<class SOURCE_TYPE>	//Szablon filtru - LessEqual
class LE_filter : public threshold_filter_base<SOURCE_TYPE>
//---------------------------------------------------------
{
public:
    /// Konstruktor.
    /// \param  i_val to próg?
    /// \param    ini to seria źródłowa.
    /// \param i_miss to wartość dla "missing data".
    /// \param format to sposób tworzenia nazwy filtra z nazwy źródła i progu(?).
    explicit LE_filter(double i_val,
                       SOURCE_TYPE *ini = NULL,
                       double i_miss = symshell2::default_missing<double>()/*DEFAULT_MISSING*/,
                        const char *format = "(%s)<=%g")
    : threshold_filter_base<SOURCE_TYPE>(i_val, ini, i_miss, format)
    {}

    /// Funkcja dostarczająca wartość zgodnie z warunkiem.
    double _get(const double &pom) override
    {
        if(threshold_filter_base<SOURCE_TYPE>::IsMissing(pom))
            return this->miss;
        if(pom <= this->thr_val)
            return pom;
        else
            return this->miss; //Usuwa wartość ze "strumienia"
    }
};

/// WERSJA ZGENERALIZOWANA SZABLONU `LE_filter`.
typedef LE_filter<data_source_base> generic_LE_filter;
//--------------------------------------------------------------------------

template<class SOURCE_TYPE>	//Szablon filtru - MoreThan
class GT_filter : public threshold_filter_base<SOURCE_TYPE>
//---------------------------------------------------------
{
public:
    /// Constructor.
    /// \param  i_val to próg?
    /// \param    ini to seria źródłowa.
    /// \param i_miss to wartość dla "missing data".
    /// \param format to sposób tworzenia nazwy filtra z nazwy źródła i progu(?).
    explicit GT_filter( double i_val,
                        SOURCE_TYPE *ini = NULL,
                        double i_miss = symshell2::default_missing<double>()/*DEFAULT_MISSING*/,
                        const char *format = "(%s)>%g")
    : threshold_filter_base<SOURCE_TYPE>(i_val, ini, i_miss, format)
    {}

    /// Funkcja dostarczająca wartość zgodnie z warunkiem.
    double _get(const double &pom) override
    {
        if(threshold_filter_base < SOURCE_TYPE > ::is_missing(pom))
            return this->miss;
        if(pom > this->thr_val)
            return pom;
        else
            return this->miss; //Usuwa wartość ze "strumienia"
    }
};

/// WERSJA ZGENERALIZOWANA SZABLONU `GT_filter`.
typedef GT_filter<data_source_base> generic_GT_filter;
//--------------------------------------------------------------------------

/// Szablon filtru "Equal of more".
template<class SOURCE_TYPE>
class GE_filter : public threshold_filter_base<SOURCE_TYPE>
//---------------------------------------------------------
{
public:
    /// Constructor.
    /// \param  i_val to próg?
    /// \param    ini to seria źródłowa.
    /// \param i_miss to wartość dla "missing data".
    /// \param format to sposób tworzenia nazwy filtra z nazwy źródła i progu(?).
    explicit GE_filter(double        i_val,
                       SOURCE_TYPE   *ini = NULL,
                       double      i_miss = symshell2::default_missing<double>()/*DEFAULT_MISSING*/,
                       const char *format = "(%s)>=%g")
    : threshold_filter_base<SOURCE_TYPE>(i_val, ini, i_miss, format)
    {}

    /// Funkcja dostarczająca wartość zgodnie z warunkiem.
    double _get(const double &I) override //TODO Test! Jednak oryginalnie parameter był "pom"?!
    {
        double pom = threshold_filter_base < SOURCE_TYPE > ::get(I); //!?!?!?!?!?!?!!?!?!?!?!
        assert("Strange and not tested code in use!!!" == NULL);
        if(threshold_filter_base < SOURCE_TYPE > ::IsMissing(pom))
            return this->miss;
        if(pom >= this->thr_val)
            return pom;
        else
            return this->miss; //Usuwa wartość ze "strumienia"
    }
};

/// WERSJA ZGENERALIZOWANA SZABLONU `GE_filter`.
typedef GE_filter<data_source_base> generic_GE_filter;
//--------------------------------------------------------------------------


// IMPLEMENTACJE METOD:
// ////////////////////

template<class SOURCE_TYPE>
const char *threshold_filter_base<SOURCE_TYPE>::name()
//Musi zwracać nazwę serii albo "" - NIE NULL!!!
{
    const char *pom = this->Source->name();
    if(!this->_name.OK() || strstr(this->_name.get_ptr_val(), pom) == NULL) //Jeśli jeszcze nie ma albo zmieniło się w obiekcie źródła
    {
        this->_name.alloc(strlen(title_util::name()) + strlen(pom) + 1 + ZAPAS_NA_CYFRY/*Najdłuższa możliwa liczba?*/);
        sprintf(this->_name.get_ptr_val(), title_util::name(), pom, thr_val);
    }
    return this->_name.get_ptr_val();
}

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
#endif //SYMSHELL2_FILTERS_SOUR_HPP_INCLUDED_




