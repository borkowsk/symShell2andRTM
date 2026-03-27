/// @file
/// Implementation of the lifeworld class. Virtual output method.
//---------------------------------------------------------------
/// Its is rather simply implementation. You can replace it if you need.
// /////////////////////////////////////////////////////////////////////
/// @date 2026-03-27 (modified)

#include "lworld.h"

int lifeworld::implement_output(ostream& o) const
{
	int ret=world::implement_output(o);
	if(ret!=1) return ret;
	o<<separator;
	o<<MyWidth<<separator;	//Obwod (?) torusa

	o<<IleKate<<separator;	//Ilosc kategori w mapach
	o<<IleSasiad<<separator;	//8==Gestosc sasiedztwa
	o<<OdlSasiad<<separator;	//Rozmiar sasiedztwa

	o<<Noise<<separator<<endl;		//Szum informacyjny
	o<<Agenci<<endl;
	return 1;
}

// /////////////////////////////////////////////
// Example for SYMSHELL CLASSES library.
/// @author Wojciech Borkowski, iss.uw.edu.pl
// https://github.com/borkowsk/symShell2andRTM
/// @copyright 2000 - 2026
// /////////////////////////////////////////////


