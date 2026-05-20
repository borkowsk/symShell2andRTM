/// @file
/// @brief **THE TYPE OF MAIN "SCREEN" AREA MANAGER** /<br>
///         _KLASA GŁÓWNEGO ZARZĄDCY OBSZARÓW "EKRANU"._
/// @date 2026-05-20 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_MAIN_MNGR_HPP_INCLUDED_
#define SYMSHELL2_MAIN_MNGR_HPP_INCLUDED_
#ifndef __cplusplus
#error C++ required
#endif

#include "areamngr.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "modernize-use-auto"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

/// Zmodernizowane klasy do symulacji w C++.
namespace sym2 { namespace visual
{

/// @brief @EN{ A class for the main manager of single-window application areas based on the portable _SymShellLight_ interface. }
///        @PL{ Klasa głównego zarządcy obszarów aplikacji jednookiennych oparta na przenośnym interfejsie _SymShellLight_. }
/// @details ...
///		J e d n o w c i e l e n i o w a,  czyli w danej aplikacji SymShell-a może być tylko jedna instancja tej klasy.
///		Zajmuje się inicjalizacja grafiki i obsługa zdarzeń w dwu trybach:
///		- wywołania w reakcji na zdarzenie.
///		- w zamkniętej pętli wywołującej wirtualną metodę `on_idle`, gdy brak zdarzeń i jest ustawiona flaga pracy w tle.
class main_area_manager: public area_manager
//------------------------------------------
{
    /// @name Główne zmienne systemowe.
    /// @{
    static int		initialized;			///< Flaga określająca, czy inicjalizacja grafiki została już dokonana.
    static int		how_many_areas;			///< Określa, ile jest zarządzanych obszarów.
    static wb_color color_marker;			///< Kolor do znakowania obszarów przez użytkownika.
    static int		idle_must_work;			///< Flaga wywoływania `on_idle`.
    static wb_pchar	old_win_title;			///< Zapamiętana poprzednia nazwa głównego okna, ustalona przez `set_title`.
    static void*	sub_menu_handle;		///< Uchwyt do odpowiedniego submenu. Do modyfikacji.
                                            ///< @note Aktualnie działa tylko pod MS Windows.
    /// @}

    /// @name Dump file support
    /// @{
    static wb_pchar	def_dump_name;				///< Baza nazwy pliku zrzutu.
    static size_t	screen_number;				///< Numer pliku zrzutu.
    static int 		screen_number_precision;	///< Określa, z ilu cyfr będzie maksymalny numer zrzutu. Domyślnie 7.
    /// @}

    //jakby static bo globalne z przestrzeni "C":
    //WB_error_enter_before_clean=1;

    /// @name Zabezpieczenia przed wielokrotnymi instancjami.
    /// @{
    static int		counter;		///< StepCounter obiektów typu "main_area_manager". Ma być 1.
    /// Operacja przypisania jest zabroniona.
    void operator = (const main_area_manager&); //Nie wolno!!!
    /// @}

public:
    /// @name KONSTRUKTORY i DESTRUKTOR
    /// @{

    /// \brief Konstruktor dający zarządcę o określonym rozmiarze listy obszarów.
    /// \note Wywołanie więcej niż jednego konstruktora tego typu obiektów powoduje aborcje procesu!!!
    /// \param size to rozmiar listy.
    /// \param width to wymagana wstępnie szerokość obszaru roboczego (okna systemowego albo pliku).
    /// \param height to wymagana wstępnie wysokość obszaru roboczego (okna systemowego albo pliku).
    /// \param ibkg to kolor tła. Domyślnie szarość 50%.
    main_area_manager(size_t size,
                      int width, int height,
                      unsigned ibkg=default_half_gray
                    );

    // Konstruktor z listą częściowo wypełnioną (PO CO?) - nigdzie nie był używany.
    // >> main_area_manager(size_t size,
    //				  int width, int height,
    //				  //bkg i frm domyślne — można zmienić potem
    //				  drawable_base* /*first...NULL*/);

    /// Wirtualny destruktor.
    ~main_area_manager() override;
    /// @}

    // AKCESORY OGÓLNE — MUSZĄ BYĆ INNE BO GŁÓWNY MANAGER SPRZĘŻONY Z MENU
    //----------------------------------------------------------------------

    /// @name Wymienianie i usuwanie z listy obszarów (jest na razie zabronione).
    /// @details  Te metody zawsze zwracają -1 i nic nie robią.
    /// @{

    /// Usuwanie z listy jest zabronione.
    /// Co raz włożono, musi zostać, choć można zminimalizować.
    int    remove(size_t index) override {return -1;}
    int    replace(const char* nam,wb_ptr<drawable_base> drw) override {return -1;}
    int    replace(size_t    index,wb_ptr<drawable_base> drw) override {return -1;}
    /// @}

    /// @name Wstawianie obszarów na listę zarządzania.
    /// @details Poza dodaniem do listy, pod MS Windows, dodają też nazwę okna do menu.
    /// @{

    /// @brief TA metoda zabiera zawartość w zarząd, czyli będzie DEALOKOWAĆ. Można podać tylko zmienną ze sterty.
    int    insert(drawable_base*	drw)  override { wb_ptr<drawable_base> tmp(drw); return insert(tmp);}

    /// @brief Ta metoda dodaje obszar wskazywany przez ćwierćinteligentny wskaźnik. Zwraca pozycje albo -1(błąd).
    int    insert(wb_ptr<drawable_base>	drw) override ;
    /// @}

    /// @name Ukrywanie, czyli minimalizowanie obszarów.
    /// @{
    int    minimize(size_t index) override ; ///< @param index pojedynczego obszaru.
    int    minimize(const wb_dynarray<int>& lst_index) override ; ///< @param lst to lista indeksów obszarów.
    /// @}

    /// @name Odtwarzanie pierwotnego położenia i rozmiaru obszarów.
    /// @{
    int    original(size_t  index) override ; ///< @param index pojedynczego obszaru.
    int    original(const wb_dynarray<int>& lst_index) override ; ///< @param lst to lista indeksów obszarów.
    /// @}

    /// @name Przywracanie widoczności.
    /// @details Używają poprzedniego położenia i rozmiarów uprzednio ukrytego (minimize) obszaru.
    /// @{

    /// Przywracanie pojedynczego obszaru, wskazanego przez `index`.
    int    restore(size_t  index) override ;

    /// Przywracanie WSZYSTKICH uprzednio ukrytych (minimize) obszarów.
    int    restore(/*ALL*/) override ; //Robi restore dla wszystkich pod-obszarów
    /// @}

    /// @name AKCESORY POL
    /// @{
    static int			is_initialised()  { return initialized;}
    static wb_color		get_marker()      { return color_marker;}
    static const char*	get_dump_name()   { return def_dump_name.get();}
    static size_t&		get_dump_number() { return screen_number;}
    static void 		reset_dump_number(unsigned reset_val=0) { screen_number=reset_val;}

    int					set_marker(wb_color new_mark); /// @returns 1, jeśli OK, lub 0 jak błąd.
    void				set_dump_name(const char* name,size_t number=-1);
    /// @}

    /// @name METODY SPECYFICZNE DLA KLASY `main_area_manager`.
    /// @{

    /// @brief Inicjacja trybu graficznego w momencie dogodnym dla projektanta aplikacji.
    /// @returns 1, jeżeli wszystko OK.
    /// @details
    /// Przed pomyślnym wykonaniem `start` nie wolno bezpośrednio lub pośrednio wywoływać funkcji rysujących po ekranie.
    /// Funkcja jest wirtualna, na wypadek, gdyby klasa dziedzicząca miała także coś do zrobienia na początku.
    virtual int start(const char* win_title, int argc, const char* argv[], int double_buffering=-1);

    /// Aktualny tytuł systemowy okna.
    const char* get_title() override;

    /// @brief Zmiana tytułu okna systemowego.
    /// @returns Poprzedni tytuł lub NULL jak nie był ustalony.
    int set_main_title(const char* win_title);

    /// Metoda ukrywająca dostęp do `WB_error_enter_before_clean` z modułu SymShellLight.
    void need_confirmation_before_clean(int yes=0);

    /// Obsługa wszelkich zdarzeń przychodzących z zewnątrz. Wychodzi, gdy nie ma już co robić.
    void process_input();

    /// @brief Tworzy tekstowy lufcik o nazwie HELP opisujący używanie managera.
    /// Jest też wywoływane przy pierwszym naciśnięciu Ctrl-I przez użytkownika.
    virtual
    void make_help_area(const char* text=NULL);

    /// Zrzut kolejnego ekranu.
    void dump_screen();
    /// @}

    /// @name Przechwycenie całości sterowania
    /// @{
    void run_input_loop();				///< Przechwytuje, ale wychodzi, gdy user zakończy program lub zostanie wywołane...
    void break_input_loop();			///< Przerywa przechwycenie sterowania.
    void enable_background();			///< Odblokowuje pracę w tle, czyli wywoływanie "on_idle".
    static void disable_background()	///< Blokuje pracę w tle, czyli wywoływanie "on_idle".
    { idle_must_work=0;}
    static int  background_enabled()	///< Informuje, czy praca w tle się odbywa.
    { return idle_must_work!=0;}
    /// @}

protected:
    /// @name ELASTYCZNE UCHWYTY OBSŁUGI
    /// @{
    virtual int _pre_process_input(int input_char);  ///< @brief Przed obsługą domyślną zdarzenia.
                                                     ///< @return 1, jeśli zdołał obsłużyć.
    virtual int _post_process_input(int input_char); ///< @brief Po obsłudze domyślnej, jeśli wcześniej nie obsłużono.
                                                     ///< @return 1, jeśli obsłużył.
    virtual int _on_idle(); ///< Uruchamiane przez `run_input_loop`, gdy nie ma zdarzeń do obsługiwania.
    /// @}
};

}} // namespace sym2

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
#endif //SYMSHELL2_MAIN_MNGR_HPP_INCLUDED_





