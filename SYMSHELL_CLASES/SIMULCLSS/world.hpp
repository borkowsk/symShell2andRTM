/// @file world.hpp
/// Interface for the base world class.
// ////////////////////////////////////////////////////////////////////
/// @date 2026-03-27 (modified)

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

class world	//Podstawy u¿ytkowe dla ca³ego œwiata symulacji
//---------------------------------------------------------
{
/// @brief NA RAZIE NIE WOLNO TAKIEJ FUNKCJI! CHOÆ MUSI BYÆ ZDEFINIOWANA.
/// Pomys³ zewnêtrznego manager-a danych okaza³ siê niesprwadzony.
virtual void   make_basic_sources(sources_menager& WhatSourMen);

public:
//STEROWANIE ZAPISEM EKRANÓW
static 	  bool 			continous_dump; //=false;
//Œredni czas pojedynczego kroku i czas ostatniego
wb_pchar				ClockTime;
//Data rozpoczêcia symulacji
wb_pchar				StartTime; //Nie zapisywana!
//Licznik kroków symulacji
unsigned long			Licznik; //jest resetowany przez "initialize" i zwiêkszany przez "simulate"
//Ogranicznik kroków symulacji
unsigned long			MaxIterations;
//Co ile kroków symulacji sprawdzaæ pisanie do logu
unsigned				LogRatio;
//PONI¯SZE NIE SA ZAPISYWANE W OPERACJACH I/O
//Co ile kroków symulacji sprawdzaæ wejœcie
unsigned				InputRatio;

private:
//Uchwyt do aktualnego manager danych
main_area_menager*		AreaMenager;
//Data/Czas aktualnego kroku
wb_pchar				TimeStamp;
protected:
//Obszar bezpoœredniego wyœwietlania do wypisywania statusu.
//Jest zarz¹dzany przez zarz¹dcê obszarów i mo¿e byæ NULL!!!
text_area*				OutArea;
//Zarz¹dzanie Ÿród³ami danych
sources_menager			Sources;	//Zarz¹dca seri przekaŸnikowych
//Obiekt rz¹dz¹cy zapisem historii
logfile					Log;		//plik z zapisem historii symulacji
//Strumieñ do zapisu ca³oœci przebiegu symulacji lub do czytania/odtwarzania

wb_ptr<fstream>			Out;		//Jeœli niezerowy to zwala ca³oœæ symulacji na ten strumieñ.
                                    // Otwiera, jeœli jest nazwa, a nie ma strumienia.
static char				separator;
public:

//FLAGI I STERUJ¥CE NAZWY PLIKÓW
//Nazwa symulacji
wb_pchar				SimulName;   //G³ówna nazwa symulacji
wb_pchar				OutName;	 //Nazwa pliku historii. Zamyka jak niezgodnoœæ nazw.
wb_pchar 				DumpNetName; //Czy zrzucaæ plik sieci razem z danymi

public:
//Konstrukcja i destrukcja
	world(const char* log_name,
		  size_t max_sources=50):
			MaxIterations(0xffffffff),Licznik(0),
			LogRatio(1),InputRatio(1),
			OutArea(nullptr),AreaMenager(nullptr),
			Sources(max_sources),Log(max_sources,log_name),
			Out(nullptr)
			{SimulName="TheSymulationWorld";}
virtual
	~world()
	{
        //TODO - Tu jest b³¹d (ERROR)
        //Jeœli jakieœ Ÿród³o zarejestrowane w logu nie jest u¿ywane w wizualizacji
		//to destruktor próbuje obliczaæ wartoœci ze zniszczonego ju¿ œwiata!!!
		//TODO SPRAWDZIÆ!
	    Log.try_writing(); //Stan koñcowy, jeœli nie byl zapisany  */
	}

protected:
//Akcesory
int 				HasAreaMenager() { return AreaMenager!=nullptr; }
area_menager&		MyAreaMenager(); //Aktualny zarz¹dca ekranu pod³¹czony do œwiata

//W³aœciwa implementacja kroku symulacji — do zaimplementowania
virtual void simulate_one_step()=0;	

// "user defined" actions for initialization.
virtual void initialize_layers()=0;

/// Derived class action after read simulation state from an image file.
virtual void after_read_from_image()=0;

/// Generuje podstawowe Ÿród³a dla wbudowanego zarz¹dcy danych.
virtual void make_basic_sources()=0;

/// Wspó³praca z zarz¹dc¹ wyœwietlania. Tworzy domyœlne "lufciki" i umieszcza w nim.
virtual void make_default_visualisation()=0;

/// Aktualizacja zawartoœci okna statusu po `n` krokach symulacji.
/// Domyœlnie wyœwietla numer kroku lub informacje o trybie interaktywnym.
virtual void actualize_out_area(); 

public:
/// Ustawianie nazwy symulacji.
virtual
int				set_simulation_name(const char* name); //@returns 1 jak siê uda³o, ale mo¿e byæ niedozwolone.

/// Odczytywanie nazwy symulacji.
const char*		get_simulation_name() const	{ return SimulName.get(); }

//Co ile kroków symulacji zapisuje na wyjœcie.
void			set_log_ratio(unsigned ratio){LogRatio=ratio;}

/// Czyta `InputRatio`, czyli co ile kroków symulacji sprawdzaæ wejœcie.
void			set_input_ratio(unsigned ratio){InputRatio=ratio;}

//Wspó³praca z licznikiem kroków:
void			set_max_iteration(unsigned long iMaxIter){MaxIterations=iMaxIter;}
unsigned long	get_current_step(){return Licznik;}		//!<  Czyta licznik kroków symulacji.

//Ustawianie strumienia do zapisu historii
int				set_history_stream(const char* name);

// G³ówne akcje:
// /////////////

// stan startowy symulacji — jeœli pierwszy raz to tworzy bazowe Ÿród³a
// i ewentualnie podstawowe grafy i wywo³uje `initialise_leyers()`
// jeœli `Replay!=0` to inicjalizuje warstwy z zerowego kroku
// pliku historii za pomoc¹ funkcji "initialize_from_image".
void	initialize(main_area_menager* Menager=nullptr,int Replay=0);

// Powtórzenie inicjalizacji dla powtórnego przebiegu symulacji.
// Nie odtwarza strony wizualizacyjnej.
void    restart();

//Wczytanie pojedynczego obrazu symulacji np. dla inicjalizacji.
//Jeœli nie ma nazwy to z nazwy `OutName`
void    initialize_from_image(const char* FileName=nullptr);

/// Kolejny(e) krok(i) symulacji.
void	simulate(unsigned Steps=1);	

/// Prosta pêtla symulacyjna sprzê¿ona z wizualizacja.
void	simulation_loop(int ret_after);

/// Pêtla wczytywania symulacji.
void	read_loop(int ret_after);

///Jak trzeba KONIECZNIE cos dopisaæ do logu.
ostream& MyLogStream();

///Implementacja strumieniowego wyjœcia. @returns 1,  jeœli sukces!
virtual
int		implement_output(ostream& o) const=0;

///Implementacja strumieniowego wejœcia. @returns 1,  jeœli sukces!
virtual
int		implement_input(istream& i)=0;

///Zapis. U¿ywa `implement_output()`, ¿eby zapisaæ zawartoœæ.
friend
ostream& operator << (ostream& o,const world& w);

///Odczyt. U¿ywa `implement_input()`, ¿eby odczytaæ zawartoœæ.
friend
istream& operator >> (istream& i,world& w);

///Implementacja zapisu stanu symulacji w formacie NET lub NET2 (z atrybutami).
virtual //Domyœlnie puste
void dump_net_file(const char* core_name,unsigned long Step){}
};

inline
ostream& world::MyLogStream()
{ return Log.GetStream(); }

inline
area_menager&		world::MyAreaMenager()
//Aktualny zarz¹dca ekranu pod³¹czony do œwiata
{
                                                assert(AreaMenager!=nullptr);
    return *AreaMenager;
}

#endif // !defined(AFX_WORLD_HPP__2C221821_7F0D_11D2_8525_0020AF422F75__INCLUDED_)
/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */



