/// @file
/// @brief **Default context menu definition for area manager class** /<br>
///         _Domyślna definicja menu kontekstowego dla zarządcy obszarów._
/// @date 2026-05-16 (last modification)
//        ============================================================
//  Created by borkowsk on 17.04.26.

#ifdef __cplusplus
#error This file is only for pure "C" compilation.
#endif

#include "symshell.h"
#include "amngrcmd.h"

/* Domyślna definicja menu kontekstowego dla zarządcy obszarów z biblioteki klas SymShell-a.
 * Default context menu definition for area manager class. */
ssh_menu_item_definition  context_menu_default[]= {
        { "QUIT", SSH_FILE_EXIT }, //                    50012
        { "HELP", SSH_HELP_SHORTCUTHELP  }, //           50091
        { "ABOUT", SSH_HELP_AUTHORSWWWPAGE }, //          50092

        { "One step", SSH_ONESTEP }, //50001
        { "Start/Stop", SSH_STARTSTOP }, //                    50002
        { "Dump screen", SSH_FILE_DUMPSCREEN }, //              50011
        { "-----------", 0 },
        { "Tile All", SSH_WINDOWS_TILE_ALL }, //             50020
        { "Mark All", SSH_WINDOWS_MARKALLAREAS }, //         50025
        { "Unmark All", SSH_WINDOWS_UNMARKALLAREAS }, //       50026
        { "Tile Marked", SSH_WINDOWS_TILEMARKEDAREAS  }, //     50023
        { "Hide All Marked", SSH_WINDOWS_HIDEMARKEDAREAS }, //      50021
        { "Uncover hidden", SSH_WINDOWS_UNCOVERHIDDENAREAS }, //   50022
        { "Restore original", SSH_WINDOWS_RESTORETOORGINALPOSITION }, // 50024

        {"... ", -1 } //Oddaj obsługę programowi (jak prawy klawisz myszy)
    };

unsigned context_menu_default_size= sizeof(context_menu_default) / sizeof(context_menu_default[0]);

/*v******************************************************************/
/*              SYMSHELLLIGHT version 2026-04...                    */
/*v******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/*v******************************************************************/
