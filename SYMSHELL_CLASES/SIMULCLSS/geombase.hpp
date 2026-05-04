/// @file
/// @brief GEOMETRIA — SPOSÓB ORGANIZACJI AGENTÓW W WARSTWIE./ GEOMETRY — THE WAY OF ORGANIZING AGENTS IN A LAYER.
/// @date 2026-05-04 (modified)
///     Geometria jest obiektem, który potrafi opisać położenie agentów w warstwie,
///     a także wzajemnie względem siebie i przetworzyć je na liniowy indeks tablicy.
// *********************************************************************************************************************
//
#ifndef __GEOMBASE_HPP__
#define __GEOMBASE_HPP__

#include <cstddef>
#include <climits>
#include <cfloat>
#include <cstring>
#include <cassert>
//#include "platform.hpp"
#include <cstdint>
#include <iostream>

/// Typ uchwytowy do iteratorów geometrii.
//---------------------------------------
typedef void* iteratorh; //Wersja stara, ale jara TODO NADAL???

/*
class geometry_base::iterator_base; //???Pomys�y na wersj� zabezpieczon�?
class iteratorh
//----------------
{
iterator_base* val;
public:
iteratorh():val(0){}
explicit iteratorh(unsigned long Init):val((iterator_base*)Init){}
void set(unsigned long Init){ val=(iterator_base*)Init;}
operator iterator_base* () {return val;}
operator void* () {return (void*)val;}
};
*/

/// INTERFACE dla geometrii świata symulacji.
class geometry_base 
//-----------------
{
public:
    enum my_full:uintptr_t {FULL=(UINTPTR_MAX)}; //!< Wartość największego możliwego wskaźnika. Zamiast #define FULL

    //LOKALNE KLASY i STRUKTURY
    //-------------------------------------

    /// Struktura (dawniej unia!) dla wyrażania współrzędnych.
    struct coord
    //---------------
    {
        //struct{double X,Y,Z,T,U,V;}; //!< Miał być łatwy dostęp, ale z czasem okazało się, że kłopot.
        double C[6]{};
        coord():C{0,0,0,0,0,0} {} //!< Initial values are zeros.
        double& X(){ return C[0];}
        double& Y(){ return C[1];}
        double& Z(){ return C[2];}
        double& T(){ return C[3];}
        double& U(){ return C[4];}
        double& V(){ return C[5];}
        /* TODO Kompilator ma niekiedy problem, którą wybrać — `const`, czynie `const`. Może zmienić na setX itp?! */
        double X()const { return C[0];}
        double Y()const { return C[1];}
        double Z()const { return C[2];}
        double T()const { return C[3];}
        double U()const { return C[4];}
        double V()const { return C[5];}
    };

    /// Ograniczenia wartości współrzędnych.
    struct MD_info
    //-------------
    {
        coord min;	//!< Najmniejsze wartości dla każdej współrzędnej.
        coord max;	//!< Największe wartości dla każdej współrzędnej.
    };

    struct view_info
    //---------------
    {
        coord pos;	//!< Pozycja oka kamery.
        coord dia;	//!< Na jaką odległość chce widzieć kamera.
        coord sst;	//!< Sugerowany minimalny krok przesuwu.
    };

    /// Klasa bazowa dla wszystkich iteratorów geometrii.
    class iterator_base
    //------------------
    {
    protected:
        size_t    items;	//!< Ile elementów — trzeba to ustawić!!!
        unsigned marker;	//!< Dla sprawdzania, że to faktycznie iterator.

    public:
        //Optymalizacja alokacji!!!
        //-------------------------
        void* operator new (size_t s);	//!< Klasowy alokator iteratora.
        void  operator delete (void* p, size_t s);	//!< Klasowy dealokator dla iteratora.

        /// Konstruktor, poza tym, co robi niejawnie, ustawia też `marker`.
        iterator_base(size_t iite):
                marker(0xfedcba00),items(iite){}

        /// Destruktor, na wszelki wypadek wirtualny. Zeruje `marker`.
        virtual	~iterator_base()	{ marker=0;}

        /// Sprawdza, czyto na pewno iterator. Np. dla asercji.
        int is_iterator() const	{ return marker==0xfedcba00;}

        /// Podaje ile jest elementów do iteracji.
        size_t number_of_items() const { return items;}

        /// Implementacja pobrania następnego elementu.
        virtual void _next(const geometry_base&,size_t& ret,size_t& end)=0;
    };

private:
    int dimension;	//!< Liczba wymiarów geometrii — żeby móc użyć typu `coord`.

protected:
    /// Sprawdza, czyVMT i dimension są takie same.
    int _compare_geometry_base(geometry_base* second);

public:
    // METODY INFORMACYJNE:
    // ////////////////////

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
    virtual iteratorh	make_view_iterator() const=0;

    /// Tworzy iterator po całości. Alokuje operatorem "new", a do likwidacji należy używać `destroy_iterator`.
    virtual iteratorh	make_global_iterator() const=0;

    /// Tworzy globalny iterator monte-carlo. Do likwidacji należy używać `destroy_iterator`.
    virtual iteratorh	make_random_global_iterator(size_t how_many=-1) const=0;

    /// Tworzy iterator po sąsiadach. @param dist jako R we wszystkich kierunkach.
    virtual iteratorh	make_neighbour_iterator(size_t center,size_t dist=1) const=0;

    /// Tworzy losowy iterator po sąsiadach. @param dist jako R we wszystkich kierunkach.
    virtual iteratorh	make_random_neighbour_iterator(size_t center,size_t dist=1,size_t how_many=-1) const=0;

    // METODY UŻYWAJĄCE ITERATORÓW:
    //-----------------------------

    /// Jawna iteracja zwraca indeks do aktualnego agenta i przesuwa iterator.
    /// Zeruje iterator, jeśli koniec danych. Zwraca `FULL` jeżeli nie ma w tym miejscu agenta (missing).
    size_t			get_next(iteratorh& p) const;

    /// Likwiduje już niepotrzebny iterator. Np. taki któremu nie dano dojść do konca.
    virtual void	destroy_iterator(iteratorh& p) const;

    // CONSTRUCTION/DESTRUCTION:
    //--------------------------

    /// Konstruktor z liczbą wymiarów. @param dims — liczba wymiarów.
    geometry_base(int dims):dimension(dims){}

    virtual ~geometry_base()= default;

};

/// Tymczasem nazwa "geometry" zarezerwowana...
typedef geometry_base geometry;


// IMPLEMENTATIONS:
// ////////////////

inline
int geometry_base::_compare_geometry_base(geometry_base* second)
//Sprawdza, czyVMT i dimension są takie same.
{
    return memcmp((void*)this, second, sizeof(geometry_base)); // NOLINT(*-suspicious-memory-comparison)
}

inline
size_t geometry_base::get_next(iteratorh& p) const
//Zwraca indeks do aktualnego i przesuwa iterator.
//Zeruje iterator, gdy koniec danych. Zwraca uFULL, gdy missing...
{                 static_assert( sizeof(size_t) == sizeof(uintptr_t) ,"sizeof(size_t) must be equal sizeof(uintptr_t)");
    size_t ret=FULL; //TODO rozważyć użycie `uintptr_t`
    size_t end=0; //
    
    iterator_base* pom=(iterator_base*)p; //NIEŁADNY CHWYT ALE DZIAŁA
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
void    geometry_base::destroy_iterator(iteratorh& p) const
//Likwiduje iterator, któremu nie dano dojść do końca.
{
    if(p==nullptr)
    {
        return; // Czy to się zdarza?
    }
    void* ptr=p; //Uwolnić samą wartość.
    iterator_base* pom=(iterator_base*)ptr ; //NIEŁADNY CHWYT, ALE DZIAŁA, choc dobrze sprawdzić, czyfaktycznie
                                                                                     assert(pom->is_iterator());
    delete pom; //Usuwany iterator
    p=nullptr;
}

inline
double      geometry_base::get_max_distance() const
//Informacja o maksymalnej możliwej odległości. Potrzebna np. dla "Spatial correlation".
{
    assert("Pure virtual method 'geometry_base::get_max_distance() const' used."==0);
    return 0;
}

inline
double     geometry_base::get_distance(size_t first,size_t second) const
//Informacja o odległości dwóch obiektów o określonych indeksach.
{
    assert("Pure virtual method 'geometry_base::get_distance() const' used."==0);
    return 0;
}

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


