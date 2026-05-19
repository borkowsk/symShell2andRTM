/// @file
/// @brief
///        @PL{ Selektor języka komunikatów ekranowych. }
///        @EN{ On-screen language message selector. }
/// @date 2026-05-19 (modified)
///       ===============================================
/// @details Poprawiony z bardzo starej wersji.
/// @note Cała biblioteka WB_RTM to jest nieco odremontowane muzeum kodu z wieku XX.!!!
#ifndef _WBRTM_LINGO_HPP_
#define _WBRTM_LINGO_HPP_

namespace wbrtm {

    extern "C" {
        /// Selektor języka komunikatów. 0-Polski, 1-Angielski.
        /// To jest zadeklarowane w "sshutils", ale trzeba gdzieś zdefiniować z wartością.
        extern unsigned lang_selector;
    }

    inline
    const char* _lingo(const char* _PL,const char* _IN)
    {
        if(lang_selector==0)
            return _PL;
        else
            return _IN;
    }

} //namespace

#ifndef _LPL
#define _LPL  (wbrtm::_lingo)
#endif

#ifndef lang
#define lang  (wbrtm::_lingo)
#endif

/* ******************************************************************/
/*                     WBRTM  version 2026                          */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif
