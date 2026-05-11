# CMAKE configuration for BASICTESTS simulation project
######################################################################
# @author	: Wojciech Borkowski
# @date 2026-05-11 (last update)
#

cmake_minimum_required(VERSION 3.20)

project(BASICTESTS)

set(CMAKE_CXX_STANDARD 11) # See: https://matgomes.com/setting-cpp-standard-cmake/
#set(CMAKE_CXX_STANDARD_REQUIRED On)
#set(CMAKE_CXX_EXTENSIONS Off)

IF (WIN32)
  # set stuff for windows
  message( "MS Windows platform assumed.\n\tInside C/C++ code automatic _MSC_VER variable will be used!" )
  add_compile_definitions( _CRT_SECURE_NO_WARNINGS )
  add_definitions( -DUSE_WINDOWS )
ELSE()
  # set stuff for other systems
  message( "Linux/unix platform assumed.")
  #add_compile_definitions( ... )
  add_definitions( -DHAVE_X11 -DUSE_ZENITY ) # -DUSE_X11  DUSE_CONSOLE_ONLY -DDEF_MAXTHREADS=16 -DMULTITR
ENDIF()

if (SYMSHELL2_ALREADY_IMPORTED)
    message( "Symshell2 libraries already imported from master CMakeLists.txt" )
else ()
    set( WBSLIBS   "../../../../public/symShell2andRTM" )
    get_filename_component( WBRTM     "${WBSLIBS}/RTM" ABSOLUTE)
    get_filename_component( SYMSHELL  "${WBSLIBS}/SYMSHELL" ABSOLUTE)
    get_filename_component( SYMSCLSS  "${WBSLIBS}/SYMSHELL_CLASES" ABSOLUTE)
    message( "Assumed sources in\nWBRTM\t at ${WBRTM}\nSYMSHELL\t at ${SYMSHELL}\nAnd classes at ${SYMSCLSS}\n")

    #add_subdirectory( ${} ${} )
    add_subdirectory( "${SYMSHELL}/" "${SYMSHELL}/" )
    add_subdirectory( "${SYMSCLSS}/" "${SYMSCLSS}/" )
    add_subdirectory( "${WBRTM}/"   "${WBRTM}/"  )

    include_directories( "${WBRTM}/INCLUDE"  "${SYMSHELL}/INCL/" "${SYMSHELL}/INCL/RTM_LIGHT"
                         "${SYMSCLSS}/DATACLSS"
                         "${SYMSCLSS}/SIMULCLSS"
                         "${SYMSCLSS}/VISUALCLSS"
                        )
endif ()

IF (WIN32)
    #set( WINDOWS_ONLY_SRC  "${WBRTM}/BASE/wb_assert.c" )
ELSE ()
    #set( X_ONLY_SRC  "" )
ENDIF()

add_executable( testmngr2
                src/Testmngr2.cpp
                ../../SYMSHELL_CLASES/src/visual/mainpopmenu.c
                # $WINDOWS_ONLY_SRC
                # $X_ONLY_SRC
              )


IF (WIN32)
#target_compile_options( dimensions  PRIVATE ...)
target_link_libraries( testmngr2
#        "-L ${MYLIBS}"
        wbsyshclss wbrtm wbsyshLight
        ${CMAKE_THREAD_LIBS_INIT}
     )
ELSE()
target_compile_options( testmngr2  PRIVATE "${CMAKE_CXX_FLAGS}" "-pthread" "-fpermissive")
target_link_libraries( testmngr2
#        "-L ${MYLIBS}"
        wbsyshclss wbrtm wbsyshLight
        ${CMAKE_THREAD_LIBS_INIT}
        pthread
        rt
     )
ENDIF()


