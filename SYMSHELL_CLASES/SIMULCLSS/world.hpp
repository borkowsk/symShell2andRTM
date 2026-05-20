/// @file
/// @brief **Interface for the whole world** /<br> _Interfejs dla klasy reprezentującej cały świat._
/// @date 2026-05-20 (modified)
// ********************************************************************************************************************
//
#if !defined(WORLD_HPP_INCLUDED_)
#define WORLD_HPP_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "simpsour.hpp"
#include "sourmngr.hpp"
#include "mainmngr.hpp"
#include "textarea.hpp"
#include "logfile.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "modernize-use-auto"

namespace sym2 { namespace shell {

/// @brief @PL{ Podstawy użytkowe dla całego świata symulacji. }
///        @EN{ Usage foundations for the entire world of simulation. }
class world
//---------
{
    /// @brief @PL{ Pomysł zewnętrznego manager-a danych okazał się nieużyteczny. }
    ///        @EN{ The idea of an external data manager turned out to be useless. }
    /// @PL{ NA RAZIE NIE WOLNO ZROBIĆ TAKIEJ FUNKCJI! CHOĆ MUSI BYĆ ZADEKLAROWANA.
    /// @EN{ FOR THE TIME BEING, SUCH A FUNCTION IS NOT ALLOWED! ALTHOUGH IT MUST BE DECLARED. }
    virtual void   make_basic_sources(data::sources_manager& WhatSourMen);

    /// @PL{ "Uchwyt" do aktualnego manager danych. } @EN{ "Handle" to the current data manager. }
    sym2::visual::main_area_manager*		AreaManager;

    /// @PL{ Data/Czas aktualnego kroku w wersji tekstowej. } @EN{ Date/Time of the current step in text version. }
    wb_pchar								TimeStamp;

protected:
    /// Obszar bezpośredniego wyświetlania do wypisywania statusu.
    /// Jest zarządzany przez zarządcę obszarów i może być NULL!!!
    sym2::visual::text_area*				OutArea;

    /// Zarządzanie źródłami danych. Czyli WBUDOWANY zarządca serii danych.
    data::sources_manager					Sources;

    /// Obiekt rządzący zapisem historii.
    data::logfile							Log;

    /// Strumień do dokładnego zapisu przebiegu symulacji pozwalającego na odtwarzanie.
    /// Jeśli wskaźnik nie jest pusty, to zrzuca całość symulacji na ten strumień.
    /// Otwiera strumień, jeśli jest podana nazwa, ale nie ma strumienia.
    wb_ptr<fstream>							Out;

    /// Separator do strumienia logowania `Out`.
    static char								separator;

public:
    /// Średni czas pojedynczego kroku i czas ostatniego.
    wb_pchar				ClockTime;
    /// Data i czas rozpoczęcia symulacji.
    wb_pchar				StartTime; //Nie zapisywana! (?)
    /// Licznik kroków symulacji.
    /// Jest resetowany przez "initialize" i zwiększany przez "simulate"
    unsigned long			StepCounter;
    /// Ogranicznik kroków symulacji.
    unsigned long			MaxIterations;
    /// Co ile kroków symulacji sprawdzać pisanie do logu.
    unsigned				LogRatio;
    /// Co ile kroków symulacji sprawdzać wejście.
    unsigned				InputRatio;
    /// STEROWANIE ZAPISEM EKRANÓW.
    static 	  bool 			continuous_dump; //=false;
    /// @name STERUJĄCE NAZWY PLIKÓW
    /// @{
    wb_pchar				SimulName;		//!< Główna nazwa symulacji.
    wb_pchar				OutName;		//!< Nazwa pliku historii. Zamyka jak niezgodność nazw.
    wb_pchar 				DumpNetName;	//!< Nazwa pliku sieci do zrzutu razem z danymi.
    /// @}
public:
    //Konstrukcja i destrukcja:
    //=========================

    /// @brief @PL{ Główny konstruktor świata. } @EN{ "The chief constructor of the world". }
    explicit world(const char* log_name,
                   size_t max_sources=50)
    : MaxIterations(0xffffffff), StepCounter(0),
      LogRatio(1), InputRatio(1),
      OutArea(nullptr), AreaManager(nullptr),
      Sources(max_sources), Log(max_sources,log_name),
      Out(nullptr)
    { SimulName="TheBasicSimulationWorld"; }

    /// @brief @PL{ Wirtualny destruktor. Sporo robi w ukryciu. } @EN{ Virtual destructor, which does a lot of hidden jobs. }
    virtual  ~world();

    // Wymagane albo zachęcające do implementacji specyficznych akcji:
    //================================================================

    /// @brief @IN{WYMAGANA|REQUIRED}: @PL{ Właściwa implementacja jednego kroku symulacji — do zaimplementowania. } @EN{ Real implementation of one simulation step - to be implemented. }
    virtual void		simulate_one_step()=0;

    /// @brief  @IN{WYMAGANA|REQUIRED}: @EN{ Derived types action for world initialization. } @PL{ Akcja inicjalizacji świata - do zaimplementowania dla typów pochodnych. }
    virtual void		initialize_layers()=0;

    /// @brief  @IN{WYMAGANA|REQUIRED}: @EN{ Derived types action after reading simulation state from an image file. } @PL{ Akcja typów pochodnych po odczytaniu stanu symulacji z pliku graficznego. }
    virtual void		after_read_from_image()=0;

    /// @brief  @IN{WYMAGANA|REQUIRED}: @PL{ Generuje podstawowe źródła dla wbudowanego zarządcy danych. } @EN{ Makes the primary source for general data management. }
    virtual void		make_basic_sources()=0;

    /// @brief  @IN{WYMAGANA|REQUIRED}: @PL{ Współpraca z zarządcą wyświetlania. Tworzy domyślne "lufciki" i umieszcza w nim. } @EN{ Collaborates with the display manager. Creates default "lufties" and places them there. }
    virtual void		make_default_visualisation()=0;

    /// @brief @PL{ Aktualizacja zawartości okna statusu po jednym lub wielu krokach symulacji. } @EN{ Updating the contents of the status window after one or more simulation steps. }
    /// @details @PL{ Domyślnie wyświetla numer kroku lub informacje o trybie interaktywnym. } @EN{ By default, displays the step number or interactive mode information. }
    virtual void		actualize_out_area();

    /// @L2{WYMAGANA|REQUIRED}: @PL{ Implementacja strumieniowego wyjścia. @returns 1, jeśli sukces! } @EN{ Streaming output implementation. @returns 1 if successful! }
    virtual int		implement_output(ostream& o) const=0;

    /// @L2{WYMAGANA|REQUIRED}: @PL{ Implementacja strumieniowego wejścia. @returns 1, jeśli sukces! } @EN{ Streaming input implementation. @returns 1 if successful! }
    virtual int		implement_input(istream& i)=0;

    //Akcesory:
    //=========

    /// @brief @PL{ Ustawianie nazwy symulacji. @returns 1 jak się udało, ale może być niedozwolone. }
    ///        @EN{ Setting the simulation name. @returns 1 if successful, but may be illegal. }
    virtual //TODO Dlaczego wirtualne?
    int				set_simulation_name(const char* name);

    /// @brief @PL{ Odczytywanie nazwy symulacji. } @EN{ Reading the simulation name. }
    const char*		get_simulation_name() const	{ return SimulName.get(); }

    /// @brief @PL{ Ustawianie strumienia do zapisu historii. } @EN{ Setting a stream to record history. }
    int				set_history_stream(const char* name);

    /// @brief @PL{ Ustawianie maksymalnej wartości licznika kroków symulacji. } @EN{ Set the maximum value of the simulation step counter. }
    /// Po osiągnięciu tej wartości symulacja zostaje zatrzymana.
    void			set_max_iteration(unsigned long iMaxIter){MaxIterations=iMaxIter;}

    /// @brief @PL{ Czyta licznik kroków symulacji. } @EN{ Reads the simulation step counter. }
    unsigned long	get_current_step() const { return StepCounter; }

    /// @brief @PL{ Co ile kroków symulacji zapisuje na wyjście. } @EN{ After how many simulation steps it writes to the output. }
    void			set_log_ratio(unsigned ratio){LogRatio=ratio;}

    /// @brief @PL{ Ustawia `InputRatio`, czyli co ile kroków symulacji sprawdzać wejście. }
    ///        @EN{ Sets the `InputRatio`, i.e. after how many simulation steps to check the input. }
    void			set_input_ratio(unsigned ratio){InputRatio=ratio;}

    /// @brief @PL{ Aktualny zarządca ekranu podłączony do tego świata. } @EN{ Current screen manager connected to this world. }
    sym2::visual::area_manager&	MyAreaManager();

    /// @brief @PL{ Sprawdzenie, czy ma już podłączonego zarządcę okien. } @EN{ Checking if it already has a window manager connected. }
    int 			HasAreaMenager() { return AreaManager!=nullptr; }

    /// @brief @PL{ Jak trzeba KONIECZNIE coś dopisać do logu. } @EN{ If necessary, MUST add something to the log. }
    ostream&		MyLogStream();

    // Główne akcje świata:
    //=====================

    /// @brief @PL{ Przygotowuje stan startowy symulacji. } @EN{ Prepares the starting state of the simulation. }
    /// @details
    /// Jeśli pierwszy raz to
    ///     - wywołuje `initialise_layers()`
    ///     - tworzy bazowe źródła
    ///     - i opcjonalnie podstawowe grafy
    /// , jeśli z `Replay != 0` to inicjalizuje warstwy z zerowego kroku
    /// pliku historii za pomocą funkcji "initialize_from_image".
    void		initialize(sym2::visual::main_area_manager* mainAreaManager=nullptr, int Replay=0);

    /// @brief @PL{ Powtórzenie inicjalizacji dla powtórnego przebiegu symulacji. }
    ///        @EN{ Repeat initialization for a second simulation run. }
    /// @details Nie odtwarza strony wizualizacyjnej.
    void		restart();

    /// @brief @PL{ Wczytanie pojedynczego obrazu symulacji np. dla inicjalizacji. } @EN{ Loading a single simulation image, e.g. for initialization. }
    /// TODO CHECK... Jeśli nie ma nazwy to z nazwy `OutName`
    void		initialize_from_image(const char* FileName=nullptr);

    /// @brief @PL{ Wykonuje kolejny(e) krok(i) symulacji. } @EN{ Performs the next simulation step(s). }
    /// Opakowuje "inteligentnie" `simulate_one_step`.
    /// @param Steps określa ile prostych kroków należy wykonać.
    void		simulate(unsigned Steps=1);

    /// @brief @PL{ Prosta pętla symulacyjna sprzężona z wizualizacją. } @EN{ A simple simulation loop coupled with visualization. }
    /// @param ret_after ???.
    void		simulation_loop(int ret_after);

    /// @brief @PL{ Pętla wczytywania symulacji z pliku. } @EN{ Loop for loading simulation from a file. }
    /// @param ret_after ???.
    void		read_loop(int ret_after);

    /// @name Operatory i/o chcą mieć dostęp do wirtualnej implementacji i separatora.
    /// @{
    ///Zapis na strumień. Opakowuje `implement_output`, żeby zapisać zawartość.
    friend ostream& operator << (ostream& o,const world& w);

    ///Odczyt ze strumienia. Odpakowuje, używając `implement_input` żeby odczytać zawartość.
    friend istream& operator >> (istream& i,world& w);
    /// @}

    /// @PL{ Odczyt domyślnego separatora. } @EN{ Reading the default separator. }
    char sepa() const { return separator; }

    /// @PL{ Implementacja zapisu stanu symulacji w formacie NET lub NET2 (z atrybutami). }
    /// @EN{ Implementation of saving the simulation state in NET or NET2 format (with attributes). }
    /// @details Domyślnie puste, nie wiem, czy kiedykolwiek używane.
    virtual
    void dump_net_file(const char* core_name,unsigned long Step){}
};

// Lokalne implementacje `inline`:
//-===============================

inline
ostream& world::MyLogStream()
//Dostęp do strumienia logu (żeby coś "ręcznie" dopisać)
{
    return Log.GetStream();
}

inline
sym2::visual::area_manager&		world::MyAreaManager()
//Aktualny zarządca ekranu podłączony do świata
{
                                                assert(AreaManager!=nullptr);
    return *AreaManager;
}

}} //namespace sym2::shell

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
#endif // !defined(WORLD_HPP_INCLUDED_)





