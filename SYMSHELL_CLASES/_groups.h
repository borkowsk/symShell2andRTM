/** @file
 * @brief **DOXYGEN GROUP DEFINITIONS FOR "DATACLSS"** /<br>
 *         _DEFINICJE GRUP DOXYGENA DLA "DATACLSS"_
 * @date 2026-05-16 (modified)
 ** \if POLSKI
 * @defgroup	GRUPA_WZORCOWA Tytuł dla grupy wzorcowej
 * @brief	Przykład jak definiować grupę
 * @details
 *	ŁADNY TYTUŁ
 * 	-----------
 * 	Jakiś rozbudowany tekst opisujący tę grupę.
 *
 ** \endif
 ** \if ENGLISH
 * @defgroup	GRUPA_WZORCOWA Title for the model group
 * @brief	An example of how to define a group
 * @details
 *	NICE TITLE
 * 	-----------
 * 	Some extensive text describing this group.
 *
 ** \endif
 *
 * @defgroup	GRUPA_DATACLSS System źródeł danych
 * @brief	Różne źródła danych bazujące na wspólnym interfejsie i z możliwościami czerpania od siebie.
 * @details
 *	WSTĘP DO IDEI ŹRÓDEŁ DANYCH
 *	---------------------------
 * 	  .....
 * 	  .....
 *
 *  PODSTAWOWY INTERFEJS
 *  --------------------
 *  Cała hierarchia startuje z abstrakcyjnej klasy `data_source_base`, która definiuje też cały wspólny interfejs.
 *  Jednak niektóre klasy źródeł mają też własne metody uwzględniające ich specyficzny charakter — np. dwuwymiarowość
 *  udostępnianych danych.
 *
 *  ...
 *
 *  ŹRÓDŁA POCHODNE I ŁĄCZENIE ŹRÓDEŁ
 *  ---------------------------------
 *
 *  ....
 *  ....
 *
 * ---------------------------------------------------------------------------------------------------------------
 */
 /// Symshell2 library.
namespace sym2{
    /// Data sources hierarchy.
    namespace data {}
    /// Graphic areas hierarchy.
    namespace visual {}
    /// World, layers and agents.
    namespace shell{}
}
 /**
 * @copyright Wojciech Borkowski wborkowski (_at_) uw.edu.pl
 *
 */
