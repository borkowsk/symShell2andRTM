/// @file
/// @brief **Definition of the base type and templates specialization of simulation layers ** /<br>
///         _Definicja bazy i szablonów warstw symulacji._
/// @date 2026-05-18 (modified)
// ********************************************************************************************************************
#ifndef SYMSHELL2_LAYER_HPP_INCLUDED_
#define SYMSHELL2_LAYER_HPP_INCLUDED_

#include "rectgeom.hpp"
#include "datasour.hpp"
#include "simpsour.hpp"
#include "filtsour.hpp"
#include "statsour.hpp"
#include "fifosour.hpp"
//#include "sourmngr.hpp" any_layer_base::const unsigned long FULL=UINT_MAX;

#include "wb_limits.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace shell {

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief Base types for all derived types of simulation layers.
class any_layer_base
#else
/// @brief Klasa bazowa dla wszystkich pochodnych typów warstw symulacji.
class any_layer_base
#endif
//---------------------------
{
public:
    /// @if POLISH
    /// @brief Typ indeksu dowolnego obiektu w warstwie.
    /// @note Wzięty z geometrii i aktualnie tożsamy z `size_t`, więc trudno wykryć niespójności.
    /// Niezależnie od geometrii warstwy każdy element powinien być też dostępny w iteracji liniowej.
    /// @elseif ENGLISH
    /// @brief The index type of any object in the layer.
    /// @note Taken from the geometry and currently identical to `size_t`, so inconsistencies are difficult to detect.
    /// Regardless of the layer geometry, each element should also be accessible in linear iteration.
    /// @endif
    typedef geometry_base::index_t lin_index_t;

    /// Największa wartość dowolnego indeksu — marker nieznalezienia itp. Kiedyś było `static const unsigned long FULL`.
    enum my_full:lin_index_t { FULL=limit<lin_index_t>::Max() }; //Zamiast #define FULL albo const full

    /// Wirtualny destruktor.
    virtual	~any_layer_base()= default;

    /// Rejestracja źródła/źródeł w zarządcy danych.
    /// W klasach specjalizowanych można zdefiniować automatyczna rejestracje.
    /// @return 0 oznacza brak automatycznej rejestracji. Inne oznaczają sukces.
    virtual int registry_sources(sources_manager_base&		Sources)
    {return 0;}

    /// Zwraca wskaźnik do geometrii. Nie wolno go z-delete-ować.
    virtual const geometry_base* get_geometry()=0;

    //virtual void swap(size_t index1,size_t index2)=0; //Zamienia ze soba dwa elementy
    //virtual void clean(size_t index)=0; //Czyści obiekt sposobem zdefiniowanym dla konkretnego typu warstwy

    /// @name Implementacja wejścia/wyjścia.
    /// @details Metody wirtualne zwracają 1, jeśli mają sukces.
    /// @{
    virtual
    int		implement_output(ostream& o) const=0;

    virtual
    int		implement_input(istream& i)=0;

    //i samych operatorów strumieniowych
    friend
    ostream& operator << (ostream& o,const any_layer_base& w);

    friend
    istream& operator >> (istream& i,any_layer_base& w);
    /// @}
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief A layer template for elements of a given type. Used to enforce the `get` method.
template<class TYPE>
class layer:public any_layer_base
#else
/// @brief Szablon warstwy elementów o zadanym typie. Służy do wymuszenia metody `get`.
template<class TYPE>
class layer:public any_layer_base
#endif
{
public:
    /// Akcesor dający dostęp do elementu o indeksie obliczonym przez geometrie.
    virtual TYPE& get(lin_index_t index)=0;
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief A type implementing the properties typical of a rectangular layer.
/// @details Designed for multiple inheritance, so it does not inherit from `layer`.
class rectangle_layer
#else
/// @brief Klasa implementująca własności typowe dla warstwy prostokątnej.
/// @details Przeznaczona do wielodziedziczenia, dlatego nie dziedziczy po `layer`.
/// TODO powinna się może inaczej nazywać? Np. ze słowem "implementation"?
class rectangle_layer
#endif
//---------------------
{
protected:
    rectangle_geometry		MainGeometry; ///< Geometria dla operacji na tej warstwie.
    //rectangle_geometry	VisoGeometry; ///< Geometria dla serii danych - w celu ich wizualizacji. TODO POMYSŁ PORZUCONY?

public:
    /// @name AKCESORY ZALEŻNE OD WŁASNOŚCI PROSTOKĄTA
    /// @{

    /// Wczytanie pliku GIF, BMP lub XBM na warstwę. Plik musi mieć rozmiar zgodny z rozmiarem warstwy.
    int init_from_bitmap(const char* filename,void* user_data=NULL);

    /// Przypisanie elementowi warstwy wartości RGB np. pobranej z bitmapy.
    virtual
    void assign_rgb(size_t TargetX,size_t TargetY,
                    unsigned char Red,
                    unsigned char Green,
                    unsigned char Blue,
                    void* user_data=0
                    )=0;

    /// Czyszczenie pojedynczego elementu — ze sprawdzaniem zakresu lub zawijaniem w torus.
    /// @note PURE VIRTUAL NEED TO BE IMPLEMENTED!
    virtual void clean(size_t TargetX,size_t TargetY)=0;

    /// Zamiana elementów.
    virtual void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY)=0;

    /// Czyszczenie linii elementów w warstwie.
    void clean_line(int X1,int Y1,int X2,int Y2);

    /// Czyszczenie wiersza elementów w warstwie.
    void clean_horizontal(int X1,int Y1,size_t N);

    /// Czyszczenie koła  elementów w warstwie.
    void clean_circle(int X,int Y,size_t R);

    /// Czyszczenie losowo wybranych elementów.
    void clean_randomly(int how_many);

    /// Sprawdzenie, czy jest "aktywny" element w tym miejscu.
    /// @note PURE VIRTUAL NEED TO BE IMPLEMENTED!
    virtual bool filled(int X,int Y)=0;
    /// @}

// Akcesory i metody ogólne
//================================

    const rectangle_geometry* get_rect_geometry()
    { return &MainGeometry; }

    //virtual rectangle_source_base* make_source(const char* name)=0; //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwę.

    /// @name KONSTRUKTOR i DESTRUKTOR.
    /// @{

    /// Konstruktor przede wszystkim ustawia geometrię warstwy.
    rectangle_layer(size_t Width,
                    size_t Height):
                    MainGeometry(Width,Height)
                    //VisoGeometry(Width,Height)
    {}

    virtual ~rectangle_layer()= default;

    /// Przywrócenie do nowości. Nie nazwałem `reset` bo to by mogło być niebezpieczne.
    /// @return `false`, jeśli nie ma takiej możliwości lub coś nie wyszło.
    virtual	bool Reinitialise()=0;
    /// @}
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief Rectangular layer template for any scalar (non-pointer!) type.
/// @details Interface taken from `layer<>` and implementation from `rectangle_layer`.
#else
/// @brief Szablon warstwy prostokątnej dla dowolnego typu skalarnego (nie-wskaźnikowego!).
/// @details Interface bierze z `layer<>` a implementację z `rectangle_layer`.
#endif
template<class SCALAR>
class rectangle_unilayer:public layer<SCALAR>,public rectangle_layer
//----------------------------------------------
{
    wb_dynarray<SCALAR> table; //!< Do przechowywania elementów.
    SCALAR            cleaner; //!< "Obiekt" do zamazywania elementów.

public:
    /// Constructor.
    /// @param Width to oczywiście szerokość warstwy, czyli liczba kolumn.
    /// @param Height to oczywiście wysokość warstwy, czyli liczba wierszy.
    /// @param iclean to element wzorcowy dla elementów skasowanych/pustych.
    rectangle_unilayer(size_t Width, size_t Height,
                       const SCALAR& iclean
            ):
            rectangle_layer(Width,Height),
            table(Width*Height),		//odpowiednia ilość elementów.
            cleaner(iclean)
        {
            size_t N=table.get_size();
            for(size_t i=0;i<N;i++)
                table[i]=cleaner; //Każdy zostanie zainicjalizowany "na pusto".
        }

    ~rectangle_unilayer() override= default;

    /// Zmiana cleaner-a.
    /// @param i_cleaner to nowy "element" do zamazywania.
    void set_cleaner(SCALAR i_cleaner)
    {
        cleaner=i_cleaner;
    }

    /// Przywrócenie do nowości. Nie nazwałem `reset` bo to by mogło być niebezpieczne.
    bool Reinitialise() override
    {
        size_t N=table.get_size();
        for(size_t i=0;i<N;i++)
        {
            table[i]=cleaner;		//assign!
        }
        return true; //OK
    }

// Metody Pure-virtual, które muszą zostać zdefiniowane dla każdej warstwy
//==========================================================================

    const geometry_base* get_geometry()
    //Wypełnienie obowiązku pure-virtual
    { return &MainGeometry;}

    /// Daje dostęp do elementu o indeksie obliczonym przez geometrie.
    SCALAR& get(geometry_base::index_t index)
    { return table[index]; }

    /// @name Metody specyficzne dla warstw prostokątnych.
    /// @{

    SCALAR&	get(size_t X,size_t Y)
    //Bezpośredni dostęp do pola wg. współrzędnych prostokątnych.
    {
        size_t lindex=MainGeometry.get(X,Y);
        assert(lindex!=rectangle_geometry::FULL); //Jedyne sprawdzanie zakresów, żeby nie spowalniać przetestowanej symulacji.
        return table[lindex];
    }

    /// Czyszczenie pojedynczego pola, konieczne, bo pure-virtual.
    /// @note Takiej metody nie można użyć bezpośrednio do wskaźników.
    void clean(size_t TargetX,size_t TargetY) override
    { get(TargetX,TargetY)=cleaner;}

    /// Zamiana elementów.
    void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY) override

    {
        SCALAR& Target=get(TargetX,TargetY);
        SCALAR& Source=get(SourceX,SourceY);
        SCALAR& tmp=Target;
        Target=Source;
        Source=tmp;
    }

    /// Sprawdzenie, czy jest agent w tym miejscu. Coś zawsze jest, bo to skalary, ale może powinien porównywać z "cleaner"?
    bool filled(int X,int Y) override
    {
        return true;
    }

    /// Przypisanie elementowi wartości RGB. Zazwyczaj z bitmapy — domyślnie przekształcone na szarość.
    void assign_rgb(size_t TargetX,size_t TargetY,
                    unsigned char Red,unsigned char Green,unsigned char Blue,
                    void* user_data=0) override
    {
        //Uproszczone, bo możnaby zastosować specjalny wzór z wagami,
        unsigned long pom=(unsigned long)Red+(unsigned long)Green+(unsigned long)Blue;
        pom/=3; //Średnia intensywność — w zakresie 0..255
        get(TargetX,TargetY)=(unsigned char)pom;
    }
    /// @}

    /// Tworzenie źródła do czytania danych.
    /// Źródło dostaje tylko informacje o szerokości i wysokości, więc ma własną niezależną geometrię.
    virtual /*rectangle_source_base**/
    matrix_source<SCALAR>* make_source(const char* name)	//Tworzy zawsze/wielokrotnie taka sama, ale nie tę samą warstwę
    {
        return new matrix_source<SCALAR>(name,MainGeometry.get_width(),
                                         MainGeometry.get_height(),
                                         table.get_ptr_val());
    }

    // Alternatywne.
    // Źródło dostaje wskaźnik do geometrii, dzięki czemu dane moga być potem we właściwy sposób wizualizowane.
    // `matrix_source<SCALAR_T>* make_source(const char* name)`

    /// @name Implementacja wejścia/wyjścia. Zwracają 1, jeśli sukces!
    /// @{
    int		implement_output(ostream& o) const
    {
        o<<table<<' '<<cleaner; return 1;
    }

    int		implement_input(istream& i)
    {
        i>>table;
        i>>cleaner;
        if(i.fail())
            return 0;
        return 1;
    }
    /// @}
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// @brief Rectangular layer template for any structure type.
#else
/// @brief Szablon warstwy prostokątnej dla dowolnego typu strukturalnego.
#endif
template<class STRUCT_T>
class rectangle_layer_of_struct:public layer<STRUCT_T>,public rectangle_layer
//----------------------------------------------
{
    wb_dynarray<STRUCT_T> table; //!< Prosty kontener na dane.

public:
    /// Constructor.
    /// @param Width, Height to wymiary prostokąta.
    /// Zakładamy, że do inicjalizacji wystarcza to, co robi bezparametrowy konstruktor struktury.
    rectangle_layer_of_struct(
                    size_t Width,
                    size_t Height
                    )
    : rectangle_layer(Width,Height), table(Width*Height)	//odpowiednia ilość pół.
    {}

    //Empty constructor for reading?
    //rectangle_layer_of_struct(){}

    /// Destructor.
    ~rectangle_layer_of_struct() override= default;

    /// Przywrócenie do nowości. Nie nazwałem `reset` bo to by mogło być niebezpieczne.
    bool Reinitialise() override
    {
        size_t N=table.get_size();
        STRUCT_T pom;			//construction here!!!
        for(size_t i=0;i<N;i++)
        {
            table[i]=pom;		//assign!
        }
        return true;
    }

// Metody Pure-virtual, które muszą zostać zdefiniowane dla każdej warstwy
//==========================================================================

    const geometry_base* get_geometry()
    //Wypełnienie obowiązku pure-virtual
    { return &MainGeometry;}

    STRUCT_T& get(geometry_base::index_t index)
    //Daje dostęp do elementu o indeksie obliczonym przez geometrie
    { return table[index]; }

// Metody specyficzne dla warstw prostokątnych
//==========================================================================

    /// Bezpośredni dostęp do pola.
    STRUCT_T&	get(size_t X,size_t Y)

    {
        size_t lindex=MainGeometry.get(X,Y);
        assert(lindex!=rectangle_geometry::FULL); //Jedyne sprawdzanie zakresów, żeby nie spowalniać przetestowanej symulacji
        return table[lindex];
    }

    /// Czyszczenie pojedynczego pola bezparametrowym konstruktorem.
    void clean(size_t TargetX,size_t TargetY) override
    {
        get(TargetX,TargetY)=STRUCT_T();
    }

    /// Zamiana elementów używająca `memcpy`, żeby unikać konstruowania.
    void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY) override
    {
        STRUCT_T& Target=get(TargetX,TargetY);
        STRUCT_T& Source=get(SourceX,SourceY);
        char tmp[sizeof(STRUCT_T)];
        //Powinna być opcja rozwijania takich funkcji jak memcpy.
        memcpy(tmp,&Target,sizeof(STRUCT_T));
        memcpy(&Target,&Source,sizeof(STRUCT_T));
        memcpy(&Source,&tmp,sizeof(STRUCT_T));
    }

    /// Sprawdzenie, czy jest agent w tym miejscu.
    /// Struktura jest zawsze. Jak może nie być to trzeba użyć warstwy innego typu.
    bool filled(int X,int Y) override
    {
        return true;
    }

    /// @name Implementacja funkcji `init_from_bitmap` wymaga pomocniczej klasy.
    /// @{

    /// Wskaźnik do komponentu funkcyjnego struktury elementu.
    typedef void (STRUCT_T::* assign_rgb_fun)(unsigned char,unsigned char,unsigned char);

    /// Struktura przechowująca wskaźnik do komponentu funkcyjnego struktury elementu.
    struct assign_rgb_stc
    {
        assign_rgb_fun AssFun;
        explicit assign_rgb_stc(assign_rgb_fun par):AssFun(par){}
    };

    /// Inicjalizacja warstwy z bitmapy za pomocą struktury funkcyjnej `assign_rgb_stc`.
    int init_from_bitmap(const char* filename,
                         assign_rgb_stc user_fun)
    {
        return rectangle_layer::init_from_bitmap(filename,&user_fun);
    }

    /// Przypisanie polu wartości RGB z bitmapy — domyślnie przekształcone na szarość.
    void assign_rgb(size_t TargetX,size_t TargetY,		//też konieczne bo pure-virtual
                    unsigned char Red,unsigned char Green,unsigned char Blue,
                    void* user_data=0
                    ) override
    {
        assign_rgb_fun AssignFun=((assign_rgb_stc*)user_data)->AssFun;
        (get(TargetX,TargetY).*AssignFun)(Red,Green,Blue);
    }
    /// @}

    /// @name Tworzenie źródeł do czytania danych.
    /// @details Najlepiej, gdyby to był szablon, ale to było jeszcze niestandardowe C++ gdy kod powstawał.
    ///          Tworzy zawsze/wielokrotnie takie samo, ale nie to samo źródło funkcyjne.
    /// @{

    /// Warstwa danych, gdy typ pola jest `short`.
    virtual
    struct_matrix_source<STRUCT_T,short>* make_source(const char* name,short STRUCT_T::* field_ptr)
    {
        return new struct_matrix_source<STRUCT_T,short>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    /// Warstwa danych, gdy typ pola jest `bool`.
    virtual
    struct_matrix_source<STRUCT_T,bool>* make_source(const char* name,bool STRUCT_T::* field_ptr)
    {
        return new struct_matrix_source<STRUCT_T,bool>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    /// Warstwa danych, gdy typ pola jest `unsigned`.
    virtual
    struct_matrix_source<STRUCT_T,unsigned>* make_source(const char* name,unsigned STRUCT_T::* field_ptr)
    {
        return new struct_matrix_source<STRUCT_T,unsigned>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    /// Warstwa danych, gdy typ pola jest `unsigned short`.
    virtual
    struct_matrix_source<STRUCT_T,unsigned short>* make_source(const char* name,unsigned short STRUCT_T::* field_ptr)
    {
        return new struct_matrix_source<STRUCT_T,unsigned short>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    /// Warstwa danych, gdy typ pola jest `unsigned char`.
    virtual
    struct_matrix_source<STRUCT_T,unsigned char>* make_source(const char* name,unsigned char STRUCT_T::* field_ptr)
    {
        return new struct_matrix_source<STRUCT_T,unsigned char>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    /// Warstwa danych, gdy typ pola jest `double`.
    virtual
    struct_matrix_source<STRUCT_T,double>* make_source(const char* name,double STRUCT_T::* field_ptr)
    {
        return new struct_matrix_source<STRUCT_T,double>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    /// Warstwa danych, gdy typ pola jest `double`, ale dostęp przez metodę.
    virtual
    method_matrix_source<STRUCT_T,double>* make_source(const char* name,double (STRUCT_T::* method_ptr)() )
    {
        return new method_matrix_source<STRUCT_T,double>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }

    /// Warstwa danych, gdy typ pola jest `short int`, ale dostęp przez metodę.
    virtual
    method_matrix_source<STRUCT_T,short int>* make_source(const char* name,short int (STRUCT_T::* method_ptr)())
    {
        return new method_matrix_source<STRUCT_T,short int>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }

    /// Warstwa danych, gdy typ pola jest `bool`, ale dostęp przez metodę.
    virtual
    method_matrix_source<STRUCT_T,bool>* make_source(const char* name,bool (STRUCT_T::* method_ptr)())
    {
        return new method_matrix_source<STRUCT_T,bool>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }

    /// Warstwa danych, gdy typ pola jest `int`, ale dostęp przez metodę.
    virtual
    method_matrix_source<STRUCT_T,int>* make_source(const char* name,int (STRUCT_T::* method_ptr)())
    {
        return new method_matrix_source<STRUCT_T,int>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }

    /// Warstwa danych, gdy typ pola jest `long`, ale dostęp przez metodę.
    virtual
    method_matrix_source<STRUCT_T,long>* make_source(const char* name,long (STRUCT_T::* method_ptr)())
    {
        return new method_matrix_source<STRUCT_T,long>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }

    /// Warstwa danych, gdy typ pola jest `unsigned`, ale dostęp przez metodę.
    virtual
    method_matrix_source<STRUCT_T,unsigned>* make_source(const char* name,unsigned (STRUCT_T::* method_ptr)())
    {
        return new method_matrix_source<STRUCT_T,unsigned>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }

    /// Warstwa danych, gdy typ pola jest `unsigned long`, ale dostęp przez metodę.
    virtual method_matrix_source<STRUCT_T,unsigned long>* make_source(const char* name,unsigned long (STRUCT_T::* method_ptr)())
    {
        return new method_matrix_source<STRUCT_T,unsigned long>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }
    /// @}

    /// @name Implementacja wejścia/wyjścia. Zwracają 1, jeśli sukces!
    /// @{
    int		implement_input(istream& i)
    {
        i>>table;

        if(i.fail())
            return 0;
        return 1;
    }

    int		implement_output(ostream& o) const
    {
        o<<table;
        if(o.fail())
            return 0;
        return 1;
    }
    /// @}
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// Definition of the agent interface that must be met for the layers to be able to support.
#else
/// Definicja interfejsu agenta, który musi być spełniony, żeby warstwy mogły obsługiwać.
#endif
class agent_base
//--------------------------------------------------
{
public:
    agent_base()= default;
    agent_base(const agent_base& ini)= default;

    /// Destruktor musi być wirtualny.
    virtual ~agent_base()= default;

    /// Funkcja czyszczenia musi być dostarczona.
    virtual void clean()=0;

    //agent_base* clone() const { return new agent(*this);}
    //friend ostream& operator << (ostream& o, agent a)
    //friend istream& operator >> (ostream& i, agent a)
}; //!< Prosty kontener na dane.

#ifdef USE_ENGLISH_IF_POSSIBLE
/// A layer template of a structured type compatible with the `agent_base` type.
#else
/// Szablon warstwy typu strukturalnego zgodnego z typem `agent_base`.
#endif
template<class AGENT>
class rectangle_layer_of_agents:public layer<AGENT>,public rectangle_layer
//------------------------------------------------------------------------
{
    wb_dynarray<AGENT>		  table; //!< Prosty kontener na dane.
    AGENT					cleaner; //!< Obiekt do zamazywania.
    int					use_cleaner; //!< Czy potrzebne jest użycie `cleaner`-a,
                                     //!<, czy wystarczy konstruktor bezparametrowy i/albo `clean`.
public:
    /// Constructor 1.
    /// @param Width, Height to wymiary prostokąta.
    /// Zakładamy, że wystarcza to, co robi bezparametrowy konstruktor agenta.
    rectangle_layer_of_agents(
                size_t Width,
                size_t Height
                )
    : rectangle_layer(Width,Height), table(Width*Height),		//odpowiednia ilość pól
      use_cleaner(0), cleaner()
    {}

    /// Constructor 2.
    /// @param Width, Height to wymiary prostokąta.
    /// @param i_use_cleaner oznacza czy używamy (1) czy nie używamy obiektu czyszczącego.
    /// @param i_clean - adres do obiektu czyszczącego.
    /// Obiekt czyszczący miał być przekazywany przez adres, żeby można oznaczać brak, ale to był ryzykowny pomysł.
    rectangle_layer_of_agents(
                size_t Width,
                size_t Height,
                int   i_use_cleaner,
                const AGENT* i_clean
            )
    : rectangle_layer(Width,Height),
      table(Width*Height),		//odpowiednia ilość pól
      use_cleaner(i_use_cleaner), cleaner(*i_clean)
    {
        if(use_cleaner)	//Na wypadek, gdy konstruktor nie wystarcza
        {
            size_t N=Width*Height;
            for(size_t i=0;i<N;i++)
                table[i]=cleaner;
        }
    }

    //Empty constructor ???
    //rectangle_layer_of_agents(){}

    bool Reinitialise() override
    {
        if(use_cleaner)	//Na wypadek, gdy konstruktor nie wystarcza
        {
            size_t N=table.get_size();
            for(size_t i=0;i<N;i++)
                table[i]=cleaner;
        }
        else
        {
            size_t N=table.get_size();
            for(size_t i=0;i<N;i++)
            {
                AGENT pom;		//construction!!!
                table[i]=pom;	//assign!
            }
        }
        return true;
    }

// Metody Pure-virtual, które muszą zostać zdefiniowane dla każdej warstwy
//==========================================================================

    /// Wskaźnik do obowiązującej geometrii warstwy. Wypełnienie obowiązku pure-virtual
    const geometry_base* get_geometry()
    { return &MainGeometry;}

    /// Daje dostęp do elementu o indeksie obliczonym przez geometrie.
    AGENT& get(size_t index)
    { return table[index]; }

// Metody specyficzne dla warstw prostokątnych
//==========================================================================

    /// Bezpośredni dostęp do pola.
    AGENT&	get(size_t X,size_t Y)
    {
        size_t lindex=MainGeometry.get(X,Y);
        assert(lindex!=rectangle_geometry::FULL); //Jedyne sprawdzanie zakresów, żeby nie spowalniać przetestowanej symulacji
        return table[lindex];
    }

    AGENT& operator () (size_t X,size_t Y)
    {
        return get(X,Y);
    }

    /// Czyszczenie pojedynczego pola. Konieczne, bo pure-virtual.
    void clean(size_t TargetX,size_t TargetY) override
    {
        if(use_cleaner)
            get(TargetX,TargetY)=cleaner;
        else
            get(TargetX,TargetY).clean();
    }

    /// Zamiana elementów używa `memcpy`, żeby omijać konstrukcje/destrukcje.
    void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY) override
    {
        AGENT& Target=get(TargetX,TargetY);
        AGENT& Source=get(SourceX,SourceY);
        char tmp[sizeof(AGENT)];
        //Powinna być opcja rozwijania takich funkcji jak memcpy.
        memcpy(tmp,&Target,sizeof(AGENT));
        memcpy(&Target,&Source,sizeof(AGENT));
        memcpy(&Source,&tmp,sizeof(AGENT));
    }

    /// Sprawdzenie, czy jest "żywy" agent w tym miejscu.
    /// Agent musi mieć funkcję `is_alive`.
    bool filled(int X,int Y) override
    {
        AGENT& Target=get(X,Y);
        return Target.is_alive();
    }

    /// @name Przykrycie funkcji init_from_bitmap
    /// @{
    typedef void (AGENT::* assign_rgb_fun)(unsigned char,unsigned char,unsigned char);

    /// @brief @PL{ Pomocnicza struktura wykonująca przypisanie RGB. }
    ///        @EN{ A helper structure that performs RGB assignment. }
    struct assign_rgb_stc
    {
        assign_rgb_fun AssFun;
        assign_rgb_stc(assign_rgb_fun par):AssFun(par){} // NOLINT(*-explicit-constructor)
    };

    int init_from_bitmap(const char* filename,
                         assign_rgb_stc user_fun)
    {
        return rectangle_layer::init_from_bitmap(filename,&user_fun);
    }

    /// Przypisanie polu wartości RGB. Zwykle z bitmapy — domyślnie przekształcone na szarość.
    void assign_rgb(size_t TargetX,size_t TargetY,		//też konieczne bo pure-virtual
                    unsigned char Red,unsigned char Green,unsigned char Blue,
                    void* user_data=0
                    ) override
    {
    assign_rgb_fun AssignFun=((assign_rgb_stc*)user_data)->AssFun;
    (get(TargetX,TargetY).*AssignFun)(Red,Green,Blue);
    }
    /// @}

    /// @name Tworzenie źródeł do czytania danych.
    /// @details Najlepiej, gdyby to był szablon, ale to było jeszcze niestandardowe C++ gdy kod powstawał.
    ///          Tworzy zawsze/wielokrotnie takie samo, ale nie to samo źródło funkcyjne.
    /// @{

    virtual /*rectangle_source_base**/
    struct_matrix_source<AGENT,bool>* make_source(const char* name,bool AGENT::* field_ptr)
    {
        return new struct_matrix_source<AGENT,bool>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    virtual /*rectangle_source_base**/
    struct_matrix_source<AGENT,short>* make_source(const char* name,short AGENT::* field_ptr)
    {
        return new struct_matrix_source<AGENT,short>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    virtual /*rectangle_source_base**/
    struct_matrix_source<AGENT,unsigned short>* make_source(const char* name,unsigned short AGENT::* field_ptr)
    {
        return new struct_matrix_source<AGENT,unsigned short>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    virtual /*rectangle_source_base**/
    struct_matrix_source<AGENT,int>* make_source(const char* name,int AGENT::* field_ptr)
    {
        return new struct_matrix_source<AGENT,int>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    virtual /*rectangle_source_base**/
    struct_matrix_source<AGENT,unsigned>* make_source(const char* name,unsigned AGENT::* field_ptr)
    {
        return new struct_matrix_source<AGENT,unsigned>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    virtual /*rectangle_source_base**/
    struct_matrix_source<AGENT,long>* make_source(const char* name,long AGENT::* field_ptr)
    {
        return new struct_matrix_source<AGENT,long>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    virtual /*rectangle_source_base**/
    struct_matrix_source<AGENT,unsigned long>* make_source(const char* name,unsigned long AGENT::* field_ptr)
    {
        return new struct_matrix_source<AGENT,unsigned long>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    virtual /*rectangle_source_base**/
    struct_matrix_source<AGENT,unsigned char>* make_source(const char* name,unsigned char AGENT::* field_ptr)
    {
        return new struct_matrix_source<AGENT,unsigned char>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    virtual /*rectangle_source_base**/
    struct_matrix_source<AGENT,double>* make_source(const char* name,double AGENT::* field_ptr)
    {
        return new struct_matrix_source<AGENT,double>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               field_ptr
                                               );
    }

    virtual
    method_matrix_source<AGENT,bool>* make_source(const char* name,bool (AGENT::* method_ptr)())
    {
        return new method_matrix_source<AGENT,bool>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }


    virtual
    method_matrix_source<AGENT,short>* make_source(const char* name,short (AGENT::* method_ptr)())
    {
        return new method_matrix_source<AGENT,short int>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }

    virtual
    method_matrix_source<AGENT,unsigned short>* make_source(const char* name,unsigned short (AGENT::* method_ptr)())
    {
        return new method_matrix_source<AGENT,unsigned short>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }

    virtual
    method_matrix_source<AGENT,int>* make_source(const char* name,int (AGENT::* method_ptr)())
    {
                                                    assert(name!=NULL);
                                                    assert(method_ptr!=NULL);
        return new method_matrix_source<AGENT,int>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }

    virtual
    method_matrix_source<AGENT,unsigned>* make_source(const char* name,unsigned (AGENT::* method_ptr)())
    {
        return new method_matrix_source<AGENT,unsigned>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }

    virtual
    method_matrix_source<AGENT,long>* make_source(const char* name,long (AGENT::* method_ptr)())
    {
        return new method_matrix_source<AGENT,long>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }

    virtual
    method_matrix_source<AGENT,unsigned long>* make_source(const char* name,unsigned long (AGENT::* method_ptr)())
    {
        return new method_matrix_source<AGENT,unsigned long>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }

    virtual
    method_matrix_source<AGENT,double>* make_source(const char* name,double (AGENT::* method_ptr)() )
    {
        return new method_matrix_source<AGENT,double>(
                                               name,
                                               MainGeometry,
                                               table.get_ptr_val(),
                                               method_ptr
                                               );
    }
    /// @}


    /// @name Implementacja wejścia/wyjścia. Zwracają 1, jeśli sukces!
    /// @{
    int		implement_input(istream& i)
    {
        i>>table;
        i>>cleaner;
        i>>use_cleaner;
        if(i.fail())
            return 0;
        return 1;
    }

    int		implement_output(ostream& o) const
    {
        o<<table;
        o<<' '<<cleaner;
        o<<' '<<use_cleaner<<' ';
        return 1;
    }
    /// @}
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// A template layer of pointers to a structured type compatible with `agent_base`.
#else
/// Szablon warstwy wskaźników do typu strukturalnego zgodnego z `agent_base`.
#endif
template<class AGENT>
class rectangle_layer_of_ptr_to_agents:public layer<AGENT>,public rectangle_layer
//--------------------------------------------------------------
{
    wb_dynarray<wb_ptr<AGENT> >			table; //!< Kontener na dane.
    wb_ptr<AGENT>				 model_object; //!< Obiekt do zamazywania.
    wb_ptr<AGENT>				  empty_guard; //!< Zwracany jako reprezentant pustych pól.
    int						  full_allocation; //!< Wszystkie wskaźniki mają być pełne.

public:
    /// Konstruktor.
    /// @param Width, Height to wymiary prostokąta.
    /// @param initializer to obiekt czyszczący przekazywany przez adres, żeby można oznaczać brak.
    rectangle_layer_of_ptr_to_agents(
        size_t Width,
        size_t Height,
        const  AGENT* initializer=NULL,
        int    allocate_all=0
        )
    : rectangle_layer(Width,Height),
      table(Width*Height),		//odpowiednia ilość pól
      model_object((initializer?initializer->clone():NULL)),
      empty_guard((initializer?initializer->clone():NULL)),
      full_allocation(allocate_all)
    {
        assert(sizeof(wb_ptr<AGENT>)==sizeof(AGENT*)); //Będzie taki cast w środku.
        if(allocate_all)	  //Na wypadek, gdy konstruktor nie wystarcza.
            reallocate_all(); //Realokuje lub klonuje wszystkie
        else
            deallocate_all(); //Wpisuje wszędzie NULL dla pewności
    }

    void reallocate_all()
    {
        size_t N=table.get_size();
        if(!model_object)
        {
            for(size_t i=0;i<N;i++)
                table[i]=new AGENT();
        }
        else
        {
            for(size_t i=0;i<N;i++)
                table[i]=model_object->clone();
        }
    }

    void deallocate_all()
    {
        size_t N=table.get_size();
        for(size_t i=0;i<N;i++)
            table[i]=NULL;
    }

    void deallocate_not_OK()
    {
        size_t N=table.get_size();
        for(size_t i=0;i<N;i++)
            if(table[i])
                if(!table[i]->IsOK()) //Jeśli nie jest OK to go kasuje
                    table[i]=NULL;
    }

    /// Na razie nie działająca, ale musi być.
    bool Reinitialise() override
    {
        return false;
    }

    // Metody Pure-virtual, które muszą zostać zdefiniowane dla każdej warstwy
    //==========================================================================

    const geometry_base* get_geometry()
    //Wypełnienie obowiązku pure-virtual
    {
        return &MainGeometry;
    }

    /// Daje dostęp do elementu o indeksie obliczonym przez geometrie.
    AGENT& get(size_t index)
    {
        //assert(index<);Sprawdzenie zakresu
                                            assert(index!=any_layer_base::FULL);
        if(!table[index]) //wb_ptr ma NULL
            return *empty_guard;
        else
            return *table[index];
    }

    int   is_empty(const AGENT& check)
    {
        return &check == empty_guard.get_ptr_val();
    }

    wb_ptr<AGENT>&	get_ptr(size_t index)
    //Dostęp do inteligentnego wskaźnika
    {                                                   assert(index!=any_layer_base::FULL);
        return table[index];
    }


    // Metody specyficzne dla warstw prostokątnych
    //==========================================================================

    AGENT&	get(size_t X,size_t Y)
    // Bezpośredni dostęp do struktury
    {
        size_t lindex=MainGeometry.get(X,Y);           assert(lindex!=any_layer_base::FULL); //Jedyne sprawdzanie zakresów, żeby nie spowalniać przetestowanej symulacji
        return *table[lindex];
    }

    AGENT& operator () (size_t X,size_t Y)
    {
        return get(X,Y);
    }

    /// Dostęp do (pół-) inteligentnego wskaźnika budującego warstwę.
    wb_ptr<AGENT>&	get_ptr(size_t X,size_t Y)
    {
        size_t lindex=MainGeometry.get(X,Y);
        assert(lindex!=any_layer_base::FULL); //Jedyne sprawdzanie zakresów, żeby nie spowalniać przetestowanej symulacji
        return table[lindex];
    }

    bool filled(int X,int Y) override
    {
        if(get_ptr(X,Y).OK()) //  !=NULL
            return true;
        else
            return false;
    }

    /// Czyszczenie pojedynczej struktury.
    void clean(size_t TargetX,size_t TargetY) override
    //Konieczne, bo pure-virtual,
    {
        if(full_allocation)
        {
            get_ptr(TargetX,TargetY)=model_object->clone();
        }
        else
        {
            get_ptr(TargetX,TargetY)=NULL; //Automatyczna destrukcja
        }
    }

    /// Zamiana elementów za pomocą wskaźników. Nie trzeba kopiować danych.
    void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY) override
    {
        wb_sptr<AGENT>& Target=get_ptr(TargetX,TargetY);
        wb_sptr<AGENT>& Source=get_ptr(SourceX,SourceY);
        wb_sptr<AGENT>  tmp;
        //Kopiuje wskaźniki, więc omija mechanizm konstrukcji/destrukcji.
        tmp=Target;		//Target jest teraz pusty,
        Target=Source;	//teraz Source jest pusty
        Source=tmp;		//a teraz tmp jest pusty, a Source i Target są zamienione
    }

    /// @name Przykrycie funkcji `init_from_bitmap`.
    /// @{
    typedef void (AGENT::* assign_rgb_fun)(unsigned char,unsigned char,unsigned char);

    /// @brief Struktura do przechowywania (?) funkcji wczytującej kolory.
    struct assign_rgb_stc
    {
        assign_rgb_fun AssFun;
        /// @note This constructor has to be without `explicit`.
        assign_rgb_stc(assign_rgb_fun par):AssFun(par){} // NOLINT(*-explicit-constructor)
    };

    int init_from_bitmap(const char* filename,
        assign_rgb_stc user_fun)
    {
        return rectangle_layer::init_from_bitmap(filename,&user_fun);
    }

    /// Przypisanie polu wartości RGB z bitmapy. Domyślnie przekształcone na szarość.
    void assign_rgb(size_t TargetX,size_t TargetY,		//też konieczne bo pure-virtual
        unsigned char Red,unsigned char Green,unsigned char Blue,void* user_data) override
    {
        assign_rgb_fun AssignFun=((assign_rgb_stc*)user_data)->AssFun;
        wb_ptr<AGENT>& Target=get_ptr(TargetX,TargetY);

        if(!Target)
        {
            if(!model_object)
                Target=new AGENT();
            else
                Target=model_object->clone();
        }
        assert(Target.OK());
        assert(AssignFun!=NULL);
        AGENT* pom=Target.get_ptr_val();
        (pom->*AssignFun)(Red,Green,Blue);
    }
    /// @}


    /// @name Tworzenie źródeł do czytania danych.
    /// @details Najlepiej, gdyby to był szablon, ale to było jeszcze niestandardowe C++ gdy kod powstawał.
    ///          Tworzy zawsze/wielokrotnie takie samo, ale nie to samo źródło funkcyjne.
    /// @{
    virtual
        ptr_to_struct_matrix_source<AGENT,unsigned>* make_source(const char* name,unsigned AGENT::* field_ptr)
    {
        return new ptr_to_struct_matrix_source<AGENT,unsigned>( name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            field_ptr
            );
    }

    virtual
        ptr_to_struct_matrix_source<AGENT,unsigned short>* make_source(const char* name,unsigned short AGENT::* field_ptr)
    {
        return new ptr_to_struct_matrix_source<AGENT,unsigned short>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            field_ptr
            );
    }

    virtual
        ptr_to_struct_matrix_source<AGENT,unsigned long>* make_source(const char* name,unsigned long AGENT::* field_ptr)
    {
        return new ptr_to_struct_matrix_source<AGENT,unsigned long>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            field_ptr
            );
    }

    virtual
        ptr_to_struct_matrix_source<AGENT,short>* make_source(const char* name,short AGENT::* field_ptr)
    {
        return new ptr_to_struct_matrix_source<AGENT,short>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            field_ptr
            );
    }


    virtual
        ptr_to_struct_matrix_source<AGENT,unsigned char>* make_source(const char* name,unsigned char AGENT::* field_ptr)
    {
        return new ptr_to_struct_matrix_source<AGENT,unsigned char>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            field_ptr
            );
    }

    virtual
        ptr_to_struct_matrix_source<AGENT,double>* make_source(const char* name,double AGENT::* field_ptr)
    {
        return new ptr_to_struct_matrix_source<AGENT,double>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            field_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,double>* make_source(const char* name,double (AGENT::* method_ptr)() )
    {
        return new method_by_ptr_matrix_source<AGENT,double>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,short>* make_source(const char* name,short (AGENT::* method_ptr)())
    {
        return new method_by_ptr_matrix_source<AGENT,short>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }


    virtual
        method_by_ptr_matrix_source<AGENT,int>* make_source(const char* name,int (AGENT::* method_ptr)())
    {
        return new method_by_ptr_matrix_source<AGENT,int>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,unsigned>* make_source(const char* name,unsigned (AGENT::* method_ptr)())
    {
        return new method_by_ptr_matrix_source<AGENT,unsigned>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,long int>* make_source(const char* name,long int (AGENT::* method_ptr)())
    {
        return new method_by_ptr_matrix_source<AGENT,long int>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,unsigned long>* make_source(const char* name,unsigned long (AGENT::* method_ptr)())
    {
        return new method_by_ptr_matrix_source<AGENT,unsigned long>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }
    /// @}

    /// @name Implementacja wejścia/wyjścia. Zwracają 1, jeśli sukces!
    /// @{
    int		implement_output(ostream& o) const
    {
        o << table << ' ' << model_object << ' '; //Obiekt do zamazywania
        o<<empty_guard<<' '<<full_allocation<<' '; //Zwracany jako reprezentant pustych pól
        return 1;
    }

    int		implement_input(istream& i)
    {
        i>>table;
        i >> model_object;
        i>>empty_guard;
        i>>full_allocation;
        if(i.fail())
            return 0;
        return 1;
    }
    /// @}
};

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
#endif


