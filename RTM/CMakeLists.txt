cmake_minimum_required(VERSION 3.0)

project(wbrtm)

set(WBRTM "./")
set(CONTRLIBS "../CONTRLIBS")

include_directories("${WBRTM}" "${CONTRLIBS}")

if(true)# SVG SYMSHELL
add_library( wbsyshSVG STATIC #"${WBRTM}/SYMSHELL/src/svg/symshsvg20170411.cpp"
    "${WBRTM}/SYMSHELL/src/svg/symshsvg.cpp"
    "${WBRTM}/SYMSHELL/src/Unix/sshmenufdummy.c" #zamienia funkcje obslugi menu na puste wywołania
    "${WBRTM}/SYMSHELL/src/svg/errorent.c"
)
endif()

if(true)# only for X11
add_definitions( -DHAVE_X11 -DUSE_X11 )
add_library( wbsyshX11 STATIC 
    "${WBRTM}/SYMSHELL/src/Unix/symshx11.c"
    "${WBRTM}/SYMSHELL/src/Unix/wbx11ico.c"
    "${WBRTM}/SYMSHELL/src/Unix/sshdefs.c"
    "${WBRTM}/SYMSHELL/src/Unix/sshmenufdummy.c" #zamienia funkcje obslugi menu na puste wywołania
    "${WBRTM}/SYMSHELL/src/errorent.c"
    #Dwa poniższe zniknęły z github'a
    #"${CONTRLIBS}/MessageBox-X11/messagebox.c" #lightweight message box, using only X11 for rendering
    #"${CONTRLIBS}/OpenFile-X11/libsofd.c" #Simple Open File Dialog [for X11 without toolkit]

    #Do graficznego menu trzeba zainstalować Xm albo Xaw3d, a to starocie
    #"${WBRTM}/SYMSHELL/src/Unix/sshmenux11.c"
    #"${WBRTM}/SYMSHELL/src/Unix/sshmenufx11.c"
    #"${WBRTM}/SYMSHELL/src/Unix/sshempty.c"
)
endif()

if(false)# only for WINDOWS
add_library( wbsyshWIN STATIC
    "${WBRTM}/SYMSHELL/src/Windows/sshmenufwin.c"
    "${WBRTM}/SYMSHELL/src/Windows/symshwin.c"
    "${WBRTM}/_COMPAT/autor.cpp"
    #"${WBRTM}/SYMSHELL/MFC5_include/l.cht"
    #"${WBRTM}/SYMSHELL/MFC5_include/l.chs"
    "${WBRTM}/SYMSHELL/src/errorent.c"
    )
endif()

if(true)# wbrtm/symshell light/edu Linux - zawiera sshutils i ewentualnie inne często potrzebne
add_library(wbsymshlight STATIC
    "${WBRTM}/SYMSHELL/src/Unix/symshx11.c"
    "${WBRTM}/SYMSHELL/src/Unix/wbx11ico.c"
    "${WBRTM}/SYMSHELL/src/BGraphix/sshutils.cpp"
    "${WBRTM}/SYMSHELL/src/Unix/sshmenufdummy.c" #zamienia funkcje obslugi menu na puste wywołania
#    "${WBRTM}/SYMSHELLLIGHT/symshx11.c"
#    "${WBRTM}/SYMSHELLLIGHT/wbx11ico.c"
#    "${WBRTM}/SYMSHELLLIGHT/sshutils.cpp"
    "${WBRTM}/SYMSHELL/src/errorent.c"
    )
endif()

if(false)# wbrtm/symshell light/edu Windows - zawiera sshutils i ewentualnie inne często potrzebne
add_library( wbsymshlight STATIC
    "${WBRTM}/SYMSHELLLIGHT/symshwin.c"
    "${WBRTM}/SYMSHELLLIGHT/sshutils.cpp"
    "${WBRTM}/SYMSHELLLIGHT/sshmenufwin.c"
    "${WBRTM}/SYMSHELL/src/errorent.c"
    )
endif()

if(true)# C++ SYMulation SHELL functions & classes
add_library( wbsymshell STATIC
    "${WBRTM}/SYMSHELL/src/BGraphix/sshutils.cpp"
    "${WBRTM}/SYMSHELL/src/BGraphix/strtorgb.cpp"
#    "${WBRTM}/SYMSHELL/src/BGraphix/pseudobgi.cpp" # bardzo stare
    "${WBRTM}/SYMSHELL/src/Simul/world_io.cpp"
    "${WBRTM}/SYMSHELL/src/Simul/worldstrm.cpp"
    "${WBRTM}/SYMSHELL/src/Simul/worldistr.cpp"
    "${WBRTM}/SYMSHELL/src/Simul/aleyerostr.cpp"
    "${WBRTM}/SYMSHELL/src/Simul/worldostr.cpp"
    "${WBRTM}/SYMSHELL/src/Simul/worldimpo.cpp"
    "${WBRTM}/SYMSHELL/src/Simul/layer.cpp"
    "${WBRTM}/SYMSHELL/src/Simul/world.cpp"
    "${WBRTM}/SYMSHELL/src/Simul/layer_gd.cpp"
    "${WBRTM}/SYMSHELL/src/Simul/alayeristr.cpp"
    "${WBRTM}/SYMSHELL/src/Simul/geombase.cpp"
    "${WBRTM}/SYMSHELL/src/Simul/worldimpi.cpp"
    "${WBRTM}/SYMSHELL/src/DataSources/sourbase.cpp"
    "${WBRTM}/SYMSHELL/src/DataSources/datalog.cpp"
    "${WBRTM}/SYMSHELL/src/DataSources/sourmngr.cpp"
    "${WBRTM}/SYMSHELL/src/GAreas/rgb_graphs.cpp"
    "${WBRTM}/SYMSHELL/src/GAreas/areamngr.cpp"
    "${WBRTM}/SYMSHELL/src/GAreas/points_plot.cpp"
    "${WBRTM}/SYMSHELL/src/GAreas/mainmngr.cpp"
    "${WBRTM}/SYMSHELL/src/GAreas/gadgets.cpp"
    "${WBRTM}/SYMSHELL/src/GAreas/graphs.cpp"
    "${WBRTM}/SYMSHELL/src/errorent.c"
    )
endif()

if(false)# wbrtm basic
add_library( wbsps STATIC
    "${WBRTM}/SPS/spsGenLink.cpp"
    "${WBRTM}/SPS/spsSwiatInit.cpp"
    "${WBRTM}/SPS/spsGenProc.cpp"
    "${WBRTM}/SPS/spsParaLink.cpp"
    "${WBRTM}/SPS/spsSwiatKrokSymul.cpp"
    "${WBRTM}/SPS/spsGenNode.cpp"
    "${WBRTM}/SPS/spsGenInfo.cpp"
    "${WBRTM}/SPS/spsSwiatProc.cpp"
    "${WBRTM}/SPS/spsArcLink.cpp"
    "${WBRTM}/SPS/spsElement.cpp"
    "${WBRTM}/SPS/spsKonstrLst.cpp"
    "${WBRTM}/SPS/spsMatNodeFiles.cpp"
    "${WBRTM}/SPS/spsMatrixNode.cpp"
    "${WBRTM}/SPS/spsSwiatWizual.cpp"
    "${WBRTM}/SPS/spsSwiatBaza.cpp"
    )
endif()

if(true)# wbrtm basic
add_library( wbrtm STATIC
    "${WBRTM}/BASE/cast_checker.cpp"
    "${WBRTM}/BASE/wb_strrnd.c"
    "${WBRTM}/BASE/wb_assert.c"
    "${WBRTM}/BASE/wb_cmdline.cpp"
    "${WBRTM}/BASE/wb_randg.c"
    "${WBRTM}/BASE/wb_rand.cpp"
    "${WBRTM}/BASE/wb_compdate.c"
    "${WBRTM}/BASE/RealPath.cpp"
    "${WBRTM}/BASE/ViewHtmlPchar.cpp" #Do wyswietlanie helpów etc...
    "${WBRTM}/BASE/ViewHtmlStr.cpp"
    "${WBRTM}/_COMPAT/stricmp.c"
    #"${WBRTM}/_COMPAT/autor.cpp" #TYLKO WINDOWS
    "${WBRTM}/PTRS/wb_pchario.cpp"
    "${WBRTM}/PTRS/wb_pchar_verboten.cpp"
    "${WBRTM}/PTRS/wb_pchar.cpp"
    "${WBRTM}/EXCEPT/errorhan.cpp"
    "${WBRTM}/EXCEPT/boundexc.cpp"
    "${WBRTM}/EXCEPT/extextex.cpp"
    "${WBRTM}/EXCEPT/ioexcep.cpp"
    "${WBRTM}/EXCEPT/excpprin.cpp"
    "${WBRTM}/VIRTUALIO/iosuppor.cpp"
    "${WBRTM}/VIRTUALIO/iodatabase.cpp"
    "${WBRTM}/VIRTUALIO/iocharp.cpp"
    "${WBRTM}/VIRTUALIO/vobject.cpp"
    "${WBRTM}/MISCCLASSES/Smartlog.cpp"
    "${WBRTM}/MISCCLASSES/Chameleon.cpp"
    "${WBRTM}/MISCCLASSES/ConfigData.cpp"
    "${WBRTM}/MISCCLASSES/Wieloboki.cpp"
    "${WBRTM}/MISCCLASSES/TabelaTabDeli.cpp"
    "${WBRTM}/BASE/error_enter_before_clean.c"
#    "${WBRTM}/MISCCLASSES/Nodemem.cpp"
    )
endif()

add_library( wbgdrasters STATIC
    "${WBRTM}/RASTERS/gdPCXrea.cpp"
    "${WBRTM}/RASTERS/gd_image.cpp"
    "${WBRTM}/RASTERS/gdbmprea.cpp"
    "${CONTRLIBS}/GD2/gd.c" #
#    "${WBRTM}/RASTERS/gdbmprea2.cpp" #OLD VERSION
    "${WBRTM}/BASE/error_enter_before_clean.c"
    )

if(true) #ADDITIONAL LIBRARIES
add_library( wblexer STATIC
    "${WBRTM}/LEXER/regex.c"
    "${WBRTM}/LEXER/regexpr.cpp"
    "${WBRTM}/LEXER/lexer.cpp"
    "${WBRTM}/MISCCLASSES/Iter_dir.cpp"
    "${WBRTM}/BASE/error_enter_before_clean.c"    
    )

add_library( wbdata STATIC
    "${WBRTM}/DATASHT/datashtn.cpp"
    "${WBRTM}/DATASHT/data_sht.cpp"
    "${WBRTM}/DATASHT/datashsv.cpp"
    "${WBRTM}/DATASHT/datashls.cpp"
    "${WBRTM}/DATASHT/datashtf.cpp"
    "${WBRTM}/DATASHT/datasht1.cpp"
    "${WBRTM}/DATASHT/datashtt.cpp"
    "${WBRTM}/DATASHT/datashio.cpp"
    "${WBRTM}/DATASHT/datawarn.cpp"
    "${WBRTM}/DATASHT/datashcs.cpp"
    "${WBRTM}/BASE/error_enter_before_clean.c"    
    )
endif()

if(true) #PRE STL CONTINERS
add_library( wbcontiners STATIC
    "${WBRTM}/CONTAINERS/_ikeycon.cpp"
    "${WBRTM}/CONTAINERS/Str2IntDict.cpp"
    "${WBRTM}/CONTAINERS/Bitarray.cpp"
    "${WBRTM}/CONTAINERS/Absolete/Resources.cpp"
    "${WBRTM}/CONTAINERS/Absolete/EnvEntry.cpp"
    "${WBRTM}/CONTAINERS/Absolete/EnvCache.cpp"
    "${WBRTM}/CONTAINERS/AssoBase.cpp"
    "${WBRTM}/CONTAINERS/Absolete/ConfigRc.cpp"
    "${WBRTM}/BASE/error_enter_before_clean.c"    
    )
target_compile_options( wbcontiners PRIVATE "-fpermissive" )
endif()

#VERY ABSOLETE

if(false) #BASIC like string class
add_library(wbstrings STATIC
    "${WBRTM}/SOSSTRING/sosstr7.cpp"
    "${WBRTM}/SOSSTRING/sosstr3.cpp"
    "${WBRTM}/SOSSTRING/sosstr6.cpp"
    "${WBRTM}/SOSSTRING/sosstr2.cpp"
    "${WBRTM}/SOSSTRING/sosstr9.cpp"
    "${WBRTM}/SOSSTRING/sosstr1.cpp"
    "${WBRTM}/SOSSTRING/sosscat.cpp"
    "${WBRTM}/SOSSTRING/sosstr8.cpp"
    "${WBRTM}/SOSSTRING/sosstr0.cpp"
    "${WBRTM}/SOSSTRING/sosstr5.cpp"
    "${WBRTM}/SOSSTRING/sosstr4.cpp"
    )
endif()

if(false) # wbrtm Posix comp. for old Windows
    add_library(wbwinposix STATIC
     "${WBRTM}/_COMPAT/winmain.c"
#    "${WBRTM}/_COMPAT/wb_sleep_win.c"
     "${WBRTM}/_COMPAT/wb_spawn_unix.c" #very old
#    "${WBRTM}/_COMPAT/wb_posix_win.c"
#    "${WBRTM}/MISCCLASSES/Threads.cpp"
#    "${WBRTM}/MISCCLASSES/Sockets.cpp" #rene.nyffenegger@adp-gmbh.ch. WINDOWS ONLY
#    "${WBRTM}/MISCCLASSES/WebServer.cpp" #WINDOWS ONLY
    )
endif()

install(TARGETS wbrtm  wbsyshX11  wbcontiners  wbdata  wbgdrasters  wblexer  
#       wbsps  
        wbsymshell  wbsymshlight  
#       wbsyshSVG  
        wbsyshX11 DESTINATION lib)

## PROGRAMY TESTOWE ##
######################


#"${WBRTM}/TESTS/bitarraytest.cpp"

if(false)
##
#"${WBRTM}/CONTAINERS/_nottested2013/setbase.cpp"
#"${WBRTM}/CONTAINERS/_nottested2013/hdiction.cpp"
#"${WBRTM}/CONTAINERS/_nottested2013/dictofch.cpp"
#"${WBRTM}/CONTAINERS/_nottested2013/cgi_conf.cpp"
##

##
#"${WBRTM}/SYMSHELL/tests/symshelllinktest.cpp"
#"${WBRTM}/SYMSHELL/tests/Testsyms.cpp"
#"${WBRTM}/SYMSHELL/tests/Testsysh.cpp"
#"${WBRTM}/SYMSHELL/tests/Testarea.cpp"
#"${WBRTM}/SYMSHELL/tests/LapunowNew1_61.c
#"${WBRTM}/SYMSHELL/tests/LapunowNew2_1.cpp"
#"${WBRTM}/SYMSHELL/tests/LapunowNew.c
#"${WBRTM}/SYMSHELL/tests/LapunowNew2.c
#"${WBRTM}/SYMSHELL/tests/Testrand.cpp"

##
#"${WBRTM}/SYMSHELL/tests/Testmenu.c
#"${WBRTM}/SYMSHELL/tests/Testmngr.cpp"
#"${WBRTM}/SYMSHELL/tests/Testgena.cpp"
#"${WBRTM}/SYMSHELL/tests/Testpoly.cpp"
#"${WBRTM}/SYMSHELL/tests/Testpale.cpp"
#"${WBRTM}/SYMSHELL/tests/Testextd.cpp"
#"${WBRTM}/SYMSHELL/tests/Testmous.cpp"
#"${WBRTM}/SYMSHELL/symshskeleton.cpp"
#"${WBRTM}/SYMSHELLLIGHT/tests/Testsyms.cpp"
#"${WBRTM}/SYMSHELLLIGHT/tests/Testsysh.cpp"
#"${WBRTM}/SYMSHELLLIGHT/tests/Testrand.cpp"
#"${WBRTM}/SYMSHELLLIGHT/tests/Testpoly.cpp"
#"${WBRTM}/SYMSHELLLIGHT/tests/Testpale.cpp"
#"${WBRTM}/SYMSHELLLIGHT/tests/Testmous.cpp"
##

##
#"${WBRTM}/SYMSHELL/src/Unix/sshmenudx11.c
#"${WBRTM}/SYMSHELL/src/Unix/help_apps/sshmenux11.c
#"${WBRTM}/SYMSHELL/src/Unix/help_apps/xmenu8.c
#"${WBRTM}/SYMSHELL/src/Unix/help_apps/menux11.c
#"${WBRTM}/SYMSHELL/src/Unix/help_apps/menux11old.c
##

##
#"${WBRTM}/SYMSHELL/tests/X11menu/sshmenuf.c"
#"${WBRTM}/SYMSHELL/tests/X11menu/sshmenux11.c"
#"${WBRTM}/SYMSHELL/tests/X11menu/sshmenud.c.ckp"
#"${WBRTM}/SYMSHELL/tests/X11menu/motif_mn.c"
#"${WBRTM}/SYMSHELL/tests/X11menu/symshx11.c"
#"${WBRTM}/SYMSHELL/tests/X11menu/testmenu.c"
#"${WBRTM}/SYMSHELL/tests/X11menu/menusmpl.c"
#"${WBRTM}/SYMSHELL/tests/X11menu/menusmp2.c"
#"${WBRTM}/SYMSHELL/tests/X11menu/sshmenud.c"
#"${WBRTM}/SYMSHELL/tests/X11menu/emptymenu.c"
#"${WBRTM}/SYMSHELL/tests/X11menu/wbx11ico.c"
##
endif()

#KONIEC - THE END
#********************************************************************/
#*              SYMSHELLLIGHT  version 2022-08-12                   */
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

