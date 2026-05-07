/// @file
/// @brief  Specific source classes - access to data in arrays and variables/
///         Konkretne klasy źródeł — dostęp do danych w tablicach i zmiennych.
/// @date 2026-05-07 (modified)
// ********************************************************************************************************************
// ZA DUŻO BŁĘDÓW - SPRAWDZANIE SIĘ NIE SPRAWDZA :-P
#ifndef SYMSHELL2_SCALAR_SOUR_HPP_INCLUDED_
#define SYMSHELL2_SCALAR_SOUR_HPP_INCLUDED_

#include "sourbase.hpp"
#include "datasour.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

/// Klasa źródła przechowującego i dającego pojedyncza wartość.
template<class Typek>
class scalar_source:public  template_scalar_source_base<Typek>
//-----------------------------------------------------------------
{
protected:
    Typek val; ///< Miejsce na przechowanie pojedynczej wartości danego typu.

public:
    /// Constructor.
    /// \param ini - początkowa wartość.
    /// \param title - nazwa źródła, czyli tej wartości.
    /// \param min,max  - zakres, w jakim powinna się mieścić wartość aktualna i kolejne.
    scalar_source(const Typek& ini,const char* title,
                  const Typek& min=0,const Typek& max=0)
        : template_scalar_source_base<Typek>(title,min,max)
        {
            change_val(ini);
        }

    /// Możliwość zmiany przechowywanej wartości. Zwraca uwagę na zakres `[min,max]`.
    void  change_val(const Typek& next);

    /// Uproszczona wersja `get` dająca dostęp do zawsze tylko jednej wartości.
    // TODO const T&  - TU ZMIENIĆ, GDY `source_base` stanie sie szablonem - mogą być kłopoty z MISSING VALUE
    double   get() override
        { return val;}

};

/// Klasa źródła dającego pojedyncza wartość, czytaną przez wskaźnik.
template<class T>
class ptr_to_scalar_source:public  template_scalar_source_base<T>
//-----------------------------------------------------------------
{
protected:
    const T* ptr;

public:
    using data_source_base::iteratorh;

    /// Constructor.
    /// \param ini
    /// \param title
    /// \param min,max
    ptr_to_scalar_source(const T* ini,const char* title,const T& min=0,const T& max=0)
    : template_scalar_source_base<T>(title,min,max)
    { ptr=ini;}

    //Accesors:
    //---------

    virtual void  bounds(size_t& N,double& min,double& max)
    //Ile elementów 0, czy1,wartość minimalna i maksymalna
    //sczytane z wewnetrznych pól, w razie koniecznosci rozszerzone
    //o wartość aktualna. Jeśli pominie się wywołanie bounds to
    //zakres min-max może nie odzwierciedlac prawdziwej zmiennosci
    {
      if(ptr)
        {
        N=1;
        if(this->CheckMinMax)	//this-> dodane ze względu na problemy z GCC (07.2011)
            {
            if(*ptr>this->y_max) this->y_max=*ptr;
            if(*ptr<this->y_min) this->y_min=*ptr;
            }
        }
        else N=0;
      min=this->y_min;
      max=this->y_max;
    }

    virtual data_source_base::iteratorh   reset()
    //Umożliwia czytanie od początku — iteratorh jest uchwytem iteratora
    //domyślnie z obiektu Source, ale czasem nie
    { return reinterpret_cast<data_source_base::iteratorh>(data_source_base::index_t(ptr?1:0));}

    void  change_ptr(T* next)
    //Zmienia wskaźnik a wartości mogą się zmieniać niezależnie
    { ptr=next; }

    //const T&  - - - TU ZMIENIC GDY source_base stanie szablonem
    double   get()  //Uproszczona wersja get() bo i tak jest tylko jedna wartość. Musi być double a nie T bo miss jest typu double!!!
    {
        if(ptr==NULL)
            return data_source_base::get_missing();
        else
            if(*ptr==data_source_base::get_missing())
                return data_source_base::get_missing();
            else
                return *ptr;
    }

};

/// Klasa źródła dającego pojedyncza wartość czytana przez wywołanie
/// bezparametrowej funkcji lub statycznej metody.
template<class RET>
class ptr_to_function_source: public  template_scalar_source_base<RET>
//-----------------------------------------------------------------
{
    typedef RET (*F)(void);
protected:
    F ptr; //Jest wskaźnikiem do funkcji bezparametrowej
public:
    //Constructor
    ptr_to_function_source(F ini, const char* title, const RET& min=0, const RET& max=0):
       ptr(ini),
       template_scalar_source_base<RET>(title,min,max)
    {}

//Accesors:
//=========

    virtual void  bounds(size_t& N,double& min,double& max)
    //Ile elementów 0, czy1,wartość minimalna i maksymalna
    //sczytane z wewnętrznych pól, w razie konieczności rozszerzone
    //o wartość aktualną. Jeśli pominie się wywołanie bounds to
    //zakres min-max może nie odzwierciedlać prawdziwej zmienności
    {
        if(ptr)
        {
        N=1;
        RET r=ptr();
        if(this->CheckMinMax)
            {
            if(r>this->y_max) this->y_max=r;
            if(r<this->y_min) this->y_min=r;
            }
        }
        else N=0;
        min=this->y_min;
        max=this->y_max;
    }

    virtual data_source_base::iteratorh   reset()
    //Umożliwia czytanie od początku — iteratorh jest uchwytem iteratora
    //domyślnie z obiektu Source, ale czasem nie
    { return reinterpret_cast<data_source_base::iteratorh>(static_cast<data_source_base::index_t>(ptr?1u:0u));}

    void  change_ptr(F next)
    //Zmienia wskaźnik, a wartości mogą się zmieniać niezależnie.
    { ptr=next; }

    // TODO const T&  - - - TU ZMIENIĆ GDY source_base stanie szablonem
    double   get()  //Uproszczona wersja get() bo i tak jest tylko jedna wartość. Musi być double a nie T bo miss jest typu double!!!
    {
        if(ptr==NULL)
            return data_source_base::get_missing();
        else
        {
            RET r=ptr();
            if(r==data_source_base::get_missing())
                return data_source_base::get_missing();
            else
                return r;
        }
    }

};

// IMPLEMENTACJE INLINE
//=====================

template<class Typek> inline
void  scalar_source<Typek>::change_val(const Typek& next)
{
    val=next;
    //Uaktualnienie zakresu min-max
    if(this->CheckMinMax) //this-> dodane ze względu na problemy z GCC (07.2011)
    {
        if(val>this->y_max) this->y_max=val;
        if(val<this->y_min) this->y_min=val;
    }
}

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
#endif //SYMSHELL2_SCALAR_SOUR_HPP_INCLUDED_





