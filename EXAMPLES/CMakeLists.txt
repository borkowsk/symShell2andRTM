## Symshell2 examples
## @date 2026-05-07 (modified)
cmake_minimum_required(VERSION 3.10)

# Nazwa całego projektu zbiorczego
project(SymShell_Examples_Suite)

# Ustawienia standardu C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dodaj ścieżkę do plików nagłówkowych samej biblioteki,
# jeśli nie jest zainstalowana w systemie
set( WBSLIBS   "../../../public/symShell2andRTM" )
get_filename_component( WBRTM     "${WBSLIBS}/RTM" ABSOLUTE)
get_filename_component( SYMSHELL  "${WBSLIBS}/SYMSHELL" ABSOLUTE)
get_filename_component( SYMSCLSS  "${WBSLIBS}/SYMSHELL_CLASES" ABSOLUTE)
message( "Assumed sources in\nWBRTM\t at ${WBRTM}\nSYMSHELL\t at ${SYMSHELL}\nAnd classes at ${SYMSCLSS}\n")

#add_subdirectory( ${} ${} )
add_subdirectory( "${SYMSHELL}/" "${SYMSHELL}/" )
add_subdirectory( "${SYMSCLSS}/" "${SYMSCLSS}/" )
add_subdirectory( "${WBRTM}/"   "${WBRTM}/"  )

include_directories( "${WBRTM}/INCLUDE"  "${SYMSHELL}/INCL/"
        "${SYMSCLSS}/DATACLSS"
        "${SYMSCLSS}/SIMULCLSS"
        "${SYMSCLSS}/VISUALCLSS"
)

# Informacja dla konfiguracji katalogów podrzędnych, żeby nie próbowały same zaciągać bibliotek samodzielnie:
set(SYMSHELL2_ALREADY_IMPORTED true)

# Lista podkatalogów z projektami
# Każdy z tych katalogów powinien mieć własny plik CMakeLists.txt
add_subdirectory(basictest)
add_subdirectory(attitude)
add_subdirectory(convince)
add_subdirectory(life)
add_subdirectory(conflicts)
add_subdirectory(kattitude)
add_subdirectory(need4closure)
add_subdirectory("../../../archivals/Languages_model" "../../../archivals/Languages_model")

#COEVO2 przestało działać ze względu na hakowane wskaźniki do pól. Wymaga ostrożnych zmian...
add_subdirectory("../../../archivals/Coevo2_model" "../../../archivals/Coevo2_model")

#DYNEGO kiedys zostanie uwolnione...
add_subdirectory("../../../private/DYNEGO_model" "../../../private/DYNEGO_model")

# Lista przykładów do zdefiniowania metatargetu
set(EXAMPLES_LIST
        basictest
        attitude
        convince
        life
        kattitude
        need4closure
        conflicts
)

# Definicja zbiorczego targetu "all_example"
# Nazwa pliku wykonywalnego (targetu wewnątrz subdirectory) to nazwa katalogu.
add_custom_target(all_examples
        DEPENDS ${EXAMPLES_LIST}
        COMMENT "Building all SymShell examples..."
)

# Informacja dla użytkownika po zakończeniu konfiguracji
message(STATUS "All projects:" $EXAMPLES_LIST "have been configured.")
