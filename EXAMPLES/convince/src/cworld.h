//DECLARATION OF   W O R L D  FOR "attitudeS" SIMULATION
/////////////////////////////////////////////////////////
#include <limits.h> //SHRT_MAX
#include "world.hpp"
#include "layer.hpp"
#include "cagent.h" //Definicja agenta

class aWorld:public world	//Caly swiat symulacji
//--------------------------------------------------
{
//Parametry jednowartosciowe
/////////////////////////////////
size_t				MyWidth;	//Obwod torusa
bool				Synchronic; //Synchroniczna zmiana pogladow
wb_pchar			MappName;	//nazwa pliku inicjujacej bitmapy
wb_pchar			MaplName;	//nazwa pliku inicjujacej bitmapy
wb_pchar			MaskName;	//nazwa pliku inicjujacej bitmapy

//Warstwy symulacji (sa torusami)
/////////////////////////////////
//rectangle_unilayer<unsigned char> zdatnosc;//Warstwa definiujaca zdatnosc do zasiedlenia
rectangle_layer_of_ptr_to_agents<anAgent> Agenci;  //Wlaœciwa warstwa agentow zasiedlajacych

//Glowne serie - wygodniej miec wskazniki niz odszukiwac z Sources po nazwach
////////////////////////////////////////////////////////////////////////////////
ptr_to_struct_matrix_source<anAgent,short>		*Firsts;//=Agenci.make_source("First mem",&anAgent::First);		
ptr_to_struct_matrix_source<anAgent,short>		*Seconds;//=Agenci.make_source("Second mem",&anAgent::Second);

ptr_to_struct_matrix_source<anAgent,short>		*Powers;//=Agenci.make_source("Power",&anAgent::Power);
//method_by_ptr_matrix_source<anAgent,long>		*Classif;//=Agenci.make_source("Classification",&anAgent::Classif);

int CheckChange(const geometry_base* MyGeom,size_t index,anAgent& CenterAgent);

public:
//KONSTRUKCJA DESTRUKCJA
	aWorld(
		unsigned iWidth,		//Szerokosc torusa macierzy agentow		
		double iToBeNewProb=0.1,//Prawd. spontanicznej zmiany pogladow samotnika na nowy typ rozrywki/sportu
		double iReverseProb=0.9,//Prawd. reversji pogladow na 0 - brak pomyslu na rozrywke
		double iSupportLevel=0.5,//Sila wsparcia gdy ma jakies towarzystwo	
		char* ilog_name="convince.log",		//Nazwa pliku do zapisywania histori
		char* imapl_name=NULL,	//Nazwa (bit)mapy inicjujacej "skladowe"
		char* imapp_name=NULL,	//Nazwa (bit)mapy inicjujacej "sily"
		char* ilive_mask=NULL,	//Czarne w tej mapie sa kasowane  		   			
		short imax_sila=100,	//Maksymalna sila agenta
		short imin_sila=100	//Minimalna sila 
		);	

~aWorld(){}

protected:
//AKCJE
void	initialize_layers();	//Stan startowy symulacji
void	after_read_from_image();//actions after read state from file. Aktualizacja pol static anAgent'a!!!
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
