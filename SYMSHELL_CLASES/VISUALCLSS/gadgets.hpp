/// @file
/// @brief **GADGETS, I.E. AUXILIARY TYPES OF WINDOW AREAS** /<br>
///         _GADŻETY CZYLI POMOCNICZE TYPY OBSZARÓW OKNA._
/// @date 2026-05-19 (modified)
///     Służą głównie jako typy bazowe dla klas specjalizowanych, wykonujących
///     jakieś akcje w metodzie `_on_click` i ewentualnie podobnych.
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_GADGETS_HPP_INCLUDED_
#define SYMSHELL2_GADGETS_HPP_INCLUDED_
#ifndef __cplusplus
#error C++ required
#endif

#include "drawable.hpp"
#include "datasour.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

/// Zmodernizowane klasy do symulacji w C++
namespace sym2
{
    using namespace sym2::data;

/// @brief @EN{ . }
///        @PL{ Klasa bazowa wszystkich gadżetów ekranowych. }
/// @details ...
class gadget:public drawable_base
//-------------------------------------
{
protected:
    wb_color draw_color; //!< Główny kolor. Pozostałe dziedziczone z `drawable_base`.

public:
    gadget(	int ix1,int iy1,int ix2,int iy2,		//!< Położenie gadżetu.
            wb_color icolor,						//!< Kolor znaczących elementów.
            wb_color ibkg=default_white,            //!< Kolor tła.
            wb_color ifr=default_white              //!< Kolor ramki.
            ):
            drawable_base(ix1,iy1,ix2,iy2,ibkg,ifr),draw_color(icolor)
                {}

    /// WYMAGANE: Rysuje treść graficzną gadgetu (np. X). Do reimplementacji w klasach potomnych.
    void _replot() override =0;

    /// Obsługa reakcji na klikniecie.
    /// Bazowa wersja sprawdza, czy klik "is_inside" i rysuje za pomocą `replot`, ewentualnie w inwersji.
    /// @returns wynik działania funkcji `is_inside`.
    int  on_click(int x,int y,int click=0) override;

    // TODO Poniżej chyba pozostałość po starym projekcie?

    /// Wewnętrzna akcja klasy potomnej jest w środku tej metody wywoływanej w domyślnym `on_click`.
    /// @returns 1, jeśli "nie obsłużono" albo "2", jeśli obsłużono.
    ///          Domyślna wersja zwraca 1 == "nie obsłużono".
    virtual int _on_click(int /*x*/,int /*y*/,int /*click*/)
                {return 1;}

};

/// @brief @EN{ . }
///        @PL{  Klasa bazowa dla gadżetów reagujących na zdarzenia użytkownika. }
/// @details ...
class sensitive_area:public gadget
//--------------------------------------------
{
public:
    sensitive_area( int ix1,int iy1,int ix2,int iy2, //!< Położenie gadżetu.
                    unsigned icolor,                 //!< Kolor znaczących elementów.
                    unsigned ibkg=default_white,     //!< Kolor tła.
                    unsigned ifr=default_transparent //!< Kolor ramki.
                    ):
            gadget(ix1,iy1,ix2,iy2,icolor,ibkg,ifr)
    {}

    void _replot() override; // Rysuje gadżet — np. X. Do reimplementacji w klasach potomnych

    // TODO Poniżej chyba pozostałość po starym projekcie?
    //int _on_click(int /*x*/,int /*y*/,int /*click*/)	//Prawdziwa akcja klasy potomnej powinna być w środku tej metody
    //			{return 2;}							  //i zwracać 2, jeśli "obsłużono".
};

typedef sensitive_area empty_area;

/// @brief @EN{ . }
///        @PL{ Klasa gadżetu implementująca przycisk. }
/// @details ...
/// Dwa możliwe tryby wyświetlania:
/// 0 -> vertical print mode
/// 1 -> horizontal print mode
class button:public sensitive_area
//---------------------------------------------
{
protected:
    unsigned vh_mode:1;		//!< Tryb wyświetlania: vert(0) vs. hor(1).
    unsigned reserved:15;	//!< Co najmniej 15 bitów i tak będzie zajęte.

public:
    button(int ix1,int iy1,int ix2,int iy2,	//!< Położenie gadżetu.
            const char* i_prompt,			//!< Tekst na przycisku.
            unsigned i_mode=1,               //!< Tryb wyświetlania.
            unsigned i_color=default_black,  //!< Kolor znaczących elementów.
            unsigned i_background=default_white, //!< Kolor tła strzałek/tarczy.
            unsigned i_frame=128             //!< Kolor ramki.
            ):
            sensitive_area(ix1, iy1, ix2, iy2, i_color, i_background, i_frame), vh_mode(i_mode), reserved(0)
        {
            set_title(i_prompt); //Prompt jest pamiętany jako tytuł okna, ale wyświetlany ręcznie.
            set_title_color(default_transparent); // Wiec domyślnie ten tytuł...
            set_title_back(default_transparent);  // ...jest niewidoczny (bo by się wyświetlał nie tak i nie tam).
        }

    void _replot() override;  // Odrysowuje. W tym ewentualne składowe.

    // TODO Poniżej chyba pozostałość po starym projekcie?
    //int _on_click(int /*x*/,int /*y*/,int /*click*/)	//Prawdziwa akcja klasy potomnej powinna być w środku tej metody
    //			{return 2;}							  //i zwracać 2, jeśli "obsłużono".
};

/// @brief @EN{ . }
///        @PL{ Klasa gadżetu implementująca strzałkę w jednym z czterech kierunków lub tarcze (jako 3 wymiar albo coś). }
/// @details ...
/// Znaczenie `mode` lub parametru `imode` konstruktora:
/// 0 — print target (-(+)-)
/// 1 — print up arrow
/// 2 — print left arrow
/// 3 — print down arrow
/// 4 — print right arrow
class arrow_button:public sensitive_area
//---------------------------------------------
{
protected:
    int mode; //!< Tryb wyświetlania, czyli kierunek strzałki albo tarcza.

public:
    arrow_button(int ix1,int iy1,int ix2,int iy2,	//!< Położenie gadżetu.
            int i_mode=0,							//!< Inicjalizacja trybu wyświetlania (patrz na opis klasy).
            unsigned i_color=default_black,			//!< Kolor znaczących elementów.
            unsigned i_background=default_white,	//!< Kolor tła strzałek/tarczy.
            unsigned i_frame=default_transparent	//!< Kolor ramki.
            ):
        sensitive_area(ix1, iy1, ix2, iy2, i_color, i_background, i_frame),
        mode(i_mode)
        {}

    void _replot() override; // Odrysowuje składowe.

};

/// @brief @EN{ . }
///        @PL{ Klasa gadżetu łącząca strzałkę w lewo i strzałkę w prawo. }
/// @details ...
/// @note To jest klasa bazowa dla klas reimplementujących `_user_action`.
class left_right_button: public sensitive_area
//==============================================
{
protected:
    wb_ptr<drawable_base> left;  //!< Podobiekt (strzałka) ruchu w lewo.
    wb_ptr<drawable_base> right; //!< Podobiekt (strzałka) ruchu w prawo.
public:
    /// Konstruktor umieszczający gadżet w konkretnym miejscu.
    left_right_button(int ix1, int iy1, int ix2, int iy2);

    void _replot() override; // Odrysowuje składowe.

    /// Implementacja reakcji na zmianę rozmiarów lub przesunięcie.
    /// Musi przesunąć współrzędne składowych razem ze swoimi.
    int on_change(const gps_area&) override;

    /// Akcja do wykonania w `on_click`. W tej klasie nic nie robi, choć sygnalizuje obsłużenie.
    virtual int _user_action( int left_or_right, //!< Jeśli lewo to -1, a jeśli prawo to 1
                              int click          //!< Może mieć znaczenie, który przycisk myszki.
                              );

    /// Implementacja reakcji na kliknięcie.
    /// Przepytuje składowe i jeśli któraś została trafiona
    /// to wywołuje `_user_action` z odpowiednim parametrem.
    int on_click(int x,int y,int click=0) override;

};

/// @brief @EN{ . }
///        @PL{ Klasa gadżetu do zmiany pola widzenia na prostokątną serię danych. }
/// @details ...
/// Pozwala zmieniać wielkość okna i przesuwać to okno w czterech kierunkach.
/// Tło gadżetu domyślnie jest takie, jakie dla całego okna!
class steering_wheel:public sensitive_area
//----------------------------------------
{
    /// Wskaźniki do podpiętych danych.
    /// @note Pamięć żadnej z seri danych nie jest tu zarządzana!
    wb_dynarray<rectangle_source_base*> data;

    //Zarządzane wskaźniki do elementów sterowania
    wb_ptr<drawable_base> resizing; //!< Podobiekt zwiększania/zmniejszania pola widzenia.
    wb_ptr<drawable_base> left;		//!< Podobiekt (strzałka) ruchu w lewo.
    wb_ptr<drawable_base> up;		//!< Podobiekt (strzałka) ruchu w górę.
    wb_ptr<drawable_base> right;	//!< Podobiekt (strzałka) ruchu w prawo.
    wb_ptr<drawable_base> down;		//!< Podobiekt (strzałka) ruchu w dół.

public:
    /// Konstruktor pobierający składowe gadżetu i pobierający jeden adres serii danych.
    steering_wheel( rectangle_source_base*     i_dat, //!< Dane. Pamięć seri nigdy nie jest tu zarządzana.
                    wb_ptr<drawable_base> i_res,      //Pamięć dla pod-obszarów jest
                    wb_ptr<drawable_base> i_up,       // z a w s z e
                    wb_ptr<drawable_base> i_left,     //zarządzana.
                    wb_ptr<drawable_base> i_down,     //Współrzędne obszaru steering wheel są
                    wb_ptr<drawable_base> i_right     //ustalane ze współrzędnych jego składowych.
                    );

    /// Konstruktor pobierający składowe gadżetu i pobierający adresy danych z tablicy.
    steering_wheel( wb_dynarray<rectangle_source_base*>&  i_dat, //!< Dane. Pamięć seri nigdy nie jest tu zarządzana.
                    wb_ptr<drawable_base> i_res,     //Pamięć dla pod-obszarów jest
                    wb_ptr<drawable_base> i_up,      // z a w s z e
                    wb_ptr<drawable_base> i_left,    //zarządzana.
                    wb_ptr<drawable_base> i_down,    //Współrzędne obszaru steering wheel są
                    wb_ptr<drawable_base> i_right    //ustalane ze współrzędnych jego składowych.
                    );

    /// Konstruktor kładący gadżet w konkretnym miejscu i pobierający jeden adres danych.
    steering_wheel( int ix1,int iy1,int ix2,int iy2,  //!< Położenie gadżetu.
                    rectangle_source_base*  i_dat     //!< Dane. Pamięć seri nigdy nie jest tu zarządzana.
                    )
    : sensitive_area(ix1,iy1,ix2,iy2,128),data(1)
    { data[0]=i_dat; assert("Użyto nietestowanego konstruktora" == NULL);  }

    /// Konstruktor kładący gadżet w konkretnym miejscu i pobierający adresy danych z tablicy.
    steering_wheel( int ix1,int iy1,int ix2,int iy2, //!< Położenie gadżetu.
                    wb_dynarray<rectangle_source_base*>&  idat  //!< Dane. Pamięć seri nigdy nie jest tu zarządzana.
                    );

    void _replot() override; // Odrysowuje składowe.

    /// Implementacja reakcji na zmianę rozmiarów lub przesunięcie.
    /// Musi przesunąć współrzędne składowych razem ze swoimi.
    int on_change(const gps_area&) override;

    /// Implementacja reakcji na kliknięcie.
    /// Przepytuje składowe i jeśli któraś została trafiona to
    /// adekwatnie zmienia serie za pomocą metody sub()
    /// oraz wymusza odnowienie ekranu
    int on_click(int x,int y,int click=0) override;

};

/// @brief @EN{ . }
///        @PL{ Szablon klasy gadżetu do zmiany wartości zmiennej liczbowej dowolnego typu. }
/// @details ...
/// ...
template<class NUMBER>
class knob_for_value:public left_right_button
// ----------------------------------------
{
protected:
    NUMBER  min,max,step; //!< Parametry zmiany podpiętej zmiennej.
    NUMBER*      val_ptr; //!< Adres podpiętej zmiennej.

public:
    /// Konstruktor.
    knob_for_value( int ix1,int iy1,int ix2,int iy2,	//!< Położenie gadżetu.
                    const char* Title="knob",			//!< Tytuł gadżetu (rzadko potrzebny).
                    NUMBER* val_ptr=NULL,				//!< Adres zmiennej, która ma być modyfikowana przez gadżet.
                    NUMBER min=0,						//!< Dolne ograniczenie zmiany.
                    NUMBER max=100,						//!< Górne ograniczenie zmiany.
                    double i_prop=0.01					//!< Krok zmiany.
                    ):
            left_right_button(ix1, iy1, ix2, iy2)
    {										assert(min<max);
        set_title(Title);
        this->val_ptr=val_ptr;
        this->max=max;
        this->min=min;
        set_step_proportionally(i_prop);
    }

    /// Akcja używana w `on_click`.
    int _user_action( int left_or_right, //!< Jeśli lewo to -1, a jeśli prawo to 1
                      int click          //!< Może mieć znaczenie, który przycisk myszki.
                    ) override
    {
        if(val_ptr == nullptr)
            return 0;

        if(left_or_right > 0)
        {
            if(click == 1)
                *val_ptr += step;
            else if(click == 2)
                *val_ptr += step / 10;

            if(*val_ptr > max)
                *val_ptr = max;
        }
        else if(left_or_right < 0)
        {
            if(click == 1)
                *val_ptr -= step;
            else if(click == 2)
                *val_ptr -= step / 10;

            if(*val_ptr < min) *val_ptr = min;
        }

        return 2; //Nie 1!!!
    }

    /// Dostęp do adresu modyfikowanej zmiennej.
    /// @note Po co to?
    NUMBER* get_ptr() { return val_ptr;}

    /// Zmiana adresu modyfikowanej zmiennej.
    /// @note Po co to?
    virtual void change_ptr(NUMBER* i_val_ptr)
    {
        val_ptr=i_val_ptr;
        if(*val_ptr < min || max < *val_ptr)
            *val_ptr= (min + max) / 2;
    }

    /// Zmiana kroku modyfikacji zmiennej.
    virtual void set_step(NUMBER i_step)
    {
        if((max-min) < i_step)
            step=i_step;
    }

    /// Zmiana kroku modyfikacji zmiennej w proporcji (0..0.5).
    virtual void set_step_proportionally(double proportion)
    {
        if(0<proportion && proportion<=0.5) //Co najmniej 2 kroki!!!
            step=(max-min)*proportion;
    }
};

} // namespace sym2

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
#endif


