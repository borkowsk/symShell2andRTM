//DECLARATION OF   W O R L D  FOR "attitudeS" SIMULATION
/////////////////////////////////////////////////////////
#include <limits.h> //SHRT_MAX
#include "world.hpp"
#include "layer.hpp"
#include "aagent.h" //Definicja agenta

class aworld:public world	//Caly swiat symulacji
//--------------------------------------------------
{
//Parametry jednowartosciowe
/////////////////////////////////
size_t				MyWidth;	//Obwod torusa
short				MaxSila;	//Maksymalna sila agenta
short				TrsSila;	//Treshold sily powyzej ktorego nie ma zmian
short				IleKate;	//Ilosc kategori w mapach
short				IleSasiad;	//8==Gestosc sasiedztwa
short				OdlSasiad;	//Rozmiar sasiedztwa
short				BierzWszystko;//Czy ma brac wszystko z sasiedztwa
short				UseSelf;	//Czy ma brac siebie pod uwage
double				Noise;		//Szum informacyjny
bool				Synchronic; //Synchroniczna zmiana pogladow
wb_pchar			MappName;	//nazwa pliku inicjujacej bitmapy
wb_pchar			MaplName;	//nazwa pliku inicjujacej bitmapy
wb_pchar			MaskName;	//nazwa pliku inicjujacej bitmapy

//Warstwy symulacji (sa torusami)
/////////////////////////////////
//rectangle_unilayer<unsigned char> zdatnosc;//Warstwa definiujaca zdatnosc do zasiedlenia
rectangle_layer_of_ptr_to_agents<aagent> Agenci;  //Wlaœciwa warstwa agentow zasiedlajacych

//Glowne serie - wygodniej miec wskazniki niz odszukiwac z Sources po nazwach
////////////////////////////////////////////////////////////////////////////////
ptr_to_struct_matrix_source<aagent,short>		*Firsts;//=Agenci.make_source("First mem",&aagent::First);		
ptr_to_struct_matrix_source<aagent,short>		*Seconds;//=Agenci.make_source("Second mem",&aagent::Second);

ptr_to_struct_matrix_source<aagent,short>		*Powers;//=Agenci.make_source("Power",&aagent::Power);
//method_by_ptr_matrix_source<aagent,long>		*Classif;//=Agenci.make_source("Classification",&aagent::Classif);

int CheckChange(const geometry_base* MyGeom,size_t index,aagent& CenterAgent);

public:
//KONSTRUKCJA DESTRUKCJA
aworld(size_t Width,	//Szerokosc torusa macierzy agentow
	  char* log_name,	//Nazwa pliku do zapisywania histori
	  char* mapl_name,	//Nazwa (bit)mapy inicjujacej "skladowe"
	  char* mapp_name,	//Nazwa (bit)mapy inicjujacej "sily"
	  char* live_mask,	//Czarne w tej mapie sa kasowane
	  double Noise=0,
	  short	max_sila=255,//Maksymalna sila agenta
	  short	ile_kate=256,//Ilosc kategori w mapach	
	  short	odl_sasiad=1,//Rozmiar sasiedztwa
	  short	ile_sasiad=8, //8==Gestosc sasiedztwa	- jesli -1 to wszystko po kolei
	  short need_use_self=0,
	  bool	synchronicly=true,
	  short walkpower=0,	//Czy sila rosnie z wiekiem agenta 
	  short trespower=SHRT_MAX,  //Sila powyzej ktorej agent jest odporny na wplyw
	  double spontanic=0	//Prawdopodobienstwo spontanicznej zmiany pogladu
	  );	

~aworld(){}

protected:
//AKCJE
void	initialize_layers();	//Stan startowy symulacji
void	after_read_from_image();//actions after read state from file. Aktualizacja pol static aagent'a!!!
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
