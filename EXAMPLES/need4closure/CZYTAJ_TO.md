# Need for Closure — opis modelu i programu

## Cel programu

`need4closure` to przykład symulacji agentowej napisany w C++ z użyciem biblioteki SymShell. Program implementuje uproszczony, inspirowany teorią Ariego Kruglanskiego model społecznego kształtowania i domykania przekonań. Jest to model poglądów na osi lewica–centrum–prawica, a nie pełna implementacja teorii psychologicznej.

Punktem wejścia jest `need4clos.cpp`. Plik ten odczytuje parametry, uruchamia okno graficzne i tworzy obiekt świata `kWorld`. Definicja agenta znajduje się w `kAgent.h`, a logika świata i kroku symulacji — w `kworld.cpp`. `kWorld.h` deklaruje strukturę świata i jego interfejs, natomiast `kvisual.cpp` przygotowuje źródła danych, wykresy i mapy.

## Model świata

Świat jest kwadratową siatką o wymiarach `WIDTH × WIDTH` (domyślnie `100 × 100`), z geometrią torusa: przeciwległe krawędzie siatki sąsiadują ze sobą. Komórka może zawierać agenta albo pozostać pusta. Początkowe wypełnienie można ograniczyć parametrem `FILL`.

Początkowy rozkład postaw jest losowany z prawdopodobieństw zadanych przez `ILEF` i `IRIG`; pozostała część agentów zaczyna jako niezdecydowana. Domyślne wartości to odpowiednio 5% i 2,5%. Są to udziały początkowe, a nie gwarantowane liczebności dokładne. Stan agentów można też inicjalizować z bitmap: osobno dla siły (`MAPP`) i postaw/liczników argumentów (`MAPL`); czarne pola maski `MASK` usuwają agentów z siatki.

## Agent i jego stan

`kAgent` dziedziczy po `agent_base`. Każdy agent przechowuje:

- `First` — aktualną postawę: `-1` (lewa), `0` (neutralna), `1` (prawa);
- `Second` — poprzednią lub przygotowaną postawę, wykorzystywaną przy aktualizacji i wizualizacji;
- `Power` — indywidualną siłę, losowaną przy tworzeniu w zakresie od 1 do maksymalnej siły;
- `ForLeft` i `ForRight` — liczniki/siły argumentów za lewą i prawą postawą;
- `DurCh` — znacznik, że zmiana postawy została wyliczona i oczekuje na zastosowanie.

Na początku postawa jest losowana według parametrów `ILEF` i `IRIG`, a oba liczniki argumentów otrzymują losowe wartości. W trakcie zmiany `new_attitude()` zapisuje proponowaną postawę, zaś `update()` przełącza ją na aktualną. Puste komórki są reprezentowane przez niepoprawnego agenta (`Power == -1`).

## Reguła oddziaływania i zmiany postawy

W każdym kroku świat odwiedza agentów i dla każdego aktywnego agenta losuje partnerów z sąsiedztwa. Liczbę próbek kontroluje `PRTR`, a parametr `NFOC` przekazywany jest do generatora sąsiedztwa jako skala jego zasięgu. Puste komórki, pozycje poza geometrią i sam agent są pomijane.

Wpływ partnera zależy od jego siły, parametru potrzeby domknięcia oraz odległości. W `CheckChange()` kod wylicza go w postaci `Power^NFOC / √distance`; wkład własny agenta jest ważony przez `SELF`. Agent o postawie `-1` lub `0` wnosi argument `ForLeft`, a agent o postawie `1` lub `0` — `ForRight`. Zatem agent neutralny może wspierać obie strony. Zebrane wkłady są łączone z dotychczasowymi licznikami, po czym przekształcane potęgą zależną od `NFOC`.

Jeżeli bezwzględna różnica między licznikami przekracza `TRSP` (próg, domyślnie 100), agent przyjmuje postawę strony z większym licznikiem. W przeciwnym razie zachowuje dotychczasową postawę. Warto odróżnić ten próg zmiany od parametru `NFOC`, który kształtuje zasięg i sposób ważenia wpływów.

Dostępne są dwa porządki aktualizacji:

- **synchroniczny** (`SYNC=Y`) — najpierw obliczane są propozycje zmian wszystkich agentów, a dopiero w drugiej przebiegowej pętli zmiany są wprowadzane;
- **Monte Carlo** (`SYNC=N`) — agenci są wybierani losowo, a wyliczona zmiana jest stosowana od razu; w takim przebiegu agent może zostać wylosowany ponownie.

## Przebieg programu i wyniki

`main()` w `need4clos.cpp` wykonuje następujące czynności:

1. Parsuje parametry uruchomienia i inicjalizuje menedżera okna SymShell.
2. Tworzy `kWorld`, inicjalizuje generator losowy i ustawia limity iteracji oraz częstotliwości wizualizacji i zapisu.
3. Inicjalizuje warstwy agentów oraz wizualizację, a następnie uruchamia pętlę symulacji. W trybie `REPL=...` zamiast nowej symulacji odtwarza zapisaną historię.
4. Po zakończeniu zwalnia obiekt świata.

Domyślnie symulacja ma bardzo duży limit kroków (`MAX`), więc może działać do ręcznego przerwania. `STOP=Y` kończy ją po osiągnięciu limitu. `AUTO=N` uruchamia serię automatycznych powtórzeń; między powtórzeniami świat jest restartowany.

Okno prezentuje mapę aktualnych i poprzednich postaw, mapy liczników argumentów oraz siły, histogram postaw i wykresy historii m.in. liczby klas, entropii, zmian i przybliżonej wielkości klastrów. Dane statystyczne i historię można zapisywać do plików wskazanych parametrami `LOGF` i `HIST`.

## Najważniejsze parametry uruchomienia

Opcje podaje się jako argumenty `NAZWA=WARTOŚĆ`; wielkość liter nie ma znaczenia. Najważniejsze z nich:

| Parametr | Znaczenie |
| --- | --- |
| `WIDTH=N` | Bok siatki świata. Domyślnie 100. |
| `FILL=x` | Początkowy udział zajętych pól; wartość większa od 1 jest traktowana jako procent. |
| `ILEF=x`, `IRIG=x` | Początkowe udziały postaw lewicowej i prawicowej; wartość większa od 1 jest traktowana jako procent. |
| `NFOC=x` | Parametr potrzeby domknięcia, wpływający na zasięg/ważenie oddziaływań. |
| `SELF=x` | Waga własnego wkładu agenta. Domyślnie 9. |
| `PRTR=N` | Liczba losowanych partnerów; kod dopuszcza także `PRTR=-1` jako wariant „wszyscy w sąsiedztwie”. |
| `TRSP=N` | Próg różnicy liczników wymagany do zmiany postawy. |
| `MPOW=N` | Maksymalna siła przy inicjalizacji agentów. |
| `SYNC=Y/N` | Synchroniczny albo Monte Carlo porządek aktualizacji. |
| `MAX=N`, `STOP=Y/N` | Limit kroków i decyzja o zakończeniu po jego osiągnięciu. |
| `VIEW=N`, `LOGC=N` | Częstotliwość odświeżania wizualizacji i zapisu logu. |
| `LOGF=plik`, `HIST=plik`, `REPL=plik` | Plik logu, plik historii lub odtwarzanie historii. |
| `MAPL=plik`, `MAPP=plik`, `MASK=plik` | Bitmapy inicjalizacyjne i maska zajętości. |
| `NOIP=N`, `SPCH=N`, `PMIG=N` | Parametry szumu, spontanicznych zmian i migracji dostępne w interfejsie programu. |

### Uwagi o implementacji

Nie wszystkie parametry opisane przez parser są widoczne w regule zmiany postawy. W szczególności `Noise`, `MutationProb` i `MigrationP` są przechowywane/przekazywane, ale w bieżącym kodzie `CheckChange()` nie widać ich zastosowania; migracja jest też oznaczona w deklaracji świata jako nieużywana. Nie należy więc zakładać, że zmiana tych opcji wpływa na dynamikę bez sprawdzenia lub uzupełnienia implementacji. Podobnie komunikat pomocy wypisuje `VIEV`, natomiast parser rozpoznaje `VIEW=`.

## Główne pliki katalogu `src/`

- `need4clos.cpp` — punkt wejścia, parametry aplikacji i sterowanie przebiegiem.
- `kAgent.h` — stan, inicjalizacja i operacje agenta.
- `kWorld.h` — deklaracja świata, parametrów i warstwy agentów.
- `kworld.cpp` — inicjalizacja świata, logika jednego kroku i reguła oddziaływań.
- `kvisual.cpp` — serie danych, statystyki, wykresy i mapy.

## Zakres interpretacji

Opis reguł powyżej odzwierciedla kod źródłowy tego przykładu. Jest to model uproszczony i nie należy utożsamiać parametrów ani mechanizmu aktualizacji z pełną, empirycznie zweryfikowaną formalizacją teorii potrzeby domknięcia poznawczego.
