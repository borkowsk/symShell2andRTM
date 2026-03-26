#include <math.h> //cos jest potrzebne

#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "simpsour.hpp"
#include "funcsour.hpp"
#include "sourmngr.hpp"
#include "gadgets.hpp"
#include "graphs.hpp"
#include "areamngr.hpp"     
#include "mainmngr.hpp"
#include "INCLUDE/wb_ptr.hpp"
#include "INCLUDE/wb_rand.hpp"

//int x,y,vx,vy;
const unsigned SWIDTH=360;
const unsigned SHEIGHT=270;

wb_ptr<drawable_base> lufcik[16];//Wskazniki do 16 obszarow wykresow

// Tablice przykladowych danych
int dane1[16]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
double dane2[16]={-2,-1.8,-1.2,-0.96,-0.45,-0.1,0.1,0.48,0.88,1.33,1.8,2.3,3.1,4.5,6.6,7.8};
int dane3[16]={0,1,0,1,
			 1,0,1,0,
			 0,1,0,1,
			 1,0,1,0};
float dane4[16]={15.7f,
				 13.8f,
				 33.33f,
				 18.1f,
				 18.8f,
				 1.1f,
				 0.88f,
				 0.11f,
				 0.11f,
				 0.87f,
				 0.99f,
				 17.0f,
				 19.0f,
				 22.0f,
				 11.4f,
				 14.5f
				};
int dane5[]={64,49,36,25,16,9,4,1,0};
array_source<int> Seria1(sizeof(dane1)/sizeof(*dane1),dane1,"\"A\"data ");
matrix_source<int> Zoom1(4,4,dane1,"\"A\"matrix",0);
array_source<double> Seria2(sizeof(dane2)/sizeof(*dane2),dane2,"\"B\"data ");
array_source<int> Seria3(sizeof(dane3)/sizeof(*dane3),dane3,"\"C\"data ");
array_source<float> Seria4(sizeof(dane4)/sizeof(*dane4),dane4,"\"D\"data ");
function_source<cosinus> Seria5(16,0,3.1415,"Cos(x)",-1,1);//Podane dokladne min i max
function_source<sinus> Seria6(16,0,9.45,"Sin(x)");

//array_source<int> Seria5(sizeof(dane5)/sizeof(*dane5),dane5,"Parabola ");

graph::series_info Series[7]={
	graph::series_info(&Seria3,0,-1,new circle_point,1),
	graph::series_info(&Zoom1,0,-1,new hash_point,1),
	graph::series_info(&Seria1,0,-1),						
	graph::series_info(&Seria2,0,-1),		
	graph::series_info(&Seria4,0,-1),
	graph::series_info(&Seria5,0,-1),
	graph::series_info(&Seria6,0,-1)
							 };
// FUNKCJE TESTUJACA
//------------------------------
void test_series()
{
}

void tworz_lufciki_testowe()
{
graph* pom;

lufcik[0]=pom=new carpet_graph(1,1,89,89,	4,4,&Seria1);
assert(pom!=NULL);
pom->settitle("CARPET GRAPH");
//pom->setdatacolors(16,255);//Jesli Zoom jest bez torusa to rezerwuje czarny na miss-value

lufcik[1]=pom=new carpet_graph(90,1,179,89,	4,4,&Seria2);
assert(pom!=NULL);
pom->setdatacolors(32,132);
pom->setframe(150);

lufcik[2]=pom=new carpet_graph(180,1,269,89,	3,3,&Zoom1);
int subtab[]={2,3,1,3};
//int ret=Zoom1.setsub(subtab);//Wycinek tablicy            ???????????
//assert(ret==0);//Wycinek prawidlowy
assert(pom!=NULL);
//pom->settitle("ZOOM-TORUS");
pom->settitle("ZOOM");

lufcik[3]=pom=new carpet_graph(270,1,359,89,	4,4,&Seria3);
pom->settitle("CHESS-BOARD");
pom->setframe(200);
pom->setdatacolors(254,255);

lufcik[4]=pom=new bars_graph(1,90,89,179,		&Seria1,0,&Seria2,0,0);
assert(pom!=NULL);
pom->settitle("2D BARS GRAPH");
pom->setframe(200);

lufcik[5]=pom=new bars_graph(90,90,179,179,		&Seria2);
assert(pom!=NULL);

lufcik[6]=pom=new rainbow_graph(300/*270*/,180,359,269,&Seria4,0,&Seria1,0,"%g %%");
pom->settitle("RAINBOW GRAPH");
pom->setframe(88);

lufcik[7]=pom=new scatter_graph(180,90,269,179,		&Seria4,0,&Seria2,0,0,0,0,0,new circle_point,1);
pom->settitle("SCATTER PLOT");
pom->setframe(180);
/*{
graph::config_point confstruct;
confstruct.ptr=new graph::circle_point;
confstruct.menage=1;
pom->configure(&confstruct);
}*/

lufcik[8]=pom=new scatter_graph(270,90,359,179,		&Seria4,0,&Seria2,0,&Seria1,0,&Seria3,0);
pom->setdatacolors(1,254);

lufcik[9]=pom=new sequence_graph(1,180,89,269,		4,Series+3);
pom->settitle("TIME SERIES");

lufcik[10]=pom=new sequence_graph(90,180,179,269,		4,Series,1,-5/*Wspolne min/max*/);
																 //2,-10,40/*Fixed min/max*/);															
																 //2,-0.0001,18/*Za maly zakres min/max */);


pom->setframe(253);

lufcik[11]=pom=new manhattan_graph(180,180,299,269,4,4, &Seria4,0,&Seria2,0);
pom->settitle("MANHATTAN PLOT");
}

/* OGOLNA FUNKCJA REPLOT */
/*-----------------------*/
int Index=-1;//Index wyroznionego lucfika
gps_area old_settings;//Stare polozenie wyroznionego lufcika

void mouse_check()
{
int xpos=0,ypos=0,click=0;//Myszowate
get_mouse_event(&xpos,&ypos,&click);
if(Index==-1)
 {
 for(unsigned i=0;lufcik[i];i++)
   if(lufcik[i]->is_inside(xpos,ypos))
	{
	Index=i;
	old_settings.set(1,1,SWIDTH-1,SHEIGHT-1);
	lufcik[i]->swap(old_settings);
	set_char('\r');
	return;
	}
 }
 else
 {
 //Przywracanie
 lufcik[Index]->swap(old_settings);
 Index=-1;
 set_char('\r');			
 }
}

void replot()
{
int old=mouse_activity(0);
///clear_screen();
if(Index==-1)
   for(unsigned i=0;lufcik[i];i++)
	{
	lufcik[i]->clear();
	lufcik[i]->replot();
	}
   else
   {
	lufcik[Index]->clear();
	lufcik[Index]->replot();
   }   
flush_plot();
mouse_activity(old);
}

/*  OGOLNA FUNKCJA MAIN */
/************************/

int main(int argc,const char* argv[])
{
//int i=0;
int cont=1;//flaga kontynuacji
int std=0;
test_series();
mouse_activity(1);
set_background(20);
//buffering_setup(1);/* Wlaczona animacja */
shell_setup("SYMSHELL's PAINT AREA TEST",argc,argv);
printf("COLORS= 256 q-quit s-swich stdout on/off\n"
	   "setup options:\n"
	" -mapped -buffered -bestfont -traceevt\n"
	" + for enable\n - for disable\n ! - for check\n");
/*
RANDOMIZE();
*/
if(!init_plot(SWIDTH,SHEIGHT,0,0))//4x3 Lufciki 90x90
		{
		 printf("%s\n","Can't initialize graphics");
		 exit(1);
		}
tworz_lufciki_testowe();
//replot();
while(cont)
{
char tab[2];
tab[1]=0;
if(input_ready())
   {
	tab[0]=get_char();
   switch(tab[0])
		{
     	case '@':
		case '\r':replot();break;
		case '\b':mouse_check();break;
		case 's':std=!std;break;
   	case 'q':
		case EOF:
		cont=0;
		break;
		}
	if(std)
    {
	 printf("stdout<<%s\n",tab);
	 fflush(stdout);
    }
   flush_plot();
   }

}

close_plot();
printf("Do widzenia!!!\n");
WB_error_enter_before_clean=1;
return 0;
}

/* For close_plot() */
int WB_error_enter_before_clean=0;
