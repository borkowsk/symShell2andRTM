/// @file
/// @brief **BASIC INTERFACE DECLARATIONS FOR SCREEN AREAS** /<br>
///         _DEKLARACJE PODSTAWOWEGO "SPRZĘGU" DLA OBSZARÓW EKRANU._
/// @date 2026-06-02 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_DRAWABLE_HPP_INCLUDED_
#define SYMSHELL2_DRAWABLE_HPP_INCLUDED_

#include <cassert>

#include "wb_ptr.hpp"
#include "sshutils.hpp"
#include "titleuti.hpp" //title_util

using namespace wbrtm;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace visual
{

/// @brief @EN{ Parameters of a designated area on the screen or in a window. }
///        @PL{ Parametry obszaru wydzielonego na ekranie albo w oknie. }
class gps_area
//------------------------------------------------------
{
public:
    /// @brief @EN{ A type that stores the required coordinate of the area. }
    ///        @PL{ Typ przechowujący wymagane współrzędne obszaru. }
    /// @detail
    ///      The fractional part has room for invisible details, so you can scale the areas in a wide range of sizes.<br>
    ///      W części ułamkowej jest zapas na niewidoczne szczegóły.
    ///      Dzięki temu można wykonywać skalowanie obszarów w szerokim zakresie rozmiarów.
    typedef double xy_info;

    /// @brief @EN{ Access to private attributes of the child type. }
    ///        @PL{ Dostęp do atrybutów prywatnych klasy potomnej. }
    friend class drawable_base;

private:
    xy_info x1,y1; ///< @EN{ upper-left corner. } @PL{ lewy górny róg. }
    xy_info x2,y2; ///< @EN{ lower-right corner. } @PL{ prawy dolny róg. }

public:
    /// Default constructor.
    gps_area()
        { set(0,0,0,0); }

    /// Regular constructor.
    gps_area(xy_info ix1, xy_info iy1, xy_info ix2, xy_info iy2)
        { set(ix1,iy1,ix2,iy2); }

    /// Copy constructor.
    gps_area(const gps_area& p)
        { load(p); }

    /// Sets all fields.
    void set(xy_info ix1, xy_info  iy1, xy_info  ix2, xy_info  iy2)
    {
        if(ix1>ix2)
        {
            cerr<<"Invalid area x1:"<<ix1<<"> x2:"<<ix2<<endl;
        }
        if(iy1>iy2)
        {
            cerr<<"Invalid area y1:"<<iy1<<"> y2:"<<iy2<<endl;
        }
        x1=xy_info(ix1); x2=xy_info(ix2);
        y1=xy_info(iy1); y2=xy_info(iy2);
    }

    /// @name READ-ONLY ACCESSORS.
    /// @{
    xy_info _x1() const { return x1; }
    xy_info _y1() const { return y1; }
    xy_info _x2() const { return x2; }
    xy_info _y2() const { return y2; }
    /// @}

    /// Compare by all fields.
    int  operator == (const gps_area& p) const
    {
        return p.x1==x1 && p.x2==x2 && p.y1==y1 && p.y2==y2;
    }

    /// Makes area that holds `p`.
    void add(const gps_area& p);

    /// Loads settings from `p`.
    void load(const gps_area& p);

    /// Loads setting from `p`, but save old settings to `p`.
    void swap(gps_area& p);

    /// Changes `x` and `y` relatively to area and returns `1` if point is inside area.
    int  translate(int& x,int& y) const;

    /// Move this area to point.
    void moveto(xy_info ix1, xy_info  iy1)
    {
        xy_info dx= x1 - ix1;
        xy_info dy= y1 - iy1;
        x1-=dx; y1-=dy;
        x2-=dx; y2-=dy;
    }

    /// Prepares a transformation based on two areas and places the result in an array.
    void get_transform_to(const gps_area& t,float out_tab[6]) const;

    /// Performs the transformation given in the table on this area.
    void transform(const float in_tab[6]);

    /// Checks whether the given point lies within the area.
    /// @return 1 if point is inside area, but is not embedded action,
    ///         but 0, if point is NOT inside area
    int  is_inside(xy_info  x, xy_info  y) const;

    /// Checks whether two areas overlap.
    int  is_overlapped(const gps_area& t) const;
};

/// @brief @EN{ Base type for screen areas such as graphs. }
///        @PL{ Klasa bazowa dla obszarów ekranu, takich jak wykresy(graphs). }
/// @details ...
class drawable_base:public gps_area,public sym2::title_util
//------------------------------------------------------------
{
    int     frame_width; ///< Szerokość ramki obszaru.
    wb_color  frame_col; ///< Kolor ramki. Jeśli `== default_transparent` to ramka wyłączona.
    wb_color    tit_bck; ///< Kolor tła tytułu — jeśli `default_color` to domyślny.
    wb_color    tit_col; ///< Kolor tytułu — jeśli `default_color` to domyślny.
    wb_color background; ///< Kolor tła. Jeśli `== default_transparent` to tło przezroczyste.
                         ///< Wtedy w metodzie `replot` można nakładać obszary.
public:
    /// CONSTRUCTOR.
    drawable_base(int ix1,int iy1,int ix2,int iy2,wb_color ibkg=default_half_gray,wb_color ifr=default_white)
    : gps_area(ix1,iy1,ix2,iy2), background(ibkg), frame_width(def_frame_width), frame_col(ifr),
      title_util(NULL), tit_bck(ibkg), tit_col(default_transparent)
    {}

    /// Destructor. Need to free up title memory.
    ~drawable_base() override{ set_title(NULL);}

// ACCESSORS:
//===========
    wb_color set_background(wb_color color);	///< Accessor: `{wb_color old=background;background=color;return old;}`
    wb_color get_background() const;			///< Accessor: `{return background; }`... Może być inny niż globalny!

    wb_color set_frame(wb_color color);			///< Accessor: `{wb_color old=frame_col;frame_col=color;return old;}`
    wb_color get_frame() const;					///< Accessor: `{return frame_col;}`
    int      get_frame_with() const;			///< Accessor: `{return frame_width;}`

    wb_color set_title_color(wb_color color);	///< Accessor: `{wb_color old=tit_bck;tit_bck=color;return old;}`
    wb_color set_title_back(wb_color color);	///< Accessor: `{wb_color old=tit_col;tit_col=color;return old;}`
    void     set_title_colors(wb_color color, wb_color back); ///< Accessor: `{tit_col=color;tit_bck=back;}`

    int get_start_x();	///< @brief @PL{ Początkowe `x` obszaru użytkowego. } @EN{ Beginning `x` of usable area. }
    int get_start_y();	///< @brief @PL{ Początkowe `y` obszaru użytkowego. } @EN{ Initial `y` of the usable area. }
    int get_width ();	///< @brief @PL{ Szerokość obszaru użytkowego. } @EN{ Width of usable area. }
    int get_height();	///< @brief @PL{ Wysokość obszaru użytkowego. } @EN{ Height of usable area. }

//		ACTIONS:
// -------------

    /// @name  @EN{ Main drawing methods.} @PL{ Główne metody związane z rysowaniem.}
    /// @details The first three do nothing if the area dimensions are 0x0. They conditionally flush.
    ///          They are also declared as virtual, but you should be very careful about overriding them!
    ///          Trzy pierwsze nie robią nic, jeśli rozmiary obszaru wynoszą `0 × 0`. Warunkowo robią flush.
    ///          Zostały też zadeklarowane jako wirtualne, ale należy bardzo uważać z ich przesłanianiem!
    /// @{

    /// @brief @EN{ Draws a frame, maybe a title and virtual content by calling `_replot()`. }
    ///        @PL{ Rysuje ramkę, może tytuł i wirtualnie zawartość wołając `_replot`. }
    virtual void replot(int flush=1);

    /// @brief @EN{ Clears the area with the background color set for the platform (e.g., SYMSHELL). }
    ///        @PL{ Czyści obszar kolorem background, ustalonym dla platformy (SYMSHELL-a np.). }
    virtual void clear(int flush=1);

    /// @brief @EN{ Guarantees content transfer to a screen/window/vector file (etc.). }
    ///        @PL{ Gwarantuje przesłanie zawartości na ekran/okno/plik wektorowy (etc.). }
    virtual void flush();

    // METODA KONIECZNA W KLASACH POTOMNYCH:
    //======================================

    /// @brief  @EN{ Implementation of area redrawing } @PL{ Implementacja odrysowywania obszaru. }
    /// @detail Implements content drawing for area. For area `0 × 0`, it is not called at all!!!<br>
    ///         Implementuje rysowanie zawartości. Dla obszaru `0 × 0` w ogóle nie jest wywoływana!!!
    virtual void _replot()=0;
    /// @}
// METHODS OF RESPONDING TO EVENTS/METODY REAKCJI NA ZDARZENIA.
//------------------------------------------------------------

    /// @brief @EN{ Mouse click response. } @PL{ Obsługa kliku myszką. }
    /// @returns
    ///     * 2 if area got a position message for embedded action.
    ///     * 1 if point is inside area, but is not embedded action.
    ///     * 0 if point is NOT inside the area.
    /// @details For a subarea managing. Default action — point test only!
    virtual int  on_click(int x,int y,int /*click*/=0)
    { return is_inside(x,y);}

    /// Reaction to area size change.
    /// Always need to be possible to resize area to 0x0 == deactivate.
    /// @details Need to call this if you want to resize or move area.
    /// @returns
    ///     * 1 if OK
    ///     * 0 if area is not resizeable/movable.
    virtual int on_change(const gps_area& /*new_position*/) { return 1; }

    /// Reaction to character input.
    /// Manager can call it for (active) area.
    /// @return
    ///     * 0 if area doesn't want input
    ///     * 1 if area has processed this input
    virtual int on_input(int /*input_char*/) { return 0;}

    /// Called if the program has nothing better to do.
    /// @returns 0 if it did not use the processor time sensibly (possible optimization)
    ///          or 1 if CPU time has been used.
    virtual int on_idle() { return 0;}
};

// INLINE IMPLEMENTATIONS:
//========================

inline wb_color drawable_base::set_background(wb_color color)
{
    wb_color old=background;
    background=color;
    return old;
}

inline wb_color drawable_base::get_background() const
{
    return background; //Może być inny niż globalny!
}

inline wb_color drawable_base::set_frame(wb_color color)
{
    wb_color old=frame_col;
    frame_col=color;
    return old;
}

inline wb_color drawable_base::get_frame() const
{
    return frame_col;
}

inline int      drawable_base::get_frame_with() const
{
    return frame_width;
}

inline wb_color drawable_base::set_title_color(wb_color color)
{
    wb_color old=tit_bck;
    tit_bck=color;
    return old;
}

inline wb_color drawable_base::set_title_back(wb_color color)
{
    wb_color old=tit_col;
    tit_col=color;
    return old;
}

inline void     drawable_base::set_title_colors(wb_color color, wb_color back)
{
    tit_col=color;
    tit_bck=back;
}

}} // namespace sym2

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
#endif


