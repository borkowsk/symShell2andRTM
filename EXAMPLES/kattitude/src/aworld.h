/// @file
/// @brief DECLARATION OF THE WORLD FOR "attitudeS" SIMULATION (kattitude old example for SymShell)
/// @date 2026-05-07 (modified)
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include <limits.h> //SHRT_MAX
#pragma once

#include "world.hpp"
#include "layer.hpp"
#include "aagent.h" //Definicja agenta

/// Cały świat symulacji `kattitude`.
class aworld:public symshell2::world
//--------------------------------------------------
{
// Parametry jednowartościowe
// ///////////////////////////////
size_t				MyWidth;			//!< Obwód torusa.
short				MaxPower;			//!< Maksymalna sila agenta.
short				ThrPower;			//!< Threshold sily powyżej, którego nie ma zmian.
short				IleKate;			//!< Liczba kategorii w mapach.
short				NeiDens;			//!< 8 == Gęstość sąsiedztwa.
short				NeiSize;			//!< Rozmiar sąsiedztwa (Neighborhood)
short				TakeAll;			//!< Określa, czy ma brać wszystko z sąsiedztwa.
double				WeightOfSelf;		//!< Z jaką wagą brać siebie pod uwagę (0..1).
double				NeedForClosure;		//!< Znaczenie może być różne, zależnie od implementacji.
double				Noise;				//!< /Szum informacyjny.
double				LifeFill;			//!< Udział żywych na początku.
double				MigrProb;			//!< Prawdopodobieństwo migracji.
bool				Synchronic;			//!< Synchroniczna zmiana poglądów.
wb_pchar			MappName;			//!< Nazwa pliku bitmapy inicjującej siły.
wb_pchar			MaplName;			//!< Nazwa pliku bitmapy inicjującej stany.
wb_pchar			MaskName;			//!< Nazwa pliku bitmapy inicjującej maskę obszarów zdatnych.

// Warstwy symulacji (są torusami):
// ////////////////////////////////

//rectangle_unilayer<unsigned char> suitability;		//!< Warstwa definiująca zdatność do zasiedlenia (suitability or usefulness)
symshell2::rectangle_layer_of_ptr_to_agents<aagent> Agenci;		//!< Właściwa warstwa agentów zasiedlających.

// Główne serie danych. Wygodniej i efektywniej mieć wskaźniki niż odszukiwać z Sources po nazwach:
// ////////////////////////////////////////////////////////////////////////////////////////////////
ptr_to_struct_matrix_source<aagent,short>		*Firsts;		//!< =Agenci.make_source("First mem",&aagent::First);
ptr_to_struct_matrix_source<aagent,short>		*Seconds;		//!< =Agenci.make_source("Second mem",&aagent::Second);

ptr_to_struct_matrix_source<aagent,short>		*Powers;		//!< =Agenci.make_source("Power",&aagent::Power);
ptr_to_struct_matrix_source<aagent,short>		*Pressure;		//!< =Agenci.make_source("Pressure",&aagent::Press);
//method_by_ptr_matrix_source<aagent,long>		*Classif;		//!< =Agenci.make_source("Classification",&aagent::classif);

scalar_source<double>*       ptrStres;			//!< Do przekazywania aktualnie najważniejszych danych na okno statusu
scalar_source<double>*       ptrClsSize;		//!< ...

int  CountCh;			//!< Ilu ostatnio zmieniło pogląd (do celów statystycznych).
int  CountMig;			//!< Ilu ostatnio migrowało (do celów statystycznych).
    
ptr_to_scalar_source<int>*       ptrLastChanged;			//!< Do przekazywania liczników zmian.
ptr_to_scalar_source<int>*       ptrLastMigration;			//!< Do przekazywania liczników zmian.

double MaxPressure; //Do zapamiętania teoretycznie największej wartości "presji".

// Właściwa implementacja symulacji:
// /////////////////////////////////
int CheckChange(const symshell2::rectangle_geometry* MyGeom,size_t index,aagent& CenterAgent);		//!< Sprawdzenie zmiany stanów.
int DoMigration(const symshell2::rectangle_geometry* MyGeom,size_t index,aagent& CenterAgent);		//!< Sprawdzenie możliwości migracji.

public:
//KONSTRUKCJA DESTRUKCJA
aworld(size_t Width,			//!< Szerokość torusa macierzy agentów.
      char* log_name,			//!< Nazwa pliku do zapisywania historii.
      char* mapl_name,			//!< Nazwa bitmapy inicjującej "składowe".
      char* mapp_name,			//!< Nazwa bitmapy inicjującej "siły".
      char* live_mask,			//!< Czarne w tej mapie są kasowane.
      double noise,				//!< Szum informacyjny.
      short	max_sila,			//!< Maksymalna sila agenta.
      short	ile_kate,			//!< Liczba kategorii w mapach.
      short	nei_radius,			//!< Rozmiar sąsiedztwa.
      short	nei_density,			//!< 8 == Gęstość sąsiedztwa.
      double need_use_self,		//!< Z jaką wagą ma brać siebie.
      double need_for_something,	//!< Z jaka waga brać innych.
      bool	synchronously,		//!< ...
      short walk_power,			//!< ...
      short thr_power,		//!< ...
      double spontaneously,		//!< ...
      double fill,				//!< ...
      double migration_prob,	//!< ...
      double majority			//!< ...
      );

~aworld() override = default;

protected:
// KONIECZNE AKCJE:
// ////////////////
void	initialize_layers() override;		//!<Stan startowy symulacji
void	after_read_from_image() override;	//!<Actions after read state from a file. Aktualizacja pól statycznych aagent-a!!!
void	simulate_one_step() override;		//!<Właściwa implementacja kroku symulacji

// Współpraca z zarządcą wyświetlania i zarządcą danych:
//------------------------------------------------------

/// Tworzenie domyślnych "lufcików" i umieszczanie ich na liście zarządcy.
void	make_default_visualisation() override;

/// Wypisywanie/dopisywanie na konsole statusu.
void    actualize_out_area() override; //!< Aktualizacja zawartości `OutArea` co ileś kroków symulacji.

/// Generuje podstawowe źródła dla wbudowanego zarządcy danych.
void	make_basic_sources() override;

// Implementacja wejścia/wyjścia:
//-------------------------------
int		implement_output(ostream& o) const override;		//!< Serializacja. @returns 1, jeśli sukces!
int		implement_input(istream& i) override;				//!< Deserializacja. @returns 1, jeśli sukces!
};


/* **************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/* **************************************************************** */
