//                        IMPLEMENTACJA ZARZADCY SERI DANYCH
//*****************************************************************************************
// ...
//*/////////////////////////////////////////////////////////////////////////////////////////
#include <stdarg.h>
#include "wb_ptr.hpp"
#include "sourmngr.hpp"

// NAJPROSTRZA IMPLEMENTACJA - Z PRZESZUKIWANIEM LINIOWYM. 
// Moze byc malo efektywna przy wiekszej liczbie seri.
// ZARZADCA zaklada pelne panowanie nad "wlozonymi" do niego seriami.
// Musza by� one utworzone w pamieci dynamicznej i nie wolno ich 
// usun�� w inny spos�b, ni� odbierajac je uprzednio zarzadcy.

sources_menager::sources_menager(size_t N):tab(N)
//Konstruktor tylko na rozmiar
		{
        //tab.fill(NULL);
				assert(tab.get_ptr_val(N-1)!=NULL);
		}

sources_menager::sources_menager(size_t N,data_source_base* first,.../*NULL*/):tab(N)
//Konstruktor z inicjujaca lista
		{
        //tab.fill(NULL);
		tab[0].set(first,0);//0=="W domysle sa zarzadzane dynamicznie"
	
		unsigned i=1;
		data_source_base* pom;
		va_list list;
		va_start(list,first);

		while((pom=va_arg(list,data_source_base*))!=NULL) //Ostatni musi byc NULL
			{ 
			tab[i].set(pom,0);//W domysle sa zarzadzane dynamicznie
			if(++i >= tab.get_size())
				goto END;
			}
		END:

		va_end(list);
		}

size_t sources_menager::get_size()
//Podaje po prostu rozmiar.
{ 
	return tab.get_size();
}

int sources_menager::search(const char*		sname)
//Odnajduje na liscie. Zwraca -----//----	
{										assert(sname!=NULL);
size_t N=tab.get_size();
for(size_t i=0;i<N;i++)
  if( tab[i].get() && strcmp(tab[i].get()->name(),sname)==0 )//Czy nie pusty i czy ta nazwa
			return i;//Znalazl

return -1; //Przeszedl wszystkie i nie znalazl	
}


//Ustalanie informacji o wersji obslugiwanych danych.
//Pierwsza serie na sile, potem w petli najpierw sprawdza czy samo sie zmieni,
// jesli sie nie zgadza to wymusza.
void  sources_menager::new_data_version(int change,unsigned increment)
{
size_t N=tab.get_size();

assert(tab[0].get()!=NULL);
data_source_base* pom=tab[0].get();
if(pom==NULL) return;//Musi byc pierwsza

pom->new_data_version(change,increment);
long version=pom->data_version();//Taka ma byc tez w pozostalych

for(size_t i=0;i<N;i++)
  if( (pom=tab[i].get())!=NULL )//Czy nie pusty i czy ta nazwa
	if(pom->data_version()!=version)
			pom->new_data_version(change,increment);

}

void  sources_menager::restart_data_version()
{
size_t N=tab.get_size();
data_source_base* pom=NULL;
for(size_t i=0;i<N;i++)
  if( (pom=tab[i].get())!=NULL )//Czy nie pusty
		if(pom->data_version()!=-1)
			pom->restart_counting();
}

int  sources_menager::insert(data_source_base*	ser,int not_menage)
//Dodaje serie do listy. Zwraca pozycje albo -1(blad)
{
size_t N=tab.get_size();
for(size_t i=0;i<N;i++)
  if(!tab[i].get()) //Tylko wtedy gdy znajdzie pusty wieszak
		{
		tab[i].set(ser,not_menage);// umieszcza nowa serie.
		return i;
		}
return -1;//Nie znalazl wolnego
}

int  sources_menager::replace(size_t pos,data_source_base* ser,int not_men)
{
if(pos>=tab.get_size())
		return -1;
tab[pos].set(ser,not_men);//Tu usuwa stary i zastepuje nowym, albo NULL'em
return pos;
}

int  sources_menager::replace(const char* nam,data_source_base*	ser,int not_men)
//Wymienia na liscie. Jak nie znajdzie to zwraca -1.
{
int pos=search(nam);
if(pos==-1)
		return -1;
tab[pos].set(ser,not_men);//Tu usuwa stary i zastepuje nowym, albo NULL'em
return pos;
}


int    sources_menager::set_info(size_t index,
			wb_color	  ico,//Czy kolor ustalony
			config_point* fig)//Obiekt rysujacy punkty	
//Zapamientuje informacje o wizualizacji serii						
{
if(index<tab.get_size())
	{
	tab[index].col=ico;
	tab[index].fig=fig;
	return index;
	}
return -1;
}

wb_dynarray<graph::series_info> sources_menager::make_series_info(wb_dynarray<int> indexes)
{
size_t i,siz=indexes.get_size();
wb_dynarray<graph::series_info> todo(siz+1);//Musi byc jedna pusta na koncu
for(i=0;i<siz;i++)
	{
	size_t index=indexes[i];
	data_source_base* gcc_pom=tab[index].get();
	config_point* gcc_pom2=tab[index].fig?tab[index].fig.get_ptr_val():NULL;
	graph::series_info pom(//nie ma zarzadzac
			gcc_pom,	//Wskaznik do zrodla danych
			0,									//Nie zarzadzaj!
			tab[index].col,	//Jesli nie default_color, ustala kolor
			gcc_pom2,	//Wskaznik rysika.Jesli NULL to laczy liniami
			0);									//Nie zarzadzaj!
	todo[i]=pom;//Zywcem przepisanie razem ze wskaznikami
	}
return todo;
}

wb_dynarray<graph::series_info> sources_menager::make_series_info(int start,.../* ostatnia -1*/)
//Tworza dynamicznie tablice serii
{
va_list list;
//va_list(list);
size_t siz=1;

//zliczanie pozostalych parametrow
va_start(list,start);
while(va_arg(list,int)!=-1) siz++;
va_end(list);

wb_dynarray<graph::series_info> todo(siz+1);//Musi byc jedna pusta na koncu

//Wypelnianie tablicy seri.
va_start(list,start);
size_t i,index;//Index odczytany z parametru. i - pozycja w tod
for(index=start,i=0;i<siz;index=va_arg(list,int),i++)
	{
	data_source_base* gcc_pom=tab[index].get();
	config_point* gcc_pom2=tab[index].fig?tab[index].fig.get_ptr_val():NULL;
	graph::series_info pom(		
			gcc_pom,	//Wskaznik do zrodla danych
			0,									//Nie zarzadzaj!
			tab[index].col,	//Jesli !=-1, ustala kolor
			gcc_pom2,	//Wsaznik rysika.Jesli NULL to laczy liniami
			0);									//Nie zarzadzaj!
	todo[i]=pom;//Zywcem przepisanie razem ze wskaznikami
	}
va_end(list);
return todo;
}

data_source_base/*const*/* sources_menager::get(size_t index) //Nie wolno zwalniac!!!			
{  
//										assert(index>=0);
	if(index<tab.get_size())	
		return tab[index].get(); 
		else
		return NULL;
}

//wb_dynarray<graph::series_info>  sources_menager::make_series_info(const int* first/*ostatnia -1*/)
//wg indeksow podanych jako parametry lub w tablicy
//{
//}

//Destruktor jest oczywi?cie virtualny z uwagi na klasy potomne
#ifdef __BORLANDC__
#include <alloc.h> //Pod borlandami jest heapcheck()
sources_menager::~sources_menager()
		{
				assert( heapcheck() != _HEAPCORRUPT );
        }
#else
sources_menager::~sources_menager()
        {} //Tu tez si? r�?ne rzeczy dziej? ale po cichu :-)
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

