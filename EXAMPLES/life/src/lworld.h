/// @file
/// DECLARATION OF   W O R L D  FOR "Conways Life" SIMULATION.
// ///////////////////////////////////////////////////////////
/// @date 2026-03-26 (modified)

#include <limits.h> //SHRT_MAX
#include "world.hpp"
#include "layer.hpp"
#include "lagent.h" //Definicja agenta

/// Cały świat symulacji Life.
class lifeworld:public world
//--------------------------------------------------
{
// Parametry jednowartościowe:
// ///////////////////////////

size_t				MyWidth;	//!< Obwod torusa
short				IleKate;	//!< Ilosc kategori w mapach
short				IleSasiad;	//!< 8 == Gestosc sasiedztwa
short				OdlSasiad;	//!< Rozmiar sasiedztwa
short				BierzWszystko; //!< Czy ma brac wszystko z sasiedztwa

double				Noise;		//!< Szum informacyjny
bool				Synchronic; //!< Synchroniczna zmiana "pogladow"
wb_pchar			MaplName;	//!< Nazwa pliku inicjujacej bitmapy


// Warstwy symulacji (są torusami):
// ////////////////////////////////

//rectangle_unilayer<unsigned char> zdatnosc; //!< Warstwa definiujaca zdatnosc do zasiedlenia
rectangle_layer_of_agents<lifeagent> Agenci;  //!< Właściwa warstwa agentów zasiedlajacych

// Główne serie — wygodniej miec wskaźniki niż odszukiwać z Sources po nazwach:
// //////////////////////////////////////////////////////////////////////////////

struct_matrix_source<lifeagent,short>		*Firsts;//=Agenci.make_source("First mem",&lifeagent::First);		
struct_matrix_source<lifeagent,short>		*Seconds;//=Agenci.make_source("Second mem",&lifeagent::Second);


int CheckChange(const geometry_base* MyGeom,size_t index,lifeagent& CenterAgent);

public:
//KONSTRUKCJA DESTRUKCJA
lifeworld(
	  size_t Width,	//Szerokość torusa / macierzy agentów
	  char* log_name,	//Nazwa pliku do zapisywania historii
	  char* mapl_name,	//Nazwa (bit)mapy inicjującej "składowe"
	  double Noise=0,
	  short	ile_kate=2,		//Liczba kategorii w mapach
	  short	odl_sasiad=1,	//Rozmiar sąsiedztwa
	  short	ile_sasiad=8,	//8 == Gęstość sąsiedztwa — jeśli -1 to wszystko po kolei
	  bool	synchronicly=true,
	  double spontanic=0	//Prawdopodobieństwo spontanicznej zmiany "stanu"
	  );	

~lifeworld(){}

protected:
//AKCJE
void	initialize_layers();	//Stan startowy symulacji
void	after_read_from_image();//Actions after read state from file. Aktualizacja pol static lifeagent'a!!!
void	simulate_one_step();	//Właściwa implementacja kroku symulacji

//Wspolpraca z menagerem wyswietlania
//---------------------------------------------
void	make_default_visualisation() override; //Tworzy domyślne "lufciki" i umieszcza w zarządcy lufcików ustawionym w world::initialise(...)
//void actualize_out_area(); // aktualizacja zawartości `OutArea` po `n` krokach symulacji

//Generuje podstawowe zrodla dla wbudowanego manager-a danych lub innego
void	make_basic_sources(sources_menager& WhatSourMen);

//Implementacja wejścia/wyjścia. Zwracaj 1 jeśli sukces!
virtual
int		implement_output(ostream& o) const;
virtual
int		implement_input(istream& i);
};

// /////////////////////////////////////////////
// Example for SYMSHELL library.
/// @author Wojciech Borkowski, iss.uw.edu.pl
// /////////////////////////////////////////////
