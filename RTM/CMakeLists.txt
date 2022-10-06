cmake_minimum_required(VERSION 3.0)

project(wbrtm)

set(WBRTM "./src")
set(WBRTMINC "./INCLUDE" )
#set(CONTRLIBS "../CONTRLIBS")

include_directories( "." "${WBRTMINC}" ) #"${CONTRLIBS}" )


if(true)# wbrtm basic
add_library( wbrtm STATIC
#    "${WBRTM}/BASE/cast_checker.cpp"
#    "${WBRTM}/BASE/wb_strrnd.c"
#    "${WBRTM}/BASE/wb_assert.c"
#    "${WBRTM}/BASE/wb_cmdline.cpp"
    "${WBRTM}/BASE/wb_randg.c"
    "${WBRTM}/BASE/wb_rand.cpp"
#    "${WBRTM}/BASE/wb_compdate.c"
#    "${WBRTM}/BASE/RealPath.cpp"
#    "${WBRTM}/BASE/ViewHtmlPchar.cpp" #Do wyświetlania helpów etc...
#    "${WBRTM}/BASE/ViewHtmlStr.cpp"
    "${WBRTM}/COMPATIBILITY/stricmp.c"
#    "${WBRTM}/_COMPAT/autor.cpp" #TYLKO WINDOWS
    "${WBRTM}/PTRS/wb_pchario.cpp"
    "${WBRTM}/PTRS/wb_pchar_verboten.cpp"
    "${WBRTM}/PTRS/wb_pchar.cpp"
#    "${WBRTM}/EXCEPT/errorhan.cpp"
#    "${WBRTM}/EXCEPT/boundexc.cpp"
#    "${WBRTM}/EXCEPT/extextex.cpp"
#    "${WBRTM}/EXCEPT/ioexcep.cpp"
#    "${WBRTM}/EXCEPT/excpprin.cpp"
#    "${WBRTM}/VIRTUALIO/iosuppor.cpp"
#    "${WBRTM}/VIRTUALIO/iodatabase.cpp"
#    "${WBRTM}/VIRTUALIO/iocharp.cpp"
#    "${WBRTM}/VIRTUALIO/vobject.cpp"
#    "${WBRTM}/MISCCLASSES/Smartlog.cpp"
#    "${WBRTM}/MISCCLASSES/Chameleon.cpp"
#    "${WBRTM}/MISCCLASSES/ConfigData.cpp"
    "${WBRTM}/BASE/error_enter_before_clean.c"
#    "${WBRTM}/MISCCLASSES/Nodemem.cpp"
    )
endif()


#install(TARGETS wbrtm DESTINATION lib)


#KONIEC - THE END
#********************************************************************/
#*              z SYMSHELLLIGHT  version 2022-08-12                 */
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
