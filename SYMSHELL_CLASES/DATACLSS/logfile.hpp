/// @file
/// @brief __The base type of the interface for writing data from single-value sources to a file and its simplest implementation.__ /<br>
/// _Klasa bazowa interface-u zapisywania do pliku danych ze źródeł jednowartościowych oraz jego najprostsza implementacja._
/// @date 2026-05-18 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_LOG_FILE_HPP_INCLUDED_
#define SYMSHELL2_LOG_FILE_HPP_INCLUDED_

#include <ctime>
#include <fstream>
using namespace std;

#include "datasour.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {

/// INTERFACE bazowy zapisywania do pliku danych ze źródeł jednowartościowych.
class data_logfile_base
//---------------------
{
    /// Znak separujący kolumny ustawiany w konstruktorze, który nie może być potem zmieniony.
    char separator_char;

protected:
    /// Konstruktor do użytku klas potomnych
    explicit data_logfile_base(char init_separator) : separator_char(init_separator)
    {}

    /// Zapewnienie wirtualności destruktorów.
    virtual ~data_logfile_base() = default;

public:
    /// Łączy serię jednowartościową z kolumną pliku log. Wymusza wyplucie nagłówka w `try_writing`.
    virtual int link_data_source(size_t column, scalar_source_base *Source) = 0;

    /// WYMAGANA: Próba zapisu do pliku.
    /// Jeśli choć jedna seria ma nową wersję danych, to wypluwa całą linię danych.
    /// Jeśli użyto `link_data_source` to wypluwa też wiersz nagłówków.
    virtual int try_writing() = 0;

    /// Akcesor separatora.
    char separator() const
    { return separator_char; }
};

/// NAJPROSTSZA IMPLEMENTACJA ZAPISYWANIA DO PLIKÓW `log`.
class logfile : public data_logfile_base
//-----------------------------------------------------
{
    title_util missing_string;
    int          need_headers;
    size_t        next_column;

    ostream       &log_stream;
    long    curr_data_version;

    /// Tablica zarejestrowanych źródeł skalarnych.
    wb_dynarray<scalar_source_base *> table;

protected:
    /// Podaje ostatnio wykrytą wersję danych.
    long GetVersion() const
    { return curr_data_version; }

    /// Sprawdza, czy źródło dla którejś z kolumn się nie zmieniło.
    int CheckVersion();

public:
    /// Konstruktor.
    /// \param     max_column to maksymalna przewidywana liczba kolumn logu, więc i zarejestrowanych źródeł.
    /// \param       filename to nazwa pliku logu.
    /// \param init_separator to inicjator separatora.
    /// \param       miss_str to łańcuch tekstowy wstawiany w miejsce brakujących wartości ("missing values").
    /// \param         n_mode to tryb otwarcia pliku logu. Ważne czy jest `ios::app`, czy nie.
    logfile(size_t     max_column,
            const char  *filename,
            char  init_separator = '\t',
            const char *miss_str = "-9999",
            ios::openmode n_mode = ios::out | ios::app )
    : data_logfile_base(init_separator),
      table(max_column),
      need_headers(1),
      next_column(0),
      curr_data_version(-1),
      missing_string(miss_str),
      log_stream( *(new ofstream(filename, n_mode)) )  ///< @note Destruktor zwalnia także pamięć ofstream-u, więc nie można dać statycznego!
    {
        time_t ltime;
        time(&ltime);
        assert(log_stream);
        log_stream << '\n' << "EXP:" << separator() << ctime(&ltime);
        //Zerowanie tablicy wskaźników
        for(size_t i = 0; i < max_column; i++)
            table[i] = NULL;
    }

    /// Wirtualny destruktor. Zwalnia pamięć, także pamięć ofstream-u, więc nie można dać statycznego.
    ~logfile() override;

    /// Akcesor strumienia — na wypadek konieczności dopisania czegoś ręcznie. Używać ostrożnie!
    ostream &GetStream()
    { return log_stream; }

    /// Akcesor liczby zajętych kolumn.
    size_t GetNumberOfColumns() const
    { return next_column; }

    /// Łączy serię jednowartościową z kolumną pliku log.
    /// Wymusza wyplucie nagłówka w `try_writing`. NULL jako parametr `Source` oznacza usunięcie łącza.
    int link_data_source(size_t column, scalar_source_base *Source = NULL) override;

    int insert(scalar_source_base *Source)
    { return link_data_source(GetNumberOfColumns(), Source); }

    /// Jeśli choć jedna seria ma nową wersję danych, to wypluwa całą linię danych.
    /// Jeśli użyto `link_data_source` to wypluwa też wiersz nagłówków.
    int try_writing() override;

    /// Inteligentnie "Zeruje" (tzn. -1!!!) wersje źródeł.
    /// Wszystkich aktualnie niewyzerowanych, ale ze względu na te, którymi nie zarządza właściwy zarządca (np. pod-źródła).
    void restart_data_version();
};

}} // end-of-namespaces sym2::data

#pragma clang diagnostic pop
/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
#endif

