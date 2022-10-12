/**  \file     compatyb.hpp
* \details  Plik definiujący zastępcze funkcje "C",
*    	    których brak w poszczególnych kompilatorach i systemach
* **************************************************************************
*/

#ifdef unix 
#include <cstdio>
#include <cstring>
#include <iostream>

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

} //end of namespace wbrtm

/* end of unix part */

#elif defined( __MSVC__ )   || defined(__BORLANDC__)
/* begin of part of MSVC++ or Borland-Emabrcadero */

#include <cstdio>
#include <cstring>
#include <iostream>

namespace wbrtm {

    int eat_chars(std::istream& file,const char* charset) //Zjada znaki jesli sa w charset
    {
        int znak=0;

        while( (znak=file.get())!=EOF
                && strchr(charset,znak)!=NULL 		)//Koniec gdy pierwszy nie zawarty w zbiorze
                        ;

        if(znak!=EOF)
            {
                file.putback(znak);
                return 0;
            }
            else
            return EOF;
    }

} //end of namespace wbrtm

#if defined( __MSVC_2000__ )  || defined(__BORLANDC__)
int eat_blanks(std::istream& file) //nakladka na istream::eatwhite w MSVC++
{
	return eat_chars(file," \t\r\n");
}

int eat_blanks_2(istream& file) //nakladka na istream::eatwhite w starym MSVC++
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
#endif
/* end of part of MSVC++ or Borland */

#else
/* other compilers part */
#include <cstdio>
#include <cstring>
#include <iostream>

inline
int eat_chars(std::istream& file,const char* charset) //Zjada znaki jesli sa w charset
{
int znak=0;

while( (znak=file.get())!=EOF
		&& strchr(charset,znak)!=NULL 		)//Koniec gdy pierwszy nie zawarty w zbiorze
				;

if(znak!=EOF)
	{
		file.putback(znak);
		return 0;
	}
	else
	return EOF;
}

LOCAL
int eat_blanks(std::istream& file)
{
    return eat_chars(file," \t\r\n");
}

/* end of other compilers part */
#endif
/* *******************************************************************/
/*		               WBRTM  version 2022                           */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*         WWW:  ...                                                 */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/


