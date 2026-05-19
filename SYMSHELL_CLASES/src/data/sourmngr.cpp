/// @file
/// @brief  **DATA SOURCE MANAGER IMPLEMENTATION** /<br>
///          _IMPLEMENTACJA ZARZĄDCY ŹRÓDEŁ DANYCH._
/// @date 2026-05-19 (modified)
//*********************************************************************************************************************

#include <stdarg.h>
#include "wb_ptr.hpp"
#include "sourmngr.hpp"

using namespace sym2::data;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

// NAJPROSTSZA IMPLEMENTACJA — Z PRZESZUKIWANIEM LINIOWYM.
// Może być mało efektywna przy większej liczbie seri.
// ZARZĄDCA zakłada pełne panowanie nad "włożonymi" do niego seriami.
// Muszą być one utworzone w pamięci dynamicznej i nie wolno ich
// usunąć w inny sposób, niż odbierając je uprzednio zarządcy.

sources_manager::sources_manager(size_t N): tab(N)
//Konstruktor tylko na rozmiar
{
//tab.fill(NULL);
        assert(tab.get_ptr_val(N-1)!=NULL);
}

sources_manager::sources_manager(size_t N, data_source_base* first, .../*NULL*/): tab(N)
//Konstruktor z inicjująca lista
{
    //tab.fill(NULL);
    tab[0].set(first,0); //0 == "W domyśle są zarządzane dynamicznie"

    unsigned i=1;
    data_source_base* pom;
    va_list list;
    va_start(list,first);

    while((pom=va_arg(list,data_source_base*))!=NULL) //Ostatni musi być NULL
        {
        tab[i].set(pom,0); //W domyśle są zarządzane dynamicznie
        if(++i >= tab.get_size())
            goto END;
        }
    END:

    va_end(list);
}

size_t sources_manager::get_size()
//Podaje po prostu rozmiar.
{ 
    return tab.get_size();
}

int sources_manager::search(const char*		s_name)
//Odnajduje na liście. Zwraca -----//----
{										assert(s_name != NULL);
    size_t N=tab.get_size();
    for(size_t i=0;i<N;i++)
      if( tab[i].get() && strcmp(tab[i].get()->name(), s_name) == 0 ) //Czy nie pusty i czy ta nazwa
                return i; //Znalazł. Konwersja uint na int jest tu raczej bezpieczna. Małe szanse, żeby były ponad 2 miliardy serii!

    return -1; //Przeszedł wszystkie i nie znalazł
}


//Ustalanie informacji o wersji obsługiwanych danych.
//Pierwsza serie na sile, potem w pętli najpierw sprawdza, czy samo się zmieni,
// jeśli się nie zgadza, to wymusza.
void  sources_manager::new_data_version(int change, unsigned increment)
{
    size_t N=tab.get_size();

    assert(tab[0].get()!=NULL);
    data_source_base* pom=tab[0].get();
    if(pom==NULL) return; //Musi być pierwsza

    pom->new_data_version(change,increment);
    long version=pom->data_version(); //Taka ma być też w pozostałych

    for(size_t i=0;i<N;i++)
      if( (pom=tab[i].get())!=NULL ) //Czy nie pusty i czy ta nazwa
        if(pom->data_version()!=version)
                pom->new_data_version(change,increment);

}

void  sources_manager::restart_data_version()
{
    size_t N=tab.get_size();
    data_source_base* pom=NULL;
    for(size_t i=0;i<N;i++)
      if( (pom=tab[i].get())!=NULL ) //Czy nie pusty
            if(pom->data_version()!=-1)
                pom->restart_counting();
}

int  sources_manager::insert(data_source_base*	ser, int not_menage)
//Dodaje serie do listy. Zwraca pozycje albo -1(błąd)
{
    size_t N=tab.get_size();
    for(size_t i=0;i<N;i++)
      if(!tab[i].get()) //Tylko wtedy gdy znajdzie pusty wieszak
            {
            tab[i].set(ser,not_menage); // umieszcza nowa serie.
            return i;
            }
    return -1; //Nie znalazł wolnego
}

int  sources_manager::replace(size_t pos, data_source_base* ser, int not_men)
{
    if(pos>=tab.get_size())
            return -1;
    tab[pos].set(ser,not_men); //Tu usuwa stary i zastępuje nowym albo NULL-em.
    return pos;
}

int  sources_manager::replace(const char* nam, data_source_base*	ser, int not_men)
//Wymienia na liście. Jak nie znajdzie, to zwraca -1.
{
int pos=search(nam);
if(pos==-1)
        return -1;
tab[pos].set(ser,not_men); //Tu usuwa stary i zastępuje nowym albo NULL-em
return pos;
}


int    sources_manager::set_info(size_t index,
                                 wb_color	  ico,	//Czy kolor ustalony
            sym2::config_point* fig) //Obiekt rysujący punkty
// Zapamiętuje informacje o wizualizacji serii.
{
if(index<tab.get_size())
    {
    tab[index].col=ico;
    tab[index].fig=fig;
    return index;
    }
return -1;
}

wb_dynarray<sym2::graph::series_info> sources_manager::make_series_info(wb_dynarray<int> indexes)
{
    size_t i,siz=indexes.get_size();
    wb_dynarray<sym2::graph::series_info> todo(siz + 1); //Musi być jedna pusta na końcu
    for(i=0;i<siz;i++)
        {
        size_t index=indexes[i];
        data_source_base* gcc_pom=tab[index].get();
        sym2::config_point* gcc_pom2= tab[index].fig?tab[index].fig.get_ptr_val():NULL;
        sym2::graph::series_info pom(//nie ma zarządzać!
                gcc_pom,							//Wskaźnik do źródła danych
                0,									//Nie zarządzaj!
                tab[index].col,						//Jeśli nie default_color, ustala kolor
                gcc_pom2,							//Wskaźnik rysika. Jeśli NULL, to łączy liniami
                0);									//Nie zarządzaj!
        todo[i]=pom; //Żywcem przepisanie razem ze wskaźnikami
        }
    return todo;
}

wb_dynarray<sym2::graph::series_info> sources_manager::make_series_info(int start, .../* ostatnia -1*/)
// Tworzenie dynamicznie tablice serii
{
    va_list list;
    //va_list(list);
    size_t siz=1;

    //Zliczanie pozostałych parametrów:
    va_start(list,start);
    while(va_arg(list,int)!=-1) siz++;
    va_end(list);

    wb_dynarray<sym2::graph::series_info> todo(siz + 1); //Musi być jedna pusta na końcu.

    //Wypełnianie tablicy seri.
    va_start(list,start);
    size_t i,index; //Index odczytany z parametru. i - pozycja w tod(?)
    for(index=start,i=0;i<siz;index=va_arg(list,int),i++)
        {
            data_source_base* gcc_pom=tab[index].get();
            sym2::config_point* gcc_pom2= tab[index].fig?tab[index].fig.get_ptr_val():NULL;

            sym2::graph::series_info pom(
                    gcc_pom,		//Wskaźnik do źródła danych
                    0,				//Nie zarządzaj!
                    tab[index].col,	//Jeśli !=-1, ustala kolor
                    gcc_pom2,		//Wskaźnik rysika. Jeśli NULL to łączy liniami
                    0);			    //Nie zarządzaj pamięcią!

            todo[i]=pom; //przepisanie razem ze wskaźnikami (shallow copy).
        }
    va_end(list);
    return todo;
}

data_source_base/*const*/* sources_manager::get(size_t index) //Nie wolno zwalniać!!!
{  
//										assert(index>=0);
    if(index<tab.get_size())
        return tab[index].get();
    else
        return NULL;
}

//wb_dynarray<graph::series_info>  sources_manager::make_series_info(const int* first/*ostatnia -1*/)
//wg indeksów podanych jako parametry lub w tablicy
//{}


#ifdef __BORLANDC__
#include <alloc.h> //Pod kompilatorami Borlanda jest `heapcheck()``
sources_manager::~sources_manager()
        {
                assert( heapcheck() != _HEAPCORRUPT );
        }
#else
sources_manager::~sources_manager() //Destruktor jest oczywiście wirtualny z uwagi na klasy potomne
        {}
#endif

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

