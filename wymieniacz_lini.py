#!/usr/bin/env python3
## @file 
## @brief  Problem polega na wymianie wielu linii w wielu plikach tekstowych. 
## @date 2026 (modified)
# ==============================================================================
#
# Instrukcja ma formę pliku tekstowego UTF-8 w formacie linii:
#
# ```
# ścieżka_pliku:numer lini:nowa treść linii
# ```
#
# Można ją uzyskać np. tworząc plik za pomocą grep, a potem edytując go:
#
# ```
# grep -rHn "////" --include="*.?pp" > kom_lines.txt ; edit kom_lines.txt &
# ```
#
# Skrypt czyta taka receptę i wymienia CAŁE LINIE. Spacje wiodące są zachowywane.
# Nie można wymienić jednej lini na więcej!
# Instrukcja może wyglądać na przykład tak:
# ```
# third_party/GD2/ext/gdpcxread.cpp:0://==========PCX LOAD FOR GD================
# third_party/GD2/ext/gdpcxread.cpp:114://===========================
# SYMSHELL_CLASES/src/data/datalog.cpp:262://==============================================================
# SYMSHELL_CLASES/src/data/sourbase.cpp:326://======================================
# SYMSHELL_CLASES/src/simul/worldistr.cpp:276://==============================================================
# SYMSHELL_CLASES/src/simul/world.cpp:171://==============================================================
# SYMSHELL_CLASES/src/simul/world.cpp:9233:    //===============
# SYMSHELL_CLASES/src/simul/world.cpp:10127:        //======================
# SYMSHELL_CLASES/src/simul/world.cpp:10421:        //============
# SYMSHELL_CLASES/src/simul/world.cpp:11277:        //==========================
# ```
## @note Numeracja linii zaczyna się od `1`! 
#
# @authors GEMINI & borkowsk
#
import sys
import collections

# Sprawdzenie, czy użytkownik podał plik jako argument
if len(sys.argv) < 2:
    print("Błąd: Nie podano pliku z instrukcjami!")
    print(f"Użycie: python3 {sys.argv[0]} <plik_z_instrukcjami> ")
    sys.exit(1)

nazwa_instrukcji = sys.argv[1]

# Słownik przechowujący zadania: { ścieżka_do_pliku: [(nr_linii, "nowa treść"), ...] }
zadania = collections.defaultdict(list)

# 1. Wczytanie instrukcji
with open(nazwa_instrukcji, 'r', encoding='utf-8') as f:
    for linia in f:
        if not linia.strip():
            continue
        # Dzielimy tylko na 3 części: plik, linia, treść (żeby nie popsuć treści zawierającej dwukropki)
        czesci = linia.split(':', 2)
        if len(czesci) < 3:
            continue
            
        sciezka, nr_str, tresc = czesci
        nr_linii = int(nr_str)-1
        # Usuwamy znak nowej linii z końca wpisu, ale zachowujemy spacje na początku treśći
        tresc = tresc.rstrip('\r\n') 
        #print( sciezka, nr_linii, tresc )
        zadania[sciezka].append((nr_linii, tresc))


# 2. Przetwarzanie plików
for sciezka, modyfikacje in zadania.items():
    try:
        # Sortujemy modyfikacje malejąco po numerze linii
        modyfikacje.sort(key=lambda x: x[0], reverse=True)
        #print(modyfikacje)
        
        # Wczytujemy zawartość pliku docelowego
        with open(sciezka, 'r', encoding='utf-8') as f:
            linie_pliku = f.readlines()
        
        # Aplikujemy zmiany od końca pliku
        for nr_linii, nowa_tresc in modyfikacje:
            if nr_linii < len(linie_pliku):
                # Zamiana istniejącej linii (jeśli się różni)
                if linie_pliku[nr_linii].rstrip('\r\n') != nowa_tresc:
                    print( sciezka,nr_linii,"-",linie_pliku[nr_linii] )
                    linie_pliku[nr_linii] = nowa_tresc + '\n'
                    print( sciezka,nr_linii,"+",linie_pliku[nr_linii] )
                    # JEŚLI CHCIAŁEŚ WSTAWIĆ NOWĄ LINIĘ zamiast nadpisywać, użyj:
                    # linie_pliku.insert(nr_linii, nowa_tresc + '\n')
                #else:
                    #print( nr_linii,"skipped.")       
            else:
                # Jeśli numer linii wykracza poza plik, doklejamy na koniec
                print( "Nowa linia:",nr_linii)
                print( "++",nowa_tresc)
                linie_pliku.append(nowa_tresc + '\n')
                
        # Zapisujemy zmodyfikowany plik
        #with open(sciezka, 'w', encoding='utf-8') as f:
        #    f.writelines(linie_pliku)
        #print(f"Zaktualizowano: {sciezka}")
        
    except Exception as e:
        print(f"Błąd podczas przetwarzania {sciezka}: {e}")
        
        
