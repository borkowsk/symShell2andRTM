/// @file
/// @brief Różne klasy dwuzrodlowych filtrow danych
/// @date 2026-05-04 (modified)
// ********************************************************************************************************************
//
#ifndef __BIFILTSOUR_HPP__
#define __BIFILTSOUR_HPP__

//#include "math.h" /*DLA FILTROW */
#include "bifilter.hpp"

/// Prosta klasa SUMUJACA dwa źródła bez użycia cache'a.
class summ_source : public bi_filter_source_base
//----------------------------------------------------
{
public:
    summ_source(data_source_base *ini1 = NULL,
                data_source_base *ini2 = NULL,
                const char *format = "SUMM(%s , %s)") :
            bi_filter_source_base(ini1, ini2, format)
    {}

//DOSTĘP DO DANYCH:
//UWAGA: Zlozona iteracja wykonuje się do konca krotszego ze źródeł.
//--------------------------------------------------------------------

//implementacja decyzji o wartości minimalnej i maksymalnej
    virtual void _bounds(double &min1, double &max1,
                         double &min2, double &max2,
                         double &min, double &max)
    {
        //jeśli jest ustawione to zostawiamy
        if(ymin < ymax)
        {
            min = ymin;
            max = ymax;
            return;
        }
        //jeśli nie ustawione to liczymy
        //Wersja prowizoryczna.
        //Naprawde powinien być cache wartości
        min = min1 < min2?min1:min2;
        max = max1 + max2;
    }

//Implementacja konkretnej operacji - musi sprawdzac, czy
//któraś z wartości lub obie nie są "missing"
    virtual double _get(double val1, double val2)
    {
        if(FromFirstIsMissing(val1) || FromSecondIsMissing(val2))
            return miss;
        else
            return val1 + val2;
    }

};

//Klasa zwracajaca wartości z drugiego źródła, jeśli pierwsze,
//traktowane jako warunek zwraca wartość nie "missing"
class if_then_source : public bi_filter_source_base
//---------------------------------------------------
{
    geometry_base *the_geom;
public:
    if_then_source(data_source_base *ini1 = NULL,
                   data_source_base *ini2 = NULL,
                   const char *format = "IF %s THEN %s ;") :
            bi_filter_source_base(ini1, ini2, format), the_geom(reinterpret_cast<geometry_base *>(ULONG_MAX))
    {}

//DOSTĘP DO DANYCH:
//UWAGA: Liniowa iteracja wykonuje się do konca krotszego ze źródeł.
//--------------------------------------------------------------------

//implementacja decyzji o wartości minimalnej i maksymalnej
    virtual void _bounds(double &min1, double &max1,
                         double &min2, double &max2,
                         double &min, double &max)
    {
        //jeśli jest ustawione to zostawiamy
        if(ymin < ymax)
        {
            min = ymin;
            max = ymax;
            return;
        }

        //jeśli nie ustawione to bierzemy z drugiego źródła bo jego
        //wartości faktycznie trafiaja na wyjscie
        min = min2;
        max = max2;
        the_geom = reinterpret_cast<geometry_base *>(ULONG_MAX); //Żeby znowu sprawdzic jak będzie potrzebne
    }

//Implementacja konkretnej operacji - musi sprawdzac, czy
//któraś z wartości lub obie nie są "missing"
    virtual double _get(double val1, double val2)
    {
        //jeśli pierwsza seria ma "missing" to zwracamy missing
        //==Realizacja warunku
        if(FromFirstIsMissing(val1))
            return miss;

        //jeśli druga seria ma i tak missing to konwertujemy
        //wartość na lokalne "miss", a, jeśli wartość jest właściwa
        //to ja zwracamy
        if(FromSecondIsMissing(val2))
            return miss;
        else
            return val2;
    }

    virtual geometry_base *getgeometry()
//Powinien zwracać wskaźnik do obowiazujacej geometrii danych
//Oba źródła musz mieć ta sama geometrie
    {
        if(the_geom != reinterpret_cast<geometry_base *>(ULONG_MAX))
        {
            return the_geom;
        } else
        {
            if((the_geom = Source2->getgeometry()) == Source->getgeometry())
                return the_geom;
            else
                return the_geom = NULL;
        }
    }

};

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



