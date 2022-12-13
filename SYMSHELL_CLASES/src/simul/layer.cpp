//Implementacje dla klasy bazowej warstwy symulacji
// - czysto wirtualnej, ale implementacje funkcji muszą (powinny być)
//*////////////////////////////////////////////////////////////////////
#include <limits.h>
//#include "compatyb.h"
#define USES_STDC_RAND
#include "random.h"

#include "layer.hpp"

//Sprawdzenie czy jest agent w tym miejscu
bool rectangle_layer::filled(int /*X*/,int /*Y*/)
{
	return true;
}

//Czyszczenie lini
void rectangle_layer::clean_line(int /*X1*/,int /*Y1*/,int /*X2*/,int /*Y2*/)
{}   //NIGDY JESZCZE NIE ZAIMPLEMENTOWANE? TODO

//Czyszczenie wiersza punktow
void rectangle_layer::clean_horizontal(int xxp,int yyp,size_t n)
{
for(unsigned i=0;i<n;i++)
	clean(xxp+i,yyp);
}

//Czyszczenie losowo wybranych elementow    
void rectangle_layer::clean_randomly(int how_many)
{
    int Width=this->MainGeometry.get_width();
	int Heigh=this->MainGeometry.get_height();									assert(how_many<Width*Heigh);//Inaczej petla nieskonczona

    for(int i=0;i<how_many;i++)
    {
		int x=RANDOM(Width);
        int y=RANDOM(Heigh);

        if(filled(x,y)) //Troche nieefektywne, ale kiedys sie skonczy
            clean(x,y);                  
        else
            i--;        //Nie trafiony, jeszcze raz
    }
}

//Czyszczenie kola
void rectangle_layer::clean_circle(int x,int y,size_t r)
{
if(r==0) return;
// Robi dziure z obszarze symulacji
if(r==1)
	{ clean(x,y); return; }
//ClearEllipse(x,y,r,r);// poniżej rozwinięta "na małpę" żeby było szybciej
int xxs=x;  //Przypisanie pseudoparametrów
int yys=y;
int bb=r;
int aa=r;
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
//#if sizeof(int) < sizeof(long) //by?o sizeod(ind) ale to raczej literowka TODO CHECK
if(aa<(long)-INT_MAX)
			yi=-INT_MAX;
    else if(aa>(long)INT_MAX) //może być true jak sizeof(int)<sizeof(long) - by?o sizeod(ind) ale to raczej literowka
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
	else // Uzupelnienie linii o punkty brzezne
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
yyp=yi; // zapamietaj do porownania
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

_1240: 			/* krok ukosny */
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

