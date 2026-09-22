/// @file
/// @brief
///        @PL{ Wyświetlanie pliku HTML lub innego obsługiwanego przez przez system. }
///        @EN{ Displaying an HTML or other file supported by the system. }
/// @date 2026-05-19 (modified)
///       ====================================================
/// @note Cała biblioteka WB_RTM to jest nieco odremontowane muzeum kodu z wieku XX, ale ten pomysł wciąż działa :-)
#ifndef WB_RTM_VIEW_HTML_HPP_INCLUDED_
#define WB_RTM_VIEW_HTML_HPP_INCLUDED_
#include <string>

/// @name Odpalanie pliku w przeglądarce WWW albo w ogóle w viewerze/edytorze domyślnym w danym systemie.
/// @details tak naprawdę to wszystkiego, co może wyświetlić przeglądarka
/// @{
bool ViewHtml(std::string URL);
int  ViewHtml(const char* URL);
inline bool view_html(std::string URL) { return ViewHtml(URL); }
inline int  view_html(const char* URL) { return ViewHtml((const char*)URL); }
/// @}

/// @brief Funkcja rozwija w stringu zmienne systemowe i '~' jako równoważnik $HOME.
/// @param input_path - string wejściowy, najprawdopodobniej jakaś ścieżka.
/// @note Implementacja w pliku "expand_enviroment_variables.cpp"
std::string expand_environment_variables(const std::string& input_path);

/// @brief Funkcja udostępnia `expand_environment_variables` dla składni wielbłądowej.
inline std::string ExpandEnvironmentVariables(const std::string& input_path)
{ return expand_environment_variables(input_path); }

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

