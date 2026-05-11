/// @file
/// @brief Wyświetlanie pliku HTML poprzez system shell.
/// @date 2026-05-11 (modified)
/// @note Cała biblioteka WB_RTM to jest nieco odremontowane muzeum kodu z wieku XX., ale ten pomysł wciąż działa :-)
#ifndef WB_RTM_VIEW_HTML_HPP_INCLUDED_
#define WB_RTM_VIEW_HTML_HPP_INCLUDED_
#include <string>

/// @name Odpalanie pliku w przeglądarce WWW albo w ogóle w  viewerze/edytorze domyślnym w danym systemie.
/// @details tak naprawdę to wszystkiego, co może wyświetlić przeglądarka
/// @{
bool ViewHtml(std::string URL);
int  ViewHtml(const char* URL);
/// @}

/* ******************************************************************/
/*                      WBRTM  version 2026                         */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif

