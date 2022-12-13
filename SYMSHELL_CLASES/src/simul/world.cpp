// world.cpp: implementation of the world class.
//
//*////////////////////////////////////////////////////////////////////
#include "world.hpp"
#include "wb_cpucl.hpp"

bool world::continous_dump=false;

int	world::set_simulation_name(const char* name)
//zwraca 1 jak sie udalo. Moze byc niedozwolone
{
	SimulName=name;
	return 1;
}

//Ustawianie strumienia do zapisu histori
int	world::set_history_stream(const char* name)
{
	if( int(OutName) && strcmp(name,OutName)!=0 && Out)
		Out.dispose();
	OutName=name;
	return 0;
}

//inline
void world::make_default_visualisation()
{
	if(!HasAreaMenager())
				return; //SKORO NIE MA MENAGERA TO NIE MA CO ROBI�

	//UTWORZENIE LUFCIKA NA INFORMACJE
	OutArea=new text_area(0,0,8*80,25*16,
		"Initialising in progress..."
		,
		default_white,default_black,128,40);

	if(!OutArea) goto ERROR;
		OutArea->settitle("STATUS");
	this->MyAreaMenager().insert(/*dynamic_cast<drawable_base*>*/(OutArea));
	//MyAreaMenager().insert(OutArea);//!!! Tak powinno dzialac! ANSI ???
	//MyAreaMenager().insert(wb_ptr<drawable_base>(OutArea)); //???Dziala, ale to jest inny konstruktor

ERROR://...akcja na niepogode
	;//error_message(...)
}

//inline
void world::initialize(main_area_menager* Menager,int Replay)
// stan startowy symulacji
{
    wb_cpu_clock timer;

	if(Replay==0)
		initialize_layers();	//Inicjalizacja danych uzytkownika
		else
		initialize_from_image();//Inicjalizacja z wycinka histori lub z histori

	if(Sources.get(0)==NULL)//jeszcze nie inicjowano zrodel
		make_basic_sources();//umieszcza je we wlasnym menagerze danych
		else
		{
		Sources.new_data_version();
		Licznik=0;
		}

	if(AreaMenager==NULL )//Nie by�o jeszcze wskaznika do Menagera ekranu
	{                                   //Ale o ile zosta� dostarczony to czy jest ju� zainicjalizowany?
		if(Menager!=NULL && Menager->is_initialised() )
				AreaMenager=Menager;
	}

	make_default_visualisation();
					//Tworzy z�o�one �r�d�a danych i definiuje log a potem
					//O ILE JEST DOST�PNY MyManager obszar�w/lufcik�w
					//umieszcza w nim domyslne wizualizacje

	if(OutArea!=NULL)
	{
		OutArea->add_text(
				"HINTS:\n"
				"Press ctrl-I to view/hide the HELP window,\n"
				"Press arrows to scroll windows,\n"
				"Press ctrl-B to start/stop symulation.\n"
				);
		OutArea->add_text("Initialisation completed.");
	}

	{
	time_t ltime;
	time( &ltime );
	StartTime=clone_str(ctime( &ltime ));
	StartTime[strlen(StartTime)-1]='\0';//Kasuje \n
	TimeStamp=clone_str(StartTime.get());
	}

    ClockTime.alloc(200);       //Alokacja - az za duzo
    ClockTime.prn("Time of the initialisation : %g s\n",double(timer)/InputRatio);
    if(OutArea!=NULL)
			OutArea->add_text(ClockTime.get());
}

void world::restart()
{
	Log.try_writing();//Zapis ostatniego stanu symulacji
	Licznik=0;

	Sources.restart_data_version(); //Bedzie zniszczenie i odbudowa od nowa
	Log.restart_data_version();		//Wymuszenie dla zle zarzadzanych podzrodel
	initialize_layers();			//�UBUDUBUDU!!!!
	Sources.new_data_version();		//Nowy swiat po potopie

	if(OutArea!=NULL)
	{
		OutArea->add_text("Reinitialisation completed.");
	}
	{
	time_t ltime;
	time( &ltime );
	StartTime=clone_str(ctime( &ltime ));
	StartTime[strlen(StartTime)-1]='\0';//Kasuje \n
	TimeStamp=clone_str(StartTime.get());
	}
}

//inline
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

			if(int(OutName) && (*OutName)!='\0' && !int(Out) )//Jesli jest niepusta nazwa a nie ma strumienia
			{

				Out.set(
					new fstream(OutName.get(),ios::out|ios::app
#ifdef __MSVC_2000__
						,_SH_SECURE //zamiast ios_base::_Openprot
//Albo kiedy� by�o:
//								filebuf::sh_read |
//								filebuf::sh_write
#endif
					));//Jak to w UNIXie - wszystko wolno

				if(!int(Out) || !(*Out))
				{
					cerr<<"Can't open "<<OutName<<" for no-replaced output."<<endl;
					Out.dispose();
				}
			}

			if(int(Out))		//Zapis calego stanu "swiata", je�li jest na co
			{
				(*Out)<<(*this);
				Out->flush();
			}

			if(continous_dump)
			{
				actualize_out_area();		//Aktualizacja informacji
				AreaMenager->_replot();		//Wizualizacja
				AreaMenager->flush();
				//I zapis do obrazu pliku
				wb_pchar Buf;
				Buf.alloc(100);
				const char* pom=this->DumpNetName.get_ptr_val();
				if(pom==NULL) pom="D_";
				Buf.prn("%s%06u",pom,this->get_current_step());
				dump_screen(Buf.get_ptr_val());
			}
		}

		simulate_one_step();//Wlasciwa symulaca

		Licznik++;
		i++;
        if(Steps>2)
            cerr<<'\r'<<Licznik<<' '<<i<<"                         ";//Zeby bylo wiadomo ze cos robi

		Sources.new_data_version(1,1);//Oznajmia seriom ze dane sie uaktualnily

		{   //Marker czasu
			time_t ltime;
			time( &ltime );
			TimeStamp=clone_str(ctime( &ltime ));
			TimeStamp[strlen(TimeStamp)-1]='\0';//Kasuje \n
		}

        if(Steps>2 && AreaMenager)//O ile tryb rzadkiej wizualizacji i sa w ogole podlaczne lufciki to...
        {
            if(!AreaMenager->background_enabled())//Jesli wstrzymany i praca krok po kroku to by bylo bez wizualizacji
            {
                actualize_out_area();		//Aktualizacja informacji
                AreaMenager->_replot();			//Wizualizacja
                AreaMenager->flush();
            }
            AreaMenager->process_input();//Obsluga zdarzen zewnetrznych	zeby okno nie bylo martwe
            if(!AreaMenager->should_continue() )	//CZY PRZYPADKIEM NIE KONIEC W DZIWNYM MOMENCIE?
            {
                OutArea->add_text("User break the simulation.");
                if(!AreaMenager->background_enabled())//Przy takim przerwaniu trzeba by i tak jeszcze raz zamykac okno
                {
                    OutArea->add_text("The simulation loop was stopped in interactive mode."
                                      "\nYou can inspect simulation and continue");
                    AreaMenager->need_break_action(0);
                }
                OutArea->replot();
				break;//Koniec zabawy!!!
            }
        }

	}
	while(i<Steps);

}
//inline
void world::actualize_out_area()
// aktualizacja zawartosci OutArea po n krokach symulacji
{
	if(OutArea)
	{
		wb_pchar bufor(1024);//ze sporym zapsem
		bufor.prn("%lu SYMULATION STEP. %s\n",(unsigned long)get_current_step(),ClockTime.get());
		OutArea->clean();
		OutArea->add_text(bufor.get_ptr_val());
		if(AreaMenager!=NULL && (!AreaMenager->background_enabled()))
			OutArea->add_text("INTERACTIVE MODE (ctrl-B: switch to auto mode)\n");
	}
}


//inline
void world::simulation_loop(int ret_after)
{

	if(AreaMenager && !AreaMenager->should_continue())
			return; //NIESTETY OD RAZU KONIEC!

	//Wstepne dzialania
	////////////////////
	if(AreaMenager)
	{
//		AreaMenager->enable_background();//Dla pewnosci
		actualize_out_area();			 //Aktualizacja informacji
		if(get_current_step()==0)
		{
			AreaMenager->flush();
			AreaMenager->process_input();//Obsluga zdarzen zewnetrznych przed startem - jakby co...
		}
	}

	wb_cpu_clock looptime;//Czas wykonania kroku petli
	do{
		//CZY DALEJ SYMULUJEMY?
		if(get_current_step()>=MaxIterations )
		{
			if(ret_after || AreaMenager==NULL)
			{
				break;//KONIEC PETLI
			}
			else
			{
				AreaMenager->disable_background();	//WYLACZA SYMULOWANIE
				MaxIterations=LONG_MAX;
			}
		}

		//Obsluga okna przed krokiem symulacji
		////////////////////////////////////////
		if(AreaMenager)
		{
			actualize_out_area();		//Aktualizacja informacji
			AreaMenager->_replot();			//Wizualizacja
			AreaMenager->flush();
		}

		//Wlasciwa symulacja
		//////////////////////////////////////
		if(AreaMenager==NULL || AreaMenager->background_enabled()) //Jak w og�le nie ma Menagera albo praca w tle jest dozwolona
		{
			wb_cpu_clock timer;         //Automatyczny start timera kroku
			//-----------------------------
			simulate(InputRatio);		//Kolejne N krok�w symulacji
			//------------------------------
			ClockTime.alloc(100);       //Alokacja - az za duzo
			ClockTime.prn("\n  Step time: %g s\n  Loop time: %g s",double(timer)/InputRatio,double(looptime));
		}

		looptime.reset(); //Drobne oszustwo - obsluga zdarzen zewnetrznych zostanie doliczona do czasu nastepnego kroku

		//Obsluga okna po kroku symulacji lub bez niej
		////////////////////////////////////////////////
		if(AreaMenager)
		{
			AreaMenager->process_input();//Obsluga zdarzen zewnetrznych
			if(!AreaMenager->should_continue() )	//CZY NIE KONIEC?
					break;//Koniec zabawy!!!
		}

	}while(1);
}

void world::make_basic_sources()
//Umieszcza serie danych w swoim menagerze serii danych
{
	//Zerowa seria w menagerze danych powinna byc pusta - sluzy
	//do kontroli wersji danych. Menager moze tworzyc ja sam,
	//ale zawsze mozna potem podmienic
	ptr_to_scalar_source<unsigned long>* sca=new ptr_to_scalar_source<unsigned long>(&Licznik,"Step:");
	if(!sca) goto ERROR;
    Sources.replace(size_t(0),sca);
ERROR://...akcja na niepogode TODO
	;//error_message(...)
}

void   world::make_basic_sources(sources_menager& WhatSourMen)
//NA RAZIE NIE WOLNO TAKIEJ FUNKCJI! Pomys� zewn�trznego menagera danych okaza� si� niesprwadzony
{
	assert("Never use: world::make_basic_sources(sources_menager& WhatSourMen) !"==NULL);
}

/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/


