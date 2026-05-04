/// @file
/// @brief OBSZAR OKNA ("LUFCIK") NAŚLADUJĄCY TERMINAL
/// @date 2026-05-04 (modified)
// ********************************************************************************************************************
//
#ifndef __TEXT_AREA_HPP__
#define __TEXT_AREA_HPP__
#ifndef __cplusplus
#error C++ required
#endif

#include "drawable.hpp"

/// @brief KLASA OBSZARU "TELETYPE".
/// Obsługuje tylko tekst. Mogą być w nim znaki \n oraz %@R^L^C zaraz po nich.
class text_area:public drawable_base
//----------------------------------
{
protected:
    size_t				user_size;	//Rozmiar bufora na linie. -1 ustawiany przez set_text
    size_t					index;	//Pierwsza wolna pozycja listy linii
    wb_color			curr_col;	//Aktualny kolor tekstu
    wb_dynarray< wb_dynarray<char> >  linie; //Bufor z liniami.
public:
    //KONSTRUKCJA / DESTRUKCJA
    text_area(int ix1,int iy1,int ix2,int iy2,
            const char* itext,		//TEXT inicjujący
            unsigned icolor=default_black,
            unsigned ibackground=default_white,
            unsigned iframe=128,
            size_t   buffsize=size_t(-1)	//Inicjalny rozmiar bufora.
            );

    ~text_area(){ clean(); }

    //AKCESORY
    wb_color	gettextcolor() {return curr_col;}
    void		settextcolor(wb_color nw) {curr_col=nw;}
    void		clean();
    int			add_text(const char* ini); //ret 1, jeśli OK
    int			add_line(const char* ln); //dodaje linie na końcu. W razie co przesuwa do gory bufor.
    void		_replot(); //Wypisuje i już
};

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




