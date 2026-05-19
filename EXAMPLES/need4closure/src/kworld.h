/// @file
/// @brief
///     @EN{ DECLARATION OF THE WORLD FOR "need 4 closure" SIMULATION. }
///     @PL{  }
/// @date 2026-05-19 (modified)
/// ====================================================================================================================
/// @details
/// (old example for SymShell implementing Kruglanskis like model)
//
#pragma once
#include <climits> //SHRT_MAX
#include "world.hpp"
#include "layer.hpp"
#include "kagent.h" //Definicja agenta

/// Cały świat symulacji "need for closure".
class kworld:public sym2::world
//--------------------------------------------------
{
    // Single-value model parameters:
    // //////////////////////////////

    size_t			MyWidth;		///< Obwód torusa.
    short			MaxSila;		///< Maksymalna siła agenta.
    short			Treshold;		///< Próg siły powyżej którego nie ma zmian.
    short			IleSasiad;		///< 8 == Gęstość sąsiedztwa.
    //short			OdlSasiad;		///< Rozmiar sasiedztwa.
    double			WeightOfSelf;	///< Z jaka waga brac siebie pod uwage (0..1).
    double			NeedForClosure;	///< Znaczenie może byc różne, zależnie od implementacji.
    double			Noise;			///< Szum informacyjny.
    double			Fill;			///< Udział żywych na początku.
    double			Migr;			///< Prawdopodobieństwo migracji.
    bool			Synchronic;		///< Synchroniczna zmiana poglądów.
    wb_pchar		MappName;		///< Nazwa pliku inicjującej bitmapy.
    wb_pchar		MaplName;		///< Nazwa pliku inicjującej bitmapy.
    wb_pchar		MaskName;		///< Nazwa pliku inicjującego maskę zdatności (?).

// Simulation layers (are tori):
// /////////////////////////////

    //rectangle_unilayer<unsigned char> zdatnosc; //Warstwa definiująca zdatność do zasiedlenia
    sym2::rectangle_layer_of_ptr_to_agents<kagent> Agenci;		///< Właściwa warstwa agentów zasiedlających.

// Main data series. It's convenient to have pointers rather than searching for them in Sources by name:
// /////////////////////////////////////////////////////////////////////////////////////////////////////

    sym2::data::ptr_to_struct_matrix_source<kagent,short>		*Firsts;	///< =Agenci.make_source("First mem",&kagent::First);
    sym2::data::ptr_to_struct_matrix_source<kagent,short>		*Seconds;	///< =Agenci.make_source("Second mem",&kagent::Second);

    sym2::data::ptr_to_struct_matrix_source<kagent,short>		*Powers;	///< =Agenci.make_source("Power",&kagent::Power);
    sym2::data::ptr_to_struct_matrix_source<kagent,unsigned>    *ForLeft;	///< =Agenci.make_source("Power",&kagent::ForLeft);
    sym2::data::ptr_to_struct_matrix_source<kagent,unsigned>    *ForRight;	///< =Agenci.make_source("Power",&kagent::ForRight);

//ptr_to_struct_matrix_source<kagent,short>		*Pressure;	///<  =Agenci.make_source("Pressure",&kagent::Press);
//method_by_ptr_matrix_source<kagent,long>		*Classify;	///< =Agenci.make_source("Classification",&kagent::classif);

    sym2::data::scalar_source<double>*       ptrStres;	///< Do przekazywania aktualnie najważniejszych danych na okno statusu.
    sym2::data::scalar_source<double>*       ptrClsSize;

int  CountCh;	///< Ilu ostatnio zmieniło pogląd. Do celów statystyki.
int  CountMig;	///< Ilu ostatnio migrowało. Do celów statystyki, o ile jest zaimplementowana migracja.

    sym2::data::ptr_to_scalar_source<int>*       ptrLastChanged;	///< Do przekazywania liczników zmian.
    sym2::data::ptr_to_scalar_source<int>*       ptrLastMigration;	///< Do przekazywania liczników migracji.

double MaxPressure; 	///< Do zapamiętania teoretycznie największej wartości "presji".

// Here is the actual simulation implementation:
// /////////////////////////////////////////////

/// Zmiana stanów.
int CheckChange(const sym2::rectangle_geometry* MyGeom, size_t index, kagent& CenterAgent);

/// Ewentualna migracja.
int DoMigration(const  sym2::rectangle_geometry* MyGeom, size_t index, kagent& CenterAgent);

public:
// CONSTRUCTION AND DESTRUCTION:
// /////////////////////////////

kworld(size_t Width,		///< Szerokość torusa macierzy agentów.
      char* log_name,		///< Nazwa pliku do zapisywania historii.
      char* mapl_name,		///< Nazwa (bit)mapy inicjującej "składowe".
      char* mapp_name,		///< Nazwa (bit)mapy inicjującej "siły".
      char* live_mask,		///< Czarne w tej mapie są kasowane.
      double noise,			///< Szum informacyjny.
      short	max_sila,		///< Maksymalna sila agenta.

      short	how_many_neib,		///< 8 == Gęstość sąsiedztwa.
      double need_use_self,	///< Z jaką wagą ma brać siebie (0,1,2...?).
      double need_for_something,	///< Z jaka wagą brać innych.
      bool	synchronicly,

      short treshold,
      double spontanic,
      
      double fill,
      double migrprob,
      double majority,
      double minority
      );

~kworld() override = default;

protected:

// TYPICAL ACTIONS OF EACH SIMULATION:
// ///////////////////////////////////

void	initialize_layers() override;	///< Stan startowy symulacji.
void	after_read_from_image() override;	///< Actions after read state from file. TAKŻE aktualizacja pól static kagent'a!!!
void	simulate_one_step() override;	///< Właściwa implementacja kroku symulacji.

// Collaboration with the area manager:
//-------------------------------------

/// Tworzy domyślne "lufciki" i umieszcza na liście zarządcy.
void	make_default_visualisation() override;
/// Wypisywanie/dopisywanie na konsole statusu. Aktualizacja zawartości `OutArea` zwykle co `n` kroków symulacji.
void	actualize_out_area() override;

/// Generuje podstawowe źródła dla wbudowanego manager-a danych lub innego
void	make_basic_sources() override;

// Input/output implementation:
//-----------------------------
int		implement_output(ostream& o) const override; ///< @returns 1, gdy sukces!
int		implement_input(istream& i) override; ///< @returns 1, gdy sukces!
};

/* ****************************************************************** */
/*        SYMSHELL2 EXAMPLE  version 2006/2022/2026                   */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */


