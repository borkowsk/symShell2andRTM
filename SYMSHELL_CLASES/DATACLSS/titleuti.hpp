/// @file
/// @brief  Klasa pomocnicza zarządzająca nazwą obiektu serii danych i nie tylko.
/// @date 2026-05-15 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_TITLE_UTIL_HPP_INCLUDED_
#define SYMSHELL2_TITLE_UTIL_HPP_INCLUDED_

#include <cstddef>
#include "wb_clone.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

//using namespace wbrtm;
using wbrtm::clone_str;

/// Klasa pomocnicza zarządzająca nazwą obiektu serii danych i nie tylko.
class title_util
//--------------
{
protected:
    char *title; ///< Wskaźnik do treści tytułu/nazwy.

public:
    /// Destructor.
    ~title_util()
    {
        //if(title)
        delete title; //Deleting NULL pointer has no effect.
    }

    /// Sole constructor.
    explicit title_util(const char *i_tit = NULL)
    {
        if(i_tit != NULL)
            title = clone_str(i_tit);
        else
            title = NULL;
    }

    /// Zmiana tytułu/nazwy.
    void set_title(const char *i_tit)
    {
        //if(title)
        delete title;

        if(i_tit != NULL)
            title = clone_str(i_tit);
        else
            title = NULL;
    }

    virtual /// Aktualny tytuł bez sprawdzania, czy nie NULL.
    const char *get_title()
    {
        return title;   //Zwraca nazwę albo NULL
    }

    // Aktualny tytuł bez sprawdzania, czy nie NULL.
    // Kłóci się aktualnie z get zawracającym wartość z jednoelementowej seri danych.
//    const char *get()
//    {
//        return title;    //Zwraca nazwę albo NULL
//    }

    /// Zwraca tytuł/nazwę albo pusty łańcuch. Nigdy NULL.
    const char *name()
    {
        return title != NULL?title:"";
    }

};

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
#endif //SYMSHELL2_TITLE_UTIL_HPP_INCLUDED_
