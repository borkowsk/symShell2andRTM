/// @file
/// @brief  Klasa pomocnicza zarządzająca nazwą obiektu serii danych i nie tylko.
/// @date 2026-04-27 (modified)
// ********************************************************************************************************************
//
#ifndef __TITLEUTIL_HPP__
#define __TITLEUTIL_HPP__

#include <cstddef>
#include "wb_clone.hpp"

//using namespace wbrtm;
using wbrtm::clone_str;

/// Klasa pomocnicza zarządzająca nazwą obiektu serii danych i nie tylko.
class title_util
//--------------
{
protected:
char* title;
public:
//Destructor 
~title_util()
    {
    if(title)
        delete title;
    }

//Constructor
title_util(const char* tit=NULL)
    {
    if(tit!=NULL)
        title=clone_str(tit);
        else
        title=NULL;
    }

void settitle(const char* tit)			
    {
    if(title)
        delete title;
    if(tit!=NULL)
        title=clone_str(tit);
        else
        title=NULL;
    }

const char* gettitle()	//Zwraca nazwę albo NULL
    {
    return title;
    }

const char* name()	//Zwraca nazwę albo pusty łańcuch
    {
    return title!=NULL?title:"";
    }

};

/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*  Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk              */
/*        MAIL: borkowsk@iss.uw.edu.pl                               */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
#endif
