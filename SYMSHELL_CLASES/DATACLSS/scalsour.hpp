/// @file
/// @brief  Specific source classes - access to data in variables/
///         Konkretne klasy źródeł — dostęp do danych w zmiennych.
/// @date 2026-05-15 (modified)
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

namespace sym2 { namespace data {

/// Klasa źródła przechowującego i dającego pojedyncza wartość.
template<class VAL_TYPE>
class scalar_source:public  template_scalar_source_base<VAL_TYPE>
//-----------------------------------------------------------------
{
protected:
    VAL_TYPE val; ///< Miejsce na przechowanie pojedynczej wartości danego typu.

public:
    /// Constructor.
    /// \param ini - początkowa wartość.
    /// \param title - nazwa źródła, czyli tej wartości.
    /// \param min,max  - zakres, w jakim powinna się mieścić wartość aktualna i kolejne.
    scalar_source(const VAL_TYPE& ini,
                  const char* title,
                  const VAL_TYPE& min=0, const VAL_TYPE& max=0)
    : template_scalar_source_base<VAL_TYPE>(title, min, max)
    {
        change_val(ini);
    }

    /// Destructor.
    ~scalar_source() override=default;

    /// Możliwość zmiany przechowywanej wartości. Zwraca uwagę na zakres `[minimum, maksimum]`.
    void  change_val(const VAL_TYPE& next);

//Accessors:
//---------

    /// Implementacja ograniczeń.
    /// Ile elementów? 1! Wartość minimalna i maksymalna sczytane z wewnętrznych pól.
    /// W razie konieczności rozszerzone o wartość aktualną.
    /// Jeśli pominie się wywołanie `bounds`, to zakres min-max może nie odzwierciedlać prawdziwej zmienności.
    void  bounds(size_t& N,double& min,double& max) override;

    /// Uproszczona wersja `get` dająca dostęp do zawsze tylko jednej wartości.
    // TODO const T&  - TU ZMIENIĆ, GDY `source_base` stanie się szablonem - mogą być kłopoty z MISSING VALUE
    double   get() override
    { return val;}

};

/// Klasa źródła dającego pojedyncza wartość, czytaną przez wskaźnik.
template<class T>
class ptr_to_scalar_source:public  template_scalar_source_base<T>
//-----------------------------------------------------------------
{
protected:
    const T* ptr; /// Wskaźnik do wartości/zmiennej danego typu.

public:
    using data_source_base::iterator_h;

    /// Constructor.
    /// \param ini - początkowa wartość wskaźnika do zmiennej.
    /// \param title - nazwa źródła, czyli tej wartości.
    /// \param min,max  - zakres, w jakim powinna się mieścić wartość aktualna i kolejne.
    ptr_to_scalar_source(const T* ini,const char* title,const T& min=0,const T& max=0)
    : template_scalar_source_base<T>(title,min,max), ptr(ini)
    {}

    /// Destructor.
    ~ptr_to_scalar_source() override=default;

    /// Zmienia wskaźnik, ale wartości mogą się zmieniać niezależnie.
    void  change_ptr(T* next)
    { ptr=next; }

//Accessors:
//---------

    /// Implementacja ograniczeń.
    /// Ile elementów? 0 albo 1! Wartość minimalna i maksymalna sczytane z wewnętrznych pól.
    /// W razie konieczności rozszerzone o wartość aktualną.
    /// Jeśli pominie się wywołanie `bounds`, to zakres min-max może nie odzwierciedlać prawdziwej zmienności.
    void  bounds(size_t& N,double& min,double& max) override;

    /// Początek "pseudoiteracji". @return zakodowane 0 albo 1, zależnie od tego, czy wewnętrzny wskaźnik nie jest NULL.
    virtual data_source_base::iterator_h   reset()
    { return reinterpret_cast<data_source_base::iterator_h>(data_source_base::index_t(ptr?1:0));}

    /// Uproszczona wersja `get` bo i tak jest tylko jedna wartość.
    /// Musi być `double`, a nie T, bo miss jest typu double!!!
    // TODO const T&  - - - TU ZMIENIĆ GDY `source_base` stanie szablonem.
    double   get()
    {
        auto miss=data_source_base::get_missing();
        if(ptr==NULL || *ptr==miss)
            return miss;
        else
            return *ptr;
    }

};

/// Klasa źródła dającego pojedynczą wartość uzyskiwaną przez wywołanie bezparametrowej funkcji lub statycznej metody.
template<class RET>
class ptr_to_function_source: public  template_scalar_source_base<RET>
//-----------------------------------------------------------------
{
protected:
    typedef RET return_type;
    typedef RET (*func_type)();
    func_type    ptr; ///< Jest wskaźnikiem do funkcji bezparametrowej

public:
    /// Constructor.
    /// \param ini - początkowa wartość wskaźnika do funkcji.
    /// \param title - nazwa źródła, czyli tej wartości.
    /// \param min,max  - zakres, w jakim powinna się mieścić wartość aktualna i kolejne.
    ptr_to_function_source( func_type ini,
                            const char* title,
                            const RET& min=0, const RET& max=0)
    : template_scalar_source_base<RET>(title,min,max), ptr(ini)
    {}

    /// Destructor.
    ~ptr_to_function_source() override=default;

    /// Zmienia wskaźnik do funkcji, a wartości mogą się zmieniać niezależnie.
    void  change_ptr(func_type next)
    { ptr=next; }

//Accessors:
//==========

    /// Implementacja ograniczeń.
    /// Ile elementów? 0 albo 1! Wartość minimalna i maksymalna sczytane z wewnętrznych pól.
    /// W razie konieczności rozszerzone o wartość aktualną.
    /// Jeśli pominie się wywołanie `bounds`, to zakres min-max może nie odzwierciedlać prawdziwej zmienności.
    void  bounds(size_t& N,double& min,double& max) override;

    /// Początek "pseudoiteracji". @return zakodowane 0 albo 1, zależnie od tego, czy wewnętrzny wskaźnik nie jest NULL.
    virtual data_source_base::iterator_h   reset()
    { return reinterpret_cast<data_source_base::iterator_h>(static_cast<data_source_base::index_t>(ptr?1u:0u));}

    /// Uproszczona wersja `get`, bo i tak jest tylko jedna wartość.
    /// Musi być `double`, a nie T bo miss jest typu double!!!
    // TODO const T&  - - - TU ZMIENIĆ GDY source_base stanie szablonem
    double   get()  override
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

template<class VAL_TYPE>
void scalar_source<VAL_TYPE>::bounds(size_t &N, double &min, double &max)
{
    N=1;
    if(this->CheckMinMax)	//this-> dodane ze względu na problemy z GCC (07.2011)
    {
        if(this->val > this->y_max) this->y_max=this->val;
        if(this->val < this->y_min) this->y_min=this->val;
    }

    min=this->y_min;
    max=this->y_max;
}

template<class T>
void ptr_to_scalar_source<T>::bounds(size_t &N, double &min, double &max)
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
    else
    { N=0; }
    min=this->y_min;
    max=this->y_max;
}

template<class RET>
void ptr_to_function_source<RET>::bounds(size_t &N, double &min, double &max)
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

template<class VAL_TYPE> inline
void  scalar_source<VAL_TYPE>::change_val(const VAL_TYPE& next)
{
    val=next;
    //Uaktualnienie zakresu min-max
    if(this->CheckMinMax) //this-> dodane ze względu na problemy z GCC (07.2011)
    {
        if(val>this->y_max) this->y_max=val;
        if(val<this->y_min) this->y_min=val;
    }
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
#endif //SYMSHELL2_SCALAR_SOUR_HPP_INCLUDED_





