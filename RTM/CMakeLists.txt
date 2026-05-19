# CMAKE configuration for WBRTM library `wbrtm`
######################################################
# (c)ISS UW, @author Wojciech Borkowski
# @date 2026-05-19 (last update)
######################################################

project(wbrtm)
cmake_minimum_required(VERSION 3.00)

set(CMAKE_CXX_STANDARD 11) # See: https://matgomes.com/setting-cpp-standard-cmake/
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)  

set(WBRTM "./src/")
set(WBRTMINC "./INCLUDE/" )
#set(CONTRLIBS "../CONTRLIBS/")

IF (WIN32)
  # set stuff for windows
  message( ${PROJECT_NAME} ": MS Windows platform assumed.\n\tInside C/C++ code automatic _MSC_VER variable will be used!" )
  #set( WINDOWS_ONLY_SRC  "${WBRTM}/BASE/wb_assert.c" )
  add_compile_definitions( _CRT_SECURE_NO_WARNINGS )
ELSE()
  # set stuff for other systems
  message( ${PROJECT_NAME} ": Linux/unix platform assumed." )
  #set( X_ONLY_SRC  "" )
  #add_compile_options( -Wall )
ENDIF()

include_directories( "${WBRTMINC}" ) #"${CONTRLIBS}" )

if(true) # wbrtm basic
add_library( wbrtm STATIC
#   "${WBRTM}/MISCCLASSES/Chameleon.cpp"
#   "${WBRTM}/MISCCLASSES/ConfigData.cpp"
#   "${WBRTM}/MISCCLASSES/Nodemem.cpp"
#   "${WBRTM}/TOOLS/RealPath.cpp"
#   "${WBRTM}/TOOLS/wb_compdate.c"
#   "${WBRTM}/TOOLS/cast_checker.cpp"
#   "${WBRTM}/TOOLS/wb_strrnd.c"
    "${WBRTM}/TOOLS/SMARTLOG/smartlog.cpp"
    "${WBRTM}/TOOLS/TABDELIMITED/TabDelimited.cpp"
    "${WBRTM}/BASE/viewHtmlPchar.cpp" #Do wyświetlania helpów etc...
    "${WBRTM}/BASE/viewHtmlStr.cpp"
    "${WBRTM}/PTRS/wb_pchario.cpp"
    "${WBRTM}/PTRS/wb_pchar_verboten.cpp"
    "${WBRTM}/PTRS/wb_pchar.cpp"
    "${WBRTM}/BASE/wb_randg.c"
    "${WBRTM}/BASE/wb_rand.cpp"
    "${WBRTM}/EXCEPT/ioexcep.cpp"
    "${WBRTM}/EXCEPT/excpprin.cpp"
    "${WBRTM}/EXCEPT/boundexc.cpp"
    "${WINDOWS_ONLY_SRC}" #TYLKO WINDOWS
    "${WBRTM}/VIRTUALIO/iosuppor.cpp"
    "${WBRTM}/VIRTUALIO/iodatabase.cpp"
    "${WBRTM}/VIRTUALIO/iocharp.cpp"
    "${WBRTM}/VIRTUALIO/vobject.cpp"
#   "${WBRTM}/EXCEPT/extextex.cpp"
#   "${WBRTM}/BASE/wb_cmdline.cpp"
#   "${WBRTM}/BASE/wb_assert.c"
    "${WBRTM}/BASE/errorhan.cpp"
    "${WBRTM}/BASE/error_enter_before_clean.c"
    "${WBRTM}/COMPATIBILITY/stricmp.c"
    "${WBRTM}/COMPATIBILITY/struprstrlwr.c" 
    "${WBRTMINC}/arrays.hpp"
    "${WBRTMINC}/assoctab.hpp"
    "${WBRTMINC}/compatyb.hpp"
    "${WBRTMINC}/excpmem.hpp"
    "${WBRTMINC}/iocharp.hpp"
    "${WBRTMINC}/mem_guard.h"
    "${WBRTMINC}/random.h"
    "${WBRTMINC}/var_contr.hpp"
    "${WBRTMINC}/_voidnew.hpp"
    "${WBRTMINC}/wb_limits.hpp"
    "${WBRTMINC}/wb_rand.hpp"
    "${WBRTMINC}/containe.hpp"
    "${WBRTMINC}/excpothr.hpp"
    "${WBRTMINC}/ioexcep.hpp"
    "${WBRTMINC}/TabDelimited.hpp"
    "${WBRTMINC}/viewHtml.hpp"
    "${WBRTMINC}/wb_bits.h"
    "${WBRTMINC}/wb_pchario.hpp"
    "${WBRTMINC}/wb_smartlog.hpp"
    "${WBRTMINC}/arr_base.hpp"
    "${WBRTMINC}/bounded.hpp"
    "${WBRTMINC}/errorhan.hpp"
    "${WBRTMINC}/iosuppor.hpp"
    "${WBRTMINC}/optParam.hpp"
    "${WBRTMINC}/typenames.hpp"
    "${WBRTMINC}/virtualConstructor.h"
    "${WBRTMINC}/wb_clone.hpp"
    "${WBRTMINC}/wb_ptr.hpp"
    "${WBRTMINC}/wb_swap.hpp"
    "${WBRTMINC}/assobase.hpp"
    "${WBRTMINC}/compatyb.h"
    "${WBRTMINC}/excpbase.hpp"
    "${WBRTMINC}/_ikeycon.hpp"
    "${WBRTMINC}/lingo.hpp"
    "${WBRTMINC}/outdated"
    "${WBRTMINC}/use_math_defines.h"
    "${WBRTMINC}/vobject.hpp"
    "${WBRTMINC}/wb_cpucl.hpp"
    "${WBRTMINC}/wb_ptrio.h"
    "${WBRTMINC}/wb_tnames.h"
    "${WBRTMINC}/_groups.h"
    "${WBRTMINC}/_obsolete.h"
    "${WBRTMINC}/arrays.imp"
    "${WBRTMINC}/assoctab.IMP"
     )
endif()


#install(TARGETS wbrtm DESTINATION lib)


#KONIEC - THE END
#********************************************************************/
#*                     WBRTM version 2026                           */
#********************************************************************/
#*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
#*            W O J C I E C H   B O R K O W S K I                   */
#*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
#*    GITHUB: https://github.com/borkowsk                           */
#*                                                                  */
#*    Instytut Studiów Społecznych im. Profesora Roberta Zajonca    */
#*    Uniwersytetu Warszawskiego                                    */
#*    PL.WWW: http://iss.uw.edu.pl/                                 */
#*                                                                  */
#*    Robert Zajonc Institute for Social Studies                    */
#*    University of Warsaw                                          */
#*    EN.WWW: http://iss.uw.edu.pl/en/                              */
#*                                                                  */
#*                               (Don't change or remove this note) */
#********************************************************************/


