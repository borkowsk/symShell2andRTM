/// @file
/// @brief
///  @EN{ DECLARATION OF THE "W O R L D" ('aworld') FOR "attitude" SIMULATION. }
///  @PL{ DEKLARACJA "ŚWIATA" ('aworld') NA RZECZ SYMULACJI POSTAW "attitude". }
/// @date 2026-05-21 (modified)
/// =========================================================
/// @PL{ DOXYGENIZACJA WYŁĄCZNIE PO POLSKU. }
/// @EN{ DOXYGENIZATION IN POLISH ONLY. }
//======================================================================================================================

#include <climits> //SHRT_MAX
#include "world.hpp"
#include "layer.hpp"
#include "aagent.h" //Definicja agenta

using namespace sym2::data;
using namespace sym2::shell;
using namespace sym2::visual;

/// @brief @PL{ Cały świat symulacji "attitudes".}
///        @EN{ A whole world of "attitudes" simulations. }
class attitude_world: public sym2::shell::world
//---------------------------------------------
{
    // Parametry modelu:
    // /////////////////
    size_t				 MyWidth;	///< Obwód torusa.
    short				 MaxSila;	///< Maksymalna siła agenta.
    short			ThresholdStr;	///< Próg siły, powyżej którego nie ma zmian.
    short			NofCategories;	///< Liczba kategorii w mapach.
    short				NofNeigh;	///< 8 == Gęstość sąsiedztwa (ilu sąsiadów losujemy albo bierzemy wszystkich gdy -1).
    short			 NeighRadius;	///< Rozmiar sąsiedztwa.
    short				 TakeAll;	///< Określa, czy ma brać wszystko z sąsiedztwa.
    short				 UseSelf;	///< Określa, czy ma brać siębie pod uwagę.
    double				   Noise;	///< Szum informacyjny.
    bool			  SyncChange;	///< Synchroniczna (lub nie) zmiana poglądów.
    wb_pchar			MappName;	///< Nazwa pliku inicjującej bitmapy.
    wb_pchar			MapLName;	///< Nazwa pliku inicjującej bitmapy.
    wb_pchar			MaskName;	///< Nazwa pliku inicjującej bitmapy.

    /// @brief Warstwa symulacji, czyli warstwa agentów zasiedlających (torus).
    rectangle_layer_of_ptr_to_agents<attitude_agent>    Agents;

    /// @name Główne serie — wygodniej mieć wskaźniki niż odszukiwać z Sources po nazwach.
    // ///////////////////////////////////////////////////////////////////////////////////
    /// @{
    sym2::data::ptr_to_struct_matrix_source<attitude_agent,short>		*Firsts; //=Agents.make_source("First mem",&aagent::First);
    sym2::data::ptr_to_struct_matrix_source<attitude_agent,short>		*Seconds; //=Agents.make_source("Second mem",&aagent::Second);
    sym2::data::ptr_to_struct_matrix_source<attitude_agent,short>		*Powers; //=Agents.make_source("Power",&aagent::Power);
    /// @}

    int CheckChange(const geometry_base* MyGeom, size_t index, attitude_agent& CenterAgent);

public:
    // KONSTRUKCJA DESTRUKCJA:
    // ///////////////////////

    /// Konstruktor.
    attitude_world(size_t	Width,				///< Szerokość torusa macierzy agentów.
            char*	log_name,			///< Nazwa pliku do zapisywania historii.
            char*	map_l_name,			///< Nazwa (bit-) mapy inicjującej "składowe".
            char*	mapp_name,			///< Nazwa (bit-) mapy inicjującej "sily".
            char*	live_mask,			///< Czarne w tej mapie są kasowane.
            double	noise_p=0,
                   short	max_str=255,		///< Maksymalna siła agenta.
            short	n_of_cate=256,		///< Liczba kategorii w mapach.
            short	neigh_radius=1,		///< Rozmiar sąsiedztwa.
            short	n_of_neigh=8,		///< 8 == Gęstość sąsiedztwa — jeśli -1, to wszystko po kolei.
            short	need_use_self=0,	///< Określa, czy własny stary pogląd jest brany pod uwagę przy wyborze.
            bool	sync_update=true,	///< Określa, czy update agentów jest syn chroniczny, czy Monte Carlo.
            short	walk_str=0,			///< Określa, czy siła rośnie z wiekiem agenta.
            short	str_threshold=SHRT_MAX,	///< Sila, powyżej której agent jest odporny na wpływ.
            double	spon_prob=0			///< Prawdopodobieństwo spontanicznej zmiany poglądu.
          );

    /// Destruktor.
    ~attitude_world() override = default;

protected:
    // AKCJE:
    // //////
    void	initialize_layers() override;	///< Stan startowy symulacji.
    void	after_read_from_image() override;	///< Akcja po odczytaniu stanu z pliku. Aktualizacja pól `static` agenta!!!
    void	simulate_one_step() override;	///< Właściwa implementacja kroku symulacji.

    // Współpraca z managerem wyświetlania i zarządcą źródeł danych:
    //--------------------------------------------------------------

    /// Tworzy domyślne "lufciki" i umieszcza na głównym obszarze (w zarządcy wyświetlania).
    void	make_default_visualisation() override;

    /// Generuje podstawowe źródła dla wbudowanego manager-a danych lub innego.
    void	make_basic_sources() override;

    //void actualize_out_area();	///< aktualizacja zawartości `OutArea` po "n." krokach symulacji.

    /// @name Implementacja wejścia/wyjścia.
    /// @details Zwracają 1, jeśli sukces!
    /// @{
    int		implement_output(ostream& o) const override;
    int		implement_input(istream& i) override;
    /// @}
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

