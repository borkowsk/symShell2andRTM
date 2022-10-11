/** @file virtualConstructor.cpp
*  Implementation of virtual constructors registration.
*====================================================================================================================
*  \details Obiekty wirtualnych konstruktorów rejestrowane są na standardowej mapie z kluczem typu std::string.
*           Created by borkowsk on 12.08.22.
*  \copyright Now, this is a part of GuestXR project.
*  \date 2022-10-05
*/

#include <map>
#include <string>
#include <iostream>
#include "virtualConstructor.h"

/// \brief Ukryta lokalnie mapa konstruktorów dla implementacji rejestracji i odnajdywania wirtualnych konstruktorów.
/// \note Można wyobrazić sobie inne implementacje niz oparte obiekcie typu std::map<>
static std::map<std::string,wbrtm::InterfaceOfVirtualConstructor*> *mapOfVirtualConstructors=nullptr;

/// \note Implementacja oparta na globalnym obiekcie typu std::map<>
void  wbrtm::InterfaceOfVirtualConstructor::registerThis(InterfaceOfVirtualConstructor* what)
{
    if(mapOfVirtualConstructors== nullptr)
    {
        mapOfVirtualConstructors=new std::map<std::string,InterfaceOfVirtualConstructor*>();
    }
    //std::string key=what->className;
    const char* key=what->className;
    (*mapOfVirtualConstructors)[key]=what;
#ifndef _NDEBUG
    //std::cerr<<what->className<<" registered!"<<std::endl;
#endif
}

/// \note Implementacja oparta na globalnym obiekcie typu std::map<>
wbrtm::InterfaceOfVirtualConstructor* wbrtm::InterfaceOfVirtualConstructor::getVirtualConstructorOf(const char* key)
{
    auto value=(*mapOfVirtualConstructors)[key];
    // ... A jak nie ma takiego klucza? Wtedy co się dzieje? TODO ?
    return value;
}

//*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//       Uzupełnienie do pakietu "Rozumek sieciowy" wersja 2022-08-16
//       (c) ISS UW 2010-2013-2016 Wojciech Borkowski, modyfikacja 2021/2022.01-08
//*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

