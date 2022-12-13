//		KLASY DEFINIUJACE ROZNORODNE TYPY WYKRESOW
//--------------------------------------------------
#ifndef __GRAPHS_HPP__
#define __GRAPHS_HPP__
#ifndef __cplusplus
#error C++ required
#endif

#include <assert.h>
#include <string.h>

//Niezbedne definicje bazowe
//------------------------------------------
//#include "platform.hpp"
#include <iostream>
#include "wb_clone.hpp"
#include "sshutils.hpp"
#include "datasour.hpp"
#include "drawable.hpp"

//INTERFACE DO GRAFU - KLASY WYPROWADZONE POWINNY TYLKO IMPLEMENTOWAC
//--------------------------------------------------------------------------

//Klasa bazowa dla konfiguracji punktow
class config_point
	{	
    float base_fract;//Rozmiar minimalny punktu. W ulamku obszaru (default 5%%)
	float max_fract; //Rozmiar maksymalny punktu. W ulamku obszaru (default 2%)
	public:
    size_t baselen;  //Aktualnie przyjety rozmiar okna. Mozna zmieniac!
	config_point():base_fract(0.005f),max_fract(0.02f),baselen(1000)//Domyslnie duzy baselen, zeby w awaryjnie bylo cos pixelach
        {}
    config_point(float base,float max):base_fract(base),max_fract(max)
        {}
    unsigned base_size() 
        { return unsigned(baselen*base_fract);}
    unsigned max_size() 
        { return unsigned(baselen*max_fract);}
	virtual 
    void plot(int x, int y, unsigned short size, wb_color color);//Defaultowe rysowanie
	};

//Klasa rysujaca prosty punkt
class simple_point:public config_point
	{//Ignoruje rozmiar!!!
	public:
	simple_point(){}
    void plot(int x,int y, unsigned short size, wb_color color);
	};

//Klasa rysujaca krzyzyk
class hash_point:public config_point
	{
	public:
	hash_point(){}
    hash_point(float base,float max):config_point(base,max){}
    void plot(int x,int y, unsigned  short size, wb_color color);
	};

//Klasa rysujaca kolko
class circle_point:public config_point
	{
	public:
	circle_point(){}
    circle_point(float base,float max):config_point(base,max){}
    void plot(int x,int y, unsigned short size, wb_color color);
	};

//Klasa rysujaca romb
class rhomb_point:public config_point
	{
	public:
	rhomb_point(){}
    rhomb_point(float base,float max):config_point(base,max){}
    void plot(int x,int y, unsigned short size, wb_color color);
	};

//Klasa rysujaca kwadracik/prostokąt
class rect_point:public config_point
	{
	public:
	rect_point(){}
    rect_point(float base,float max):config_point(base,max){}
    void plot(int x,int y, unsigned short size, wb_color color);
	};

class graph:public drawable_base
//--------------------------------------------------
{
//INTERFACE UZYTKOWNIKA
//---------------------
public:

//Klasa informacji o serii dla wykresow o nieustalonej liczbie serii
class series_info
	{
	public:
	series_info(data_source_base*	ipt=NULL, //Wskaznik do serii
				int					ime=0,	  //Czy zarzadza seria
				wb_color			ico=default_color,//Czy kolor ustalony
				config_point*		fig=NULL, //Obiekt rysujacy punkty
				int					imep=0	  //Czy zarzadza obk.rys.pkt.
				):ptr(ipt),menage(ime),figure(fig),men_p(imep),color(ico)
			{}

	~series_info()
			{
			if(menage && ptr!=NULL)
				{ delete ptr; }
			if(men_p && figure!=NULL)
				{ delete figure; }
			};
	//Musi zywcem przepisywac wskazniki, jesli nimi nie zarzadza!!!
	//zalozenie wykonane na razie przez domyslny operator =
	data_source_base*	ptr;     //Wskaznik do zrodla danych
	wb_color		    color;   //Jesli !=-1, ustala kolor
	config_point*		figure;  //Jesli NULL to laczy liniami
	int					menage:1;//Zarzadzanie pamiecia zrodla
	int					men_p:1; //Zarzadzanie pamiecia punktu
	};

//Metody do zaimplementowania w klasach potomnych
public:
virtual const char* user_help_text()=0; //Zwraca tekst opisujacy typ wykresu
virtual int configure(const void*)=0;//Ustala parametry wyswietlania poprzez odpowiednia strukture. Zwraca -1 jesli bledny parametr
//Ustalaja kolor lub zbior kolorow dla tekstow oraz kolor lub zakres dla danych
virtual int settextcolors(wb_color start_i,wb_color end_i=default_white);//Ustala kolor dla tekstow
virtual int setdatacolors(wb_color start_i,wb_color end_i);//Ustala kolor lub zakres dla danych
virtual int setseries(size_t index,data_source_base* data,int menage=0)=0;//zwraca -1 jesli indeks za duzy
virtual data_source_base* getseries(size_t index)=0;//zwraca NULL jesli indeks za duzy

//INTERFACE IMPLEMENTATORA KLAS POTOMNYCH
//---------------------------------------------
// reals[] zawiera wspolzedne (wartosci serii glownych) oraz wartosc ustalajaca kolor (seria koloru - jesli jest)
// Wymaga sie by dzialal prawidlowo po wywolaniu funkcji _replot() !!!
protected:
virtual int _rescale_data_point(const double reals[],long in_area[])=0;//zwraca -1 jesli blad. Np
	//ktoras z podanych wartosci nie zawiera sie w podanym przez serie zakresie <min,max>

graph(int x1,int y1,int x2,int y2,
				wb_color ibkg=default_white,
				wb_color ifr=default_transparent):
			drawable_base(x1,y1,x2,y2,ibkg,ifr),
			c_range(default_black,default_white)
				{
				vis_title=vis_leg1=vis_leg2=vis_leg3=vis_leg4=1;
				}

public://TYMCZASOWO
//enable/disable elements
unsigned vis_title:1;
unsigned vis_leg1:1;
unsigned vis_leg2:1;
unsigned vis_leg3:1;
unsigned vis_leg4:1;
//other settings
struct scaling_info
	{ 
	double min,max,scale;//Wartosci potrzebne do przeliczenia
	unsigned fix_min:1;
	unsigned fix_max:1;
	scaling_info(double imin=0,double imax=1,double range=1):
		min(imin),max(imax),scale(0)
		{ 
		fix_min=fix_max=0;//Nic nie blokuje, bierze jak seria daje 
		set(range);
		}
	void set(double imin,double imax,double range)
		{ 
		if(!fix_max)
			max=imax;
		if(!fix_min)
			min=imin;
		assert(min<=max);
		if(min!=max)
			scale=range/(max-min);
			else
			scale=0;//Awaryjnie;
		}	
	void set(double range)
		{ 
		assert(min<=max);
		if(min!=max)
			scale=range/(max-min);
			else
			scale=0;//Awaryjnie
		}	
	double get(double val)
		{ 
		return scale*(val-min); 
		}
	void OXaxis(int x1,int y1,int x2,int y2,wb_color col,wb_color bcg);
	void OYaxis(int x1,int y1,int x2,int y2,wb_color col,wb_color bcg);
	};

struct color_info
	{
	wb_color start;//Musi byc zawsze
	wb_color end;  //Jesli >start to oznacza zakres kolorow
	color_info(wb_color ista=0,wb_color iend=0):start(ista),end(iend) {}
	int plot(int x1,int y1,int x2,int y2);
	};

protected:
color_info c_range;	//Kolory obiektow. Najwyzej tyle ile udostepnia platforma!
color_info t_colors;//Kolory tekstow, najwyzej dwa
gps_area   graph_core;//Wlasciwy obszar rysowania po wycieciu wrzystkich marginesow
};


//				IMPLEMENTACJE ROZNYCH GRAFOW
//---------------------------------------------------------------

class carpet_graph:public graph //Prostokat z roznokolorowych kwadracikow
//------------------------------------------------------------------------
{
	unsigned print_title:1;

public:
//HELP:
const char* user_help_text(){ return
"CARPET GRAPH:\n"
"Pseudo 2 dimensional, 1 series for colors (index 0)\n"
"Rectangles are aranged on AxB base.\n"
"By default colors are mapping from 0 to 255.\n"
"If textcolor!=background the legend is printing\n"
"\n"
"\n"
"\n"
"\n";}
//DESTRUCTOR
~carpet_graph();
//CONSTRUCTOR(S)
carpet_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru
			 unsigned A,unsigned B,			 //A-ile kolumn,B-ile wierszy
			 data_source_base* data,int menage=0, //data-zrodlo danych o kolorach
			 bool idirect_color=false);
carpet_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru. Seria musi miec geometrie 2D
			 data_source_base* data,int menage=0,//data-zrodlo danych o kolorach
			 bool idirect_color=false);

// IMPLEMENTATION OF VIRTUAL METHODS
int configure(const void*)//Nie ma parametrow konfiguracji. Zwasze zwraca -1 
	{return -1;}
int setseries(size_t index,data_source_base* data,int menage=0);//zwraca -1 jesli indeks za duzy
data_source_base* getseries(size_t index);//zwraca NULL jesli indeks za duzy
// reals[] zawiera jedynie wartosc ustalajaca kolor
int _rescale_data_point(const double reals[1],long in_area[1]);//zwraca -1 jesli nie w oknie
void _replot();// Rysuje wlasciwy wykres a pod nim ewentualnie legende

// ONLY FOR DEVELOPERS OF THIS CLASS
protected:
const geometry_base* read_dim(size_t& aa,size_t& bb);
size_t		AA,BB;
int			menage;
data_source_base* data;
bool direct_color;//=false
graph::scaling_info mm;
wb_ptr<geometry_base> zastepnik;
};


class true_color_carpet_graph:public graph //Prostokat z kwadracikow o kolorach składanych z trzech serii 
//---------------------------------------------------------------------------------------------------------
{
public:
//HELP:
const char* user_help_text(){ return
"TRUE COLOR CARPET GRAPH:\n"
"Pseudo 2 dimensional, 3 series for colors (RGB)\n"
"Rectangles are aranged on AxB base.\n"
"By default colors are mapping from 0 to 255.\n"
"If textcolor!=background the legend is printing\n"
"\n"
"\n"
"\n"
"\n";}
//DESTRUCTOR
~true_color_carpet_graph();
//CONSTRUCTOR(S)
true_color_carpet_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru
			 unsigned A,unsigned B,			 //A-ile kolumn,B-ile wierszy
			 data_source_base* RedData,int menage_r=0,
			 data_source_base* GreenData=NULL,int menage_g=0,
			 data_source_base* BlueData=NULL,int menage_b=0
			 );//data-zrodla danych o kolorach
true_color_carpet_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru. Serie musza miec taka sama geometrie 2D			
			 data_source_base* RedData,int menage_r=0,
			 data_source_base* GreenData=NULL,int menage_g=0,
			 data_source_base* BlueData=NULL,int menage_b=0
			 );//data-zrodla danych o kolorach

// IMPLEMENTATION OF VIRTUAL METHODS
int configure(const void*)//Nie ma parametrow konfiguracji. Zwasze zwraca -1 
	{return -1;}
int setseries(size_t index,data_source_base* data,int menage=0);//zwraca -1 jesli indeks za duzy
data_source_base* getseries(size_t index);//zwraca NULL jesli indeks za duzy
// reals[] zawiera jedynie wartosc ustalajaca kolor
int _rescale_data_point(const double reals[3],long in_area[3]);//zwraca -1 jesli nie w oknie
void _replot();// Rysuje wlasciwy wykres a pod nim ewentualnie legende

// ONLY FOR DEVELOPERS OF THIS CLASS
protected:
const geometry_base* read_dim(size_t& aa,size_t& bb);
size_t		AA,BB;
int			menage[3];
data_source_base* data[3];
graph::scaling_info mm[3];
wb_ptr<geometry_base> zastepnik;
};

template<class DATA_SOURCE,int DIRECT_COLOR=false>
class fast_carpet_graph:public carpet_graph //Dywan z samych pixeli.
//------------------------------------------------------------------
{
public:
//CONSTRUCTOR(S)
fast_carpet_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru
			 unsigned A,unsigned B,			 //A-ile kolumn,B-ile wierszy
			 DATA_SOURCE* data,int menage=0)://data-zrodlo danych o kolorach
			 carpet_graph(x1,y1,x2,y2,A,B,data,menage){}
fast_carpet_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru. Seria musi miec geometrie 2D			
				  DATA_SOURCE* data,int menage=0)://data-zrodlo danych o kolorach
			 carpet_graph(x1,y1,x2,y2,data,menage){}
void _replot();// Rysuje wlasciwy wykres a pod nim ewentualnie legende
			   // Typ Zrodla jest ustalony wiec metody moga byc inlinowane. 
};

class bars_graph:public graph // Zwykle 2D "slupki" roznej wielkosci i ewentualnie koloru
//------------------------------------------------------------------------
{
public:
//HELP:
const char* user_help_text(){ return
"2D BARS PLOT:\n"
"2 Dimensional, 2 series:\n"
"index 0: heights of bars\n"
"index 1: colors of bars (optional)\n"
"By default colors range are from 0 to 255.\n"
"and default bar color is 128.\n"
"If textcolor!=bacground legend is printing\n"
"\n";}
//DESTRUCTOR
~bars_graph();
//CONSTRUCTOR(S)
bars_graph(int x1,int y1,int x2,int y2,				   //Polozenie obszaru		
			 data_source_base* idatas,int       menage_d=0,//datas-dane o wysokosciach
			 data_source_base* icolors=NULL,int menage_c=0,//i o kolorach
			 int zero_mod=1			//1-Slupki zaczynaja sie conajmniej od 0!
									//0- to zaczynaja sie od min (moze >0)
			 );//colors-zrodlo danych o kolorach
// IMPLEMENTATION OF VIRTUAL METHODS
struct config_zero
	{
	config_zero(int z=0):zero_mode(z){}
	int zero_mode;
	};
int configure(const void* config);//Konfiguruje tryb wyswietlania	
int setseries(size_t index,data_source_base* data,int menage=0);//zwraca -1 jesli indeks za duzy
data_source_base* getseries(size_t index);//zwraca NULL jesli indeks za duzy
// reals[] zawiera wysokosc i wartosc ustalajaca kolor
int _rescale_data_point(const double reals[2],long in_area[2]);//zwraca -1 jesli nie w oknie
void _replot();// Rysuje wlasciwy wykres a pod nim ewentualnie legende

// ONLY FOR DEVELOPERS OF THIS CLASS
private:
int mode;
int d_menage;
int c_menage;
data_source_base* datas;
data_source_base* colors;
scaling_info s_data;
scaling_info s_colo;
};


class rainbow_graph:public graph//kolumna lub wiersz liczb, kazda na innym tle!
//------------------------------------------------------------------------------
{
public:
//HELP:
const char* user_help_text(){ return
"RAINBOW PLOT:\n"
"0 Dimensional, 2 series:\n"
"index 0: values to print\n"
"index 1: colors of bars (optional)\n"
"By default colors range are from 0 to 255.\n"
"and default bar color is background.\n"
"If textcolor!=bacground legend is printing\n"
"User can configure format of printing values.\n"
"\n";}
//DESTRUCTOR
~rainbow_graph();
//CONSTRUCTOR(S)
rainbow_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru		
			 data_source_base* idatas,int menage_d=0,//datas-dane==wartosci
			 data_source_base* icolors=NULL,int menage_c=0,//colors-zrodlo danych o kolorach
			 const char* format="%g"//format liczby - nie moze byc wiecej niz 1 parametr!
			 );
// IMPLEMENTATION OF VIRTUAL METHODS
int configure(const void* format);//Wymaga const char* format jako parametr
int setseries(size_t index,data_source_base* data,int menage=0);//zwraca -1 jesli indeks za duzy
data_source_base* getseries(size_t index);//zwraca NULL jesli indeks za duzy
// reals[] zawiera jedynie wartosc ustalajaca kolor
int _rescale_data_point(const double reals[1],long in_area[1]);//skaluje tylko kolor
void _replot();// Rysuje wlasciwy wykres a pod nim ewentualnie legende

// ONLY FOR DEVELOPERS OF THIS CLASS
private:
int d_menage;
int c_menage;
data_source_base* datas;
data_source_base* colors;
scaling_info s_colo;
char* format;
};


class manhattan_graph:public graph// Dywanik slupkow 3D
//-------------------------------------------------------------------------
{
public:
//HELP:
const char* user_help_text(){ return
"MANHATTAN PLOT:\n"
"Pseudo 3 Dimensional, 2 series:\n"
"index 0: heights of bars\n"
"index 1: colors of bars (optional)\n"
"Bars are arranged on AxB base.\n"
"By default colors range are from 0 to 255.\n"
"and default bar color is 128.\n"
"If textcolor!=bacground legend is printing\n"
"\n";}
//DESTRUCTOR
~manhattan_graph();
//CONSTRUCTOR(S)
manhattan_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru		
			 unsigned A,unsigned B,				//A-ile kolumn, B-ile wierszy
			 data_source_base* idatas,int menage_d=0,//datas-dane o wysokosciach
			 data_source_base* icolors=NULL,int menage_c=0,//Zrodlo danych o kolorach
			 int zero_mod=1,			//Slupki zaczynaja sie conajmniej od 0!
									//Jesli 0 to zaczynaja sie od min>0
			 double Hoffs=0.33,		//Ulamek szerokosci przeznaczony na perspektywe
			 double	Voffs=0.33		//Ulamek wysokosci  przeznaczony na perspektywe
			 );
manhattan_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru		
			 data_source_base* idatas,int menage_d=0,//datas-dane o wysokosciach
			 data_source_base* icolors=NULL,int menage_c=0,//Zrodlo danych o kolorach
			 //Musza miec zgodne geometrie 
			 int zero_mod=1,		//Slupki zaczynaja sie conajmniej od 0!
									//Jesli 0 to zaczynaja sie od min>0
			 double Hoffs=0.33,		//Ulamek szerokosci przeznaczony na perspektywe
			 double	Voffs=0.33		//Ulamek wysokosci  przeznaczony na perspektywe
			 );
// IMPLEMENTATION OF VIRTUAL METHODS
struct config
	{
	config(int c_offs=0,unsigned col_wire=0,int z=0):
				color_offset(c_offs),wire(col_wire),zero_mode(z){}
	int color_offset;
	unsigned wire;
	int zero_mode;
	};
int configure(const void* config);//Konfiguruje tryb wyswietlania	
int setseries(size_t index,data_source_base* data,int menage=0);//zwraca -1 jesli indeks za duzy
data_source_base* getseries(size_t index);//zwraca NULL jesli indeks za duzy
// reals[] zawiera wysokosc i wartosc ustalajaca kolor
int _rescale_data_point(const double reals[2],long in_area[2]);//zwraca -1 jesli nie w oknie
void _replot();// Rysuje wlasciwy wykres a pod nim ewentualnie legende

// ONLY FOR DEVELOPERS OF THIS CLASS
private:
const geometry_base* read_dim(size_t& aa,size_t& bb);
int mode;
int c_offset;
wb_color wire;
unsigned AA;
unsigned BB;
double Hoffs;
double Voffs;
int d_menage;
int c_menage;
data_source_base* datas;
data_source_base* colors;
scaling_info s_data;
scaling_info s_colo;
wb_ptr<geometry_base> zastepnik;
};


class true_color_manhattan_graph:public graph// Dywanik slupkow 3D
//-------------------------------------------------------------------------
{
public:
//HELP:
const char* user_help_text(){ return
"MANHATTAN PLOT:\n"
"Pseudo 3 Dimensional, 4 series:\n"
"index 0: heights of bars\n"
"index 1-3: colors of bars (optional)\n"
"Bars are arranged on AxB base.\n"
"By default RGBs range are from 0 to 255.\n"
"and default bar color is blue (?).\n"
"If textcolor!=bacground legend is printing\n"
"\n";}
//DESTRUCTOR
~true_color_manhattan_graph();
//CONSTRUCTOR(S)
true_color_manhattan_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru		
			 unsigned A,unsigned B,				//A-ile kolumn, B-ile wierszy
			 data_source_base* idatas,int menage_d=0,//datas-dane o wysokosciach
			 data_source_base* RedData=NULL,int menage_r=0,
			 data_source_base* GreenData=NULL,int menage_g=0,
			 data_source_base* BlueData=NULL,int menage_b=0,
			 int zero_mod=1,			//Slupki zaczynaja sie conajmniej od 0!
									//Jesli 0 to zaczynaja sie od min>0
			 double Hoffs=0.33,		//Ulamek szerokosci przeznaczony na perspektywe
			 double	Voffs=0.33		//Ulamek wysokosci  przeznaczony na perspektywe
			 );
true_color_manhattan_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru		
			 data_source_base* idatas,int menage_d=0,//datas-dane o wysokosciach
			 data_source_base* RedData=NULL,int menage_r=0,
			 data_source_base* GreenData=NULL,int menage_g=0,
			 data_source_base* BlueData=NULL,int menage_b=0,
			 //Musza miec zgodne geometrie 
			 int zero_mod=1,		//Slupki zaczynaja sie conajmniej od 0!
									//Jesli 0 to zaczynaja sie od min>0
			 double Hoffs=0.33,		//Ulamek szerokosci przeznaczony na perspektywe
			 double	Voffs=0.33		//Ulamek wysokosci  przeznaczony na perspektywe
			 );
// IMPLEMENTATION OF VIRTUAL METHODS
struct config
	{
	config(int c_offs=0,unsigned col_wire=0,int z=0):
				color_offset(c_offs),wire(col_wire),zero_mode(z){}
	int color_offset;
	unsigned wire;
	int zero_mode;
	};
int configure(const void* config);//Konfiguruje tryb wyswietlania	
int setseries(size_t index,data_source_base* data,int menage=0);//zwraca -1 jesli indeks za duzy
data_source_base* getseries(size_t index);//zwraca NULL jesli indeks za duzy
// reals[] zawiera wysokosc i wartosc ustalajaca kolor
int _rescale_data_point(const double reals[4],long in_area[4]);//zwraca -1 jesli nie w oknie
void _replot();// Rysuje wlasciwy wykres a pod nim ewentualnie legende

// ONLY FOR DEVELOPERS OF THIS CLASS
private:
const geometry_base* read_dim(size_t& aa,size_t& bb);
int mode;
int c_offset;
wb_color wire;
unsigned AA;
unsigned BB;
double Hoffs;
double Voffs;
int d_menage;
int c_menage[3];
data_source_base* datas;
data_source_base* colors[3];
scaling_info s_data;
scaling_info s_colo[3];
wb_ptr<geometry_base> zastepnik;
};


class sequence_graph:public graph// Kolejne punkty ewentualnie polaczone liniami. Dowolna ilosc seri!!!
//-----------------------------------------------------------------------------------------------------
{
public:
//HELP:
const char* user_help_text(){ return
"N SEQUENCES PLOT:\n"
"2 Dimensional, many series.\n"
"Case number or time on X axes.\n"
"index 0..N: series to print\n"
"By default colors range are from 0 to 255\n"
"and must contain as many colors as series exists.\n"
"If textcolor!=bacground max,min legend is printing \n"
"and series names are printing inside graph!\n"
"\n";}
//DESTRUCTOR
~sequence_graph();
//CONSTRUCTOR(S)
sequence_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru		
			   int N,					//Ilosc serii
			   series_info* series,		//Tablica struktur zawierajacych informacje o seriach
                                        //Moze przejmowac w zarzad serie i ich obiekty rys. punkty 
			   int mode=0,	//0 - tryb z pelnym reskalowaniem
							//1 - tryb ze wspolnym minimum,maximum,pobieranym z parametrow i serii
							//2 - tryb z zafiksowanym minimum i maksimum
			   double min=0.0,double max=0.0);
sequence_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru		
			   int N,					//Ilosc serii
			   data_source_base** series,//Tablica serii Struktury informacji o seriach zostana wypelnione domyslnie
                                        //Nie ma zarzadzania pamiecia serii.
			   int mode=0,	//0 - tryb z pelnym reskalowaniem
							//1 - tryb ze wspolnym minimum,maximum,pobieranym z parametrow i serii
							//2 - tryb z zafiksowanym minimum i maksimum
			   double min=0.0,double max=0.0);
// IMPLEMENTATION OF VIRTUAL METHODS
struct config_seq
	{
	config_seq(int im=0,double imi=0.0,double ima=0.0):
				mode(im),min(imi),max(ima){}
	int mode;  
	double min;
	double max;
	};
int configure(const void* config); //Parametr typu config_seq. Jesli NULL to tryb 0;
int setseries(size_t index,data_source_base* data,int menage=0);//zwraca -1 jesli indeks za duzy
data_source_base* getseries(size_t index);//zwraca NULL jesli indeks za duzy
// reals[] zawiera M wysokosci (Y) i numer kroku dla ustalenia X-a
int _rescale_data_point(const double reals[/*M+1*/],long in_area[/*M+1*/]);//zwraca -1 jesli nie w oknie
void _replot();// Rysuje wlasciwy wykres a pod nim ewentualnie legende

// ONLY FOR DEVELOPERS OF THIS CLASS
unsigned color(unsigned val);
private:
size_t M;			//Rozmiar tablicy serii. Nie wszystkie musza byc pelne
int mode;			//Aktualny tryb pracy
series_info* series;//Tablica serii
scaling_info* scales;//Tablica skal
scaling_info scolors;//Dla niepodanych kolorow
scaling_info scale_x;//Dla osi X
scaling_info scale_y;//Dla osi Y w trybie 1 i 2
};


class scatter_graph:public graph//Punkty w dwu wymiarach (dowolne X i Y)
//------------------------------------------------------------------------
{
public:
//HELP:
const char* user_help_text(){ return
"SCATTER PLOT GRAPH:\n"
"2 Dimensional, 4 series:\n"
"index 1: X's of points\n"
"index 2: Y's of points\n"
"index 3: colors of points (optional - may be NULL)\n"
"index 4: size of points (optional - may be NULL)\n"
"By default colors range are from 0 to 255.\n"
"and default bar color is 128.\n"
"If textcolor!=bacground legend is printing\n"
"User can configure type of points by class\n"
"derived from config_point.\n"
"\n";}
//DESTRUCTOR
~scatter_graph();
//CONSTRUCTOR(S)
scatter_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru		
			 data_source_base* Xdata,int menage_x  ,//dane o X-ach
			 data_source_base* Ydata,int menage_y=0,//dane o Y-ach
			 data_source_base* colors=NULL,int menage_c=0,//dane o kolorach
			 data_source_base* sizes=NULL,int menage_s=0,//dane o rozmiarach
			 config_point*  fig=new hash_point,int fmenage=1//figura domyslna
			 );

// IMPLEMENTATION OF VIRTUAL METHODS
struct config_scat
{
config_scat(config_point*  ifig=NULL,int imen=0):fig(ifig),menage(imen){}
config_point*  fig;
int menage;
};

int configure(const void*);//Parametr typu scatter_graph::config_scat*
int setseries(size_t index,data_source_base* data,int menage=0);//zwraca -1 jesli indeks za duzy
data_source_base* getseries(size_t index);//zwraca NULL jesli indeks za duzy
// reals[] zawiera X,Y ewentualnie wartosc dla koloru i rozmiaru.
int _rescale_data_point(const double reals[4],long in_area[4]);//zwraca -1 jesli nie w oknie
void _replot();// Rysuje wlasciwy wykres a pod nim ewentualnie legende

//Special for this class
void fix_X_minmax(double min,double max)
{
	scale_x.fix_min=scale_x.fix_max=0;//Odfiksuj
	if(min>-DBL_MAX)
	{
		scale_x.min=min;
		scale_x.fix_min=1;
	}
	if(max>-DBL_MAX)
	{
		scale_x.max=max;
		scale_x.fix_max=1;
	}
}
void fix_Y_minmax(double min,double max)
{
	scale_y.fix_min=scale_y.fix_max=0;//Odfiksuj
	if(min>-DBL_MAX)
	{
		scale_y.min=min;
		scale_y.fix_min=1;
	}
	if(max>-DBL_MAX)
	{
		scale_y.max=max;
		scale_y.fix_max=1;
	}
}

// ONLY FOR DEVELOPERS OF THIS CLASS
protected:
config_point* CurrConfig;
int     menage_p;
data_source_base* Xdata;
int		menage_x;
data_source_base* Ydata;
int		menage_y;
data_source_base* colors; 
int		menage_c;
data_source_base* sizes;
int		menage_s;
scaling_info scale_x;//Skalowanie dla X-ow
scaling_info scale_y;//Skalowanie dla Y-ow
scaling_info scale_c;//Skalowanie dla kolorow
scaling_info scale_s;//Skalowanie dla rozmiarow
};


class net_graph:public scatter_graph//Punkty w dwu wymiarach (dowolne X i Y)
//------------------------------------------------------------------------
{
public:
//HELP:
const char* user_help_text(){ return
"NETWORK PLOT GRAPH:\n"
"2 Dimensional\n" 

"4 SERIES FOR POINTS:\n"
"index 1: X's of points\n"
"index 2: Y's of points\n"

"index 3: colors of points (optional - may be NULL)\n"
"index 4: size of points (optional - may be NULL)\n"

"4 SERIES FOR CONNECTIONS\n"
"index 5: Source point index\n"
"index 6: Target point index\n"

"index 7: Arrow yes/no (0/1)\n"
"index 8: colors for lines\n"

"By default colors range are from 0 to 255.\n"
"and default color is 128.\n"
"If textcolor!=bacground legend is printing\n"
"Default point is \"dot\" but user can provide config_point object for \n"
"configure point printing.\n"
"\n";}
//DESTRUCTOR
~net_graph();
//CONSTRUCTOR(S)
net_graph(int x1,int y1,int x2,int y2,//Polozenie obszaru		
			 data_source_base* Xdata,int menage_x  ,//dane o X-ach
			 data_source_base* Ydata,int menage_y,//dane o Y-ach
             data_source_base* Sources,int menage_so,//indeksy zrodel
             data_source_base* Targets,int menage_t,//indeksy celów
			 
             data_source_base* Colors=NULL,int menage_c=0,//dane o kolorach
			 data_source_base* Sizes=NULL,int menage_s=0,//dane o rozmiarach
             data_source_base* Arrows=NULL,int menage_a=0,//rozmiary strzalek - 0 brak
             data_source_base* ArrColors=NULL,int menage_ac=0,//Kolory strzalek
			 
			 config_point*  fig=NULL,int fmenage=0//figura. Domyslnie brak bo moga byc same polaczenia!
			 );

// IMPLEMENTATION OF VIRTUAL METHODS
//???NIEPOTRZEBNE BO DZIEDZICZONE?
//int configure(const void*);//Parametr typu scatter_graph::config_scat*
int setseries(size_t index,data_source_base* data,int menage=0);//zwraca -1 jesli indeks za duzy
data_source_base* getseries(size_t index);//zwraca NULL jesli indeks za duzy
// reals[] zawiera X,Y ewentualnie wartosc dla koloru i rozmiaru.
//int _rescale_data_point(const double reals[4],long in_area[4]);//zwraca -1 jesli nie w oknie
void _replot();// Rysuje wlasciwy wykres a pod nim ewentualnie legende

// ONLY FOR DEVELOPERS OF THIS CLASS
protected:
data_source_base* Sources;
int		menage_so;
data_source_base* Targets;
int		menage_t;
data_source_base* Arrows;
int		menage_a;
data_source_base* ArrColors; 
int		menage_ac;

//scaling_info scale_s;//Skalowanie dla rozmiarow grotow
scaling_info scale_ac;//Skalowanie dla kolorow lini
};

template<class DATA_SOURCE, int DIRECT_COLOR >
void fast_carpet_graph<DATA_SOURCE,DIRECT_COLOR >::_replot()// Rysuje wlasciwy wykres a pod nim ewentualnie legende
{
int x1=getstartx();
int y1=getstarty();
int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
int y2=y1+getheight()-1;														assert(x1<=x2);//Czy aby na pewno
																				assert(y1<=y2);//sensowne okno. Moze miec zerowy rozmiar ale nie ujemny
read_dim(AA,BB);
if(AA<=1 && BB<=1)
	{
	print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CInvalid data");
	return;
	}

double min;//=0;
double max;//=1;
size_t num;//=0;
data->bounds(num,min,max);
//double missing=data->missing();

//Do skalowania kolorow
if(DIRECT_COLOR)
{
	//NIEPOTRZEBNE?
	///???
}
else
{
	mm.set(min,max,c_range.end-c_range.start+0.999);
}

//R y s o w a n i e  l e g e n d y
if(t_colors.start!=getbackground() && 
	 char_height('X')<getheight())
	{	
	int x=x1;
	int y=y2-char_height('X')+1;//+1 bo y2 ma byc zarysowane
	int width=0;
//----------
	y2=y;//Zabiera dolna czesc na legende
//----------
	width=print_width(x,y,(x2-x)/3*2,t_colors.start,getbackground(),
					"%s",data->name());
	
	if(width==0) goto KWADRACIKI;
		else	 x+=width+1;

	width=print_width(x,y,(x2-x)/2,c_range.start,
						  c_range.start!=getbackground()?getbackground():c_range.end,
						  "<%g",min);
	
	if(width==0) goto KWADRACIKI;
		else	x+=width+1;
	
	width=print_width(x,y,x2-x,c_range.end,
						  c_range.end!=getbackground()?getbackground():c_range.start,
						",%g>",max);
	}

//Rysowanie skali -jesli sa conajmniej dwa kolory i jest miejsce na conajmniej 2 pixele
if( !DIRECT_COLOR )
if( (x2-x1) >=	double((AA>BB?AA:BB)+6) && //W zasadzie dobrze, ale male szansze zeby przekroczylo zakres uint32
	c_range.end>c_range.start	&& 
	c_range.end-c_range.start>=2	)
	{
		c_range.plot(x2-6,y1,x2,y2);
		x2-=7;
	}

//Rysowanie punktow vel "KWADRACIKOW"  a właściwie tylko punktów
KWADRACIKI:																		assert(x2>x1);
																				assert(y2>y1);
if(AA>=1 && BB>=1 && AA<=size_t(x2-x1+1) && BB<=size_t(y2-y1+1))
	{
	//Tu CAST na znany dobrze typ zrodla - zeby byl szybszy dostep
	//----------------------------------------
	DATA_SOURCE* fast_data=(DATA_SOURCE*)data;
	//----------------------------------------
																				assert(c_range.end-c_range.start>=1);

	size_t i,j;//Indeksy po wierszach i kolumnach
	int width=x2-x1+1;//Juz moga byc inne
	int height=y2-y1+1;//Niz dla calego obszaru	
	int offsetA=(AA<double(width) ?(width-AA) /2:0);
	int offsetB=(BB<double(height)?(height-BB)/2:0);
	offsetA+=x1;
	offsetB+=y1;

	//Rysowanie
	data_source_base::iteratorh  h = fast_data->reset();
	
	wb_color back=getbackground();//Dla sprawdzania kiedy kolor kwadratu taki jak kolor tla.
	
	//Pixelami panowie!!!
	if(DIRECT_COLOR)
		{
		   for(j=0;j<BB;j++)
				for(i=0;i<AA;i++)
				{
				  double test=fast_data->get(h);
				  if(data->is_missing(test))
					continue;//Nie rysowac jesli wartosc nieosiagalna
				  unsigned C=test;//Zakładamy że to surowe kolory?
				  unsigned R=(C & 0x0000ff);
				  unsigned G=(C & 0x00ff00)>>8;
				  unsigned B=(C & 0xff0000)>>16;
				//color+=c_range.start;
				  plot_rgb(offsetA+i,offsetB+j,R,G,B);
				}
		}
		else
		{ //SKALA KOLORÓW
			for(j=0;j<BB;j++)
				for(i=0;i<AA;i++)
				{
				double test;
				wb_color color=(wb_color)(mm.get(test=fast_data->get(h)));
				if(data->is_missing(test))
					continue;//Nie rysowac jesli wartosc nieosiagalna
				color+=c_range.start;
				plot(offsetA+i,offsetB+j,color);
				}
	 }

	fast_data->close(h);
	x1=offsetA+AA+1;//Dla skali zostaje prawy margines
	}
	else
	{
	print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CTo small area");
	}

//NIE_DA_SIE:;
}

/*
class scatter3D_graph:public graph//Punkty w trzech wymiarach (dowolne X,Y,Z)
*/

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



