/// @file
/// @brief
///        @PL{ Szablon klasy pozwalającej na zmianę wartości zmiennej w taki sposób, że po wyjściu z bloku przywracana jest wartość pierwotna. }
///        @EN{ A template that allows you to change the value of a variable in such a way that the original value is restored after exiting the block. }
/// @date 2026-06-03 (modified)
///       =====================
/// @note Cała biblioteka WB_RTM to jest nieco odremontowane muzeum kodu z wieku XX.!!!
//=====================================================================================
#ifndef WB_RTM_VARIABLE_CONTROLLER_HPP_INCLUDED_
#define WB_RTM_VARIABLE_CONTROLLER_HPP_INCLUDED_

template<class K, class V>
class interceptor
{
    K* OriginalPos;
    K  OriginalVal;

public:
    interceptor(K& What, const V& NewValue): OriginalPos(&What)
    {
        OriginalVal=*OriginalPos; //Zabezpieczenie starej wartości
        //What=(K&)NewValue;	 //Zamienienie na nową — Niebezpieczne
        //K pom=const_cast<K>(static_cast<const K>(NewValue)); //Za mało uniwersalne
        K pom=(K&)NewValue;     // TODO Niestety nadal niebezpieczne
        *OriginalPos=pom;
    }

    explicit interceptor(K& What): OriginalPos(&What)
    {
        OriginalVal=*OriginalPos; //Zabezpieczenie starej wartości
    }

    ~interceptor()
    {
        *OriginalPos=OriginalVal; //Przywrócenie oryginalnej wartości
    }
};

//template<class K, class V>        //JAK ZDEFINIOWAĆ SZABLONOWY TYPEDEF ???
//typedef interceptor<K,V> przechwytywacz_t; //Tak się niby kompiluje, ale nie działa


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




