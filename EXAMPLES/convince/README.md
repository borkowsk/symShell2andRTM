# Convince — symulacja zmian przekonań

`convince` jest przykładową, interaktywną symulacją agentową opartą na modelu opisywanym jako model D. Stauffera. Program działa na kwadratowej sieci agentów, gdzie każdy agent może pozostawać bez przekonania (`0`) albo reprezentować jedną z dwóch lub więcej opcji przekonania. Implementacja wykorzystuje SymShell/RTM i jest przeznaczona do obserwacji lokalnej dynamiki społeczeństwa oraz zapisywania jej stanu i statystyk.

## Jak działa model

- **Przestrzeń:** kwadratowa sieć `WIDTH × WIDTH`, domyślnie `50 × 50`, z geometrią torusa — sąsiednie brzegi sieci są ze sobą połączone.
- **Stan agenta:** agent ma bieżące przekonanie `First`, poprzednie przekonanie `Second` oraz siłę perswazji `Power`.
- **Stan neutralny:** wartość `0` oznacza brak przekonania albo „brak pomysłu na rozrywkę”. Wszystkie wartości dodatnie oznaczają konkretne przekonanie lub kategorię.
- **Kategoria początkowa:** kod inicjalizuje agenta bez przekonania z prawdopodobieństwem `1 - NewProbability`; w przeciwnym razie otrzymuje losową kategorię z zakresu `1 ... NumOfCate - 1`. Zbiór domyślny ma dwie klasy: `0` i `1`.
- **Lokalne otoczenie:** dla losowo wybranego agenta program sprawdza sąsiadów w otoczeniu Moore'a. Maksymalnie może znaleźć ośmiu sąsiadów.
- **Wsparcie sąsiadów:** jeśli agent ma co najmniej jednego sąsiada o tym samym przekonaniu, może go wzmocnić lub wybrać losowego sąsiada o tej samej opinii jako partnera.
- **Zaprzestanie przekonania:** gdy sąsiedzi o tej samej opinii istnieją, agent traci przekonanie z prawdopodobieństwem `ReverseProb - SupportLevel`; bez wsparcia jest natomiast tracony z prawdopodobieństwem `ReverseProb`.
- **Spontaniczne przyjęcie przekonania:** neutralny agent losowo przyjmuje nową kategorię z prawdopodobieństwem `NewProbability`.
- **Zarażanie otoczenia:** para agentów o tej samej opinii może przekonać neutralnych agentów znajdujących się w prostokątnym, lokalnym obszarze pomiędzy nimi. Każdy neutralny agent przechodzi z `0` na przekonanie pary z prawdopodobieństwem `NewInfectProb`.
- **Siła agenta:** `Power` jest losowana z przedziału `MinStrength ... MaxStrength`, jeśli granice są różne. Gdy są równe, wszyscy agenci mają tę samą siłę. Siła jest zapisywana i prezentowana w wizualizacji, choć podstawowa reguła aktualna w `simulate_one_step()` nie wykorzystuje jej bezpośrednio.
- **Aktualizacja:** jeden krok Monte Carlo wykonuje `WIDTH × WIDTH` prób losowo wybranych agentów. Aktualizacja bieżącego stanu jest wykonywana sekwencyjnie; przed krokiem wartości `First` są zapisywane do `Second` na potrzeby statystyk dynamiki.

Zmienne statyczne agentów są ustawiane przez `convWorld`. Wartości modelu z głównego programu są połączone następująco:

| Wartość kodu | Parametr programu | Domyślnie |
| --- | --- | ---: |
| `convAgent::ToBeNewProb` | `NewProbability` | `0.01` |
| `convAgent::NewInfectProb` | `InfectionProb` | `0.5` |
| `convAgent::SupportLevel` | `SupportLev` | `0.9` |
| `convAgent::ReverseProb` | obliczane jako `1 - NewProbability` | `0.99` |

`InfectionProb` nie jest osobnym prawdopodobieństwem odwrócenia przekonania: w `convWorld` trafia do `convAgent::NewInfectProb`, a więc steruje prawdopodobieństwem zarażenia neutralnych sąsiadów.

## Wizualizacja i statystyki

Aplikacja tworzy między innymi:

- mapę aktualnych przekonań,
- mapę poprzednich przekonań,
- mapę siły agentów,
- histogram przekonań,
- historię liczby kategorii i dominującej kategorii,
- historię stresu i entropii,
- macierz współwystępowania bieżących i poprzednich przekonań,
- korelację pomiędzy przekonaniem bieżącym i poprzednim.

Log tekstowy może być zapisywany co `LOGC` kroków. Nazwy domyślne wynikające z kodu programu:

| Opcja | Domyślny plik | Zastosowanie |
| --- | --- | --- |
| `LOGF` | `attitude.log` | log i statystyki symulacji |
| `HIST` | `convinces.otx` | pełna historia stanu programu |
| `MAPL` | `convinces.gif` | mapa początkowych przekonań |
| `MAPP` | `powers.gif` | mapa początkowych sił |
| `MASK` | `mask.gif` | maska aktywnych agentów; czarne piksele usuwają agentów |

Nazwy map są przekazywane do warstwy SymShell i nie muszą istnieć. Przy braku kompletnej kombinacji mapy postaw i sił program inicjalizuje agentów przez ich konstruktory. Czarne piksele maski wyłączają agentów z sieci.

## Budowanie

Wymagane są C++11, CMake oraz biblioteki SymShell/RTM. Budowa na Linuksie włącza obsługę X11 i Xpm; Windows korzysta z natywnej obsługi graficznej.

```bash
sudo apt install cmake libx11-dev libxpm-dev
git clone --recursive https://github.com/borkowsk/symShell2andRTM.git
cd symShell2andRTM

cmake -S EXAMPLES/convince -B build/convince
cmake --build build/convince --target convince
```

Jeżeli projekt już został skonfigurowany z biblioteki przez nadrzędny `CMakeLists.txt`, ustawienie `SYMSHELL2_ALREADY_IMPORTED` pozwala uniknąć ponownego importowania katalogów `WBRTM`, `SYMSHELL` i `SYMSHELL_CLASES`. W przeciwnym razie `CMakeLists.txt` zakłada obecność repozytorium pod względną ścieżką `../../../../public/symShell2andRTM`; po innym położeniu należy skorygować zmienne `WBSLIBS`, `SYMSHELL`, `SYMSCLSS` i `WBRTM` albo zbudować projekt z pełnej konfiguracji repozytorium.

## Uruchamianie

Po poprawnej kompilacji uruchom przykładowy plik wykonywalny z katalogu programu:

```bash
./build/convince/convince
```

Parametry są przekazywane w postaci `NAZWA=WARTOŚĆ`. Wielkość liter nie jest istotna — nazwy są zamieniane na wielkie przed interpretacją. Argumenty zaczynające się od `-` są przekazywane dalej jako opcje SymShell i nie są interpretowane przez `parse_options()`.

Przykład:

```bash
./build/convince/convince WIDTH=80 WIDTHWIN=900 HEIGHTWIN=650 MPOW=10 MAX=5000 VIEW=2 LOGC=5 STOP=Y
```

### Najważniejsze parametry modelu i sieci

| Parametr | Znaczenie | Walidacja | Domyślnie |
| --- | --- | --- | ---: |
| `WIDTH=NN` | szerokość i wysokość torusa agentów | `3 <= WIDTH < WIDTHWIN`; musi być mniejsze niż szerokość okna | `50` |
| `MPOW=NN` | maksymalna siła agenta | powinno być `>= 1` | `1` |
| `MAX=NNNN` | maksymalna liczba kroków Monte Carlo | musi być `> 0`; jednocześnie ustawia długość wewnętrznego logu na `MAX + 1` | `0xffffffff` |
| `VIEW=N` | częstotliwość odświeżania wizualizacji | musi być `> 0` | `1` |
| `LOGC=N` | częstotliwość zapisu do logu | musi być `> 0` | `1` |
| `ILOG=NNNN` | długość wewnętrznych serii statystycznych | wartość poniżej `50` jest zastępowana przez `50` | `7000` |
| `WIDTHWIN=XXX` | początkowa szerokość okna | musi być `>= 50` | `750` |
| `HEIGHTWIN=XXX` | początkowa wysokość okna | musi być `>= 50` | `550` |

Parametry `NewProbability`, `InfectionProb` i `SupportLev` nie są obecnie odczytywane z linii poleceń. Ich wartości definiuje się w `src/convince.cpp`. W tej wersji liczba klas jest ustawiana na `2` w konstruktorze świata i nie ma aktywnej opcji `CLSS=`.

### Sterowanie przebiegiem i pliki

| Parametr | Znaczenie | Domyślnie |
| --- | --- | ---: |
| `AUTO=N` | wykonaj `N` pełnych przebiegów automatycznie | `0` |
| `STOP=Y` | zakończ po osiągnięciu `MAX`; `N` pozostawia symulację w trybie sterowanym | `N` |
| `LOGF=plik.log` | plik logu | `attitude.log` w pamięci? / `attitude.log` |
| `MAPL=plik.gif` | mapa początkowych przekonań; GIF lub BMP | `convinces.gif` |
| `MAPP=plik.gif` | mapa początkowych sił | `powers.gif` |
| `MASK=plik.gif` | maska obszarów zamieszkanych; czarny kolor oznacza brak agentów | `mask.gif` |
| `HIST=plik.otx` | zapis pełnej historii symulacji | `convinces.otx` |
| `REPL=plik.otx` | odtwórz zapis zamiast uruchamiać symulację | brak |

Należy zachować ostrożność przy nazwach plików: kod domyślnie używa ciągu `attitude.log` dla zmiennej `LogName`, więc mimo nazwy projektu warto jawnie podać `LOGF=convince.log`. Ponadto `REPL` nadpisuje `HistName` podaną ścieżką i włącza odczyt historii.

### Tryb interaktywny i automatyczny

- Bez `AUTO` program najpierw czeka na zdarzenia interfejsu (`Ctrl-B` kończy początkową obsługę zdarzeń), a następnie uruchamia pętlę symulacji.
- `AUTO=N` uruchamia `N` kolejnych przebiegów. Po każdym przebiegu, oprócz ostatniego, świat jest ponownie inicjalizowany.
- `STOP=Y` przekazuje symulatorowi polecenie zakończenia po osiągnięciu `MAX`. Nie myli się z natychmiastowym zatrzymaniem programu.
- `REPL=plik.otx` inicjalizuje wizualizację do odczytu i odtwarza poprzednio zapisaną historię zamiast wykonania nowych kroków modelu.

## Struktura kodu

- `src/convince.cpp` — punkt wejścia, parametry procesu, parser argumentów oraz sterowanie trybem symulacji i odtworzenia.
- `src/cagent.h` — deklaracja agentów: przekonanie bieżące i poprzednie, siła, inicjalizacja z bitmap oraz operacje wejścia/wyjścia.
- `src/cworld.h` — deklaracja świata, warstwy agentów, statystyk i interfejsu wizualizacji.
- `src/cworld.cpp` — konstruktory agentów i świata, inicjalizacja map oraz główna reguła Monte Carlo.
- `src/cworldimpi.cpp`, `src/cworldimpo.cpp` — odczyt i zapis stanu świata.
- `src/crand.h` — wybór generatora losowego.
- `src/resamngr.h` — stałe identyfikatorów menu i zasobów interfejsu.
- `src/RES/convince.rc` i `src/RES/convince.ico` — zasoby interfejsu Windows.

## Ograniczenia i interpretacja

- Wyniki zależą od generatora losowego, kolejności prób Monte Carlo i losowej inicjalizacji.
- Aktualna implementacja utrzymuje domyślnie tylko dwie klasy przekonań. Kod zawiera mechanizm obsługi potęg dwójki, ale `convWorld` ustawiany jest w wartość `2`.
- Siła perswazji jest zapisana i wyświetlana, lecz bieżąca wersja funkcji `simulate_one_step()` nie bierze jej bezpośrednio przy wyborze neutralnego sąsiada.
- Kod zawiera kilka opisowych oznaczeń parametrów i komunikatów o statusie; część komentarzy i komunikatów błędów jest niepełna lub niezgodna z końcową implementacją.
- Domyślna nazwa logu w `convince.cpp` jest dziedzictwem starszego przykładu i powinna być zmieniana przez `LOGF=`.
