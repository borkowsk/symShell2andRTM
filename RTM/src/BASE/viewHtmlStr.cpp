/// @file
/// @brief Call HTML viewer - string version
/// @date 2026-05-18 (modified)
//================================================

#include <cstdlib>
#include <cstdio>
#include <string>
#include "viewHtml.hpp"

using namespace std;

#if defined( _Windows )
// MS Windows Header
#include <Windows.h> 

bool ViewHtml(string URL)
{
    string bufor;
    bufor=string("start /max ")+URL;
    int ret=WinExec(bufor.c_str(),SW_SHOWMINIMIZED); //Jak dla Windows 98

    if(ret==ERROR_FILE_NOT_FOUND  ||  ret==ERROR_PATH_NOT_FOUND )
    {
        bufor=string("cmd /C start /max ")+URL;
        ret=WinExec(bufor.c_str(),SW_SHOWMINIMIZED); //Jak dla Windows NT
    }

    if(ret<33)
    {
        return false; //Strange convention for WinExec
    }

    return true; //but only for starting cmd interpreter, not for its commands
}
#else
#include <iostream>

bool ViewHtml(std::string URL)
{   //http://askubuntu.com/questions/8252/how-to-launch-default-web-browser-from-the-terminal
    string bufor;
    bufor=string("xdg-open \"")+URL+"\" &";
    int ret=system(bufor.c_str());
    cerr<<"Command "<<bufor.c_str()<<" returned with code "<<ret<<endl;
    return true;
}

#endif
/* ****************************************************************** */
/*                WB RTM  version 2006/2022/2026                      */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */

