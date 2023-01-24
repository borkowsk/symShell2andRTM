/**  \file     compatyb.hpp
* \details   Plik definiujący funkcje eat_blanks i eat_chars,
*    	     w sposób zależny od platform kompilacji
* **************************************************************************
*  \date 2022-10-25 (last modification)
*/
#ifndef _COMPATYB_HPP_INCLUDED_
#define _COMPATYB_HPP_INCLUDED_

#include "compatyb.h"
#include <cstdio>
#include <cstring>
#include <iostream>

#ifdef unix 
/* Linux & any modern unix part*/

namespace wbrtm {

    /// \brief Zjada wszelkie odstępy ze strumienia.
    /// \note Zamiast istream::eatwhite (z Borlanda)
    inline
    int eat_blanks(std::istream &file)
    {
        int znak = 0;

        while ((znak = file.get()) != file.eof()
               && isspace(znak))//Koniec gdy pierwszy nie bialy
            ;

        if (znak != file.eof()) {
            file.putback(znak);
            return 0;
        } else
            return file.eof();
    }

    /// \brief Zjada ze strumienia znaki, jeśli są w zadanym \p 'charset'.
    inline
    int eat_chars(std::istream &file, const char *charset)
    {
        int znak = 0;

        while ((znak = file.get()) != file.eof()
               && strchr(charset, znak) != NULL)//Koniec gdy pierwszy nie zawarty w zbiorze
            ;

        if (znak != file.eof()) {
            file.putback(znak);
            return 0;
        } else
            return file.eof();
    }

} //end of namespace wbrtm for unix

#elif defined( __MSVC__ )
/* begin of part of MSVC++ */ 

namespace wbrtm {

    /// \brief Zjada wszelkie odstępy ze strumienia.
    /// \note Zamiast istream::eatwhite (z Borlanda)
    inline int eat_blanks(std::istream& file)
    {
        int znak = 0;

        while (  !file.eof() // czy pierwsze sprawdzenie to potrzebne?
            &&   (znak = file.get())!=EOF 
            &&   isspace(znak) 
            )//Koniec gdy pierwszy nie bialy
            ;

        if ( !file.eof() // czy pierwsze sprawdzenie to potrzebne?
            && znak != EOF)
        {
            file.putback(znak);
            return 0;
        }
        else
            return file.eof();
    }

    /// \brief Zjada ze strumienia znaki, jeśli są w zadanym \p 'charset'.
    inline int eat_chars(std::istream& file,const char* charset) //Zjada znaki jesli są w charset
    {
        int znak=0;

        while( (znak=file.get())!=EOF
                && ::strchr(charset,znak)!=NULL 		) //Koniec gdy pierwszy nie zawarty w zbiorze
                        ;

        if(znak!=EOF)
            {
                file.putback(znak);
                return 0;
            }
            else
            return EOF;
    }

} //end of namespace wbrtm for MSVC++

#elif defined(__BORLANDC__)
/* begin of part for Borland - Emabrcadero */

namespace wbrtm {

inline int eat_blanks(std::istream& file) //nakladka na istream::eatwhite w MSVC++
{
	return eat_chars(file," \t\r\n");
}

inline int eat_blanks_2(istream& file) //nakladka na istream::eatwhite w starym MSVC++
{
	file.eatwhite();

	if(!file.eof())
	{
		return 0;
	}
	else
	{
		return EOF;
	}
}

} //namespace wbrtm

#else /* end of Borland-Embarcadero part */
/* others compilers part */

namespace wbrtm {

    inline int eat_chars(std::istream& file, const char* charset) //Zjada znaki jesli sa w charset
    {
        int znak = 0;

        while ((znak = file.get()) != EOF
            && ::strchr(charset, znak) != NULL)//Koniec gdy pierwszy nie zawarty w zbiorze
            ;

        if (znak != EOF)
        {
            file.putback(znak);
            return 0;
        }
        else
            return EOF;
    }

    inline int eat_blanks(std::istream& file)
    {
        return eat_chars(file, " \t\r\n");
    }

} //namespace wbrtm

#endif /* end of other compilers part */

/* ******************************************************************/
/*                WBRTM  version 2022 for GuestXR                   */
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


