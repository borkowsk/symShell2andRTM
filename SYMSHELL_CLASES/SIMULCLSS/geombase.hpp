/// @file
/// @brief **GEOMETRY — THE WAY OF ORGANIZING AGENTS OR DATA IN A LAYER.GEOMETRIA** /<br>
///         _GEOMETRIA — SPOSÓB ORGANIZACJI AGENTÓW LUB DANYCH W WARSTWIE._
/// @date 2026-05-18 (modified)
// *********************************************************************************************************************
//
#ifndef SYMSHELL2_GEOM_BASE_HPP_INCLUDED_
#define SYMSHELL2_GEOM_BASE_HPP_INCLUDED_

#include <cstddef>
#include <climits>
#include <cfloat>
#include <cstring>
#include <cassert>
#include <cstdint>
#include <iostream>

#include "wb_limits.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace shell {

/// @brief
/// @PL{ Typ uchwytowy do iteratorów geometrii. Idea trochę stara, ale wciąż lata... }
/// @EN{ A handle type for geometry iterators. A bit old, but still flying... }
typedef void* iterator_h;

/// @brief
/// @PL{ Typ uchwytowy do iteratorów geometrii. Jeszcze starsza wersja dla kompatybilności. }
/// @EN{ A handle type for geometry iterators. Even older version for backward compatibility. }
typedef iterator_h iteratorh;

/*
class geometry_base::iterator_base; // Pomysły na wersję zabezpieczoną???

class iterator_h
//----------------
{
    iterator_base* val;
    public:
    iterator_h():val(0){}
    explicit iterator_h(unsigned long Init):val((iterator_base*)Init){}
    void set(unsigned long Init){ val=(iterator_base*)Init; }
    operator iterator_base* () {return val;}
    operator void* () {return (void*)val;}
};
*/

/// @if POLSKI
/// @brief INTERFACE dla geometrii świata symulacji.
/// @details
///     Geometria jest obiektem, który potrafi opisać położenie agentów w warstwie
///     a także wzajemnie względem siebie i przetworzyć je na liniowy indeks tablicy.
/// @elseif ENGLISH
/// @brief INTERFACE for the geometry of the simulation world.
/// @details
///     Geometry is an object that can describe the position of agents in a layer
///     and relative to each other, and convert it into a linear array index.
/// @endif
class geometry_base 
//-----------------
{
public:
    typedef uintptr_t index_t; //!< Typ całkowity równoważny rozmiarem z typem wskaźnikowym.

    enum my_full:index_t { FULL=wbrtm::limit<index_t>::Max() }; //!< Wartość największego możliwego wskaźnika i indeksu.

//LOKALNE KLASY i STRUKTURY
//-------------------------------------

    /// Struktura (dawniej unia!) dla wyrażania współrzędnych.
    struct coord
    //---------------
    {
        //struct{double X, Y, Z, T, U, V}; //!< Miał być łatwy dostęp, ale z czasem okazało się, że kłopot.
        double C[6]{};
        /// Construction. All initial values are zeros.
        coord():C{0,0,0,0,0,0} {}
        /// @name Attribute accessors.
        /// @{
        double& X(){ return C[0];}
        double& Y(){ return C[1];}
        double& Z(){ return C[2];}
        double& T(){ return C[3];}
        double& U(){ return C[4];}
        double& V(){ return C[5];}
        /* TODO Kompilator ma niekiedy problem, którą wybrać — `const`, czy nie `const`. Może zmienić na setX itp?! */
        double X()const { return C[0];}
        double Y()const { return C[1];}
        double Z()const { return C[2];}
        double T()const { return C[3];}
        double U()const { return C[4];}
        double V()const { return C[5];}
        /// @}
    };

    /// Ograniczenia wartości współrzędnych.
    struct MD_info
    //-------------
    {
        coord min;	//!< Najmniejsze wartości dla każdej współrzędnej.
        coord max;	//!< Największe wartości dla każdej współrzędnej.
    };

    /// Informacja o ustawieniach "kamery".
    struct view_info
    //---------------
    {
        coord pos;	//!< Pozycja oka kamery.
        coord dia;	//!< Na jaką odległość chce widzieć kamera.
        coord sst;	//!< Sugerowany minimalny krok przesuwu.
    };

    /// @brief
    /// @PL{ Klasa bazowa dla wszystkich iteratorów geometrii. }
    /// @EN{ Base type for all geometry iterators. }
    class iterator_base
    //------------------
    {
    protected:
        index_t   items;	//!< Ile elementów — trzeba to ustawić!!!
        unsigned marker;	//!< Dla sprawdzania, że to faktycznie iterator.

    public:
        /// @name Optymalizacja alokacji!!!
        /// @details "Clang-Tidy: Declaration of 'operator new' has no matching declaration of 'operator delete' at the same scope"
        ///          <br> REALLY?
        /// @{

        /// Klasowy de-alokator dla iteratora.
        static void  operator delete (void* p, size_t s) noexcept;

        /// Klasowy alokator iteratora.
        static void* operator new (size_t s) noexcept;

        // Wersja delete, której szuka Clang-Tidy, ale jest sprzeczna z logiką rozwiązania.
        //static void  operator delete(void* p) noexcept;
        /// @}

        /// Konstruktor, poza tym, co robi niejawnie, ustawia też `marker`.
        explicit iterator_base(size_t i_ite):
                marker(0xfedcba00),items(i_ite){}

        /// Destruktor, na wszelki wypadek wirtualny. Zeruje `marker`.
        virtual	~iterator_base()	{ marker=0;}

        /// Sprawdza, czy to na pewno iterator. Np. dla asercji.
        bool is_iterator() const	{ return marker==0xfedcba00;}

        /// Podaje ile jest elementów do iteracji.
        index_t number_of_items() const { return items;}

        /// Implementacja pobrania następnego elementu.
        virtual void _next(const geometry_base&,index_t& ret,index_t& end)=0;
    };

private:
    int dimension;	//!< Liczba wymiarów geometrii — żeby móc użyć typu `coord`.

protected:
    /// Sprawdza, czy pointer do VMT i dimension są takie same.
    int _compare_geometry_base(geometry_base* second);

public:
    // METODY INFORMACYJNE:
    //=====================

    /// Informacja o liczbie wymiarów topologicznych geometrii. Może być 0, jeśli to pojęcie nie ma sensu (np. dla grafowych).
    int					get_dimension() const {return dimension;}

    /// Informacja o maksymalnej możliwej odległości. Potrzebna np. dla "Spatial correlation".
    virtual double		get_max_distance() const =0;

    /// Informacja o odległości dwóch obiektów o określonych indeksach. Dla "Spatial correlation" i ważenia oddziaływań.
    virtual double		get_distance(size_t first,size_t second) const =0;

    /// Informacja o rozmiarze użytecznej przestrzeni.
    virtual MD_info*	get_info(MD_info* pom=nullptr) const=0;

    /// Informacja o położeniu i zasięgu kamery. Np. dla `view_iteratora`.
    virtual view_info*	get_view_info(view_info* pom=nullptr) const=0;

    /// Ustawianie położenia i zasięgu kamery dla `view_iterator`. @returns 0, jeśli jest OK.
    virtual int			set_view_info(const view_info*)=0;

    // PORÓWNANIA I OPERATORY:
    //------------------------

    /// Funkcja porównania dwóch geometrii. Można przedefiniowywać, jeśli są powody!
    virtual int  compare(geometry_base& sec)
    {
        return _compare_geometry_base(&sec);
    }

    int operator == (geometry_base& sec)
    {
        return compare(sec)==0;
    }

    int operator != (geometry_base& sec)
    {
        return compare(sec)!=0;
    }

    // METODY TWORZĄCE ITERATORY DO PRZECHODZENIA GEOMETRII:
    //------------------------------------------------------

    /// Tworzy iterator po obszarze wizualizacji.
    virtual iterator_h	make_view_iterator() const=0;

    /// Tworzy iterator po całości. Alokuje operatorem "new", a do likwidacji należy używać `destroy_iterator`.
    virtual iterator_h	make_global_iterator() const=0;

    /// Tworzy globalny iterator monte-carlo. Do likwidacji należy używać `destroy_iterator`.
    virtual iterator_h	make_random_global_iterator(size_t how_many=-1) const=0;

    /// Tworzy iterator po sąsiadach. @param dist jako R we wszystkich kierunkach.
    virtual iterator_h	make_neighbour_iterator(size_t center, size_t dist=1) const=0;

    /// Tworzy losowy iterator po sąsiadach. @param dist jako R we wszystkich kierunkach.
    virtual iterator_h	make_random_neighbour_iterator(size_t center, size_t dist=1, size_t how_many=-1) const=0;

    // METODY UŻYWAJĄCE ITERATORÓW:
    //-----------------------------

    /// Jawna iteracja zwraca indeks do aktualnego agenta i przesuwa iterator.
    /// Zeruje iterator, jeśli koniec danych. Zwraca `FULL` jeżeli nie ma w tym miejscu agenta (missing).
    virtual index_t		get_next(iterator_h& p) const;

    /// Likwiduje już niepotrzebny iterator. Np. taki któremu nie dano dojść do końca.
    virtual void		destroy_iterator(iterator_h& p) const;

    // CONSTRUCTION/DESTRUCTION:
    //--------------------------

    /// Konstruktor z liczbą wymiarów. @param dims — liczba wymiarów.
    explicit geometry_base(int dims):dimension(dims){}

    virtual ~geometry_base()= default;

};

/// @PL{ Rezerwacja nazwy "geometry". }
/// @EN{ Reservation of the name "geometry". }
typedef geometry_base geometry;


// IMPLEMENTATIONS:
//=================

inline
int geometry_base::_compare_geometry_base(geometry_base* second)
{
    /// @internal
    ///  The second operand of this 'memcmp' call is a pointer to dynamic type 'geometry_base';
    ///  vtable pointer will be compared. (AND THIS IS INTENTIONAL!)
    return memcmp((void*)this, second, sizeof(geometry_base)); // NOLINT(*-suspicious-memory-comparison)
}

inline
geometry_base::index_t geometry_base::get_next(iterator_h& p) const
//Zwraca indeks do aktualnego i przesuwa iterator.
//Zeruje iterator, gdy koniec danych. Zwraca FULL, gdy missing...
{                 static_assert( sizeof(size_t) == sizeof(uintptr_t) ,"sizeof(size_t) must be equal sizeof(uintptr_t)");
    index_t ret=FULL; //TODO rozważyć użycie `uintptr_t`
    index_t end=0;
    
    iterator_base* pom=(iterator_base*)p; //NIEŁADNY CHWYT, ALE DZIAŁA
    //iterator_base* pom1=(iterator_base*)(&p); assert(pom1==pom); --> A TAK NIE DZIAŁA (
                                                                                      assert(pom!=nullptr);
                                                                                      assert(pom->is_iterator());

    //Właściwe odczytanie i przesuniecie iteratora
    pom->_next(*this,ret,end);
    if(end)
    {                                                                                 assert(pom->is_iterator());
        destroy_iterator(p); //Usuwamy iterator
    }

    return ret;
}


inline
void    geometry_base::destroy_iterator(iterator_h& p) const
//Likwiduje iterator, któremu nie dano dojść do końca.
{
    if(p==nullptr)
    {
        return; // Czy to się zdarza?
    }
    void* ptr=p; //Uwolnić samą wartość.
    iterator_base* pom=(iterator_base*)ptr ; //NIEŁADNY CHWYT, ALE DZIAŁA, choć dobrze sprawdzić, czy faktycznie
                                                                                     assert(pom->is_iterator());
    delete pom; //Usuwany iterator
    p=nullptr;
}

inline
double      geometry_base::get_max_distance() const
//Informacja o maksymalnej możliwej odległości. Potrzebna np. dla "Spatial correlation".
{
#ifdef _NDEBUG
    return 0;
#else
    assert("Pure virtual method 'geometry_base::get_max_distance() const' used."==0);
#endif
}

inline
double     geometry_base::get_distance(size_t first,size_t second) const
//Informacja o odległości dwóch obiektów o określonych indeksach.
{
#ifdef _NDEBUG
    return 0;
#else
    assert("Pure virtual method 'geometry_base::get_distance() const' used."==0);
#endif
}

}} //namespace sym2::shell

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
#endif //SYMSHELL2_GEOM_BASE_HPP_INCLUDED_


