/// @file
/// @EN{  }
/// @PL{  }
/// @date 2026-05-19 (modified)
/// =========================================================
///
/// @brief Przykładowy program SYMSHELL-z z klasami.
/// @date 2026-05-19 (modified)
///       ------------------------------------------
/// @details
///     Demonstracja stosowania obiektowego pietra SYMSHELL-a
///     Tworzy kilka przykładowych seri danych i umieszcza je w zarządcy danych.
///     Tworzy kilka obszarów i umieszcza je pod zarządem managera obszarów.
///     Ostatecznie oddaje sterowanie zarządcy obszarów opartemu na SYMSHELL-u.
//======================================================================================================================


#include <cmath> //coś tam jest potrzebne

#include <cstdio>
#include <cstdlib>
//#include <iostream>

//#include "simpsour.hpp"
#include "arrasour.hpp"
#include "funcsour.hpp"
#include "mattsour.hpp"
#include "sourmngr.hpp"
#include "gadgets.hpp"
#include "graphs.hpp"

//#include "areamngr.hpp"
#include "mainmngr.hpp"
using namespace sym2;
using namespace sym2::data;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nullptr"
//#include "symshell.h" - niepotrzebne — już opakowane obiektami
//int x,y,vx,vy;

/// @name INICJALNE ROZMIARY OKNA.
/// @{
const int SCR_WIDTH=390;
const int SCR_HEIGHT=280;
/// @}

/// Klasa funkcyjna opakowująca tangens.
class tangens
{
public:
    double operator () (double x)
        { return tan(x);}
};


/// @name Tablice przykładowych danych.
/// @{
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
/// @}

/// Obiekt zarządcy danych. Inicjowany ad hoc tworzonymi seriami czerpiącymi z tablic przykładowych.
/// Zarządca danych nie sortuje serii i trzyma je w takiej kolejności
/// , jak programista je wkładał w konstruktorze i później...
/// Powyższa właściwość powinna być zachowana w klasach potomnych!
sources_manager Series
            (16,
            new matrix_source<int>("A-matrix",5,5,dane1),
            new array_source<int>(sizeof(dane1)/sizeof(*dane1),dane1,"A-data"),
            new matrix_source<double>("B-matrix",5,5,dane2),
            new array_source<int>(sizeof(dane3)/sizeof(*dane3),dane3,"C-data"),
            new array_source<float>(sizeof(dane4)/sizeof(*dane4),dane4,"D-data"),
            new function_source<sinus>(25,0,2*3.141595,"Sin(x)"),
            new function_source<cosinus>(25,0,2*3.141595,"Cos(x) ",-1,1), //Podane dokładne min i max.
            //new array_source<int>(sizeof(dane5)/sizeof(*dane5),dane5,"Parabola ");
            new array_source<int>(conlen,connections_s,"Sources"),
            new array_source<int>(conlen,connections_t,"Targets"),
            new array_source<float>(conlen,arrows,"Directed"),
            new array_source<float>(conlen,aweights,"Weights"),
            NULL);


/// Obiekt zarządcy obszarów okna ("lufcików").
main_area_manager Lufciki(100, SCR_WIDTH, SCR_HEIGHT);

/// Funkcja generowania lufcików demonstracyjnych.
void make_test_areas()
{
    graph* pom;
    int ret=0;

    //Przyklejenie do niektórych seri nie-domyślnych atrybutów.
    ret=Series.set_info(Series.search("A-data"),default_color,new hash_point);		assert(ret!=-1);
    ret=Series.set_info(Series.search("C-data"),default_color,new circle_point);		assert(ret!=-1);

    //Tworzenie obszarów z wykresami
    pom=new carpet_graph(1,1,89,89,5,5,Series.get(1));				assert(pom!=NULL);
    pom->set_title("CARPET GRAPH");
    //pom->set_data_colors(16,255); //Jeśli zoom jest bez torusa, to rezerwuje czarny na miss-value {????????}
    Lufciki.insert(pom);

    pom=new carpet_graph(90,1,179,89,Series.get(0));						assert(pom!=NULL);
    pom->set_title("ZOOM-TORUS");
    pom->set_background(default_half_gray);
    Lufciki.insert(pom);

    /*
    pom=new carpet_graph(90,1,179,89,5,5,Series.get(2));     assert(pom!=NULL);
    pom->set_data_colors(32,132);
    pom->set_frame(150);
    pom->settitle("FIXED SIZE CARPET GRAPH");
    Lufciki.insert(pom);
    pom->set_title("");
    */
    /*{
    int subtab[] = {0,0,3,3}; //Wiemy, że to jest seria macierzowa!
                              //W przeciwnym wypadku trzeba by sprawdzić metoda box()
    int ret=Series.get(0)->sub(subtab); //Ustalenie wycinka tablicy
    assert(ret==0); //Wycinek prawidłowy
    } */

    //pom=new carpet_graph(270,1,359,89,4,4,Series.get(3));
    //pom->set_title("CHESS-BOARD");
    //pom->set_frame(200);
    //pom->set_data_colors(254,255);
    //Lufciki.insert(pom);
    pom=new net_graph(180,1,269,89,Series.get(4),0, //X
                                   Series.get(2),0, //Y
                                   Series.get(7),0, //Connection starts
                                   Series.get(8),0, //Connection ends
                                   Series.get(1),0, //Colors
                                   0,0,
                                   Series.get(9),0, //Arrows heads
                                   Series.get(10),0 //Arrows weights
                                   );																assert(pom!=NULL);
    pom->set_title("NETWORK GRAPH");
    pom->set_frame(200);
    pom->set_data_colors(0, 255);
    Lufciki.insert(pom);


    pom=new bars_graph(1,90,89,179,Series.get(4),0,
                       Series.get(1),0,0);							assert(pom!=NULL);
    pom->set_title("2D BARS GRAPH");
    pom->set_frame(200);
    pom->set_data_colors(0, 255);
    Lufciki.insert(pom);

    pom=new bars_graph(90,90,179,179,Series.get(2));						assert(pom!=NULL);
    pom->set_title("BLUE 2D BARS");
    Lufciki.insert(pom);
    pom->set_title("");

    pom=new rainbow_graph(300/*270*/,180,359,269,Series.get(4),0,
                          Series.get(1),0,"%@C%8.2f %%");				assert(pom!=NULL);
    pom->set_title("RAINBOW GRAPH");
    pom->set_frame(88);
    Lufciki.insert(pom);

    pom=new scatter_graph(180,90,269,179,Series.get(4),0,
                                         Series.get(2),
                                         0,0,0,0,0,
                                         new circle_point,1);							assert(pom!=NULL);
    pom->set_title("SCATTER PLOT");
    pom->set_frame(180);
    pom->set_background(default_light_gray);
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
                                                   Series.get(3),0);			assert(pom!=NULL);
    pom->set_data_colors(1, 254);
    pom->set_title("CROSS POINT SCATTER PLOT");
    Lufciki.insert(pom);
    pom->set_title("");

    pom=new sequence_graph(1,180,89,269,4,
                           Series.make_series_info(3,4,5,6,-1).get_ptr_val());			assert(pom!=NULL);
    pom->set_title("TIME SERIES");
    Lufciki.insert(pom);

    pom=new sequence_graph(90,180,179,269,4,
                           Series.make_series_info(0,2,3,4,-1).get_ptr_val(),
                           1,-5/*Wspólne min/max*/);										assert(pom!=NULL);
                                                                //2,-10,40/*Fixed min/max*/
                                                                //2,-0.0001,18/*Za mały zakres min/max */    assert(pom!=NULL);
    pom->set_title("OTHER TIME SERIES");
    pom->set_frame(253);
    Lufciki.insert(pom);
    pom->set_title("");

    pom=new manhattan_graph(180,180,299,269,
                            Series.get(2),0,
                            Series.get(0),0);									assert(pom!=NULL);
    pom->set_title("MANHATTAN PLOT");
    Lufciki.insert(pom);

    //Tworzenie obszaru sterującego — na raty dla debugging-u
    auto* ser1=Series.get(0);                                                   assert(ser1->valid_memory());
    auto* ser2=Series.get(2);                                                   assert(ser2->valid_memory());
    auto* rer1=(rectangle_source_base*)ser1;                                    assert(rer1->valid_memory());
    auto* rer2=(rectangle_source_base*)ser2;                                    assert(rer2->valid_memory());
    wb_dynarray<rectangle_source_base*> tmp_lst(2,rer1,rer2);

    drawable_base* pom2=new steering_wheel(270,1,360,89,tmp_lst);				assert(pom2!=NULL);

    pom2->set_background(10);
    Lufciki.insert(pom2);
    pom2->set_title("");
}


/**  OGÓLNA FUNKCJA MAIN.  */
/* *********************** */

int main(int argc,const char* argv[])
{
    //test_series(); //Sprawdzanie poprawności seri

    printf("SYMSHELL's AREA MANAGER TEST\n");
    printf("Use -help for graphic setup information.\n");
    printf("Ctrl-I for HELP, Ctrl-Q to quit.\n");
    /*
    RANDOMIZE();
    */
    if(!Lufciki.start("SYMSHELL's AREA MANAGER TEST",argc,argv))
    {
        printf("%s\n","Can't initialize graphics");
        exit(1);
    }

    make_test_areas();

    Lufciki.run_input_loop(); //ODDAJE STEROWANIE JAK W Qt! NIE MOŻNA W TEN SPOSÓB ROBIĆ SYMULACJI!

    printf("Bye,bye!!!\n");
    return 0;
    //Gdzieś tu albo trochę dalej destruktory...
}

#pragma clang diagnostic pop

/* ***************************************************************** */
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/* Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk              */
/*        MAIL: borkowsk@iss.uw.edu.pl                               */
/*                               (Don't change or remove this note)  */
/* ***************************************************************** */

