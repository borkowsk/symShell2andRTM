//DECLARATION OF   W O R L D  FOR "attitudeS" SIMULATION
/////////////////////////////////////////////////////////
#include <limits.h> //SHRT_MAX
#include "world.hpp"
#include "layer.hpp"
#include "lagent.h" //Definicja agenta

class lifeworld:public world	//Caly swiat symulacji
//--------------------------------------------------
{
//Parametry jednowartosciowe
/////////////////////////////////
size_t				MyWidth;	//Obwod torusa
short				IleKate;	//Ilosc kategori w mapach
short				IleSasiad;	//8==Gestosc sasiedztwa
short				OdlSasiad;	//Rozmiar sasiedztwa
short				BierzWszystko;//Czy ma brac wszystko z sasiedztwa

double				Noise;		//Szum informacyjny
bool				Synchronic; //Synchroniczna zmiana pogladow
wb_pchar			MaplName;	//nazwa pliku inicjujacej bitmapy


//Warstwy symulacji (sa torusami)
/////////////////////////////////
//rectangle_unilayer<unsigned char> zdatnosc;//Warstwa definiujaca zdatnosc do zasiedlenia
rectangle_layer_of_agents<lifeagent> Agenci;  //Wlaœciwa warstwa agentow zasiedlajacych

//Glowne serie - wygodniej miec wskazniki niz odszukiwac z Sources po nazwach
////////////////////////////////////////////////////////////////////////////////
struct_matrix_source<lifeagent,short>		*Firsts;//=Agenci.make_source("First mem",&lifeagent::First);		
struct_matrix_source<lifeagent,short>		*Seconds;//=Agenci.make_source("Second mem",&lifeagent::Second);


int CheckChange(const geometry_base* MyGeom,size_t index,lifeagent& CenterAgent);

public:
//KONSTRUKCJA DESTRUKCJA
lifeworld(
	  size_t Width,	//Szerokosc torusa macierzy agentow
	  char* log_name,	//Nazwa pliku do zapisywania histori
	  char* mapl_name,	//Nazwa (bit)mapy inicjujacej "skladowe"
	  double Noise=0,
	  short	ile_kate=2,		//Ilosc kategori w mapach	
	  short	odl_sasiad=1,	//Rozmiar sasiedztwa
	  short	ile_sasiad=8,	//8==Gestosc sasiedztwa	- jesli -1 to wszystko po kolei
	  bool	synchronicly=true,
	  double spontanic=0	//Prawdopodobienstwo spontanicznej zmiany pogladu
	  );	

~lifeworld(){}

protected:
//AKCJE
void	initialize_layers();	//Stan startowy symulacji
void	after_read_from_image();//actions after read state from file. Aktualizacja pol static lifeagent'a!!!
void	simulate_one_step();	//Wlasciwa implementacja kroku symulacji

//Wspolpraca z menagerem wyswietlania
//---------------------------------------------
void	make_default_visualisation(area_menager_base& Menager);//Tworzy domyslne "lufciki" i umieszcza w 
//void actualize_out_area(); // aktualizacja zawartosci OutArea po n krokach symulacji

//Generuje podstawowe zrodla dla wbudowanego menagera danych lub innego
void	make_basic_sources(sources_menager& WhatSourMen);

//Implementacja wejscia/wyjscia. Zwracaj 1 jesli sukces!
virtual
int		implement_output(ostream& o) const;
virtual
int		implement_input(istream& i);
};
