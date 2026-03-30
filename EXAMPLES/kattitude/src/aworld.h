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
double				WeightOfSelf;//Z jaka waga brac siebie pod uwage (0..1)
double				NeedForClosure;//Znaczenie moze byc rozne, zaleznie od implementacji
double				Noise;		//Szum informacyjny
double              Fill;       //Udzial zywych na poczatku
double              Migr;       //Prawdopodobienstwo migracji
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
ptr_to_struct_matrix_source<aagent,short>		*Firsts; //=Agenci.make_source("First mem",&aagent::First);		
ptr_to_struct_matrix_source<aagent,short>		*Seconds;//=Agenci.make_source("Second mem",&aagent::Second);

ptr_to_struct_matrix_source<aagent,short>		*Powers; //=Agenci.make_source("Power",&aagent::Power);
ptr_to_struct_matrix_source<aagent,short>		*Pressure;// =Agenci.make_source("Pressure",&aagent::Press);
//method_by_ptr_matrix_source<aagent,long>		*Classif;//=Agenci.make_source("Classification",&aagent::Classif);

scalar_source<double>*       ptrStres;          //Do przekazywania aktualnie najwazniejszych danych na okno statusu
scalar_source<double>*       ptrClsSize;

int  CountCh;    //Ilu ostatnio zmienilo poglad - do celow statystyki
int  CountMig;   //Ilu ostatnio migrowalo - do celow statystyki
    
ptr_to_scalar_source<int>*       ptrLastChanged;          //Do przekazywania licznikow zmian
ptr_to_scalar_source<int>*       ptrLastMigration;        

double MaxPressure;//Do zapamietania teoretycznie najwiekszej wartosci "presji"

//Wlasciwa implementacja symulacji
int CheckChange(const rectangle_geometry* MyGeom,size_t index,aagent& CenterAgent);
int DoMigration(const rectangle_geometry* MyGeom,size_t index,aagent& CenterAgent);
						
public:
//KONSTRUKCJA DESTRUKCJA
aworld(size_t Width,		//Szerokosc torusa macierzy agentow
	  char* log_name,	//Nazwa pliku do zapisywania histori
	  char* mapl_name,	//Nazwa (bit)mapy inicjujacej "skladowe"
	  char* mapp_name,	//Nazwa (bit)mapy inicjujacej "sily"
	  char* live_mask,	//Czarne w tej mapie sa kasowane
	  double noise,		//Szum informacyjny
	  short	max_sila,	//Maksymalna sila agenta
	  short	ile_kate,	//Ilosc kategori w mapach	
	  short	odl_sasiad,	//Rozmiar sasiedztwa
	  short	ile_sasiad, //8==Gestosc sasiedztwa
	  double need_use_self,//Z jaka waga ma brac siebie
	  double need_for_something,//Z jaka waga brac innych
	  bool	synchronicly,
	  short walkpower,
	  short trespower,
	  double spontanic,
      double fill,
      double migrprob,
      double majority
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

//Wypisywanie/dopisywanie na konsole statusu
void    actualize_out_area(); 

//Implementacja wejscia/wyjscia. Zwracaj 1 jesli sukces!
virtual
int		implement_output(ostream& o) const;
virtual
int		implement_input(istream& i);
};
