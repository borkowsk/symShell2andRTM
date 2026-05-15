/// @file
/// @brief **CLASSES DEFINING VARIOUS TYPES OF CHARTS** /<br>
///         _KLASY DEFINIUJĄCE RÓŻNORODNE TYPY WYKRESÓW._
/// @date 2026-05-16 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_GRAPHS_HPP_INCLUDED_
#define SYMSHELL2_GRAPHS_HPP_INCLUDED_

#include <cassert>
#include <cstring>
#include <iostream>

#include "wb_clone.hpp"
#include "sshutils.hpp"
#include "datasour.hpp"
#include "drawable.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

/// Zmodernizowane klasy do symulacji w C++.
namespace symshell2
{
    using namespace sym2::data;

//INTERFACE DO GRAFU — KLASY WYPROWADZONE POWINNY TYLKO IMPLEMENTOWAĆ:
//--------------------------------------------------------------------

/// @brief Base class for data point display configuration./<br>
///        Klasa bazowa dla konfiguracji wyświetlania punktów danych.
class config_point
{
    float base_fract; ///< Rozmiar minimalny punktu. Jako ułamek obszaru (default 5%%).
    float  max_fract; ///< Rozmiar maksymalny punktu. Jako ułamek  obszaru (default 2%).

public:
    size_t  base_len;  ///< Aktualnie przyjęty rozmiar okna. Można zmieniać!

    /// Konstruktor domyślny. Domyślnie duży `base_len`, żeby awaryjnie było coś pikselach.
    config_point()
    : base_fract(0.005f), max_fract(0.02f), base_len(1000)
    {}

    /// Konstruktor ustawiający. Domyślnie duży `base_len`, żeby awaryjnie było coś pikselach.
    config_point(float base,float max)
    :base_fract(base),max_fract(max), base_len(1000)
    {}

    /// Wyliczenie faktycznego rozmiaru bazowego w pikselach.
    unsigned base_size() const { return unsigned(double(base_len) * base_fract); }

    /// Wyliczenie faktycznego rozmiaru maksymalnego w pikselach.
    unsigned max_size() const { return unsigned(double(base_len) * max_fract); }

    /// Rysowanie punktu danych.
    virtual void plot(int x, int y, unsigned short size, wb_color color); //Defaultowe rysowanie
};

/// Class to draw a simple point (Ignores size!!!)/<br>
/// Klasa rysująca prosty punkt (Ignoruje rozmiar!!!).
class simple_point:public config_point
{
public:
    simple_point() = default;
    void plot(int x,int y, unsigned short size, wb_color color) override;
};

/// A class drawing a cross/hash./<br>
/// Klasa rysująca krzyżyk.
class hash_point:public config_point
{
public:
    hash_point() = default;
    hash_point(float base,float max):config_point(base,max){}
    void plot(int x,int y, unsigned  short size, wb_color color) override;
};

/// A class that draws a circle/<br>
/// Klasa rysująca kólko.
class circle_point:public config_point
{
public:
    circle_point() = default;
    circle_point(float base,float max):config_point(base,max){}
    void plot(int x,int y, unsigned short size, wb_color color) override;
};

/// A class that draws a rhombus/<br>
/// Klasa rysująca romb.
class rhomb_point:public config_point {
public:
    rhomb_point() = default;
    rhomb_point(float base, float max) : config_point(base, max) {}
    void plot(int x, int y, unsigned short size, wb_color color) override;
};

/// Square/Rectangle Drawing Class (UNUSED?)/<br>
/// Klasa rysująca kwadracik/prostokąt (NIEUŻYWANA?).
class rect_point:public config_point
{
public:
    rect_point()= default;
    rect_point(float base,float max):config_point(base,max){}
    void plot(int x,int y, unsigned short size, wb_color color) override;
};

/// @brief COMMON USER INTERFACE for all charts and similar classes./<br>
///        WSPÓLNY INTERFACE UŻYTKOWNIKA dla wszystkich wykresów i klas podobnych.
class graph:public drawable_base
//------------------------------
{
public:

    /// Klasa informacji o serii dla wykresów o nieustalonej liczbie serii.
    class series_info
    {
    public:
        /// Constructor.
        explicit series_info(
                    data_source_base*	i_pt=NULL,	///< Wskaźnik do serii danych.
                    int					i_me=0,		///< Określa, czy zarządza serią danych (zwalnianiem).
                    wb_color			i_co=default_color,	///< Ustalanie koloru.
                    config_point*		fig=NULL,	///< Obiekt rysujący punkty.
                    int					i_mep=0		///< Określa, czy zarządza obiektem rysującym (zwalnianiem).
                )
        : ptr(i_pt), menage(i_me), figure(fig), men_p(i_mep), color(i_co)
        {}

        /// Destructor.
        ~series_info()
        {
            if(menage && ptr!=NULL)
                { delete ptr; }
            if(men_p && figure!=NULL)
                { delete figure; }
        };

        /// Wskaźnik do źródła danych.
        /// @details Musi żywcem przepisywać wskaźniki, jeśli nimi nie zarządza!!!
        ///          To założenie wykonane na razie przez domyślny `operator = `.
        data_source_base*	ptr;

        wb_color			color;		///< Kolor punktów. Jeśli !=-1, ustala taki indeks koloru.
        config_point*		figure;		///< Jeśli NULL to łączy liniami.
        int					menage:1;	///< Flaga zarządzania pamięcią źródła
        int					men_p:1;	///< Flaga zarządzania pamięcią obiektem rysującym.
    };

public:
    /// @name AKCESORY Z MOŻLIWOŚCIĄ ALBO KONIECZNOŚCIĄ REIMPLEMENTACJI.
    /// @details Ustalają kolor lub zbiór kolorów dla tekstów oraz kolor, lub zakres dla danych.
    /// @{

    /// Ma zwracać tekst opisujący typ wykresu.
    virtual const char* user_help_text()=0;

    /// Ustala kolor dla tekstów.
    virtual int set_text_colors(wb_color start_i, wb_color end_i= default_white);

    /// Ustala zakres kolorów indeksowanych dla danych.
    virtual int set_data_colors(wb_color start_i, wb_color end_i);

    /// Pozwala przypisać serię danych pod konkretnym indeksem tablicy/listy seri.
    /// @returns -1, jeśli indeks jest za duży.
    virtual int set_series(size_t index, data_source_base* data, int menage= 0)=0;

    /// Podaje modyfikowalny wskaźnik do serii znajdującej się w konkretnej pozycji tablic/listy.
    /// @returns NULL, jeśli indeks za duży.
    virtual data_source_base* get_series(size_t index)=0;
    /// @}

    /// @name Metody do koniecznego zaimplementowania w klasach potomnych.
    /// @{

    /// Ma ustalać parametry wyświetlania poprzez odpowiednią strukturę. @return -1, jeśli błędny parametr.
    virtual int configure(const void*)=0;

protected:
    /// Przeskalowywanie wartości współrzędnych punktu danych.
    /// Wymaga się, by działał prawidłowo po wywołaniu funkcji `_replot`.
    /// @returns -1, jeśli błąd. Np. któraś z podanych wartości nie zawiera się w podanym przez serie zakresie
    ///         `<minimum, maksimum>`.
    /// @param reals zawiera współrzędne (wartości serii z głównych)
    ///        oraz wartość ustalająca kolor z seri dla koloru (jeśli jest).
    /// @param in_area ...
    virtual int _rescale_data_point(const double reals[],long in_area[])=0;
    /// @}

    /// Constructor.
    /// @param x1,y1,x2,y2 to współrzędne rogów obszaru.
    /// @param ibkg,ifr to kolor tła i kolor ramki.
    /// @details Domyślnie wszystkie elementy mają ustawioną wizualizacje.
    graph(  int x1,int y1,int x2,int y2,
            wb_color ibkg=default_white,
            wb_color ifr=default_transparent)
    : drawable_base(x1,y1,x2,y2,ibkg,ifr),c_range(default_black,default_white)
    {
        vis_title=vis_leg1=vis_leg2=vis_leg3=vis_leg4=1;
    }

public:
    /// @name FLAGI WIZUALIZACJI TYPOWYCH SKŁADOWYCH TYMCZASOWO PUBLICZNE ???
    /// @{
    unsigned vis_title:1; //!< Flaga wizualizacji tytułu.
    unsigned vis_leg1:1;  //!< Flaga wizualizacji legendy dla serii 1 i 2.
    unsigned vis_leg2:1;  //!< Flaga wizualizacji legendy dla serii 3.
    unsigned vis_leg3:1;  //!< Flaga wizualizacji skali kolorów.
    unsigned vis_leg4:1;  //!< Flaga wizualizacji legendy dla rozmiarów.
    /// @}

    /// Struktura informacji o skalowaniu serii danych.
    struct scaling_info
    {
        double min,max,scale; ///< Wartości potrzebne do przeliczenia
        /// @name Flagi blokady minimum i maksimum.
        /// @{
        unsigned fix_min:1;
        unsigned fix_max:1;
        /// @}

        /// Constructor. Domyślnie nie blokuje ani minimum, ani maksimum — bierze tak, jak seria daje.
        explicit scaling_info(double i_min=0, double i_max=1, double range=1)
        : min(i_min), max(i_max), scale(0)
        {
            fix_min=fix_max=0; //Domyślnie nie blokuje
            set(range);
        }

        /// Ustala minimum i maksimum, jeśli nie są zablokowane flagami.
        /// Potem wylicza skalowanie, używając parametru `range`.
        void set(double i_min, double i_max, double range)
        {
            if(!fix_max) max=i_max;
            if(!fix_min) min=i_min;
            assert(min<=max);

            if(min!=max)
                scale=range/(max-min);
            else
                scale=0; //Awaryjnie;
        }

        /// Wylicza skalowanie, używając parametru docelowego `range`.
        void set(double range)
        {
            assert(min<=max);
            if(min!=max)
                scale=range/(max-min);
            else
                scale=0; //Awaryjnie
        }

        /// Wylicza z `val` odpowiednik przeskalowany i przesunięty.
        double get(double val) const
        {
            return scale*(val-min);
        }

        /// Rysuje w zadanym oknie os 0X.
        void OX_axis(int x1, int y1, int x2, int y2, wb_color col, wb_color bcg) const;

        /// Rysuje w zadanym oknie oś 0Y.
        void OY_axis(int x1, int y1, int x2, int y2, wb_color col, wb_color bcg) const;
    };

    /// Struktura informacji o kolorze serii.
    struct color_info
    {
        wb_color start; ///< To musi być zawsze.
        wb_color end;   ///< Jeśli `end>start` to oznacza, że chodzi o zakres kolorów.
        explicit color_info(wb_color i_sta=0,wb_color i_end=0):start(i_sta),end(i_end) {}
        int plot(int x1,int y1,int x2,int y2) const;
    };

protected:
    color_info	   c_range;	//!< Kolory obiektów. Najwyżej tyle ile udostępnia platforma.
    color_info	  t_colors;	//!< Kolory tekstów, najwyżej dwa.
    gps_area	graph_core;	//!< Właściwy obszar rysowania po wycięciu wszystkich marginesów.
};


//		   RÓŻNE RODZAJE GRAFÓW/WYKRESÓW:
//		DIFFERENT TYPES OF GRAPHS/CHARTS:
//=======================================
//---------------------------------------

/// @brief A rectangle made of multi-colored squares/<br>
///        Prostokąt z różnokolorowych kwadracików.
class carpet_graph:public graph
//------------------------------------------------------------------------
{
    unsigned print_title:1; ///< Flaga, czy drukować tytuł.

public:
    //HELP:
    const char* user_help_text() override { return
    "CARPET GRAPH:\n"
    "Pseudo 2-dimensional, 1. serie for colors (index 0)\n"
    "Rectangles are arranged on AxB base.\n"
    "By default colors are mapping from 0 to 255.\n"
    "If textcolor!=background the legend is printing\n"
    "\n"
    "\n"
    "\n"
    "\n";}

    /// @name CONSTRUCTOR(S).
    /// @{

    /// 1.
    carpet_graph(int x1,int y1,int x2,int y2,				///< Położenie obszaru.
                 unsigned A,unsigned B,						///< A-ile kolumn, B-ile wierszy.
                 data_source_base* data,int i_menage=0,		///< Źródło danych o kolorach i czy ma być zwalniane.
                 bool i_direct_color=false					///< Blokuje skalowanie kolorów.
                                                            ///< Wtedy liczba z danych oznacza indeks koloru.
                );

    /// 2.
    carpet_graph(int x1,int y1,int x2,int y2,				///< Położenie obszaru. Seria musi mieć geometrie 2D.
                 data_source_base* data,int i_menage=0,		///< Źródło danych o kolorach i czy ma być zwalniane.
                 bool i_direct_color=false					///< Blokuje skalowanie kolorów.
                                                            ///< Wtedy liczba z danych oznacza indeks koloru.
                );
    /// @}

    ///DESTRUCTOR.
    ~carpet_graph() override;

    /// @name IMPLEMENTATION OF VIRTUAL METHODS.
    /// @{

    /// Nie ma możliwości ani parametrów konfiguracji. Zawsze zwraca -1.
    int configure(const void*) override {return -1;}

    /// Tylko jedna seria i tylko `index == 0` jest dozwolony.
    int set_series(size_t index, data_source_base* i_data, int i_menage= 0) override;

    /// Tylko jedna seria i tylko `index == 0` jest dozwolony.
    data_source_base* get_series(size_t index) override;

    /// Wersja uproszczona. @param reals ma zawierać jedynie wartość ustalającą kolor.
    int _rescale_data_point(const double reals[1],long in_area[1]) override;

    /// Rysuje właściwy wykres, a pod nim ewentualnie legendę.
    void _replot() override;
    /// @}

protected:
    // ONLY FOR DEVELOPERS OF THIS CLASS:
    /// Czyta wymiary z geometrii lub z ustawionego AA i BB.
    const geometry_base* read_dim(size_t& aa,size_t& bb);

    size_t					AA,BB;
    bool					menage;
    data_source_base*		data;
    bool					direct_color; ///< Blokuje skalowanie kolorów. Wtedy liczba z danych oznacza indeks koloru.
    graph::scaling_info		mm;
    wb_ptr<geometry_base>	deputy; ///< Geometria używana, gdy podano AA i BB w konstruktorze.
};

/// @brief  A rectangle made of squares with colors composed of three data series/<br>
///         Prostokąt z kwadracików o kolorach składanych z trzech serii danych.
class true_color_carpet_graph:public graph
//----------------------------------------
{
public:
    //HELP:
    const char* user_help_text() override { return
    "TRUE COLOR CARPET GRAPH:\n"
    "Pseudo 2-dimensional, three series for colors (RGB)\n"
    "Rectangles are arranged on AxB base.\n"
    "By default colors are mapping from 0 to 255.\n"
    "If 'textcolor!=background' the legend is printing\n"
    "\n"
    "\n"
    "\n"
    "\n";}

    /// @name CONSTRUCTOR(S)
    /// @details Serie muszą mieć taka sama geometrie 2D!
    /// @param x1,y1,x2,y2 to współrzędne rogów obszaru.
    /// @param RedData,GreenData,BlueData to serie danych, z których brane są składowe RBG.
    /// @param menage_r,menage_g,menage_b to flagi zarządzania pamięcią poszczególnych serii.
    /// @{
    true_color_carpet_graph(int x1,int y1,int x2,int y2,
                 unsigned A,unsigned B,			 ///< A-ile kolumn, B-ile wierszy.
                 data_source_base* RedData,int menage_r=0,
                 data_source_base* GreenData=NULL,int menage_g=0,
                 data_source_base* BlueData=NULL,int menage_b=0
                 );

    true_color_carpet_graph(int x1,int y1,int x2,int y2,
                 data_source_base* RedData,int menage_r=0,
                 data_source_base* GreenData=NULL,int menage_g=0,
                 data_source_base* BlueData=NULL,int menage_b=0
                 );
    /// @}

    /// DESTRUCTOR.
    ~true_color_carpet_graph() override;

    /// @name IMPLEMENTATION OF VIRTUAL METHODS
    /// @{
    /// Nie ma parametrów konfiguracji. Zawsze zwraca -1.
    int configure(const void*) override {return -1;}
    int set_series(size_t index, data_source_base* i_data, int i_menage= 0) override;
    data_source_base* get_series(size_t index) override;
    int _rescale_data_point(const double reals[3],long in_area[3]) override;
    /// Rysuje właściwy wykres, a pod nim ewentualnie legendę.
    void _replot() override;
    /// @}
protected:
    // ONLY FOR DEVELOPERS OF THIS CLASS:
    /// Czyta wymiary z geometrii lub z ustawionego AA i BB.
    const geometry_base* read_dim(size_t& aa,size_t& bb);

    size_t					AA,BB;
    bool					menage[3];
    data_source_base*		data[3];
    graph::scaling_info		mm[3];
    wb_ptr<geometry_base>	deputy; ///< Geometria używana, gdy podano AA i BB w konstruktorze.
};

/// @brief Accelerated carpet, made entirely of pixels\<br>
///        Dywan przyspieszony, z samych pikseli.
template<class DATA_SOURCE,int DIRECT_COLOR=false>
class fast_carpet_graph:public carpet_graph
//-----------------------------------------
{
public:
    /// @name CONSTRUCTOR(S) and DESTRUCTOR
    /// @{
    /// 1.
    fast_carpet_graph(int x1,int y1,int x2,int y2,		///< Położenie obszaru grafu.
                 unsigned A,unsigned B,					///< A-ile kolumn, B-ile wierszy.
                 DATA_SOURCE* data,int menage=0)		///< `data` to źródło danych o kolorach.
    : carpet_graph(x1,y1,x2,y2,A,B,data,menage)
    {}

    /// 2.
    fast_carpet_graph(int x1,int y1,int x2,int y2,		///< Położenie obszaru. Seria musi mieć geometrie 2D.
                      DATA_SOURCE* data,int menage=0)	///< `data` to źródło danych o kolorach.
    : carpet_graph(x1,y1,x2,y2,data,menage)
    {}

    /// DESTRUCTOR.
    ~fast_carpet_graph() override = default;
    /// @}

    /// Rysuje właściwy wykres, a pod nim ewentualnie legendę.
    /// Typ źródła jest ustalony na DATA_SOURCE, wiec wywołania wewnętrzne mogą być rozwijane inline.
    void _replot() override;

};

/// @brief  Usually 2D-bars of different sizes and possibly colors/<br>
///         Zwykle słupki 2D różnej wielkości i ewentualnie koloru.
class bars_graph:public graph
//---------------------------
{
public:
    //HELP:
    const char* user_help_text() override { return
    "2D BARS PLOT:\n"
    "2-Dimensional, two series:\n"
    "- index '0': heights of bars\n"
    "- index '1': colors of bars (optional)\n"
    "By default colors range are from 0 to 255.\n"
    "and default bar color is 128.\n"
    "If 'textcolor!=background' legend is printing\n"
    "\n";}

    /// CONSTRUCTOR(S)
    /// @param menage_d,menage_c to flagi zarządzania pamięcią (delete) serii danych o wysokościach i o kolorach.
    bars_graph(int x1, int y1, int x2, int y2,						///< Położenie obszaru grafu.
                 data_source_base* i_datas, int menage_d=0,			///< `datas` to dane o wysokościach słupków.
                 data_source_base* i_colors=NULL, int menage_c=0,	///< `i_colors` to dane o kolorach (indeksowanych).
                 int zero_mod=1										///< Tryb. 1-Słupki zaczynają się co najmniej od 0!
                                                                    ///<       0- to zaczynają się od min (może >0)
                 );

    /// DESTRUCTOR.
    ~bars_graph() override;

    /// Dane do konfiguracji.
    struct config_zero
    {
        explicit config_zero(int z=0):zero_mode(z){}
        int zero_mode;
    };

    /// @name IMPLEMENTATION OF VIRTUAL METHODS
    /// @{
    /// Konfiguruje tryb wyświetlania za pomocą struktury typu `bars_graph::config_zero`. @param config to adres struktury.
    int configure(const void* config) override;
    int set_series(size_t index, data_source_base* i_data, int i_menage= 0) override;
    data_source_base* get_series(size_t index) override;
    int _rescale_data_point(const double reals[2],long in_area[2]) override;
    /// Rysuje właściwy wykres a pod nim ewentualnie legendę.
    void _replot() override;
    /// @}
private:    // ONLY FOR DEVELOPERS OF THIS CLASS
    int					mode;
    bool				d_menage;
    bool				c_menage;
    data_source_base*	datas;
    data_source_base*	colors;
    scaling_info		s_data;
    scaling_info		s_colo;
};

/// @brief A column or row of numbers, each on a different color background!/<br>
///        Kolumna lub wiersz liczb, każda na innym tle kolorystycznym!
class rainbow_graph:public graph
//------------------------------
{
public:
    //HELP:
    const char* user_help_text() override { return
    "RAINBOW PLOT:\n"
    "0 Dimensional, two series:\n"
    "- index 0: values to print\n"
    "- index 1: colors of bars (optional)\n"
    "By default colors range are from 0 to 255.\n"
    "and default bar color is background.\n"
    "If `textcolor!=background` legend is printing\n"
    "User is able to configure the format of printing values.\n"
    "\n";}

    /// @name CONSTRUCTOR(S) and DESTRUCTOR.
    /// @{
    /// Constructor.
    rainbow_graph(int x1, int y1, int x2, int y2,					///< Położenie obszaru
                 data_source_base* i_datas, int menage_d=0,			///< `datas` to dane o wartościach, opcjonalnie zarządzane.
                 data_source_base* i_colors=NULL, int menage_c=0,	///< `colors` to źródło danych o kolorach, opc. zarządzane.
                 const char* format="%g"							///< Format liczby — nie może być więcej niż 1 kod `%`!
                 );

    /// Destructor.
    ~rainbow_graph() override;
    /// @}

    /// @name IMPLEMENTATION OF VIRTUAL METHODS
    /// Konfiguracja formatu. @param format wymaga `const char*` z jednym kodem procentowym, np. "%f".
    int configure(const void* format) override;
    int set_series(size_t index, data_source_base* data, int menage= 0) override;
    data_source_base* get_series(size_t index) override;
    /// Skalowany jest tylko kolor.
    int _rescale_data_point(const double reals[1],long in_area[1]) override;
    /// Rysuje "tęczę" z liczbami.
    void _replot() override;

private:    // ONLY FOR DEVELOPERS OF THIS CLASS
    bool					d_menage;
    bool					c_menage;
    data_source_base*		datas;
    data_source_base*		colors;
    scaling_info			s_colo;
    char*					format;
};

/// @brief 3D Pillar/Bar Carpet/<br>
///        Dywanik słupków 3D
class manhattan_graph:public graph
//-------------------------------------------------------------------------
{
public:
    //HELP:
    const char* user_help_text() override { return
    "MANHATTAN PLOT:\n"
    "Pseudo 3 Dimensional, two series:\n"
    "- index `0`: heights of bars\n"
    "- index `1`: colors of bars (optional)\n"
    "Bars are arranged on 'A x B' base.\n"
    "By default colors range are from 0 to 255.\n"
    "and default bar color is 128.\n"
    "If 'textcolor!=background' legend is printing\n"
    "\n";}


    /// @name CONSTRUCTOR(S) and DESTRUCTOR.
    /// @details Serie danych muszą mieć the_same geometrie.
    /// @{
    manhattan_graph(int x1, int y1, int x2, int y2,						///< Położenie obszaru wykresu.
                     unsigned A, unsigned B,							///< A-ile kolumn, B-ile wierszy.
                     data_source_base* i_datas, int menage_d=0,			///< `datas` to dane o wysokościach.
                     data_source_base* i_colors=NULL, int menage_c=0,	///< `colors` to źródło danych o kolorach.
                     int zero_mod=1,			///< Tryb: 1 - Słupki zaczynają się co najmniej od 0.
                                                ///< Jeśli 0 to zaczynają się od min>0.
                     double H_offs=0.33,		///< Ułamek szerokości przeznaczony na perspektywę.
                     double	V_offs=0.33			///< Ułamek wysokości  przeznaczony na perspektywę
                 );

    manhattan_graph(int ix1, int iy1, int ix2, int iy2,					///< Położenie obszaru wykresu.
                     data_source_base* i_datas, int menage_d=0,			///< `datas` to dane o wysokościach.
                     data_source_base* i_colors=NULL, int menage_c=0,	///< `colors` to źródło danych o kolorach.
                     int zero_mod=1,			///< Tryb: 1 - Słupki zaczynają się co najmniej od 0.
                                                ///< Jeśli 0 to zaczynają się od min>0.
                     double H_offs=0.33,		///< Ułamek szerokości przeznaczony na perspektywę.
                     double	V_offs=0.33			///< Ułamek wysokości  przeznaczony na perspektywę
                 );

    /// Destructor.
    ~manhattan_graph() override;
    /// @}


    /// Struktura do konfiguracji,
    struct config
    {
        explicit config(int c_offs=0,unsigned col_wire=0,int z=0)
        : color_offset(c_offs),wire(col_wire),zero_mode(z)
        {}

        int color_offset; ///< ...
        unsigned    wire; ///< ...
        int    zero_mode; ///< ...
    };

    /// @name IMPLEMENTATION OF VIRTUAL METHODS
    /// Konfiguruje tryb wyświetlania, używając struktury typu `manhattan_graph::config`.
    int configure(const void* config) override;
    int set_series(size_t index, data_source_base* data, int menage= 0) override;
    data_source_base* get_series(size_t index) override;
    int _rescale_data_point(const double reals[2],long in_area[2]) override;
    /// Rysuje właściwy wykres a pod nim ewentualnie legendę
    void _replot() override;

private:
    // ONLY FOR DEVELOPERS OF THIS CLASS
    /// Czyta wymiary z geometrii lub z ustawionego A i B.
    const geometry_base* read_dim(size_t& aa,size_t& bb);

    int					mode;
    int					c_offset;
    wb_color			wire;
    unsigned			AA;
    unsigned			BB;
    double				h_offs;
    double				v_offs;
    int					d_menage;
    int					c_menage;
    data_source_base*	datas;
    data_source_base*	colors;
    scaling_info		s_data;
    scaling_info		s_colo;
    wb_ptr<geometry_base> deputy; ///< Zastępcza geometria, gdy w konstruktorze podano A i B.
};

/// @brief RGB 3D Pillar/Bar Carpet/<br>
///        Dywanik słupków 3D w kolorach RGB.
class true_color_manhattan_graph:public graph
//-------------------------------------------------------------------------
{
public:
    //HELP:
    const char* user_help_text() override{ return
    "MANHATTAN PLOT:\n"
    "Pseudo 3 Dimensional, four series:\n"
    "- index 0: heights of bars\n"
    "- index 1..3: colors of bars components (optional)\n"
    "Bars are arranged on `A x B` base.\n"
    "By default RGBs range are from 0 to 255.\n"
    "and default bar color is blue (?).\n"
    "If 'textcolor!=background legend' is printing\n"
    "\n";}

    /// @name CONSTRUCTOR(S) and DESTRUCTOR
    /// @details Serie danych muszą mieć the_same geometrie.
    /// @param RedData,GreenData,BlueData to serie danych, z których brane są składowe RBG.
    /// @param menage_r,menage_g,menage_b to flagi zarządzania pamięcią poszczególnych serii.
    /// @{
    /// 1.
    true_color_manhattan_graph(int x1, int y1, int x2, int y2,		///< Położenie obszaru.
                 unsigned A, unsigned B,							///< A-ile kolumn, B-ile wierszy.
                 data_source_base* i_datas, int menage_d=0,			///< `datas` to dane o wysokościach.
                 data_source_base* RedData=NULL, int menage_r=0,
                 data_source_base* GreenData=NULL, int menage_g=0,
                 data_source_base* BlueData=NULL, int menage_b=0,
                 int zero_mod=1,								///< Gdy 1 to słupki zaczynają się co najmniej od 0.
                                        						///< Jeśli 0 to zaczynają się od `min>0`.
                 double H_offs=0.33,								///< Ułamek szerokości przeznaczony na perspektywę.
                 double	V_offs=0.33								///< Ułamek wysokości  przeznaczony na perspektywę.
                 );

    /// 2..
    true_color_manhattan_graph(int ix1, int iy1, int ix2, int iy2,		///< Położenie obszaru.
                 data_source_base* i_datas, int menage_d=0,				///< `datas` to dane o wysokościach.
                 data_source_base* RedData=NULL, int menage_r=0,
                 data_source_base* GreenData=NULL, int menage_g=0,
                 data_source_base* BlueData=NULL, int menage_b=0,
                 int zero_mod=1,								///< Gdy 1 to słupki zaczynają się co najmniej od 0.
                                                                ///< Jeśli 0 to zaczynają się od `min>0`.
                 double H_offs=0.33,								///< Ułamek szerokości przeznaczony na perspektywę.
                 double	V_offs=0.33								///< Ułamek wysokości  przeznaczony na perspektywę.
                 );

    /// Destructor.
    ~true_color_manhattan_graph() override;
    /// @}

    /// Struktura do konfiguracji wyświetlania.
    struct config
    {
        explicit config(int c_offs=0,unsigned col_wire=0,int z=0)
        : color_offset(c_offs),wire(col_wire),zero_mode(z)
        {}

        int  color_offset;
        unsigned     wire;
        int     zero_mode;
    };

    /// @name IMPLEMENTATION OF VIRTUAL METHODS
    /// @{
    /// Konfiguruje tryb wyświetlania, używając struktury typu `true_color_manhattan_graph::config`.
    int configure(const void* config) override;
    int set_series(size_t index, data_source_base* i_data, int i_menage=0) override;
    data_source_base* get_series(size_t index) override;
    int _rescale_data_point(const double reals[4],long in_area[4]) override;
    /// Rysuje właściwy wykres a wokół niego ewentualnie legendę.
    void _replot() override;
    /// @}

private:
    // ONLY FOR DEVELOPERS OF THIS CLASS
    /// Czyta wymiary z geometrii lub z ustawionego A i B.
    const geometry_base* read_dim(size_t& aa,size_t& bb);

    int							mode;
    int							c_offset;
    wb_color					wire;
    unsigned					AA;
    unsigned					BB;
    double						h_offs;
    double						v_offs;
    bool						d_menage;
    bool						c_menage[3];
    data_source_base*			datas;
    data_source_base*			colors[3];
    scaling_info				s_data;
    scaling_info				s_colo[3];
    wb_ptr<geometry_base>		deputy;
};

/// @brief A typical line graph. Consecutive points optionally connected by lines. Any number of series./<br>
///        Typowy wykres liniowy.  Kolejne punkty ewentualnie połączone liniami. Dowolna ilość seri.
class sequence_graph:public graph
//-----------------------------------------------------------------------------------------------------
{
public:
    //HELP:
    const char* user_help_text() override{ return
    "N SEQUENCES PLOT:\n"
    "2 Dimensional, many series.\n"
    "- Case number or time on X axi.\n"
    "- index 0...N: series to print\n"
    "By default colors range are from 0 to 255\n"
    "and must contain as many colors as series exists.\n"
    "If `textcolor!=background` max,min legend is printing \n"
    "and series names are printing inside graph!\n"
    "\n";}

    /// @name CONSTRUCTOR(S) and DESTRUCTOR
    /// @{
    /// 1.
    sequence_graph(int x1,int y1,int x2,int y2,		///< Położenie obszaru
                   int N,							///< Liczba serii
                   series_info* series,				///< Tablica struktur zawierających informacje o seriach
                                                    ///< Może przejmować w zarząd serie i ich obiekty rysujące punkty.
                   int mode=0,	///<  - 0 - tryb z pełnym reskalowaniem
                                ///<  - 1 - tryb ze wspólnym minimum, maximum, pobieranym z parametrów i serii
                                ///<  - 2 - tryb z zafiksowanym minimum i maksimum
                   double min=0.0,double max=0.0 ///< Zafiksowanie zakresu minimum-maksimum.
                  );

    /// 1.
    sequence_graph(int x1,int y1,int x2,int y2,		///< Położenie obszaru
                   int N,							///< Liczba serii
                   data_source_base** series,		///< Tablica serii. Struktury informacji o seriach zostaną wypełnione domyślnie.
                                                    ///< Nie ma możliwości zarządzania pamięcią seri.
                   int mode=0,	///<  - 0 - tryb z pełnym reskalowaniem
                                ///<  - 1 - tryb ze wspólnym minimum, maximum, pobieranym z parametrów i serii
                                ///<  - 2 - tryb z zafiksowanym minimum i maksimum
                   double min=0.0,double max=0.0 ///< Zafiksowanie zakresu minimum-maksimum.
                  );


    /// Destructor.
    ~sequence_graph() override;
    /// @}

    /// Struktura konfiguracji wyświetlania.
    struct config_seq
    {
        explicit config_seq(int im=0,double imi=0.0,double ima=0.0):
                    mode(im),min(imi),max(ima){}
        int    mode; ///< Tryb wyświetlania o znaczeniach jak w konstruktorach.
        double  min; ///< Możliwość określenia minimum.
        double  max; ///< Możliwość określenia maksimum.
    };

    /// @name IMPLEMENTATION OF VIRTUAL METHODS
    /// @{
    /// Konfiguracja. @param config ma być typu `sequence_graph::config_seq`, a jeśli NULL to tryb 0.
    int configure(const void* config) override;
    int set_series(size_t index, data_source_base* data, int menage= 0) override;
    data_source_base* get_series(size_t index) override;
    int _rescale_data_point(const double reals[/*M+1*/],long in_area[/*M+1*/]) override;
    /// Rysuje właściwy wykres a pod nim i na nim ewentualnie legendę.
    void _replot() override;
    /// @}

    /// Class specific method.
    unsigned color(unsigned val);

private:
    // ONLY FOR DEVELOPERS OF THIS CLASS
    int    M;				///< Rozmiar tablicy serii. Nie wszystkie muszą być pełne.
    int mode;				///< Aktualny tryb pracy.
    series_info*   series;	///< Tablica serii.
    scaling_info*  scales;	///< Tablica skal.
    scaling_info s_colors;	///< Skalowanie dla niepodanych kolorów.
    scaling_info  scale_x;	///< Skalowanie dla  osi X.
    scaling_info  scale_y;	///< Skalowanie dla  osi Y w trybie 1 i 2.
};

//Punkty w dwu wymiarach (dowolne X i Y)
class scatter_graph:public graph
//------------------------------------------------------------------------
{
public:
    //HELP:
    const char* user_help_text() override { return
    "SCATTER PLOT GRAPH:\n"
    "2 Dimensional, 4 series:\n"
    "index 1: X's of points\n"
    "index 2: Y's of points\n"
    "index 3: colors of points (optional - may be NULL)\n"
    "index 4: size of points (optional - may be NULL)\n"
    "By default colors range are from 0 to 255.\n"
    "and default bar color is 128.\n"
    "If `textcolor!=background` legend is printing\n"
    "User can configure type of points by class\n"
    "derived from config_point.\n"
    "\n";}

    /// @name CONSTRUCTOR(S) and DESTRUCTOR
    /// @{
    scatter_graph(int x1,int y1,int x2,int y2,						///< Położenie obszaru
                 data_source_base* Xdata,int menage_x  ,			///< Dane o X-ach
                 data_source_base* Ydata,int menage_y=0,			///< Dane o Y-ach
                 data_source_base* colors=NULL,int menage_c=0,		///< Dane o kolorach
                 data_source_base* sizes=NULL,int menage_s=0,		///< Dane o rozmiarach
                 config_point*  fig=new hash_point,int f_menage=1	///< Obiekt rysujący punkty, z domyślną wartością "hash".
                 );

    /// Destructor.
    ~scatter_graph() override;
    /// @}

    /// Struktura konfiguracji wyświetlania.
    struct config_scat
    {
        explicit config_scat(config_point*  i_fig=NULL, int i_men=0)
        : fig(i_fig), menage(i_men)
        {}

        config_point*  fig; ///< Wyświetlacz punktów.
        int         menage; ///< Określa, czy wyświetlacz jest zarządzany.
    };

    /// @name IMPLEMENTATIONS OF VIRTUAL METHODS
    /// @{
    /// Konfiguracja wyświetlania. Parametr musi być adresem do `scatter_graph::config_scat`.
    int configure(const void*) override;
    int set_series(size_t index, data_source_base* data, int menage=0) override;
    data_source_base* get_series(size_t index) override;
    int _rescale_data_point(const double reals[4],long in_area[4]) override;
    /// Rysuje właściwy wykres a pod nim ewentualnie legendę.
    void _replot() override;
    /// @}

    /// @name Specials for this class.
    /// @{

    /// Zafiksowanie zakresu X lub od-fiksowanie gdy `min == max. == -DBL_MAX`.
    void fix_X_minmax(double min,double max);

    /// Zafiksowanie zakresu Y lub od-fiksowanie gdy `min == max. == -DBL_MAX`.
    void fix_Y_minmax(double min,double max);
    /// @}

protected:
    // ONLY FOR DEVELOPERS OF THIS CLASS
    config_point*		CurrConfig;
    bool				menage_p;
    data_source_base*	Xdata;
    bool				menage_x;
    data_source_base*	Ydata;
    bool				menage_y;
    data_source_base*	colors;
    bool				menage_c;
    data_source_base*	sizes;
    bool				menage_s;
    scaling_info		scale_x; ///< Skalowanie dla X-ów.
    scaling_info		scale_y; ///< Skalowanie dla Y-ów.
    scaling_info		scale_c; ///< Skalowanie dla kolorów.
    scaling_info		scale_s; ///< Skalowanie dla rozmiarów.
};

//Punkty w dwu wymiarach (dowolne X i Y)
class net_graph:public scatter_graph
//------------------------------------------------------------------------
{
public:
    //HELP:
    const char* user_help_text() override{ return
    "NETWORK PLOT GRAPH:\n"
    "2 Dimensional\n"

    "4 SERIES FOR POINTS:\n"
    "index 1: X's of points\n"
    "index 2: Y's of points\n"

    "index 3: colors of points (optional - may be NULL)\n"
    "index 4: size of points (optional - may be NULL)\n"

    "4 SERIES FOR CONNECTIONS\n"
    "index 5: Source point index\n"
    "index 6: Target point index\n"

    "index 7: Arrow yes/no (0/1)\n"
    "index 8: colors for lines\n"

    "By default colors range are from 0 to 255.\n"
    "and default color is 128.\n"
    "If `textcolor!=background` legend is printing\n"
    "Default point is \"dot\" but user can provide `config_point` object for \n"
    "configure point printing.\n"
    "\n";}

    /// @name CONSTRUCTOR(S) and DESTRUCTOR
    /// @{
    /// 1.
    net_graph(   int x1,int y1,int x2,int y2,						///< Położenie obszaru.
                 data_source_base* Xdata,int menage_x  ,			///< Dane o X-ach.
                 data_source_base* Ydata,int menage_y,				///< Dane o Y-ach.
                 data_source_base* Sources,int menage_so,			///< Indeksy źródeł.
                 data_source_base* Targets,int menage_t,			///< Indeksy celów.

                 data_source_base* Colors=NULL,int menage_c=0,		///< Dane o kolorach.
                 data_source_base* Sizes=NULL,int menage_s=0,		///< Dane o rozmiarach.
                 data_source_base* Arrows=NULL,int menage_a=0,		///< Rozmiary strzałek - 0 brak.
                 data_source_base* ArrColors=NULL,int menage_ac=0,	///< Kolory strzałek.
                 config_point*     fig=NULL,int menage_f=0			///< Figura do rysowania punktów/węzłów.
                                                                    ///< Domyślnie brak, bo mogą być same połączenia.
                 );


    /// Destructor.
    ~net_graph() override;
    /// @}

    /// @name IMPLEMENTATION OF VIRTUAL METHODS
    /// @details NIEKTÓRE NIEPOTRZEBNE BO DZIEDZICZONE.
    /// @{
    //int configure(const void*); //Parametr typu scatter_graph::config_scat*
    int set_series(size_t index, data_source_base* data, int menage= 0) override;
    data_source_base* get_series(size_t index) override;
    // Parametr reals[] zawiera X oraz Y ewentualnie wartość dla koloru i rozmiaru.
    // Funkcja: int _rescale_data_point(const double reals[4], long in_area[4]); //zwraca -1, jeśli nie w oknie
    void _replot() override;
    /// @}

protected:
    // ONLY FOR DEVELOPERS OF THIS CLASS
    data_source_base*	Sources;
    int					menage_so;
    data_source_base*	Targets;
    bool				menage_t;
    data_source_base*	Arrows;
    bool				menage_a;
    data_source_base*	ArrColors;
    bool				menage_ac;

    //scaling_info	scale_s;	///< Skalowanie dla rozmiarów grotów
    scaling_info	scale_ac;	///< Skalowanie dla kolorów linii
};

// IMPLEMENTACJE "inline"
//=======================

template<class DATA_SOURCE, int DIRECT_COLOR>
void fast_carpet_graph<DATA_SOURCE, DIRECT_COLOR>::_replot()
{
    int x1 = get_start_x();
    int y1 = get_start_y();
    int x2 = x1 + get_width() - 1; //-1, bo `width` obejmuje pierwszy piksel.
    int y2 = y1 + get_height() - 1;
    assert(x1 <= x2); //Czy aby na pewno
    assert(y1 <= y2); //Sensowne okno? Może mieć zerowy rozmiar, ale nie ujemny.

    read_dim(AA, BB);
    if(AA <= 1 && BB <= 1)
    {
        print_width(x1, (y1 + y2) / 2, x2 - x1, t_colors.start, get_background(), "%@CInvalid data");
        return;
    }

    double min; //=0;
    double max; //=1;
    size_t num; //=0;
    data->bounds(num, min, max);
    //double missing=data->missing();

    //Do skalowania kolorów
    if(DIRECT_COLOR)
    {
        //NIEPOTRZEBNE?
        ///???
    } else
    {
        mm.set(min, max, c_range.end - c_range.start + 0.999);
    }

    //R y s o w a n i e  l e g e n d y
    if(t_colors.start != get_background() &&
       char_height('X') < get_height())
    {
        int x = x1;
        int y = (y2 - (int)char_height('X') + 1); //+1, bo y2 ma być zarysowane
        int width = 0;
//----------
        y2 = y; //Zabiera dolna część na legendę
//----------
        width = print_width(x, y, (x2 - x) / 3 * 2, t_colors.start, get_background(),
                            "%s", data->name());

        if(width == 0) goto KWADRACIKI;
        else x += width + 1;

        width = print_width(x, y, (x2 - x) / 2, c_range.start,
                            c_range.start != get_background()?get_background():c_range.end,
                            "<%g", min);

        if(width == 0) goto KWADRACIKI;
        else x += width + 1;

        /*width =*/ print_width(x, y, x2 - x, c_range.end,
                                c_range.end != get_background()?get_background():c_range.start,
                            ",%g>", max);
    }

    //Rysowanie skali — jeśli są co najmniej dwa kolory i jest miejsce na co najmniej 2 piksele.
    if(!DIRECT_COLOR)
        if((x2 - x1) >= double((AA > BB?AA:BB) + 6) &&
           //W zasadzie dobrze, ale male szanse, żeby przekroczyło zakres uint32
           c_range.end > c_range.start &&
           c_range.end - c_range.start >= 2)
        {
            c_range.plot(x2 - 6, y1, x2, y2);
            x2 -= 7;
        }

    //Rysowanie punktów vel "KWADRACIKÓW"  a właściwie tylko punktów
    KWADRACIKI:
    assert(x2 > x1);
    assert(y2 > y1);
    if(AA >= 1 && BB >= 1 && AA <= (x2 - x1 + 1) && BB <= (y2 - y1 + 1))
    {
        //Tu CAST na znany dobrze typ źródła — żeby był szybszy dostęp
        //----------------------------------------
        DATA_SOURCE *fast_data = (DATA_SOURCE *) data;
        //----------------------------------------
        assert(c_range.end - c_range.start >= 1);

        int i, j; //Indeksy po wierszach i kolumnach. Przecież w zupełności wystarczy 31 bitów!!!
        int width = x2 - x1 + 1; //już mogą być inne
        int height = y2 - y1 + 1; //Niż dla całego obszaru
        int offsetA = (AA < double(width)?(width - AA) / 2:0);
        int offsetB = (BB < double(height)?(height - BB) / 2:0);
        offsetA += x1;
        offsetB += y1;

        //Rysowanie
        data_source_base::iterator_h h = fast_data->reset();

        //wb_color back = get_background(); //Dla sprawdzania, kiedy kolor kwadratu taki jak kolor tła.

        //Pikselami panowie!!!
        if(DIRECT_COLOR)
        {
            for(j = 0; j < BB; j++)
                for(i = 0; i < AA; i++)
                {
                    double test = fast_data->get(h);
                    if(data->is_missing(test))
                        continue; //Nie rysować, jeśli wartość nieosiągalna
                    unsigned C = (unsigned)test; //Zakładamy, że to surowe kolory?
                    unsigned R = (C & 0x0000ff);
                    unsigned G = (C & 0x00ff00) >> 8;
                    unsigned B = (C & 0xff0000) >> 16;
                    //color+=c_range.start;
                    plot_rgb(offsetA + i, offsetB + j, R, G, B);
                }
        }
        else
        { //SKALA KOLORÓW
            for(j = 0; j < BB; j++)
                for(i = 0; i < AA; i++)
                {
                    double test;
                    wb_color color = (wb_color) (mm.get(test = fast_data->get(h)));
                    if(data->is_missing(test))
                        continue; //Nie rysować, jeśli wartość nieosiągalna
                    color += c_range.start;
                    plot(offsetA + i, offsetB + j, color);
                }
        }

        fast_data->close(h);
        x1 = offsetA + AA + 1; //Dla skali zostaje prawy margines TODO Niby nie używane...
    }
    else
    {
        print_width(x1, (y1 + y2) / 2, x2 - x1, t_colors.start, get_background(), "%@CTo small area");
    }
//NIE_DA_SIE:;
}

/*
class scatter3D_graph:public graph//Punkty w trzech wymiarach (dowolne X,Y,Z)
*/

} // namespace symshell2

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




