// Implementacja najprostszej klasy zapisywania do pliku danych ze źródeł jednowartościowych
//*///////////////////////////////////////////////////////////////////////////////////////////
#include "logfile.hpp"
#include <time.h>

/*
class logfile:public logfile_base
//-----------------------------------------------------
{
protected:
ostream& log_stream;
wb_dynarray< scalar_source* > table;
public:
// Konstruktor dla klas potomnych
	logfile(size_t maxcolumn,const char* filename, int nMode = ios::out|ios::app,int init_separator='\t'):
	  logfile_base(init_separator),
	  table(maxcolumn),	
	  LogStream(new ofstream(filename,nMode))
	  {
	  assert(&LogStream!=NULL);
	  }
//Wirtualny destruktor
~logfile_base();
//Akcesor strumienia - na wypadek koniecznosci dopisania czegos - uzywac ostroznie!
ostream& GetStream(){return log_stream;}
//Laczy serie jednowartosciowa z kolumna pliku log. Wymusza wyplucie naglowka w try_writing
int link_data_source(size_t column,scalar_source_base* Source);
//Jesli choc jedna seria ma nowa wersje to wypluwa cala linie danych. Jesli uzyto link_data_source
//to wypluwa tez wiersz nagl�wk�w
int try_writing();
};
*/

//Wirtualny destruktor
logfile::~logfile()
{
delete &log_stream;
}

//Laczy serie jednowartosciowa z kolumna pliku log. Wymusza wyplucie naglowka w try_writing
int logfile::link_data_source(size_t column,scalar_source_base* Source)
{
if(column>=table.get_size()) return 0;//Przekroczony zakres
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
	if( (pom=table[i])!=NULL )//Czy nie pusty
		if(pom->data_version()!=-1) //Czy juz nie wyzerowany
			pom->restart_counting();
curr_data_version=-1;
}

//Jesli choc jedna seria ma nowa wersje to wypluwa cala linie danych. Jesli uzyto link_data_source
//to wypluwa tez wiersz nagl�wk�w
int logfile::try_writing()
{
size_t i=0;
if(CheckVersion())
	{
	if(need_headers!=0)
		{
		need_headers=0;//Zapiszemy
		log_stream<<"DATA_VER"<<separator();
		for(i=0;i<next_column;i++)
			{
			scalar_source_base* pom=table[i];
			if(pom!=NULL)
				log_stream<<pom->name();
			log_stream<<separator();
			}
		log_stream<<"CLOCK"<<'\n';
		log_stream.flush();
		}
	
	log_stream<<GetVersion()<<separator();
	for(i=0;i<next_column;i++)
			{
			if(table[i]!=NULL)
				{
				data_source_base::iteratorh in=table[i]->reset();
				double pom=table[i]->get(in);
				if(table[i]->get_missing()==pom)
					log_stream<<missing_string.name();
					else
					log_stream<<pom;
				table[i]->close(in);
				}
			log_stream<<separator();
			}
	log_stream<<clock()<<'\n';
	log_stream.flush();
	return 1;//Zapisywal
	}
return 0;//Nie zapisywal
}

int logfile::CheckVersion()
//Sprawdza czy ktoras kolumna sie nie zmieni�a
{
for(size_t i=0;i<next_column;i++)
	if(table[i]!=NULL)
		{
		long data=table[i]->data_version();
		if(data>GetVersion())//Znalazl jedna nowsza
			{
			curr_data_version=data;
			return 1;
			}
		}
return 0;
}

/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/
