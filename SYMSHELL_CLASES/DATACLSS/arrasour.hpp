/// @file
/// @brief Konkretne klasy źródeł — dostęp do danych w tablicach liniowych.
/// @date 2026-05-02 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_ARRAY_SOUR_HPP_INCLUDED_
#define SYMSHELL2_ARRAY_SOUR_HPP_INCLUDED_

#include "datasour.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"

/// Klasa przekazująca kolejne dane z liniowej tablicy.
/// Dane w tablicy mogą się zmieniać, ale raczej nie w trakcie iteracji ;)
template<class T>
class array_source:public linear_source_base
//--------------------------------------------------------
{
T* arra;
public:
/// @brief A sole constructor.
/// @param in_array is a ptr of connected C understandable array.
/// @param in_N is a size of connected C understandable array.
/// @param src_name is a name of data source.
array_source(size_t in_N, T* in_array, const char* src_name):
        linear_source_base(in_N, src_name), arra(in_array)
    {
        miss=default_missing<T>(); /// @internal BARDZO WAŻNE DLA SZABLONÓW!!!
    }

// Others method:
//---------------

/// Zmienia tablicę danych źródłowych.
void set_source(size_t NewN,T* in_array)
    {
        arra=in_array;
        linear_source_base::N=NewN;
    }

/// Podaje, ile jest elementów oraz wartości minimalną i maksymalną.
void  bounds(size_t& num,double& min,double& max) override
    {
    num=N;
    if(ymin<ymax) //Sa podane.
        {
        min=ymin;
        max=ymax;
        return;
        }

    //Nie są już dane, więc próbkujemy.
    min=wbrtm::limit<T>::Max();
    max=wbrtm::limit<T>::Min();
    for(size_t i=0;i<N;i++)
        {
        if(min>arra[i]) min=arra[i];
        if(max<arra[i]) max=arra[i];
        }
    }

/// Daje następna z N liczb.
double get(iteratorh& ptr_to_iterator) override
    {
    assert(ptr_to_iterator!=NULL);
    return arra[ _next(ptr_to_iterator) ];
    }

/// Przetwarza index uzyskany z geometrii, na wartość z serii.
/// O ile jest możliwe czytanie w losowej kolejności.
double get(size_t index) override
    {
    assert(index<get_size());
    return arra[ index ];
    }

};

/// Klasa przekazująca kolejne dane z liniowej tablicy struktur za pomocą wskaźników do składowych.
template<class STRUCT_T,class FIELD_T>
class struct_array_source:public linear_source_base
//--------------------------------------------------------
{
typedef FIELD_T     STRUCT_T::* TYP_POLA;
STRUCT_T*             arra;   //!< Liniowa tablica struktur.
TYP_POLA        pComponent;   //!< Wskaźnik do składowej struktury.
public:

/// Sole constructor.
struct_array_source(size_t          i_N, //!< Rozmiar podawanej tablicy struktur.
                    STRUCT_T*   i_array, //!< Wskaźnik do tablicy struktur.
                    TYP_POLA i_ptr_comp, //!< Selektor pola (wskaźnik do składowej struktury).
                    const char*   i_tit  //!< Nazwa źródła.
                    ):
        linear_source_base(i_N, i_tit), arra(i_array), pComponent(i_ptr_comp)
    {
        miss=default_missing<FIELD_T>(); /// @internal BARDZO WAŻNE DLA SZABLONÓW!!!
    }

// Others methods:
//----------------

void set_source(size_t NewN,STRUCT_T* i_array)
    {
        arra=i_array;
        linear_source_base::N=NewN;
    }

/// Podaje, ile jest elementów oraz wartości minimalną i maksymalną.
void  bounds(size_t& num,double& min,double& max) override
    {
    num=N;
    //cerr<<this->name()<<" ?\n";
    if(ymin<ymax)//Sa dane
        {//cerr<<"???\n";
        min=ymin;max=ymax;
        return;
        }
    //Nie są dane, wiec próbkujemy...
    min=wbrtm::limit<FIELD_T>::Max();
    max=wbrtm::limit<FIELD_T>::Min();
    for(size_t i=0;i<N;i++)
        {
        FIELD_T pom= arra[i].*pComponent;
        if(pom==miss) continue; //Skip missing values!!!
        if(min>pom) min=pom;
        if(max<pom) max=pom;
        }
    }

/// Daje następną z N liczb!!!
double get(iteratorh& ptr_to_iterator) override
    {
    assert(ptr_to_iterator!=NULL);
    size_t pom=_next( ptr_to_iterator );
    if(pom!=ULONG_MAX)
    {
        double val= arra[ pom ].*pComponent;
        return val;
    }
    else
        return miss;
    }

/// Przetwarza index liniowy na wartość z serii. O ile jest możliwe czytanie w losowej kolejności. Tu jest możliwe.
double get(size_t index) override
    { //assert(index<get_size());
    if(index<get_size())
        return arra[ index ].*pComponent;
    else
        return miss;
    }

};

/// Klasa przekazująca kolejne dane z liniowej tablicy wskaźników do struktur
/// , za pomocą wskaźnika do składowej.
template<class STRUCT_T,class FIELD_T>
class ptr_to_struct_array_source:public linear_source_base
//--------------------------------------------------------
{
typedef FIELD_T STRUCT_T::* TYP_POLA;
STRUCT_T**      arra; //!< Tablica wskaźnikowa do struktur.
TYP_POLA  pComponent; //!< Wskaźnik do składowej struktury.
public:
// Constructor 
ptr_to_struct_array_source(size_t iN,
                           STRUCT_T**  i_array,
                           TYP_POLA i_ptr_comp,
                           const char*   i_tit
                           ):
        linear_source_base(iN, i_tit), arra(i_array), pComponent(i_ptr_comp)
    {
        miss=default_missing<FIELD_T>(); /// @internal BARDZO WAŻNE DLA SZABLONÓW!!!
    }

// Others method:
//---------------

void set_source(size_t NewN,STRUCT_T** i_array)
    {
        arra=i_array;
        linear_source_base::N=NewN;
    }

/// Podaje, ile jest elementów, a także wartość minimalną i maksymalną.
void  bounds(size_t& num,double& min,double& max) override
    {
    num=N;
    if(ymin<ymax)//Sa dane
        {
        min=ymin;max=ymax;
        return;
        }
    //Nie są dane, wiec próbkujemy...
    min=wbrtm::limit<FIELD_T>::Max();
    max=wbrtm::limit<FIELD_T>::Min();
    for(size_t i=0;i<N;i++)
        {
        if(min> arra[i]->*pComponent) min= arra[i]->*pComponent;
        if(max< arra[i]->*pComponent) max= arra[i]->*pComponent;
        }
    }

/// Daje następną z N liczb.
double get(iteratorh& ptr_to_iterator) override
    {
    assert(ptr_to_iterator!=NULL);
    size_t pom=_next( ptr_to_iterator );
    if(pom!=ULONG_MAX)
    {
        double val= arra[ pom ]->*pComponent;
        return val;
    }
    else
        return miss;
    }

/// Przetwarza index liniowy na wartość z serii.
/// O ile jest możliwe czytanie w losowej kolejności. Tu jest możliwe.
double get(size_t index) override
    {
    assert(index<get_size());
    return arra[ index ]->*pComponent;
    }

};

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
#endif //SYMSHELL2_ARRAY_SOUR_HPP_INCLUDED_
