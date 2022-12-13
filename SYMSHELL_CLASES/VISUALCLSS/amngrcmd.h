//Komendy menagera obszarów dla shell'a symulacji
/////////////////////////////////////////////////////
#if 1
#define SSH_ONESTEP                      50001
#define SSH_STARTSTOP                    50002

#define SSH_FILE_DUMPSCREEN              50011
#define SSH_FILE_EXIT                    50012


#define SSH_WINDOWS_TILE_ALL             50020
#define SSH_WINDOWS_HIDEMARKEDAREAS      50021
#define SSH_WINDOWS_UNCOVERHIDDENAREAS   50022
#define SSH_WINDOWS_TILEMARKEDAREAS      50023
#define SSH_WINDOWS_RESTORETOORGINALPOSITION 50024
#define SSH_WINDOWS_MARKALLAREAS         50025
#define SSH_WINDOWS_UNMARKALLAREAS       50026

#define SSH_HELP_SHORTCUTHELP            50091
#define SSH_HELP_AUTHORSWWWPAGE          50092

#define SSH_FIRST_FREE_MESSAGE           50100

#else

#define SSH_ONESTEP                      40010
#define SSH_STARTSTOP                    40011

#define SSH_FILE_DUMPSCREEN              40012
#define SSH_FILE_EXIT                    40013

#define SSH_WINDOWS_TILE_ALL             40014
#define SSH_WINDOWS_HIDEMARKEDAREAS      40015
#define SSH_WINDOWS_UNCOVERHIDDENAREAS   40016
#define SSH_WINDOWS_TILEMARKEDAREAS      40017
#define SSH_WINDOWS_RESTORETOORGINALPOSITION 40018
#define SSH_WINDOWS_MARKALLAREAS         40019
#define SSH_WINDOWS_UNMARKALLAREAS       40020

#define SSH_HELP_SHORTCUTHELP            40021
#define SSH_HELP_AUTHORSWWWPAGE          40022

#define SSH_FIRST_FREE_MESSAGE           40030

#endif
