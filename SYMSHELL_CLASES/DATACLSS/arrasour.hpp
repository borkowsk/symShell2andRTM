/// @file
/// @brief **Specific source types — access to data in linear arrays** /<br>
///         _Konkretne klasy źródeł — dostęp do danych w tablicach liniowych._
/// @date 2026-05-20 (modified)
// ********************************************************************************************************************
// PL: W tym pliku użyto warunkowej kompilacji do selekcji języka dokumentacji.
// NIESTETY rozbija to podpowiedzi (tooltips) nawet w CLion.
// EN: This file uses conditional compilation to select the documentation language.
// UNFORTUNATELY, this breaks tooltips even in CLion.
#ifndef SYMSHELL2_ARRAY_SOUR_HPP_INCLUDED_
#define SYMSHELL2_ARRAY_SOUR_HPP_INCLUDED_

#include "datasour.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief Data source type passing subsequent values from a linear array.
/// @details Data in the array can change, but probably not during iteration ;)
template<class T>
class array_source : public linear_source_base
#else
/// @brief Klasa źródła danych przekazująca kolejne wartości z liniowej tablicy.
/// @details Dane w tablicy mogą się zmieniać, ale raczej nie w trakcie iteracji ;)
template<class T>
class array_source : public linear_source_base
#endif
//--------------------------------------------------------
{
    T *arra; ///< @brief @in{Wskaźnik do tablicy źródłowej | Pointer to the source array}.

public:
    /// @brief Constructor.
    /// @param in_array @in{ jest to ptr do tablicy w stylu dla C. | is a ptr of connected C understandable array. }
    /// @param in_N     @in{ to rozmiar podłączonej tablicy w stylu C. | is a size of connected C understandable array. }
    /// @param src_name @in{ to nazwa źródła danych. | src_name is a name of data source. }
    array_source(size_t in_N, T *in_array, const char *src_name) :
            linear_source_base(in_N, src_name), arra(in_array)
    {
        miss = default_missing<T>(); /// @internal BARDZO WAŻNE DLA SZABLONÓW!!!
    }

    /// @brief @in{ Specyficzna dla tej klasy metoda zmiany tablicy danych źródłowych. }
    ///           { A method specific to this type for changing the source data array. }
    void set_source(size_t NewN, T *in_array)
    {
        assert(NewN>0 && in_array!=NULL);
        arra = in_array; linear_source_base::N = NewN;
    }

// Others method:
//---------------
    /// @brief @if POLISH
    /// Podaje, ile jest elementów oraz wartości minimalną i maksymalną.
    /// @details
    /// Jeśli minimum i maksimum nie zostały wcześniej podane jawnie albo już wcześniej obliczone, to są tu odnajdywane w pętli.
    /// @elseif ENGLISH
    /// Returns the number of elements and the minimum and maximum values.
    /// @details
    /// If the minimum and maximum values were not explicitly specified or were already calculated, they are found here in the loop.
    /// @endif
    void bounds(size_t &num, double &min, double &max) override;

    /// @brief @in{ Daje następna z N liczb. | Gives the next of N numbers. }
    double get(iterator_h &ptr_to_iterator) override
    {
        assert(ptr_to_iterator != NULL);
        return arra[_next(ptr_to_iterator)];
    }

    /// @brief @if POLISH
    /// Przetwarza index uzyskany z geometrii, na wartość z serii.
    /// @elseif ENGLISH
    /// Converts the index obtained from the geometry into a value from the series.
    /// @endif
    double get(size_t index) override
    {
        assert(index < get_size());
        return arra[index];
    }

};

/// @if POLSKI
/// @brief Klasa przekazująca kolejne dane z liniowej tablicy struktur za pomocą wskaźników do składowych.
/// @endif @if ENGLISH
/// @brief A type that passes subsequent data from a linear array of structures using pointers to members.
/// @endif
template<class STRUCT_T, class FIELD_T>
class struct_array_source : public linear_source_base
//--------------------------------------------------------
{
    typedef FIELD_T STRUCT_T::* TYP_POLA;
    STRUCT_T      *arra;   //!< @brief \in{ Liniowa (1D) tablica struktur | Linear (1D) array of structures}.
    TYP_POLA member_ptr;   //!< @brief \in{ Wskaźnik do składowej struktury | A pointer to a structure member}.

public:
    /// Constructor.
    /// @if ENGLISH
    /// \param i_N is the size of the provided array of structures.
    /// \param i_array is a pointer to an array of structures.
    /// \param i_ptr_comp is a field selector (a pointer to a structure member).
    /// \param i_tit is the name of the data source.
    /// @elseif POLISH
    /// \param i_N to rozmiar podawanej tablicy struktur.
    /// \param i_array to wskaźnik do tablicy struktur.
    /// \param i_ptr_comp to selektor pola (wskaźnik do składowej struktury).
    /// \param i_tit to nazwa źródła danych.
    /// @endif
    struct_array_source(size_t i_N, STRUCT_T *i_array,TYP_POLA i_ptr_comp,const char *i_tit)
        :linear_source_base(i_N, i_tit), arra(i_array), member_ptr(i_ptr_comp)
    {
        miss = default_missing<FIELD_T>(); /// @internal BARDZO WAŻNE DLA SZABLONÓW!!!
    }

    /// @brief @i3{ Specyficzna dla tej klasy metoda zmiany tablicy danych źródłowych. |<BR>|
    ///             A method specific to this type for changing the source data array. }
    void set_source(size_t NewN, STRUCT_T *i_array)
    {
        arra = i_array; linear_source_base::N = NewN;
    }


// Others methods:
//----------------
    /// @brief @if POLSKI
    /// Podaje, ile jest elementów oraz wartości minimalną i maksymalną.
    /// @details Jeśli minimum i maksimum nie zostały wcześniej podane jawnie albo już wcześniej obliczone, to są tu odnajdywane w pętli.
    /// @elseif ENGLISH
    /// Calculates the number of elements and the minimum and maximum values.
    /// @details If the minimum and maximum values were not explicitly specified or were already calculated, they are found here in the loop.
    /// @endif
    void bounds(size_t &num, double &min, double &max) override;

    /// @brief @i3{ Daje następną z N liczb. |<br>| Gives the next of N numbers. }
    double get(iterator_h &ptr_to_iterator) override
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

    /// @brief
    ///  @PL{ Przetwarza index liniowy na wartość z serii. O ile jest możliwe czytanie w losowej kolejności. Tu jest możliwe. }
    ///  @EN{ Converts a linear index to a series value. If possible, read in random order. This is possible here. }
    double get(size_t index) override
    { //assert(index<get_size());
        if(index < get_size())
            return arra[index].*member_ptr;
        else
            return miss;
    }

};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief A type that passes subsequent data from a linear array of pointers to structures
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
    STRUCT_T     **arra; //!< @in{ Tablica wskaźników do struktur.| Array of pointers to structures. }
    TYP_POLA member_ptr; //!< @in{ Wskaźnik do składowej struktury.| A pointer to a structure member. }

public:
#ifdef USE_ENGLISH_IF_POSSIBLE
    /// Constructor.
    /// \param i_N is the size of the provided array of structures.
    /// \param i_array is a pointer to an array of structure pointers.
    /// \param i_ptr_comp is a field selector (a pointer to a structure member).
    /// \param i_tit is the name of the data source.
    ptr_to_struct_array_source(size_t iN,STRUCT_T **i_array,TYP_POLA i_ptr_comp,const char *i_tit)
#else
    /// Konstruktor.
    /// \param i_N to rozmiar podawanej tablicy struktur.
    /// \param i_array to wskaźnik do tablicy wskaźników do struktur.
    /// \param i_ptr_comp to selektor pola (wskaźnik do składowej struktury).
    /// \param i_tit to nazwa źródła danych.
    ptr_to_struct_array_source(size_t iN,STRUCT_T **i_array,TYP_POLA i_ptr_comp,const char *i_tit)
#endif
        : linear_source_base(iN, i_tit), arra(i_array), member_ptr(i_ptr_comp)
    {
        miss = default_missing<FIELD_T>(); /// @internal BARDZO WAŻNE DLA SZABLONÓW!!!
    }

    /// @brief @PL{ Specyficzna dla tej klasy metoda zmiany tablicy danych źródłowych.}
    ///        @EN{ A method specific to this type for changing the source data array.}
    void set_source(size_t NewN, STRUCT_T **i_array)
    {
        arra = i_array;
        linear_source_base::N = NewN;
    }

// Others method:
//---------------

    /// @brief @if POLISH
    /// Podaje, ile jest elementów oraz wartości minimalną i maksymalną.
    /// @details Jeśli minimum i maksimum nie zostały wcześniej podane jawnie albo już wcześniej obliczone, to są tu odnajdywane w pętli.
    /// @elseif ENGLISH
    /// Calculates the number of elements and the minimum and maximum values.
    /// @details If the minimum and maximum values were not explicitly specified or were already calculated, they are found here in the loop.
    /// @endif
    void bounds(size_t &num, double &min, double &max) override;

    /// @brief @i3{ Daje następną z N liczb. |<br>| Gives the next of N numbers. }
    double get(iterator_h &ptr_to_iterator) override
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

    /// @brief
    ///  @PL{ Przetwarza index liniowy na wartość z serii. O ile jest możliwe czytanie w losowej kolejności. Tu jest możliwe. }
    ///  @EN{ Converts a linear index to a series value. If reading in random order is possible. This is possible here. }
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
    //Nie są dane, więc próbkujemy...
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

    //Nie są dane, więc próbkujemy...
    min = wbrtm::limit<FIELD_T>::Max();
    max = wbrtm::limit<FIELD_T>::Min();
    for(size_t i = 0; i < N; i++)
    {
        if(min > arra[i]->*member_ptr) min = arra[i]->*member_ptr;
        if(max < arra[i]->*member_ptr) max = arra[i]->*member_ptr;
    }
}

}} // end-of-namespaces sym2::data

#pragma clang diagnostic pop
/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/              */
/*        MAIL:  wborkowski@uw.edu.pl                                 */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
#endif //SYMSHELL2_ARRAY_SOUR_HPP_INCLUDED_
