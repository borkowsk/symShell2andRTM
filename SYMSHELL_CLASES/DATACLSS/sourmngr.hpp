//      	KLASA ZARZADCY SERI DANYCH
//********************************************
#ifndef __SOURMNGR_HPP__
#define __SOURMNGR_HPP__
#ifndef __cplusplus
#error C++ required
#endif

#include "datasour.hpp"
#include "graphs.hpp"

//	Interface zarzadcy danych
//----------------------------------
class sources_menager_base
{
public:
virtual ~sources_menager_base(){}		 //Destruktor virtualny z uwagi na klasy potomne
//	AKCESORY
//-------------
virtual size_t get_size()=0;		//Podaje po prostu rozmiar.
virtual int    insert(data_source_base*	ser,int not_men=0)=0;//Dodaje serie do listy. Zwraca pozycje albo -1(blad)
virtual int    replace(size_t pos,data_source_base* ser=NULL,int not_men=0)=0;//Wymienia, ale jak indeks niepoprawny to zwraca -1
virtual int    replace(const char* nam,data_source_base* ser=NULL,int not_men=0)=0;//Wymienia na liscie. Jak nie znajdzie to zwraca -1.
virtual int    search(const char* nam)=0;//Odnajduje na liscie. Zwraca -----//----	
virtual int    set_info(size_t   index,//Zapamientuje informacje o wizualizacji serii
						wb_color ico=default_color,//Czy kolor ustalony
						config_point* fig=NULL)=0;//Obiekt rysujacy punkty			
// Tworza dynamicznie tablice serii - potrzebne dla grafow:
virtual wb_dynarray<graph::series_info> make_series_info(int start,.../* ostatnia -1*/)=0;//Z listy parametrow
virtual wb_dynarray<graph::series_info> make_series_info(wb_dynarray<int>)=0;				//Z tablicy dynamicznej
//wb_dynarray<graph::series_info> make_series_info(const int* first/*ostatnia -1*/);//Z tablicy typu C zakonczonej -1
//Udostepnianie samych seri:
virtual data_source_base /*const*/* get(size_t index)=0; //Bez mozliwosci modyfikacji i zwolnienia
//reserved:
//virtual
//int	eval(istream& script);//Wykonuje skrypt laczenia danych
//int	save(ostream& script);//Zapisuje sie w formacie skryptu laczenia danych
};

// NAJPROSTRZY ZARZADCA DANYCH
// Zaklada pelne panowanie nad "wlozonymi" do niego seriami, ktore
// musza być one utworzone w pamieci dynamicznej chyba ze w metodach
// insert() lub replace() podano inaczej - tzn not_menage==1 (obiekt statyczny)
// Normalnie zwalnianie wykonuje zarzadca. Recznie mozna uzyc do tego
// metody replace() z parametrem NULL.
class sources_menager:public sources_menager_base
{
private:
class internal
	{
	private:
	data_source_base*		 dat;//Wskaznik na serie
	int				not_menage:1;//Nie zwalniac
	public:
	wb_color				 col;//Czy kolor ustalony
	wb_ptr<config_point>	 fig;//Obiekt rysujacy punkty
	//MANIPULATORY
	void set(data_source_base* d,int not_men)
	{
		clear();
		dat=d;not_menage=not_men;
	}

	void clear()
	{
	if(not_menage==0 && dat!=NULL)
		{
#ifndef NDEBUG
		//cerr<<dat->name()<<'\n';
		fprintf(stderr,"%s\n",dat->name());
#endif
		delete dat;
		}
	dat=NULL;not_menage=0;
	}

	data_source_base* get(){ return dat;}//read-only
	//KONSTRUKTORY/DESTRUKTORY
	internal():
		not_menage(1),col(default_color),dat(NULL),fig(NULL)
		{}
	//internal(const internal& ini):col(ini.col),dat(ini.dat),fig(ini.fig),not_menage(ini.not_menage){}
	~internal()	{clear();}
	};

wb_dynarray<internal> tab;

public:
	sources_menager(size_t N); //Konstruktor tylko na rozmiar
	sources_menager(size_t N,data_source_base* .../*NULL*/);//Konstruktor z inicjujaca lista zakonczona NULL
virtual
	~sources_menager();		   //Destruktor virtualny z uwagi na klasy potomne
//AKCESORY
size_t get_size();		//Podaje po prostu rozmiar, lacznie z pozycjami pustymi
int    insert(data_source_base*	ser,int not_men=0);//Dodaje serie do listy. Zwraca pozycje albo -1(blad)
int    replace(size_t pos,data_source_base* ser=NULL,int not_men=0);
int    replace(const char* nam,data_source_base*	ser=NULL,int not_men=0);//Wymienia na liscie. Jak nie znajdzie to zwraca -1.
int    search(const char*		nam);//Odnajduje na liscie. Zwraca -----//----

int    set_info(size_t index,//Zapamientuje informacje o wizualizacji serii
				wb_color ico=default_color,//Czy kolor ustalony
				config_point* fig=NULL);//Obiekt rysujacy punkty
// Tworza dynamicznie tablice serii - potrzebne dla wykresow:
wb_dynarray<graph::series_info> make_series_info(int start,.../* ostatnia -1*/);//Z listy parametrow
wb_dynarray<graph::series_info> make_series_info(wb_dynarray<int>);				//Z tablicy dynamicznej
//wb_dynarray<graph::series_info> make_series_info(const int* first/*ostatnia -1*/);//Z tablicy typu C zakonczonej -1
// Udostepnianie samych seri:
data_source_base/*const*/* get(size_t index); //Nie wolno zwalniac!!!
//reserved:
//virtual
//int	eval(istream& script);//Wykonuje skrypt laczenia danych
//int	save(ostream& script);//Zapisuje sie w formacie skryptu laczenia danych

//Ustalanie informacji o wersji obslugiwanych danych.
//Pierwsza serie na sile, potem w petli najpierw sprawdza czy samo sie zmieni,
// jesli sie nie zgadza to wymusza.
void  new_data_version(int change=1,unsigned increment=1);
// Zeruje informacje o versjach danych
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

