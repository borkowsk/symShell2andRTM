/// @file
/// @brief Specific source classes — access to data in linear arrays/
///        Konkretne klasy źródeł — dostęp do danych w tablicach liniowych.
/// @date 2026-05-07 (modified)
// ********************************************************************************************************************
// PL: W tym pliku użyto warunkowej kompilacji do selekcji języka dokumentacji.
// NIESTETY rozbija to podpowiedzi (tooltips) nawet w CLion.
// EN: This file uses conditional compilation to select the documentation language.
// UNFORTUNATELY this breaks tooltips even in CLion.
#ifndef SYMSHELL2_ARRAY_SOUR_HPP_INCLUDED_
#define SYMSHELL2_ARRAY_SOUR_HPP_INCLUDED_

#include "datasour.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

#if USE_ENGLISH_IF_POSSIBLE
/// @brief Data source class passing subsequent values from a linear array.
/// Data in the array can change, but probably not during iteration ;)
template<class T>
class array_source : public linear_source_base
#else
/// @brief Klasa źródła danych przekazująca kolejne wartości z liniowej tablicy.
/// Dane w tablicy mogą się zmieniać, ale raczej nie w trakcie iteracji ;)
template<class T>
class array_source : public linear_source_base
#endif
//--------------------------------------------------------
{
    T *arra; ///< Wskaźnik do tablicy źródłowej/Pointer to the source array.

public:
    /// @brief Constructor.
#if USE_ENGLISH_IF_POSSIBLE
    /// @param in_array is a ptr of connected C understandable array.
    /// @param in_N is a size of connected C understandable array.
    /// @param src_name is a name of data source.
#else
    /// @param in_array jest to ptr do tablicy w stylu dla C.
    /// @param in_N to rozmiar podłączonej tablicy w stylu C.
    /// @param src_name to nazwa źródła danych.
#endif
    array_source(size_t in_N, T *in_array, const char *src_name) :
            linear_source_base(in_N, src_name), arra(in_array)
    {
        miss = symshell2::default_missing<T>(); /// @internal BARDZO WAŻNE DLA SZABLONÓW!!!
    }

    /// Specyficzna dla tej klasy metoda zmiany tablicy danych źródłowych.
    void set_source(size_t NewN, T *in_array)
    {
        assert(NewN>0 && in_array!=NULL);
        arra = in_array; linear_source_base::N = NewN;
    }

// Others method:
//---------------

    /// Podaje, ile jest elementów oraz wartości minimalną i maksymalną.
    /// Jeśli minimum i maksimum nie zostały wcześniej podane jawnie albo już wcześniej obliczone
    /// , to są tu odnajdywane w pętli.
    void bounds(size_t &num, double &min, double &max) override;

    /// Daje następna z N liczb.
    double get(iteratorh &ptr_to_iterator) override
    {
        assert(ptr_to_iterator != NULL);
        return arra[_next(ptr_to_iterator)];
    }

    /// Przetwarza index uzyskany z geometrii, na wartość z serii.
    /// O ile jest możliwe czytanie w losowej kolejności.
    double get(size_t index) override
    {
        assert(index < get_size());
        return arra[index];
    }

};

#if USE_ENGLISH_IF_POSSIBLE
/// @brief A class that passes subsequent data from a linear array of structures using pointers to members.
template<class STRUCT_T, class FIELD_T>
class struct_array_source : public linear_source_base
#else
/// @brief Klasa przekazująca kolejne dane z liniowej tablicy struktur za pomocą wskaźników do składowych.
template<class STRUCT_T, class FIELD_T>
class struct_array_source : public linear_source_base
#endif
//--------------------------------------------------------
{
    typedef FIELD_T STRUCT_T::* TYP_POLA;
    STRUCT_T      *arra;   //!< Liniowa tablica struktur.
    TYP_POLA member_ptr;   //!< Wskaźnik do składowej struktury.

public:
    /// Constructor.
#if USE_ENGLISH_IF_POSSIBLE
    /// \param i_N is the size of the provided array of structures.
    /// \param i_array is a pointer to an array of structures.
    /// \param i_ptr_comp is a field selector (a pointer to a structure member).
    /// \param i_tit is the name of the data source.
    struct_array_source(size_t i_N, STRUCT_T *i_array,TYP_POLA i_ptr_comp,const char *i_tit)
#else
    /// \param i_N to rozmiar podawanej tablicy struktur.
    /// \param i_array to wskaźnik do tablicy struktur.
    /// \param i_ptr_comp to selektor pola (wskaźnik do składowej struktury).
    /// \param i_tit to nazwa źródła danych.
    struct_array_source(size_t i_N, STRUCT_T *i_array,TYP_POLA i_ptr_comp,const char *i_tit)
#endif
        :linear_source_base(i_N, i_tit), arra(i_array), member_ptr(i_ptr_comp)
    {
        miss = symshell2::default_missing<FIELD_T>(); /// @internal BARDZO WAŻNE DLA SZABLONÓW!!!
    }

    /// Specyficzna dla tej klasy metoda zmiany tablicy danych źródłowych.
    void set_source(size_t NewN, STRUCT_T *i_array)
    {
        arra = i_array; linear_source_base::N = NewN;
    }


// Others methods:
//----------------

    /// Podaje, ile jest elementów oraz wartości minimalną i maksymalną.
    /// Jeśli minimum i maksimum nie zostały wcześniej podane jawnie albo już wcześniej obliczone
    /// , to są tu odnajdywane w pętli.
    void bounds(size_t &num, double &min, double &max) override;

    /// Daje następną z N liczb!!!
    double get(iteratorh &ptr_to_iterator) override
    {
        assert(ptr_to_iterator != NULL);
        size_t pom = _next(ptr_to_iterator);
        if(pom != ULONG_MAX)
        {
            double val = arra[pom].*member_ptr;
            return val;
        } else
            return miss;
    }

    /// Przetwarza index liniowy na wartość z serii. O ile jest możliwe czytanie w losowej kolejności. Tu jest możliwe.
    double get(size_t index) override
    { //assert(index<get_size());
        if(index < get_size())
            return arra[index].*member_ptr;
        else
            return miss;
    }

};

#if USE_ENGLISH_IF_POSSIBLE
/// @brief A class that passes subsequent data from a linear array of pointers to structures
/// , using a pointer to the member. MAY BE POORLY TESTED.
template<class STRUCT_T, class FIELD_T>
class ptr_to_struct_array_source : public linear_source_base
#else
/// @brief Klasa przekazująca kolejne dane z liniowej tablicy wskaźników do struktur
/// , za pomocą wskaźnika do składowej. MOŻE BYĆ SŁABO PRZETESTOWANA.
template<class STRUCT_T, class FIELD_T>
class ptr_to_struct_array_source : public linear_source_base
#endif
//--------------------------------------------------------
{
    typedef FIELD_T STRUCT_T::* TYP_POLA;
    STRUCT_T     **arra; //!< Tablica wskaźnikowa do struktur.
    TYP_POLA member_ptr; //!< Wskaźnik do składowej struktury.

public:
    /// Constructor.
#if USE_ENGLISH_IF_POSSIBLE
    /// \param i_N is the size of the provided array of structures.
    /// \param i_array is a pointer to an array of structure pointers.
    /// \param i_ptr_comp is a field selector (a pointer to a structure member).
    /// \param i_tit is the name of the data source.
    ptr_to_struct_array_source(size_t iN,STRUCT_T **i_array,TYP_POLA i_ptr_comp,const char *i_tit)
#else
    /// \param i_N to rozmiar podawanej tablicy struktur.
    /// \param i_array to wskaźnik do tablicy wskaźników do struktur.
    /// \param i_ptr_comp to selektor pola (wskaźnik do składowej struktury).
    /// \param i_tit to nazwa źródła danych.
    ptr_to_struct_array_source(size_t iN,STRUCT_T **i_array,TYP_POLA i_ptr_comp,const char *i_tit)
#endif
        : linear_source_base(iN, i_tit), arra(i_array), member_ptr(i_ptr_comp)
    {
        miss = symshell2::default_missing<FIELD_T>(); /// @internal BARDZO WAŻNE DLA SZABLONÓW!!!
    }

    /// Specyficzna dla tej klasy metoda zmiany tablicy danych źródłowych.
    void set_source(size_t NewN, STRUCT_T **i_array)
    {
        arra = i_array;
        linear_source_base::N = NewN;
    }

// Others method:
//---------------

    /// Podaje, ile jest elementów, a także wartość minimalną i maksymalną.
    /// Jeśli minimum i maksimum nie zostały wcześniej podane jawnie albo już wcześniej obliczone,
    /// to są tu odnajdywane w pętli.
    void bounds(size_t &num, double &min, double &max) override;

    /// Daje następną z N liczb.
    double get(iteratorh &ptr_to_iterator) override
    {
        assert(ptr_to_iterator != NULL);
        size_t pom = _next(ptr_to_iterator);
        if(pom != ULONG_MAX)
        {
            double val = arra[pom]->*member_ptr;
            return val;
        } else
            return miss;
    }

    /// Przetwarza index liniowy na wartość z serii.
    /// O ile jest możliwe czytanie w losowej kolejności. Tu jest możliwe.
    double get(size_t index) override
    {
        assert(index < get_size());
        return arra[index]->*member_ptr;
    }

};

// INLINE IMPLEMENTATIONS:
//========================

template<class T> inline
void array_source<T>::bounds(size_t &num, double &min, double &max)
{
    num = N;
    if(y_min < y_max) //Sa podane.
    {
        min = y_min;
        max = y_max;
        return;
    }

    //Nie są już dane, więc próbkujemy.
    min = wbrtm::limit<T>::Max();
    max = wbrtm::limit<T>::Min();
    for(size_t i = 0; i < N; i++)
    {
        if(min > arra[i]) min = arra[i];
        if(max < arra[i]) max = arra[i];
    }
}

template<class STRUCT_T, class FIELD_T> inline
void struct_array_source<STRUCT_T, FIELD_T>::bounds(size_t &num, double &min, double &max)
{
    num = N;
    //cerr<<this->name()<<" ?\n";
    if(y_min < y_max)	//Sa dane
    {//cerr<<"???\n";
        min = y_min;
        max = y_max;
        return;
    }
    //Nie są dane, wiec próbkujemy...
    min = wbrtm::limit<FIELD_T>::Max();
    max = wbrtm::limit<FIELD_T>::Min();
    for(size_t i = 0; i < N; i++)
    {
        FIELD_T pom = arra[i].*member_ptr;
        if(pom == miss) continue; //Skip missing values!!!
        if(min > pom) min = pom;
        if(max < pom) max = pom;
    }
}

template<class STRUCT_T, class FIELD_T>
void ptr_to_struct_array_source<STRUCT_T, FIELD_T>::bounds(size_t &num, double &min, double &max)
{
    num = N;
    if(y_min < y_max)	//Sa dane
    {
        min = y_min;
        max = y_max;
        return;
    }

    //Nie są dane, wiec próbkujemy...
    min = wbrtm::limit<FIELD_T>::Max();
    max = wbrtm::limit<FIELD_T>::Min();
    for(size_t i = 0; i < N; i++)
    {
        if(min > arra[i]->*member_ptr) min = arra[i]->*member_ptr;
        if(max < arra[i]->*member_ptr) max = arra[i]->*member_ptr;
    }
}

}} // end of namespaces sym2::data

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
#endif //SYMSHELL2_ARRAY_SOUR_HPP_INCLUDED_
