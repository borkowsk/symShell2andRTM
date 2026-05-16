/// @file
/// @brief **Implementation of graph classes using RGB coloring** /<br>
///         _IMPLEMENTACJA KLAS GRAFÓW UŻYWAJĄCYCH KOLOROWANIA RGB._
/// @date 2026-05-16 (modification)
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cstdio> //sprintf!!!
#include <iostream>

#include "symshell.h"
#include "graphs.hpp"
#include "wb_ptr.hpp"
#include "toitoutoll.hpp"

using namespace sym2;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

//`void rect(int x1,int y1,int x2,int y2,wb_color frame_c);`

static char* trunc(char* bufor,unsigned width)
{
    while( string_width(bufor) > width ) //Symshell źle zwraca ??? tzn?
    {
        size_t size=strlen(bufor);
        if(size==1) break;
        bufor[size-2]='*';
        bufor[size-1]='\0';
    }
    return bufor;
}

template<class T>
static inline void swap(T& a,T& b)
{
    T c=a;a=b;b=c;
}

template<class T>
static inline int max_(const T& a, const T& b)
{
    return a>b?a:b;
}

template<class T>
static inline int min_(const T& a, const T& b)
{
    return a<b?a:b;
}

//true color carpet_graph
//-------------------------------------------

//DESTRUCTOR
true_color_carpet_graph::~true_color_carpet_graph()
{
    if(menage[0] && data[0]) delete data[0];
    if(menage[1] && data[1]) delete data[1];
    if(menage[2] && data[2]) delete data[2];
    data[0]=data[1]=data[2]=nullptr;
    menage[0]=menage[1]=menage[2]=false;
}

//CONSTRUCTOR(S)
true_color_carpet_graph::true_color_carpet_graph(int ix1,int iy1,int ix2,int iy2,	//Położenie obszaru
                                                 unsigned iA,unsigned iB,			//A-ile kolumn, B-ile wierszy
                                                 data_source_base* RedData,int menage_r,
                                                 data_source_base* GreenData,int menage_g,
                                                 data_source_base* BlueData,int menage_b
                                                 )//data-źródło danych o kolorach
: graph(ix1,iy1,ix2,iy2),AA(iA),BB(iB),menage{false,false,false},data{NULL,NULL,NULL}
{
    data[0]=RedData;menage[0]=menage_r;
    data[1]=GreenData;menage[1]=menage_g;
    data[2]=BlueData;menage[2]=menage_b;
    assert(data[0]!=nullptr || data[1]!=nullptr || data[2]!=nullptr);
    assert(AA>=2 && BB>=2);
}

true_color_carpet_graph::true_color_carpet_graph(int ix1,int iy1,int ix2,int iy2,	//Położenie obszaru.
                                                 data_source_base* RedData,int menage_r, //Serie muszą mieć taką samą geometrię 2D!
                                                 data_source_base* GreenData,int menage_g,
                                                 data_source_base* BlueData,int menage_b
                                                 )//data-źródło danych o kolorach
: graph(ix1,iy1,ix2,iy2),AA(1),BB(1),menage{false,false,false},data{NULL,NULL,NULL}
{
    data[0]=RedData;menage[0]=menage_r;
    data[1]=GreenData;menage[1]=menage_g;
    data[2]=BlueData;menage[2]=menage_b;
    assert(data[0]!=nullptr || data[1]!=nullptr || data[2]!=nullptr);
    const geometry_base* ptr=read_dim(AA,BB);
    assert(ptr!=nullptr);
}

// IMPLEMENTATION OF VIRTUAL METHODS oF carpet_graph
//-----------------------------------------------------
const geometry_base* true_color_carpet_graph::read_dim(size_t& aa,size_t& bb)
{
    const geometry_base* MyGeometry[3]={nullptr,nullptr,nullptr};
    int posit=0;

    if(data[0])
        MyGeometry[posit++]= data[0]->get_geometry();
    if(data[1])
        MyGeometry[posit++]= data[1]->get_geometry();
    if(data[2])
        MyGeometry[posit++]= data[2]->get_geometry();

    if( MyGeometry[0]==nullptr || //Gdyby nie było wcale albo była wadliwa
        MyGeometry[0]->get_dimension()<2 )
    {
        //	A=subtab[1];
        //	B=subtab[3];
        if(!deputy && AA > 1 && BB > 1)
            deputy=new rectangle_geometry(AA, BB, 0); //Nie torus
        else
            return nullptr; //Wadliwe dane
        aa=AA;
        bb=BB;
        return deputy.get_ptr_val();
    }
    else
    {
        //	A=subtab[1];
        //	B=subtab[3];
        geometry::view_info pom[3];
        MyGeometry[0]->get_view_info(&pom[0]);

        if(MyGeometry[1]) //Sprawdzanie, czy inne geometrie są takie same
        {
            if(MyGeometry[0]->get_dimension()!=MyGeometry[1]->get_dimension())
                return nullptr;
            MyGeometry[1]->get_view_info(&pom[1]);
            if(pom[0].dia.X()!=pom[1].dia.X() ||
                pom[0].dia.Y()!=pom[1].dia.Y())
                return nullptr; //Wadliwe dane
        }
        if(MyGeometry[2])
        {
            if(MyGeometry[0]->get_dimension()!=MyGeometry[2]->get_dimension())
                return nullptr;
            MyGeometry[2]->get_view_info(&pom[2]);
            if(pom[0].dia.X()!=pom[2].dia.X() ||
                pom[0].dia.Y()!=pom[2].dia.Y())
                return nullptr; //Wadliwe dane
        }

        aa=size_t(pom[0].dia.X()*2);
        bb=size_t(pom[0].dia.Y()*2);

        return MyGeometry[0];
    }
}

int true_color_carpet_graph::set_series(size_t index, data_source_base* i_data, int i_menage)
//zwraca -1, jeśli indeks za duży
{
    if(index>2) return -1; //Tylko jedna seria
    assert(i_data != nullptr);
    if(menage[index]) delete data[index];
    data[index]=i_data;
    menage[index]=i_menage;
    return 0;
}

data_source_base* true_color_carpet_graph::get_series(size_t index)
//zwraca nullptr, jeśli indeks za duży
{
    if(index>2)
        return nullptr;
    return data[index];
}

void true_color_carpet_graph::_replot() // Rysuje właściwy wykres a pod nim ewentualnie legendę
{
    int x1= get_start_x();
    int y1= get_start_y();
    int x2= x1 + get_width() - 1; //-1, bo width obejmuje pierwszy piksel
    int y2= y1 + get_height() - 1;

    assert(x1<=x2); //Czy aby na pewno
    assert(y1<=y2); //Sensowne okno. Może miec zerowy rozmiar, ale nie ujemny

    double min[3]={0,0,0};
    double max[3]={1,1,1};
    double missing[3];

    size_t A=1,B=1; //Lokalne A i B

    const geometry_base* MyGeometry=read_dim(A,B);
    {
        size_t num;
        if(data[0])
            data[0]->bounds(num,min[0],max[0]);
        if(data[1])
            data[1]->bounds(num,min[1],max[1]);
        if(data[2])
            data[2]->bounds(num,min[2],max[2]);
    }

    if(MyGeometry==nullptr || (A<=1 && B<=1) || (min[0]>=max[0]) || (min[1]>=max[1]) || (min[2]>=max[2]))
    {
        print_width(x1,(y1+y2)/2,x2-x1, t_colors.start, get_background(), "%@CInvalid data");
        return;
    }

    //Wartości zwracane, gdy brak danych
    if(data[0])
        missing[0]=data[0]->get_missing();
    if(data[1])
        missing[1]=data[1]->get_missing();
    if(data[2])
        missing[2]=data[2]->get_missing();

    //Do skalowania kolorów
    if(data[0])
        mm[0].set(min[0],max[0],c_range.end-c_range.start+0.999);
    if(data[1])
        mm[1].set(min[1],max[1],c_range.end-c_range.start+0.999);
    if(data[2])
        mm[2].set(min[2],max[2],c_range.end-c_range.start+0.999);

    //R y s o w a n i e  l e g e n d y
    if(t_colors.start != get_background() &&
       char_height('X') < get_height())
    {
        int x=x1;
        int y=toi(y2+1-char_height('X')); //+1, bo y2 ma być zarysowane
        int width=0;
        //----------
        y2=y; //Zabiera dolna część na legendę
        //----------
        if(data[0]!=nullptr)
        {
            width=print_width(x, y,(x2-x)/3*2, 50, get_background(),	//50 to czerwone z palety
                "%s", data[0]->name());

            if(width==0) goto KWADRACIKI;
            else	 x+=width+1;

            width=print_width(x,y,(x2-x)/2,50,
                              c_range.start != get_background()?get_background():c_range.end,
                "<%g",min[0]);

            if(width==0) goto KWADRACIKI;
            else	x+=width+1;

            width=print_width(x,y,x2-x,255,	//Białe na czerwonym z palety
                50,
                ",%g>",max[0]);

            if(width==0) goto KWADRACIKI;
            else	x+=width+5;
        }
        if(data[1]!=nullptr)
        {
            width=print_width(x, y,(x2-x)/3*2, 220, get_background(),	//220 to zielone z palety
                "%s", data[1]->name());

            if(width==0) goto KWADRACIKI;
            else	 x+=width+1;

            width=print_width(x,y,(x2-x)/2,220,
                              c_range.start != get_background()?get_background():c_range.end,
                "<%g",min[1]);

            if(width==0) goto KWADRACIKI;
            else	x+=width+1;

            width=print_width(x,y,x2-x,255,	//Białe na zielonym z palety
                220,
                ",%g>",max[1]);

            if(width==0) goto KWADRACIKI;
            else	x+=width+5;
        }
        if(data[2]!=nullptr)
        {
            width=print_width(x, y,(x2-x)/3*2, 128, get_background(),	//128 to niebieski z palety
                "%s", data[2]->name());

            if(width==0) goto KWADRACIKI;
            else	 x+=width+1;

            width=print_width(x,y,(x2-x)/2,128,
                              c_range.start != get_background()?get_background():c_range.end,
                "<%g",min[2]);

            if(width==0) goto KWADRACIKI;
            else	x+=width+1;

            width=print_width(x,y,x2-x,255,	//Białe na niebieskim z palety
                128,
                ",%g>",max[2]);

            if(width==0) goto KWADRACIKI;
            else	x+=width+5;
        }
    }


    //Rysowanie kwadracików
KWADRACIKI:

    assert(x2>x1);
    assert(y2>y1);
    if(A>=1 && B>=1 && A<=(x2-x1+1) && B<=(y2-y1+1))
    {
        assert(c_range.end-c_range.start>=1);
        //size_t i,j; //Indeksy po wierszach i kolumnach
        int width=x2-x1+1; //Już moga być inne
        int height=y2-y1+1; //Niż dla całego obszaru
        int gristA= width / toi(A);
        int gristB= height / toi(B);
        //Musi być kwadratowo, bo inaczej jest nieładnie
        if(gristA > 1 && gristB > 1)
        {
            if(gristA > gristB) gristA=gristB;
            else gristB=gristA;
        }
        else
        {
            gristA= gristB=1; //Pikselami panowie!!!
        }

        int offsetA= (width - gristA * toi(A)) / 2;
        int offsetB= (height - gristB * toi(B)) / 2;

        //Rysowanie
        data_source_base::iterator_h h=MyGeometry->make_view_iterator();
        //wb_color back= get_background(); //Dla sprawdzania, kiedy kolor kwadratu taki jak kolor tla.

        if(gristA == 1) //starczy jedna sprawdzić, bo kwadrat
        { //Pikselami panowie!!!
            for(int j=0;j<B;j++)
                for(int i=0;i<A;i++)
                {																		assert(h!=nullptr);
            size_t G_ind=MyGeometry->get_next(h);
            double test[3]={0,0,0};
            wb_color color[3]={0,0,0}; //Na razie wb_color jest unsigned, więc działa, ale ...
            if(data[0]!=nullptr)
            {
                test[0]=data[0]->get(G_ind);
                if(test[0]!=missing[0])
                {
                    color[0]=dtou(mm[0].get(test[0]))+c_range.start;					assert(color[0]<=c_range.end);
                    if(color[0]>c_range.end)
                        goto NIE_DA_SIE; //Awaria
                }
            }
            if(data[1]!=nullptr)
            {
                test[1]=data[1]->get(G_ind);
                if(test[1]!=missing[1])
                {
                    color[1]=dtou(mm[1].get(test[1]))+c_range.start;					assert(color[1]<=c_range.end);
                    if(color[1]>c_range.end)
                        goto NIE_DA_SIE; //Awaria
                }
            }
            if(data[2]!=nullptr)
            {
                test[2]=data[2]->get(G_ind);
                if(test[2]!=missing[2])
                {
                    color[2]=dtou(mm[2].get(test[2]))+c_range.start;					assert(color[2]<=c_range.end);
                    if(color[2]>c_range.end)
                        goto NIE_DA_SIE; //Awaria
                }
            }
            plot_rgb(offsetA+x1+i,offsetB+y1+j,color[0],color[1],color[2]);
            //else goto NIE_DA_SIE;
            }
        }
        else
        {//Kwadratami
            for(int j=0;j<B;j++)
                for(int i=0;i<A;i++)
                {																		assert(h!=nullptr);
            size_t G_ind=MyGeometry->get_next(h); //rectangle_geometry
            double test[3]={0,0,0};
            wb_color color[3]={0,0,0}; //Na razie wb_color jest `unsigned`, wiec działa, ale ...
            if(data[0]!=nullptr)
            {
                test[0]=data[0]->get(G_ind);
                if(test[0]!=missing[0])
                {
                    color[0]=dtou(mm[0].get(test[0]))+c_range.start;					assert(color[0]<=c_range.end);
                    if(color[0]>c_range.end)
                        goto NIE_DA_SIE; //Awaria
                }
            }
            if(data[1]!=nullptr)
            {
                test[1]=data[1]->get(G_ind);
                if(test[1]!=missing[1])
                {
                    color[1]=dtou(mm[1].get(test[1]))+c_range.start;					assert(color[1]<=c_range.end);
                    if(color[1]>c_range.end)
                        goto NIE_DA_SIE; //Awaria
                }
            }
            if(data[2]!=nullptr)
            {
                test[2]=data[2]->get(G_ind);
                if(test[2]!=missing[2])
                {
                    color[2]=dtou(mm[2].get(test[2]))+c_range.start;					assert(color[2]<=c_range.end);
                    if(color[2]>c_range.end)
                        goto NIE_DA_SIE; //Awaria
                }
            }
            /*
            if(color==back && gristA>3)
            {
            rect(offsetA+x1+i*gristA,offsetB+y1+j*gristB,
            offsetA+x1+(i+1)*gristA-1,offsetB+y1+(j+1)*gristB-1,
            255!=back?255:0);
            }
            else*/
            {
                set_brush_rgb(color[0],color[1],color[2]);
                fill_rect_d(offsetA+x1+ i * gristA, offsetB + y1 + j * gristB,
                    offsetA+x1+ (i+1) * gristA, offsetB + y1 + (j + 1) * gristB);
            }

            }
        }
        MyGeometry->destroy_iterator(h);
    }
    else
    {
        print_width(x1,(y1+y2)/2,x2-x1, t_colors.start, get_background(), "%@CToo small area");
    }

    return;
NIE_DA_SIE:
    print_width(x1,(y1+y2)/2,x2-x1, t_colors.start, get_background(), "%@CInvalid colors found.");
    print_width(x1,toi((y1+y2)/2+char_height('X')),x2-x1, t_colors.start, get_background(), "%@CProbably min or max not properly set.");
}


int true_color_carpet_graph::_rescale_data_point(const double reals[3],long in_area[3])
// `reals` zawiera jedynie wartość ustalająca kolor albo coś spoza zakresu.
// Zwraca -1, jeśli nie w oknie.
{
    int ile=0;
    if(data[0]!=nullptr && reals[0]<=mm[0].max && reals[0]>=mm[0].min)
    {
        unsigned color=(unsigned)mm[0].get(reals[0]);
        in_area[0]=color+c_range.start;
        ile++;
    }
    if(data[1]!=nullptr && reals[1]<=mm[1].max && reals[1]>=mm[1].min)
    {
        unsigned color=(unsigned)mm[0].get(reals[1]);
        in_area[1]=color+c_range.start;
        ile++;
    }
    if(data[2]!=nullptr && reals[2]<=mm[2].max && reals[2]>=mm[2].min)
    {
        unsigned color=(unsigned)mm[2].get(reals[2]);
        in_area[2]=color+c_range.start;
        ile++;
    }
    return (ile==0?-1:0); //Błąd, jeśli nie było żadnego lub żadne nie było w oknie
}

// true_color_manhattan_graph
//--------------------------------------------

//DESTRUCTOR
true_color_manhattan_graph::~true_color_manhattan_graph()
{
    if(datas && d_menage) delete datas;
    datas=nullptr;
    d_menage=false;
    if(colors[0] && c_menage[0]) delete colors[0];
    colors[0]=nullptr;
    c_menage[0]=false;
    if(colors[1] && c_menage[1]) delete colors[1];
    colors[1]=nullptr;
    c_menage[1]=false;
    if(colors[2] && c_menage[2]) delete colors[2];
    colors[2]=nullptr;
    c_menage[2]=false;
}

//CONSTRUCTOR(S)
true_color_manhattan_graph::true_color_manhattan_graph(int ix1, int iy1, int ix2, int iy2,	//Położenie obszaru
                                                       unsigned iA, unsigned iB,
                                                       data_source_base* i_datas, int menage_d,	//datas-dane o wysokościach
                                                       data_source_base* RedData, int menage_r,
                                                       data_source_base* GreenData, int menage_g,
                                                       data_source_base* BlueData, int menage_b,
                                                       int zero_mod,		//tryb wyświetlania
                                                       double H_offs,		//Ułamek szerokości przeznaczony na perspektywę
                                                       double	V_offs		//Ułamek wysokości  przeznaczony na perspektywę
                                                       )
: graph(ix1,iy1,ix2,iy2), AA(iA), BB(iB),
  datas(i_datas), d_menage(menage_d),
  c_menage{false,false,false},colors{NULL,NULL,NULL},
  mode(zero_mod), c_offset(0), wire(get_background()),
  h_offs(H_offs), v_offs(V_offs)
{
    colors[0]=RedData;c_menage[0]=menage_r;
    colors[1]=GreenData;c_menage[1]=menage_g;
    colors[2]=BlueData;c_menage[2]=menage_b;
    assert(datas!=nullptr);
    assert(AA!=0);
    assert(BB!=0);
    assert(v_offs > 0 && v_offs < 1);
    assert(h_offs > 0 && h_offs < 1);
}

true_color_manhattan_graph::true_color_manhattan_graph(int ix1, int iy1, int ix2, int iy2,	//Położenie obszaru
                                                       data_source_base* i_datas, int menage_d,	//datas-dane o wysokościach
                                                       data_source_base* RedData, int menage_r,
                                                       data_source_base* GreenData, int menage_g,
                                                       data_source_base* BlueData, int menage_b,
                                                       int zero_mod, //tryb wyświetlania
                                                       double H_offs,		//Ułamek szerokości przeznaczony na perspektywę
                                                       double	V_offs		//Ułamek wysokości  przeznaczony na perspektywę
                                                       )
: graph(ix1,iy1,ix2,iy2), AA(1), BB(1),
  datas(i_datas), d_menage(menage_d),
  c_menage{false,false,false},colors{NULL,NULL,NULL},
  mode(zero_mod), c_offset(0), wire(get_background()),
  h_offs(H_offs), v_offs(V_offs)
{
    colors[0]=RedData;c_menage[0]=menage_r;
    colors[1]=GreenData;c_menage[1]=menage_g;
    colors[2]=BlueData;c_menage[2]=menage_b;
    assert(datas!=nullptr);
    assert(AA!=0);
    assert(BB!=0);
    assert(v_offs > 0 && v_offs < 1);
    assert(h_offs > 0 && h_offs < 1);
}

// IMPLEMENTATION OF VIRTUAL METHODS OF true_color_manhattan_graph
//-----------------------------------------------------
const geometry_base* true_color_manhattan_graph::read_dim(size_t& aa,size_t& bb)
{
    geometry_base* MyGeometry= datas->get_geometry();
    if( MyGeometry==nullptr ||
        MyGeometry->get_dimension()<2 )
    {
        //	A=subtab[1];
        //	B=subtab[3];
        if(!deputy)
            deputy=new rectangle_geometry(AA, BB, 0); //Nie torus
        aa=AA;
        bb=BB;
        return deputy.get_ptr_val();
    }
    else
    {
        //Sprawdzenie, czy serie się nie zgadzają — czy mają dokładnie tę samą geometrię.
        if(colors[0]!=nullptr)
        {
            geometry_base* ColGeom= colors[0]->get_geometry();
            if(*MyGeometry!=*ColGeom)
            {//jeśli nie to pozbywamy się składowej
                if(c_menage[0]) delete colors[0];
                colors[0]=nullptr;
                c_menage[0]=false;
            }
        }
        if(colors[1]!=nullptr)
        {
            geometry_base* ColGeom= colors[1]->get_geometry();
            if(*MyGeometry!=*ColGeom)
            {//jeśli nie to pozbywamy się składowej
                if(c_menage[1]) delete colors[1];
                colors[1]=nullptr;
                c_menage[1]=false;
            }
        }
        if(colors[2]!=nullptr)
        {
            geometry_base* ColGeom= colors[2]->get_geometry();
            if(*MyGeometry!=*ColGeom)
            {//jeśli nie to pozbywamy się składowej
                if(c_menage[2]) delete colors[2];
                colors[2]=nullptr;
                c_menage[2]=false;
            }
        }
        geometry::view_info pom;
        MyGeometry->get_view_info(&pom);
        aa=size_t(pom.dia.X()*2);
        bb=size_t(pom.dia.Y()*2);
        return MyGeometry;
    }

}


int true_color_manhattan_graph::configure(const void* p)
{
    const config* conf=((const config*)p);
    if(p==nullptr)
    {
        mode=1;
        c_offset=0;
        wire= get_background();
        return -1;
    }
    else
    {
        mode=conf->zero_mode;
        wire=conf->wire;
        c_offset=conf->color_offset;
        assert(mode==0 || mode==1);
        return 0;
    }
}


int true_color_manhattan_graph::set_series(size_t index, data_source_base* i_data, int i_menage)
//zwraca -1, jeśli indeks za duży
{
    if(index>4) return -1; //Tylko dwie serie
    assert(i_data != nullptr);
    if(index==0)
    {
        if(d_menage) delete datas;
        datas=i_data;
        d_menage=i_menage;
    }
    else
    {
        if(c_menage[index-1]) delete colors[index-1];
        colors[index-1]=i_data;
        c_menage[index-1]=i_menage;
    }
    return 0;
}

data_source_base* true_color_manhattan_graph::get_series(size_t index)
//zwraca nullptr, jeśli indeks za duży
{
    if(index>4)
        return nullptr;
    if(index==0)
        return datas;
    else
        return colors[index-1];
}


void true_color_manhattan_graph::_replot() // Rysuje właściwy wykres a pod nim ewentualnie legendę
{
    int x1= get_start_x();
    int y1= get_start_y();
    int x2= x1 + get_width() - 1; //-1, bo width obejmuje pierwszy piksel
    int y2= y1 + get_height() - 1;
    double min,max,min_c[3],max_c[3];
    double miss,missing_c[3];
    size_t anty_width,A,B;
    size_t height=y2-y1;
    size_t width=x2-x1;
    int flaga=0;

    //Trzeba sprawdzić wymiary obszaru wizualizacji
    const geometry_base* MyGeometry=read_dim(A,B);

    //Legenda wtedy, jeśli jest potrzebna
    if(t_colors.start != get_background() || (t_colors.end != get_background() && (colors[0] || colors[1] || colors[2]) ))
        height-=2*char_height('0'); //będzie legenda

    //Strzałka, jeśli jest potrzebna
    if(t_colors.start != get_background())
        width-=3; //Na strzałko-ramkę.

    //Skala, jeśli jest potrzebna
    if(t_colors.end != get_background() && (colors[0] || colors[1] || colors[2]) )
        width-=5;

    //Danina wysokości i szerokości na perspektywę
    anty_width=dtou((double)(width) * h_offs);
    width=dtou((double)(width)*(1 - h_offs));
    height=dtou((double)(height)*(1 - v_offs));

    //Danina na podzielność przez A i B
    if(A<=1 || B<=1) //Nie ma danych
    {
        print_width(x1,(y1+y2)/2,x2-x1, t_colors.start, get_background(), "%@CInvalid data");
        return;
    }

    anty_width+= width % A; //Z szerokości cos wpada do antyszerokości
    width-=width%A; //W szerokości musi się mieścić `A` kolumn

    //Już wiadomo, jeśli się nie zmieści
    if(width==0 || anty_width / B * B == 0) //W antyszerokości musi być co najmniej po 1 piksel na wiersz.
    {
        print_width(x1,(y1+y2)/2,x2-x1, t_colors.start, get_background(), "%@CTo small area for %ux%u graph", A, B);
        return;
    }

    //Minimum i maksimum do skalowania słupków
    {
        size_t num_height;
        datas->bounds(num_height,min,max);
    }

    miss=datas->get_missing();
    if(mode==1)
        if(min>0) min=0; // Słupki co najmniej od zera
    s_data.set(min,max,(double)height+0.999);

    //Do skalowania kolorów, jeśli jest seria
    if(colors[0]!=nullptr)
    {
        size_t num_color;
        colors[0]->bounds(num_color, min_c[0], max_c[0]);
        s_colo[0].set(min_c[0], max_c[0], c_range.end - c_range.start + 0.999);
        missing_c[0]=colors[0]->get_missing();
    }
    if(colors[1]!=nullptr)
    {
        size_t num_color;
        colors[1]->bounds(num_color, min_c[1], max_c[1]);
        s_colo[1].set(min_c[1], max_c[1], c_range.end - c_range.start + 0.999);
        missing_c[1]=colors[1]->get_missing();
    }
    if(colors[2]!=nullptr)
    {
        size_t num_color;
        colors[2]->bounds(num_color, min_c[2], max_c[2]);
        s_colo[2].set(min_c[2], max_c[2], c_range.end - c_range.start + 0.999);
        missing_c[2]=colors[2]->get_missing();
    }

    //Wypisywanie legendy dla 1 serii
    if(t_colors.start != get_background())
    {
        print_width(x1, y1,((x2-x1)/5)*4, t_colors.start, get_background(), "%g", max);
        const char* pom=datas->name();
        print_width(x1,toi(y2-char_height('0')), ((x2-x1)/4), t_colors.start, get_background(),
                    "%g %*s    ", min,strlen(pom)>60?60:strlen(pom), pom);
        flaga=1;
    }

    //A teraz dla składowych koloru
    {int loc_width,x= (x2 - x1) / 4,y= toi(y2 - char_height('0'));
    if(colors[0]!=nullptr)
    {
        loc_width=print_width(x, y, (x2 - x) / 3 * 2, 50, get_background(),	//50 to czerwone z palety
            "%s", colors[0]->name());

        if(loc_width == 0) goto RYSOWANIE;
        else	 x+= loc_width + 1;

        loc_width=print_width(x, y, (x2 - x) / 2, 50,
                          c_range.start != get_background()?get_background():c_range.end,
                              "<%g", min_c[0]);

        if(loc_width == 0) goto RYSOWANIE;
        else	x+= loc_width + 1;

        loc_width=print_width(x, y, x2 - x, 255,	//Białe na czerwonym z palety
                                             50, ",%g>", max_c[0]);

        if(loc_width == 0) goto RYSOWANIE;
        else	x+= loc_width + 5;
    }
    if(colors[1]!=nullptr)
    {
        loc_width=print_width(x, y, (x2 - x) / 3 * 2, 220, get_background(),	//220 to zielone z palety
            "%s", colors[1]->name());

        if(loc_width == 0) goto RYSOWANIE;
        else	 x+= loc_width + 1;

        loc_width=print_width(x, y, (x2 - x) / 2, 220,
                          c_range.start != get_background()?get_background():c_range.end,
                              "<%g", min_c[1]);

        if(loc_width == 0) goto RYSOWANIE;
        else	x+= loc_width + 1;

        loc_width=print_width(x, y, x2 - x, 255,	//Białe na zielonym z palety
            220,
                              ",%g>", max_c[1]);

        if(loc_width == 0) goto RYSOWANIE;
        else	x+= loc_width + 5;
    }
    if(colors[2]!=nullptr)
    {
        loc_width=print_width(x, y, (x2 - x) / 3 * 2, 128, get_background(),	//128 to niebieski z palety
            "%s", colors[2]->name());

        if(loc_width == 0) goto RYSOWANIE;
        else	 x+= loc_width + 1;

        loc_width=print_width(x, y, (x2 - x) / 2, 128,
                          c_range.start != get_background()?get_background():c_range.end,
                              "<%g", min_c[2]);

        if(loc_width == 0) goto RYSOWANIE;
        else	x+= loc_width + 1;

        loc_width=print_width(x, y, x2 - x, 255,	//Białe na niebieskim z palety
            128,
                              ",%g>", max_c[2]);

        if(loc_width == 0) goto RYSOWANIE;
        else	x+= loc_width + 5;
    }
    }
RYSOWANIE:
    //Zawężenie po y-grekach
    if(flaga)
    {
        y1+=toi(char_height('0'));
        y2-=toi(char_height('0'));
    }

    flaga=0;
    x2-=3;

    //Ramka dla Y-kow
    if(x2-x1>10 && y2-y1>10)
    {
        int a_width= toi(anty_width / B * B );
        int fr_below= toi( (y2 - y1 - height) / B * B ); //Ile tylna os jest podsunięta do gory
        line(x1+3,toi( y2-height), x1 + 3 + a_width, toi(y2 - height - fr_below), t_colors.start);
        line(x1+3, y2, x1 + 3 + a_width, y2 - fr_below, t_colors.start);
        s_data.OY_axis(x1, toi(y2 - height), x1 + 6, y2, t_colors.start, get_background());
        s_data.OY_axis(x1 + a_width, toi(y2 - height - fr_below), x1 + 6 + a_width, y2 - fr_below, t_colors.start, get_background());
        x1+=3;
    }

    //Rysowanie właściwych słupków
    int zero=y2;
    if(min<0 && max>0)
    {
        zero=int(y2-s_data.get(0)); //Ma się mieścić — patrz warunek
        //fill_rect(x1,zero,x2+1,zero+1,t_colors.start);
    }

    int gristH=toi(width / toi(A)); //Grubości słupka w poziomie
    int gristV=toi((y2 - y1 - height) / B); //Grubości słupka w pionie
    int offsetH=toi((x2-x1-width)/B); //Przesuniecie kolejnych wierszy

    if(gristH >= 1 && gristV >= 1 && offsetH >= 1 )
    {
        settings_bar3d conf(gristH, offsetH,
                            gristV, wire != get_background()?wire:0, get_background());
        bar3d_config(&conf);

        data_source_base::iterator_h h=MyGeometry->make_view_iterator();

        for(int B_pos=0; B_pos < B; B_pos++) //W którym wierszu
            for(int A_pos=0; A_pos < A; A_pos++) //W której kolumnie
            {
                double test;
                int X,Y,Y2;		  //Robocze pozycje
                double r[4]={-1,-1,-1,-1};
                long   a[4]={0,0,0,0};
                size_t G_ind=MyGeometry->get_next(h); //czyta index
                r[0]=test=datas->get(G_ind); //i czyta wartość

                if(test==miss) //Jeśli missing
                    continue; //Nie rysuj

                if(colors[0]!=nullptr)
                {
                    r[1]=test=colors[0]->get(G_ind);
                    if(test == missing_c[1])
                        goto NIE_DA_SIE; //Awaria — "missing-i" muszą się pokrywać
                }
                if(colors[1]!=nullptr)
                {
                    r[2]=test=colors[1]->get(G_ind);
                    if(test == missing_c[1])
                        goto NIE_DA_SIE; //Awaria — "missing-i" muszą się pokrywać
                }
                if(colors[2]!=nullptr)
                {
                    r[3]=test=colors[2]->get(G_ind);
                    if(test == missing_c[2])
                        goto NIE_DA_SIE; //Awaria — "missing-i" muszą się pokrywać
                }

                _rescale_data_point(r,a);

                X=toi(x2 - (B_pos + 1) * offsetH - (A - A_pos) * gristH);
                Y=toi(zero- (B-(1 + B_pos)) * gristV);
                Y2=toi(y2 - (B-(1 + B_pos)) * gristV - a[0]); //Wysokość od min

                if(Y2>Y)
                    ::swap(Y,Y2);

                //Słupek 3D RGB
                bar3dRGB(X,Y,Y-Y2,toi(a[1]),toi(a[2]),toi(a[3]),2);
            }

        MyGeometry->destroy_iterator(h);
    }
    else
    {
        printc(x1,(y1+y2)/2, t_colors.start, get_background(), "Too small area");
    }

    return;
NIE_DA_SIE:
    print_width(x1,(y1+y2)/2,x2-x1, t_colors.start, get_background(), "%@CInvalid colors found. Probably");
    print_width(x1,toi((y1+y2)/2+char_height('X')),x2-x1, t_colors.start, get_background(), "%@C min or max is not set properly");
}


int true_color_manhattan_graph::_rescale_data_point(const double reals[],long in_area[])
// `reals` zawiera  wysokość słupka i wartość ustalająca kolor albo -1.
// Zwraca -1, jeśli nie w oknie
{
    int flaga=0;

    if(reals[0]<=s_data.max && reals[0]>=s_data.min)
    {
        unsigned height=(unsigned)s_data.get(reals[0]);
        in_area[0]=height;
    }
    else flaga=1;

    if(colors[0])
    {
        if(reals[1]<=s_colo[0].max && reals[1]>=s_colo[0].min)
        {
            unsigned color=(unsigned)s_colo[0].get(reals[1]);
            in_area[1]=color+c_range.start;
        }
        else flaga=1;
    }
        if(colors[1])
        {
            if(reals[2]<=s_colo[1].max && reals[2]>=s_colo[1].min)
            {
                unsigned color=(unsigned)s_colo[1].get(reals[2]);
                in_area[2]=color+c_range.start;
            }
            else flaga=1;
        }
            if(colors[2])
            {
                if(reals[3]<=s_colo[2].max && reals[3]>=s_colo[2].min)
                {
                    unsigned color=(unsigned)s_colo[2].get(reals[3]);
                    in_area[3]=color+c_range.start;
                }
                else flaga=1;
            }

                if(flaga)
                    return -1;
                else
                    return 0;
}

#pragma clang diagnostic pop
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



