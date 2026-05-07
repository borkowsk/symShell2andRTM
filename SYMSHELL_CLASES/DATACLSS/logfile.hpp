/// @file
/// @brief Klasa bazowa interface-u zapisywania do pliku danych ze źródeł jednowartościowych
///        oraz najprostsza klasa implementująca taki interface.
/// @date 2026-05-07 (modified)
// ********************************************************************************************************************
//
#ifndef __LOGFILE_HPP__
#define __LOGFILE_HPP__

#include <time.h>
//#include "platform.hpp"

#include <fstream>

using namespace std;

#include "datasour.hpp"

namespace sym2 { namespace data {

/// INTERFACE bazowy zapisywania do pliku danych ze źródeł jednowartościowych.
class data_logfile_base
//---------------------
{
//Znak separujacy kolumny ustawiany w konstruktorze i nie może być potem zmieniony
    int separator_char;
protected:
// Konstruktor dla klas potomnych
    data_logfile_base(int init_separator) : separator_char(init_separator)
    {}

public:
//Zapewnienie wirtualnosci destruktor�w
    virtual ~data_logfile_base()
    {}

//Laczy serie jednowartosciowa z kolumna pliku log. Wymusza wyplucie naglowka w try_writing
    virtual int link_data_source(size_t column, scalar_source_base *Source) = 0;

//Jeśli choc jedna seria ma nowa wersje to wypluwa cala linie danych. Jeśli użyto link_data_source
//to wypluwa tez wiersz nagl�wk�w
    virtual int try_writing() = 0;

//Akcesor separatora
    char separator()
    { return separator_char; }
};

//NAJPROSTSZA IMPLEMENTACJA
class logfile : public data_logfile_base
//-----------------------------------------------------
{
    title_util missing_string;
    int need_headers;
    size_t next_column;
    ostream &log_stream;
    long curr_data_version;
    wb_dynarray<scalar_source_base *> table;

protected:
    long GetVersion()
    { return curr_data_version; }

    int CheckVersion(); //Sprawdza, czyktóraś kolumna się nie zmieni�a
public:
// Konstruktor dla klas potomnych
    logfile(size_t maxcolumn, const char *filename,
            int init_separator = '\t', const char *missstr = "-9999",
            ios::openmode nMode = ios::out | ios::app) :
            data_logfile_base(init_separator),
            table(maxcolumn),
            need_headers(1),
            next_column(0),
            curr_data_version(-1),
            missing_string(missstr),
            log_stream(*(new ofstream(filename, nMode)))  //Otwarcie logu
    {
        time_t ltime;
        time(&ltime);
        assert(log_stream);
        log_stream << '\n' << "EXP:" << separator() << ctime(&ltime);
        //Zerowanie tablicy wskaźników
        for(size_t i = 0; i < maxcolumn; i++)
            table[i] = NULL;
    }

//Wirtualny destruktor
    ~logfile();

//Akcesor strumienia — na wypadek koniecznosci dopisania czegos — używac ostroznie!
    ostream &GetStream()
    { return log_stream; }

//Akcesor ilośći zajetych kolumn
    size_t GetNumberOfColumns()
    { return next_column; }

//Laczy serie jednowartosciowa z kolumna pliku log. Wymusza wyplucie naglowka w try_writing
    int link_data_source(size_t column, scalar_source_base *Source = NULL); //NULL oznacza brak lacza
    int insert(scalar_source_base *Source)
    { return link_data_source(GetNumberOfColumns(), Source); }

//Jeśli choc jedna seria ma nowa wersje to wypluwa cala linie danych. Jeśli użyto link_data_source
//to wypluwa tez wiersz nagłówków.
    int try_writing();

//Inteligentnie "Zeruje" (tzn -1!!!) wersje źródeł — wszystkich niewyzerowanych,
//ale ze wzgledu na te którymi nie zarządza właściwy menager(np podźrodła)
    void restart_data_version();
};

}} // end of namespaces sym2::data

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

