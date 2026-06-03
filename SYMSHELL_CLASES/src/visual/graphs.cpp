/// @file
/// @brief **IMPLEMENTATION OF BASIC GRAPH CLASSES** /<br>
///         _IMPLEMENTACJA PODSTAWOWYCH KLAS GRAFÓW._
/// @date 2026-06-03 (modified)
//==========================================================================================

//#include <cstdarg>
//#include <cerrno>
//#include <cstdlib>
#include <cctype>
#include <cstdio> //sprintf!!!
#include <cassert>

#include <iostream>
#include <limits>

#include "wb_ptr.hpp"
#include "symshell.h"
#include "sshutils.hpp"
#include "graphs.hpp"
#include "toitoutoll.hpp"

namespace sym2{ namespace visual
{

using namespace sym2;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"


//`void rect(int x1,int y1,int x2,int y2,wb_color frame_c)`

/// Abbreviation of a too long string (with an asterisk at the end)/<br>
/// Skrót zbyt długiego napisu (z gwiazdką na końcu).
static char* trunc(char* bufor,unsigned width)
{
    while( size_t(string_width(bufor))>width ) //Symshell zle zwraca ??? tzn.?
    {
        size_t size=strlen(bufor);  //TODO Mocno nieoptymalna metoda.
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

void gps_area::load(const gps_area& p)
//load settings from p
{
    x1=p.x1;
    y1=p.y1;
    x2=p.x2;
    y2=p.y2;
}

void gps_area::add(const gps_area& p)
//load settings from p
{
    if(x1>p.x1)x1=p.x1;
    if(y1>p.y1)y1=p.y1;
    if(x2<p.x2)x2=p.x2;
    if(y2<p.y2)y2=p.y2;
}

void gps_area::swap(gps_area& p)
//load setting from p, but old settings to p
{
    ::swap(x1,p.x1);
    ::swap(y1,p.y1);
    ::swap(x2,p.x2);
    ::swap(y2,p.y2);
}

int  gps_area::is_inside(xy_info x, xy_info  y) const
//return 1 if point is inside area, else return 0
{
return x1<=x && x<=x2 && y1<=y && y<=y2;
}

int  gps_area::translate(int& x,int& y) const
//return 1 if point is inside area, and change x,y relatively to area
{
    if(x1<=x && x<=x2 && y1<=y && y<=y2)
    {
        x-=int(x1);y-=int(y1);
        return 1;
    }
    return 0;
}

void gps_area::get_transform_to(const gps_area& t,float tab[6]) const
{   //xy_info pom jest teraz typu `double`, więc rzuty na `double` są zbędne.
    //#warning "Many narrowing conversions from double to float, but don`t worry."
    tab[0]=float((t.x2+t.x1)/2-(x2+x1)/2); //px
    tab[1]=float((t.y2+t.y1)/2-(y2+y1)/2); //py
    tab[2]=float((t.x2-t.x1)/(x2-x1)); //kx
    tab[3]=float((t.y2-t.y1)/(y2-y1)); //ky
    tab[4]=float((t.x2+t.x1)/2); //x'0
    tab[5]=float((t.y2+t.y1)/2); //y'0
}

void gps_area::transform(const float tab[6])
{
    x1=tab[4]*(1-tab[2])+tab[2]*(x1+tab[0]);
    y1=tab[5]*(1-tab[3])+tab[3]*(y1+tab[1]);
    x2=tab[4]*(1-tab[2])+tab[2]*(x2+tab[0]);
    y2=tab[5]*(1-tab[3])+tab[3]*(y2+tab[1]);
}

int  gps_area::is_overlapped(const gps_area& t) const
{
    if(is_inside(t.x1,t.y1) ||
                 t.is_inside(x1,y1) ||
       is_inside(t.x2,t.y2) ||
                t.is_inside(x2,y2) ||
       is_inside(t.x1,t.y2) ||
                t.is_inside(x1,y2) ||
       is_inside(t.x2,t.y1) ||
                t.is_inside(x2,y1)
                ) return 1;
    return 0;
}

int drawable_base::get_start_x()
//Początek — x obszaru użytkowego
{
    int ret=int(x1);
    if(get_frame() != default_transparent && frame_width > 0)
        ret+=frame_width;
    return ret;
}

int drawable_base::get_start_y()
//Początek — y obszaru użytkowego
{
    int ret=int(y1);
    if(get_frame() != default_transparent && frame_width > 0)
        ret+=frame_width;
    if(title && (tit_col != default_transparent || tit_bck != default_transparent))
    {
        ssh_natural stringH=string_height(title);                 assert(stringH!=-1);
        ret+=toi(stringH);
    }
    return ret;
}

int drawable_base::get_width()
//Szerokość obszaru użytkowego
{
    int ret=int(x2-x1);
    ret++;
    if(get_frame() != default_transparent && frame_width > 0)
        ret-=2*frame_width;
    return ret;
}

int drawable_base::get_height()
//Wysokość obszaru użytkowego
{
    int ret=int(y2-y1);
    ret++;
    if(get_frame() != default_transparent && frame_width > 0)
        ret-=2*frame_width;

    if(title && (tit_col != default_transparent || tit_bck != default_transparent))
        {
        int r=toi(string_height(title));
        assert(r>-1);
        ret-=r;
        }
    return ret;
}

void drawable_base::flush()
{
    flush_plot();
}

void drawable_base::clear(int need_flush)
//Czyści obszar
{
    if(x1==x2 && y1==y2) return; //Uśpiony obszar o zerowym rozmiarze

    fill_rect(int(x1),int(y1),int(x2+1),int(y2+1),::background());
    if(need_flush)
            flush_plot();
}

//Rysuje ramkę, może tytuł i wirtualnie zawartość
void drawable_base::replot(int need_flush)
{
    if(x1==x2 && y1==y2) return; //Uśpiony obszar o zerowym rozmiarze.

    int tx=int(x1);
    int ty=int(y1);
    int tw=int(x2-x1);
    int high_enough= (y2 - y1) >= (char_height('X') + (2 * frame_width)); //Czy jest miejsce co najmniej na tytuł i/albo resztę

    //Czyszczenie tłem lufcika, jeśli jest ustawione
    if(get_background() != default_transparent)
        fill_rect(dtoi(x1), dtoi(y1), dtoi(x2+1), dtoi(y2+1), get_background());

    if(high_enough && title && (tit_col != default_transparent || tit_bck != default_transparent))
    { // TEN BLOK TYLKO WTEDY GDY ISTNIEJE WIDOCZNY TYTUŁ OBSZARU.
        int sw=0; //Current string width
        unsigned col1=(tit_col != default_transparent ? tit_col:get_frame());
        unsigned col2=(tit_bck != default_transparent ? tit_bck:get_background());
        if(col1==default_transparent)
            col1=default_black;

        char*  pom=clone_str(title);

        size_t len=strlen(pom);
        if(len==0)	goto REZYGNACJA;

        //skracanie
        while((sw=toi(string_width(pom)))>tw)
            {
            pom[len-1]='\0';
            len--;
            if(len==1) goto REZYGNACJA;
            }

        if(col1==col2) //Jeśli takie same To tlo staje się takie jak całości obszaru
            if((col2= get_background()) == col1) //a jeśli już niestety było
                if((col1=default_black)==col2) //to text czarny, a jak już był
                    col1=default_white; //to biały

        printc(tx+(tw/2-sw/2),ty,col1,col2,"%s",pom);

        // czy jest jeszcze miejsce na cokolwiek poza tytułem
        high_enough= (y2 - y1) > char_height('X') * 2 + frame_width; //Na frame asekurancko.

   REZYGNACJA:
        delete pom;
    }

    if((x2-x1>(2*frame_width)) && high_enough) //Nie robi _replot dla zbyt wąskich obszarów. Mają tylko tytuł.
    {
        //assert(this->CurrConfig!=nullptr);
        _replot();
        if(strlen(this->name())==0) //Zupełnie legalnie podobszary, np. typu "arrow_button" mogą nie mieć nazw, ale lepiej, żeby miały.
           cerr<<"Noname area detected... Set breakpoint at line "<<__LINE__<<" in "<<__FILE__<<endl; //PLACE FOR A BREAKPOINT?
    }
    else
    {
        // O WIELE ZA MAŁY OBSZAR, ŻEBY BYŁO COŚ WIĘCEJ NIŻ TYTUŁ. ALE TO PRZECIEŻ NIC SZCZEGÓLNEGO...
        //cerr<<'\"'<<this->name()<<"\" is a tiny area! "<<x2-x1<<"x"<<y2-y1<<endl; //PLACE FOR A BREAKPOINT?
    }

    if(get_frame() != default_transparent && frame_width > 0)
    {
        tx+=frame_width;
        ty+=frame_width;
        tw-=2*frame_width;
        rect(dtoi(x1), dtoi(y1), dtoi(x2), dtoi(y2), get_frame());
    }

    if(need_flush)
        flush_plot();
}

void drawable_base::_replot()
{
    assert("drawable_base::_replot(); should be reimplemented!"==0);
}

//graph implementation
//----------------------------

int graph::set_text_colors(wb_color start_i, wb_color end_i)
//Ustala kolor lub zbiór kolorów dla tekstów.
{
    t_colors.start=start_i;
    t_colors.end=end_i;
    return 0;
}


int graph::set_data_colors(wb_color start_i, wb_color end_i)
//Ustala kolor lub zakres dla danych
{
    c_range.start=start_i;
    c_range.end=end_i;
    if(end_i<=start_i) //Błąd
        {
        c_range.end=c_range.start+1;
        return -1;
        }
    return 0;
}


int graph::color_info::plot(int x1,int y1,int x2,int y2) const
{                                          assert(end>=start);
    unsigned ile_skali=end-start+2;
    int skok=1;
    int height=y2-y1;
    while(ile_skali>height)
        {
        skok*=2;
        if((ile_skali/=2)==0)
            return -1; //Nie da się
        }

    int weight= toi( height / ile_skali );
    int kolejny=0;

    //Pętla po kolorach palety. Pola c_range są `unsigned`!?
    for(size_t i=start;i<end;i+=skok)
        {
        fill_rect(x1,y1+ kolejny * weight, x2 + 1, y1 + (kolejny + 1) * weight, i);
        kolejny++;
        }

    fill_rect(x1,y1+ kolejny * weight, x2 + 1, y1 + (kolejny + 1) * weight, end);
    return 0;
}

void graph::scaling_info::OY_axis(int x1, int y1, int x2, int y2, wb_color col, wb_color bcg) const
{
    assert(x1<x2 && y1<y2);
    int width=x2-x1;
    fill_rect(x1+width/2,y1,x1+width/2+1,y2+1,col);
    line(x1+width/2,y1,x1,y1+width/2,col);
    line(x1+width/2,y1,x1+width-1,y1+width/2,col);

    int poz=0;
    if(min<0 && max>0)
            {
            poz=dtoi(y2-get(0)); //Obcina do współrzędnych ekranowych
            fill_rect(x1+width/2,poz,x1+width,poz+1,col);
            printc(x1+width,toi(poz-char_height('o')/2),col,bcg,"o");
            }

    if(min<1 && max>1)
    {
        poz=dtoi(y2-get(1)); //Obcina do współrzędnych ekranowych
        fill_rect(x1+width/2,poz,x1+width,poz+1,col);
        //printc(x1+6,poz-char_height('1')/2,col,get_background(),"1");
    }

    if(max-min<=(y2-y1)/2.) //między minimum a maksimum jest miejsce w pikselach
    {
        for(int d = int(min + 1); d < max; d++)
        {
            poz = dtoi(y2 - get(d)); //Wiec tu double zmieści się w int
            if(poz < y2 - int(y2 - y1) - 6) break; //Żeby nie naszło na grot strzałki
            fill_rect(x1 + 1, poz, x1 + width / 2, poz + 1, col);
        }
    }
}

void graph::scaling_info::OX_axis(int x1, int y1, int x2, int y2, wb_color col, wb_color bcg) const
{
    assert(x1<x2 && y1<y2);
    int width=y2-y1;
    fill_rect(x1,y2-3,x2,y2-3+1,col);
    line(x2,y2-width/2,x2-6,y2,col);
    line(x2,y2-width/2,x2-6,y2-width,col);

    int poz=0;
    if(min<0 && max>0)
    {
        poz=dtoi(x1+get(0)); //Ma się mieścić — patrz warunek.
        fill_rect(poz,y2-width,poz+1,y2+1,col);
        printc(poz,toi(y2-6-char_height('o')/2),col,bcg,"o");
    }

    if(min<1 && max>1)
    {
        poz=dtoi(x1+get(1)); //Ma się mieścić — patrz warunek.
        fill_rect(poz,y2-width,poz+1,y2+1,col);
    }

    if(max-min<=(x2-x1)/2.-6)
    {
        for(int d = int(min + 1); d < max; d++)
        {
            poz = dtoi(x1 + get(d)); //Ma się mieścić — patrz warunek.
            if(poz > x1 + int(x2 - x1) - 6) break;
            fill_rect(poz, y2 - width / 2, poz + 1, y2 + 1, col);
        }
    }
}



//carpet_graph
//-------------------------------------------

//DESTRUCTOR
carpet_graph::~carpet_graph()
{
    if(menage && data) delete data;
    data=nullptr;
    menage=false;
}

//CONSTRUCTOR(S)
carpet_graph::carpet_graph(int ix1,int iy1,int ix2,int iy2, //Położenie obszaru
             unsigned iA,unsigned iB,			 //A-ile kolumn, B-ile wierszy
             data_source_base* idata,int i_menage, //data-źródło danych o kolorach
             bool i_direct_color
             )
: direct_color(i_direct_color), print_title(false),
  graph(ix1,iy1,ix2,iy2), AA(iA), BB(iB), data(idata), menage(i_menage)
{
    assert(data!=nullptr);
    assert(AA>=2 && BB>=2);
}

carpet_graph::carpet_graph(int ix1,int iy1,int ix2,int iy2,  //Położenie obszaru
             data_source_base* idata,int i_menage,           //data-źródło danych o kolorach
             bool i_direct_color)
: print_title(0),direct_color(i_direct_color), graph(ix1, iy1, ix2, iy2), AA(1), BB(1), data(idata), menage(i_menage)
{
    assert(data!=nullptr);
    read_dim(AA,BB);
}

// IMPLEMENTATION OF VIRTUAL METHODS oF carpet_graph:
//---------------------------------------------------

const geometry_base* carpet_graph::read_dim(size_t& aa,size_t& bb)
{
    geometry_base* MyGeometry= data->get_geometry();
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
            //	A=subtab[1];
            //	B=subtab[3];
            geometry::view_info pom;
            MyGeometry->get_view_info(&pom);
            aa=size_t(pom.dia.X()*2);
            bb=size_t(pom.dia.Y()*2);
            return MyGeometry;
        }
}


int carpet_graph::set_series(size_t index, data_source_base* i_data, int i_menage)
//zwraca -1, jeśli indeks za duży
{
    if(index>0) return -1; //Tylko jedna seria
    assert(i_data != nullptr);
    if(menage) delete data;
    data=i_data;
    menage=i_menage;
    return 0;
}

data_source_base* carpet_graph::get_series(size_t index)
//zwraca nullptr, jeśli indeks za duży
{
    if(index>0)
        return nullptr;
    return data;
}

void carpet_graph::_replot()
// Rysuje właściwy wykres, a pod nim ewentualnie legendę.
{
    unsigned CO_ILE_KOMOREK=1;
    int x1= get_start_x();
    int y1= get_start_y();
    int x2= x1 + get_width() - 1; //-1, bo width obejmuje pierwszy piksel
    int y2= y1 + get_height() - 1;

    assert(x1<=x2); //Czy aby na pewno
    assert(y1<=y2); //Sensowne okno. Może miec zerowy rozmiar, ale nie ujemny

    double min,max;
    double missing;

    size_t A=1,B=1; //Lokalne A i B
    const geometry_base* MyGeometry=read_dim(A,B);

    size_t NumberOfEl;
    data->bounds(NumberOfEl,min,max);

    if((A<=1 && B<=1)||(min>=max))
        {
        print_width(x1,(y1+y2)/2,x2-x1,  get_background(),t_colors.start, "%@CInvalid data");
        return;
        }

    //Wartość zwracana, gdy nie ma wartości.
    missing=data->get_missing();

    //Do skalowania kolorów
    if(!direct_color)
        mm.set(min,max,c_range.end-c_range.start+0.999);

JESZCZE_RAZ_PRZELICZ:
    //R y s o w a n i e  l e g e n d y
    if(t_colors.start != get_background() &&
       char_height('X') < get_height())
    {
        int x=x1;
        int y=toi(y2-char_height('X')+1); //+1, bo y2 ma być zarysowane
        int width=0;
    //----------
        y2=y; //Zabiera dolną część na legendę.
    //----------
        width=print_width(x, y,(x2-x)/3*2, t_colors.start, get_background(),
                          "%s", data->name());

        if(width==0) goto KWADRACIKI;
        else	 x+=width+1;

        width=print_width(x,y,(x2-x)/2,c_range.start,
                          c_range.start != get_background()?get_background():c_range.end,
                              "<%g",min);

        if(width==0) goto KWADRACIKI;
        else	x+=width+1;

        width=print_width(x,y,x2-x,c_range.end,
                          c_range.end != get_background()?get_background():c_range.start,
                            ",%g>",max);
    }

    //Rysowanie skali — jeśli są co najmniej dwa kolory i jest miejsce na co najmniej 2 piksele
    if(!direct_color)
     if(x2-x1 >= max_(A, B) + 6 && c_range.end > c_range.start && c_range.end - c_range.start >= 2)
        {
        c_range.plot(x2-6,y1,x2,y2);
        x2-=7;
        }

    //Rysowanie kwadracików
KWADRACIKI:                                                                                               assert(x2>x1);
                                                                                                          assert(y2>y1);
    while(!( A/CO_ILE_KOMOREK <= (x2-x1+1) && B/CO_ILE_KOMOREK <= (y2-y1+1)))
    {
       CO_ILE_KOMOREK++;
       print_width(x1, y1,x2-x1, t_colors.start, get_background(), "%@CView reduced %u times", CO_ILE_KOMOREK);
    }

    //RYSOWANIE
    {
                                                                                   assert(c_range.end-c_range.start>=1);
        size_t i,j; //Indeksy po wierszach i kolumnach
        int width=x2-x1+1; //Już moga być inne
        int height=y2-y1+1; //Niż dla całego obszaru

        int thicknessA,thicknessB;
        if(CO_ILE_KOMOREK==1)
        {
            thicknessA=toi(width / A);
            thicknessB=toi(height / B);
        }
        else thicknessA=thicknessB=1;

        //Musi być kwadratowo, bo inaczej jest nieładnie
        if(thicknessA > 1 && thicknessB > 1)
            {
            if(thicknessA > thicknessB)
                thicknessA=thicknessB;
            else
                thicknessB=thicknessA;
            }
        else
            {
                thicknessA= thicknessB=1; //Pikselami będzie !!!
            }

        int offsetA= toi(width - thicknessA * (A / CO_ILE_KOMOREK)) / 2;
        int offsetB= toi(height - thicknessB * (B / CO_ILE_KOMOREK)) / 2;

        //Rysowanie
        const rectangle_geometry* MyGeomRect=                                                           //Musi taka być!
                             dynamic_cast<const rectangle_geometry*>(MyGeometry);           assert(MyGeomRect!=nullptr);
    //    long index=MyGeomRect->get(0,0); //Zerowa komórka

        data_source_base::iterator_h h=MyGeometry->make_view_iterator();                           assert(h != nullptr);
        wb_color back= get_background(); //Dla sprawdzania, kiedy kolor kwadratu taki jak kolor tła.
        if(thicknessA == 1) //starczy jedna sprawdzić, bo kwadrat.
            {
            //Pikselami panowie!!!
            for(j=0;j<B;j+=CO_ILE_KOMOREK)
              for(i=0;i<A;i+=CO_ILE_KOMOREK)
                {
                    size_t G_ind = 0;
                    if(CO_ILE_KOMOREK == 1)
                        G_ind = MyGeometry->get_next(h);
                    else if(i < A && j < B)
                        G_ind = MyGeomRect->get(tol(i), tol(j) ); //Nie zadziała dla zmniejszonego view
                    else
                        continue; //Pomijamy go.

                    double test = data->get(G_ind);

                    if (test == missing)
                                continue; // Nie rysować, jeśli wartość nieosiągalna

                    if (!direct_color)
                    {
                                wb_color color = wb_color(mm.get(test));
                                color += c_range.start;
                                if (color <= c_range.end)
                                    plot( toi(offsetA + x1 + i/CO_ILE_KOMOREK), toi(offsetB + y1 + j/CO_ILE_KOMOREK), color);
                                else
                                    goto NIE_DA_SIE;
                    }
                    else
                    {           //assert("NOT TESTED CODE?"==0); // TESTED TESTED :-D
                                unsigned C=dtou(test); //Zakładamy, że to surowe kolory? double na uint32?
                                unsigned Red=(C & 0x0000ff);
                                unsigned Gre=(C & 0x00ff00) >> 8;
                                unsigned Blu=(C & 0xff0000) >> 16;
                                // color+=c_range.start;
                                plot_rgb(toi(offsetA +x1 +i/CO_ILE_KOMOREK),toi(offsetB +y1 +j/CO_ILE_KOMOREK), Red, Gre, Blu);
                    }
                }
            }
        else
            {
            //Kwadratami
            for(j=0;j<B;j++)
              for(i=0;i<A;i++)
              {																assert(h!=nullptr);
                  size_t G_ind=MyGeometry->get_next(h); //rectangle_geometry
                  double test=data->get(G_ind);
                  if(test==missing)
                      continue; //Nie rysować, jeśli wartość nieosiągalna
                  if (!direct_color)
                  {
                    wb_color color=wb_color( mm.get(test) );
                    color+=c_range.start;

                    if(color > c_range.end) //reverted/simplified expression.
                        goto NIE_DA_SIE;

                    if(color==back && thicknessA > 3)
                    {
                      rect(toi(offsetA+x1+ i * thicknessA), toi(offsetB + y1 + j * thicknessB),
                           toi(offsetA + x1 + (i+1) * thicknessA - 1), toi(offsetB + y1 + (j + 1) * thicknessB - 1),
                           255!=back?255:0);
                    }
                    else
                    {
                      fill_rect(toi(offsetA+x1+ i * thicknessA), toi(offsetB + y1 + j * thicknessB),
                                toi(offsetA+x1+ (i+1) * thicknessA), toi(offsetB + y1 + (j + 1) * thicknessB),
                                color);
                    }
                  }
                  else //Jeżeli direct_color
                  {
                        //assert("NOT TESTED CODE?"==0); A jednak jest to używane w "Languages"!
                        unsigned C=dtou(test); //Zakładamy, że to surowe kolory? double na uint32?
                        unsigned Red=(C & 0x0000ff);
                        unsigned Gre=(C & 0x00ff00) >> 8;
                        unsigned Blu=(C & 0xff0000) >> 16;
                        set_pen_rgb(Red, Gre, Blu, 1, SSH_SOLID_PUT);
                        set_brush_rgb(Red, Gre, Blu);
                        fill_rect_d(toi(offsetA+x1+ i * thicknessA), toi(offsetB + y1 + j * thicknessB),
                                    toi(offsetA+x1+ (i+1) * thicknessA), toi(offsetB + y1 + (j + 1) * thicknessB));
                  }
              }
            }
        MyGeometry->destroy_iterator(h);
    }

    return;
NIE_DA_SIE:
    print_width(x1,(y1+y2)/2,x2-x1, t_colors.start, get_background(), "%@CInvalid colors found.");
    print_width(x1,toi((y1+y2)/2+char_height('X')),x2-x1, t_colors.start, get_background(), "%@CProbably min or max not properly set.");
}

// reals[] zawiera jedynie wartość ustalająca kolor albo coś spoza zakresu
int carpet_graph::_rescale_data_point(const double reals[],long in_area[])
//zwraca -1, jeśli nie w oknie
{
    if(reals[0]<=mm.max && reals[0]>=mm.min)
        {
        unsigned color=(unsigned)mm.get(reals[0]);
        in_area[0]=color+c_range.start;
        return 0;
        }
    return -1;
}

//extern _instantiate_by_dummy_(fast_carpet_graph< matrix_source<double> >&);

// bars_graph
//--------------------------------------------

//DESTRUCTOR
bars_graph::~bars_graph()
{
    if(datas && d_menage) delete datas;
    datas=nullptr;
    d_menage=false;
    if(colors && c_menage) delete colors;
    colors=nullptr;
    c_menage=false;
}

//CONSTRUCTOR(S)
bars_graph::bars_graph(  int ix1, int iy1, int ix2, int iy2, //Położenie obszaru
                         data_source_base* i_datas, int menage_d, //datas-dane o wysokościach
                         data_source_base* i_colors, int menage_c, //colors-źródło danych o kolorach
                         int zero_mod) //tryb wyświetlania
: graph(ix1,iy1,ix2,iy2),datas(i_datas), d_menage(menage_d),colors(i_colors), c_menage(menage_c),mode(zero_mod)
{
    bars_graph::set_data_colors(128, 129);
    assert(datas!=nullptr);
}

// IMPLEMENTATION OF VIRTUAL METHODS oF bars_graph
//-----------------------------------------------------
int bars_graph::configure(const void* config)
{
    int old=mode;
    mode=((const config_zero*)config)->zero_mode;
    assert(mode==0 || mode==1);
    if(mode==0 || mode==1)
    {
        return 0;
    } else
    {
        mode = old;
        return -1;
    }
}

int bars_graph::set_series(size_t index, data_source_base *i_data, int i_menage)
//zwraca -1, jeśli indeks za duży
{
    if(index>1) return -1; //Tylko dwie serie
    assert(i_data != nullptr);
    if(index==0)
        {
        if(d_menage) delete datas;
        datas=i_data;
        d_menage=i_menage;
        }
    else
        {
        if(c_menage) delete colors;
        colors=i_data;
        c_menage=i_menage;
        }
    return 0;
}

data_source_base* bars_graph::get_series(size_t index)
//zwraca nullptr, jeśli indeks za duży
{
    if(index>0)
        return nullptr;
    if(index==0)
        return datas;
    else
        return colors;
}

void bars_graph::_replot() // Rysuje właściwy wykres a pod nim ewentualnie legendę
{
    int x1 = get_start_x();
    int y1 = get_start_y();
    int x2 = x1 + get_width() - 1; //-1, bo width obejmuje pierwszy piksel (?).
    int y2 = y1 + get_height() - 1;
    double min, max, minc, max_c;
    double miss, miss_c;
    size_t num_height,num_color; ///< Faktycznie może być tak dużo danych, że potrzeba size_t?
    unsigned height;
    int flaga = 0;

    //Legenda wtedy, jeśli jest potrzebna
    if(t_colors.start != get_background() || (t_colors.end != get_background() && colors))
        height = (y2 - y1) - 2 * char_height('0'); //będzie legenda.
    else
        height = y2 - y1; //Nie będzie legendy

    //Do skalowania słupków
    datas->bounds(num_height, min, max);

    miss = datas->get_missing();
    if(mode == 1)
        if(min > 0) min = 0; // Słupki co najmniej od zera.
    s_data.set(min, max, height + 0.999);

    //Do skalowania kolorów
    if(colors != nullptr)
    {//Jeśli jest seria
        colors->bounds(num_color, minc, max_c);
        assert(num_height <= num_color);
        s_colo.set(minc, max_c, c_range.end - c_range.start + 0.999);
        miss_c = colors->get_missing();
    } else
    {   //Nie ma serii dla kolorów, więc kolory arbitralne
        s_colo.set(0, toi(num_height - 1), c_range.end - c_range.start + 0.9999);
    }

    //Wypisywanie legendy dla 1 serii
    if(t_colors.start != get_background())
    {
        const char *pom = datas->name();
        print_width(x1, y1, (x2 - x1) / 2, t_colors.start, get_background(),
                    "%g", max);
        print_width(x1, toi(y2 - char_height('0')), (x2 - x1) / 3 * 2, t_colors.start, get_background(),
                    "%g %*s", min, strlen(pom) > 60?60:strlen(pom), pom);
        flaga = 1;
    }

    // A teraz dla drugiej
    if(t_colors.end != get_background() && colors != nullptr)
    {
        int width = print_width(x2 - (x2 - x1) / 2, y1, (x2 - x1) / 2,
                                c_range.start, c_range.start != get_background()?get_background():c_range.end,
                                "%@R%g", minc);

        const char *pom = colors->name();
        print_width((x2 + x1) / 2, y1, (x2 - x1) / 2 - width, get_background(), c_range.start,
                    "%@R%*s", strlen(pom) > 60?60:strlen(pom), pom);

        print_width(x2 - (x2 - x1) / 3, toi(y2 - char_height('0')), (x2 - x1) / 3,
                    c_range.end, c_range.end != get_background()?get_background():c_range.start,
                    "%@R%g", max_c);

        flaga = 1;
    }

    //Zawężenie po y-ach
    if(flaga)
    {
        y1 += toi(char_height('0'));
        y2 -= toi(char_height('0'));
        if(y1>y2) //Zrobiło się za małe.
            return;
    }
    flaga = 0;

    //Strzałka w gore
    if(x2 - x1 > 10 && y2 - y1 > 10)
    {
        s_data.OY_axis(x1, y1, x1 + 6, y2, t_colors.start, get_background());
        x1 += 6;
    }

    //Rysowanie skali — jeśli jest seria kolorów — tzn. kolory są wymuszone
    //Są co najmniej dwa kolory skali i jest miejsce na co najmniej 5 pikseli
    if(colors != nullptr && x2 - x1 >= 10 && c_range.end > c_range.start && c_range.end - c_range.start >= 1)
    {
        c_range.plot(x2 - 5, y1, x2, y2);
        x2 -= 5;
    }
    //NIE_DA_SIE:;

    //Linia zerowa
    int zero = y2;
    if(min < 0 && max > 0)
    {
        zero = int(y2 - s_data.get(0)); //Musi być miedzy y1 a y2
        fill_rect(x1, zero, x2 + 1, zero + 1, t_colors.start);
    }

    //Rysowanie właściwych słupków
    int thickness; //Grubość słupka
    if(num_height > 0 && (thickness = toi( (x2 - x1) / (num_height)) ) >= 1)
    {
        data_source_base::iterator_h h = datas->reset();
        data_source_base::iterator_h c = colors?colors->reset():nullptr;
        for(unsigned i = 0; i < num_height; i++) //64-bitowe `size_t` to już za dużo tutaj! :-D
        {
            double test;
            double r[2] = {-1, -1};
            long a[2] = {0, (c_range.end - c_range.start) / 2};
            r[0] = test = datas->get(h);
            if(test == miss)
                continue; //nie rysuj

            if(colors)
            {
                r[1] = test = colors->get(c);
                if(test == miss_c)
                    continue; //nie rysuj
            } else
                r[1] = i; //Arbitralny przydział kolejnych kolorów

            _rescale_data_point(r, a);

            int y_up = toi(y2 - a[0]);
            int y_dow = zero;
            if(y_up > y_dow)
            {
                ::swap(y_up, y_dow);
                //y_up++; //Male oszustwo, żeby też się zaczynało od zerowej linii.
            }

            assert(y_up <= y_dow);
            if(wb_color(a[1]) != get_background())
                fill_rect(toi(x1 + i * thickness), y_up,
                          toi(x1 + (i + 1) * thickness), y_dow + 1,
                          a[1]); //Puki kolory indeksowe to pół biedy.
            else
                rect(toi(x1 + i * thickness), y_up,
                     toi(x1 + (i + 1) * thickness - 1), y_dow,
                     255 != get_background()?255:0);
        }
        datas->close(h);
        if(colors) colors->close(c);
    } else
    {
        print_width(x1, (y1 + y2) / 2, x2 - x1, t_colors.start, get_background(), "%@CInvalid data or too small area");
    }
}


int bars_graph::_rescale_data_point(const double reals[], long in_area[])
//zwraca -1, jeśli nie w oknie; reals[] zawiera  wysokość słupka i wartość ustalająca kolor albo -1.
{
    int flaga = 0;

    if(reals[0] <= s_data.max && reals[0] >= s_data.min)
    {
        unsigned height = (unsigned) s_data.get(reals[0]);
        in_area[0] = height;
    } else flaga = 1;

    if(reals[1] <= s_colo.max && reals[1] >= s_colo.min)
    {
        unsigned color = (unsigned) s_colo.get(reals[1]);
        in_area[1] = color + c_range.start;
    }

    if(flaga)
        return -1;
    else
        return 0;
}


// manhattan_graph
//--------------------------------------------

//DESTRUCTOR
manhattan_graph::~manhattan_graph()
{
    if(datas && d_menage) delete datas;
    datas = nullptr;
    d_menage = 0;
    if(colors && c_menage) delete colors;
    colors = nullptr;
    c_menage = 0;
}

//CONSTRUCTOR(S)
manhattan_graph::manhattan_graph(int ix1, int iy1, int ix2, int iy2, //Położenie obszaru
                                 unsigned iA, unsigned iB,
                                 data_source_base *i_datas, int menage_d, //datas-dane o wysokościach
                                 data_source_base *i_colors, int menage_c, //colors-źródło danych o kolorach
                                 int zero_mod,        // tryb wyświetlania
                                 double H_offs,       // Ułamek szerokości przeznaczony na perspektywę
                                 double V_offs        // Ułamek wysokości  przeznaczony na perspektywę
                                )
    :
    graph(ix1, iy1, ix2, iy2), AA(iA), BB(iB),
    datas(i_datas), d_menage(menage_d),
    colors(i_colors), c_menage(menage_c),
    mode(zero_mod), c_offset(0), wire(get_background()),
    h_offs(H_offs), v_offs(V_offs)
{
    assert(datas != nullptr);
    assert(AA != 0);
    assert(BB != 0);
    assert(v_offs > 0 && v_offs < 1);
    assert(h_offs > 0 && h_offs < 1);
}

manhattan_graph::manhattan_graph(int ix1, int iy1, int ix2, int iy2, //Położenie obszaru
                                 data_source_base *i_datas, int menage_d, //datas-dane o wysokościach
                                 data_source_base *i_colors, int menage_c, //colors-źródło danych o kolorach
                                 int zero_mod,         // tryb wyświetlania
                                 double H_offs,        // Ułamek szerokości przeznaczony na perspektywę
                                 double V_offs         // Ułamek wysokości  przeznaczony na perspektywę
                                )
    :
    graph(ix1, iy1, ix2, iy2), AA(1), BB(1),
    datas(i_datas), d_menage(menage_d),
    colors(i_colors), c_menage(menage_c),
    mode(zero_mod), c_offset(0), wire(get_background()),
    h_offs(H_offs), v_offs(V_offs)
{
    assert(datas != nullptr);
    assert(AA != 0);
    assert(BB != 0);
    assert(v_offs > 0 && v_offs < 1);
    assert(h_offs > 0 && h_offs < 1);
}

// IMPLEMENTATION OF VIRTUAL METHODS of manhattan_graph
//-----------------------------------------------------
const geometry_base *manhattan_graph::read_dim(size_t &aa, size_t &bb)
{
    geometry_base *MyGeometry = datas->get_geometry();
    if(MyGeometry == nullptr ||
       MyGeometry->get_dimension() < 2)
    {
        //	A=subtab[1];
        //	B=subtab[3];
        if(!deputy)
            deputy = new rectangle_geometry(AA, BB, 0); //Nie torus
        aa = AA;
        bb = BB;
        return deputy.get_ptr_val();
    } else
    {
        //Sprawdzenie, czy serie się nie zgadzają — czy mają ta sama geometrie
        if(colors != nullptr)
        {
            geometry_base *ColGeom = colors->get_geometry();
            if(*MyGeometry != *ColGeom)
            {//jeśli nie to pozbywamy się koloru
                if(c_menage) delete colors;
                colors = nullptr;
            }
        }
        geometry::view_info pom;
        MyGeometry->get_view_info(&pom);
        aa = size_t(pom.dia.X() * 2);
        bb = size_t(pom.dia.Y() * 2);
        return MyGeometry;
    }

}


int manhattan_graph::configure(const void *p)
{
    const config *conf = ((const config *) p);
    if(p == nullptr)
    {
        mode = 1;
        c_offset = 0;
        wire = get_background();
        return -1;
    } else
    {
        mode = conf->zero_mode;
        wire = conf->wire;
        c_offset = conf->color_offset;
        assert(mode == 0 || mode == 1);
        return 0;
    }
}


int manhattan_graph::set_series(size_t index, data_source_base *idata, int imenage)
//zwraca -1, jeśli indeks za duży
{
    if(index > 1) return -1; //Tylko dwie serie
    assert(idata != nullptr);
    if(index == 0)
    {
        if(d_menage) delete datas;
        datas = idata;
        d_menage = imenage;
    } else
    {
        if(c_menage) delete colors;
        colors = idata;
        c_menage = imenage;
    }
    return 0;
}

data_source_base *manhattan_graph::get_series(size_t index)
//zwraca nullptr, jeśli indeks za duży
{
    if(index > 0)
        return nullptr;
    if(index == 0)
        return datas;
    else
        return colors;
}


void manhattan_graph::_replot() // Rysuje właściwy wykres a pod nim ewentualnie legendę
{
    int zero=0;
    int x1 = get_start_x();
    int y1 = get_start_y();
    int x2 = x1 + get_width() - 1; //-1, bo width obejmuje pierwszy piksel
    int y2 = y1 + get_height() - 1;
    double min, max, min_c, max_c;
    double miss, miss_c;
    size_t anty_width, A, B;
    unsigned height = y2 - y1;
    unsigned width = x2 - x1;
    int flaga = 0;

//Trzeba sprawdzić wymiary obszaru wizualizacji
    const geometry_base *MyGeometry = read_dim(A, B);

//Legenda wtedy, jeśli jest potrzebna
    if(t_colors.start != get_background() || (t_colors.end != get_background() && colors))
        height -= 2 * char_height('0'); //będzie legenda

//Strzałka, jeśli jest potrzebna
    if(t_colors.start != get_background())
        width -= 3; //Na strzałko-ramkę

//Skala, jeśli jest potrzebna
    if(t_colors.end != get_background() && colors != nullptr)
        width -= 5;

   //Danina wysokości i szerokości na perspektywę
    anty_width = size_t(width * h_offs);
    width = size_t(width * (1 - h_offs));
    height = size_t(height * (1 - v_offs));

    //Danina na podzielność przez A i B
    if(A <= 1 || B <= 1) //Nie ma danych
    {
        print_width(x1, (y1 + y2) / 2, x2 - x1, t_colors.start, get_background(), "%@CInvalid data");
        return;
    }

    anty_width += width % A; //Z szerokości coś wpada do antyszerokości
    width -= width % A; //W szerokości musi się mieścić A kolumn

   //już wiadomo,jeśli się nie zmieści
    if(width == 0 || anty_width / B * B == 0) //W antyszerokości musi bys co najmniej po 1 piksel na wiersz
    {
        print_width(x1,y1,x2 - x1, t_colors.start, get_background(), "%@CToo small area for %ux%u graph", A,
                    B);
        return;
    }

//minimum i maksimum do skalowania słupków
    {
        size_t num_height;
        datas->bounds(num_height, min, max);
    }

    miss = datas->get_missing();
    if(mode == 1)
        if(min > 0) min = 0; // Słupki co najmniej od zera
    s_data.set(min, max, height + 0.999);

//Do skalowania kolorów, jeśli jest seria
    if(colors != nullptr)
    {
        size_t num_color;
        colors->bounds(num_color,min_c, max_c);
        s_colo.set(min_c, max_c, c_range.end - c_range.start + 0.999);
        miss_c = colors->get_missing();
    }

//Wypisywanie legendy dla 1 serii
    if(t_colors.start != get_background())
    {
        print_width(x1, y1, ((x2 - x1) / 5) * 4, t_colors.start, get_background(), "%g", max);
        const char *pom = datas->name();
        print_width(x1, toi(y2 - char_height('0')), ((x2 - x1) / 5) * 4, t_colors.start, get_background(),
                    "%g %*s", min, strlen(pom) > 60?60:strlen(pom), pom);
        flaga = 1;
    }

// i dla drugiej
    if(t_colors.end != get_background() && (colors != nullptr))
    {
        int loc_width = print_width(x2 - (((x2 - x1) / 3) * 2), y1, ((x2 - x1) / 3) * 2,
                                    c_range.start, c_range.start != get_background()?get_background():c_range.end,
                                    "%@R%g", min_c);

        const char *pom = colors->name();
        print_width(x1 + (x2 - x1) / 5, y1, ((x2 - x1) / 5 * 4) - loc_width, get_background(), c_range.start,
                    "%@R%*s", strlen(pom) > 60?60:strlen(pom), pom);

        print_width(x2 - (x2 - x1) / 2, toi(y2 - char_height('0')), (x2 - x1) / 2,
                    c_range.end, c_range.end != get_background()?get_background():c_range.start,
                    "%@R%g", max_c);
        flaga = 1;
    }

    //Zawężenie po y-grekach
    if(flaga)
    {
        y1 += toi(char_height('0'));
        y2 -= toi(char_height('0'));
        if(y1>y2) return;
            //goto NOT_ENOUGH_HEIGH; //TODO W jeszcze innych grafach!!!
    }
    flaga = 0;

//SKIP:
//Rysowanie skali -jeśli są co najmniej dwa kolory i jest miejsce na co najmniej 2 piksele.
    if(colors != nullptr && x2 - x1 >= 10 && c_range.end > c_range.start && c_range.end - c_range.start >= 1)
    {
        c_range.plot(x2 - 5, y1, x2, y2);
        x2 -= 5;
    }
//NIE_DA_SIĘ: ;

//Ramka dla Y-kow
    if(x2 - x1 > 10 && y2 - y1 > 10)
    {
        int awidth = toi(anty_width / B * B);
        int oddolu = toi((y2 - y1 - height) / B * B); //Ile tylna oś jest podsunięta do gory
        line(x1 + 3, toi(y2 - height), x1 + 3 + awidth, toi(y2 - height - oddolu), t_colors.start);
        line(x1 + 3, y2, x1 + 3 + awidth, y2 - oddolu, t_colors.start);
        s_data.OY_axis(x1, toi(y2 - height), x1 + 6, y2, t_colors.start, get_background());
        s_data.OY_axis(x1 + awidth, toi(y2 - height - oddolu), x1 + 6 + awidth, y2 - oddolu, t_colors.start, get_background());
        x1 += 3;
    }

//Rysowanie właściwych słupków
    {
    int zero = y2;
    if(min < 0 && max > 0)
    {
        zero = dtoi(y2 - s_data.get(0)); //Ma się mieścić — patrz warunek
        //fill_rect(x1,zero,x2+1,zero+1,t_colors.start);
    }

    int gristH = toi(width / A ); //Grubość słupka w poziomie
    int gristV = toi((y2 - y1 - height) / B); //Grubość słupka w pionie
    int offsetH = toi((x2 - x1 - width) / B); //Przesuniecie kolejnych wierszy

    if(gristH >= 1 && gristV >= 1 && offsetH >= 1)
    {
        settings_bar3d conf(gristH, offsetH,
                            gristV, wire != get_background()?wire:0, get_background());
        bar3d_config(&conf);

        data_source_base::iterator_h h = MyGeometry->make_view_iterator();
        geometry_base *color_geom = nullptr;
        data_source_base::iterator_h c = nullptr;

        if(colors) //Jeśli jest kolor to
        {
            color_geom = colors->get_geometry(); //Wez z niego geometrie
            if(color_geom)                    //Jeśli jest geometria to stwórz iterator
                c = color_geom->make_view_iterator();
        }

        for(unsigned Bpos = 0; Bpos < B; Bpos++) //W którym wierszu
            for(unsigned Apos = 0; Apos < A; Apos++) //W której kolumnie
            {
                double test;
                int X, Y, Y2;          //Robocze pozycje
                double r[2] = {-1, -1};
                long a[2] = {0, (c_range.end - c_range.start) / 2};
                size_t hi = MyGeometry->get_next(h); //czyta index
                r[0] = test = datas->get(hi); //i czyta wartość
                if(test == miss) //Jeśli missing
                {//Wywal odpowiadająca wartość dla color
                    if(color_geom)
                        color_geom->get_next(c);
                    continue; //Nie rysuj
                }
                //s_data.min;
                if(color_geom)
                {
                    size_t ci = color_geom->get_next(c); //czyta index
                    r[1] = test = colors->get(ci); //i czyta wartość
                    if(test == miss_c)
                        continue; //Nie rysuj
                }

                _rescale_data_point(r, a);

                X = toi(x2 - (Bpos + 1) * offsetH - (A - Apos) * gristH);
                Y = toi(zero - (B - (1 + Bpos)) * gristV);
                Y2 = toi(y2 - (B - (1 + Bpos)) * gristV - a[0]); //Wysokość od min

                if(Y2 > Y)
                    ::swap(Y, Y2);

                //Słupek 3D
                bar3d(X, Y, Y - Y2, a[1], a[1] + c_offset);
            }
        MyGeometry->destroy_iterator(h);
        if(color_geom) color_geom->destroy_iterator(c);
    } else goto NOT_ENOUGH_HEIGH;
    }
    return;
NOT_ENOUGH_HEIGH:
        printc(x1, (y1 + y2) / 2, t_colors.start, get_background(), "Too small area");
}

// reals[] zawiera  wysokość słupka i wartość ustalająca kolor albo -1
int manhattan_graph::_rescale_data_point(const double reals[], long in_area[])
//zwraca -1, jeśli nie w oknie
{
    int flaga = 0;

    if(reals[0] <= s_data.max && reals[0] >= s_data.min)
    {
        unsigned height = (unsigned) s_data.get(reals[0]);
        in_area[0] = height;
    } else flaga = 1;

    if(colors)
        if(reals[1] <= s_colo.max && reals[1] >= s_colo.min)
        {
            unsigned color = (unsigned) s_colo.get(reals[1]);
            in_area[1] = color + c_range.start;
        }

    if(flaga)
        return -1;
    else
        return 0;
}



//sequence_graph
//--------------------------------------------

//DESTRUCTOR
sequence_graph::~sequence_graph()
{
    if(series != nullptr)
        for(size_t i = 0; i < M; i++)
            if(series[i].menage && series[i].ptr != nullptr)
                delete series[i].ptr;
}

//CONSTRUCTOR(S) for  series_info* & for data_source_base*
sequence_graph::sequence_graph(int ix1, int iy1, int ix2, int iy2, //Położenie obszaru
                               int N,            //Liczba serii
                               series_info *iseries,
                               int imode,    //0 - tryb z pełnym reskalowaniem
                               //1 - tryb ze wspólnym minimum, maximum, pobieranym z parametrów i serii
                               //2 - tryb z zafiksowanym minimum i maksimum
                               double imin, double imax
                               )  //Tablica struktur zawierających informacje o seriach
: graph(ix1, iy1, ix2, iy2), M(0),
  series(nullptr), scales(nullptr), mode(imode), scale_y(imin, imax)
{
    assert(scale_y.min <= scale_y.max);
    wb_ptr<series_info> se(new series_info[N]);
    wb_ptr<scaling_info> sc(new scaling_info[N]);
    series = se.give();
    assert(series != nullptr);
    scales = sc.give();
    assert(scales != nullptr);

    if(series != nullptr && scales != nullptr)
        M = N; //Już jest tyle miejsca

    if(iseries != nullptr)
    {
        for(size_t i = 0; i < M; i++)
        {
            if(iseries[i].ptr == nullptr)
                break;
            series[i] = iseries[i];
        }
    }
}

sequence_graph::sequence_graph(int ix1, int iy1, int ix2, int iy2, //Położenie obszaru
                               int N,            //Liczba serii
                               data_source_base **iseries,
                               int imode,    //0 - tryb z pełnym reskalowaniem
        //1 - tryb ze wspólnym minimum,maximum,pobieranym z parametrów i serii
        //2 - tryb z zafiksowanym minimum i maksimum
                               double imin, double imax
) :  //Tablica struktur zawierających informacje o seriach
        graph(ix1, iy1, ix2, iy2), M(0),
        series(nullptr), scales(nullptr), mode(imode), scale_y(imin, imax)
{
    assert(scale_y.min <= scale_y.max);
    wb_ptr<series_info> se(new series_info[N]);
    wb_ptr<scaling_info> sc(new scaling_info[N]);
    series = se.give();
    assert(series != nullptr);
    scales = sc.give();
    assert(scales != nullptr);

    if(series != nullptr && scales != nullptr)
        M = N; //Już jest tyle miejsca

    if(iseries != nullptr)
    {
        for(size_t i = 0; i < M; i++)
        {
            if(iseries[i] == nullptr)
                break;
            series[i].ptr = iseries[i]; //Reszta domyślna
        }
    }
}

// IMPLEMENTATION OF VIRTUAL METHODS
int sequence_graph::configure(const void *config)
//Parametr typu config_seq. Jeśli nullptr to tryb 0;
{
    config_seq *c = (config_seq *) config;
    if(c != nullptr)
    {
        mode = c->mode;
        scale_y.set(c->min, c->max, 1);
    } else mode = 0;
    return 0;
}

int sequence_graph::set_series(size_t index, data_source_base *data, int menage)
//zwraca -1, jeśli indeks za duży
{
    assert(series != nullptr);
    if(index >= M)
        return -1;
    if(series[index].menage && series[index].ptr != nullptr)
        delete series[index].ptr;
    series[index].ptr = data;
    series[index].menage = menage;
    return 0;
}

data_source_base *sequence_graph::get_series(size_t index)
//zwraca nullptr, jeśli indeks za duży
{
    assert(series != nullptr);
    if(index >= M)
        return nullptr;
    return series[index].ptr;
}

// reals[] zawiera wysokość i wartość ustalająca kolor
int sequence_graph::_rescale_data_point(const double reals[], long in_area[])
//zwraca -1, jeśli nie w oknie
{
    int flaga = 0;

    if(mode == 0)
    {
        for(unsigned i = 0; i < M; i++)
            if(reals[i] <= scales[i].max && reals[i] >= scales[i].min)
            {
                unsigned pom = (unsigned) scales[i].get(reals[i]);
                in_area[i] = pom;
            } else flaga = 1;
    } else
    {
        for(unsigned i = 0; i < M; i++)
            if(reals[i] <= scale_y.max && reals[i] >= scale_y.min)
            {
                unsigned pom = (unsigned) scale_y.get(reals[i]);
                in_area[i] = pom;
            } else
            {
                flaga = 1;
                if(reals[i] > scale_y.max)
                    in_area[i] = INT_MAX;
                if(reals[i] < scale_y.min)
                    in_area[i] = INT_MIN;
            }
    }

    if(reals[M] <= scale_x.max && reals[M] >= scale_x.min)
    {
        unsigned pom = (unsigned) scale_x.get(reals[M]);
        in_area[M] = pom;
    } else flaga = 1;

    if(flaga)
        return -1;
    else
        return 0;
}

inline unsigned sequence_graph::color(unsigned ind)
{
    assert(ind < M);
    if(ind >= M) return unsigned(-1); //Błąd
    if(series[ind].color != -1)
        return series[ind].color;
    else
        return unsigned(c_range.start + s_colors.get(ind));
}


void sequence_graph::_replot()
// Rysuje właściwy wykres a pod nim ewentualnie legendę
{
    unsigned i, MaxNum = 0;
    int x1 = get_start_x();
    int y1 = get_start_y();
    int x2 = x1 + get_width() - 1; //-1, bo width obejmuje pierwszy piksel
    int y2 = y1 + get_height() - 1;
    int height = y2 - y1;
    int width = x2 - x1;
    assert(M >= 1);

    wb_dynarray<data_source_base::iterator_h> Hh(M);
    wb_dynarray<double> Rh(M + 1);
    wb_dynarray<long> Ah(M + 1);
    wb_dynarray<long> Bh(M + 1);
    wb_dynarray<unsigned> Ch(M);
    wb_dynarray<unsigned> Nh(M);
    wb_dynarray<char> Oh(M);
    wb_dynarray<double> Missing(M + 1);

    data_source_base::iterator_h *H = Hh.get_ptr_val();
    double *Miss = Missing.get_ptr_val(); //Missing values
    double *R = Rh.get_ptr_val(); //Input dla _rescale_data_point
    long *A = Ah.get_ptr_val(); //Output dla _rescale_data_point
    long *B = Bh.get_ptr_val(); //--------//--------
    unsigned *C = Ch.get_ptr_val(); //Kolory
    unsigned *Num = Nh.get_ptr_val(); //Iteratory
    char *OK = Oh.get_ptr_val(); //Flagi zakresowe dla trybu 2

    assert(H != nullptr);
    assert(R != nullptr);
    assert(A != nullptr);
    assert(B != nullptr);
    assert(Num != nullptr);


    if(t_colors.start != get_background()) //Będzie legenda
    {
        height -= toi(3 * char_height('0'));
        width -= 4;
    }

    if(mode == 2) //Konfiguracja min max dla osi Y
    {
        //assert(scale_y.min<scale_y.max);
        if(scale_y.min >= scale_y.max)
        {//Awaria — zle parametry
            print_width(x1, (y1 + y2) / 2, x2 - x1, t_colors.start, get_background(), "%@CInvalid min,max range:<%g,%g>",
                        scale_y.min, scale_y.max);
            return;
        }
    } else
    {
        if(scale_y.min > scale_y.max)
        {//Awaria — zle parametry
            print_width(x1, (y1 + y2) / 2, x2 - x1, t_colors.start, get_background(), "%@CInvalid min,max range:<%g,%g>",
                        scale_y.min, scale_y.max);
            return;
        }
        if(scale_y.min == scale_y.max) //Np. default ==0
        {//Do ustalenia w czasie przeszukiwania serii
            scale_y.min = DBL_MAX;
            scale_y.max = -DBL_MAX;
        }
    }

    for(i = 0; i < M; i++)
    {
        if(series[i].ptr != nullptr)
        {
            double min, max;
            size_t num;
            series[i].ptr->bounds(num, min, max);
            scales[i].set(min, max, height);
            Miss[i] = series[i].ptr->get_missing();
            Num[i] = num;
            if(num > MaxNum)
                MaxNum = num;
            if(mode != 2)
            {
                if(scale_y.min > min)
                    scale_y.min = min;
                if(scale_y.max < max)
                    scale_y.max = max;
            }
        }
    }

    if(MaxNum == 0) //Brak danych
    {
        print_width(x1, (y1 + y2) / 2, x2 - x1, t_colors.start, get_background(), "%@CInvalid data(s)");
        return;
    }

//Dodatkowe skalowania
    scale_y.set(scale_y.min, scale_y.max, height); //Dla osi Y w trybie 1 i 2
    scale_x.set(0, MaxNum, width); //Dla osi X
    s_colors.set(0,toi(M), c_range.end - c_range.start + 1); //Dla kolorów serii

    if(t_colors.start != default_transparent && t_colors.start != get_background()) //Rysujemy legendę
    {
        char bufor[80];
        int thickness = toi((x2 - x1) / M);
        if(mode == 0 && thickness > 1)
        {
            for(i = 0; i < M; i++)
                if(series[i].ptr != nullptr)
                {
                    sprintf(bufor, "%g ", scales[i].max);
                    printc(toi(x1 + i * thickness), y1, color(i), get_background(), "%s", trunc(bufor, thickness));
                }
            for(i = 0; i < M; i++)
                if(series[i].ptr != nullptr)
                {
                    sprintf(bufor, "%g ", scales[i].min);
                    printc(toi(x1 + i * thickness), toi(y2 - 2 * char_height('0')), color(i), get_background(), "%s",
                           trunc(bufor, thickness));
                }
        } else //mode>0
        {
            sprintf(bufor, "%g ", scale_y.max);
            printc(x1, y1, t_colors.start, get_background(), "%s", trunc(bufor, width));
            sprintf(bufor, "%g ", scale_y.min);
            printc(x1,toi(y2 - 2 * char_height('0')), t_colors.start, get_background(), "%s", trunc(bufor, width));
        }

        if(thickness > 1)
            for(i = 0; i < M; i++)
                if(series[i].ptr != nullptr)
                {
                    const char *pom = series[i].ptr->name();
                    int len = toi(strlen(pom));
                    sprintf(bufor, "%*s;", len > 60?60:len, pom);
                    printc(toi(x1 + i * thickness), toi(y2 - char_height('0')), color(i), get_background(), "%s",
                           trunc(bufor, thickness));
                }
        y1 += toi(char_height('0'));
        y2 -= toi(2 * char_height('0'));

        //Rysowanie strzałki
        if(x2 - x1 > 10 && y2 - y1 > 10)
        {
            if(mode > 0)
            {
                scale_y.OY_axis(x1, y1, x1 + 6, y2, t_colors.start, get_background());
            } else
            {
                scaling_info pom(DBL_MAX / 2.0, DBL_MAX, INT_MAX); //Żeby nie było żadnej skali ani 0
                pom.OY_axis(x1, y1, x1 + 6, y2, t_colors.start, get_background());
            }
            x1 += 4;
        }

    } //KONIEC LEGENDY

//WYPISANIE LICZBY PRÓBEK
    if(t_colors.end != default_transparent && t_colors.end != get_background())
    {
        print_width(x2 - (x2 - x1) / 3, toi(y2 - char_height('0')), (x2 - x1) / 3, get_background(), t_colors.end,
                    "%@R%u", MaxNum);
    }

//Rysowanie punktów lub linii
//...
    if(x2 - x1 > 10 && y2 - y1 > 5) //Musi być troszkę miejsca i troche danych
    {
        for(i = 0; i < M; i++) //Inicjowanie tablic
        {
            if(series[i].ptr != nullptr)
            {
                H[i] = series[i].ptr->reset();
                C[i] = color(i);
            } else C[i] = default_color;
            R[i] = scales[i].max; //także w R coś na początek, żeby nie było przypadkowo
        }


        for(unsigned j = 0; j < MaxNum; j++) // Iteracja po krokach
        {
            double test;
            long *pom = (j % 2 == 0?A:B); //Wybór aktualnej tablicy wyników
            long *old = (j % 2 == 1?A:B); //Wybór starej tablicy wyników
            R[M] = j;

            for(i = 0; i < M; i++) //Ładowanie. Pętla po seriach
                if(C[i] != -1) //Seria aktywna
                {
                    OK[i] = 1; //Z zaufaniem, że jest dobrze
                    if(j >= Num[i])
                    {
                        C[i] = default_color; //Zakończono rysowanie tej seri
                        continue;
                    }

                    test = series[i].ptr->get(H[i]);
                    if(test == Miss[i])
                    {
                        OK[i] = 0; //Missing,zostaje stara wartość
                        continue;
                    }

                    R[i] = test;
                }

            if(_rescale_data_point(R, pom) == -1) //Czy jest jakiś poza zakresem
            {
                if(mode != 2)
                {
                    assert(1);
                    continue;    //Nie rysuje, gdy coś wyłazi
                }
            }
            else
            {
                for(i = 0; i < M; i++)
                {
                    if(pom[i] == INT_MAX)
                    {
                        pom[i] = height + 1;
                        if(old[i] == height + 1) //Oj, niedobrze!
                            OK[i] = 0;
                    }
                    if(pom[i] == INT_MIN)
                    {
                        pom[i] = -1;
                        if(old[i] == -1)
                            OK[i] = 0; //Oj, niedobrze!
                    }
                }
            }

            //Rysowanie punktami
            for(i = 0; i < M; i++)
                if(C[i] != -1 && series[i].figure && OK[i])
                {
                    series[i].figure->base_len = this->get_height();
                    series[i].figure->plot(toi(x1 + pom[M]), toi(y2 - pom[i]), series[i].figure->base_size(), C[i]);
                }

            //Rysowanie liniami
            if(j > 0) //Musi być już jeden krok
                for(i = 0; i < M; i++)
                    if(C[i] != -1 && series[i].figure == nullptr && OK[i])
                    {
                        int oldX = toi(old[M]);
                        if(old[i] == -1 || old[i] == height + 1)
                            oldX = toi(pom[M]); //Pion
                        line(x1 + oldX, toi(y2 - old[i]), toi(x1 + pom[M]), toi(y2 - pom[i]), C[i]);
                    }
        }

        //Zamykanie iteratorów
        for(i = 0; i < M; i++)
            if(series[i].ptr != nullptr)
                series[i].ptr->close(H[i]);

    } else
    {
        print_width(x1, (y1 + y2) / 2, x2 - x1, t_colors.start, get_background(), "%@CToo small area");
    }
}


// rainbow_graph
//--------------------------------------------

//DESTRUCTOR
rainbow_graph::~rainbow_graph()
{
    //if(format)
    delete format;
    if(datas && d_menage) delete datas;
    datas = nullptr;
    d_menage = false;
    if(colors && c_menage) delete colors;
    colors = nullptr;
    c_menage = false;
}

//CONSTRUCTOR(S)
rainbow_graph::rainbow_graph(int ix1, int iy1, int ix2, int iy2, //Położenie obszaru
                             data_source_base *i_datas, int menage_d, //datas-dane == wartości
                             data_source_base *i_colors, int menage_c, //colors-źródło danych o kolorach
                             const char *iformat   //format liczby — nie może być więcej niż 1 parametr!
) : graph(ix1, iy1, ix2, iy2),
    datas(i_datas), d_menage(menage_d),
    colors(i_colors), c_menage(menage_c),
    format(nullptr)
{
    assert(datas != nullptr);
    rainbow_graph::configure(iformat);
}


// IMPLEMENTATION OF VIRTUAL METHODS of rainbow_graph
//-----------------------------------------------------
int rainbow_graph::configure(const void *vformat)
//Wymaga const char* format jako parametr
{
    const char *iformat = (const char *) vformat;
    if(iformat == nullptr)
    {
        //if(format)
        delete format;
        format = clone_str("%%@R%g"); //default
        return 0;
    }
    const char *pom = iformat;
    int licznik = 0;
    while((pom = strchr(pom, '%')) != nullptr)
    {
        pom++;
        if(*pom == '%' || *pom == '@')
        {
            pom++;
            continue; //Nie liczba — powtórz!
        }
        while(!isalpha(*pom)) pom++;
        if(tolower(*pom) == 'g' ||
           tolower(*pom) == 'e' ||
           tolower(*pom) == 'f')
            licznik++; //Musi być jeden!!!
        else
            return -1; //Niedopuszczalny!
    }
    if(licznik != 1) return -1; //Za dużo lub za mało
    //if(format)
    delete format;
    format = clone_str(iformat);
    return 0;
}

int rainbow_graph::set_series(size_t index, data_source_base *idata, int imenage)
//zwraca -1, jeśli indeks za duży
{
    if(index > 1) return -1; //Tylko dwie serie
    assert(idata != nullptr);
    if(index == 0)
    {
        if(d_menage) delete datas;
        datas = idata;
        d_menage = imenage;
    } else
    {
        if(c_menage) delete colors;
        colors = idata;
        c_menage = imenage;
    }
    return 0;
}

data_source_base *rainbow_graph::get_series(size_t index)
//zwraca nullptr, jeśli indeks za duży
{
    if(index > 0)
        return nullptr;
    if(index == 0)
        return datas;
    else
        return colors;
}


void rainbow_graph::_replot()
{
    int x1 = get_start_x();
    int y1 = get_start_y();
    int x2 = x1 + get_width() - 1; //-1, bo width obejmuje pierwszy piksel
    int y2 = y1 + get_height() - 1;
    double minc, maxc;
    size_t num, num_color, height;
    int flaga = 0;

    assert(x1 < x2);
    assert(y1 < y2);
    assert(format != nullptr);

    char *Format = format; //Format dla sprintf
    char Format2[6] = "%@R%s"; //Format dla print_width
    if(Format[0] == '%' && Format[1] == '@')
    {
        Format2[2] = Format[2];
        Format += 3;
    }

//Trzeba znać num!
    datas->bounds(num, minc, maxc);

//Legenda wtedy, jeśli jest potrzebna
    if(t_colors.start != get_background() || (t_colors.end != get_background() && colors))
        height = (y2 - y1) - 2 * char_height('0'); //będzie legenda
    else
        height = y2 - y1; //nie będzie legendy

//Do skalowania kolorów, jeśli jest seria
    if(colors != nullptr)
    {
        colors->bounds(num_color, minc, maxc);
        assert(num <= num_color);
        s_colo.set(minc, maxc, c_range.end - c_range.start + 0.999);
    }

//Wypisywanie legendy dla wartości
    if(t_colors.start != default_transparent && t_colors.start != get_background())
    {
        const char *pom = datas->name();
        print_width(x1, toi(y2 - char_height('0')), (x2 - x1) / 2, t_colors.start, get_background(),
                    "%*s", strlen(pom) > 79?79:strlen(pom), pom);
        flaga = 1;
    }

// i dla drugiej
    if(t_colors.end != default_transparent && t_colors.end != get_background() && colors != nullptr)
    {
        int width = print_width(x2 - (x2 - x1) / 2, y1, (x2 - x1) / 2,
                                c_range.start, c_range.start != get_background()?get_background():c_range.end,
                                "%@R%g", minc);

        const char *pom = colors->name();
        print_width((x2 + x1) / 2, y1, (x2 - x1) / 2 - width, get_background(), c_range.start,
                    "%@R%*s", strlen(pom) > 60?60:strlen(pom), pom);

        print_width(x2 - (x2 - x1) / 2, toi(y2 - char_height('0')), (x2 - x1) / 2,
                    c_range.end, c_range.end != get_background()?get_background():c_range.start,
                    "%@R%g", maxc);

        flaga++;
    }

//Zawężenie po y-grekach
    if(flaga == 2)
    {
        y1 += toi(char_height('0'));
        y2 -= toi(char_height('0'));
    } else if(flaga == 1)
        y2 -= toi(char_height('0'));

//Rysowanie skali -jeśli są co najmniej dwa kolory i jest miejsce na co najmniej 2 piksele
    if(colors != nullptr && x2 - x1 >= 10 && c_range.end > c_range.start && c_range.end - c_range.start >= 1)
    {
        c_range.plot(x2 - 5, y1, x2, y2);
        x2 -= 7;
    }
//NIE_DA_SIE:;



//Wypisywanie wartości
    if(y2 - y1 >= 2 * char_height('0'))
    {
        data_source_base::iterator_h h = datas->reset();
        data_source_base::iterator_h c = colors?colors->reset():nullptr;
        int thickness = toi(char_height('0'));
        assert(Format != nullptr);
        for(unsigned i = 0; i < num; i++)
        {
            double test=datas->get_missing();
            wb_color color = default_black;
            //int straz1=0xabcd;
            char bufor[128] = {0};
            //int straz2=0xabcd;
            assert(thickness >= 1);
            if((i + 2) * thickness >= size_t(y2 - y1) )
            {
                strcpy(bufor, "...");
            }
            else
            {
                sprintf(bufor, Format, test = datas->get(h) );
            }

            if(test == datas->get_missing())
                bufor[0] = '\0';

            assert(strlen(bufor) <= 127);

            if(colors == nullptr)
            {
                print_width(x1, toi(y1 + i * thickness), x2 - x1, c_range.start, get_background(), Format2, bufor);
            }
            else
            {
                test = s_colo.get(colors->get(c));
                color = int(test); //Test dla warunku dalej.
                color += c_range.start;
                if(color != get_background() && test != colors->get_missing())
                {
                    fill_rect(x1, toi(y1 + i * thickness),
                              x2 + 1, toi(y1 + (i + 1) * thickness + 1), color);
                    print_width(x1, toi(y1 + i * thickness), x2 - x1, get_background(), color, Format2, bufor);
                } else
                {
                    color = (255 != get_background()?255:0);
                    print_width(x1, toi(y1 + i * thickness), x2 - x1, color, get_background(), Format2, bufor);
                    rect(x1, toi(y1 + i * thickness),
                         x2, toi(y1 + (i + 1) * thickness),
                         color);
                }
            }

            if((i + 2) * thickness >= size_t(y2 - y1))
                break; //Nie ma miejsca na resztę pętli
        }
        datas->close(h);
        if(colors) colors->close(c);
    } else
    {
        printc(x1, (y1 + y2) / 2, t_colors.start, get_background(), "Too small area");
    }
}


// reals[] zawiera wartość ustalająca kolor albo (min - 1)
int rainbow_graph::_rescale_data_point(const double reals[], long in_area[])
// zwraca -1, jeśli nie w oknie
{
    if(reals[0] <= s_colo.max && reals[0] >= s_colo.min)
    {
        unsigned color = (unsigned) s_colo.get(reals[0]);
        in_area[0] = color + c_range.start;
        return 0;
    }
    return -1;
}

//scatter_graph & net_graph
//-----------------------------------------------------------------------
//DESTRUCTOR
scatter_graph::~scatter_graph()
{
    if(CurrConfig == nullptr)
        fprintf(stderr,"Warning: Visualisation config of `%s` is NULL!\n",this->name());
    if(menage_p)
        delete CurrConfig;
    if(Xdata != nullptr && menage_x)
        delete Xdata;
    if(Ydata != nullptr && menage_y)
        delete Ydata;
    if(colors != nullptr && menage_c)
        delete colors;
    if(sizes != nullptr && menage_s)
        delete sizes;
}

net_graph::~net_graph()
{
    if(Sources != nullptr && menage_so)
        delete Sources;
    if(Targets != nullptr && menage_t)
        delete Targets;
    if(Arrows != nullptr && menage_a)
        delete Arrows;
    if(ArrColors != nullptr && menage_ac)
        delete ArrColors;
}

//CONSTRUCTOR(S)
scatter_graph::scatter_graph(int ix1,  int iy1, int ix2, int iy2, //Położenie obszaru
                             data_source_base *i_X_data, int i_menage_x, //dane o X-ach
                             data_source_base *i_Y_data, int i_menage_y, //dane o Y-ach
                             data_source_base *i_colors, int i_menage_c, //dane o kolorach
                             data_source_base  *i_sizes, int i_menage_s, //dane o rozmiarach
                             config_point        *i_fig, int f_menage    //figura domyślna
) :
        graph(ix1, iy1, ix2, iy2),
        CurrConfig(nullptr), menage_p(false),
        Xdata(i_X_data), menage_x(i_menage_x),
        Ydata(i_Y_data), menage_y(i_menage_y),
        colors(i_colors), menage_c(i_menage_c),
        sizes(i_sizes), menage_s(i_menage_s)
{
    assert(Xdata != nullptr);
    assert(Ydata != nullptr);
    config_scat pom(i_fig, f_menage);
    if(i_fig != nullptr)
        scatter_graph::configure(&pom);
    else
        scatter_graph::configure(nullptr);
}

net_graph::net_graph(int ix1, int iy1, int ix2, int iy2, //Położenie obszaru
                     data_source_base *iXdata, int imenage_x, //dane o X-ach
                     data_source_base *iYdata, int imenage_y, //dane o Y-ach
                     data_source_base *iSources, int imenage_so, //indeksy źródeł
                     data_source_base *iTargets, int imenage_t, //indeksy celów
                     data_source_base *iColors, int imenage_c, //dane o kolorach
                     data_source_base *iSizes, int imenage_s, //dane o rozmiarach
                     data_source_base *iArrows, int imenage_a, //rozmiary strzałek - 0 brak
                     data_source_base *iArrColors, int imenage_ac, //Kolory strzałek
                     config_point *ifig, int menage_f //figura domyślna
) :
        scatter_graph(ix1, iy1, ix2, iy2,
                      iXdata, imenage_x, //dane o X-ach
                      iYdata, imenage_y, //dane o Y-ach
                      iColors, imenage_c, //dane o kolorach
                      iSizes, imenage_s, //dane o rozmiarach
                      ifig, menage_f //Domyślny może być brak rysowania węzłów sieci
        ),
        Sources(iSources), Targets(iTargets), Arrows(iArrows), ArrColors(iArrColors),
        menage_so(imenage_so), menage_t(imenage_t), menage_a(imenage_a), menage_ac(imenage_ac)
{
    assert(Sources != nullptr);
    assert(Targets != nullptr);
}

// IMPLEMENTATION OF VIRTUAL METHODS
int scatter_graph::configure(const void *p)
//Parametr typu scatter_graph::config*
{
    if(menage_p && CurrConfig != nullptr)
        delete CurrConfig;
    if(p != nullptr)
    {
        CurrConfig = ((config_scat *) p)->fig;
        menage_p = ((config_scat *) p)->menage;
    } else
    {
        CurrConfig = nullptr; //new hash_point;
        menage_p = false;
    }
    return 0;
}

int scatter_graph::set_series(size_t index, data_source_base *data, int menage)
//zwraca -1, jeśli indeks za duży
{
    assert(data != nullptr);
    switch(index)
    {
        case 0:
            if(menage_x && Xdata != nullptr)
                delete Xdata;
            Xdata = data;
            menage_x = menage;
            break;
        case 1:
            if(menage_y && Ydata != nullptr)
                delete Ydata;
            Ydata = data;
            menage_y = menage;
            break;
        case 2:
            if(menage_c && colors != nullptr)
                delete colors;
            colors = data;
            menage_c = menage;
            break;
        case 3:
            if(menage_s && sizes != nullptr)
                delete sizes;
            sizes = data;
            menage_s = menage;
            break;
        default:
            return -1;
    }
    return 0;
}

int net_graph::set_series(size_t index, data_source_base *data, int menage)
//zwraca -1, jeśli indeks za duży
{
    assert(data != nullptr);
    switch(index)
    {
        case 0:
            if(menage_x && Xdata != nullptr)
                delete Xdata;
            Xdata = data;
            menage_x = menage;
            break;
        case 1:
            if(menage_y && Ydata != nullptr)
                delete Ydata;
            Ydata = data;
            menage_y = menage;
            break;
        case 2:
            if(menage_c && colors != nullptr)
                delete colors;
            colors = data;
            menage_c = menage;
            break;
        case 3:
            if(menage_s && sizes != nullptr)
                delete sizes;
            sizes = data;
            menage_s = menage;
            break;
        case 4:
            if(menage_so && Sources != nullptr)
                delete Sources;
            Sources = data;
            menage_so = menage;
            break;
        case 5:
            if(menage_t && Targets != nullptr)
                delete Targets;
            Targets = data;
            menage_t = menage;
            break;
        case 6:
            if(menage_a && Arrows != nullptr)
                delete Arrows;
            Arrows = data;
            menage_a = menage;
            break;
        case 7:
            if(menage_ac && ArrColors != nullptr)
                delete ArrColors;
            ArrColors = data;
            menage_ac = menage;
            break;
        default:
            return -1;
    }
    return 0;
}

data_source_base *scatter_graph::get_series(size_t index)
//zwraca NULL, jeśli indeks za duży...
{
    switch(index)
    {
        case 0:
            return Xdata;
        case 1:
            return Ydata;
        case 2:
            return colors;
        case 3:
            return sizes;
        default:
            return nullptr;
    }
}

data_source_base *net_graph::get_series(size_t index)
//zwraca nullptr, jeśli indeks za duży
{
    switch(index)
    {
        case 0:
            return Xdata;
        case 1:
            return Ydata;
        case 2:
            return colors;
        case 3:
            return sizes;
        case 4:
            return Sources;
        case 5:
            return Targets;
        case 6:
            return Arrows;
        case 7:
            return ArrColors;
        default:
            return nullptr;
    }
}


int scatter_graph::_rescale_data_point(const double reals[], long in_area[])
//zwraca -1, jeśli coś nie tak
{
    int flaga = 0;

    if(reals[0] <= scale_x.max && reals[0] >= scale_x.min)
    {
        unsigned pom = (unsigned) scale_x.get(reals[0]);
        in_area[0] = pom;
    } else flaga = 1;

    if(reals[1] <= scale_y.max && reals[1] >= scale_y.min)
    {
        unsigned pom = (unsigned) scale_y.get(reals[1]);
        in_area[1] = pom;
    } else flaga = 1;

    if(reals[2] <= scale_c.max && reals[2] >= scale_c.min)
    {
        unsigned color = (unsigned) scale_c.get(reals[2]);
        in_area[2] = color + c_range.start;
    }

    if(reals[3] <= scale_s.max && reals[3] >= scale_s.min)
    {
        unsigned size = (unsigned) scale_s.get(reals[3]);
        in_area[3] = size + CurrConfig->base_size();
    }

    if(flaga)
        return -1;
    else
        return 0;

    return -1;
}


void scatter_graph::_replot()
// Rysuje właściwy wykres a pod nim ewentualnie legendę
{
    int x1 = get_start_x();
    int y1 = get_start_y();
    int x2 = x1 + get_width() - 1; //-1, bo width obejmuje pierwszy piksel
    int y2 = y1 + get_height() - 1;
    size_t num, num_X, num_Y, num_color, num_size; //Number of values in each series
    double missX, missY, missColor, missSize;    //missing values for each series
    int flaga = 0;

    num = num_X = num_Y = num_color = num_size = 0; //Abrakadabra

    if((y2-y1-2*frame_width) < 1.2 * char_height('X')) //Za małe tak czy siak...
    {
        print_width(x1,y1,x2 - x1, t_colors.start, get_background(), "%@CToo small area");
        return;
    }

//Miejsce na legendę, wtedy gdy jest potrzebna
    unsigned height, width;
    if((t_colors.start != default_transparent && t_colors.start != get_background()) ||
       (t_colors.end != default_transparent && t_colors.end != get_background() && (colors || sizes)))
    {
        height = (y2 - y1) - (2 * char_height('0') + 3); //będzie legenda
        if(sizes != nullptr && t_colors.end != get_background())
        {
            CurrConfig->base_len = min_(this->get_width(), this->get_height()); //szacunkowo
            /*height-=char_height('0');*/height -= max_(CurrConfig->max_size(), unsigned(char_height('0')));
        }
        width = (x2 - x1) - 3;
        if(colors != nullptr && t_colors.end != get_background())
            width -= 10; //Musi być jakiś odstęp
    } else
    {
        height = y2 - y1; //nie będzie legendy
        width = x2 - x1;
    }

    {//Ustawianie struktur skalujących
        double min = 0;
        double max = -1; //Byle co, i tak zaraz zostanie przypisane

//Do skalowania X-ow
        assert(Xdata != nullptr);
        Xdata->bounds(num_X, min, max);
        if(num_X == 0 || min == max || min > max) goto SKIP; //Seria wadliwa — PUSTA?
        missX = Xdata->get_missing();
        scale_x.set(min, max, width);

//Do skalowania Y-ow
        assert(Ydata != nullptr);
        Ydata->bounds(num_Y, min, max);
        if(num_Y == 0 || min == max || min > max) goto SKIP; //Seria wadliwa — PUSTA?
        missY = Ydata->get_missing();
        scale_y.set(min, max, height);

//Do skalowania kolorów, jeśli jest seria i ma odstęp między min i max
        if(colors != nullptr)
        {
            colors->bounds(num_color, min, max);
            missColor = colors->get_missing();
            if(num_color == 0 || min == max || min > max) //Seria wadliwa — PUSTA?
            {
                num_color = num_X; //Trzeba, żeby się rysowało bez tego — taki sygnał...
            } else
            {
                scale_c.set(min, max, c_range.end - c_range.start +
                                      0.999); //Do kolorów potrzebna poprawka 0.999 w skalowaniu, bo nie będzie białego (max.)
            }
        } else num_color = num_X;

//Do skalowania rozmiarów, jeśli jest seria i ma odstęp miedzy min i max
        if(sizes != nullptr)
        {
            //DEBUG min=max=0;
            sizes->bounds(num_size, min, max);
            missSize = sizes->get_missing();
            if(num_size == 0 || min == max || min > max) //Seria wadliwa — PUSTA?
            {
                num_size = num_X; //Trzeba, żeby się rysowało bez tego — taki sygnał...
            } else
            {
                CurrConfig->base_len = min_(this->get_width(), this->get_height());
                scale_s.set(min, max, CurrConfig->max_size() - CurrConfig->base_size());
            }
        } else num_size = num_X;

    }

    //Rysowanie legendy dla rozmiarów
    if(t_colors.end != default_transparent && t_colors.end != get_background() && (sizes != nullptr) && vis_leg4)
    {
        double min, max;
        unsigned rmin, rmax;
        size_t dummy;
        sizes->bounds(dummy, min, max);
        rmin = dtoi(scale_s.get(min) + CurrConfig->base_size()); //scale_s.get(scale_s.min)+1
        rmax = dtoi(scale_s.get(max) + CurrConfig->base_size()); //scale_s.get(scale_s.max)+1 ALE: (CurrConfig->max_size()-CurrConfig->base_size())/2 ???

        CurrConfig->plot(toi(x1 + rmin / 2), toi(y1 + rmin / 2), rmin, (c_range.end - c_range.start) / 2);
        CurrConfig->plot(toi(x2 - rmax / 2), toi(y1 + rmax / 2), rmax, (c_range.end - c_range.start) / 2);

        int x = toi(x1 + CurrConfig->base_size() + 1);
        int xE = toi(x2 - CurrConfig->max_size() - 1);
        int loc_width = xE - x;
        loc_width = print_width(x, y1, loc_width / 3, t_colors.end, get_background(),
                                "%g", scale_s.min);

        if(loc_width == 0) goto SKIP; //Nie ma miejsca?
        else x += loc_width;

        const char *pom = sizes->name();
        loc_width = print_width(x, y1, (xE - x) / 2, get_background(), t_colors.end,
                                " %*s", strlen(pom) > 60?60:strlen(pom), pom);

        if(loc_width == 0) goto SKIP;
        else x += loc_width;

        print_width(x, y1, xE - x, t_colors.end, get_background(),
                    "%@R%g", scale_s.max);

        y1 += max_(CurrConfig->max_size(), unsigned(char_height('0')));
    }

    SKIP:
    // Wypisywanie legendy dla 1. i 2. seri.
    if(t_colors.start != default_transparent && t_colors.start != get_background() && vis_leg1)
    {
        int swidth = 0;
        int pos = x1;
        //Do drukowania maksimum Y
        print_width(x1, y1, (x2 - x1) / 2, t_colors.start, get_background(),
                    "%g", scale_y.max);

        const char *pom2 = Xdata->name();
        const char *pom1 = Ydata->name();

        //Min Y i seria I
        swidth = print_width(pos, toi(y2 - char_height('0')), (x2 - x1) / 2, t_colors.start, get_background(),
                             "%g %*s", scale_y.min, strlen(pom1) > 60?60:strlen(pom1), pom1);
        pos += swidth;

        //seria II
        swidth = print_width(pos, toi(y2 - char_height('0')), (x2 - pos) / 3 * 2, t_colors.start, get_background(),
                             ",%*s", strlen(pom2) > 60?60:strlen(pom2), pom2);
        if(pos == 0) goto SKIP2;
        pos += swidth;

        //Min X
        swidth = print_width(pos, toi(y2 - char_height('0')), (x2 - pos) / 2, t_colors.start, get_background(),
                             "<%g", scale_x.min);
        if(pos == 0) goto SKIP2;
        pos += swidth;

        //Max X
        print_width(pos, toi(y2 - char_height('0')), x2 - pos, t_colors.start, get_background(),
                    "%@R,%g>", scale_x.max);

        flaga = 1;
    }

    SKIP2:
    // i dla trzeciej
    if(t_colors.end != default_transparent && t_colors.end != get_background() && (colors != nullptr) && vis_leg2)
    {
        int swidth = (x2 - x1) / 2;
        //Zakres serii kolorów — najwyżej 2/3 miejsca
        int width1 = print_width(x2 - swidth / 3, y1, swidth / 3,
                                 c_range.end, c_range.end != get_background()?get_background():c_range.start,
                                 "%@R,%g>", scale_c.max);

        int width2 = print_width(x2 - width1 - swidth / 3, y1, swidth / 3,
                                 c_range.start, c_range.start != get_background()?get_background():c_range.end,
                                 "%@R<%g", scale_c.min);

        // Nazwa serii kolorów — reszta
        const char *pom = colors->name();
        print_width((x1 + x2) / 2, y1, swidth - width1 - width2, get_background(), t_colors.start,
                    "%@R%*s", strlen(pom) > 60?60:strlen(pom), pom);

        flaga = 1;
    }

    //Zawężenie po y-grekach
    if(flaga)
    {
        y1 += toi(char_height('0'));
        y2 -= toi(char_height('0'));
        if(y1>=y2) return; //ZA NISKO!
           // goto ZA_NISKO;
    }


    //Rysowanie skali -jeśli są co najmniej dwa kolory i jest miejsce na co najmniej 2 piksele
    if(t_colors.end != default_transparent && t_colors.end != get_background() && colors != nullptr &&
       x2 - x1 >= 10 && c_range.end > c_range.start && c_range.end - c_range.start >= 1 && vis_leg3)
    {
        c_range.plot(x2 - 5, y1, x2, y2);
        x2 -= 10;
    }
//NIE_DA_SIE:;

    //Strzałka w prawo
    if(x2 - x1 > 10 && y2 - y1 > 10)
    {
        scale_x.OX_axis(x1 + 3, y2 - 6, toi(x1 + 3 + width), y2, t_colors.start, get_background());
        y2 -= 3;
    }

    //Strzałka w gore
    if(x2 - x1 > 10 && y2 - y1 > 10)
    {
        scale_y.OY_axis(x1, y1, x1 + 6, y2, t_colors.start, get_background());
        x1 += 3;
    }

    //Rysowanie punktów
    num = min_(min_((size_t) num_X, num_Y), min_((size_t) num_size, num_color)); //cast for gcc

    graph_core.set(x1, y1, x2, y2); //Zapamiętanie gdzie leży właściwy obszar rysowania

    if(num > 0 && x2 - x1 > 10 && y2 - y1 > 10)
    {
        if(CurrConfig != nullptr) //Jeśli użytkownik w ogóle życzy sobie rysować jakieś punkty (bo klasa potomna może nie chcieć!)
        {
            data_source_base::iterator_h ix = Xdata->reset();
            data_source_base::iterator_h iy = Ydata->reset();
            data_source_base::iterator_h ic = colors?colors->reset():nullptr;
            data_source_base::iterator_h is = sizes?sizes->reset():nullptr;
            if(CurrConfig !=
               nullptr) //Jeśli użytkownik w ogóle życzy sobie rysować jakieś punkty (bo klasa potomna może nie chcieć!)
                for(unsigned i = 0; i < num; i++)
                {
                    CurrConfig->base_len = x2 - x1; //Ustawiamy bazowy rozmiar obszaru rysowania
                    double test;
                    double r[4] = {-1, -1, -1, -1};
                    long a[4] = {0, 0, (c_range.end - c_range.start) / 2,
                                 (CurrConfig->max_size() - CurrConfig->base_size()) / 2};
                    r[0] = test = Xdata->get(ix);
                    if(test == missX) continue;
                    r[1] = test = Ydata->get(iy);
                    if(test == missY) continue;
                    if(colors)
                    {
                        r[2] = test = colors->get(ic);
                        if(test == missColor) continue;
                    }
                    if(sizes)
                    {
                        r[3] = test = sizes->get(is);
                        if(test == missSize) continue;
                    }
                    _rescale_data_point(r, a);

                    CurrConfig->plot(toi(x1 + a[0]), toi(y2 - a[1]), a[3] + 1, a[2]);
                }
            Xdata->close(ix);
            Ydata->close(iy);
            if(sizes) sizes->close(is);
            if(colors) colors->close(ic);
        }
    } else
    {
        ZA_NISKO:
        print_width(x1, (y1 + y2) / 2, x2 - x1, t_colors.start, get_background(), "%@CInvalid data or too small area");
    }
}

void net_graph::_replot()
// Rysuje wykres punktowy a pod nim ewentualnie legendę
//a do tego łączy punkty liniami
{
    scatter_graph::_replot();

    assert(Sources != nullptr);
    assert(Targets != nullptr);

    size_t num_so, num_t, num_a, num_c, num; //Liczba elementów w każdej z serii i ostatecznie liczba minimalna (num)
    double mini, maxi, mina, maxa;
    double SameSize = sqrt(sqr(graph_core._x1() - graph_core._x2()) + sqr(graph_core._y1() - graph_core._y2())) * 0.025;

    Sources->bounds(num_so, mini, maxi);

    Targets->bounds(num_t, mini, maxi);

    //Sprawdzenie, czy jest wystarczająca ilość informacji o grotach
    if(Arrows)
    {
        Arrows->bounds(num_a, mina, maxa);
        if(num_a == 0) goto ERROR_HAPPENED;
    } else
        num_a = num_so; //Jak nie ma to, żeby num było dobrze ustawione

    //Do skalowania kolorów, jeśli jest seria
    if(ArrColors != nullptr)
    {
        ArrColors->bounds(num_c, mina, maxa);
        if(num_c == 0) goto ERROR_HAPPENED;
        //if(mina==maxa){mina--;maxa++;}???
        scale_ac.set(mina, maxa, c_range.end - c_range.start); //(c_range.end-1)-c_range.start+0.999);
    } else num_c = num_so; //Jak nie ma, to żeby num było dobrze ustawione

    //USTAWIANIE num NA DŁUGOŚĆ NAJKRÓTSZEJ SERII
    num = min_(min_(num_so, num_t), min_(num_a, num_c));

    if(num > 0)
    {
        //Wszystkie serie muszą udostępniać bezpośrednie czytanie!!!
        //for(size_t i=0;i<num;i++)
        for(size_t j = 0, i = num - 1; j < num; i--, j++) //W odwrotnej kolejności!
        {
            assert(i < num);
            double Si = Sources->get(i);
            double Ti = Targets->get(i);
            if(Sources->is_missing(Si) || Targets->is_missing(Ti))
                continue;

            double Present = 0;
            if(Arrows != nullptr)
            {
                Present = Arrows->get(i);
                if(Arrows->is_missing(Present))
                    continue;
            }

            double AColor = 0.5;
            if(ArrColors != nullptr)
            {
                AColor = ArrColors->get(i);
                if(ArrColors->is_missing(AColor))
                    continue;
            }

            assert(Si == size_t(Si));
            double ASize = 0.05; //Długość grota `== 5%` długości strzałki
            double test;

            double r[4] = {-1, -1, -DBL_MAX, -DBL_MAX};
            long a1[4] = {0, 0, -INT_MAX, -INT_MAX}; //TO MUSI BYĆ `long` bo jest brane do funkcji przez adres.
            long a2[4] = {0, 0, -INT_MAX, -INT_MAX};

            r[0] = test = Xdata->get(
                    size_t(Si)); //Seria reprezentowana przez Si zawiera indeksy obiektów, pobieramy współrzędne
            if(Xdata->is_missing(test))
                goto ERROR_HAPPENED;

            r[1] = test = Ydata->get(size_t(Si)); //Ta seria jest zawiera indeksy obiektów
            if(Ydata->is_missing(test))
                goto ERROR_HAPPENED;

            _rescale_data_point(r, a1); //Odziedziczone po scatter_plot
            assert(Ti == size_t(Ti));
            r[0] = test = Xdata->get(
                    size_t(Ti)); //Seria reprezentowana przez Ti zawiera indeksy obiektów, pobieramy współrzędne
            if(Xdata->is_missing(test))
                goto ERROR_HAPPENED;

            r[1] = test = Ydata->get(size_t(Ti)); //Ta seria jest zawiera indeksy obiektów
            if(Ydata->is_missing(test))
                goto ERROR_HAPPENED;

            _rescale_data_point(r, a2); //Odziedziczone po scatter_plot

            unsigned color = 255;
            if(AColor <= scale_ac.max && AColor >= scale_ac.min)
            {
                color = (unsigned) scale_ac.get(AColor) + c_range.start;
            }

            if(Present > 0)
            {
                if(Present < 1)
                    ASize = Present;
                else
                {   // `a1` i `a2` są typu `lang`, ale inicjalizacja wskazuje, że nie spodziewamy się wartości spoza zakresu `int`.
                    double D = toi( sqr(a1[0] - a2[0]) + sqr(a1[1] - a2[1]) );
                    if(D > 0)
                        D = sqrt(D);
                    else
                        D = 0;
                    if(SameSize < D / 2)
                        ASize = SameSize;
                    else
                        ASize = D / 2;
                }

                arrow(toi(tol(graph_core._x1()) + a1[0]), toi( tol(graph_core._y2()) - a1[1]),
                      toi(tol(graph_core._x1()) + a2[0]), toi( tol(graph_core._y2()) - a2[1]),
                      color,
                      ASize, 3.27);
            } else
            {
                line(toi(tol(graph_core._x1()) + a1[0]), toi(tol(graph_core._y2()) - a1[1]),
                     toi(tol(graph_core._x1()) + a2[0]), toi(tol(graph_core._y2()) - a2[1]),
                     color);
            }
        }
    } else
    {
        goto ERROR_HAPPENED;
    }

    return;
    ERROR_HAPPENED:
    print_width(this->get_start_x() + 1, this->get_start_y() + this->get_height() / 2,
                min_(int(string_width("Invalid connections data")), this->get_width()),
                128, get_background(), "%@CInvalid connections data");
}


void scatter_graph::fix_Y_minmax(double min, double max)
{
    scale_y.fix_min=scale_y.fix_max=0; //Odfiksuj
    if(min>-DBL_MAX)
    {
        scale_y.min=min;
        scale_y.fix_min=1;
    }
    if(max>-DBL_MAX)
    {
        scale_y.max=max;
        scale_y.fix_max=1;
    }
}

void scatter_graph::fix_X_minmax(double min, double max)
{
    scale_x.fix_min=scale_x.fix_max=0; //Odfiksuj!
    if(min>-DBL_MAX)
    {
        scale_x.min=min;
        scale_x.fix_min=1;
    }
    if(max>-DBL_MAX)
    {
        scale_x.max=max;
        scale_x.fix_max=1;
    }
}

#pragma clang diagnostic pop
}} // end-of-namespace sym2::visual

/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/              */
/*        MAIL:  wborkowski@uw.edu.pl                                 */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
