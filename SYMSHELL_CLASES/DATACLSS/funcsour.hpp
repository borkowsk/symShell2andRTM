/// @file
/// @brief Classes of functional sources and their parameters/
///        Klasy źródeł funkcyjnych i ich parametrów.
/// @date 2026-05-07 (modified)
// ********************************************************************************************************************
//                       Zmodyfikowana znacząco 05.04.2008
//
#ifndef SYMSHELL2_FUNCTION_SOURCES_HPP_INCLUDED_
#define SYMSHELL2_FUNCTION_SOURCES_HPP_INCLUDED_

#ifndef __cplusplus
#error C++ required
#endif

#include "datasour.hpp" //Podstawowy interface

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

namespace sym2 { namespace data {

/// @name Przykładowe funkcje dla źródła funkcyjnego/Example functions for the function data source.
/// @param x to wartość argumentu funkcji.
/// @{

#ifdef USE_ENGLISH_IF_POSSIBLE
/// A function class defining a constant function, i.e. one that always returns the same value.
#else
/// Klasa funkcyjna definiująca funkcję stałą, czyli zwracająca zawsze te sama wartość.
#endif
template<int VDef>
class constans
{
    double value; ///< Wartość tej "stałej".

public:
    explicit constans(double IniVal) : value(IniVal)
    {}

    constans() : value(VDef)
    {}

    /// Operator obliczeniowy. @return stałą wartość `value`.
    double operator()(double x)
    { return value; }
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// A functional class wrapping the linear function `y = x` — needed e.g. to make a scatterplot for one series.
#else
/// Klasa funkcyjna opakowująca funkcje liniowa `y = x` — potrzebna np. do zrobienia scatterplot-a dla jednej serii.
#endif
class y_eq_x
{
public:
    /// Operator obliczeniowy. @returns x.
    double operator()(double x)
    { return x; }
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// A functional class wrapping cosine.
#else
/// Klasa funkcyjna opakowująca cosinus.
#endif
class cosinus
{
public:
    /// Operator obliczeniowy. @returns `cos(x)`.
    double operator()(double x)
    { return cos(x); }
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// A functional class that wraps the sine.
#else
/// Klasa funkcyjna opakowująca sinus.
#endif
class sinus
{
public:
    /// Operator obliczeniowy. @returns `sin(x)`.
    double operator()(double x)
    { return sin(x); }
};
/// @}

#ifdef USE_ENGLISH_IF_POSSIBLE
/// A function source template parameterized by a function type.
/// @tparam F must be a class with a parameterless constructor and a main method defined as `double operator () (double)`.
#else
/// Szablon źródła funkcyjnego sparametryzowany typem funkcyjnym.
/// @tparam F musi być klasą z bezparametrowym konstruktorem i główną metodą o definicji `double operator () (double)`.
#endif
template<class F>
class function_source : public function_source_base
//-------------------------------------------------
{
protected:
    F f; ///< Obiekt klasy funkcyjnej.

public:
    /// Constructor.
    /// \param i_N to gęstość próbkowania zakresu X-ow.
    /// \param i_x_min, i_x_max określają zakres po X-ach.
    /// \param i_tit to nazwa funkcji, do podpisu na wykresie.
    /// \param i_y_min, i_y_max określają zakres na y-ach. Oszczędza to czas liczenia minimum i maksimum.
    explicit function_source(size_t i_N = 10,
                             double i_x_min = 0, double i_x_max = 1,
                             const char *i_tit = "f(x)",
                             double i_y_min = 0, double i_y_max = 0) :
            function_source_base(i_N, i_x_min, i_x_max, i_tit, i_y_min, i_y_max)
    {
        //Jeśli nie podano ograniczenia po Y-ach albo podano źle.
        if(y_min == y_max)	//to jest szukane
        {
            y_min = DBL_MAX;
            y_max = -DBL_MAX;
            for(size_t i = 0; i < N; i++)
            {
                double pom = f(x_min + i * step);
                if(pom < y_min) y_min = pom;
                if(pom > y_max) y_max = pom;
            }
        }
        assert(y_min < y_max);
    }

    double get(iteratorh &p) override;

    double get(size_t i) override;
};

// IMPLEMENTACJE INLINE:
//----------------------

template<class F>
double function_source<F>::get(iteratorh &p) //Daje następną z N liczb!!!
{
    size_t pom = reinterpret_cast<size_t>(p);
    if(pom + 1 >= N)
    {
        p = NULL;
    }
    else
    {
        p = (iteratorh) (pom + 1); //kolejny krok.
    }
    return f(x_min + step * pom);
}

template<class F>
double function_source<F>::get(size_t i) //Daje którąś z N liczb!!!
{
    if(i > N)
        return miss;
    else
        return f(x_min + step * i);
}

}} // end of namespaces sym2::data

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
#endif //SYMSHELL2_FUNCTION_SOURCES_HPP_INCLUDED_




