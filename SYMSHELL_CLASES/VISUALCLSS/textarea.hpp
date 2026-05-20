/// @file
/// @brief **WINDOW AREA ("LUFCIK") IMITATING A TERMINAL**/<br>
///         _OBSZAR OKNA ("LUFCIK") NAŚLADUJĄCY TERMINAL._
/// @date 2026-05-20 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_TEXT_AREA_HPP_INCLUDED_
#define SYMSHELL2_TEXT_AREA_HPP_INCLUDED_
#ifndef __cplusplus
#error C++ required
#endif

#include "drawable.hpp"

/// Zmodernizowane klasy do symulacji w C++.
namespace sym2 { namespace visual
{

/// @brief @EN{ . }
///        @PL{ KLASA OBSZARU "TELETYPE". }
/// @details Obsługuje tylko tekst. Mogą być w nim znaki \n oraz %@R^L^C zaraz po nich.
class text_area:public drawable_base
//----------------------------------
{
protected:
    size_t				user_size;	///< Rozmiar bufora na linie. -1 ustawiany przez set_text
    size_t					index;	///< Pierwsza wolna pozycja listy linii
    wb_color			 curr_col;	///< Aktualny kolor tekstu
    wb_dynarray< wb_dynarray<char> >  linie; ///< Bufor z liniami.

public:
    /// @name KONSTRUKCJA / DESTRUKCJA
    /// @{

    /// Konstruktor.
    /// \param ix1, iy1 lewy górny róg obszaru.
    /// \param ix2, iy2 prawy dolny róg obszaru.
    /// \param i_text TEKST inicjujący.
    /// \param i_color kolor tekstu (?).
    /// \param i_background kolor tła.
    /// \param i_frame kolor ramki.
    /// \param buff_size 	Inicjalny rozmiar bufora.
    text_area(int ix1,int iy1,int ix2,int iy2,
            const char*	i_text,
            unsigned	i_color=default_black,
            unsigned	i_background=default_white,
            unsigned	i_frame=128,
            size_t		buff_size=size_t(-1)
            );

    /// Wirtualny destructor. Zwalnia tekst.
    ~text_area() override{ clean(); }
    /// @}

    /// @name AKCESORY
    /// @{
    /// Podaje aktualny kolor tekstu.
    wb_color	get_text_color() const {return curr_col;}
    /// Ustawia nowy kolor tekstu.
    void		set_text_color(wb_color nw) { curr_col=nw;}
    /// Usuwa cały tekst.
    void		clean();
    /// Dodaje dużo tekstu. @returns 1, jeśli OK.
    int			add_text(const char* ini);
    /// Dodaje linie tekstu na końcu. W razie potrzeby przesuwa bufor do góry.
    int			add_line(const char* ln);
    /// @}

    // WYMAGANA REIMPLEMENTACJA:
    //--------------------------
    void		_replot() override;
};

}} // namespace sym2

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




