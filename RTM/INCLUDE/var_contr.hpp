/// @file
/// @brief Szablon klasy pozwalającej na zmianę wartości zmiennej w taki sposób,
///        że po wyjściu z bloku przywracana jest wartość pierwotna.
/// @date 2026-05-11 (modified)
/// @note Cała biblioteka WB_RTM to jest nieco odremontowane muzeum kodu z wieku XX.!!!
//*////////////////////////////////////////////////////////////////////////////////////
#ifndef WB_RTM_VARIABLE_CONTROLLER_HPP_INCLUDED_
#define WB_RTM_VARIABLE_CONTROLLER_HPP_INCLUDED_

template<class K, class V>
class przechwytywacz  
{
	K* OrginalPos;
	K  OrginalVal;
public:
	przechwytywacz(K& What,const V& NewValue):OrginalPos(&What)
	{
		OrginalVal=*OrginalPos; //Zabezpieczenie starej wartości
		//What=(K&)NewValue;	 //Zamienienie na nowa — Niebezpieczne
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
//typedef przechwytywacz<K,V> przechwytywacz_t; //Tak się kompiluje, ale nie działa

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




