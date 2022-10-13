/**
* @defgroup	TypesService Informacja o typach i nazewnictwo
* @brief	Ładne i przenośne nazwy typów itp.
* @details  
*	Informacje o nazwach typów
* 	--------------------------
* 	  To w C++ zawsze było problemem! 
* 	Nawet jeżeli gdy istniała obsługa informacji o typach, to ich nazwy
* 	były zalezne od kompilatora, i często mało czytelne dla człowieka.
* 	Dziwne rzeczy dzieją się też z typami prostymi.
*          
*	  Te problemy pozostał do dziś, stąd przydatność własnej obsługi nazw 
* 	typów.
*
* 	Informacja o limitach
* 	---------------------
*	  Podobnym problemem była informacja o limitach wartości typów 
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
* 	  Powstało w czasach gdy tylko niektóre kompilatory C++
* 	miały obsługę wyjątków, dlatego w założeniu cała hierarchia 
* 	mogła akbo zgłaszać wyjątki, albo obsługiwać je lokalnie
* 	jako fatalne błędy wykonania, ewentualnie ostrzeżenia.  
*
*
* @defgroup	DYNMEMORY   Zarządzanie danymi na stercie
* @brief	Najprostsze zarządzanie danymi na stercie dbające o brak wycieków. 
* @details   
* 	Inteligentne wskaźniki i proste dynamiczne tablice i macierze
*	-------------------------------------------------------------
* 	  Trochę na wzór wczesnego STL lub jakiś pomysłów Strostroupa, ale całkiem
* 	inne i raczej mało kompatybilne.
*
* 	  Zawartość wskazywana jest "sztefetowana" pomiędzy obiektami powyższych typów,
* 	co oznacza, że w konstruktorze kopiującym albo przypisaniu jest przenoszona,
* 	a nie kopiowana. Obiekt donor staje się PUSTY! Dlatego obiekty te do funkcji
* 	muszą być zawsze przekazywane przez REFERENCJE!
* 	
*
* @defgroup	VirtualConstruction Wirtualne konstruktory dla obiektów heterogenicznych
* @brief	Narzędzia do definiowania wirtualnych konstruktorów.
* 		
*
* ---------------------------------------------------------------------------------------------------------------
* 	
* @copyright Wojciech Borkowski wborkowski (_at_) uw.edu.pl
* @date 2022-10-13 (last modification)
* 
*/	
