// Klasa pomocnicza zarządzająca nazwą obiektu serii danych i nie tylko
// //////////////////////////////////////////////////////////////////////
#ifndef __TITLEUTIL_HPP__
#define __TITLEUTIL_HPP__
#include <cstddef>
#include "wb_clone.hpp"
//using namespace wbrtm;
using wbrtm::clone_str;

class title_util
//----------------------------------------------------------
{
protected:
char* title;
public:
//Destructor 
~title_util()
	{ 
	if(title) 	
		delete title;
	}

//Constructor
title_util(const char* tit=NULL)
	{
	if(tit!=NULL)
		title=clone_str(tit);
		else
		title=NULL;
	}

void settitle(const char* tit)			
	{ 
	if(title) 
		delete title;
	if(tit!=NULL)
		title=clone_str(tit);
		else
		title=NULL;
	}

const char* gettitle()	//Zwraca nazwę albo NULL
	{
	return title;
	}

const char* name()	//Zwraca nazwę albo pusty łańcuch
	{ 
	return title!=NULL?title:"";
	}

};


#endif
