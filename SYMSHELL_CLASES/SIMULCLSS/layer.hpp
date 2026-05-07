/// @file
/// @brief Definicja bazy i szablonów warstw symulacji. / Definition of the base class and simulation layer templates.
/// @date 2026-05-07 (modified)
///      ...
// ********************************************************************************************************************
#ifndef __LAYER_HPP__
#define __LAYER_HPP__

//#include "geombase.hpp"
#include "rectgeom.hpp"
#include "datasour.hpp"
#include "simpsour.hpp"
#include "filtsour.hpp"
#include "statsour.hpp"
#include "fifosour.hpp"
//#include "sourmngr.hpp" any_layer_base::const unsigned long FULL=UINT_MAX;
#include "wb_limits.hpp"

namespace symshell2 {

#ifdef USE_ENGLISH_IF_POSSIBLE
/// ...
#else
/// Klasa bazowa dla wszystkich typów warstw symulacji.
#endif
class any_layer_base
//---------------------------
{
public:
    /// Typ indeksu dowolnego obiektu w warstwie.
    /// @note Wzięty z geometrii i aktualnie tożsamy z size_t, więc trudno wykryć niespójności.
    /// Niezależnie od geometrii warstwy każdy element powinien być też dostepny w iteracji liniowej.
    typedef geometry_base::index_t lin_index_t;

    /// Najwieksza wartość dowolnego indeksu - marker nieznalezienia itp. Kiedyś było `static const unsigned long FULL;`.
    enum my_full:lin_index_t { FULL=limit<lin_index_t>::Max() }; //Zamiast #define FULL albo const full

    /// Wirtualny destruktor.
    virtual	~any_layer_base(){}

    /// Rejestracja źródła/źródeł w menagerze danych.
    /// W klasach specjalizowanych można zdefiniowac automatyczna rejestracje.
    /// @return 0 oznacza brak automatycznej rejestracji. Inne oznaczają sukces.
    virtual int registry_sources(sources_menager_base&		Sources)
    {return 0;}

    /// Zwraca wskaźnik do geometrii. Nie wolno go z-delet-ować.
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

    //i samych operatorow strumieniowych
    friend
    ostream& operator << (ostream& o,const any_layer_base& w);

    friend
    istream& operator >> (istream& i,any_layer_base& w);
    /// @}
};

#ifdef USE_ENGLISH_IF_POSSIBLE
/// ...
#else
/// Szablon warstwy elementów o zadanym typie. Służy do wymuszenia metody `get`.
#endif
template<class TYPE>
class layer:public any_layer_base
{
public:
    /// Akcesor dający dostęp do elementu o indeksie obliczonym przez geometrie.
    virtual TYPE& get(lin_index_t index)=0;
};

#ifdef USE_ENGLISH_IF_POSSIBLE
///
#else
/// Klasa implementująca własności typowe dla wartswy prostokątnej.
/// Przeznaczona do wielodziedziczenia, dlatego nie dziedziczy po layer.
/// TODO powinna się może inaczej nazywać? Np. ze słowem "implementation"?
#endif
class rectangle_layer
//---------------------
{
protected:
    rectangle_geometry		MainGeometry; //Geometria dla operacji na warstwie
    //rectangle_geometry	VisoGeometry; //Geometria dla serii danych - w celu ich wizualizacji. TODO POMYSŁ PORZUCONY?

public:
    /// @name AKCESORY ZALEZNE OD WLASNOSCI PROSTOKĄTA
    /// @{

    /// Wczytanie pliku GIF,BMP lub XBM na warstwe. Plik musi mieć rozmiar zgodny z rozmiarem warstwy.
    int init_from_bitmap(const char* filename,void* user_data=0);

    /// Przypisanie elementowi warstwy wartości RGB np. pobranej z bitmapy.
    virtual
    void assign_rgb(size_t TargetX,size_t TargetY,
                    unsigned char Red,
                    unsigned char Green,
                    unsigned char Blue,
                    void* user_data=0
                    )=0;

    /// Czyszczenie pojedynczego elementu - ze sprawdzaniem zakresu, lub zawijaniem w torus.
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

// Akcesory i metody ogolne
// ///////////////////////////////

    const rectangle_geometry* get_rect_geometry()
    { return &MainGeometry; }

    //virtual rectangle_source_base* make_source(const char* name)=0; //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe

    /// @name KONSTRUKTOR/DESTRUKTOR
    /// @{

    /// Konstruktor przede wszystkim ustawia geometrię warstwy.
    rectangle_layer(size_t Width,
                    size_t Height):
                    MainGeometry(Width,Height)
                    //VisoGeometry(Width,Height)
    {}

    virtual ~rectangle_layer(){}

    /// Przywrócenie do nowości. Nie nazwałem `reset` bo to by mogło być niebezpieczne.
    /// @return false jesli nie ma takiej możliwości lub cos nie wyszło.
    virtual	bool Reinitialise()=0;
    /// @}
};

#ifdef USE_ENGLISH_IF_POSSIBLE
///
#else
/// Szablon warstwy postokatnej dla dowolnego typu skalarnego (nie wskaźnikowego!).
/// Interface bierze z `layer<>` a implementację z `rectangle_layer`.
#endif
template<class SCALAR>
class rectangle_unilayer:public layer<SCALAR>,public rectangle_layer
//----------------------------------------------
{
    wb_dynarray<SCALAR> table; //!< Do przechowywania elementów.
    SCALAR            cleaner; //!< "Obiekt" do zamazywania elementów.

public:
    /// Constructor.
    /// @param Width to oczywiście szerokość warstwy czyli liczba kolumn.
    /// @param Height to oczywiście wysokość warstwy czyli liczba wierszy.
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
                table[i]=cleaner; //Kazdy zostanie zainicjalizowany "na pusto".
        }

    virtual ~rectangle_unilayer(){}

    /// Zmiana cleanera.
    /// @param icleaner to nowy "element" do zamazywania.
    void set_cleaner(SCALAR icleaner)
    {
        cleaner=icleaner;
    }

    /// Przywrócenie do nowości. Nie nazwałem `reset` bo to by mogło być niebezpieczne.
    virtual	bool Reinitialise()
    {
        size_t N=table.get_size();
        for(size_t i=0;i<N;i++)
        {
            table[i]=cleaner;		//assign!
        }
        return true; //OK
    }

// Metody Pure-virtual, które muszą zastac zdefiniowane dla kazdej warstwy
// /////////////////////////////////////////////////////////////////////////

    const geometry_base* get_geometry()
    //Wypełnienie obowiazku pure-virtual
    { return &MainGeometry;}

    /// Daje dostęp do elementu o indeksie obliczonym przez geometrie.
    SCALAR& get(geometry_base::index_t index)
    { return table[index]; }

    /// @name Metody specyficzne dla warstw prostokątnych.
    /// @{

    SCALAR&	get(size_t X,size_t Y)
    //Bezposredni dostęp do pola wg. współrzędnych prostokątnych.
    {
        size_t lindex=MainGeometry.get(X,Y);
        assert(lindex!=rectangle_geometry::FULL); //Jedyne sprawdzanie zakresow żeby nie spowalniać przetestowanej symulacji.
        return table[lindex];
    }

    /// Czyszczenie pojedynczego pola, konieczne bo pure-virtual.
    /// @note Takiej metody nie można uzyc bezposrednio do wskaźników.
    void clean(size_t TargetX,size_t TargetY)
    { get(TargetX,TargetY)=cleaner;}

    void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY)
    //Zamiana elementów
    {
        SCALAR& Target=get(TargetX,TargetY);
        SCALAR& Source=get(SourceX,SourceY);
        SCALAR& tmp=Target;
        Target=Source;
        Source=tmp;
    }

    bool filled(int X,int Y)
    //Sprawdzenie, czy jest agent w tym miejscu. Cos zawsze jest bo to skalary, ale może powinien porównywać z "cleaner"?
    {
        return true;
    }

    void assign_rgb(size_t TargetX,size_t TargetY,
                    unsigned char Red,unsigned char Green,unsigned char Blue,
                    void* user_data=0)
    // Przypisanie elementowi wartości RGB z bitmapy - domyślnie przeksztalcone na szarosc,
    {
        //Uproszczone - możnaby zastosowac specjalny wzor z wagami
        unsigned long pom=(unsigned long)Red+(unsigned long)Green+(unsigned long)Blue;
        pom/=3; //Srednia intensywnosc - w zakresie 0..255
        get(TargetX,TargetY)=(unsigned char)pom;
    }
    /// @}

    /// Tworzenie źródła do czytania danych.
    /// Źródło dostaje tylko informacje o szerokości i wysokości, więc ma własną niezależną geometrię.
    virtual /*rectangle_source_base**/
    matrix_source<SCALAR>* make_source(const char* name)	//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new matrix_source<SCALAR>(name,MainGeometry.get_width(),
                                         MainGeometry.get_height(),
                                         table.get_ptr_val());
    }

    // Alternatywne.
    // Źródło dostaje wskaźnik do geometrii, dzięki czemu dane moga być potem we właściwy sposób wizualizowane.
    // matrix_source<SCALAR>* make_source(const char* name)

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
/// ...
#else
/// Szablon warstwy prostokątnej dla dowolnego typu strukturalnego.
#endif
template<class STRUCT_T>
class rectangle_layer_of_struct:public layer<STRUCT_T>,public rectangle_layer
//----------------------------------------------
{
    wb_dynarray<STRUCT_T> table; //!< Prosty kontener na dane.

public:
    /// Constructor.
    /// @param Width, Height to wymiary prostokąta.
    /// Zakładamy ze do inicjalizacji wystarcza to co robi bezparametrowy konstruktor struktury.
    rectangle_layer_of_struct(
                    size_t Width,
                    size_t Height
                    ):
            rectangle_layer(Width,Height),
            table(Width*Height)	//odpowiednia ilość pół.
    {
        ;
    }

    //Empty constructor for reading?
    //rectangle_layer_of_struct(){}

    /// Destructor.
    ~rectangle_layer_of_struct(){}

    /// Przywrócenie do nowości. Nie nazwałem `reset` bo to by mogło być niebezpieczne.
    virtual	bool Reinitialise()
    {
        size_t N=table.get_size();
        STRUCT_T pom;			//construction here!!!
        for(size_t i=0;i<N;i++)
        {
            table[i]=pom;		//assign!
        }
        return true;
    }

// Metody Pure-virtual, które muszą zastac zdefiniowane dla kazdej warstwy
// /////////////////////////////////////////////////////////////////////////

    const geometry_base* get_geometry()
    //Wypełnienie obowiazku pure-virtual
    { return &MainGeometry;}

    STRUCT_T& get(geometry_base::index_t index)
    //Daje dostęp do elementu o indeksie obliczonym przez geometrie
    { return table[index]; }

// Metody specyficzne dla warstw prostokątnych
// /////////////////////////////////////////////////////////////////////////

    STRUCT_T&	get(size_t X,size_t Y)
    //Bezpośredni dostęp do pola
    {
        size_t lindex=MainGeometry.get(X,Y);
        assert(lindex!=rectangle_geometry::FULL); //Jedyne sprawdzanie zakresow żeby nie spowalniac przetestowanej symulacji
        return table[lindex];
    }

    /// Czyszczenie pojedynczego pola bezparametrowym konstruktorem.
    void clean(size_t TargetX,size_t TargetY)
    {
        get(TargetX,TargetY)=STRUCT_T();
    }

    /// Zamiana elementów używająca `memcpy`, żeby unikać konstruowania.
    void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY)
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
    bool filled(int X,int Y)
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
        assign_rgb_stc(assign_rgb_fun par):AssFun(par){}
    };

    /// Inicjalizacja warstwy z bitmapy za pomocą struktury funkcyjnej `assign_rgb_stc`.
    int init_from_bitmap(const char* filename,
                         assign_rgb_stc user_fun)
    {
        return rectangle_layer::init_from_bitmap(filename,&user_fun);
    }

    /// Przypisanie polu wartości RGB z bitmapy - domyślnie przekształcone na szarość,
    void assign_rgb(size_t TargetX,size_t TargetY,		//tez konieczne bo pure-virtual
                    unsigned char Red,unsigned char Green,unsigned char Blue,
                    void* user_data=0
                    )
    {
        assign_rgb_fun AssignFun=((assign_rgb_stc*)user_data)->AssFun;
        (get(TargetX,TargetY).*AssignFun)(Red,Green,Blue);
    }
    /// @}

    /// @name Tworzenie źródeł do czytania danych.
    /// @details Najlepiej gdyby to był szablon, ale to było jeszcze niestandardowe C++ gdy kod powstawał.
    ///          Tworzy zawsze/wielokrotnie takie samo, ale nie to samo źródło funkcyjne.
    /// @{

    /// Warstwa danych gdy typ pola jest `short`.
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

    /// Warstwa danych gdy typ pola jest `bool`.
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

    /// Warstwa danych gdy typ pola jest `unsigned`.
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

    /// Warstwa danych gdy typ pola jest `unsigned short`.
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

    /// Warstwa danych gdy typ pola jest `unsigned char`.
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

    /// Warstwa danych gdy typ pola jest `double`.
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

    /// Warstwa danych gdy typ pola jest `double`, ale dostęp przez metodę.
    virtual
    method_matrix_source<STRUCT_T,double>* make_source(const char* name,double (STRUCT_T::* method_ptr)(void) )
    {
    return new method_matrix_source<STRUCT_T,double>(
                                           name,
                                           MainGeometry,
                                           table.get_ptr_val(),
                                           method_ptr
                                           );
    }

    /// Warstwa danych gdy typ pola jest `short int`, ale dostęp przez metodę.
    virtual
    method_matrix_source<STRUCT_T,short int>* make_source(const char* name,short int (STRUCT_T::* method_ptr)(void))
    {
    return new method_matrix_source<STRUCT_T,short int>(
                                           name,
                                           MainGeometry,
                                           table.get_ptr_val(),
                                           method_ptr
                                           );
    }

    /// Warstwa danych gdy typ pola jest `bool`, ale dostęp przez metodę.
    virtual
    method_matrix_source<STRUCT_T,bool>* make_source(const char* name,bool (STRUCT_T::* method_ptr)(void))
    {
    return new method_matrix_source<STRUCT_T,bool>(
                                           name,
                                           MainGeometry,
                                           table.get_ptr_val(),
                                           method_ptr
                                           );
    }

    /// Warstwa danych gdy typ pola jest `int`, ale dostęp przez metodę.
    virtual
    method_matrix_source<STRUCT_T,int>* make_source(const char* name,int (STRUCT_T::* method_ptr)(void))
    {
    return new method_matrix_source<STRUCT_T,int>(
                                           name,
                                           MainGeometry,
                                           table.get_ptr_val(),
                                           method_ptr
                                           );
    }

    /// Warstwa danych gdy typ pola jest `long`, ale dostęp przez metodę.
    virtual
    method_matrix_source<STRUCT_T,long>* make_source(const char* name,long (STRUCT_T::* method_ptr)(void))
    {
    return new method_matrix_source<STRUCT_T,long>(
                                           name,
                                           MainGeometry,
                                           table.get_ptr_val(),
                                           method_ptr
                                           );
    }

    /// Warstwa danych gdy typ pola jest `unsigned`, ale dostęp przez metodę.
    virtual
    method_matrix_source<STRUCT_T,unsigned>* make_source(const char* name,unsigned (STRUCT_T::* method_ptr)(void))
    {
    return new method_matrix_source<STRUCT_T,unsigned>(
                                           name,
                                           MainGeometry,
                                           table.get_ptr_val(),
                                           method_ptr
                                           );
    }

    /// Warstwa danych gdy typ pola jest `unsigned long`, ale dostęp przez metodę.
    virtual method_matrix_source<STRUCT_T,unsigned long>* make_source(const char* name,unsigned long (STRUCT_T::* method_ptr)(void))
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
///
#else
/// Definicja interface'u agenta, który musi być spełniony żeby warstwy mogły obslugiwać/
#endif
class agent_base
//--------------------------------------------------
{
public:
    agent_base(){}
    agent_base(const agent_base& ini){}

    /// Destruktor musi być wirtualny.
    virtual ~agent_base(){}

    /// Funkcja czyszczenia musi być dostarczona.
    virtual void clean()=0;

    //agent_base* clone() const { return new agent(*this);}
    //friend ostream& operator << (ostream& o, agent a)
    //friend istream& operator >> (ostream& i, agent a)
};//!< Prosty kontener na dane.

#ifdef USE_ENGLISH_IF_POSSIBLE
///
#else
/// Szablon warstwy typu strukturalnego zgodnego z typem `agent_base`.
#endif
template<class AGENT>
class rectangle_layer_of_agents:public layer<AGENT>,public rectangle_layer
//----------------------------------------------
{
    wb_dynarray<AGENT>	      table; //!< Prosty kontener na dane.
    AGENT				    cleaner; //!< Obiekt do zamazywania.
    int					use_cleaner; //!< Czy potrzebne jest użycie cleanera,
                                     //!<, czy wystarczy konstruktor bezparametrowy i/albo `clean`.
public:
    /// Constructor 1.
    /// @param Width, Height to wymiary prostokąta.
    /// Zakładamy że wystarcza to co robi bezparametrowy konstruktor agenta.
    rectangle_layer_of_agents(
                size_t Width,
                size_t Height
                ):
            rectangle_layer(Width,Height),
            table(Width*Height),		//odpowiednia ilość pol
            use_cleaner(0),
            cleaner()
    {

        ;
    }

    /// Constructor 2.
    /// @param Width, Height to wymiary prostokąta.
    /// @param iusecleaner oznacza czy użwamy (1) czy nie używamy obiektu czyszczącego.
    /// @param iclean - adres do obiektu czyszczącego.
    /// Obiekt czyszczący miał być przekazywany przez adres, żeby można oznaczac brak, ale to był ryzykowny pomysł.
    rectangle_layer_of_agents(
                size_t Width,
                size_t Height,
                int   iusecleaner,
                const AGENT* iclean
        ):
        rectangle_layer(Width,Height),
        table(Width*Height),		//odpowiednia ilość pol
        use_cleaner(iusecleaner),
        cleaner(*iclean)
    {
        if(use_cleaner)	//Na wypadek gdy konstruktor nie wystarcza
        {
            size_t N=Width*Height;
            for(size_t i=0;i<N;i++)
                table[i]=cleaner;
        }
    }

    //Empty constructor ???
    //rectangle_layer_of_agents(){}

    bool Reinitialise()
    {
        if(use_cleaner)	//Na wypadek gdy konstruktor nie wystarcza
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

// Metody Pure-virtual, które muszą zastac zdefiniowane dla kazdej warstwy
// /////////////////////////////////////////////////////////////////////////

    const geometry_base* get_geometry()
    //Wypełnienie obowiazku pure-virtual
    { return &MainGeometry;}

    AGENT& get(size_t index)
    //Daje dostęp do elementu o indeksie obliczonym przez geometrie
    { return table[index]; }

// Metody specyficzne dla warstw prostokątnych
// /////////////////////////////////////////////////////////////////////////

    AGENT&	get(size_t X,size_t Y)
    //Bezposredni dostęp do pola
    {
    size_t lindex=MainGeometry.get(X,Y);
    assert(lindex!=rectangle_geometry::FULL); //Jedyne sprawdzanie zakresow żeby nie spowalniac przetestowanej symulacji
    return table[lindex];
    }

    AGENT& operator () (size_t X,size_t Y)
    {
        return get(X,Y);
    }

    void clean(size_t TargetX,size_t TargetY)
    //Czyszczenie pojedynczego pola//Konieczne bo pure-virtual
    {
    if(use_cleaner)
        get(TargetX,TargetY)=cleaner;
        else
        get(TargetX,TargetY).clean();
    }

    /// Zamiana elementów uzywa memecpy żeby omijać konstrukcje/destrukcje.
    void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY)
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
    bool filled(int X,int Y)
    {
        AGENT& Target=get(X,Y);
        return Target.is_alive();
    }

    /// @name Przykrycie funkcji init_from_bitmap
    /// @{
    typedef void (AGENT::* assign_rgb_fun)(unsigned char,unsigned char,unsigned char);

    struct assign_rgb_stc
    {
        assign_rgb_fun AssFun;
        assign_rgb_stc(assign_rgb_fun par):AssFun(par){}
    };

    int init_from_bitmap(const char* filename,
                         assign_rgb_stc user_fun)
    {
        return rectangle_layer::init_from_bitmap(filename,&user_fun);
    }

    // Przypisanie polu wartości RGB z bitmapy - domyślnie przeksztalcone na szarosc,
    void assign_rgb(size_t TargetX,size_t TargetY,		//tez konieczne bo pure-virtual
                    unsigned char Red,unsigned char Green,unsigned char Blue,
                    void* user_data=0
                    )
    {
    assign_rgb_fun AssignFun=((assign_rgb_stc*)user_data)->AssFun;
    (get(TargetX,TargetY).*AssignFun)(Red,Green,Blue);
    }
    /// @}

    /// @name Tworzenie źródeł do czytania danych.
    /// @details Najlepiej gdyby to był szablon, ale to było jeszcze niestandardowe C++ gdy kod powstawał.
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
    method_matrix_source<AGENT,bool>* make_source(const char* name,bool (AGENT::* method_ptr)(void))
    {
    return new method_matrix_source<AGENT,bool>(
                                           name,
                                           MainGeometry,
                                           table.get_ptr_val(),
                                           method_ptr
                                           );
    }


    virtual
    method_matrix_source<AGENT,short>* make_source(const char* name,short (AGENT::* method_ptr)(void))
    {
    return new method_matrix_source<AGENT,short int>(
                                           name,
                                           MainGeometry,
                                           table.get_ptr_val(),
                                           method_ptr
                                           );
    }

    virtual
    method_matrix_source<AGENT,unsigned short>* make_source(const char* name,unsigned short (AGENT::* method_ptr)(void))
    {
    return new method_matrix_source<AGENT,unsigned short>(
                                           name,
                                           MainGeometry,
                                           table.get_ptr_val(),
                                           method_ptr
                                           );
    }

    virtual
    method_matrix_source<AGENT,int>* make_source(const char* name,int (AGENT::* method_ptr)(void))
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
    method_matrix_source<AGENT,unsigned>* make_source(const char* name,unsigned (AGENT::* method_ptr)(void))
    {
    return new method_matrix_source<AGENT,unsigned>(
                                           name,
                                           MainGeometry,
                                           table.get_ptr_val(),
                                           method_ptr
                                           );
    }

    virtual
    method_matrix_source<AGENT,long>* make_source(const char* name,long (AGENT::* method_ptr)(void))
    {
    return new method_matrix_source<AGENT,long>(
                                           name,
                                           MainGeometry,
                                           table.get_ptr_val(),
                                           method_ptr
                                           );
    }

    virtual
    method_matrix_source<AGENT,unsigned long>* make_source(const char* name,unsigned long (AGENT::* method_ptr)(void))
    {
    return new method_matrix_source<AGENT,unsigned long>(
                                           name,
                                           MainGeometry,
                                           table.get_ptr_val(),
                                           method_ptr
                                           );
    }

    virtual
    method_matrix_source<AGENT,double>* make_source(const char* name,double (AGENT::* method_ptr)(void) )
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
///
#else
/// Szablon warstwy wskaźników do typu strukturalnego zgodnego z `agent_base`.
#endif
template<class AGENT>
class rectangle_layer_of_ptr_to_agents:public layer<AGENT>,public rectangle_layer
//--------------------------------------------------------------
{
    wb_dynarray<wb_ptr<AGENT> >	      table; //!< Kontener na dane.
    wb_ptr<AGENT>				     initer; //!< Obiekt do zamazywania
    wb_ptr<AGENT>				empty_guard; //!< Zwracany jako reprezentant pustych pol
    int						full_allocation; //!< Wszystkie wskaźniki mają być pełne.

public:
    /// Konstruktor.
    /// @param Width, Height to wymiary prostokąta.
    /// @param iiniter to obiekt czyszczacy przekazywany przez adres, żeby można oznaczac brak.
    rectangle_layer_of_ptr_to_agents(
        size_t Width,
        size_t Height,
        const  AGENT* iiniter=NULL,
        int    allocate_all=0
        ):
        rectangle_layer(Width,Height),
        table(Width*Height),		//odpowiednia ilość pol
        initer((iiniter?iiniter->clone():NULL)),
        empty_guard((iiniter?iiniter->clone():NULL)),
        full_allocation(allocate_all)
        {
            assert(sizeof(wb_ptr<AGENT>)==sizeof(AGENT*)); //Będzie taki cast w środku.
            if(allocate_all)	//Na wypadek gdy konstruktor nie wystarcza
                reallocate_all(); //Realokuje lub klonuje wszystkie
            else
                deallocate_all(); //Wpisuje wszedzie NULL dla pewnosci
        }

    void reallocate_all()
    {
        size_t N=table.get_size();
        if(!initer)
        {
            for(size_t i=0;i<N;i++)
                table[i]=new AGENT();
        }
        else
        {
            for(size_t i=0;i<N;i++)
                table[i]=initer->clone();
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
    bool Reinitialise()
    {
        return false;
    }

    // Metody Pure-virtual, które muszą zastac zdefiniowane dla kazdej warstwy
    // /////////////////////////////////////////////////////////////////////////

    const geometry_base* get_geometry()
    //Wypełnienie obowiazku pure-virtual
    {
        return &MainGeometry;
    }

    AGENT& get(size_t index)
    //Daje dostęp do elementu o indeksie obliczonym przez geometrie
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
    // /////////////////////////////////////////////////////////////////////////

    AGENT&	get(size_t X,size_t Y)
    // Bezposredni dostęp do struktury
    {
        size_t lindex=MainGeometry.get(X,Y);           assert(lindex!=any_layer_base::FULL); //Jedyne sprawdzanie zakresow żeby nie spowalniac przetestowanej symulacji
        return *table[lindex];
    }

    AGENT& operator () (size_t X,size_t Y)
    {
        return get(X,Y);
    }

    /// Dostęp do (pół-)inteligentnego wskaźnika budującego warstwę.
    wb_ptr<AGENT>&	get_ptr(size_t X,size_t Y)
    {
        size_t lindex=MainGeometry.get(X,Y);
        assert(lindex!=any_layer_base::FULL); //Jedyne sprawdzanie zakresow żeby nie spowalniac przetestowanej symulacji
        return table[lindex];
    }

    bool filled(int X,int Y) override
    {
        if(get_ptr(X,Y).OK()) //  !=NULL
            return true;
        else
            return false;
    }

    void clean(size_t TargetX,size_t TargetY) override
    //Konieczne bo pure-virtual, czyszczenie pojedynczej struktury
    {
        if(full_allocation)
        {
            get_ptr(TargetX,TargetY)=initer->clone();
        }
        else
        {
            get_ptr(TargetX,TargetY)=NULL; //Automatyczna destrukcja
        }
    }

    /// Zamiana elementów za pomocą wskaxników. Nie trzeba kopiowac danych.
    void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY)
    {
        wb_sptr<AGENT>& Target=get_ptr(TargetX,TargetY);
        wb_sptr<AGENT>& Source=get_ptr(SourceX,SourceY);
        wb_sptr<AGENT>  tmp;
        //Kopiuje wskaźniki wiec
        //omija mechanizm konstrukcji/destrukcji
        tmp=Target;		//Target jest teraz pusty,
        Target=Source;	//teraz Source jest pusty
        Source=tmp;		//a śteraz tmp jest pusty, a Source i Target są zamienione
    }

    /// @name Przykrycie funkcji `init_from_bitmap`.
    /// @{
    typedef void (AGENT::* assign_rgb_fun)(unsigned char,unsigned char,unsigned char);

    // Struktura do przechowywania (?) funkcji wczytujacej kolory
    struct assign_rgb_stc
    {
        assign_rgb_fun AssFun;
        assign_rgb_stc(assign_rgb_fun par):AssFun(par){}
    };

    int init_from_bitmap(const char* filename,
        assign_rgb_stc user_fun)
    {
        return rectangle_layer::init_from_bitmap(filename,&user_fun);
    }

    // Przypisanie polu wartości RGB z bitmapy - domyślnie przeksztalcone na szarosc,
    void assign_rgb(size_t TargetX,size_t TargetY,		//tez konieczne bo pure-virtual
        unsigned char Red,unsigned char Green,unsigned char Blue,void* user_data)
    {
        assign_rgb_fun AssignFun=((assign_rgb_stc*)user_data)->AssFun;
        wb_ptr<AGENT>& Target=get_ptr(TargetX,TargetY);

        if(!Target)
        {
            if(!initer)
                Target=new AGENT();
            else
                Target=initer->clone();
        }
        assert(Target.OK());
        assert(AssignFun!=NULL);
        AGENT* pom=Target.get_ptr_val();
        (pom->*AssignFun)(Red,Green,Blue);
    }
    /// @}


    /// @name Tworzenie źródeł do czytania danych.
    /// @details Najlepiej gdyby to był szablon, ale to było jeszcze niestandardowe C++ gdy kod powstawał.
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
        method_by_ptr_matrix_source<AGENT,double>* make_source(const char* name,double (AGENT::* method_ptr)(void) )
    {
        return new method_by_ptr_matrix_source<AGENT,double>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,short>* make_source(const char* name,short (AGENT::* method_ptr)(void))
    {
        return new method_by_ptr_matrix_source<AGENT,short>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }


    virtual
        method_by_ptr_matrix_source<AGENT,int>* make_source(const char* name,int (AGENT::* method_ptr)(void))
    {
        return new method_by_ptr_matrix_source<AGENT,int>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,unsigned>* make_source(const char* name,unsigned (AGENT::* method_ptr)(void))
    {
        return new method_by_ptr_matrix_source<AGENT,unsigned>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,long int>* make_source(const char* name,long int (AGENT::* method_ptr)(void))
    {
        return new method_by_ptr_matrix_source<AGENT,long int>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,unsigned long>* make_source(const char* name,unsigned long (AGENT::* method_ptr)(void))
    {
        return new method_by_ptr_matrix_source<AGENT,unsigned long>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }
    /// @}

    /// @name Implementacja wejścia/wyjścia. Zwracają 1 jeśli sukces!
    /// @{
    int		implement_output(ostream& o) const
    {
        o<<table<<' '<<initer<<' '; //Obiekt do zamazywania
        o<<empty_guard<<' '<<full_allocation<<' '; //Zwracany jako reprezentant pustych pol
        return 1;
    }

    int		implement_input(istream& i)
    {
        i>>table;
        i>>initer;
        i>>empty_guard;
        i>>full_allocation;
        if(i.fail())
            return 0;
        return 1;
    }
    /// @}
};

} //namespace symshell2

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


