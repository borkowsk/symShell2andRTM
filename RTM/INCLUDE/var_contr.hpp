//Szablon klasy pozwalajacej na zmiane wartosci zmiennej w taki sposob
//ze po wyjsciu z bloku przywracana jest wartosc pierwotna.
//*/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_VARIABLE_CONROLER_HPP_
#define _INCLUDE_VARIABLE_CONROLER_HPP_

template<class K, class V>
class przechwytywacz  
{
	K* OrginalPos;
	K  OrginalVal;
public:
	przechwytywacz(K& What,const V& NewValue):OrginalPos(&What)
	{
		OrginalVal=*OrginalPos; //Zabezpieczenie starej wartości
		//What=(K&)NewValue;	 //Zamienienie na nowa - Niebezpieczne
        //K pom=const_cast<K>(static_cast<const K>(NewValue)); //Za malo uniwersalne
        K pom=(K&)NewValue;     //Niestety nadal niebezpieczne
        *OrginalPos=pom;  
	}
	
	przechwytywacz(K& What):OrginalPos(&What)
	{
		OrginalVal=*OrginalPos;//Zabezpieczenie starej wartości
	}

	~przechwytywacz()
	{
		*OrginalPos=OrginalVal;//Przywrócenie oryginalnej wartości
	}
};

//template<class K, class V>        //JAK ZDEFINIOWAC SZABLONOWY TYPEDEF ???
//typedef przechwytywacz<K,V> przechwytywacz_t; //Tak sie kompiluje ale nie dziala

//typedef przechwytywacz<> przechwytywacz_t;   //A tak sie nawet nie kompiluje
//typedef przechwytywacz przechwytywacz_t;     //I tak tez sie nie kompiluje

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




