/// @file
/// @brief KLASY ZARZĄDCÓW OBSZARÓW EKRANU
/// @date 2026-05-13 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_AREA_MNGR_HPP_INCLUDED_
#define SYMSHELL2_AREA_MNGR_HPP_INCLUDED_

#include "drawable.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

/// Zmodernizowane klasy do symulacji w C++
namespace symshell2
{

    /// INTERFACE DO DOWOLNEGO ZARZĄDCY OBSZARU.
    /// Dobrze, żeby można było zestawiać zarządców w hierarchie.
    class area_manager_base : public drawable_base
    //------------------------------------------
    {
        int cont_actions;    ///< Flaga kontynuacji. Gdy 0 to wypada z pętli, kiedy może.

    protected:
        /// Konieczny jakikolwiek konstruktor do przekazania parametrów `drawable_base` i żeby był default.
        explicit area_manager_base(
                int ix1 = 0, int iy1 = 0, int ix2 = 0, int iy2 = 0,
                unsigned ibkg = default_color,
                unsigned ifr = default_color)
        : drawable_base(ix1, iy1, ix2, iy2, ibkg, ifr),cont_actions(1)
        {}

        /// Pusty destruktor, bo wymuszenie wirtualności destruktorów w klasie bazowej.
        ~area_manager_base() override = default;

    public:
        // Sterowanie przerywaniem działania zarządcy
        //*///////////////////////////////////////////

        /// Ustawianie wymagania zakończenia.
        virtual void need_break_action(int Yes = 1)
        { cont_actions = !Yes; }

        /// Czy user chciał przerwać?
        int should_continue() const
        { return cont_actions; }

        /// @name	AKCESORY OGÓLNE
        //-------------------------
        /// @{

        /// WYMAGANE: Dodaje obszar do listy. Zwraca pozycje albo -1(błąd).
        virtual int insert(wb_ptr<drawable_base> drw) = 0;

        /// Zabiera w zarząd zwykły wskaźnik.
        virtual int insert(drawable_base *drw)
        {
            wb_ptr<drawable_base> temp(drw);
            //Inteligentny wskaźnik tymczasowy od razu przekazuje "w głąb"
            return insert(temp);
        }

        /// WYMAGANE: Wymienia obszar na liście. Jak nie znajdzie, to zwraca -1.
        virtual int replace(const char *nam, wb_ptr<drawable_base> drw) = 0;

        /// WYMAGANE: Wymienia obszar na liście. Jak błędne parametry to zwraca -1.
        virtual int replace(size_t index, wb_ptr<drawable_base> drw) = 0;

        /// Usuwa obszar z listy.
        virtual int remove(size_t index)
        {
            wb_ptr<drawable_base> Empty;
            return replace(size_t(index), Empty);
        }

        /// WYMAGANE: Odnajduje obszar na liście. @returns index albo -1 jak nie znajdzie.
        virtual int search(const char *nam) = 0;

        /// WYMAGANE: Podaje po prostu aktualny rozmiar listy łącznie z pozycjami pustymi.
        virtual size_t get_size() = 0;

        // AKCESORY poszczególnych obszarów:
        //==================================

        /// WYMAGANE: Dostęp do obszaru z możliwością modyfikacji.
        /// Trzeba pamiętać, że pewne informacje są zapisywane w zarządcy w związku z pozycją!
        /// @note NIE WOLNO ZROBIĆ `delete`, chyba że obszar nie jest zarządzany.
        virtual wb_ptr<drawable_base> &get(size_t index) = 0;

        /// WYMAGANE: Dostęp do obszaru bez możliwości modyfikacji.
        virtual drawable_base /*const*/*get_ptr(size_t index) = 0;

        ///@}

        /// @name	REAKCJE NA ZDARZENIA
        //-----------------------------
        /// @{

        /// WYMAGANE: Przepytuje obszary z reakcji na punkt.
        /// \note   Jeśli znajdzie (zwróci 1), to można ustalić, wywołując get_last_lazy_area()
        int on_click(int x, int y, int click) override = 0;

        /// WYMAGANE: Który obszar wymaga odświeżenia lub innej uwagi.
        /// \note  Jeśli on_click() zwraca 1, to można się dowiedzieć, który obszar znalazł, wywołując właśnie to.
        /// \return  -1, jeśli już ten obszar był wzięty, lub powstał jakiś inny błąd.
        virtual int get_last_lazy_area() = 0;

        /// Akcja, gdy kliknięto tło zarządcy.
        virtual int on_margin_click(int x, int y, int click)
        { return 2; }

        /// WYMAGANE: Przepytuje obszary, czy chcą znak z wejścia (... zwykle okna graficznego).
        int on_input(int input_char) override = 0;

        /// WYMAGANE: Reaguje na zmianę rozmiarów lub położenia własnego obszaru.
        int on_change(const gps_area &ar) override = 0;

        /// WYMAGANE: Odrysowuje wszystkie (widoczne) obszary.
        void _replot() override = 0;

        /// WYMAGANE: Odrysowuje obszary "nadepnięte" przez "ar" (???).
        virtual void replot(const gps_area &ar) = 0;

        /// @}

        /// @name MANIPULATORY NA OBSZARACH LUB ICH GRUPACH
        //-------------------------------------------------
        /// @param index to zawsze pozycja obszaru na liście zarządcy.
        /// @param lst to lista pośrednia — indeksów obszarów  na liście zarządcy.
        /// @{
        //...DLA POJEDYNCZYCH OBSZARÓW
        //----------------------------

        /// WYMAGANE: Odrysowuje obszar, jeśli nie jest zminimalizowany.
        virtual int refresh(size_t index) = 0;

        /// WYMAGANE: Zaznacza obszar.
        virtual int mark(size_t index, wb_color frame = default_color) = 0;

        /// WYMAGANE: Odznacza obszar.
        virtual int unmark(size_t index) = 0;

        /// WYMAGANE: Informuje, czy obszar jest zminimalizowany.
        virtual int is_minimized(size_t index) = 0;

        /// WYMAGANE: Informuje, czy jest zaznaczony.
        virtual int is_marked(size_t index) = 0;

        /// WYMAGANE: Ustala obszar jako pierwszy do wejścia z klawiatury lub zdarzeń menu.
        virtual int set_input(size_t index) = 0;

        /// WYMAGANE: Oddaje pod-obszarowi cały zarządzany obszar.
        virtual int maximize(size_t index) = 0;

        /// WYMAGANE: Podaje `index` zmaksymalizowanego okna lub -1.
        virtual int get_maximized() = 0;

        /// WYMAGANE: Ukrywa obszar.
        virtual int minimize(size_t index) = 0;

        /// WYMAGANE: Odtwarza poprzednie położenie i rozmiar obszaru.
        virtual int restore(size_t index) = 0;

        /// WYMAGANE: Odtwarza pierwotne  położenie i rozmiar obszaru.
        virtual int original(size_t index) = 0;

        /// WYMAGANE: Uznaje aktualne położenie obszaru za oryginalne (czyli to które będzie używane przez `original`).
        virtual int as_original(size_t index) = 0;


        //...DLA GRUP OBSZARÓW
        //--------------------

        /// WYMAGANE: Zaznacza wszystkie widoczne OBSZARY. Można zmienić kolor zaznaczenia ramki (?).
        virtual int mark_all(wb_color frame = default_color) = 0;

        /// WYMAGANE: Zwraca listę zaznaczonych obszarów.
        /// Filtruje po kolorach ramek. Jeśli `what == default color` to wszystkie zaznaczone...
        /// I opcjonalnie zdejmuje zaznaczenie (`unm == 1`).
        virtual wb_dynarray<int> get_marked(wb_color filtr = default_color, int unm = 0) = 0;

        /// WYMAGANE: Ukrywa (`minimize`) obszary z listy `lst`.
        virtual int minimize(const wb_dynarray<int> &lst) = 0;

        /// WYMAGANE: Robi `restore` dla obszarów z listy `lst`.
        virtual int restore(const wb_dynarray<int> &lst) = 0;

        /// WYMAGANE: Robi `restore` dla wszystkich obszarów.
        virtual int restore(/*ALL*/) = 0;

        /// Robi `original` dla obszarów z listy `lst`.
        virtual int original(const wb_dynarray<int> &lst) = 0;

        /// WYMAGANE: Rearanżuje obszary z listy `lst` brutalnie, czyli na równe kafelki. @return -1 jak nie da się.
        virtual int tile(const wb_dynarray<int> &lst) = 0;

        /// WYMAGANE: Inteligentnie rearanżuje obszary z listy `lst`. @return -1 jak nie da się.
        virtual int arrange(const wb_dynarray<int> &lst) = 0;

        /// @}
    };


/// Klasa najprostszego, nieagresywnego, zarządcy obszarów.
/// @details
/// Implementuje, co się uda bez wiedzy o niskopoziomowej podstawie soft-hard.
/// Zakłada pełna władze nad obszarami, a w szczególności nad ich pamięcią.
/// Zdarzenia zewnętrzne trzeba przekazać "explicite" -
///  - taki zarządca nie zawłaszcza ich samodzielnie, a tym bardziej nie zabiera wątku sterowania.
    class area_manager : public area_manager_base
    {
    protected:
        /// Wewnętrzna struktura przechowywania informacji o obszarach.
        struct internal
        {
            wb_ptr<drawable_base> ptr; //!< Wskaźnik do obszaru.
            gps_area         orig_pos; //!< Parametry obszaru przy wstawieniu.
            gps_area            saved; //!< Parametry w wersji średniowymiarowej.
            wb_color        org_frame; //!< Oryginalny kolor ramki, gdy markowany.
            bool       mark: 1; //!< Flaga zamarkowania obszaru.
            bool  minimized: 1; //!< Flaga zminimalizowania obszaru.
            bool    locking;    //!< Nie wolno usunąć poza destruktorem zarządcy.

            /// Konstruktor.
            internal()
            : org_frame(default_color), mark(false), minimized(false),locking(false)
            {}
        };

        wb_dynarray<internal> tab; //!< Tablica obszarów.

        /// @name Indeksy obszarów o aktualnie specjalnym znaczeniu.
        /// @{
        int maximized;  //!< Obszar "zasłaniający" wszystko.
        int   grabbed;  //!< Obszar w pierwszym rzędzie obsługujący wejście.
        int      lazy;  //!< Obszar, który ostatnio NIE obsłużył myszy.
        /// @}
    public:
        /// Konstruktor dający zarządcę konkretnego obszaru o określonym rozmiarze listy.
        /// \param size to właśnie rozmiar listy.
        /// \param ix1, iy1 to lewy górny róg obszaru.
        /// \param ix2, iy2 to prawy dolny róg obszaru.
        /// \param i_bkg to kolor tła, który może być domyślny, czyli przezroczysty.
        /// \param i_frm to kolor ramki, który może być domyślny, co oznacza brak widocznej ramki.
        area_manager(size_t size,
                     int ix1, int iy1, int ix2, int iy2,
                     unsigned i_bkg = default_color,
                     unsigned i_frm = default_color);

        /// Wirtualny destruktor.
        ~area_manager() override;

        //	AKCESORY OGÓLNE
        //------------------

        /// Podaje po prostu aktualny rozmiar listy łącznie z pozycjami pustymi.
        size_t get_size() override;

        /// Zabiera obszar w zarząd!
        int insert(drawable_base *drw) override
        {
            wb_ptr<drawable_base> H(drw);
            return insert(H);
        }

        int insert(wb_ptr<drawable_base> drw) override; //Dodaje obszar do listy. Zwraca pozycje albo -1(błąd)
        int replace(const char *nam, wb_ptr<drawable_base> drw) override; //Wymienia na liście. Jak nie znajdzie, to zwraca -1.
        int replace(size_t index, wb_ptr<drawable_base> drw) override; //Wymienia na liście. Jak błędne parametry, to zwraca -1.
        int search(const char *nam) override;    //Odnajduje na liście. Zwraca -----//----

        // AKCESORY poszczególnych obszarów

        /// Pobranie obszaru z możliwością modyfikacji.
        /// Jednak trzeba pamiętać, że pewne informacje są zapisywane w zarządcy w związku z pozycją.
        wb_ptr<drawable_base> &get(size_t index) override;

        /// Pobranie obszaru bez możliwości modyfikacji wskaźnika a tym bardziej zwolnienia!
        drawable_base /*const*/*get_ptr(size_t index) override; //

        //	REAKCJE NA ZDARZENIA
        //--------------------------
        int on_click(int x, int y, int click) override; //Przepytuje obszary z reakcji na punkt.
        //Jeśli on_click() zwraca 1 to można się dowiedzieć, który obszar wywołując:
        int get_last_lazy_area() override; //Zwróci -1, jeśli już raz wzięte, lub inny błąd.
        int on_input(int input_char) override; //Przepytuje obszary czy chcą ten znak.
        int on_change(const gps_area &ar) override; //Reaguje na zmianę rozmiarów lub położenia własnego obszaru.
        void replot(int flus = 1) override
        { drawable_base::replot(flus); } //Odrysuj wszystko. NIE WIEM PO CO TA PSEUDO-REIMPLEMENTACJA.
        void replot(const gps_area &ar) override; //Odrysowuje obszary "nadepnięte" przez "ar"
        void _replot() override;    //Odrysowuje wszystkie (widoczne) obszary

        //  MANIPULATORY
        //----------------
        int mark(size_t index, wb_color frame = default_color) override; //Zaznacza obszar
        int mark_all(wb_color frame = default_color) override; //Zaznacza wszystkie widoczne
        int unmark(size_t index) override;    //i odznacza obszar
        int is_marked(size_t index) override; //Informuje, czy jest zaznaczony
        int is_minimized(size_t index) override; //Informuje, czy jest zminimalizowany
        wb_dynarray<int> get_marked(wb_color filtr = default_color, int unm = 0) override; //Zwraca listę zaznaczonych obszarów.
        // Jeśli `what == default color` to wszystkie zaznaczone i opcjonalnie zdejmuje zaznaczenie.

        //...DLA POJEDYNCZYCH OBSZARÓW
        int get_maximized() override
        { return maximized; }; //Zwraca index zmaksymalizowanego okna lub -1


        int set_input(size_t index) override; //Ustala obszar jako pierwszy do wejścia z klawiatury lub zdarzeń menu
        int maximize(size_t index) override; //Oddaje pod-obszarowi cały zarządzany obszar
        int minimize(size_t index) override; //Ukrywa pod-obszar
        int restore(size_t index) override; //Odtwarza poprzednie położenie i rozmiar obszaru
        int original(size_t index) override; //Odtwarza pierwotne  położenie i rozmiar obszaru
        int as_original(size_t index) override; //Uznaje aktualne położenie obszaru za oryginalne

        //...DLA GRUP OBSZARÓW
        int refresh(size_t index) override; //Odrysowuje obszar, jeśli nie zminimalizowany
        int minimize(const wb_dynarray<int> &lst) override; //Ukrywa pod-obszary
        int restore(const wb_dynarray<int> &lst) override; //Robi restore dla pod-obszarów
        int restore(/*ALL*/) override; //Robi restore dla wszystkich pod-obszarów
        int original(const wb_dynarray<int> &lst) override; //Robi original dla pod-obszarów
        int tile(const wb_dynarray<int> &lst) override;    //Rearanżuje na chama, czyli po równo, albo -1 jak nie da się
        int arrange(const wb_dynarray<int> &lst) override; //Inteligentnie rearanżuje.
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
#endif //SYMSHELL2_AREA_MNGR_HPP_INCLUDED_




