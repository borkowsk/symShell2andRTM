/// @file world.hpp
/// Interface for the base world-class.
// ////////////////////////////////////////////////////////////////////
/// @date 2026-03-28 (modified)

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

/// Podstawy u¿ytkowe dla ca³ego œwiata symulacji.
class world
//---------------------------------------------------------
{
    /// @brief NA RAZIE NIE WOLNO TAKIEJ FUNKCJI! CHOÆ MUSI BYÆ ZDEFINIOWANA.
    /// Pomys³ zewnêtrznego manager-a danych okaza³ siê niesprowadzany.
    virtual void   make_basic_sources(sources_menager& WhatSourMen);
    /// Uchwyt do aktualnego manager danych.
    main_area_menager*		AreaMenager;
    /// Data/Czas aktualnego kroku.
    wb_pchar				TimeStamp;

protected:
    /// Obszar bezpoœredniego wyœwietlania do wypisywania statusu.
    /// Jest zarz¹dzany przez zarz¹dcê obszarów i mo¿e byæ NULL!!!
    text_area*				OutArea;
    /// Zarz¹dzanie Ÿród³ami danych. Czyli WBUDOWANY zarz¹dca serii danych.
    sources_menager			Sources;
    /// Obiekt rz¹dz¹cy zapisem historii.
    logfile					Log;

    /// Strumieñ do dok³adnego zapisu przebiegu symulacji pozwalaj¹cego na odtwarzanie.
    /// Jeœli wskaŸnik nie jest pusty, to zrzuca ca³oœæ symulacji na ten strumieñ.
    /// Otwiera strumieñ, jeœli jest podana nazwa, ale nie ma strumienia.
    wb_ptr<fstream>			Out;
    static char				separator;

public:
    /// Œredni czas pojedynczego kroku i czas ostatniego.
    wb_pchar				ClockTime;
    /// Data i czas rozpoczêcia symulacji.
    wb_pchar				StartTime; //Nie zapisywana! (?)
    /// Licznik kroków symulacji.
    /// Jest resetowany przez "initialize" i zwiêkszany przez "simulate"
    unsigned long			Licznik;
    /// Ogranicznik kroków symulacji.
    unsigned long			MaxIterations;
    /// Co ile kroków symulacji sprawdzaæ pisanie do logu.
    unsigned				LogRatio;
    /// Co ile kroków symulacji sprawdzaæ wejœcie.
    unsigned				InputRatio;
    /// STEROWANIE ZAPISEM EKRANÓW.
    static 	  bool 			continous_dump; //=false;
    //STERUJ¥CE NAZWY PLIKÓW
    wb_pchar				SimulName;   //!< G³ówna nazwa symulacji.
    wb_pchar				OutName;	 //!< Nazwa pliku historii. Zamyka jak niezgodnoœæ nazw.
    wb_pchar 				DumpNetName; //!< Nazwa pliku sieci do zrzutu razem z danymi.

public:
    //Konstrukcja i destrukcja:
    // ////////////////////////

    /// G³ówny konstruktor.
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

protected:
    // Wymagane implementacji specyficznych akcji:
    // ///////////////////////////////////////////

    /// W³aœciwa implementacja jednego kroku symulacji — do zaimplementowania
    virtual void		simulate_one_step()=0;

    /// A derived classes action for world initialization.
    virtual void		initialize_layers()=0;

    /// Derived class action after read simulation state from an image file.
    virtual void		after_read_from_image()=0;

    /// Generuje podstawowe Ÿród³a dla wbudowanego zarz¹dcy danych.
    virtual void		make_basic_sources()=0;

    /// Wspó³praca z zarz¹dc¹ wyœwietlania. Tworzy domyœlne "lufciki" i umieszcza w nim.
    virtual void		make_default_visualisation()=0;

    /// Aktualizacja zawartoœci okna statusu po `n` krokach symulacji.
    /// Domyœlnie wyœwietla numer kroku lub informacje o trybie interaktywnym.
    virtual void		actualize_out_area();

    ///Implementacja strumieniowego wyjœcia. @returns 1,  jeœli sukces!
    virtual int		implement_output(ostream& o) const=0;

    ///Implementacja strumieniowego wejœcia. @returns 1,  jeœli sukces!
    virtual int		implement_input(istream& i)=0;

public:
    //Akcesory:
    // ////////

    /// Ustawianie nazwy symulacji.
    virtual //TODO Dlaczego wirtualne?
    int				set_simulation_name(const char* name); //@returns 1 jak siê uda³o, ale mo¿e byæ niedozwolone.
    /// Odczytywanie nazwy symulacji.
    const char*		get_simulation_name() const	{ return SimulName.get(); }
    /// Ustawianie strumienia do zapisu historii.
    int				set_history_stream(const char* name);
    /// Ustawianie maksymalnej wartoœci licznika kroków symulacji.
    /// Po osi¹gniêciu tej wartoœci symulacja zostaje zatrzymana.
    void			set_max_iteration(unsigned long iMaxIter){MaxIterations=iMaxIter;}
    /// Czyta licznik kroków symulacji.
    unsigned long	get_current_step() const {return Licznik;}

    /// Co ile kroków symulacji zapisuje na wyjœcie.
    void			set_log_ratio(unsigned ratio){LogRatio=ratio;}
    /// Ustawia `InputRatio`, czyli co ile kroków symulacji sprawdzaæ wejœcie.
    void			set_input_ratio(unsigned ratio){InputRatio=ratio;}

    /// Aktualny zarz¹dca ekranu pod³¹czony do tego œwiata.
    area_menager&	MyAreaMenager();
    /// Sprawdzenie, czy ma ju¿ pod³¹czonego zarz¹dcê okien.
    int 			HasAreaMenager() { return AreaMenager!=nullptr; }
    ///Jak trzeba KONIECZNIE coœ dopisaæ do logu.
    ostream&		MyLogStream();

    // G³ówne akcje œwiata:
    // ////////////////////

    /// Przygotowuje stan startowy symulacji.
    /// Jeœli pierwszy raz to
    ///     - wywo³uje `initialise_leyers()`
    ///     - tworzy bazowe Ÿród³a
    ///     - i opcjonalnie podstawowe grafy
    /// jeœli z `Replay!=0` to inicjalizuje warstwy z zerowego kroku
    /// pliku historii za pomoc¹ funkcji "initialize_from_image".
    void		initialize(main_area_menager* Menager=nullptr,int Replay=0);

    /// Powtórzenie inicjalizacji dla powtórnego przebiegu symulacji.
    /// Nie odtwarza strony wizualizacyjnej.
    void		restart();

    /// Wczytanie pojedynczego obrazu symulacji np. dla inicjalizacji.
    /// TODO CHECK... Jeœli nie ma nazwy to z nazwy `OutName`
    void		initialize_from_image(const char* FileName=nullptr);

    /// Wykonuje kolejny(e) krok(i) symulacji.
    /// Opakowuje "inteligentnie" `simulate_one_step`.
    /// @param Steps okreœla ile prostych kroków nale¿y wykonaæ.
    void		simulate(unsigned Steps=1);

    /// Prosta pêtla symulacyjna sprzê¿ona z wizualizacj¹.
    /// @param ret_after ???.
    void		simulation_loop(int ret_after);

    /// Pêtla wczytywania symulacji z pliku.
    /// @param ret_after ???.
    void		read_loop(int ret_after);

    ///Zapis na strumieñ. Opakowuje `implement_output`, ¿eby zapisaæ zawartoœæ.
    friend
    ostream& operator << (ostream& o,const world& w);

    ///Odczyt ze strumienia. Odpakowuje, u¿ywaj¹c `implement_input` ¿eby odczytaæ zawartoœæ.
    friend
    istream& operator >> (istream& i,world& w);

    ///Implementacja zapisu stanu symulacji w formacie NET lub NET2 (z atrybutami).
    virtual //Domyœlnie puste, nie wiem, czy kiedykolwiek u¿ywane.
    void dump_net_file(const char* core_name,unsigned long Step){}
};

// Lokalne implementacje `inline`:
//-///////////////////////////////

inline
ostream& world::MyLogStream()
//Dostêp do strumienia logu (¿eby coœ "rêcznie" dopisaæ)
{
    return Log.GetStream();
}

inline
area_menager&		world::MyAreaMenager()
//Aktualny zarz¹dca ekranu pod³¹czony do œwiata
{
                                                assert(AreaMenager!=nullptr);
    return *AreaMenager;
}

#endif // !defined(WORLD_HPP_INCLUDED_)
/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zak³ad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Spo³ecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */



