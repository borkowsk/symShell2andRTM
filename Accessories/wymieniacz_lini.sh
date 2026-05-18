#!/bin/bash
## @file 
## @brief  Problem polega na wymianie wielu linii w wielu plikach tekstowych. 
## @date 2026-05-18 (modified)
# ==============================================================================
# (NIEPRZETESTOWANE - WERSJA Pythonowa ZA TO DZIAŁA)
# Instrukcja ma formę pliku tekstowego UTF-8 w formacie linii:
#
# ```
# ścieżka_pliku:numer lini:nowa treść linii
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
## @note Numeracja linii zaczyna się od `0`! 
#
# @authors GEMINI & borkowsk
# https://gemini.google.com/app/fa771692e0c92806?utm_source=app_launcher&utm_medium=owned&utm_campaign=base_all

# Sprawdzenie czy podano argument
if [ -z "$1" ]; then
    echo "Błąd: Nie podano pliku z instrukcjami!"
    echo "Użycie: $0 <plik_z_instrukcjami>"
    exit 1
fi

# Sprawdzenie czy plik istnieje
if [ ! -f "$1" ]; then
    echo "Błąd: Plik '$1' nie istnieje."
    exit 1
fi

PLIK_INSTRUKCJI="$1"

# Przetwarzanie struktury za pomocą AWK
awk -F: '{
    file=$1; line=$2; 
    sub(/^[^:]+:[^:]+:/, "", $0); 
    print line "|||" $0 >> file ".patch"
}' "$PLIK_INSTRUKCJI"

# Aplikowanie poprawek
for patch_file in *.patch; do
    # Zabezpieczenie na wypadek braku plików .patch
    [ -e "$patch_file" ] || continue
    
    target_file="${patch_file%.patch}"
    
    sort -t'|' -k1,1nr "$patch_file" | while IFS="|||" read -r num content; do
        sed -i "$((num + 1))s|.*|$content|" "$target_file"
    done
    
    rm "$patch_file"
done

echo "Zakończono przetwarzanie pliku $PLIK_INSTRUKCJI"
