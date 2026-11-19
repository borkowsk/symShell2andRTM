# Attitude — symulacja zmian postaw

`attitude` jest interaktywną symulacją agentową zmian postaw w modelu przypisywanym A. Nowakowi. Program odwzorowuje lokalne oddziaływania społeczne na regularnej, dwuwymiarowej sieci agentów. Kod przykładu znajduje się w [`src/`](src/); główna pętla aplikacji jest w [`attitude.cpp`](src/attitude.cpp), a logika agentów i świata — w [`aworld.cpp`](src/aworld.cpp).

To przykład badawczy oparty na bibliotece SymShell/RTM, a nie samodzielny pakiet z niezależnym silnikiem graficznym. Starszy opis modelu i eksperymentów znajduje się w [`DOCS/attitude_dps.pdf`](DOCS/attitude_dps.pdf) (dostępna jest też wersja [HTML](DOCS/attitude_dps.htm)).

## Model użyto w następujących publikacjach
* 2009: [Wpływ społeczny - alternatywny model rozprzestrzeniania się memów](https://www.researchgate.net/publication/299283580_Wplyw_spoleczny_-_alternatywny_model_rozprzestrzeniania_sie_memow)
* 2009b: [Wpływ społeczny jako model rozprzestrzeniania się memów](https://www.researchgate.net/publication/299282906_Wplyw_spoleczny_jako_model_rozprzestrzeniania_sie_memow)
* 2005: [Wpływ społeczny jako model rozprzestrzeniania się memów (paper)](https://www.researchgate.net/publication/260059435_Wplyw_spoleczny_jako_model_rozprzestrzeniania_sie_memow)

## Jak działa model

- **Przestrzeń:** kwadratowa siatka `WIDTH × WIDTH`, domyślnie `50 × 50`, z geometrią torusa. Każda komórka może zawierać agenta albo pozostać pusta; maska wejściowa pozwala wyłączać agentów.
- **Postawa:** agent ma jedną z `CLSS` kategorii. Domyślna liczba kategorii to 2; obsługiwane są potęgi dwójki od 2 do 256. Przy inicjalizacji losowej postawa jest losowana równomiernie.
- **Siła:** agent ma zmienną `Power`, która waży jego głos w lokalnej presji społecznej. Początkowa siła jest losowana do wartości maksymalnej włącznie. Gdy włączony jest wzrost (`WPOW`), siła zmienia się przy starzeniu agenta zgodnie z implementacją modulo `MPOW` — nie jest to wzrost bez ograniczenia.
- **Reguła zmiany:** dla agenta, który nie przekroczył progu odporności, program sumuje siły sąsiadów według ich aktualnych postaw. Opcjonalnie dołącza własną siłę (`SELF=Y`). Agent przyjmuje postawę o największym łącznym poparciu; losowy offset wpływa na rozstrzyganie remisów. `NOIP` dodaje losowy szum do dodatnich sum poparcia.
- **Odporność:** jeżeli siła agenta jest większa niż próg `MPOW × TRSP / 100`, reguła wpływu go pomija. Wartość domyślna `TRSP=100` oznacza próg równy maksymalnej sile.
- **Losowe zmiany:** `SPCH` określa prawdopodobieństwo spontanicznego wylosowania nowej postawy. Ta próba jest wykonywana przed regułą wpływu sąsiadów.
- **Sąsiedztwo:** `INDI` określa promień. Przy promieniu 1 lokalne otoczenie obejmuje maksymalnie osiem sąsiednich komórek. `PRTR` ogranicza liczbę losowanych partnerów; `PRTR=-1` oznacza uwzględnienie całego otoczenia. Puste komórki nie biorą udziału w zliczaniu.
- **Aktualizacja:** `SYNC=Y` włącza aktualizację synchroniczną: obliczane są nowe postawy, a następnie stosowane wspólnie. `SYNC=N` wybiera losową kolejność Monte Carlo, w której zmiana może wpływać na późniejsze aktualizacje w tym samym kroku.

W każdej z tych konfiguracji krok symulacji obejmuje przejście po warstwie agentów. W trybie synchronicznym starzenie następuje po wspólnym zastosowaniu zmian; w trybie Monte Carlo agent starzeje się podczas swojej aktualizacji.

## Wizualizacja i pomiary

Okno prezentuje między innymi mapę aktualnych i poprzednich postaw, mapę siły, histogram postaw, historię liczby kategorii i dominującej kategorii, miary stresu i entropii oraz macierz przejść między poprzednią a aktualną postawą. Program zapisuje także dane statystyczne i historię przebiegu.

Domyślne nazwy plików:

| Opcja | Domyślny plik | Zastosowanie |
| --- | --- | --- |
| `LOGF` | `attitude.log` | log i statystyki |
| `HIST` | `attitude.otx` | historia stanu symulacji, przydatna do odtworzenia |
| `MAPL` | `attitude.gif` | wejściowa mapa postaw |
| `MAPP` | `powers.gif` | wejściowa mapa sił |
| `MASK` | `mask.gif` | maska aktywnych agentów; czarne piksele usuwają agentów |

Mapy są opcjonalne: bez map program inicjuje stan losowo. W kodzie mapy mogą być bitmapami obsługiwanymi przez warstwę SymShell; komunikaty programu wymieniają format GIF lub BMP. Mapa siły przelicza średnią wartość RGB na zakres siły. Mapa postaw koduje kategorię z kanałów czerwonego i niebieskiego.

## Budowanie

Wymagane są C++11, CMake, kompilator C++ (GCC lub Clang) oraz zależności SymShell/RTM. Na Linuksie konfiguracja przykładu włącza X11; wymagane są odpowiednie biblioteki deweloperskie X11/Xpm. Na Windowsie CMake wybiera obsługę natywną.

Repozytorium należy pobrać z submodułami:

```bash
git clone --recursive https://github.com/borkowsk/symShell2andRTM.git
```

albo uzupełnić submoduły w już sklonowanym repozytorium:

```bash
git submodule update --init --recursive
```

Konfiguracja projektu przykładu znajduje się w `EXAMPLES/attitude/CMakeLists.txt`. Można ją włączyć z nadrzędnego projektu przez mechanizm `SYMSHELL2_ALREADY_IMPORTED` albo skonfigurować osobno, jeśli ścieżki do bibliotek wskazane w tym pliku odpowiadają układowi katalogów lokalnej kopii. Po konfiguracji kompiluje się cel `attitude`:

```bash
cmake -S EXAMPLES/attitude -B build/attitude
cmake --build build/attitude --target attitude
```

Jeżeli konfiguracja nie odnajduje bibliotek, należy najpierw sprawdzić ścieżki `WBRTM`, `SYMSHELL` i `SYMSCLSS` w CMakeLists albo użyć konfiguracji nadrzędnej repozytorium. Projekt wykorzystuje starszą bibliotekę i jej zależności platformowe; powyższe polecenia nie zastępują instalacji wymaganych bibliotek systemowych.

## Uruchamianie i parametry

Program przyjmuje parametry w postaci `NAZWA=WARTOŚĆ`, na przykład:

```bash
./attitude WIDTH=80 CLSS=4 INDI=1 PRTR=-1 MAX=5000 NOIP=5 SPCH=0.1
```

Wartości ułamkowe `SPCH` podaje się jako procent (w przykładzie `0.1` oznacza 0,1%). Najważniejsze parametry:

| Parametr | Znaczenie | Domyślnie |
| --- | --- | ---: |
| `WIDTH` | szerokość i wysokość świata | `50` |
| `CLSS` | liczba postaw; w implementacji musi być potęgą 2, maksymalnie 256 | `2` |
| `MPOW` | maksymalna siła | `100` |
| `WPOW` | przyrost siły przy starzeniu; `0` wyłącza zmianę siły | `0` |
| `TRSP` | próg odporności jako procent `MPOW` | `100` |
| `INDI` | promień sąsiedztwa | `1` |
| `PRTR` | liczba losowanych sąsiadów; `-1` bierze całe otoczenie | `7` |
| `SELF` | uwzględnianie własnej postawy i siły (`Y`/`N`) | `Y` |
| `SYNC` | synchroniczny tryb aktualizacji (`Y`) lub Monte Carlo (`N`) | `N` |
| `NOIP` | poziom szumu w procentach, 0–100 | `0` |
| `SPCH` | prawdopodobieństwo spontanicznej zmiany w procentach, 0–100 | `0` |
| `MAX` | maksymalna liczba kroków | bez praktycznego limitu |
| `VIEW` | częstotliwość odświeżania wizualizacji | `1` |
| `LOGC` | częstotliwość zapisu logu | `1` |
| `ILOG` | długość wewnętrznych historii wykresów | `7000` |
| `AUTO` | liczba automatycznych uruchomień symulacji | `0` (start ręczny) |
| `STOP` | zamknięcie po zakończeniu symulacji (`Y`/`N`) | `N` |
| `MAPL`, `MAPP`, `MASK` | pliki mapy postaw, siły i maski | wartości z tabeli wyżej |
| `HIST` | ścieżka zapisu historii | `attitude.otx` |
| `REPL` | odtworzenie zapisanej historii zamiast nowej symulacji | — |
| `LOGF` | ścieżka pliku logu | `attitude.log` |

Dostępne są również `WIDTHWIN` i `HEIGHTWIN` do zmiany początkowych wymiarów okna. Program bez `AUTO` oczekuje na zdarzenia interfejsu; parametry `MAX` i `STOP=Y` są przydatne w przebiegach automatycznych. Pełną listę akceptowanych argumentów wypisuje program po podaniu nieznanej lub błędnej opcji.

> **Uwaga o wartościach:** kod wykonuje część walidacji parametrów dopiero podczas inicjalizacji. W szczególności `CLSS` powinno być potęgą dwójki z zakresu 2–256, a `WIDTH` powinno być większe od 3 i mniejsze od szerokości okna. Dla powtarzalnych eksperymentów warto jawnie ustawiać ziarno generatora losowego zgodnie z możliwościami biblioteki.

## Struktura kodu

- `src/attitude.cpp` — argumenty, konfiguracja aplikacji i uruchomienie pętli symulacji.
- `src/aagent.h` — stan i zachowanie agenta: siła, postawa, starzenie oraz konwersja danych wejściowych z bitmap.
- `src/aworld.h`, `src/aworld.cpp` — parametry świata, inicjalizacja, reguła oddziaływań, aktualizacja i wizualizacja.
- `src/aworldimpi.cpp`, `src/aworldimpo.cpp` — serializacja i odczyt stanu świata.
- `src/arand.h` — wybór generatora losowego używanego przez przykład.
- `src/resamngr.h` — identyfikatory komunikatów interfejsu.

## Ograniczenia i interpretacja

Wyniki są stochastyczne i zależą od losowej inicjalizacji, generatora losowego, parametrów sieci oraz trybu aktualizacji. Symulacja pozwala badać skutki lokalnej presji, różnic siły, odporności, szumu i mutacji, ale sama w sobie nie jest estymacją ani walidacją empiryczną konkretnej populacji. W szczególności sposób wyboru sąsiadów i aktualizacji jest częścią modelu, a nie neutralnym szczegółem implementacyjnym.
