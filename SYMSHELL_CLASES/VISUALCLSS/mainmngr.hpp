//     KLASA GLOWNEGO ZARZADCY OBSZAROW EKRANU
//------------------------------------------------
#ifndef __MAINMNGR_HPP__
#define __MAINMNGR_HPP__
#ifndef __cplusplus
#error C++ required
#endif

#include "areamngr.hpp"
//J e d n o w c i e l e n i o w a    klasa glownego zarzadcy obszarow 
//przeznaczona dla przenosnej platformy graficznej SYMSHELL'a . 

//Zajmuje sie inicjalizacja grafiki i obsluga zdarzen w dwu trybach:
//		-wywolania w reakcji na zdarzenie
//		-w zamknietej petli wywolujacej wirtualna metode on_idle gdy brak zdarzen i
//         jest ustawiona flaga pracy w tle

class main_area_menager:public area_menager
{
static int		initialized;
static int		counter;	//Licznik obiektow typu "main_area_menager". Ma byc 1
static wb_color Marker;		//Kolor do znakowania przez usera
static int		idle_must_work;	//Flaga wywolywania on_idle
static wb_pchar	def_dump_name;  //Baza nazwy pliku zrzutu
static wb_pchar old_win_title;  //Zapamiêtana poprzednia nazwa okna ustalona przez settitle
static size_t	screen_number;  //Numer pliku zrzutu
static int 		screen_number_precision;//=7; //Ilu cyfrowy bêdzie maksymalny numer
static void*    WindowMenu;     //Uchwyt do odpowiedniego submenu
static int      HowManyAreas;   //Ile "zainsertowanych" obszarow

//jakby static: 
//WB_error_enter_before_clean=1;

void operator = (const main_area_menager&);//Nie wolno!!!
public:
~main_area_menager();//Wirtualny destruktor
//KONSTRUKTORY
//---------------
//Wywolanie wiecej niz jednego konstruktora powoduje aborcje procesu!!!
main_area_menager(size_t size, //Konstruktor dajacy zarzadce o okreslonym rozmiarze listy
				int width,int height,
				unsigned ibkg=default_half_gray
				);	
main_area_menager(size_t size,//Konstruktor z lista czesciowo wypelniona
				  int width,int height,
				  //bkg i frm domyslne - mozna zmienic potem
				  drawable_base* /*first...NULL*/);

//AKCESORY OGOLNE - MUSZA BYC INNE BO GLOWNY MENAGER SPRZEZONY Z MENU
//-------------------------------------------------------------------
int    remove(size_t index){return -1;} //Usuwanie z listy zabronione. Zwraca -1
int    replace(const char* nam,wb_ptr<drawable_base> drw){return -1;}//Wymienianie na liscie na razie  zabronione. Zwraca -1
int    replace(size_t    index,wb_ptr<drawable_base> drw){return -1;}//Wymienianie na liscie na razie  zabronione. Zwraca -1

int    insert(drawable_base*	drw){ wb_ptr<drawable_base> tmp(drw);return insert(tmp);}//Zabiera zawartoœæ w zarzad!
int    insert(wb_ptr<drawable_base>	drw);//Dodaje obszar do listy i do menu. Zwraca pozycje albo -1(blad)	

int    minimize(size_t index);//Ukrywa podobszar
int    minimize(const wb_dynarray<int>& lst);//Ukrywa podobszary

int    orginal(size_t  index);//Odtwarza pierwotne  polozenie i rozmiar obszaru
int    orginal(const wb_dynarray<int>& lst); //Robi orginal dla pod-obszarow

int    restore(size_t  index);//Odtwarza poprzednie polozenie i rozmiar obszaru
int    restore(/*ALL*/); //Robi restore dla wszystkich pod-obszarow

//AKCESORY POL
//-------------
int      is_initialised() { return initialized;}
wb_color get_marker(){return Marker;}
int      set_marker(wb_color newmark);//zwraca 1 jesli OK, lub 0

const char* get_dump_name(){ return def_dump_name.get();}
void	 set_dump_name(const char* name,size_t number=-1);

size_t&  dump_number(){return screen_number;}
void 	reset_dump_number(unsigned resetval=0){screen_number=resetval;}

//METODY
//--------
//Inicjacja trybu graficznego w momencie dogodnym dla projektanta aplikacji
int start(const char* wintitle,int argc=0,const char* argv[]=NULL,int double_buffering=-1);//Zwraca 1 jesli ok
//Przed pomyslnym wywolaniem start() nie wolno bezposrednio lub posrednio wywolywac
//jakichkolwiek funkcji rysujacych po ekranie.

//Zmiana tytu³u okna
const char* gettitle(); //Poprzedni tytu³ lub NULL jak nie by³ ustalony
int settitle(const char* wintitle);


//Metoda ukrywajaca dostep do WB_error_enter_before_clean
void need_confirmation_before_clean(int yes=0);

//Obsluga wszelkich zdarzen z zewnatrz
void process_input();//Wychodzi, gdy nie ma co robic

//Przechwycenie calosci sterowania
void run_input_loop();//Wychodzi gdy user "zakonczy" program lub 
void break_input_loop();//...zostanie wywolane
int background_enabled(){ return idle_must_work!=0;}
void disable_background(){ idle_must_work=0;} //Blokuje wywolywanie on_idle
void enable_background();					  //i odblokowuje
//Tworzy tekstowy lufcik o nazwie HELP opisujacy uzywanie menagera
//Jest tez wywolywane przy pierszym nacisnieciu Ctrl-I przez usera
virtual
void make_help_area(const char* text=NULL);
//Zrzut kolejnego ekranu
void dump_screen();
protected:
//ELASTYCZNE UCHWYTY OBSLUGI
//---------------------------
virtual int _pre_process_input(int input_char);//Przed obsluga domyslna. Zwraca 1 jesli obsluzyl.
virtual int _post_process_input(int input_char);//Po obsludze domyslnej. Zwraca 1 jesli obsluzyl.
virtual int _on_idle();//Uruchamiane przez run_input_loop() gdy nie ma zdarzen.
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



