/// @file
/// @brief **Implementations for the simulation layer base class** /<br>
///         _Implementacje dla klasy bazowej warstwy symulacji._
/// @date 2026-05-16 (modified)
//*////////////////////////////////////////////////////////////////////
// Klasy czysto wirtualnej, ale implementacje funkcji muszą (powinny być).
//
#include <climits>

#define USES_STDC_RAND
#include "random.h"

#include "layer.hpp"
#include "../visual/toitoutoll.hpp"

//Sprawdzenie, czy jest agent w tym miejscu.
bool symshell2::rectangle_layer::filled(int /*X*/,int /*Y*/)
{
    return true;
}

//Czyszczenie linii.
[[maybe_unused]]//NIGDY JESZCZE NIE ZAIMPLEMENTOWANE? TODO
void symshell2::rectangle_layer::clean_line(int /*X1*/,int /*Y1*/,int /*X2*/,int /*Y2*/)
{}

//Czyszczenie wiersza punktów.
void symshell2::rectangle_layer::clean_horizontal(int xxp,int yyp,size_t n)
{
    for(unsigned i=0;i<n;i++)
        clean(xxp+i,yyp);
}

//Czyszczenie losowo wybranych elementów.
void symshell2::rectangle_layer::clean_randomly(int how_many)
{
    int Width=toi(this->MainGeometry.get_width());
    int Heigh=toi(this->MainGeometry.get_height());									assert(how_many<Width*Heigh); //Inaczej pętla nieskończona

    for(int i=0;i<how_many;i++)
    {
        int x=RANDOM(Width);
        int y=RANDOM(Heigh);

        if(filled(x,y)) //Troche nieefektywne, ale kiedyś się skończy.
            clean(x,y);                  
        else
            i--;        //Nie trafiony, jeszcze raz
    }
}

//Czyszczenie kola
void symshell2::rectangle_layer::clean_circle(int x,int y,size_t r)
{
    if(r==0) return;
    // Robi dziurę w obszarze symulacji.
    if(r==1)
        { clean(x,y); return; }
    //ClearEllipse(x,y,r,r); // poniżej rozwinięta "na małpę" żeby było szybciej
    int xxs=x;  //Przypisanie pseudoparametrów
    int yys=y;
    int bb=toi(r);
    int aa=toi(r);
    {
    register int xxp,yyp,n;
    register int xs=xxs;
    register int ys=yys;

    long a2=aa*aa;
    long b2=bb*bb;
    long a2s=a2;
    long b2s=b2*(2*aa+1);
    long DELTAi=2*(1-bb);

    if(aa==0 || bb==0)
            return ;

    register int xi=0;
    register int yi;
    //Inicjacja yi
    //#if sizeof(int) < sizeof(long) //by?o sizeod(ind) ale to raczej literówka TODO CHECK
    if(aa<(long)-INT_MAX)
                yi=-INT_MAX;
    else if(aa>(long)INT_MAX) //może być true jak sizeof(int)<sizeof(long) - było sizeod(ind) ale to raczej literówka
                yi=INT_MAX;
         else
    //#endif
         yi=aa;

    yyp=INT_MAX;
    while( yi>=0 )
    {
    xxp=xs-xi; //A inicjacja xi? - jest wcześniej
    if(yyp!=yi) // Nowa linia
        {
        n=2*xi+1;
        yyp= ys-yi;
        clean_horizontal(xxp,yyp,n);
        yyp= ys+yi;
        clean_horizontal(xxp,yyp,n);
        }
        else // Uzupełnienie linii o punkty brzeżne
        {
        xxp=xs+xi;  yyp= ys+yi;
            clean(xxp , yyp );
        xxp=xs-xi;  yyp= ys-yi;
            clean(xxp , yyp );
        xxp=xs+xi;  yyp= ys-yi;
            clean(xxp , yyp );
        xxp=xs-xi;  yyp= ys+yi;
            clean(xxp , yyp );
        }
    yyp=yi; // zapamiętaj do porównania
    //BEZRYSOWANIA:
    if(DELTAi<0L) goto _1300;
       else {if(DELTAi==0L)
            goto _1240;
          else
            goto _1180; }

    _1180:			 /* decyzja */
    if( (DELTAi+DELTAi-a2s)<=0L )
            goto _1240;
        else
            goto _1380;
    //continue;

    _1240: 			/* krok ukośny */
    xi++;
    yi--;
    a2s+=a2+a2;
    b2s-=(b2+b2);
    DELTAi+=(a2s-b2s);
    continue;

    _1300:			/* krok poziomy */
    if((DELTAi+DELTAi+b2s)>0L) goto _1240;
    xi++;
    a2s+=a2+a2;
    DELTAi+=a2s;
    continue;
    //goto BEZRYSOWANIA;

    _1380:			/* krok pionowy */
    yi--;
    b2s-=(b2+b2);
    DELTAi-=b2s;
}

}
}

/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
