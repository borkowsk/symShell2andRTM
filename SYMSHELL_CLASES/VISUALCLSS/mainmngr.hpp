/// @file
/// @brief KLASA GŁÓWNEGO ZARZĄDCY OBSZARÓW EKRANU
/// @date 2026-05-11 (modified)
// ********************************************************************************************************************
//
#ifndef __MAINMNGR_HPP__
#define __MAINMNGR_HPP__
#ifndef __cplusplus
#error C++ required
#endif

#include "areamngr.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "modernize-use-auto"

/// Zmodernizowane klasy do symulacji w C++
namespace symshell2
{

//J e d n o w c i e l e n i o w a  klasa głównego zarządcy obszarów aplikacji jednookiennych.
//przeznaczona dla przenośnej platformy graficznej SYMSHELL'a .
//
//Zajmuje się inicjalizacja grafiki i obsługa zdarzeń w dwu trybach:
//		-wywołania w reakcji na zdarzenie
//		-w zamkniętej pętli wywołującej wirtualną metodę `on_idle`, gdy brak zdarzeń i
//         jest ustawiona flaga pracy w tle
class main_area_menager:public area_menager
{
    static int		initialized;
    static int		counter;	//Licznik obiektów typu "main_area_menager". Ma być 1
    static wb_color Marker;		//Kolor do znakowania przez usera
    static int		idle_must_work;	//Flaga wywoływania on_idle
    static wb_pchar	def_dump_name;  //Baza nazwy pliku zrzutu
    static wb_pchar old_win_title;  //Zapamiętana poprzednia nazwa okna ustalona przez set_title
    static size_t	screen_number;  //Numer pliku zrzutu
    static int 		screen_number_precision; //=7; //Z ilu cyfr będzie maksymalny numer?
    static void*    WindowMenu;     //Uchwyt do odpowiedniego submenu
    static int      HowManyAreas;   //Ile "zainsertowanych" obszarów
    //jakby static bo globalne z przestrzeni "C":
    //WB_error_enter_before_clean=1;

    // Operacja przypisania jest zabroniona!
    void operator = (const main_area_menager&); //Nie wolno!!!

public:
    ~main_area_menager() override; //Wirtualny destruktor
    //KONSTRUKTÓRY
    //---------------

    /// \brief Konstruktor dający zarządcę o określonym rozmiarze listy obszarów.
    /// \note Wywołanie więcej niż jednego konstruktora powoduje aborcje procesu!!!
    main_area_menager(size_t size,
                    int width,int height,
                    unsigned ibkg=default_half_gray
                    );

    // Konstruktor z lista częściowo wypełnioną (PO CO?)
    //main_area_menager(size_t size,
    //				  int width,int height,
    //				  //bkg i frm domyślne — można zmienić potem
    //				  drawable_base* /*first...NULL*/);

    // AKCESORY OGÓLNE — MUSZĄ BYĆ INNE BO GŁÓWNY MANAGER SPRZĘŻONY Z MENU
    //----------------------------------------------------------------------
    int    remove(size_t index) override {return -1;} //Usuwanie z listy jest zabronione. Zwraca -1

    int    replace(const char* nam,wb_ptr<drawable_base> drw) override {return -1;} //Wymienianie na liście na razie też zabronione.
    int    replace(size_t    index,wb_ptr<drawable_base> drw) override {return -1;} //Wymienianie na liście na razie też zabronione.

    //TA zabiera zawartość w zarząd!
    int    insert(drawable_base*	drw)  override { wb_ptr<drawable_base> tmp(drw); return insert(tmp);}

    // Dodaje obszar do listy i do menu. Zwraca pozycje albo -1(błąd)
    int    insert(wb_ptr<drawable_base>	drw) override ;

    int    minimize(size_t index) override ; //Ukrywa podobszar
    int    minimize(const wb_dynarray<int>& lst) override ; //Ukrywa pod-obszary

    int    original(size_t  index) override ; //Odtwarza pierwotne  położenie i rozmiar obszaru
    int    original(const wb_dynarray<int>& lst) override ; //Robi `original` dla pod-obszarów

    int    restore(size_t  index) override ; //Odtwarza poprzednie położenie i rozmiar obszaru
    int    restore(/*ALL*/) override ; //Robi restore dla wszystkich pod-obszarów

    //AKCESORY POL
    //-------------
    static int			is_initialised()  { return initialized;}
    static wb_color		get_marker()      { return Marker;}
    static const char*	get_dump_name()   { return def_dump_name.get();}
    static size_t&		dump_number()     {return screen_number;}
    static void 		reset_dump_number(unsigned resetval=0) {screen_number=resetval;}

    int			set_marker(wb_color newmark); //zwraca 1, jeśli OK, lub 0
    void		set_dump_name(const char* name,size_t number=-1);

    //METODY
    //--------

    // Inicjacja trybu graficznego w momencie dogodnym dla projektanta aplikacji
    // Zwraca 1, jeżeli wszystko OK
    // Przed pomyślnym wykonaniem start() nie wolno bezpośrednio lub pośrednio
    // wywoływać funkcji rysujących po ekranie.
    // Funkcja jest wirtualna, na wypadek, gdyby klasa dziedzicząca miała także coś do zrobienia na początku.
    virtual int start(const char* wintitle,int argc,const char* argv[],int double_buffering=-1);

    // Zmiana tytułu okna
    const char* get_title(); //Poprzedni tytuł lub NULL jak nie był ustalony
    int settitle(const char* wintitle);

    // Metoda ukrywająca dostęp do WB_error_enter_before_clean
    void need_confirmation_before_clean(int yes=0);

    // Obsługa wszelkich zdarzeń przychodzących z zewnątrz. Wychodzi, gdy nie ma już co robić.
    void process_input();

    //Przechwycenie całości sterowania
    void run_input_loop();      //Wychodzi, gdy user "zakończy" program lub
    void break_input_loop();    //...zostanie wywołane

    int  background_enabled(){ return idle_must_work!=0;}
    void disable_background(){ idle_must_work=0;} //Blokuje wywoływanie on_idle

    void enable_background();					  //i odblokowuje

    //Tworzy tekstowy lufcik o nazwie HELP opisujący używanie managera
    //Jest też wywoływane przy pierwszym naciśnięciu Ctrl-I przez użytkownika.
    virtual
    void make_help_area(const char* text=NULL);

    //Zrzut kolejnego ekranu
    void dump_screen();

protected:
    //ELASTYCZNE UCHWYTY OBSŁUGI
    //---------------------------
    virtual int _pre_process_input(int input_char);  //Przed obsługą domyślną. Zwraca 1, jeśli zdołał obsłużyć.
    virtual int _post_process_input(int input_char); //Po obsłudze domyślnej. Zwraca 1, jeśli obsłużył.
    virtual int _on_idle(); //Uruchamiane przez run_input_loop() gdy nie ma zdarzeń.
};

} // namespace symshell2

#pragma clang diagnostic pop
/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
#endif





