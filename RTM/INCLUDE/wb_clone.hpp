/** \file
*	\brief
*       @PL{ OBSŁUGA KLONOWANIA DLA DOWOLNYCH OBIEKTÓW C++. }
*       @EN{ CLONING SUPPORT FOR ANY C++ OBJECTS. }
*   @date 2026-06-03 (last modification)
*          --------------------------------------
*
* \details
*       Content:
*       - wbrtm::clone_str - Function for cloning strings;
*       - wbrtm::clone - Template function for cloning scalars;
*       - wbrtm::Clone - Template class for forced cloning;
*                     In the class pointers to char could be handled intuitively - by contents, not by pointer value.
*
* \note   Like many others wbrtm files created around year 1999.
 *        Cała biblioteka WB_RTM to jest nieco odremontowane muzeum kodu z wieku XX.
* \author borkowsk
*/
#ifndef WBRTM_CLONE_HPP_INCLUDED_
#define WBRTM_CLONE_HPP_INCLUDED_

#ifndef __cplusplus
#error C++ required
#endif

#include <cstring>

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

/// \brief @PL{ Kopiuje stały łańcuch znaków na stertę. } @EN{ .... }
/// \note  @PL{ Jeśli dostaje, NULL zwraca NULL! } @EN{ .... }
/// \return @PL{ Zwraca kopię łańcucha albo NULL, jeśli nie można jej wykonać. }
///         @EN{ .... }
inline char* clone_str(const char *const p)
{
    if (p == NULL) return NULL; //DALSZE DZIAŁANIA SĄ BEZ SENSU!

    size_t size = ::strlen(p) + 1;
    char*  out = new char[ size ];
    //if(out==NULL) return NULL; // TO JUŻ NIE MOŻE SIĘ ZDARZAĆ!

    #ifdef _MSC_VER
       ::strcpy_s(out,size,p);
    #else
       ::strcpy(out,p);
    #endif

    return out;
}

/// \brief   Klonowanie łańcucha znaków zgodne z szablonem funkcyjnym clone()
/// \details Wersje dla const char *const
inline char* clone(const char *const p)
{
    return clone_str(p);
}

/// \brief   Klonowanie łańcucha znaków zgodne z szablonem funkcyjnym clone()
/// \details Wersja dla char* nie różni się niczym. Chyba zbędna. TODO?
inline char* clone(char* p)
{
    return clone_str(p);
}

/// \brief @PL{ Funkcja szablonu klonowania obiektu na stertę. } @EN{ .... }
/// \return @PL{ NULL, jeśli nie może zaalokować! } @EN{ .... }
template<class T>
#ifndef __BORLANDC__
inline 			//W Borlandzie "4.X" powoduje pad. Och  — a to ci stara historia!
#else
static
#endif
T* clone(const T* p)
{
    return (p!=NULL?new T(*p):NULL);
}

///	\brief @PL{ ... } @EN{ CLASS alternative (FIXING TYPE) of cloning template. }
template<class T>
class Clone
{
    T* ptr;
public:
    explicit Clone(const T* par){ ptr=new T(*par);}
    explicit operator T* () {return ptr;}
};

///	\brief @PL{ ... } @EN{ "char" specialization of CLASS alternative for cloning template. }
template<>
class Clone<char>
{
    char* ptr;
public:
    explicit Clone(const char *const par){ ptr=clone_str(par);}
    explicit operator char* () {return ptr;}
};

} //namespace

/* ******************************************************************/
/*                      WBRTM  version 2026                         */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif
