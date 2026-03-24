//				POMOCNICZE TYPY OBSZAROW
//--------------------------------------------------------------------
// Sluza glownie jako typy bazowe dla klas specjalizowanych, wykonujacych
// jakies akcje w metodzie is_inside() i ewentualnie podobnych.
#ifndef __GADGETS_HPP__
#define __GADGETS_HPP__
#ifndef __cplusplus
#error C++ required
#endif

#include "drawable.hpp"
#include "datasour.hpp"

/// Klasa bazowa wszystkich gadżetów ekranowych.
class gadget:public drawable_base
//-------------------------------------
{
protected:
	wb_color draw_color; //!< Główny kolor. Pozostałe dziedziczone z `drawable_base`.

public:
	gadget(	int ix1,int iy1,int ix2,int iy2,		//!< położenie gadżetu.
			wb_color icolor,						//!< kolor znaczących elementów.
			wb_color ibkg=default_white,            //!< kolor tła.
			wb_color ifr=default_white              //!< kolor ramki.
			):
			drawable_base(ix1,iy1,ix2,iy2,ibkg,ifr),draw_color(icolor)
				{}

	virtual void _replot()=0; //Rysuje - np X. Do reimplementacji w klasach potomnych

	virtual int  on_click(int x,int y,int click=0); //Jesli "inside" to rysuje w inwersji, ale zwraca wynik _on_click() lub 0

	// TODO Poniżej chyba pozostałość po starym projekcie?
	/// Wewnetrzna akcja klasy potomnej jest w środku tej metody wywoływanej w domyślnym `on_click`.
	/// @returns 1 jeśli "nie obsłużono" albo "2" jeśli obsłużono.
	virtual int _on_click(int /*x*/,int /*y*/,int /*click*/)
	     		{return 1;}							         //Domyslnie zwraca 1=="nie obsluzono"

};

/// Klasa bazowa dla gadżetów reagujących na zdarzenia użytkownika.
class sensitive_area:public gadget
//--------------------------------------------
{
public:
	sensitive_area( int ix1,int iy1,int ix2,int iy2, //!< położenie gadżetu.
					unsigned icolor,                 //!< kolor znaczących elementów.
					unsigned ibkg=default_white,     //!< kolor tła.
					unsigned ifr=default_transparent //!< kolor ramki.
					):
			gadget(ix1,iy1,ix2,iy2,icolor,ibkg,ifr)
				{}

	void _replot() override; //!< Rysuje gadżet - np X. Do reimplementacji w klasach potomnych

	// TODO Poniżej chyba pozostałość po starym projekcie?
//int _on_click(int /*x*/,int /*y*/,int /*click*/)//Prawdziwa akcja klasy potomnej powinna być w srodku tej metody
//			{return 2;}							  //i zwracac 2 jesli "obsluzono".

};

typedef sensitive_area empty_area;

/// Klasa gadźetu implementująca przycisk.
/// Dwa możliwe tryby wyświetlania:
/// 0 -> vertical print mode
/// 1 -> horizontal print mode
class button:public sensitive_area
//---------------------------------------------
{
protected:
	unsigned vhmode:1;		//!< vert(0)/hor(1) - tryb wyswietlania.
	unsigned reserved:15;	//!< co najmniej 15 bitow i tak bedzie zajete.

public:
	button(int ix1,int iy1,int ix2,int iy2,	//!< położenie gadżetu.
			const char* iprompt,			//!< tekst na przycisku.
			unsigned imode=1,               //!< tryb wyświetlania.
			unsigned icolor=default_black,  //!< kolor znaczących elementów.
			unsigned ibackground=default_white, //!< kolor tła strzalek/tarczy.
			unsigned iframe=128             //!< kolor ramki.
			):
		sensitive_area(ix1,iy1,ix2,iy2,icolor,ibackground,iframe),vhmode(imode),reserved(0)
		{
			settitle(iprompt); //Prompt jest pamiętany jako tytul okna, ale wyswietlany recznie.
			settitlecolor(default_transparent); // Wiec domyslnie ten tytuł...
			settitleback(default_transparent);  // ...jest niewidoczny (bo by się wyświetlał nie tak i nie tam).
		}

	void _replot() override;  //!< Odrysowuje. W tym ewentualne składowe.

	// TODO Poniżej chyba pozostałość po starum projekcie?
//int _on_click(int /*x*/,int /*y*/,int /*click*/)//Prawdziwa akcja klasy potomnej powinna byc w srodku tej metody
//			{return 2;}							  //i zwracac 2 jesli "obsluzono".

};

/// Klasa gadżetu implementująca strzałkę w jednym z czterech kierunków lub tarcze (jako 3 wymiar, albo coś).
/// Znaczenie `mode` lub parametru `imode` konstruktora:
/// 0 - print target (-(+)-)
/// 1 - print up arrow
/// 2 - print left arrow
/// 3 - print down arrow
/// 4 - print right arrow
class arrow_button:public sensitive_area
//---------------------------------------------
{
protected:
	int mode; //!< tryb wyswietlania, czyli kierunek strzałki albo tarcza.

public:
	arrow_button(int ix1,int iy1,int ix2,int iy2,	//!< położenie gadżetu.
			int imode=0,							//!< inicjalizacja trybu wyświetlania (patrz na opis klasy).
			unsigned icolor=default_black,			//!< kolor znaczących elementów.
			unsigned ibackground=default_white,		//!< kolor tła strzalek/tarczy.
			unsigned iframe=default_transparent     //!< kolor ramki.
			):
		sensitive_area(ix1,iy1,ix2,iy2,icolor,ibackground,iframe),
		mode(imode)
		{}

	void _replot() override; //!< Odrysowuje skladowe.

};

/// Klasa gadżetu łącząca strzałkę w lewo i strzałkę w prawo.
/// @note To jest klasa bazowa dla klas reiplementujących `_user_action`.
class leftrigt_button:public sensitive_area
//==============================================
{
protected:
	wb_ptr<drawable_base> left;  //!< podobiekt (strzałka) ruchu w lewo.
	wb_ptr<drawable_base> right; //!< podobiekt (strzałka) ruchu w prawo.
public:
	/// Konstruktor umieszczający gadżet w konkretnycm miejscu.
	leftrigt_button(int ix1,int iy1,int ix2,int iy2);

	void _replot() override; //!< Odrysowuje skladowe.

	/// Implementacja reakcji na zmianę rozmiarów lub przesunięcie.
	/// Musi przesunac wspolrzedne skladowych razem ze swoimi.
	int on_change(const gps_area&) override;

	/// Akcja do wykonania w `on_click`. W tej klasie nic nie robi, choć sygnalizuje obsłużenie.
    virtual int _user_action( int leftorright, //!< Jeśli lewo to -1, a jeśli prawo to 1
    						  int click        //!< Może mieć znaczenie, który przycisk myszki.
    						  );

	/// Implementacja reakcji na kliknięcie.
	/// Przepytuje skladowe i jesli ktoras zostala trafiona to
	/// wywołuje `_user_action()` z odpowiednim parametrem.
	int on_click(int x,int y,int click=0) override;

};

/// Klasa gadżetu do zmiany pola widzenia na prostokatną serię danych.
/// Pozwala zmieniać wielkość okna i przesuwać to okno w czterech kierunkach.
/// Tło gadżetu domyślnie jest takie jakie dla całego okna!
class steering_wheel:public sensitive_area
//----------------------------------------
{
	/// Wskazniki do podpiętych danych.
	/// @note Pamiec zadnej z seri danych nie jest tu zarządzana!
	wb_dynarray<rectangle_source_base*> data;

	//Zarządzane wskazniki do elementow sterowania
	wb_ptr<drawable_base> resizing; //!< podobiekt zwiększania/zmiejszania pola widzenia.
	wb_ptr<drawable_base> left;		//!< podobiekt (strzałka) ruchu w lewo.
	wb_ptr<drawable_base> up;		//!< podobiekt (strzałka) ruchu w górę.
	wb_ptr<drawable_base> right;	//!< podobiekt (strzałka) ruchu w prawo.
	wb_ptr<drawable_base> down;		//!< podobiekt (strzałka) ruchu w dół.

public:
	/// Konstruktor pobierający składowe gadżetu i pobierający jeden adres serii danych.
	steering_wheel( rectangle_source_base*     idat, //!< Dane. Pamiec seri nigdy nie jest tu zarządzana.
					wb_ptr<drawable_base> ires,      //Pamiec dla pod-obszarow jest
					wb_ptr<drawable_base> iup,       // z a w s z e
					wb_ptr<drawable_base> ileft,     //zarządzana.
					wb_ptr<drawable_base> idown,     //Wspolrzedne obszaru steeering wheel sa
					wb_ptr<drawable_base> iright     //ustalane z wspolrzednych jego skladowych.
					);

	/// Konstruktor pobierający składowe gadżetu i pobierający adresy danych z tablicy.
	steering_wheel( wb_dynarray<rectangle_source_base*>&  idat, //!< Dane. Pamiec seri nigdy nie jest tu zarządzana.
					wb_ptr<drawable_base> ires,     //Pamiec dla pod-obszarow jest
					wb_ptr<drawable_base> iup,      // z a w s z e
					wb_ptr<drawable_base> ileft,    //zarządzana.
					wb_ptr<drawable_base> idown,    //Wspolrzedne obszaru steeering wheel sa
					wb_ptr<drawable_base> iright    //ustalane z wspolrzednych jego skladowych.
					);

	/// Konstruktor kładący gadżet w konkretnym miejscu i pobierający jeden adres danych.
	steering_wheel( int ix1,int iy1,int ix2,int iy2, //!< położenie gadżetu.
					rectangle_source_base*  idat     //!< Dane. Pamiec seri nigdy nie jest tu zarządzana.
					);

	/// Konstruktor kładący gadżet w konkretnym miejscu i pobierający adresy danych z tablicy.
	steering_wheel( int ix1,int iy1,int ix2,int iy2, //!< położenie gadżetu.
					wb_dynarray<rectangle_source_base*>&  idat  //!< Dane. Pamiec seri nigdy nie jest tu zarządzana.
					);

	void _replot() override; //!< Odrysowuje skladowe.

	/// Implementacja reakcji na zmianę rozmiarów lub przesunięcie.
	/// Musi przesunac wspolrzedne skladowych razem ze swoimi.
	int on_change(const gps_area&) override;

	/// Implementacja reakcji na kliknięcie.
	/// Przepytuje skladowe i jesli ktoras zostala trafiona to
	/// adekwatnie zmienia serie za pomoca metody sub()
	/// oraz wymusza odnowienie ekranu
	int on_click(int x,int y,int click=0) override;

};

/// Szablon klasy gadżetu do zmiany wartości zmiennej liczbowej dowolnego typu.
template<class NUMBER>
class knob_for_value:public leftrigt_button
// ----------------------------------------
{
protected:
	NUMBER min,max,step; //!< parametry zmiany podpiętej zmiennej.
	NUMBER*      valptr; //!< adres podpiętej zmiennej.

public:
	/// Konstruktor.
	knob_for_value( int ix1,int iy1,int ix2,int iy2, //!< położenie gadżetu.
					const char* Title="knob",        //!< tytuł gadżetu (rzadko potrzebny).
					NUMBER* valptr=NULL,             //!< adres zmiennej, która ma być modyfikowana przez gadżet.
					NUMBER min=0,					 //!< dolne ograniczenie zmiany.
					NUMBER max=100,					 //!< górne ograniczenie zmiany.
					double iprop=0.01				 //!< krok zmiany.
					):
		leftrigt_button(ix1,iy1,ix2,iy2)
		{										assert(min<max);
			settitle(Title);
			this->valptr=valptr;
			this->max=max;
			this->min=min;
			set_step_proportionaly(iprop);
		}

	/// Akcja używana w `on_click()`.
	int _user_action( int leftorright, //!< Jeśli lewo to -1, a jeśli prawo to 1
					  int click        //!< Może mieć znaczenie, który przycisk myszki.
					) override
	{
		if(valptr == nullptr)
			return 0;

		if(leftorright>0)
		{
		   if(click==1)
			*valptr+=step;
		   else if(click==2)
				*valptr+=step/10;

			if(*valptr>max) *valptr=max;
		}
		else
		if(leftorright<0)
		{
		   if(click==1)
		   	*valptr-=step;
		   else if(click==2)
				*valptr-=step/10;

			if(*valptr<min) *valptr=min;
		}

		return 2; //Nie 1!!!
	}

	/// Dostęp do adresu modyfikowanej zmiennej.
	/// @note Po co to?
	NUMBER* get_ptr() { return valptr;}

	/// Zmiana adresu modyfikowanej zmiennej.
	/// @note Po co to?
	virtual void change_ptr(NUMBER* ivalptr)
	{
		valptr=ivalptr;
		if(*valptr<min || max<*valptr)
			*valptr=(min+max)/2;
	}

	/// Zmiana kroku modyfukacji zmiennej.
	virtual void set_step(NUMBER istep)
	{
		if((max-min)<istep)
			step=istep;
	}

	/// Zmiana kroku modyfukacji zmiennej w proporcji (0..0.5).
	virtual void set_step_proportionaly(double proportion)
	{
		if(0<proportion && proportion<=0.5) //Co najmniej 2 kroki!!!
			step=(max-min)*proportion;
	}
};

#endif
/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */

