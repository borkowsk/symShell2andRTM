/// @file
/// @brief KLASY ZARZĄDCÓW OBSZARÓW EKRANU
/// @date 2026-04-28 (modified)
// ********************************************************************************************************************
//
#ifndef __AREAMNGR_HPP__
#define __AREAMNGR_HPP__
#ifndef __cplusplus
#error C++ required
#endif


// Niezbędne definicje bazowe
//------------------------------------------
#include "drawable.hpp"

/// INTERFACE DO DOWOLNEGO ZARZĄDCY OBSZARU.
/// Dobrze, żeby można było zestawiać zarządców w hierarchie.
class area_menager_base:public drawable_base
//------------------------------------------
{
    int		cont_actions;	///< Flaga kontynuacji. Gdy 0 to wypada z pętli, kiedy może.

    public:
    /// Pusty destruktor, bo wymuszenie wirtualności destruktorów w klasie bazowej.
    ~area_menager_base() override= default;

    /// Konieczny jakikolwiek konstruktor do przekazania parametrów `drawable_base` i żeby był default.
    explicit area_menager_base(
                    int ix1=0,int iy1=0,int ix2=0,int iy2=0,
                    unsigned ibkg=default_color,
                    unsigned ifr=default_color):
                    drawable_base(ix1,iy1,ix2,iy2,ibkg,ifr),
                    cont_actions(1)
                        {}

    // Sterowanie przerywaniem dziaŁania menagera
    //*///////////////////////////////////////////

    /// Ustawianie wymagania zakończenia.
    virtual void need_break_action(int Yes=1){ cont_actions=!Yes;}

    /// Czy user chciał przerwać?
    int          should_continue() {return cont_actions;}

    /// @name	AKCESORY OGÓLNE
    //-------------------------
    /// @{

    /// Dodaje obszar do listy. Zwraca pozycje albo -1(blad).
    virtual int    insert(wb_ptr<drawable_base>	drw)=0;

    /// Zabiera w zarzad zwykły wskaźnik.
    virtual int    insert(drawable_base* drw)
    {
        wb_ptr<drawable_base> temp(drw);
        //Inteligentny wskaznik tymczasowy od razu przekazuje "w głąb"
        return insert(temp);
    }

    /// Wymienia obszar na liscie. Jak nie znajdzie to zwraca -1.
    virtual int    replace(const char* nam, wb_ptr<drawable_base> drw)=0;

    /// Wymienia obszar na liscie. Jak bledne parametry to zwraca -1.
    virtual int    replace(size_t    index, wb_ptr<drawable_base> drw)=0;

    /// Usuwa obszar z listy.
    virtual int    remove(size_t index)
    {
            wb_ptr<drawable_base> Empty;
            return replace(size_t(index),Empty);
    }

    /// Odnajduje obszar na liscie. @returns index albo -1 jak nie znajdzie.
    virtual int    search(const char* nam)=0;

    /// Podaje po prostu aktualny rozmiar listy lacznie z pozycjami pustymi.
    virtual size_t get_size()=0;

    // AKCESORY poszczegolnych obszarow:

    /// Dostęp z mozliwoscią modyfikacji.
    /// Trzeba pamietac ze pewne informacje sa zapisywane w zarzadcy w zwiazku z pozycja!
    /// @note NIE WOLNO ZROBIĆ `delete`, chyba że obszar nie jest zarządzany.
    virtual wb_ptr<drawable_base>& get(size_t index)=0;

    /// Bez mozliwosci modyfikacji.
    virtual drawable_base /*const*/* get_ptr(size_t index)=0;

    ///@}

    /// @name	REAKCJE NA ZDAZENIA
    //-----------------------------
    /// @{

    /// Przepytuje obszary z reakcji na punkt.
    /// \note   Jeśli znajdzie (zwroci 1) to można ustalić wywołując get_last_lazy_area()
    virtual int    on_click(int x,int y,int click)=0;

    /// Który obszar wymaga odświeżenia lub innej uwagi.
    /// \note  Jeśli on_click() zwraca 1 to można sie dowiedzieć, który obszar znalazł wywołując właśnie to.
    /// \return  -1 jeśli już ten obszar był wzięty, lub powstał jakiś inny błąd.
    virtual int    get_last_lazy_area()=0;

    /// Akcja, gdy kliknięto w tlo menagera.
    virtual int    on_margin_click(int x,int y,int click) {return 2;}

    /// Przepytuje obszary czy chcą znak z wejścia (... zwykle okna graficznego).
    virtual int    on_input(int input_char)=0;

    /// Reaguje na zmianę rozmiarów lub położenia własnego obszaru.
    virtual int    on_change(const gps_area& ar)=0;

    /// Odrysowuje obszary "nadepnięte" przez "ar" (???).
    virtual void   replot(const gps_area& ar)=0;

    /// Odrysowuje wszystkie (widoczne) obszary.
    virtual void   _replot()=0;

    /// @}

    /// @name MANIPULATORY NA OBSZARACH LUB ICH GRUPACH
    //-------------------------------------------------
    /// @param index to zawsze pozycja obszaru na liście menagera.
    /// @param lst to lista pośrednia - indeksów obszarów  na liście menagera.
    /// @{
    //...DLA POJEDYNCZYCH OBSZAROW
    //----------------------------

    /// Odrysowuje obszar jeśli nie jest zminimalizowany.
    virtual int    refresh(size_t index)=0;

    /// Zaznacza obszar.
    virtual int    mark(size_t index,wb_color frame=default_color)=0;

    /// Odznacza obszar.
    virtual int    unmark(size_t index)=0;

    /// Informuje czy obszar jest zminimalizowany.
    virtual int    is_minimized(size_t index)=0;

    /// Informuje czy jest zaznaczony.
    virtual int    is_marked(size_t index)=0;

    /// Ustala obszar jako pierwszy do wejscia z klawiatury lub zdarzen menu.
    virtual int    set_input(size_t index)=0;

    /// Oddaje podobszarowi caly zarzadzany obszar.
    virtual int    maximize(size_t index)=0;

    /// Podaje `index` zmaksymalizowanego okna lub -1.
    virtual int    get_maximized()=0;

    /// Ukrywa obszar.
    virtual int    minimize(size_t index)=0;

    /// Odtwarza poprzednie polozenie i rozmiar obszaru.
    virtual int    restore(size_t  index)=0;

    /// Odtwarza pierwotne  połozenie i rozmiar obszaru.
    virtual int    orginal(size_t  index)=0;

    /// Uznaje aktualne polozenie obszaru za orginalne (czyli to które będzie uzywane przez `oryginal`).
    virtual int    as_orginal(size_t index)=0;


    //...DLA GRUP OBSZAROW
    //--------------------

    /// Zaznacza wszystkie widoczne OBSZARY. Można zmienić kolor zanzaczenia ramki (?).
    virtual int    mark_all(wb_color frame=default_color)=0;

    /// Zwraca listę zaznaczonych obszarów.
    /// Filtruje po kolorach ramek. Jesli `what=default color` to wszystkie zaznaczone...
    /// I opcjonalnie zdejmuje zaznaczenie (`unm=1`).
    virtual wb_dynarray<int> get_marked(wb_color filtr=default_color,int unm=0)=0;

    /// Ukrywa (`minimize`) obszary z listy `lst`.
    virtual int    minimize(const wb_dynarray<int>& lst)=0;

    /// Robi `restore` dla obszarow z listy `lst`.
    virtual int    restore(const wb_dynarray<int>& lst)=0;

    /// Robi `restore` dla wszystkich obszarow.
    virtual int    restore(/*ALL*/)=0;

    /// Robi `orginal` dla obszarow z listy `lst`.
    virtual int    orginal(const wb_dynarray<int>& lst)=0;

    /// Rearanzuje obszary z listy `lst` brutalnie, czyli na rowne kafelki. @return -1 jak nie da sie.
    virtual int    tile(const wb_dynarray<int>& lst)=0;

    /// Inteligentnie rearanzuje obszary z listy `lst`. @return -1 jak nie da sie.
    virtual int    arrange(const wb_dynarray<int>& lst)=0;

    /// @}
};



/// Klasa najprostrzego, nieagresywnego, zarzadcy obszarow.
/// @details
/// Zaklada pelna wladze nad obszarami, a w szczegolnosci nad ich pamiecia.
/// Zdarzenia zewnetrzne trzeba przekazac explicite -
///  - zarządca nie zawlaszcza ich samodzielnie, a tym bardziej nie
/// zabiera watku sterowania.
class area_menager:public area_menager_base
{
protected:
    /// Wewnętrzna struktura przechowywania informacji o obszarach.
    struct internal
    {
        wb_ptr<drawable_base> ptr; //!< wskaznik do obszaru.
        gps_area          orginal; //!< parametry obszaru przy wstawieniu.
        gps_area            saved; //!< i w wersji sredniowymiarowej.
        wb_color        org_frame; //!< orginalny kolor ramki gdy markowany.
        int                mark:1; //!< flaga zamarkowania obszaru.
        int			  minimized:1; //!< flaga zminimalizowania obszaru.
        int				  locking; //!< Nie wolno usunac poza destruktorem menagera.
        /// Konstruktor.
        internal():
            mark(0),
            minimized(0),
            locking(0),
            org_frame(default_color){}
    };

    wb_dynarray<internal> tab; //!< tablica obszarow.

    int maximized;  //!< obszar "zasłaniający" wszystko.
    int   grabbed;  //!< obszar w pierwszym rzedzie obslugujacy wejscie.
    int      lazy;  //!< obszar ktory ostatnio NIE obsluzyl myszy.

public:
        /// Wirtualny destruktor.
        ~area_menager() override;

        /// Konstruktor dajacy zarzadce o okreslonym rozmiarze listy.
        area_menager(size_t size,
                    int ix1,int iy1,int ix2,int iy2,
                    unsigned ibkg=default_color,
                    unsigned ifr=default_color);

    //	AKCESORY OGOLNE
    //------------------
     size_t get_size();		//Podaje po prostu aktualny rozmiar listy lacznie z pozycjami pustymi
     int    insert(drawable_base*	drw){ wb_ptr<drawable_base> H(drw);return insert(H);} //Zabiera w zarzad!
     int    insert(wb_ptr<drawable_base>	drw); //Dodaje obszar do listy. Zwraca pozycje albo -1(blad)
     int    replace(const char* nam,wb_ptr<drawable_base> drw); //Wymienia na liscie. Jak nie znajdzie to zwraca -1.
     int    replace(size_t    index,wb_ptr<drawable_base> drw); //Wymienia na liscie. Jak bledne parametry to zwraca -1.
     int    search(const char* nam);	//Odnajduje na liscie. Zwraca -----//----

     // AKCESORY poszczegolnych obszarow
     wb_ptr<drawable_base>&  get(size_t index); //Mozliwosci modyfikacji, ale trzeba pamietac
         //ze pewne informacje sa zapisywane w zarzadcy w zwiazku z pozycja
     drawable_base /*const*/* get_ptr(size_t index); //Bez mozliwosci modyfikacji i zwolnienia

    //	REAKCJE NA ZDAZENIA
    //--------------------------
     int    on_click(int x,int y,int click); //Przepytuje obszary z reakcji na punkt.
                                             //Jesli on_click() zwraca 1 to mozna sie dowiedziec, który obszar wywołujac:
     int    get_last_lazy_area(); //zwroci -1 jesli juz raz wziete, lub inny blad
     int    on_input(int input_char); //Przepytuje obszary z chca znak
     int    on_change(const gps_area& ar); //Reguje na zmiane rozmiarow lub polozenia wlasnego obszaru
     void   replot(int flus=1) {drawable_base::replot(flus);} //Odrysuj wszystko
     void   replot(const gps_area& ar); //Odrysowuje obszary "nadepniete" przez "ar"
     void   _replot();	//Odrysowuje wszystkie (widoczne) obszary

    //  MANIPULATORY
    //----------------
     int    mark(size_t index,wb_color frame=default_color); //Zaznacza obszar
     int    mark_all(wb_color frame=default_color); //Zaznacza wszytkie widoczne
     int    unmark(size_t index);	//i odznacza obszar
     int    is_marked(size_t index); //Informuje czy jest zaznaczony
     int    is_minimized(size_t index); //Informuje czy jest zminimalizowany
     wb_dynarray<int> get_marked(wb_color filtr=default_color,int unm=0); //Zwraca liste zaznaczonych obszarow.
                                            // Jesli what=default color to wszystkie zaznaczone.
                                            // i opcjonalnie zdejmuje zaznaczenie

    //...DLA POJEDYNCZYCH OBSZAROW
     int	get_maximized(){return maximized;}; //Zwraca idex zmaksymalizowanego okna lub -1


     int    set_input(size_t index); //Ustala obszar jako pierwszy do wejscia z klawiatury lub zdarzen menu
     int    maximize(size_t index); //Oddaje podobszarowi caly zarzadzany obszar
     int    minimize(size_t index); //Ukrywa podobszar
     int    restore(size_t  index); //Odtwarza poprzednie polozenie i rozmiar obszaru
     int    orginal(size_t  index); //Odtwarza pierwotne  polozenie i rozmiar obszaru
     int    as_orginal(size_t index); //Uznaje aktualne polozenie obszaru za orginalne

    //...DLA GRUP OBSZAROW
     int	refresh(size_t index); //Odrysowuje obszar jesli nie zminimalizowany
     int    minimize(const wb_dynarray<int>& lst); //Ukrywa podobszary
     int    restore(const wb_dynarray<int>& lst); //Robi restore dla pod-obszarow
     int    restore(/*ALL*/); //Robi restore dla wszystkich pod-obszarow
     int    orginal(const wb_dynarray<int>& lst); //Robi orginal dla pod-obszarow
     int    tile(const wb_dynarray<int>& lst);    //Rearanzuje na chama, czyli po rowno, albo -1 jak nie da sie
     int    arrange(const wb_dynarray<int>& lst); //Inteligentnie rearanzuje.
};

/* ***************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/* Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk              */
/*        MAIL: borkowsk@iss.uw.edu.pl                               */
/*                               (Don't change or remove this note)  */
/* ***************************************************************** */
#endif




