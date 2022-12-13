//Klasa bazowa interface'u zapisywania do pliku danych ze zrodel jednowartosciowych
//oraz najprostrza klasa implementujaca taki interface
///////////////////////////////////////////////////////////////////////////////////
#ifndef __LOGFILE_HPP__
#define __LOGFILE_HPP__

#include <time.h>
//#include "platform.hpp"

#include <fstream>
using namespace std;

#include "datasour.hpp"

//INTERFACE - KLASA BAZOWA
class data_logfile_base
//-----------------------------------------------------
{
//Znak separujacy kolumny ustawiany w konstruktorze i nie moze byc potem zmieniony
int separator_char;
protected:
// Konstruktor dla klas potomnych
	data_logfile_base(int init_separator):separator_char(init_separator){}
public:
//Zapewnienie wirtualnosci destruktor�w
virtual ~data_logfile_base(){}
//Laczy serie jednowartosciowa z kolumna pliku log. Wymusza wyplucie naglowka w try_writing
virtual int link_data_source(size_t column,scalar_source_base* Source)=0;
//Jesli choc jedna seria ma nowa wersje to wypluwa cala linie danych. Jesli uzyto link_data_source
//to wypluwa tez wiersz nagl�wk�w
virtual int try_writing()=0;
//Akcesor separatora
char separator(){return separator_char;}
};

//NAJPROSTRZA IMPLEMNTACJA 
class logfile:public data_logfile_base
//-----------------------------------------------------
{
title_util	missing_string;
int			need_headers;
size_t		next_column;
ostream&	log_stream;
long		curr_data_version;
wb_dynarray< scalar_source_base* > table;

protected:
long	GetVersion(){ return curr_data_version;} 
int		CheckVersion();//Sprawdza czy ktoras kolumna sie nie zmieni�a
public:
// Konstruktor dla klas potomnych
	logfile(size_t maxcolumn,const char* filename,
			int init_separator='\t',const char* missstr="-9999",
			ios::openmode nMode = ios::out|ios::app):
	  data_logfile_base(init_separator),
	  table(maxcolumn),
	  need_headers(1),
	  next_column(0),
	  curr_data_version(-1),
	  missing_string(missstr),
	  log_stream(*(new ofstream(filename,nMode)))  //Otwarcie logu
	  {
	  time_t ltime;
	  time( &ltime );	
													assert(log_stream);
	  log_stream<<'\n'<<"EXP:"<<separator()<<ctime( &ltime );
	  //Zerowanie tablicy wskaznikow
	  for(size_t i=0;i<maxcolumn;i++)
		  table[i]=NULL;
	  }
//Wirtualny destruktor
~logfile();
//Akcesor strumienia - na wypadek koniecznosci dopisania czegos - uzywac ostroznie!
ostream& GetStream(){return log_stream;}
//Akcesor ilosci zajetych kolumn
size_t  GetNumberOfColumns(){return next_column;}
//Laczy serie jednowartosciowa z kolumna pliku log. Wymusza wyplucie naglowka w try_writing
int		link_data_source(size_t column,scalar_source_base* Source=NULL);//NULL oznacza brak lacza
int		insert(scalar_source_base* Source){return link_data_source(GetNumberOfColumns(),Source);}
//Jesli choc jedna seria ma nowa wersje to wypluwa cala linie danych. Jesli uzyto link_data_source
//to wypluwa tez wiersz nagl�wk�w
int		try_writing();
//Inteligentnie "Zeruje" (tzn -1!!!) wersje zrodel - wszystkich niewyzerowanych, 
//ale ze wzgledu na te ktorymi nie zarzadza wlasciwy menager(np podzrodla)
void  restart_data_version();
};

#endif
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/
