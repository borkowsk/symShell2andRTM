/// @file
/// @brief **Implementation of the simplest type for writing data from single-value sources to a file** /<br>
///         _Implementacja najprostszej klasy zapisywania do pliku danych ze źródeł jednowartościowych._
/// @date 2026-05-19 (modified)
//======================================================================================================================

#include "logfile.hpp"
#include <ctime>
using namespace sym2::data;;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

logfile::~logfile()
//Wirtualny destruktor
{
    delete &log_stream;
}

int logfile::link_data_source(size_t column,scalar_source_base* Source)
//Łączy serię jednowartościową z kolumną pliku log. Wymusza wyplucie nagłówka w `try_writing`.
{
    if(column>=table.get_size()) return 0; //Przekroczony zakres
    need_headers=1;
    table[column]=Source;
    if(column+1>next_column)
        next_column=column+1;
    return 1;
}

void  logfile::restart_data_version()
{
    size_t N=table.get_size();
    data_source_base* pom=NULL;
    for(size_t i=0;i<N;i++)
        if( (pom=table[i])!=NULL ) //Czy nie pusty
            if(pom->data_version()!=-1) //Czy już nie wyzerowany
                pom->restart_counting();
    curr_data_version=-1;
}


int logfile::try_writing()
//Jeśli choć jedna seria ma nową wersję, to wypluwa cala linie danych. Jeśli użyto link_data_source
//to wypluwa też wiersz nagłówków
{
    size_t i=0;
    if(CheckVersion())
        {
        if(need_headers!=0)
            {
            need_headers=0; //Zapiszemy
            log_stream<<"DATA_VER"<<separator();
            for(i=0;i<next_column;i++)
                {
                scalar_source_base* pom=table[i];
                if(pom!=nullptr)
                    log_stream<<pom->name();
                log_stream<<separator();
                }
            log_stream<<"CLOCK"<<'\n';
            log_stream.flush();
            }

        log_stream<<GetVersion()<<separator();
        for(i=0;i<next_column;i++)
        {
                if(table[i]!=nullptr)
                {
                    data_source_base::iterator_h in=table[i]->reset();
                    double pom;
                    // Odczyt wartości tylko wtedy gdy "in" nie jest z jakiś powodów pustym wskaźnikiem!
                    if(in==nullptr || (pom=table[i]->get(in))==table[i]->get_missing() )
                    {
                        log_stream<<missing_string.name(); //No data!
                    }
                    else
                    {
                        log_stream<<pom;
                    }
                    table[i]->close(in);
                }
            log_stream<<separator();
        }
        log_stream<<clock()<<'\n';
        log_stream.flush();
        return 1; //Zapisywał
        }
    return 0; //Nie zapisywał
}

int logfile::CheckVersion()
//Sprawdza, czy któraś kolumna się nie zmieniła
{
    for(size_t i=0;i<next_column;i++)
        if(table[i]!=NULL)
            {
            long data=table[i]->data_version();
            if(data>GetVersion()) //Znalazł jedna nowsza
                {
                curr_data_version=data;
                return 1;
                }
            }
    return 0;
}

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
