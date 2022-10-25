/** \file wb_clone.hpp
*	\brief WB CLONING SUPPORT FOR ANY C++ OBJECTS
*          --------------------------------------
* \details
*       Content:
*       - wbrtm::clone_str - Function for cloning strings;
*       - wbrtm::clone - Template function for cloning scalars;
*       - wbrtm::Clone - Template class for forced cloning;
*                     In the class pointers to char could be handled intuitively - by contents, not by pointer value.
*
* \author borkowsk
* \date    2022-10-25 (last modification)
*/

#ifndef _WB_CLONE_HPP_
#define _WB_CLONE_HPP_

#ifndef __cplusplus
#error C++ required
#endif

#include <string.h>

///\namespace wbrtm \brief WOJCIECH BORKOWSKI RUN TIME LIBRARY
namespace wbrtm {

/// \brief Kopiuje stały łańcuch znaków na stertę
/// \return  NULL jeśli nie może
inline char* clone_str(const char *const p)
{
char* out=NULL;
size_t size = ::strlen(p) + 1;
if(p!=NULL)
	out=new char[size];
if(out!=NULL)	
	::strcpy_s(out,size,p);
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

/// \brief   Funkcja szablonu klonowania obiektu na stertę,
/// \return  NULL jeśli nie może zaalokować!
template<class T>
#ifndef __BORLANDC__
inline 			//W Borlandzie 4.X powoduje pad :-D - a to ci stara historia!
#else
static
#endif
T* clone(const T* p)
{
    return (p!=NULL?new T(*p):NULL);
}

///	\brief CLASS alternative (FIXING TYPE) of cloning template
template<class T>
class Clone
{
    T* ptr;
public:
    Clone(const T* par){ ptr=new T(*par);}
    operator T* () {return ptr;}
};

///	\brief "char" specialization of CLASS alternative of cloning template
template<>
class Clone<char>
{
    char* ptr;
public:
    Clone(const char *const par){ ptr=clone_str(par);}
    operator char* () {return ptr;}
};

} //namespace
/* ******************************************************************/
/*              SYMSHELLLIGHT  version 2022                         */
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
