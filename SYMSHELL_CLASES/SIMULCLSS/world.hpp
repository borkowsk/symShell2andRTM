// world.hpp: interface for the world class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLD_HPP__2C221821_7F0D_11D2_8525_0020AF422F75__INCLUDED_)
#define AFX_WORLD_HPP__2C221821_7F0D_11D2_8525_0020AF422F75__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "simpsour.hpp"
#include "sourmngr.hpp"
#include "mainmngr.hpp"
#include "textarea.hpp"
#include "logfile.hpp"

class world	//Podstawy u¿ytkowe dla calego swiata symulacji
//--------------------------------------------------------
{
virtual void   make_basic_sources(sources_menager& WhatSourMen);//NIE WOLNO U¯YÆ TAKIEJ FUNKCJI!

public:
//STEROWANIE ZAPISEM EKRANÓW
static 	  bool 			continous_dump;//=false;
//Sredni czas pojedynczego kroku i czas ostatniego
wb_pchar				ClockTime;
//Data rozpoczecia symulacji
wb_pchar				StartTime; //Nie zapisywana!
//Licznik kroków symulacji
unsigned long			Licznik;//jest resetowany przez "initialize" i zwiekszany przez "simulate"
//Ogranicznik krokow symulacji
unsigned long			MaxIterations;
//Co ile krokow symulacji sprawdzac pisanie do logu
unsigned				LogRatio;
//PONIZSZE NIE SA ZAPISYWANE W OPERACJACH I/O
//Co ile krokow symulacji sprawdzac wejscie
unsigned				InputRatio;

private:
//Uchwyt do aktualnego menagera danych
main_area_menager*		AreaMenager;
//Data/Czas oktualnego kroku
wb_pchar				TimeStamp;
protected:
//Obszar bezposredniego wyswietlania do wypisywania statusu.
//Jest zarzadzany przez menagera obszarów i mo¿e byc NULL!!!
text_area*				OutArea;
//Zarzadzanie zrodlami danych
sources_menager			Sources;	//Zarzadca seri przekaznikowych
//Obiekt rzadzacy zapisem historii
logfile					Log;		//plik z zapisem histori symulacji
//Strumien do zapisu calosci przebiegu symulacji, lub do czytania/odtwarzania

wb_ptr<fstream>			Out;		//Jesli niezerowy to zwala calosc symulacji na ten strumien.Otwiera jesli jest nazwa a nie ma strumienia.
static char				separator;
public:

//FLAGI I STERUJ¥CE NAZWY PLIKÓW
//Nazwa symulacji
wb_pchar				SimulName;  //G³ówna nazwa symulacji
wb_pchar				OutName;	//Nazwa pliku historii. Zamyka jak niezgodnosc nazw.
wb_pchar 				DumpNetName;//Czy zrzucaæ plik sieci razem z danymi

public:
//Konstrukcja i destrukcja
	world(const char* log_name,
		  size_t max_sources=50):
			MaxIterations(0xffffffff),Licznik(0),
			LogRatio(1),InputRatio(1),
			OutArea(NULL),AreaMenager(NULL),
			Sources(max_sources),Log(max_sources,log_name),
			Out(NULL)
			{SimulName="TheSymulationWorld";}
virtual
	~world()
	{
        //UWAGA - Tu jest b³¹d (ERROR)
        //Jesli jakies zrodlo zarejestrowane w logu nie jest uzywane w wizualizacji
		//to probuje obliczac wartosci ze zniszczonego juz swiata!!!
		//*
	    Log.try_writing();//Stan koncowy, jesli nie byl zapisany  */
	}

protected:
//Akcesory
int 				HasAreaMenager() { return AreaMenager!=NULL; }
area_menager&		MyAreaMenager();//Aktualny zarzadca ekranu podlaczony do swiata

//Wlasciwa implementacja kroku symulacji - do zaimplementowania
virtual void simulate_one_step()=0;	

// "user defined" actions for initialisation.
virtual void initialize_layers()=0;	
// user defined actions after read symulation state from file
virtual void after_read_from_image()=0;

//Generuje podstawowe zrodla dla wbudowanego menagera danych
virtual void make_basic_sources();

//Wspolpraca z menagerem wyswietlania
virtual void make_default_visualisation()=0;//Tworzy domyslne "lufciki" i umieszcza w

//Aktualizacja zawartosci okna statusu po n krokach symulacji	  
virtual void actualize_out_area(); 

public:
//Ustawianie nazwy symulacji i jej odczytywanie
virtual
int				set_simulation_name(const char* name);//zwraca 1 jak sie udalo. Moze byc niedozwolone
const char*		get_simulation_name() const	{ return SimulName.get(); }
//Co ile krokow symulacji zapisuje na wyjscie
void			set_log_ratio(unsigned ratio){LogRatio=ratio;}
//Co ile krokow symulacji sprawdzac wejscie
void			set_input_ratio(unsigned ratio){InputRatio=ratio;}
//Wspólpraca z licznikiem kroków
void			set_max_iteration(unsigned long iMaxIter){MaxIterations=iMaxIter;}
unsigned long	get_current_step(){return Licznik;}		// licznik krokow symulacji
//Ustawianie strumienia do zapisu histori
int				set_history_stream(const char* name);

//Glówne akcje:
////////////////
// stan startowy symulacji - jesli pierwszy raz to tworzy bazowe zrodla
// i ewentualnie podstawowe grafy i wywoluje initilise_leyers()
// jesli Replay!=0 to inicjalizuje warstwy z zerowego kroku
// pliku histori za pomoca funkcji "initialize_from_image"
void	initialize(main_area_menager* Menager=NULL,int Replay=0);

// Powtorzenie inicjalizacji dla powtornego przebiegu symulacji.
// Nie odtwarza strony wizualizacyjnej.
void    restart();

// wczytanie pojedynczego obrazu symulacji np dla inicjalizacji
//Jesli nie ma nazwy to z nazwy OutName
void    initialize_from_image(const char* FileName=NULL);

// kolejny(e) krok(i) symulacji
void	simulate(unsigned Steps=1);	

// prosta petla symulacyjna sprzezona z wizualizacja
void	simulation_loop(int ret_after);

// petla wczytywania symulacji
void	read_loop(int ret_after);

//Jak trzeba KONIECZNIE cos dopisac do logu
ostream& MyLogStream();

//Implementacja struieniowego wejscia/wyjscia. Zwracaj 1 jesli sukces!
virtual
int		implement_output(ostream& o) const;
virtual
int		implement_input(istream& i);
friend
ostream& operator << (ostream& o,const world& w);
friend
istream& operator >> (istream& i,world& w);
//Implementacja zapisu stanu symulacji w formacie NET lub NET2 (z atrybutami)
virtual //Domyœlnie puste
void dump_net_file(const char* core_name,unsigned long Step){}
};

inline
ostream& world::MyLogStream()
{ return Log.GetStream(); }

inline
area_menager&		world::MyAreaMenager()
//Aktualny zarzadca ekranu podlaczony do swiata
{
																				assert(AreaMenager!=NULL);
	return *AreaMenager;
}

#endif // !defined(AFX_WORLD_HPP__2C221821_7F0D_11D2_8525_0020AF422F75__INCLUDED_)
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/



