// /////////////////////////////////////////////////////////////////////////////////////////
//							Przykladowy program SYMSHELLA
//-----------------------------------------------------------------------------------------
// Demonstracja stosowania obiektowego pietra SYMSHELLa
// Tworzy kilka przykladowych seri danych i umieszcza je w menagerze danych.
// Tworzy kilka obszarow i umieszcza je pod zarzadem menagera obszarow.
// Oddaje sterowanie menagerowi obszarow opartemu na SYMSHELU
// /////////////////////////////////////////////////////////////////////////////////////////

#include <math.h> //cos jest potrzebne

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>

#include "simpsour.hpp"
#include "funcsour.hpp"
#include "sourmngr.hpp"
#include "gadgets.hpp"
#include "graphs.hpp"
#include "areamngr.hpp"     
#include "mainmngr.hpp"

//#include "symshell.h" - niepotrzebne - juz opakowane obiektami

//int x,y,vx,vy;
//ROZMIARY OKNA
const SWIDTH=390;
const SHEIGHT=280;

class tangens//Klasa funkcyjna opakowujaca tangens
{
public:
double operator () (double x)
	{ return tan(x);}
};




// Tablice przykladowych danych
int dane1[25]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,25,26};
double dane2[25]={-2.1,-1.8,-1.2,-0.96,-0.45,
				 -0.1, 0.1,0.48, 0.88, 1.33,
				  1.8, 2.3, 3.1,  4.5,  6.6,
				  7.8, 6.7, 3.6,  2.3,  1.9,
				  1.1, 0.5,-0.1, -0.9,-1.99};
int dane3[25]={	0,1,0,1,0,
    			1,0,1,0,1,
				0,1,0,1,0,
				1,0,1,0,1,
				0,1,0,1,0};
float dane4[25]={15.7f,13.8f,33.33f,18.1f,18.8f,
				  1.1f,0.88f,0.11f,0.11f,0.87f,
				  0.99f,17.0f,19.0f,22.0f,11.4f,
				  14.5f,0.86f,0.14f,0.15f,0.89f,
				  15.33f,11.1f,32.13f,15.78f,19.9f,};
int dane5[]={64,49,36,25,16,9,4,1,0};

int connections_s[]=      {1,1,2,2,2,3,4,5,5,5,7, 8, 8, 8,9,10,11,20};
const size_t conlen=sizeof(connections_s)/sizeof(*connections_s);
int connections_t[conlen]={2,3,3,4,5,6,6,7,8,9,6,10,11,13,4,3,5,21};
float arrows[conlen]=       {0,1,0,1,0,1,1,1,1,1,1, 1, 1, 1,1,1,1,0.5};
float aweights[conlen]=     {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};

sources_menager Series  //Zarzadca danych i jego serie
			(16,
			new matrix_source<int>("A-matrix",5,5,dane1),
			new array_source<int>(sizeof(dane1)/sizeof(*dane1),dane1,"A-data"),
			new matrix_source<double>("B-matrix",5,5,dane2),
			new array_source<int>(sizeof(dane3)/sizeof(*dane3),dane3,"C-data"),
			new array_source<float>(sizeof(dane4)/sizeof(*dane4),dane4,"D-data"),
			new function_source<sinus>(25,0,2*3.141595,"Sin(x)"),
			new function_source<cosinus>(25,0,2*3.141595,"Cos(x) ",-1,1),//Podane dokladne min i max
			//new array_source<int>(sizeof(dane5)/sizeof(*dane5),dane5,"Parabola ");
            new array_source<int>(conlen,connections_s,"Sources"),
            new array_source<int>(conlen,connections_t,"Targets"),
            new array_source<float>(conlen,arrows,"Directed"),
            new array_source<float>(conlen,aweights,"Weights"),
			NULL);

//Menagera danych nie sortuje serii i trzyma je w takiej 
//kolejnosci jak programista je wkladal w konstruktorze i pozniej.
//Dla bardziej zaawansowanych zarzadcow seri moga istniec dodatkowe
//tablice indeksujace, ale powyzsza wlasciwosc powinna byc zachowana.

//void test_series(){}

main_area_menager Lufciki(100,SWIDTH,SHEIGHT);

void tworz_lufciki_testowe() //Generowanie lufcikow demostracyjnych
{
graph* pom;
int ret=0;

//Przyklejenie do niektorych seri nie-domyslnych atrybutow
ret=Series.set_info(Series.search("A-data"),default_color,new hash_point);
ret=Series.set_info(Series.search("C-data"),default_color,new circle_point);
		
//Tworzenie obszarow z wykresami
pom=new carpet_graph(1,1,89,89,5,5,Series.get(1));      assert(pom!=NULL);
pom->settitle("CARPET GRAPH");
//pom->setdatacolors(16,255);//Jesli Zoom jest bez torusa to rezerwuje czarny na miss-value {????????}
Lufciki.insert(pom);

pom=new carpet_graph(90,1,179,89,Series.get(0));       assert(pom!=NULL);
pom->settitle("ZOOM-TORUS");
pom->setbackground(default_half_gray);
Lufciki.insert(pom);

/*
pom=new carpet_graph(90,1,179,89,5,5,Series.get(2));     assert(pom!=NULL);
pom->setdatacolors(32,132);
pom->setframe(150);
pom->settitle("FIXED SIZE CARPET GRAPH");
Lufciki.insert(pom);
pom->settitle("");
*/
/*{
int subtab[]={0,0,3,3};//Wiemy ze to jest seria maciezowa!
					   //W przeciwnym wypadku trzeba by sprawdzic metoda box()	
int ret=Series.get(0)->sub(subtab);//Ustalenie wycinka tablicy
assert(ret==0);//Wycinek prawidlowy
}*/

//pom=new carpet_graph(270,1,359,89,4,4,Series.get(3));
//pom->settitle("CHESS-BOARD");
//pom->setframe(200);
//pom->setdatacolors(254,255);
//Lufciki.insert(pom);
pom=new net_graph(180,1,269,89,Series.get(4),0,//X
                               Series.get(2),0,//Y
                               Series.get(7),0,//Connection starts 
                               Series.get(8),0,//Connection ends
                               Series.get(1),0,//Colors
                               0,0,
                               Series.get(9),0, //Arrows heads
                               Series.get(10),0 //Arrows weights
                               );
pom->settitle("NETWORK GRAPH");
pom->setframe(200);
pom->setdatacolors(0,255);
Lufciki.insert(pom);


pom=new bars_graph(1,90,89,179,Series.get(4),0,Series.get(1),0,0);     assert(pom!=NULL);
pom->settitle("2D BARS GRAPH");
pom->setframe(200);
pom->setdatacolors(0,255);
Lufciki.insert(pom);

pom=new bars_graph(90,90,179,179,Series.get(2));            assert(pom!=NULL);
pom->settitle("BLUE 2D BARS");
Lufciki.insert(pom);
pom->settitle("");

pom=new rainbow_graph(300/*270*/,180,359,269,Series.get(4),0,Series.get(1),0,"%@C%8.2f %%");    assert(pom!=NULL);
pom->settitle("RAINBOW GRAPH");
pom->setframe(88);
Lufciki.insert(pom);

pom=new scatter_graph(180,90,269,179,Series.get(4),0,
                                     Series.get(2),
                                     0,0,0,0,0,new circle_point,1);    assert(pom!=NULL);
pom->settitle("SCATTER PLOT");
pom->setframe(180);
pom->setbackground(default_light_gray);
Lufciki.insert(pom);

/*{
config_point confstruct; printf
confstruct.ptr=new circle_point;
confstruct.menage=1;
pom->configure(&confstruct);
}*/

pom=new scatter_graph(270,90,359,179,          Series.get(4),0,
								               Series.get(2),0,
											   Series.get(1),0,
											   Series.get(3),0);    assert(pom!=NULL);
pom->setdatacolors(1,254);
pom->settitle("CROSS POINT SCATTER PLOT");
Lufciki.insert(pom);
pom->settitle("");

pom=new sequence_graph(1,180,89,269,4,Series.make_series_info(3,4,5,6,-1).get_ptr_val());    assert(pom!=NULL);
pom->settitle("TIME SERIES");
Lufciki.insert(pom);

pom=new sequence_graph(90,180,179,269,4,Series.make_series_info(0,2,3,4,-1).get_ptr_val(),
															  1,-5/*Wspolne min/max*/);    assert(pom!=NULL);
															//2,-10,40/*Fixed min/max*/);															
															//2,-0.0001,18/*Za maly zakres min/max */);    assert(pom!=NULL);
pom->settitle("OTHER TIME SERIES");
pom->setframe(253);
Lufciki.insert(pom);
pom->settitle("");

pom=new manhattan_graph(180,180,299,269,Series.get(2),0,Series.get(0),0);    assert(pom!=NULL);
pom->settitle("MANHATTAN PLOT");
Lufciki.insert(pom);

//Tworzenie obszaru sterujacego
wb_dynarray<rectangle_source_base*> tmp(2,(rectangle_source_base*)Series.get(0),
										  (rectangle_source_base*)Series.get(2));
drawable_base* pom2=new steering_wheel(270,1,360,89,tmp);			    assert(pom2!=NULL);

pom2->setbackground(10);
Lufciki.insert(pom2);
pom2->settitle("");
}


/*  OGOLNA FUNKCJA MAIN */
/************************/

main(int argc,const char* argv[])
{
int i=0;
int cont=1;//flaga kontynuacji
int std=0;

//test_series();//Sprawdzanie poprawnosci seri

printf("SYMSHELL's AREA MENAGER TEST\n");
printf("Use -help for graphisc setup information.\n");
printf("Ctrl-I for HELP, Ctrl-Q to quit.\n");
/*
RANDOMIZE();
*/
if(!Lufciki.start("SYMSHELL's AREA MENAGER TEST",argc,argv))
		{
		 printf("%s\n","Can't initialize graphics");
		 exit(1);
		}
tworz_lufciki_testowe();

Lufciki.run_input_loop();//ODDAJE STEROWANIE! NIE MOZNA W TEN SPOSOB ROBIC SYMULACJI

printf("Do widzenia!!!\n");
return 0;
//Gdzies tu,albo troche dalej destruktory...
}

