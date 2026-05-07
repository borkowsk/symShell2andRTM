/// @file
/// @brief Interface for the base world-class. / Interfejs dla bazy światowej klasy.
/// @date 2026-05-07 (modified)
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

namespace symshell2 {

/// Podstawy użytkowe dla całego świata symulacji.
class world
//---------
{
    /// @brief NA RAZIE NIE WOLNO ZROBIĆ TAKIEJ FUNKCJI! CHOĆ MUSI BYĆ ZADEKLAROWANA.
    /// Pomysł zewnętrznego manager-a danych okazał się niesprawdzony.
    virtual void   make_basic_sources(sources_menager& WhatSourMen);

    /// Uchwyt do aktualnego manager danych.
    symshell2::main_area_menager*		AreaMenager;

    /// Data/Czas aktualnego kroku w wersji tekstowej.
    wb_pchar				TimeStamp;

protected:
    /// Obszar bezpośredniego wyświetlania do wypisywania statusu.
    /// Jest zarządzany przez zarządcę obszarów i może być NULL!!!
    symshell2::text_area*				OutArea;

    /// Zarządzanie źródłami danych. Czyli WBUDOWANY zarządca serii danych.
    sources_menager			Sources;

    /// Obiekt rządzący zapisem historii.
    logfile					Log;

    /// Strumień do dokładnego zapisu przebiegu symulacji pozwalającego na odtwarzanie.
    /// Jeśli wskaźnik nie jest pusty, to zrzuca całość symulacji na ten strumień.
    /// Otwiera strumień, jeśli jest podana nazwa, ale nie ma strumienia.
    wb_ptr<fstream>			Out;

    /// Separator do strumienia logowania `Out`.
    static char				separator;

public:
    /// Średni czas pojedynczego kroku i czas ostatniego.
    wb_pchar				ClockTime;
    /// Data i czas rozpoczęcia symulacji.
    wb_pchar				StartTime; //Nie zapisywana! (?)
    /// Licznik kroków symulacji.
    /// Jest resetowany przez "initialize" i zwiększany przez "simulate"
    unsigned long			Licznik;
    /// Ogranicznik kroków symulacji.
    unsigned long			MaxIterations;
    /// Co ile kroków symulacji sprawdzać pisanie do logu.
    unsigned				LogRatio;
    /// Co ile kroków symulacji sprawdzać wejście.
    unsigned				InputRatio;
    /// STEROWANIE ZAPISEM EKRANÓW.
    static 	  bool 			continous_dump; //=false;
    //STERUJĄCE NAZWY PLIKÓW
    wb_pchar				SimulName;   //!< Główna nazwa symulacji.
    wb_pchar				OutName;	 //!< Nazwa pliku historii. Zamyka jak niezgodność nazw.
    wb_pchar 				DumpNetName; //!< Nazwa pliku sieci do zrzutu razem z danymi.

public:
    //Konstrukcja i destrukcja:
    // ////////////////////////

    /// Główny konstruktor.
    world(const char* log_name,
          size_t max_sources=50):
            MaxIterations(0xffffffff),Licznik(0),
            LogRatio(1),InputRatio(1),
            OutArea(nullptr),AreaMenager(nullptr),
            Sources(max_sources),Log(max_sources,log_name),
            Out(nullptr)
    { SimulName="TheBasicSimulationWorld"; }

    /// Wirtualny destruktor.
    virtual  ~world();

    // Wymagane implementacji specyficznych akcji:
    // ///////////////////////////////////////////

    /// Właściwa implementacja jednego kroku symulacji — do zaimplementowania
    virtual void		simulate_one_step()=0;

    /// A derived classes action for world initialization.
    virtual void		initialize_layers()=0;

    /// Derived class action after read simulation state from an image file.
    virtual void		after_read_from_image()=0;

    /// Generuje podstawowe źródła dla wbudowanego zarządcy danych.
    virtual void		make_basic_sources()=0;

    /// Współpraca z zarządcą wyświetlania. Tworzy domyślne "lufciki" i umieszcza w nim.
    virtual void		make_default_visualisation()=0;

    /// Aktualizacja zawartości okna statusu po `n` krokach symulacji.
    /// Domyślnie wyświetla numer kroku lub informacje o trybie interaktywnym.
    // Wbrew pozorom jest zdefiniowane, choć CLint się gubi...
    virtual void		actualize_out_area();

    ///Implementacja strumieniowego wyjścia. @returns 1,  jeśli sukces!
    virtual int		implement_output(ostream& o) const=0;

    ///Implementacja strumieniowego wejścia. @returns 1,  jeśli sukces!
    virtual int		implement_input(istream& i)=0;

    //Akcesory:
    // ////////

    /// Ustawianie nazwy symulacji.
    virtual //TODO Dlaczego wirtualne?
    int				set_simulation_name(const char* name); //@returns 1 jak się udało, ale może być niedozwolone.

    /// Odczytywanie nazwy symulacji.
    const char*		get_simulation_name() const	{ return SimulName.get(); }

    /// Ustawianie strumienia do zapisu historii.
    int				set_history_stream(const char* name);

    /// Ustawianie maksymalnej wartości licznika kroków symulacji.
    /// Po osiągnięciu tej wartości symulacja zostaje zatrzymana.
    void			set_max_iteration(unsigned long iMaxIter){MaxIterations=iMaxIter;}

    /// Czyta licznik kroków symulacji.
    unsigned long	get_current_step() const { return Licznik; }

    /// Co ile kroków symulacji zapisuje na wyjście.
    void			set_log_ratio(unsigned ratio){LogRatio=ratio;}

    /// Ustawia `InputRatio`, czyli co ile kroków symulacji sprawdzać wejście.
    void			set_input_ratio(unsigned ratio){InputRatio=ratio;}

    /// Aktualny zarządca ekranu podłączony do tego świata.
    symshell2::area_menager&	MyAreaMenager();

    /// Sprawdzenie, czyma już podłączonego zarządcę okien.
    int 			HasAreaMenager() { return AreaMenager!=nullptr; }

    ///Jak trzeba KONIECZNIE coś dopisać do logu.
    ostream&		MyLogStream();

    // Główne akcje świata:
    // ////////////////////

    /// Przygotowuje stan startowy symulacji.
    /// Jeśli pierwszy raz to
    ///     - wywołuje `initialise_leyers()`
    ///     - tworzy bazowe źródła
    ///     - i opcjonalnie podstawowe grafy
    /// jeśli z `Replay!=0` to inicjalizuje warstwy z zerowego kroku
    /// pliku historii za pomocą funkcji "initialize_from_image".
    void		initialize(symshell2::main_area_menager* Menager=nullptr,int Replay=0);

    /// Powtórzenie inicjalizacji dla powtórnego przebiegu symulacji.
    /// Nie odtwarza strony wizualizacyjnej.
    void		restart();

    /// Wczytanie pojedynczego obrazu symulacji np. dla inicjalizacji.
    /// TODO CHECK... Jeśli nie ma nazwy to z nazwy `OutName`
    void		initialize_from_image(const char* FileName=nullptr);

    /// Wykonuje kolejny(e) krok(i) symulacji.
    /// Opakowuje "inteligentnie" `simulate_one_step`.
    /// @param Steps określa ile prostych kroków należy wykonać.
    void		simulate(unsigned Steps=1);

    /// Prosta pętla symulacyjna sprzężona z wizualizacją.
    /// @param ret_after ???.
    void		simulation_loop(int ret_after);

    /// Pętla wczytywania symulacji z pliku.
    /// @param ret_after ???.
    void		read_loop(int ret_after);

    /// @name Operatory i/o chcą mieć dostęp do wirtualnej implementacji i separatora.
    /// @{
    ///Zapis na strumień. Opakowuje `implement_output`, żeby zapisać zawartość.
    friend ostream& operator << (ostream& o,const world& w);

    ///Odczyt ze strumienia. Odpakowuje, używając `implement_input` żeby odczytać zawartość.
    friend istream& operator >> (istream& i,world& w);
    /// @}

    /// Odczyt domyślnego separatora.
    char sepa() const { return separator; }

    ///Implementacja zapisu stanu symulacji w formacie NET lub NET2 (z atrybutami).
    virtual //Domyślnie puste, nie wiem, czy kiedykolwiek używane.
    void dump_net_file(const char* core_name,unsigned long Step){}
};

// Lokalne implementacje `inline`:
//-///////////////////////////////

inline
ostream& world::MyLogStream()
//Dostęp do strumienia logu (żeby coś "ręcznie" dopisać)
{
    return Log.GetStream();
}

inline
symshell2::area_menager&		world::MyAreaMenager()
//Aktualny zarządca ekranu podłączony do świata
{
                                                assert(AreaMenager!=nullptr);
    return *AreaMenager;
}

} //namespace symshell2

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
#endif // !defined(WORLD_HPP_INCLUDED_)





