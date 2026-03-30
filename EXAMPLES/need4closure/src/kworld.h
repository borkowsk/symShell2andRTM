//DECLARATION OF   W O R L D  FOR "need 4 closure" SIMULATION
/////////////////////////////////////////////////////////
#include <limits.h> //SHRT_MAX
#include "world.hpp"
#include "layer.hpp"
#include "kagent.h" //Definicja agenta

class kworld:public world	//Caly swiat symulacji
//--------------------------------------------------
{
//Parametry jednowartosciowe
/////////////////////////////////
size_t				MyWidth;	//Obwod torusa
short				MaxSila;	//Maksymalna sila agenta
short				Treshold;	//Treshold sily powyzej ktorego nie ma zmian
short				IleSasiad;	//8==Gestosc sasiedztwa
//short				OdlSasiad;	//Rozmiar sasiedztwa
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
rectangle_layer_of_ptr_to_agents<kagent> Agenci;  //Wlaœciwa warstwa agentow zasiedlajacych

//Glowne serie - wygodniej miec wskazniki niz odszukiwac z Sources po nazwach
////////////////////////////////////////////////////////////////////////////////
ptr_to_struct_matrix_source<kagent,short>		*Firsts; //=Agenci.make_source("First mem",&kagent::First);		
ptr_to_struct_matrix_source<kagent,short>		*Seconds;//=Agenci.make_source("Second mem",&kagent::Second);

ptr_to_struct_matrix_source<kagent,short>		*Powers; //=Agenci.make_source("Power",&kagent::Power);
ptr_to_struct_matrix_source<kagent,unsigned>    *ForLeft; //=Agenci.make_source("Power",&kagent::ForLeft);
ptr_to_struct_matrix_source<kagent,unsigned>    *ForRight; //=Agenci.make_source("Power",&kagent::ForRight);

//ptr_to_struct_matrix_source<kagent,short>		*Pressure;// =Agenci.make_source("Pressure",&kagent::Press);
//method_by_ptr_matrix_source<kagent,long>		*Classif;//=Agenci.make_source("Classification",&kagent::Classif);

scalar_source<double>*       ptrStres;          //Do przekazywania aktualnie najwazniejszych danych na okno statusu
scalar_source<double>*       ptrClsSize;

int  CountCh;    //Ilu ostatnio zmienilo poglad - do celow statystyki
int  CountMig;   //Ilu ostatnio migrowalo - do celow statystyki
    
ptr_to_scalar_source<int>*       ptrLastChanged;          //Do przekazywania licznikow zmian
ptr_to_scalar_source<int>*       ptrLastMigration;        

double MaxPressure;//Do zapamietania teoretycznie najwiekszej wartosci "presji"

//Wlasciwa implementacja symulacji
int CheckChange(const rectangle_geometry* MyGeom,size_t index,kagent& CenterAgent);
int DoMigration(const rectangle_geometry* MyGeom,size_t index,kagent& CenterAgent);
						
public:
//KONSTRUKCJA DESTRUKCJA
kworld(size_t Width,		//Szerokosc torusa macierzy agentow
	  char* log_name,	//Nazwa pliku do zapisywania histori
	  char* mapl_name,	//Nazwa (bit)mapy inicjujacej "skladowe"
	  char* mapp_name,	//Nazwa (bit)mapy inicjujacej "sily"
	  char* live_mask,	//Czarne w tej mapie sa kasowane
	  double noise,		//Szum informacyjny
	  short	max_sila,	//Maksymalna sila agenta

	  short	ile_sasiad, //8==Gestosc sasiedztwa
	  double need_use_self,//Z jaka waga ma brac siebie
	  double need_for_something,//Z jaka waga brac innych
	  bool	synchronicly,

	  short treshold,
	  double spontanic,
      
      double fill,
      double migrprob,
      double majority,
      double minority
	  );	

~kworld(){}

protected:
//AKCJE
void	initialize_layers();	//Stan startowy symulacji
void	after_read_from_image();//actions after read state from file. Aktualizacja pol static kagent'a!!!
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



/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*                                                                  */
/*      Instytut Studiow Spolecznych Uniwersytetu Warszawskiego     */
/*                                                                  */
/*        WWW:  http://www.iss.uw.edu.pl/~borkowsk                  */
/*        MAIL: borkowsk@samba.iss.uw.edu.pl                        */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
