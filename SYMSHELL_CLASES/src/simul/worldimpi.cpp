/// @file
/// @brief **Implementation of the world type -- Virtual input methods and operator >>  **
/// @date 2026-05-20 (modified)
//===----------------------------------------------------------------------------
/// @details
///     Its is rather simply implementation. You can replace it if you need.
//======================================================================================================================

//#include "INCLUDE/platform.hpp"
#include <fstream>

#include "world.hpp"

using namespace std;
using namespace sym2;
using namespace sym2::shell;

int world::implement_input(istream& i)
{
    i >> StepCounter;
    i>>TimeStamp;
    if(!int(StartTime))
        StartTime=clone_str(TimeStamp.get());
    i>>MaxIterations;
    i>>LogRatio;
    if(i.fail())
    {
        cerr<<"'world' I/O error."<<endl;
        return 0;
    }
    return 1;
}

// Wczytanie pojedynczego obrazu symulacji dla inicjalizacji.
// Jeśli nie ma nazwy to z nazwy `OutName`.
void world::initialize_from_image(const char* FileName)
{

    if(!FileName && !int(OutName))
    {
        cerr<<"An undefined name for reading."<<endl;
        return;
    }

    ifstream InFile(FileName?FileName:OutName.get()
                // ,	//Preferuje nazwę podana explicite
                        //ios::in|				//Otwiera do odczytu oczywiście
/*
#ifdef CHALA_____//NEW_FASHION_CPP
                        ios::_Nocreate
#else
                        ios::nocreate			//Nie tworzy, jeśli nie ma
#endif
#ifdef __MSVC_2000__
                        ,_SH_SECURE //zamiast ios_base::_Openprot
//Brak takich stałych, choć kiedyś były
                        ,filebuf::sh_none		//Nie czyta z otwartego i nic nie pozwala dopisywać w trakcie
#endif */
                        );

    if(!InFile || InFile.fail() )
    {
        cerr<<"Can't open: "<<OutName<<" for input."<<endl;
        return;
    }

    InFile>>(*this); //Właściwe wczytanie

    after_read_from_image(); //actions after read state from a file.

    //Niejawnie zamyka InFile w destruktorze
}

// pętla wczytywania symulacji
void world::read_loop(int ret_after)
{                            //Ta możliwość z dawna nie używana. TODO reaktywacja?
    if(!AreaManager->should_continue())
            return; //NIESTETY OD RAZU KONIEC!

    if(Sources.get(0)==NULL)		//jeśli jeszcze nie inicjowano źródeł
    {                        //TODO - nie wiadomo czy go w ogóle już ma?
                             //Ma zmienna	this->Sources więc chyba tak.
        make_basic_sources(); //umieszcza je we własnym managerze danych
    }

    if(!int(OutName))
    {
        cerr<<"Undefined name for reading."<<endl;
        return;
    }

    if(!Out)
    {
        Out.set(
            new fstream(OutName.get(),ios::in
/*
#ifdef NEW_FASHION_CPP
                        |   ios::_Nocreate
#else
                        |   ios::nocreate			//Nie tworzy, jeśli nie ma
#endif
#ifdef __MSVC_2000__
                        ,_SH_SECURE //zamiast ios_base::_Openprot
//Brak stałych choć kiedyś były
                        ,filebuf::sh_read		//Nie czyta z otwartego i nic nie pozwala dopisywać w trakcie
#endif
*/
                        ));

        if(!Out || !(*Out) )
        {
            cerr<<"Can't open: "<<OutName<<" for input."<<endl;
            return;
        }
    }

    // Wstępne działania
    //===================
    if(AreaManager)
    {
        AreaManager->enable_background(); //Dla pewności
//		actualize_out_area();			 //Aktualizacja informacji
    }

    do{
        //CZY DALEJ SYMULUJEMY?
        if(get_current_step()>=MaxIterations )
            if(ret_after || AreaManager==NULL)
                break; //KONIEC PĘTLI
            else
            {
                AreaManager->disable_background();	//WYŁĄCZA SYMULOWANIE
                MaxIterations=LONG_MAX;
            }

        // Wczytanie kolejnego kroku symulacji z zapisu
        //=======================================
        if(AreaManager==NULL || AreaManager->background_enabled())
        {
            unsigned long OldLicznik=StepCounter; //Dostęp do pola protected, fuj!

            char cpom;
            do{
                (*Out)>>cpom;
            }while(cpom=='\n'); //Ignoruje puste linie

            if(Out->eof())
                    break; //Gdyby były puste linie na końcu pliku
            Out->putback(cpom);	   //A jeśli nie to oddajemy znak
            (*Out)>>(*this);
            after_read_from_image(); //actions after read state from a file.

            if(StepCounter != OldLicznik)	//Bo na początku wczytuje już wczytany stan z kroku 0
                Sources.new_data_version(1,1); //Oznajmia seriom, że dane się uaktualniły
        }

        // Obsługa okna przed krokiem symulacji
        //=======================================
        if(AreaManager)
        {
            actualize_out_area();		//Aktualizacja informacji
            AreaManager->_replot();			//Wizualizacja
            AreaManager->flush();
        }

        // Obsługa okna po kroku symulacji lub bez niej
        //===============================================
        if(AreaManager)
        {
            AreaManager->process_input(); //Obsługa zdarzeń zewnętrznych
            if(!AreaManager->should_continue() )	//CZY NIE KONIEC?
                    break; //Koniec zabawy!!!
        }

    }while(!Out->eof());
}

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

