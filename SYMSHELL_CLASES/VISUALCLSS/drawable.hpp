/// @file
/// @brief DEKLARACJE SPRZĘGU DLA OBSZARÓW EKRANU. / COUPLING DECLARATIONS FOR SCREEN AREAS
/// @date 2026-05-04 (modified)
// ********************************************************************************************************************
//
#ifndef _DRAWABLE_HPP_
#define _DRAWABLE_HPP_

#ifndef __cplusplus
#error C++ required
#endif

#include <assert.h>
#include "wb_ptr.hpp"
using namespace wbrtm;

#include "sshutils.hpp"
#include "titleuti.hpp" //title_util


//Parametry obszaru wydzielonego na ekranie
//--------------------------------------------
class gps_area
//--------------------------------------------
{
    typedef double xyinfo; //Zapas na niewidoczne szczegóły :)
    friend class drawable_base;
    xyinfo x1,y1; //upper-left corner
    xyinfo x2,y2; //lower-right corner
    public:
    gps_area()	//default constructor
        {set(0,0,0,0);}

    gps_area(xyinfo ix1,xyinfo iy1,xyinfo ix2,xyinfo iy2)
        {set(ix1,iy1,ix2,iy2);}

    gps_area(const gps_area& p)
        {load(p);}

    void set(xyinfo ix1,xyinfo  iy1,xyinfo  ix2,xyinfo  iy2)
        {
        assert(ix1<=ix2 && iy1<=iy2);
        x1=xyinfo(ix1);x2=xyinfo(ix2);y1=xyinfo(iy1);y2=xyinfo(iy2);
        }

    xyinfo _x1(){return x1;}
    xyinfo _y1(){return y1;}
    xyinfo _x2(){return x2;}
    xyinfo _y2(){return y2;}

    int  operator == (const gps_area& p) const
        {
        return p.x1==x1 && p.x2==x2 && p.y1==y1 && p.y2==y2;
        }

    void add(const gps_area& p); //make area that hold p
    void load(const gps_area& p); //load settings from p
    void swap(gps_area& p); //load setting from p, but old settings to p

    int  translate(int& x,int& y); // change x,y relatively to area and return 1 if point is inside area

    void moveto(xyinfo ix1,xyinfo  iy1)	//Move area to point
    {
        xyinfo dx=x1-ix1;
        xyinfo dy=y1-iy1;
        x1-=dx; y1-=dy;
        x2-=dx; y2-=dy;
    }

    void get_transform_to(const gps_area& t,float tab[4]) const;
    void transform(float tab[4]);

    int  is_inside(xyinfo  x,xyinfo  y) const;
                    //return 1 if point is inside area, but is not embedded action
                    //return 0 if point is NOT inside area

    int  is_overlapped(const gps_area& t) const;
};

//Klasa bazowa dla obszarów ekranu, takich jak wykresy(graphs)
//------------------------------------------------------------
class drawable_base:public gps_area,public title_util
//------------------------------------------------------------
{
    int frame_width;
    wb_color frame_col;	//Kolor ramki. Jeśli == default_transparent to ramka wyłączona
    wb_color titbck;    //Kolor tła tytułu — jeśli DEFCOLOR to domyślne
    wb_color titcol;    //Kolor tytułu — jeśli DEFCOLOR to domyślne
    wb_color background; //kolor tła. Jeśli == default_transparent to tło przezroczyste!!!
                        //w metodzie replot() => można nakładać obszary.
    public:
    virtual  ~drawable_base(){ set_title(NULL);}
    //CONSTRUCTOR
            drawable_base(int ix1,int iy1,int ix2,int iy2,wb_color ibkg=default_half_gray,wb_color ifr=default_white):
            gps_area(ix1,iy1,ix2,iy2),background(ibkg),frame_col(ifr),title_util(NULL),titbck(ibkg),titcol(default_transparent),frame_width(def_frame_width)
    {}

    //ACCESSORS
    wb_color setbackground(wb_color color);	//{wb_color old=background;background=color;return old;}
    wb_color getbackground();				//{return background;}//Może być inny niż globalny!

    wb_color setframe(wb_color color);		//{wb_color old=frame_col;frame_col=color;return old;}
    wb_color getframe();					//{return frame_col;}
    int      getframewith();                //{return frame_width;}

    wb_color settitlecolor(wb_color color);	//{wb_color old=titbck;titbck=color;return old;}
    wb_color settitleback(wb_color color);	//{wb_color old=titcol;titcol=color;return old;}
    void     settitlecolo(wb_color color,wb_color back); //{titcol=color;titbck=back;}

    int getstartx(); //Początek x obszaru użytkowego
    int getstarty(); //Początek y obszaru użytkowego
    int getwidth (); //Szerokość obszaru użytkowego
    int getheight(); //Wysokość obszaru użytkowego

    //		ACTIONS
    // ----------------

    //Ponizsze metody z definicji nie robią nic, jeśli rozmiary obszaru wynoszą 0x0. Warunkowo robią flush.
    void clear(int flush=1); //Czyści obszar kolorem background, ustalonym dla platformy (SYMSHELL'a np)
    void replot(int flush=1); //Rysuje ramkę, może tytuł i wirtualnie zawartość.
    void flush();			 //albo samo flush plot area.

    // METODA KONIECZNA W KLASACH POTOMNYCH
    virtual
    void _replot(); //Implementuje rysowanie zawartości
                    // dla obszaru 0x0 w ogóle nie jest wywoływana!!!

    // METODY REAKCJI NA ZDAZENIA
    //---------------------------------
    virtual int  on_click(int x,int y,int /*click*/=0)
                //return 2 if area got a position message for embedded action
                //return 1 if point is inside area, but is not embedded action
                //return 0 if point is NOT inside area
                //For a subarea managing.
                //Default action — test only.
    { return is_inside(x,y);}

    virtual int on_change(const gps_area& /*new_position*/)	//Need to call this if want to resize or move area.
                //return 1 if OK
                //return 0 if area is not resizeable/movable,
                //but always need to be possible to resize area to 0x0 == deactivate.
    { return 1;}

    virtual int on_input(int /*input_char*/)	//Manager can call it for (active) area
                //return 0 if area doesn't want input
                //return 1 if area has processed this input
    { return 0;}

    virtual int on_idle() //Do wywołania, jeśli program nie ma nic lepszego do roboty
                //zwraca 0, jeśli nie wykorzystał sensownie czasu procesora (możliwa optymalizacja)
                //zwraca 1, jeśli czas procesora został użyty.
    { return 0;}
};

inline wb_color drawable_base::setbackground(wb_color color)	
{
    wb_color old=background;
    background=color;
    return old;
}

inline wb_color drawable_base::getbackground()				
{
    return background; //Może być inny niż globalny!
}

inline wb_color drawable_base::setframe(wb_color color)		
{
    wb_color old=frame_col;
    frame_col=color;
    return old;
}

inline wb_color drawable_base::getframe()						
{
    return frame_col;
}

inline int      drawable_base::getframewith()                 
{
    return frame_width;
}

inline wb_color drawable_base::settitlecolor(wb_color color)	
{
    wb_color old=titbck;
    titbck=color;
    return old;
}

inline wb_color drawable_base::settitleback(wb_color color)	
{
    wb_color old=titcol;
    titcol=color;
    return old;
}

inline void     drawable_base::settitlecolo(wb_color color,wb_color back) 
{
    titcol=color;
    titbck=back;
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


