/// @file world.cpp
/// implementation of the world-class.
// ////////////////////////////////////////////////////////////////////
/// @date 2026-05-04 (modified)

#include "world.hpp"
#include "wb_cpucl.hpp"

bool world::continous_dump=false;

int	world::set_simulation_name(const char* name)
//Zwraca 1 jak się udało. Może byc niedozwolone
{
    SimulName=name;
    return 1;
}


int	world::set_history_stream(const char* name)
//Ustawianie strumienia do zapisu historii
{
    if( int(OutName) && strcmp(name,OutName)!=0 && Out)
        Out.dispose();
    OutName=name;
    return 0;
}


void world::make_default_visualisation()
//Tworzenie obowiązkowego lufcika na informacje tekstowe.
{
    if(!HasAreaMenager()) {//SKORO NIE MA MANAGERA TO NIE MA CO ROBIĆ, ALE TO JEST BŁĄÐ!
        cerr<<"Visualization manager not connected - default graphs could not be created."<<endl;
        goto ERROR;
    }

    //UTWORZENIE OBOWIĄZKOWEGO LUFCIKA NA INFORMACJE TEKSTOWE.
    OutArea=new text_area(0,0,8*80,25*16,
        "Initialising in progress..."
        ,
        default_white,default_black,128,40);

   // if(!OutArea) goto ERROR; //To się już nie może zdarzyć od C++11

    OutArea->set_title("STATUS");

    this->MyAreaMenager().insert(/*dynamic_cast<drawable_base*>*/(OutArea));
    //MyAreaMenager().insert(OutArea);//!!! Tak powinno dzialac! ANSI ???
    //MyAreaMenager().insert(wb_ptr<drawable_base>(OutArea)); //???Dziala, ale to jest inny konstruktor

ERROR://...akcja na niepogode
    ;//error_message(...) ???
}


void world::initialize(main_area_menager* Menager,int Replay)
// Ustawia stan startowy symulacji.
{
    wb_cpu_clock timer;

    if(Replay==0)
        initialize_layers();	 //Inicjalizacja danych świata klasy potomnej
        else
        initialize_from_image(); //Inicjalizacja z wycinka historii lub z historii

    if(Sources.get(0)==nullptr)
        { //jeszcze nie inicjowano źródeł danych
        make_basic_sources();        // tworzy je i umieszcza we własnym zarządcy danych
        } else { //Manager już jest wypełniony seriami
        Sources.new_data_version();  //... więc tylko oznakowujemy, że to start nowej symulacji
        Licznik=0;
        }

    if(AreaMenager==nullptr ) //Nie było jeszcze ustawionego wskaźnika do zarządcy ekranu
    {
        //O ile został teraz dostarczony i zainicjalizowany to go ustawiamy.
        if(Menager!=nullptr && Menager->is_initialised() )
                AreaMenager=Menager;
    }

    make_default_visualisation();
                    //O ILE JEST DOSTĘPNY MyManager obszarów/lufcików
                    //to umieszcza w nim domyślne wizualizacje

    if(OutArea!=nullptr)
    {
        OutArea->add_text(
                "HINTS:\n"
                "Press ctrl-I to view/hide the HELP window,\n"
                "Press arrows to scroll windows,\n"
                "Press ctrl-B to start/stop simulation.\n"
                );
        OutArea->add_text("Initialisation completed.");
    }

    { //Przypisanie TimeStamp-u
        time_t ltime;
        time( &ltime );
        StartTime=clone_str(ctime( &ltime ));
        StartTime[strlen(StartTime)-1]='\0';//Kasuje \n
        TimeStamp=clone_str(StartTime.get());
    }

    ClockTime.alloc(200);	//Alokacja — aż za dużo
    ClockTime.prn("Time of the initialization: %g s\n",double(timer)/InputRatio);
    if(OutArea!=nullptr)
            OutArea->add_text(ClockTime.get());
}

void world::restart()
{
    Log.try_writing();				//Zapis ostatniego stanu symulacji
    Licznik=0;

    Sources.restart_data_version(); //Będzie zniszczenie i odbudowa od nowa
    Log.restart_data_version();		//Wymuszenie dla źle zarządzanych pod-źródeł danych
    initialize_layers();			//PONOWNA INICJALIZACJA ŚWIATA
    Sources.new_data_version();		//Nowy świat po potopie

    if(OutArea!=nullptr)
    {
        OutArea->add_text("Reinitialisation completed.");
    }

    { //Przypisanie TimeStamp-u
        time_t ltime;
        time( &ltime );
        StartTime=clone_str(ctime( &ltime ));
        StartTime[strlen(StartTime)-1]='\0';//Kasuje \n
        TimeStamp=clone_str(StartTime.get());
    }
}


void world::simulate(unsigned Steps)
// kolejny(e) krok(i) symulacji
{
    unsigned i=0,pom;
    if(Steps>2)
        cerr<<endl;
    do
    {
        if((pom=(Licznik%LogRatio))==0)
        {
            Log.try_writing();	//Na wszelki wypadek?

            if(int(DumpNetName))
                dump_net_file(DumpNetName.get(),Licznik);

            if(int(OutName) && (*OutName)!='\0'
            && !int(Out) //Jeżeli jest niepusta nazwa, ale nie ma strumienia
            )
            {

                Out.set(
                    new fstream(OutName.get(),ios::out|ios::app
#ifdef __MSVC_2000__
                        ,_SH_SECURE //zamiast ios_base::_Openprot
//Albo kiedyś było:
//								filebuf::sh_read |
//								filebuf::sh_write
#else
//								Jak to w UNIXie — wszystko wolno
#endif
                     ));

                if(!int(Out) || !(*Out))
                {
                    cerr<<"Can't open "<<OutName<<" for no-replaced output."<<endl;
                    Out.dispose();
                }
            }

            if(int(Out))		//Zapis całego stanu "świata", jeżeli jest na co (strumień Out jest utworzony)
            {
                (*Out)<<(*this);
                Out->flush();
            }

            if(continous_dump)
            {
                actualize_out_area();		//Aktualizacja informacji
                AreaMenager->_replot();		//Wizualizacja
                AreaMenager->flush();

                //I zapis do obrazu okna do pliku graficznego
                wb_pchar Buf;
                Buf.alloc(100);
                const char* pom=this->DumpNetName.get_ptr_val();
                if(pom==NULL) pom="D_";
                Buf.prn("%s%06u",pom,this->get_current_step());
                dump_screen(Buf.get_ptr_val());
            }
        }

        simulate_one_step(); //Właściwa symulacja

        Licznik++; //Ogólny licznik kroków
        i++;       //Lokalny licznik kroków

        if(Steps>2)
            cerr<<'\r'<<Licznik<<'\t'<<i<<"                         "; //Żeby było wiadomo, że coś wciąż robi

        Sources.new_data_version(1,1); //Oznajmia seriom danych, że dane się w layer-ach uaktualniły

        {   //Marker czasu
            time_t ltime;
            time( &ltime );
            TimeStamp=clone_str(ctime( &ltime ));
            TimeStamp[strlen(TimeStamp)-1]='\0'; //To kasuje końcowe \n
        }

        if(Steps>2 && AreaMenager) //O ile tryb rzadkiej wizualizacji i są w ogóle podłączone lufciki to...
        {
            if(!AreaMenager->background_enabled()) //Jeśli wstrzymany i praca krok po kroku to by było bez wizualizacji
            {
                actualize_out_area();		//Aktualizacja informacji
                AreaMenager->_replot();		//Wizualizacja
                AreaMenager->flush();
            }

            AreaMenager->process_input(); //Obsługa zdarzeń zewnętrznych, żeby okno nie było martwe

            if(!AreaMenager->should_continue() )	//CZY PRZYPADKIEM NIE KONIEC W DZIWNYM MOMENCIE?
            {
                OutArea->add_text("The user interrupted the simulation.");
                if(!AreaMenager->background_enabled()) //Przy takim przerwaniu trzeba by i tak jeszcze raz zamykać okno
                {
                    OutArea->add_text("The simulation loop was stopped in interactive mode."
                                      "\nYou can inspect simulation and continue");
                    AreaMenager->need_break_action(0);
                }
                OutArea->replot();
                break; //Koniec zabawy!!!
            }
        }

    }
    while(i<Steps);

}


void world::actualize_out_area()
// aktualizacja zawartości `OutArea` po `n` krokach symulacji
{
    if(OutArea)
    {
        wb_pchar bufor(1024); //ze sporym zapasem
        bufor.prn("%lu SIMULATION STEP. %s\n",(unsigned long)get_current_step(),ClockTime.get());
        OutArea->clean();
        OutArea->add_text(bufor.get_ptr_val());
        if(AreaMenager!=nullptr && (!AreaMenager->background_enabled()))
            OutArea->add_text("IN INTERACTIVE MODE (ctrl-B: switch to auto mode)\n");
    }
}


void world::simulation_loop(int ret_after)
{
    if(AreaMenager && !AreaMenager->should_continue()) {
        cerr<<"Visualization manager isn't connected and/or continuation is not possible."<<endl;
        return; //NIESTETY OD RAZU KONIEC!
    }

    // Wstępne działania:
    // //////////////////
    if(AreaMenager)
    {
//      AreaMenager->enable_background();	//Dla pewności?
        actualize_out_area();				//Aktualizacja informacji
        if(get_current_step()==0)
        {
            AreaMenager->flush();
            AreaMenager->process_input();	//Obsługa zdarzeń zewnętrznych przed startem... Jakby coś.
        }
    }

    wb_cpu_clock looptime;			///< Czas wykonania kroku pętli.
    do{
        //CZY DALEJ SYMULUJEMY?
        if(get_current_step()>=MaxIterations )
        {
            if(ret_after || AreaMenager==NULL)
            {
                break; //KONIEC PĘTLI
            }
            else
            {
                AreaMenager->disable_background();	//WYŁĄCZA SYMULOWANIE
                MaxIterations=LONG_MAX;
            }
        }

        // Obsługa okna przed krokiem symulacji:
        // /////////////////////////////////////
        if(AreaMenager)
        {
            actualize_out_area();		//Aktualizacja informacji
            AreaMenager->_replot();		//Wizualizacja
            AreaMenager->flush();
        }

        // Właściwa symulacja:
        // ///////////////////

        //Jak w ogóle nie ma zarządcy wizualizacji (why!?) albo praca w tle jest dozwolona:
        if(AreaMenager==nullptr || AreaMenager->background_enabled())
        {
            wb_cpu_clock timer;				///< Timer kroku od razu automatycznie startujący.
            //-----------------------------
            simulate(InputRatio);		//Kolejne N kroków symulacji
            //------------------------------
            ClockTime.alloc(100);		//Alokacja string-u.
            ClockTime.prn("\n  Step time: %g s\n  Loop time: %g s",double(timer)/InputRatio,double(looptime));
        }

        looptime.reset(); //Drobne oszustwo — obsługa zdarzeń zewnętrznych zostanie doliczona
                          // do czasu następnego kroku

        // Obsługa okna po kroku symulacji lub bez niej:
        // /////////////////////////////////////////////
        if(AreaMenager)
        {
            AreaMenager->process_input();			//Obsługa zdarzeń zewnętrznych
            if(!AreaMenager->should_continue() )	//CZY NIE KONIEC?
                    break;	//Koniec zabawy!!!
        }

    }while(true);
}

void world::make_basic_sources()
//Umieszcza serie danych w swoim zarządcy serii danych
{
    //Zerowa seria w zarządcy danych powinna byc pusta, gdyż służy
    //do kontroli wersji danych.
    //Zarządca może tworzyć ją sam, ale zawsze można potem podmienić.
    ptr_to_scalar_source<unsigned long>* sca=new ptr_to_scalar_source<unsigned long>(&Licznik,"Step:");
    // if(!sca) goto ERROR; //Od roku 2011 to już niemożliwe.
    Sources.replace(size_t(0),sca);
}

void   world::make_basic_sources(sources_menager& WhatSourMen)
//NA RAZIE NIE WOLNO TAKIEJ FUNKCJI! Pomysł zewnętrznego manager-a danych okazał się nieudany.
{
    assert("Never use: world::make_basic_sources(sources_menager& WhatSourMen) !"==nullptr);
}

world::~world() {
    //TODO - Tu jest błąd (ERROR)
    //Jeśli jakieś źródło zarejestrowane w logu nie jest używane w wizualizacji
    //to destruktor próbuje obliczać wartości ze zniszczonego już świata!!!
    //TODO SPRAWDZIĆ!
    Log.try_writing(); //Stan końcowy, jeśli nie byl zapisany  */
}

/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */


