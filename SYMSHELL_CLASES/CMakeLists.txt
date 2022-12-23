# CMAKE configuration for SYMSHELLCLASS libraries wbsyshclss
##################################################################################
# @author Wojciech Borkowski
# @date 2022-12-22 (last update)
######################################################

cmake_minimum_required(VERSION 3.0)

project(symshellclasses)

set(CMAKE_CXX_STANDARD 11) # See: https://matgomes.com/setting-cpp-standard-cmake/
set( WBRTMINC      "../RTM/INCLUDE" )
set( SYMSHELLLIGHT "../SYMSHELL" )

IF (WIN32)  # set stuff for windows
  message( ${PROJECT_NAME} ": MS Windows platform assumed.\n\tInside C/C++ code automatic _MSC_VER variable will be used!" )
  set( WINDOWS true )
  #set( MSVC 1 ) #old macro used also somewhere in C code?
  #set( WINDOWS_ONLY_SRC  "${WBRTM}/BASE/wb_assert.c" )
  #add_compile_options( /W4 ) #  /WX
ELSE()	     # set stuff for other systems
  message( ${PROJECT_NAME} ": Linux/unix platform assumed.")
  set( X11 true )
  #set( X_ONLY_SRC  "" )
  #add_compile_options( -Wall )
ENDIF()

include_directories( "SIMULCLSS/" "DATACLSS/" "VISUALCLSS/" "${WBRTMINC}/" "${SYMSHELLLIGHT}")

add_library( wbsyshclss STATIC
	"./src/data/datalog.cpp"
	"./src/data/sourmngr.cpp"
	"./src/data/sourbase.cpp"
	"./src/simul/worldistr.cpp"
	"./src/simul/world.cpp"
	"./src/simul/world_io.cpp"
	"./src/simul/geombase.cpp"
	"./src/simul/worldimpi.cpp"
	"./src/simul/worldimpo.cpp"
	"./src/simul/worldstrm.cpp"
	"./src/simul/aleyerostr.cpp"
#	"./src/simul/layer_gd.cpp"
	"./src/simul/layer.cpp"
	"./src/simul/alayeristr.cpp"
	"./src/simul/worldostr.cpp"
#	"./src/visual/__history/_fastcarp.c--"
	"./src/visual/graphs.cpp"
	"./src/visual/areamngr.cpp"
	"./src/visual/points_plot.cpp"
	"./src/visual/rgb_graphs.cpp"
	"./src/visual/gadgets.cpp"
	"./src/visual/mainmngr.cpp"
	)
	
#target_compile_features(wbsyshclss PUBLIC cxx_std_11)	
	
	
#********************************************************************/
#*              SYMSHELLCLASSES  version 2022-12                    */
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

