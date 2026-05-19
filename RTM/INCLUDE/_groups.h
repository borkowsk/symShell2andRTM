/** @file
 * @brief @PL{ DEFINICJE GRUP DOXYGENA DLA WB_RTM. }
 *        @EN{ ... }
 * @date 2026-05-19 (modified)
 * @details
 *      Cała biblioteka WB_RTM jest baaaardzo stara. Początki ma w latach 90. XX wieku.
 *      Trochę była poprawiana i podtrzymywana ze względu na stare aplikacje,
 *      których nie opłacało się "upgrejdować" do użycia __STL__ czy __boost__.
 *      Dziś to, poza kilkoma wciąż działającymi funkcjami, kompletne muzeum.
*/


/**
* @defgroup	TypesService Informacja o typach i nazewnictwo
* @brief	Ładne i przenośne nazwy typów itp.
* @details  
*	Informacje o nazwach typów
* 	--------------------------
 *
* 	To w C++ zawsze było problemem!
* 	Nawet jeżeli gdy istniała obsługa informacji o typach, to ich nazwy
* 	były zależne od kompilatora, i często mało czytelne dla człowieka.
* 	Dziwne rzeczy dzieją się też z typami prostymi.
*          
*	Te problemy pozostał do dziś, stąd przydatność własnej obsługi nazw
* 	typów.
*
* 	Informacja o limitach
* 	---------------------
 *
*	Podobnym problemem była informacja o limitach wartości typów
*	prostych. Dziś ten problem jest rozwiązany, ale już kwestia 
*	dobrania wartości dla "missing value" w różnych typach nadal 
*	nie jest oczywista i przenośna.
* 
*
* @defgroup	ERRORHANDLING Błędne i wyjątkowe sytuacje
* @brief	Obsługa wyjątków jako błędów czasu wykonania.
* @details 
*	Funkcje i klasy do obsługi sytuacji błędnych i wyjątkowych
*	----------------------------------------------------------
 *
* 	Powstało to w czasach gdy tylko niektóre kompilatory C++
* 	miały obsługę wyjątków, dlatego w założeniu cała hierarchia 
* 	mogła albo zgłaszać wyjątki, albo obsługiwać je lokalnie
* 	jako fatalne błędy wykonania, ewentualnie ostrzeżenia.
*
*
* @defgroup	DYNMEMORY   Zarządzanie danymi na stercie
* @brief	Najprostsze zarządzanie danymi na stercie, dbające o brak wycieków.
* @details   
* 	Inteligentne wskaźniki i proste dynamiczne tablice i macierze
*	-------------------------------------------------------------
 *
* 	Trochę na wzór wczesnego STL lub jakiś pomysłów Stroustrup'a, ale całkiem
* 	inne i raczej mało kompatybilne.
*
* 	Zawartość wskazywana jest "sztefetowana" pomiędzy obiektami powyższych typów,
* 	co oznacza, że w konstruktorze kopiującym albo przypisaniu jest przenoszona,
* 	a nie kopiowana. Obiekt donor staje się PUSTY! Dlatego obiekty te do funkcji
* 	muszą być zawsze przekazywane przez REFERENCJE!
* 	
*
* @defgroup	VirtualConstruction Wirtualne konstruktory dla obiektów heterogenicznych
* @brief	Narzędzia do definiowania wirtualnych konstruktorów.
* @details
*  OBSŁUGA ZAPISU OBIEKTÓW DO PLIKU TXT/OTX (serializacja)
*  -------------------------------------------------------
*  Serializacja i deserializacja wymaga nazw typów, które nie tylko są czytelne, ale też PRZENOŚNE między systemami.
 *  Ponadto w czasie gdy to rozwiązanie powstawało, zaledwie niektóre kompilatory posiadały nawet to niewielkie
 *  wsparcie dla nazw typów, jakie daje standard C++. Stąd cały pomysł opierał się na wymuszeniu istnienia pewnych
 *  funkcji wirtualnych w każdym typie (za pomocą makra) oraz tworzeniu podczas linkowania listy typów, którą
 *  można przeszukiwać LINIOWO po nazwach.
 *  Umożliwiało to deserializację heterogenicznych plików danych, choć spowolniona przez organizację tej listy,
 *  Obecnie programiści C++ mają do dyspozycji dużo lepsze alternatywy tego systemu.
*
* ---------------------------------------------------------------------------------------------------------------
*
* @copyright Wojciech Borkowski wborkowski (_at_) uw.edu.pl
* @date 2026-05-19 (last modification)
* 
*/
