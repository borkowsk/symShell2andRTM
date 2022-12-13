//     KLASY ZARZADCÓW OBSZAROW EKRANU
//--------------------------------------
#ifndef __AREAMNGR_HPP__
#define __AREAMNGR_HPP__
#ifndef __cplusplus
#error C++ required
#endif


//Niezbedne definicje bazowe
//------------------------------------------
#include "drawable.hpp"

//INTERFACE DO DOWOLNEGO ZARZADCY OBSZARU
//------------------------------------------
class area_menager_base:public drawable_base //Zeby mozna bylo zestawiac menagery w hierarchie
{
int		cont_actions;	//Flaga kontynuacji. Jesli 0 to wypada kiedy moze.
public:
virtual ~area_menager_base(){} //Pusty destruktor dla wymuszenia wirtualnosci destruktorow

area_menager_base(//Konieczny jakis konstruktor do przekazania parametrow drawable_base
						   //i zeby byl default.	
				int ix1=0,int iy1=0,int ix2=0,int iy2=0,
				unsigned ibkg=default_color,
				unsigned ifr=default_color):
				drawable_base(ix1,iy1,ix2,iy2,ibkg,ifr),
				cont_actions(1)
					{}

//Sterowanie przerywaniem dzialania menagera
virtual 
void need_break_action(int Yes=1){ cont_actions=!Yes;}
int should_continue(){return cont_actions;}//Czy user chcial przerwac

//	AKCESORY OGOLNE
//------------------
virtual int    insert(wb_ptr<drawable_base>	drw)=0;//Dodaje obszar do listy. Zwraca pozycje albo -1(blad)
virtual int    insert(drawable_base* drw)//Zabiera w zarzad zwykly wskaznik.
                {
			wb_ptr<drawable_base> temp(drw);
			//Inteligentny wskaznik tymczasowy od razu przekazuje "wglab"
			return insert(temp);
		}
virtual int    replace(const char* nam, wb_ptr<drawable_base> drw)=0;//Wymienia na liscie. Jak nie znajdzie to zwraca -1.
virtual int    replace(size_t    index, wb_ptr<drawable_base> drw)=0;//Wymienia na liscie. Jak bledne parametry to zwraca -1.
virtual int    remove(size_t index)//Usuwa z listy
                {
                        wb_ptr<drawable_base> Empty;
                        return replace(size_t(index),Empty);
                }
virtual int    search(const char* nam)=0;	//Odnajduje na liscie. Zwraca -----//----
virtual size_t get_size()=0;		//Podaje po prostu aktualny rozmiar listy lacznie z pozycjami pustymi

//	REAKCJE NA ZDAZENIA
//--------------------------
virtual int    on_click(int x,int y,int click)=0;//Przepytuje obszary z reakcji na punkt.
//Jesli on_click() zwraca 1 to mozna sie dowiedziec, który obszar wywo³ujac:
virtual int    get_last_lazy_area()=0;//zwroci -1 jesli juz raz wziete, lub inny blad
virtual int    on_margin_click(int x,int y,int click){return 2;}//Co robic gdy clikniete tlo menagera
virtual int    on_input(int input_char)=0;//Przepytuje obszary z chca znak
virtual int    on_change(const gps_area& ar)=0;//Reguje na zmiane rozmiarow lub polozenia wlasnego obszaru
virtual void   replot(const gps_area& ar)=0;//Odrysowuje obszary "nadepniete" przez "ar"
virtual void   _replot()=0;	//Odrysowuje wszystkie (widoczne) obszary
//  MANIPULATORY
//----------------
virtual int    refresh(size_t index)=0; //Odrysowuje obszar jesli nie zminimalizowany
virtual int    mark(size_t index,wb_color frame=default_color)=0;//Zaznacza obszar 
virtual int    mark_all(wb_color frame=default_color)=0;//Zaznacza wszytkie widoczne
virtual int    unmark(size_t index)=0;	 //i odznacza obszar
virtual int    is_minimized(size_t index)=0;//Informuje czy jest zminimalizowany
virtual int    is_marked(size_t index)=0;//Informuje czy jest zaznaczony
virtual wb_dynarray<int> get_marked(wb_color filtr=default_color,int unm=0)=0;//Zwraca liste zaznaczonych obszarow. 
									    // Jesli what=default color to wszystkie zaznaczone.
										// i opcjonalnie zdejmuje zaznaczenie
//...DLA POJEDYNCZYCH OBSZAROW
virtual int    set_input(size_t index)=0;//Ustala obszar jako pierwszy do wejscia z klawiatury lub zdarzen menu
virtual int    maximize(size_t index)=0;//Oddaje podobszarowi caly zarzadzany obszar
virtual int		      get_maximized()=0;//Zwraca idex zmaksymalizowanego okna lub -1
virtual int    minimize(size_t index)=0;//Ukrywa podobszar
virtual int    restore(size_t  index)=0;//Odtwarza poprzednie polozenie i rozmiar obszaru
virtual int    orginal(size_t  index)=0;//Odtwarza pierwotne  polozenie i rozmiar obszaru
virtual int    as_orginal(size_t index)=0;//Uznaje aktualne polozenie obszaru za orginalne

//...DLA GRUP OBSZAROW
virtual int    minimize(const wb_dynarray<int>& lst)=0;//Ukrywa podobszary
virtual int    restore(const wb_dynarray<int>& lst)=0; //Robi restore dla pod-obszarow
virtual int    restore(/*ALL*/)=0; //Robi restore dla wszystkich pod-obszarow
virtual int    orginal(const wb_dynarray<int>& lst)=0; //Robi orginal dla pod-obszarow
virtual int    tile(const wb_dynarray<int>& lst)=0;    //Rearanzuje na chama, czyli po rowno, albo -1 jak nie da sie
virtual int    arrange(const wb_dynarray<int>& lst)=0; //Inteligentnie rearanzuje.

// AKCESORY poszczegolnych obszarow
virtual wb_ptr<drawable_base>& get(size_t index)=0;//Mozliwosci modyfikacji, ale trzeba pamietac
						//ze pewne informacje sa zapisywane w zarzadcy w zwiazku z pozycja
virtual drawable_base /*const*/* get_ptr(size_t index)=0; //Bez mozliwosci modyfikacji i zwolnienia
};



//Klasa najprostrzego, nieagresywnego, zarzadcy obszarow. 
//Zaklada pelna wladze nad obszarami, a w szczegolnosci nad ich pamiecia. 
//Zdarzenia zewnetrzne trzeba przekazac explicite - 
// - menager nie zawlaszcza ich samodzielnie, a tymbardziej nie
//zabiera watku sterowania.
class area_menager:public area_menager_base
{
protected:
struct internal
	{
	wb_ptr<drawable_base> ptr;//wskaznik do obszaru
	gps_area          orginal;//parametry obszaru przy wstawieniu
	gps_area            saved;//i w wersji sredniowymiarowej
	wb_color        org_frame;//orginalny kolor ramki gdy markowany
	int                mark:1;//flaga zamarkowania obszaru
	int			  minimized:1;//flaga zminimalizowania obszaru
	int				  locking;//Nie wolno usunac poza destruktorem menagera
	internal():
		mark(0),
		minimized(0),
		locking(0),
		org_frame(default_color){}
	};

wb_dynarray<internal> tab;//tablica obszarow
int maximized;//obszar zajmujacy wszystko
int grabbed;//obszar w pierwszym rzedzie obslugujacy wejscie
int lazy;  //obszar ktory ostatnio nie obsluzyl myszy

public:
	~area_menager();//Wirtualny destruktor
	area_menager(size_t size, //Konstruktor dajacy zarzadce o okreslonym rozmiarze listy
				int ix1,int iy1,int ix2,int iy2,
				unsigned ibkg=default_color,
				unsigned ifr=default_color);	
//	AKCESORY OGOLNE
//------------------
 size_t get_size();		//Podaje po prostu aktualny rozmiar listy lacznie z pozycjami pustymi
 int    insert(drawable_base*	drw){ wb_ptr<drawable_base> H(drw);return insert(H);}//Zabiera w zarzad!
 int    insert(wb_ptr<drawable_base>	drw);//Dodaje obszar do listy. Zwraca pozycje albo -1(blad)
 int    replace(const char* nam,wb_ptr<drawable_base> drw);//Wymienia na liscie. Jak nie znajdzie to zwraca -1.
 int    replace(size_t    index,wb_ptr<drawable_base> drw);//Wymienia na liscie. Jak bledne parametry to zwraca -1.
 int    search(const char* nam);	//Odnajduje na liscie. Zwraca -----//----	
 //	REAKCJE NA ZDAZENIA
//--------------------------
 int    on_click(int x,int y,int click);//Przepytuje obszary z reakcji na punkt.
			//Jesli on_click() zwraca 1 to mozna sie dowiedziec, który obszar wywo³ujac:
 int    get_last_lazy_area();//zwroci -1 jesli juz raz wziete, lub inny blad
 int    on_input(int input_char);//Przepytuje obszary z chca znak
 int    on_change(const gps_area& ar);//Reguje na zmiane rozmiarow lub polozenia wlasnego obszaru
 void   replot(int flu=1){drawable_base::replot(flu);}//Odrysuj wszystko
 void   replot(const gps_area& ar);//Odrysowuje obszary "nadepniete" przez "ar"
 void   _replot();	//Odrysowuje wszystkie (widoczne) obszary
//  MANIPULATORY
//----------------
 int    mark(size_t index,wb_color frame=default_color);//Zaznacza obszar 
 int    mark_all(wb_color frame=default_color);//Zaznacza wszytkie widoczne
 int    unmark(size_t index);	//i odznacza obszar
 int    is_marked(size_t index);//Informuje czy jest zaznaczony
 int    is_minimized(size_t index);//Informuje czy jest zminimalizowany
 wb_dynarray<int> get_marked(wb_color filtr=default_color,int unm=0);//Zwraca liste zaznaczonych obszarow. 
									    // Jesli what=default color to wszystkie zaznaczone.
										// i opcjonalnie zdejmuje zaznaczenie

//...DLA POJEDYNCZYCH OBSZAROW
 int	get_maximized(){return maximized;};//Zwraca idex zmaksymalizowanego okna lub -1


 int    set_input(size_t index);//Ustala obszar jako pierwszy do wejscia z klawiatury lub zdarzen menu
 int    maximize(size_t index);//Oddaje podobszarowi caly zarzadzany obszar
 int    minimize(size_t index);//Ukrywa podobszar
 int    restore(size_t  index);//Odtwarza poprzednie polozenie i rozmiar obszaru
 int    orginal(size_t  index);//Odtwarza pierwotne  polozenie i rozmiar obszaru
 int    as_orginal(size_t index);//Uznaje aktualne polozenie obszaru za orginalne

//...DLA GRUP OBSZAROW
 int	refresh(size_t index); //Odrysowuje obszar jesli nie zminimalizowany
 int    minimize(const wb_dynarray<int>& lst);//Ukrywa podobszary
 int    restore(const wb_dynarray<int>& lst); //Robi restore dla pod-obszarow
 int    restore(/*ALL*/); //Robi restore dla wszystkich pod-obszarow
 int    orginal(const wb_dynarray<int>& lst); //Robi orginal dla pod-obszarow
 int    tile(const wb_dynarray<int>& lst);    //Rearanzuje na chama, czyli po rowno, albo -1 jak nie da sie
 int    arrange(const wb_dynarray<int>& lst); //Inteligentnie rearanzuje.

 // AKCESORY poszczegolnych obszarow
 wb_ptr<drawable_base>&  get(size_t index);//Mozliwosci modyfikacji, ale trzeba pamietac
						//ze pewne informacje sa zapisywane w zarzadcy w zwiazku z pozycja
 drawable_base /*const*/* get_ptr(size_t index); //Bez mozliwosci modyfikacji i zwolnienia
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



