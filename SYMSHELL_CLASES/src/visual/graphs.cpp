///////////////////////////////////////////////////////////////////////////////////////////
//IMPLEMENTACJA PODSTAWOWYCH KLAS GRAFOW
///////////////////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

//#include "platform.hpp"

#include <cstdlib>
#include <cstdio> //sprintf!!!
#include <iostream>
#include <cassert>

#include "wb_ptr.hpp"
//#include "wbminmax.hpp"
#include "symshell.h"
#include "graphs.hpp"


//void rect(int x1,int y1,int x2,int y2,wb_color frame_c);

static char* trunc(char* bufor,unsigned width)
{
while(size_t(string_width(bufor))>width)//Symshell zle zwraca ??? tzn?
		{
		int size=strlen(bufor);
		if(size==1) break;
		bufor[size-2]='*';
		bufor[size-1]='\0';
		}
return bufor;
}


static inline int toi(const double& p)
{
return int(p);//Maskuje warningi z obcinania
}

template<class T>
static inline void swap(T& a,T& b)
{
T c=a;a=b;b=c;
}

template<class T>
static inline int _max(const T& a,const T& b)
{
return a>b?a:b;
}

template<class T>
static inline int _min(const T& a,const T& b)
{
return a<b?a:b;
}

static inline size_t _min(const size_t a,const size_t b)
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

int  gps_area::is_inside(xyinfo x,xyinfo  y) const
//return 1 if point is inside area, else return 0
{
return x1<=x && x<=x2 && y1<=y && y<=y2;
}

int  gps_area::translate(int& x,int& y)
//return 1 if point is inside area, and change  x,y relatively to area
{
if(x1<=x && x<=x2 && y1<=y && y<=y2)
	{
	x-=int(x1);y-=int(y1);
	return 1;
	}
return 0;
}

void gps_area::get_transform_to(const gps_area& t,float tab[4]) const
{
tab[0]=(t.x2+t.x1)/2-(x2+x1)/2;//px //bez float() zeby zostal choc 1 "warning"
tab[1]=float((t.y2+t.y1)/2-(y2+y1)/2);//py
tab[2]=float(double(t.x2-t.x1)/(x2-x1));//kx
tab[3]=float(double(t.y2-t.y1)/(y2-y1));//ky
tab[4]=float((t.x2+t.x1)/2);//x'0
tab[5]=float((t.y2+t.y1)/2);//y'0
}

void gps_area::transform(float tab[4])
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

int drawable_base::getstartx()
//Poczatek x obszaru uzytkowego
{
int ret=int(x1);
if(getframe()!=default_transparent && frame_width>0)
	ret+=frame_width;
return ret;
}

int drawable_base::getstarty()
//Poczatek y obszaru uzytkowego
{
int ret=int(y1);
if(getframe()!=default_transparent && frame_width>0)
	ret+=frame_width;
if(title && (titcol!=default_transparent || titbck!=default_transparent))
	{
	int r=string_height(title);
	assert(r>-1);
	ret+=r;
	}
return ret;
}

int drawable_base::getwidth ()
//Szerokosc obszaru uzytkowego
{
int ret=int(x2-x1);
ret++;
if(getframe()!=default_transparent && frame_width>0)
	ret-=2*frame_width;
return ret;
}

int drawable_base::getheight()
//Wysokosc obszaru uzytkowego
{
int ret=int(y2-y1);
ret++;
if(getframe()!=default_transparent && frame_width>0)
	ret-=2*frame_width;

if(title && (titcol!=default_transparent || titbck!=default_transparent))
	{
	int r=string_height(title);
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
//Czysci obszar
{
if(x1==x2 && y1==y2) return; //uspiony obszar o zerowym rozmiarze

fill_rect(int(x1),int(y1),int(x2+1),int(y2+1),::background());
if(need_flush)
		flush_plot();
}

//Rysuje ramke,moze tytul i wirtualnie zawartosc
void drawable_base::replot(int need_flush)
{
    if(x1==x2 && y1==y2) return; //uspiony obszar o zerowym rozmiarze

    int tx=int(x1);
    int ty=int(y1);
    int tw=int(x2-x1);
    int zostalo_dosc_y=(y2-y1)>char_height('X');//Czy jest miejsce na tytul i/lub reszte

    //Czyszczenie tłem lufcika jesli jest ustawione
    if(getbackground()!=default_transparent)
        fill_rect(toi(x1),toi(y1),toi(x2+1),toi(y2+1),getbackground());

    if(zostalo_dosc_y && title && (titcol!=default_transparent || titbck!=default_transparent))
    {
        int sw=0;//Current string width
        unsigned col1=(titcol!=default_transparent?titcol:getframe());
        unsigned col2=(titbck!=default_transparent?titbck:getbackground());
        if(col1==default_transparent)
            col1=default_black;

        char*  pom=clone_str(title);
        size_t len=strlen(pom);
        if(len==0)	goto REZYGNACJA;

        //skracanie
        while((sw=string_width(pom))>tw)
            {
            pom[len-1]='\0';
            len--;
            if(len==1) goto REZYGNACJA;
            }

        if(col1==col2)//Jesli takie same To tlo staje sie takie jak calosci obszaru
            if((col2=getbackground())==col1) //a jesli juz niestety bylo
                if((col1=default_black)==col2)//to text czarny, a jak juz byl
                    col1=default_white;//to bialy

        printc(tx+(tw/2-sw/2),ty,col1,col2,"%s",pom);

        // czy jest jeszcze miejsce na cokolwiek poza tytulem
        zostalo_dosc_y=(y2-y1)>char_height('X')*2+frame_width;//Na frame asekurancko

        REZYGNACJA:
        delete pom;
    }

    if(x2-x1>(2*frame_width) && zostalo_dosc_y)
    {
        //assert(this->CurrConfig!=nullptr);
        _replot(); //Nie odrysowuje idiotycznie wąskich obszarów (?)
    }

    if(getframe()!=default_transparent && frame_width>0)
    {
        tx+=frame_width;
        ty+=frame_width;
        tw-=2*frame_width;
        rect(toi(x1),toi(y1),toi(x2),toi(y2),getframe());
    }

    if(need_flush)
            flush_plot();

}

void drawable_base::_replot()
{
	assert("drawable_base::_replot(); should be reimplemented!"==0);
}

//graph implemenmtation
//----------------------------

int graph::settextcolors(wb_color start_i,wb_color end_i)
//Ustala kolor lub zbior kolorow dla tekstow
{
t_colors.start=start_i;
t_colors.end=end_i;
return 0;
}


int graph::setdatacolors(wb_color start_i,wb_color end_i)
//Ustala kolor lub zakres dla danych
{
c_range.start=start_i;
c_range.end=end_i;
if(end_i<=start_i)//Blad
	{
	c_range.end=c_range.start+1;
	return -1;
	}
return 0;
}



int graph::color_info::plot(int x1,int y1,int x2,int y2)
{
int ile_skali=end-start+2;
int skok=1;
int height=y2-y1;
while(ile_skali>height)
	{
	skok*=2;
	if((ile_skali/=2)==0)
		return -1;//Nie da sie
	}

int grubosc=height/ile_skali;
int kolejny=0;
//Petla po kolorach palety. Pola c_range sa unsigned!?
for(size_t i=start;i<end;i+=skok)
	{
	fill_rect(x1,y1+kolejny*grubosc,x2+1,y1+(kolejny+1)*grubosc,i);
	kolejny++;
	}

fill_rect(x1,y1+kolejny*grubosc,x2+1,y1+(kolejny+1)*grubosc,end);
return 0;
}

void graph::scaling_info::OYaxis(int x1,int y1,int x2,int y2,wb_color col,wb_color bcg)
{
assert(x1<x2 && y1<y2);
int width=x2-x1;
fill_rect(x1+width/2,y1,x1+width/2+1,y2+1,col);
line(x1+width/2,y1,x1,y1+width/2,col);
line(x1+width/2,y1,x1+width-1,y1+width/2,col);
int poz=0;
if(min<0 && max>0)
		{
		poz=int(y2-get(0));//Obcina do wspolrzednych ekranowych
		fill_rect(x1+width/2,poz,x1+width,poz+1,col);
		printc(x1+width,poz-char_height('o')/2,col,bcg,"o");
		}
if(min<1 && max>1)
		{
		poz=int(y2-get(1));//Obcina do wspolrzednych ekranowych
		fill_rect(x1+width/2,poz,x1+width,poz+1,col);
		//printc(x1+6,poz-char_height('1')/2,col,getbackground(),"1");
		}

if(max-min<=(y2-y1)/2)//miedzy min a max jest miejsce w pikselach
	for(int d=int(min+1);d<max;d++)
		{
		poz=int(y2-get(d));//Wiec tu double zmiesci sie w int
		if(poz<y2-int(y2-y1)-6) break; //zeby nie naszlo na grot strzalki
		fill_rect(x1+1,poz,x1+width/2,poz+1,col);
		}
}

void graph::scaling_info::OXaxis(int x1,int y1,int x2,int y2,wb_color col,wb_color bcg)
{
assert(x1<x2 && y1<y2);
int width=y2-y1;
fill_rect(x1,y2-3,x2,y2-3+1,col);
line(x2,y2-width/2,x2-6,y2,col);
line(x2,y2-width/2,x2-6,y2-width,col);

int poz=0;
if(min<0 && max>0)
	{
	poz=int(x1+get(0));//Ma sie miescic - patrz warunek
	fill_rect(poz,y2-width,poz+1,y2+1,col);
	printc(poz,y2-6-char_height('o')/2,col,bcg,"o");
	}

if(min<1 && max>1)
	{
	poz=int(x1+get(1));//Ma sie miescic - patrz warunek
	fill_rect(poz,y2-width,poz+1,y2+1,col);
	}

if(max-min<=(x2-x1)/2-6)
	for(int d=int(min+1);d<max;d++)
		{
		poz=int(x1+get(d));//Ma sie miescic - patrz warunek
		if(poz>x1+int(x2-x1)-6) break;
		fill_rect(poz,y2-width/2,poz+1,y2+1,col);
		}
}



//carpet_graph
//-------------------------------------------

//DESTRUCTOR
carpet_graph::~carpet_graph()
{
if(menage && data) delete data;
data=nullptr;
menage=0;
}

//CONSTRUCTOR(S)
carpet_graph::carpet_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
			 unsigned iA,unsigned iB,			 //A-ile kolumn,B-ile wierszy
			 data_source_base* idata,int imenage,//data-zrodlo danych o kolorach
			 bool idirect_color
			 ):
			direct_color(idirect_color),
			graph(ix1,iy1,ix2,iy2),
			AA(iA),BB(iB),data(idata),menage(imenage)
			{
            assert(data!=nullptr);
			assert(AA>=2 && BB>=2);
			}

carpet_graph::carpet_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
			 data_source_base* idata,int imenage,//data-zrodlo danych o kolorach
			 bool idirect_color):
			direct_color(idirect_color),
			graph(ix1,iy1,ix2,iy2),
			AA(1),BB(1),data(idata),menage(imenage)
			{
            assert(data!=nullptr);
			read_dim(AA,BB);
			}

// IMPLEMENTATION OF VIRTUAL METHODS oF carpet_graph
//-----------------------------------------------------
const geometry_base* carpet_graph::read_dim(size_t& aa,size_t& bb)
{
geometry_base* MyGeometry=data->getgeometry();
if( MyGeometry==nullptr ||
    MyGeometry->get_dimension()<2 )
	{
	//	A=subtab[1];
	//	B=subtab[3];
	if(!zastepnik)
		zastepnik=new rectangle_geometry(AA,BB,0);//Nie torus
	aa=AA;
	bb=BB;
	return zastepnik.get_ptr_val();
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


int carpet_graph::setseries(size_t index,data_source_base* idata,int imenage)
//zwraca -1 jesli indeks za duzy
{
if(index>0) return -1;//Tylko jedna seria
assert(idata!=nullptr);
if(menage) delete data;
data=idata;
menage=imenage;
return 0;
}

data_source_base* carpet_graph::getseries(size_t index)
//zwraca nullptr jesli indeks za duzy
{
if(index>0)
    return nullptr;
return data;
}

void carpet_graph::_replot()// Rysuje wlasciwy wykres a pod nim ewentualnie legende
{
unsigned CO_ILE_KOMOREK=1;
int x1=getstartx();
int y1=getstarty();
int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
int y2=y1+getheight()-1;

assert(x1<=x2);//Czy aby na pewno
assert(y1<=y2);//sensowne okno. Moze miec zerowy rozmiar ale nie ujemny

double min,max;
double missing;

size_t A=1,B=1;//Lokalne A i B
const geometry_base* MyGeometry=read_dim(A,B);

size_t NumberOfEl;
data->bounds(NumberOfEl,min,max);

if((A<=1 && B<=1)||(min>=max))
	{
	print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CInvalid data");
	return;
	}

//Wartosc zwracana gdy nie ma wartosci
missing=data->get_missing();

//Do skalowania kolorow
if(!direct_color)
	mm.set(min,max,c_range.end-c_range.start+0.999);

JESZCZE_RAZ_PRZELICZ:
//R y s o w a n i e  l e g e n d y
if(t_colors.start!=getbackground() &&
	 char_height('X')<getheight())
	{
	int x=x1;
	int y=y2-char_height('X')+1;//+1 bo y2 ma byc zarysowane
	int width=0;
//----------
	y2=y;//Zabiera dolna czesc na legende
//----------
	width=print_width(x,y,(x2-x)/3*2,t_colors.start,getbackground(),
					"%s",data->name());

	if(width==0) goto KWADRACIKI;
		else	 x+=width+1;

	width=print_width(x,y,(x2-x)/2,c_range.start,
						  c_range.start!=getbackground()?getbackground():c_range.end,
						  "<%g",min);

	if(width==0) goto KWADRACIKI;
		else	x+=width+1;

	width=print_width(x,y,x2-x,c_range.end,
						  c_range.end!=getbackground()?getbackground():c_range.start,
						",%g>",max);
	}

//Rysowanie skali - jesli sa co najmniej dwa kolory i jest miejsce na conajmniej 2 pixele
if(!direct_color)
 if(x2-x1>=_max(A,B)+6 && c_range.end>c_range.start && c_range.end-c_range.start>=2)
	{
	c_range.plot(x2-6,y1,x2,y2);
	x2-=7;
	}

//Rysowanie kwadracikow
KWADRACIKI:                                                             assert(x2>x1);
                                                                        assert(y2>y1);
while(!(A/CO_ILE_KOMOREK<=size_t(x2-x1+1) && B/CO_ILE_KOMOREK<=size_t(y2-y1+1)))
{
   CO_ILE_KOMOREK++;
   print_width(x1,y1,x2-x1,t_colors.start,getbackground(),"%@CView reduced %u times",CO_ILE_KOMOREK);
}

//RYSOWANIE
	{
                                                                        assert(c_range.end-c_range.start>=1);
	size_t i,j;//Indeksy po wierszach i kolumnach
	int width=x2-x1+1;//Juz moga byc inne
	int height=y2-y1+1;//Niz dla calego obszaru

    int gruboscA,gruboscB;
    if(CO_ILE_KOMOREK==1)
    {
        gruboscA=width/A;
        gruboscB=height/B;
    }
    else gruboscA=gruboscB=1;

	//Musi byc kwadratowo bo inaczej jest nieladnie
	if(gruboscA>1 && gruboscB>1)
		{
		if(gruboscA>gruboscB) gruboscA=gruboscB;
				else		  gruboscB=gruboscA;
		}
		else
		{
        gruboscA=gruboscB=1;//Pixelami będzie !!!
		}

    int offsetA=(width-gruboscA*(A/CO_ILE_KOMOREK))/2;
    int offsetB=(height-gruboscB*(B/CO_ILE_KOMOREK))/2;

	//Rysowanie
    const rectangle_geometry* MyGeomRect=dynamic_cast<const rectangle_geometry*>(MyGeometry);           assert(MyGeomRect!=nullptr);//Musia taka być!
//    long index=MyGeomRect->get(0,0);//Zerowa komórka

    data_source_base::iteratorh h=MyGeometry->make_view_iterator();                                     assert(h!=nullptr);
	wb_color back=getbackground();//Dla sprawdzania kiedy kolor kwadratu taki jak kolor tla.
	if(gruboscA==1)//starczy jedna sprawdzic bo kwadrat
		{
		//Pixelami panowie!!!
        for(j=0;j<B;j+=CO_ILE_KOMOREK)
          for(i=0;i<A;i+=CO_ILE_KOMOREK)
            {
                size_t Gind = 0;
                if(CO_ILE_KOMOREK==1) Gind=MyGeometry->get_next(h);
                else
                if(i<A && j<B)
                    Gind=MyGeomRect->get(i,j);//Nie zadziała dla zmniejszonego view
                    else
                    continue;//Pomijamy go.

                double test = data->get(Gind);

                if (test == missing)
                            continue; // Nie rysowac jesli wartosc nieosiagalna

                if (!direct_color)
                {
                            wb_color color = wb_color(mm.get(test));
                            color += c_range.start;
                            if (color <= c_range.end)
                                plot(offsetA + x1 + i/CO_ILE_KOMOREK, offsetB + y1 + j/CO_ILE_KOMOREK, color);
                            else
                                goto NIE_DA_SIE;
                }
                else
                {
                            unsigned C=test;//Zakładamy że to surowe kolory?
                            unsigned R=(C & 0x0000ff);
                            unsigned G=(C & 0x00ff00)>>8;
                            unsigned B=(C & 0xff0000)>>16;
                            // color+=c_range.start;
                            plot_rgb(offsetA+ x1 +i/CO_ILE_KOMOREK,offsetB+ y1 +j/CO_ILE_KOMOREK,R,G,B);
                }
			}
		}
		else
		{
		//Kwadratami
		for(j=0;j<B;j++)
		  for(i=0;i<A;i++)
          {                                     								assert(h!=nullptr);
              size_t Gind=MyGeometry->get_next(h);//rectangle_geometry
              double test=data->get(Gind);
              if(test==missing)
                  continue;//Nie rysowac jesli wartosc nieosiagalna
			  if (!direct_color)
			  {
				wb_color color=wb_color( mm.get(test) );
				color+=c_range.start;

				if(!(color<=c_range.end))
					goto NIE_DA_SIE;

				if(color==back && gruboscA>3)
				{
				  rect(offsetA+x1+i*gruboscA,offsetB+y1+j*gruboscB,
					  offsetA+x1+(i+1)*gruboscA-1,offsetB+y1+(j+1)*gruboscB-1,
					  255!=back?255:0);
				}
				else
				{
				  fill_rect(offsetA+x1+i*gruboscA,offsetB+y1+j*gruboscB,
					  offsetA+x1+(i+1)*gruboscA,offsetB+y1+(j+1)*gruboscB,
					  color);
				}
			  }
			  else //Jeżeli direct_color
			  {
				unsigned C=test;//Zakładamy że to surowe kolory?
				unsigned R=(C & 0x0000ff);
				unsigned G=(C & 0x00ff00)>>8;
				unsigned B=(C & 0xff0000)>>16;
				set_pen_rgb(R,G,B,1,SSH_SOLID_PUT);
				set_brush_rgb(R,G,B);
				fill_rect_d(offsetA+x1+i*gruboscA,offsetB+y1+j*gruboscB,
					  offsetA+x1+(i+1)*gruboscA,offsetB+y1+(j+1)*gruboscB
					  );
              }
		  }
		}
	MyGeometry->destroy_iterator(h);
	}

return;
NIE_DA_SIE:
print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CInvalid colors found.");
print_width(x1,(y1+y2)/2+char_height('X'),x2-x1,t_colors.start,getbackground(),"%@CProbably min or max not properly set.");
}

// reals[] zawiera jedynie wartosc ustalajaca kolor, albo cos spoza zakresu
int carpet_graph::_rescale_data_point(const double reals[],long in_area[])
//zwraca -1 jesli nie w oknie
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
d_menage=0;
if(colors && c_menage) delete colors;
colors=nullptr;
c_menage=0;
}

//CONSTRUCTOR(S)
bars_graph::bars_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
			 data_source_base* idatas,int menage_d,//datas-dane o wysokosciach
			 data_source_base* icolors,int menage_c,//colors-zrodlo danych o kolorach
			 int zero_mod): //tryb wyswietlania
			graph(ix1,iy1,ix2,iy2),
			datas(idatas),d_menage(menage_d),
			colors(icolors),c_menage(menage_c),
			mode(zero_mod)
			{
			setdatacolors(128,129);
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
	return 0;
	else
	{
	mode=old;
	return -1;
	}
}

int bars_graph::setseries(size_t index,data_source_base* idata,int imenage)
//zwraca -1 jesli indeks za duzy
{
if(index>1) return -1;//Tylko dwie serie
assert(idata!=nullptr);
if(index==0)
	{
	if(d_menage) delete datas;
	datas=idata;
	d_menage=imenage;
	}
	else
	{
	if(c_menage) delete colors;
	colors=idata;
	c_menage=imenage;
	}
return 0;
}

data_source_base* bars_graph::getseries(size_t index)
//zwraca nullptr jesli indeks za duzy
{
if(index>0)
    return nullptr;
if(index==0)
	return datas;
	else
	return colors;
}

void bars_graph::_replot()// Rysuje wlasciwy wykres a pod nim ewentualnie legende
{
int x1=getstartx();
int y1=getstarty();
int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
int y2=y1+getheight()-1;
double min,max,minc,maxc;
double miss,missc;
size_t num_height,num_color,height;
int flaga=0;

//Legenda wtedy jesli jest potrzebna
if(t_colors.start!=getbackground() || (t_colors.end!=getbackground() && colors))
	height=(y2-y1)-2*char_height('0');//bedzie legenda
	else
	height=y2-y1;//NIe bedzie legendy

//Do skalowania slupkow
datas->bounds(num_height,min,max);

miss=datas->get_missing();
if(mode==1)
	if(min>0) min=0;// Slupki conajmniej od zera
s_data.set(min,max,height+0.999);

//Do skalowania kolorow
if(colors!=nullptr)
	{//Jesli jest seria
	colors->bounds(num_color,minc,maxc);
	assert(num_height<=num_color);
	s_colo.set(minc,maxc,c_range.end-c_range.start+0.999);
	missc=colors->get_missing();
	}
	else
	{//Nie ma serii - kolory arbitralne
	s_colo.set(0,num_height-1,c_range.end-c_range.start+0.9999);
	}

//Wypisywanie legendy dla 1 serii
if(t_colors.start!=getbackground())
	{
	const char* pom=datas->name();
	print_width(x1,y1,(x2-x1)/2,t_colors.start,getbackground(),
				"%g",max);
	print_width(x1,y2-char_height('0'),(x2-x1)/3*2,t_colors.start,getbackground(),
				"%g %*s",min,strlen(pom)>60?60:strlen(pom),pom);
	flaga=1;
	}

//...i dla drugiej
if(t_colors.end!=getbackground() && colors!=nullptr)
	{
	int width=print_width(x2-(x2-x1)/2,y1,(x2-x1)/2,
		c_range.start,c_range.start!=getbackground()?getbackground():c_range.end,
				"%@R%g",minc);

	const char* pom=colors->name();
	print_width((x2+x1)/2,y1,(x2-x1)/2-width,getbackground(),c_range.start,
		"%@R%*s",strlen(pom)>60?60:strlen(pom),pom);

	print_width(x2-(x2-x1)/3,y2-char_height('0'),(x2-x1)/3,
		c_range.end,c_range.end!=getbackground()?getbackground():c_range.start,
				"%@R%g",maxc);

	flaga=1;
	}

//Zawerzenie po y-grekach
if(flaga)
	{
	y1+=char_height('0');
	y2-=char_height('0');
	}
flaga=0;

//Strzalka w gore
if(x2-x1>10 && y2-y1>10)
	{
	s_data.OYaxis(x1,y1,x1+6,y2,t_colors.start,getbackground());
	x1+=6;
	}

//Rysowanie skali - jesli jest seria kolorów - tzn kolry sa wymuszone
//sa conajmniej dwa kolory skali i jest miejsce na conajmniej 5 pixeli
if(colors!=nullptr && x2-x1>=10 && c_range.end>c_range.start && c_range.end-c_range.start>=1)
	{
	c_range.plot(x2-5,y1,x2,y2);
	x2-=5;
	}
//NIE_DA_SIE:;

//Linia zerowa
int zero=y2;
if(min<0 && max>0 )
	{
	zero=int(y2-s_data.get(0)); //Musi byc miedzy y1 a y2
	fill_rect(x1,zero,x2+1,zero+1,t_colors.start);
	}

//Rysowanie wlasciwych slupkow
int grubosc;//Grubosc slupka
if(num_height>0 && (grubosc=(x2-x1)/(num_height))>=1)
	{
	data_source_base::iteratorh h=datas->reset();
    data_source_base::iteratorh c=colors?colors->reset():nullptr;
	for(size_t i=0;i<num_height;i++)
		{
		double		test;
		double		r[2]={-1,-1};
        long        a[2]={0,(c_range.end-c_range.start)/2};
		r[0]=test=datas->get(h);
		if(test==miss)
				continue;//NIe rysuj

		if(colors)
			{
			r[1]=test=colors->get(c);
			if(test==missc)
				continue;//NIe rysuj
			}
			else
			r[1]=i;//Arbitralny przydzial kolejnych kolorów

		_rescale_data_point(r,a);

		int yup=y2-a[0];
		int ydow=zero;
		if(yup>ydow)
			{
			::swap(yup,ydow);
			//yup++;//Male oszustwo zeby tez sie zaczynalo od zerowej lini.
			}

		assert(yup<=ydow);
		if(wb_color(a[1])!=getbackground())
			fill_rect(x1+i*grubosc,yup,
				  x1+(i+1)*grubosc,ydow+1,
					a[1]);//Puki kolory indeksowe to pol biedy
			else
			rect(x1+i*grubosc,yup,
				  x1+(i+1)*grubosc-1,ydow,
					255!=getbackground()?255:0);
		}
	datas->close(h);
	if(colors) colors->close(c);
	}
	else
	{
	print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CInvalid data or too small area");
	}
}

// reals[] zawiera  wysokosc slupka i wartosc ustalajaca kolor, albo -1
int bars_graph::_rescale_data_point(const double reals[],long in_area[])
//zwraca -1 jesli nie w oknie
{
int flaga=0;

if(reals[0]<=s_data.max && reals[0]>=s_data.min)
	{
	unsigned height=(unsigned)s_data.get(reals[0]);
	in_area[0]=height;
	}
	else flaga=1;

if(reals[1]<=s_colo.max && reals[1]>=s_colo.min)
	{
	unsigned color=(unsigned)s_colo.get(reals[1]);
	in_area[1]=color+c_range.start;
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
datas=nullptr;
d_menage=0;
if(colors && c_menage) delete colors;
colors=nullptr;
c_menage=0;
}

//CONSTRUCTOR(S)
manhattan_graph::manhattan_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
			 unsigned iA,unsigned iB,
			 data_source_base* idatas,int menage_d,//datas-dane o wysokosciach
			 data_source_base* icolors,int menage_c,//colors-zrodlo danych o kolorach
			 int zero_mod, //tryb wyswietlania
			 double iHoffs,		//Ulamek szerokosci przeznaczony na perspektywe
			 double	iVoffs		//Ulamek wysokosci  przeznaczony na perspektywe
			 ):
		graph(ix1,iy1,ix2,iy2),AA(iA),BB(iB),
		datas(idatas),d_menage(menage_d),
		colors(icolors),c_menage(menage_c),
		mode(zero_mod),c_offset(0),wire(getbackground()),
		Hoffs(iHoffs),Voffs(iVoffs)
			{
            assert(datas!=nullptr);
			assert(AA!=0);
			assert(BB!=0);
			assert(Voffs>0 && Voffs<1);
			assert(Hoffs>0 && Hoffs<1);
			}

manhattan_graph::manhattan_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
			 data_source_base* idatas,int menage_d,//datas-dane o wysokosciach
			 data_source_base* icolors,int menage_c,//colors-zrodlo danych o kolorach
			 int zero_mod, //tryb wyswietlania
			 double iHoffs,		//Ulamek szerokosci przeznaczony na perspektywe
			 double	iVoffs		//Ulamek wysokosci  przeznaczony na perspektywe
			 ):
		graph(ix1,iy1,ix2,iy2),AA(1),BB(1),
		datas(idatas),d_menage(menage_d),
		colors(icolors),c_menage(menage_c),
		mode(zero_mod),c_offset(0),wire(getbackground()),
		Hoffs(iHoffs),Voffs(iVoffs)
			{
            assert(datas!=nullptr);
			assert(AA!=0);
			assert(BB!=0);
			assert(Voffs>0 && Voffs<1);
			assert(Hoffs>0 && Hoffs<1);
			}

// IMPLEMENTATION OF VIRTUAL METHODS oF manhattan_graph
//-----------------------------------------------------
const geometry_base* manhattan_graph::read_dim(size_t& aa,size_t& bb)
{
geometry_base* MyGeometry=datas->getgeometry();
if( MyGeometry==nullptr ||
    MyGeometry->get_dimension()<2 )
	{
	//	A=subtab[1];
	//	B=subtab[3];
	if(!zastepnik)
		zastepnik=new rectangle_geometry(AA,BB,0);//Nie torus
	aa=AA;
	bb=BB;
	return zastepnik.get_ptr_val();
	}
	else
	{
	//Sprawdzenie czy serie sie nie zgadzaja - czy maja ta sama geometrie
    if(colors!=nullptr)
		{
		geometry_base* ColGeom=colors->getgeometry();
		if(*MyGeometry!=*ColGeom)
			{//jesli nie to pozbywany sie koloru
			if(c_menage) delete colors;
            colors=nullptr;
			}
		}
	geometry::view_info pom;
	MyGeometry->get_view_info(&pom);
	aa=size_t(pom.dia.X()*2);
	bb=size_t(pom.dia.Y()*2);
	return MyGeometry;
	}

}


int manhattan_graph::configure(const void* p)
{
const config* conf=((const config*)p);
if(p==nullptr)
	{
	mode=1;
	c_offset=0;
	wire=getbackground();
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


int manhattan_graph::setseries(size_t index,data_source_base* idata,int imenage)
//zwraca -1 jesli indeks za duzy
{
if(index>1) return -1;//Tylko dwie serie
assert(idata!=nullptr);
if(index==0)
	{
	if(d_menage) delete datas;
	datas=idata;
	d_menage=imenage;
	}
	else
	{
	if(c_menage) delete colors;
	colors=idata;
	c_menage=imenage;
	}
return 0;
}

data_source_base* manhattan_graph::getseries(size_t index)
//zwraca nullptr jesli indeks za duzy
{
if(index>0)
    return nullptr;
if(index==0)
	return datas;
	else
	return colors;
}


void manhattan_graph::_replot()// Rysuje wlasciwy wykres a pod nim ewentualnie legende
{
int x1=getstartx();
int y1=getstarty();
int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
int y2=y1+getheight()-1;
double min,max,minc,maxc;
double miss,missc;
size_t antywidth,A,B;
size_t height=y2-y1;
size_t width=x2-x1;
int flaga=0;

//Trzeba sprawdzic wymiary obszaru wizulaizacji
const geometry_base* MyGeometry=read_dim(A,B);

//Legenda wtedy jesli jest potrzebna
if(t_colors.start!=getbackground() || (t_colors.end!=getbackground() && colors))
	height-=2*char_height('0');//bedzie legenda

//Strzalka jesli jest potrzebna
if(t_colors.start!=getbackground())
		width-=3;//Na strzalko-ramke

//Skala jesli jest potrzebna
if(t_colors.end!=getbackground() && colors!=nullptr)
		width-=5;

//Danina wysokosci i szerokosci na perspektywe
antywidth=size_t(width*Hoffs);
width=size_t(width*(1-Hoffs));
height=size_t(height*(1-Voffs));

//Danina na podzielnosc przez A i B
if(A<=1 || B<=1)//Nie ma danych
	{
	print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CInvalid data");
	return;
	}

antywidth+=width%A;//Z szerokosci cos wpada do antyszerokosci
width-=width%A;//W szerokosci musi sie miescic A kolumn

//juz wiadomo,jesli sie nie zmiesci
if(width==0 || antywidth/B*B==0)//W antyszserokosci musi bys conajmniej po 1 pixel na wiersz
	{
	print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CTo small area for %ux%u graph",A,B);
	return;
	}

//min,max do skalowania slupkow
{
size_t num_height;
datas->bounds(num_height,min,max);
}

miss=datas->get_missing();
if(mode==1)
	if(min>0) min=0;// Slupki conajmniej od zera
s_data.set(min,max,height+0.999);

//Do skalowania kolorow, jesli jest seria
if(colors!=nullptr)
	{
	size_t num_color;
	colors->bounds(num_color,minc,maxc);
	s_colo.set(minc,maxc,c_range.end-c_range.start+0.999);
	missc =colors->get_missing();
	}

//Wypisywanie legendy dla 1 serii
if(t_colors.start!=getbackground())
	{
	print_width(x1,y1,((x2-x1)/5)*4,t_colors.start,getbackground(),"%g",max);
	const char* pom=datas->name();
	print_width(x1,y2-char_height('0'),((x2-x1)/5)*4,t_colors.start,getbackground(),
			"%g %*s",min,strlen(pom)>60?60:strlen(pom),pom);
	flaga=1;
	}

//...i dla drugiej
if(t_colors.end!=getbackground() && (colors!=nullptr) )
	{
	int width=print_width(x2-(((x2-x1)/3)*2),y1,((x2-x1)/3)*2,
		c_range.start,c_range.start!=getbackground()?getbackground():c_range.end,
				"%@R%g",minc);

	const char* pom=colors->name();
	print_width(x1+(x2-x1)/5,y1,((x2-x1)/5*4)-width,getbackground(),c_range.start,
		"%@R%*s",strlen(pom)>60?60:strlen(pom),pom);

	print_width(x2-(x2-x1)/2,y2-char_height('0'),(x2-x1)/2,
		c_range.end,c_range.end!=getbackground()?getbackground():c_range.start,
				"%@R%g",maxc);
	flaga=1;
	}


//Zawerzenie po y-grekach
if(flaga)
	{
	y1+=char_height('0');
	y2-=char_height('0');
	}
flaga=0;

//SKIP:
//Rysowanie skali -jesli sa conajmniej dwa kolory i jest miejsce na conajmniej 2 pixele
if(colors!=nullptr && x2-x1>=10 && c_range.end>c_range.start && c_range.end-c_range.start>=1)
	{
	c_range.plot(x2-5,y1,x2,y2);
	x2-=5;
	}
//NIE_DA_SIE:;

//Ramka dla Y-kow
if(x2-x1>10 && y2-y1>10)
	{
	int awidth=antywidth/B*B;
	int oddolu=(y2-y1-height)/B*B; //Ile tylna os jest podsunieta do gory
	line(x1+3,y2-height,x1+3+awidth,y2-height-oddolu,t_colors.start);
	line(x1+3,y2,x1+3+awidth,y2-oddolu,t_colors.start);
	s_data.OYaxis(x1,y2-height,x1+6,y2,t_colors.start,getbackground());
	s_data.OYaxis(x1+awidth,y2-height-oddolu,x1+6+awidth,y2-oddolu,t_colors.start,getbackground());
	x1+=3;
	}

//Rysowanie wlasciwych slupkow
int zero=y2;
if(min<0 && max>0)
		{
		zero=int(y2-s_data.get(0));//Ma sie miescic - patrz warunek
		//fill_rect(x1,zero,x2+1,zero+1,t_colors.start);
		}

int gruboscH=width/A;//Grubosc slupka w poziomie
int gruboscV=(y2-y1-height)/B; //Grubosc slupka w pionie
int offsetH=(x2-x1-width)/B;//Przesuniecie kolejnych wierszy

if(gruboscH>=1 && gruboscV>=1 && offsetH>=1 )
	{
	settings_bar3d conf(gruboscH,offsetH,
						gruboscV,wire!=getbackground()?wire:0,getbackground());
	bar3d_config(&conf);

	data_source_base::iteratorh h=MyGeometry->make_view_iterator();
    geometry_base* color_geom=nullptr;
    data_source_base::iteratorh c=nullptr;

	if(colors)//Jesli jest kolor to
		{
		color_geom=colors->getgeometry();//Wez z niego geometrie
		if(color_geom)					//Jesli jest geometria to stwórz iterator
			c=color_geom->make_view_iterator();
		}

	for(unsigned Bpos=0;Bpos<B;Bpos++)//W ktorym wierszu
	  for(unsigned Apos=0;Apos<A;Apos++)//W ktorej kolumnie
		{
		double test;
		int X,Y,Y2;		  //Robocze pozycje
		double r[2]={-1,-1};
        long   a[2]={0,(c_range.end-c_range.start)/2};
		size_t hi=MyGeometry->get_next(h);//czyta index
		r[0]=test=datas->get(hi);//i czyta wartosc
		if(test==miss)//Jesli missing
			{//Wywal odpowiadajaca wartosc dla color
			if(color_geom)
				color_geom->get_next(c);
			continue; //Nie rysuj
			}
			 //s_data.min;
		if(color_geom)
			{
			size_t ci=color_geom->get_next(c);//czyta index
			r[1]=test=colors->get(ci);//i czyta wartosc
			if(test==missc)
					continue; //Nie rysuj
			}

		_rescale_data_point(r,a);

		X=x2-(Bpos+1)*offsetH-(A-Apos)*gruboscH;
		Y=zero-(B-(1+Bpos))*gruboscV;
		Y2=y2-(B-(1+Bpos))*gruboscV-a[0];//Wysokosc od min

		if(Y2>Y)
			::swap(Y,Y2);

		//Slupek 3D
		bar3d(X,Y,Y-Y2,a[1],a[1]+c_offset);
		}
	MyGeometry->destroy_iterator(h);
	if(color_geom) color_geom->destroy_iterator(c);
	}
	else
	{
	printc(x1,(y1+y2)/2,t_colors.start,getbackground(),"Too small area");
	}

}

// reals[] zawiera  wysokosc slupka i wartosc ustalajaca kolor, albo -1
int manhattan_graph::_rescale_data_point(const double reals[],long in_area[])
//zwraca -1 jesli nie w oknie
{
int flaga=0;

if(reals[0]<=s_data.max && reals[0]>=s_data.min)
	{
	unsigned height=(unsigned)s_data.get(reals[0]);
	in_area[0]=height;
	}
	else flaga=1;

if(colors)
  if(reals[1]<=s_colo.max && reals[1]>=s_colo.min)
	{
	unsigned color=(unsigned)s_colo.get(reals[1]);
	in_area[1]=color+c_range.start;
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
if(series!=nullptr)
	for(size_t i=0;i<M;i++)
        if(series[i].menage && series[i].ptr!=nullptr)
				delete series[i].ptr;
}

//CONSTRUCTOR(S) for  series_info* & for data_source_base*
sequence_graph::sequence_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
				int N,			//Ilosc serii
				series_info* iseries,
				int imode,	//0 - tryb z pelnym reskalowaniem
							//1 - tryb ze wspolnym minimum,maximum,pobieranym z parametrow i serii
							//2 - tryb z zafiksowanym minimum i maksimum
			   double imin,double imax
				):  //Tablica struktur zawierajacych informacje o seriach
                graph(ix1,iy1,ix2,iy2),M(0),
                series(nullptr),scales(nullptr),mode(imode),scale_y(imin,imax)
{
                                                            assert(scale_y.min<=scale_y.max);
    wb_ptr<series_info> se( new series_info[N]);
    wb_ptr<scaling_info> sc( new scaling_info[N]);
    series=se.give();                                       assert(series!=nullptr);
    scales=sc.give();                                       assert(scales!=nullptr);

    if(series!=nullptr && scales!=nullptr)
        M=N;//Juz jest tyle miejsca

    if(iseries!=nullptr)
    {
        for(size_t i=0;i<M;i++)
        {
            if(iseries[i].ptr==nullptr)
                break;
            series[i]=iseries[i];
        }
    }
}

sequence_graph::sequence_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
				int N,			//Ilosc serii
				data_source_base** iseries,
				int imode,	//0 - tryb z pelnym reskalowaniem
							//1 - tryb ze wspolnym minimum,maximum,pobieranym z parametrow i serii
							//2 - tryb z zafiksowanym minimum i maksimum
			   double imin,double imax
				):  //Tablica struktur zawierajacych informacje o seriach
                graph(ix1,iy1,ix2,iy2),M(0),
                series(nullptr),scales(nullptr),mode(imode),scale_y(imin,imax)
{
                                                            assert(scale_y.min<=scale_y.max);
    wb_ptr<series_info> se( new series_info[N]);
    wb_ptr<scaling_info> sc( new scaling_info[N]);
    series=se.give();                                       assert(series!=nullptr);
    scales=sc.give();                                       assert(scales!=nullptr);

    if(series!=nullptr && scales!=nullptr)
        M=N;//Juz jest tyle miejsca

    if(iseries!=nullptr)
    {
        for(size_t i=0;i<M;i++)
        {
            if(iseries[i]==nullptr)
                break;
            series[i].ptr=iseries[i];//Reszta dumyslna
        }
    }
}

// IMPLEMENTATION OF VIRTUAL METHODS
int sequence_graph::configure(const void* config)
//Parametr typu config_seq. Jesli nullptr to tryb 0;
{
config_seq* c=(config_seq*)config;
if(c!=nullptr)
	{
	mode=c->mode;
	scale_y.set(c->min,c->max,1);
	}
	else mode=0;
return 0;
}

int sequence_graph::setseries(size_t index,data_source_base* data,int menage)
//zwraca -1 jesli indeks za duzy
{
assert(series!=nullptr);
if(index>=M)
	return -1;
if(series[index].menage && series[index].ptr!=nullptr)
			delete series[index].ptr;
series[index].ptr=data;
series[index].menage=menage;
return 0;
}

data_source_base* sequence_graph::getseries(size_t index)
//zwraca nullptr jesli indeks za duzy
{
assert(series!=nullptr);
if(index>=M)
    return nullptr;
return series[index].ptr;
}

// reals[] zawiera wysokosc i wartosc ustalajaca kolor
int sequence_graph::_rescale_data_point(const double reals[],long in_area[])
//zwraca -1 jesli nie w oknie
{
int flaga=0;

if(mode==0)
	{
	for(unsigned i=0;i<M;i++)
	  if(reals[i]<=scales[i].max && reals[i]>=scales[i].min)
		{
		unsigned pom=(unsigned)scales[i].get(reals[i]);
		in_area[i]=pom;
		}
		else flaga=1;
	}
	else
	{
	for(unsigned i=0;i<M;i++)
	  if(reals[i]<=scale_y.max && reals[i]>=scale_y.min)
		{
		unsigned pom=(unsigned)scale_y.get(reals[i]);
		in_area[i]=pom;
		}
		else
		{
		flaga=1;
		if(reals[i]>scale_y.max)
					in_area[i]=INT_MAX;
		if(reals[i]<scale_y.min)
					in_area[i]=INT_MIN;
		}
	}

if(reals[M]<=scale_x.max && reals[M]>=scale_x.min)
	{
	unsigned pom=(unsigned)scale_x.get(reals[M]);
	in_area[M]=pom;
	}
	else flaga=1;

if(flaga)
	return -1;
	else
	return 0;
}

inline unsigned sequence_graph::color(unsigned ind)
{
assert(ind<M);
if(ind>=M) return unsigned(-1);//Blad
if(series[ind].color!=-1)
	return series[ind].color;
	else
	return unsigned(c_range.start+scolors.get(ind));
}


void sequence_graph::_replot()
// Rysuje wlasciwy wykres a pod nim ewentualnie legende
{
unsigned	i,MaxNum=0;
int x1=getstartx();
int y1=getstarty();
int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
int y2=y1+getheight()-1;
int height=y2-y1;
int width=x2-x1;
assert(M>=1);

wb_dynarray<data_source_base::iteratorh> 	Hh(M);
wb_dynarray<double>					Rh(M+1);
wb_dynarray<long>					Ah(M+1);
wb_dynarray<long>					Bh(M+1);
wb_dynarray<unsigned>				Ch(M);
wb_dynarray<unsigned>				Nh(M);
wb_dynarray<char>					Oh(M);
wb_dynarray<double>     			Missing(M+1);

data_source_base::iteratorh* H=Hh.get_ptr_val();
double*   Miss=Missing.get_ptr_val();//Missing values
double*	  R=Rh.get_ptr_val();//Input dla _rescale_data_point
long*	  A=Ah.get_ptr_val();//Output dla _rescale_data_point
long*	  B=Bh.get_ptr_val();//--------//--------
unsigned* C=Ch.get_ptr_val();//Kolory
unsigned* Num=Nh.get_ptr_val();//Iteratory
char*	  OK=Oh.get_ptr_val(); //Flagi zakresowe dla trybu 2

assert(H!=nullptr);
assert(R!=nullptr);
assert(A!=nullptr);
assert(B!=nullptr);
assert(Num!=nullptr);


if(t_colors.start!=getbackground())//Bedzie legenda
		{
		height-=3*char_height('0');
		width-=4;
		}

if(mode==2)//Konfiguracja min max dla osi Y
	{
	//assert(scale_y.min<scale_y.max);
	if(scale_y.min>=scale_y.max)
		{//Awaria - zle parametry
		print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CInvalid min,max range:<%g,%g>",scale_y.min,scale_y.max);
		return ;
		}
	}
	else
	{
	if(scale_y.min>scale_y.max)
		{//Awaria - zle parametry
		print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CInvalid min,max range:<%g,%g>",scale_y.min,scale_y.max);
		return ;
		}
	if(scale_y.min==scale_y.max)//Np default ==0
		{//Do ustalenia w czasie przeszukiwania serii
		scale_y.min=DBL_MAX;
		scale_y.max=-DBL_MAX;
		}
	}

for(i=0;i<M;i++)
  {
  if(series[i].ptr!=nullptr)
	{
	double min,max;
	size_t num;
	series[i].ptr->bounds(num,min,max);
	scales[i].set(min,max,height);
	Miss[i]=series[i].ptr->get_missing();
	Num[i]=num;
	if(num>MaxNum)
			MaxNum=num;
	if(mode!=2)
		{
		if(scale_y.min>min)
			scale_y.min=min;
		if(scale_y.max<max)
			scale_y.max=max;
		}
	}
  }

if(MaxNum==0)//Brak danych
	{
	print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CInvalid data(s)");
	return;
	}

//Dodatkowe skalowania
scale_y.set(scale_y.min,scale_y.max,height);//Dla osi Y w trybie 1 i 2
scale_x.set(0,MaxNum,width);//Dla osi X
scolors.set(0,M,c_range.end-c_range.start+1);//Dla kolorow serii

if(t_colors.start!=default_transparent && t_colors.start!=getbackground())//Rysujemy legende
	{
	char bufor[80];
	int grubosc=(x2-x1)/M;
	if(mode==0 && grubosc>1)
	  {
	  for(i=0;i<M;i++)
        if(series[i].ptr!=nullptr)
		{
		sprintf(bufor,"%g ",scales[i].max);
		printc(x1+i*grubosc,y1,color(i),getbackground(),"%s",trunc(bufor,grubosc));
		}
      for(i=0;i<M;i++)
        if(series[i].ptr!=nullptr)
		{
		sprintf(bufor,"%g ",scales[i].min);
		printc(x1+i*grubosc,y2-2*char_height('0'),color(i),getbackground(),"%s",trunc(bufor,grubosc));
		}
	  }
	  else //mode>0
	  {
	  sprintf(bufor,"%g ",scale_y.max);
	  printc(x1,y1,t_colors.start,getbackground(),"%s",trunc(bufor,width));
	  sprintf(bufor,"%g ",scale_y.min);
	  printc(x1,y2-2*char_height('0'),t_colors.start,getbackground(),"%s",trunc(bufor,width));
	  }

	if(grubosc>1)
	  for(i=0;i<M;i++)
        if(series[i].ptr!=nullptr)
		{
		const char* pom=series[i].ptr->name();
        int len=strlen(pom);
        sprintf(bufor,"%*s;",len>60?60:len,pom);
		printc(x1+i*grubosc,y2-char_height('0'),color(i),getbackground(),"%s",trunc(bufor,grubosc));
		}
	 y1+=char_height('0');
	 y2-=2*char_height('0');

	//Rysowanie strzalki
	if(x2-x1>10 && y2-y1>10)
	 {
	 if(mode>0)
		{
		scale_y.OYaxis(x1,y1,x1+6,y2,t_colors.start,getbackground());
		}
		else
		{
		scaling_info pom(DBL_MAX/2.0,DBL_MAX,INT_MAX);//Zeby nie bylo zadnej skali ani 0
		pom.OYaxis(x1,y1,x1+6,y2,t_colors.start,getbackground());
		}
	 x1+=4;
	 }

}//KONIEC LEGENDY

//WYPISANIE LICZBY PROBEK
if(t_colors.end!=default_transparent && t_colors.end!=getbackground())
	{
	print_width(x2-(x2-x1)/3,y2-char_height('0'),(x2-x1)/3,getbackground(),t_colors.end,
				"%@R%u",MaxNum);
	}

//Rysowanie punktow lub linii
//...
if(x2-x1>10 && y2-y1>5) //Musi byc troszke miejsca i troche danych
  {
  for(i=0;i<M;i++)//Inicjowanie tablic
	{
    if(series[i].ptr!=nullptr)
		{
		H[i]=series[i].ptr->reset();
		C[i]=color(i);
		}
		else C[i]=default_color;
	R[i]=scales[i].max;//Takze w R cos na poczatek zeby nie bylo przypadkowo
	}


  for(unsigned j=0;j<MaxNum;j++)// Iteracja po krokach
	{
    double test;
    long*  pom=(j%2==0?A:B);//Wybor aktualnej tablicy wynikow
    long*  old=(j%2==1?A:B);//Wybor starej tablicy wynikow
	R[M]=j;

	for(i=0;i<M;i++) //Ladowanie. Petla po seriach
		if(C[i]!=-1) //Seria aktywna
			{
			OK[i]=1;//Z zaufaniem ze jest dobrze
			if(j>=Num[i])
				{
				C[i]=default_color;//Zakonczono rysowanie tej seri
				continue;
				}

			test=series[i].ptr->get(H[i]);
			if(test==Miss[i])
				{
				OK[i]=0;//Missing,zostaje stara wartosc
				continue;
				}

			R[i]=test;
			}

	if(_rescale_data_point(R,pom)==-1)//Czy jest jakis poza zakresem
		if(mode!=2)
			{
			assert(1);
			continue;	//Nie rysuje gdy cos wylazi
			}
			else
			{
			for(i=0;i<M;i++)
				{
				if(pom[i]==INT_MAX)
					{
					pom[i]=height+1;
					if(old[i]==height+1)//Oj, niedobrze!
							OK[i]=0;
					}
				if(pom[i]==INT_MIN)
					{
					pom[i]=-1;
					if(old[i]==-1)
							OK[i]=0;//Oj, niedobrze!
					}
				}
			}
	//Rysowanie punktami
	for(i=0;i<M;i++)
		if(C[i]!=-1 && series[i].figure && OK[i])
        {
            series[i].figure->baselen=this->getheight();
			series[i].figure->plot(x1+pom[M],y2-pom[i],series[i].figure->base_size(),C[i]);
        }

	//Rysowanie liniami
	if(j>0)//Musi byc juz jeden krok
	for(i=0;i<M;i++)
        if(C[i]!=-1 && series[i].figure==nullptr && OK[i])
			{
			int oldX=old[M];
			if(old[i]==-1 || old[i]==height+1)
					oldX=pom[M];//Pion
			line(x1+oldX,y2-old[i],x1+pom[M],y2-pom[i],C[i]);
			}
	}

  //Zamykanie iteratorow
  for(i=0;i<M;i++)
    if(series[i].ptr!=nullptr)
		series[i].ptr->close(H[i]);

  }
  else
  {
  print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CToo small area");
  }
}


// rainbow_graph
//--------------------------------------------

//DESTRUCTOR
rainbow_graph::~rainbow_graph()
{
if(format) delete format;
if(datas && d_menage) delete datas;
datas=nullptr;
d_menage=0;
if(colors && c_menage) delete colors;
colors=nullptr;
c_menage=0;
}

//CONSTRUCTOR(S)
rainbow_graph::rainbow_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
			 data_source_base* idatas,int menage_d,//datas-dane==wartosci
			 data_source_base* icolors,int menage_c,//colors-zrodlo danych o kolorach
			 const char* iformat//format liczby - nie moze byc wiecej niz 1 parametr!
			 ):	graph(ix1,iy1,ix2,iy2),
			datas(idatas),d_menage(menage_d),
			colors(icolors),c_menage(menage_c),
            format(nullptr)
			{
            assert(datas!=nullptr);
			configure(iformat);
			}


// IMPLEMENTATION OF VIRTUAL METHODS oF rainbow_graph
//-----------------------------------------------------
int rainbow_graph::configure(const void* vformat)
//Wymaga const char* format jako parametr
{
const char* iformat=(const char*)vformat;
if(iformat==nullptr)
	{
	if(format) delete format;
	format=clone_str("%%@R%g");//default
	return 0;
	}
const char* pom=iformat;
int licznik=0;
while((pom=strchr(pom,'%'))!=nullptr)
	{
	pom++;
	if(*pom=='%'|| *pom=='@')
		{ pom++;
		continue; //Nie liczba - powtorz!
		}
	while(!isalpha(*pom)) pom++;
	if(tolower(*pom)=='g' ||
	   tolower(*pom)=='e' ||
	   tolower(*pom)=='f' )
			licznik++;//Musi byc jeden!!!
			else
			return -1;//Niedopuszczalny!
	}
if(licznik!=1) return -1;//Za duzo lub za malo
if(format) delete format;
format=clone_str(iformat);
return 0;
}

int rainbow_graph::setseries(size_t index,data_source_base* idata,int imenage)
//zwraca -1 jesli indeks za duzy
{
if(index>1) return -1;//Tylko dwie serie
assert(idata!=nullptr);
if(index==0)
	{
	if(d_menage) delete datas;
	datas=idata;
	d_menage=imenage;
	}
	else
	{
	if(c_menage) delete colors;
	colors=idata;
	c_menage=imenage;
	}
return 0;
}

data_source_base* rainbow_graph::getseries(size_t index)
//zwraca nullptr jesli indeks za duzy
{
if(index>0)
    return nullptr;
if(index==0)
	return datas;
	else
	return colors;
}


void rainbow_graph::_replot()// Rysuje wlasciwy wykres a pod nim ewentualnie legende
{
int x1=getstartx();
int y1=getstarty();
int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
int y2=y1+getheight()-1;
double minc,maxc;
size_t num,num_color,height;
int flaga=0;

assert(x1<x2);
assert(y1<y2);
assert(format!=nullptr);

char* Format=format;//Format dla sprintf
char Format2[6]="%@R%s";//Format dla print_width
if(Format[0]=='%' && Format[1]=='@')
	{
	Format2[2]=Format[2];
	Format+=3;
	}

//Trzeba znac num!
datas->bounds(num,minc,maxc);

//Legenda wtedy jesli jest potrzebna
if(t_colors.start!=getbackground() || (t_colors.end!=getbackground()&& colors))
	height=(y2-y1)-2*char_height('0');//bedzie legenda
	else
	height=y2-y1;//NIe bedzie legendy

//Do skalowania kolorow, jesli jest seria
if(colors!=nullptr)
	{
	colors->bounds(num_color,minc,maxc);
	assert(num<=num_color);
	s_colo.set(minc,maxc,c_range.end-c_range.start+0.999);
	}

//Wypisywanie legendy dla wartosci
if(t_colors.start!=default_transparent && t_colors.start!=getbackground())
	{
	const char* pom=datas->name();
	print_width(x1,y2-char_height('0'),(x2-x1)/2,t_colors.start,getbackground(),
		"%*s",strlen(pom)>79?79:strlen(pom),pom);
	flaga=1;
	}

//...i dla drugiej
if(t_colors.end!=default_transparent && t_colors.end!=getbackground() && colors!=nullptr)
	{
	int width=print_width(x2-(x2-x1)/2,y1,(x2-x1)/2,
		c_range.start,c_range.start!=getbackground()?getbackground():c_range.end,
				"%@R%g",minc);

	const char* pom=colors->name();
	print_width((x2+x1)/2,y1,(x2-x1)/2-width,getbackground(),c_range.start,
		"%@R%*s",strlen(pom)>60?60:strlen(pom),pom);

	print_width(x2-(x2-x1)/2,y2-char_height('0'),(x2-x1)/2,
		c_range.end,c_range.end!=getbackground()?getbackground():c_range.start,
				"%@R%g",maxc);

	flaga++;
	}

//Zawerzenie po y-grekach
if(flaga==2)
	{
	y1+=char_height('0');
	y2-=char_height('0');
	}
	else
	if(flaga==1)
		y2-=char_height('0');

//Rysowanie skali -jesli sa conajmniej dwa kolory i jest miejsce na conajmniej 2 pixele
if(colors!=nullptr && x2-x1>=10 && c_range.end>c_range.start && c_range.end-c_range.start>=1)
	{
	c_range.plot(x2-5,y1,x2,y2);
	x2-=7;
	}
//NIE_DA_SIE:;



//Wypisywanie wartosci
if(y2-y1>=2*char_height('0'))
	{
	data_source_base::iteratorh h=datas->reset();
    data_source_base::iteratorh c=colors?colors->reset():nullptr;
	int grubosc=char_height('0');
    assert(Format!=nullptr);
	for(size_t i=0;i<num;i++)
		{
		double test;
		wb_color color=default_black;
		//int straz1=0xabcd;
		char bufor[128]={0};
		//int straz2=0xabcd;
		assert(grubosc>=1);
		if((i+2)*grubosc>=size_t(y2-y1))
			{
			strcpy(bufor,"...");
			}
			else
			{
			sprintf(bufor,Format,test=datas->get(h));
			}

		if(test==datas->get_missing())
			bufor[0]='\0';

		assert(strlen(bufor)<=127);

        if(colors==nullptr)
			{
			print_width(x1,y1+i*grubosc,x2-x1,c_range.start,getbackground(),Format2,bufor);
			}
			else
			{
			test=s_colo.get(colors->get(c));
			color=int(test);//Test dla warunku dalej
			color+=c_range.start;
			if(color!=getbackground() && test!=colors->get_missing())
				{
				fill_rect(x1,y1+i*grubosc,
					  x2+1,y1+(i+1)*grubosc+1,color);
				print_width(x1,y1+i*grubosc,x2-x1,getbackground(),color,Format2,bufor);
				}
				else
				{
				color=(255!=getbackground()?255:0);
				print_width(x1,y1+i*grubosc,x2-x1,color,getbackground(),Format2,bufor);
				rect(x1,y1+i*grubosc,
				  x2,y1+(i+1)*grubosc,
					color);
				}
			}

		if((i+2)*grubosc>=size_t(y2-y1))
				break;//Nie ma miejsca na reszte petli
		}
	datas->close(h);
	if(colors) colors->close(c);
	}
	else
	{
	printc(x1,(y1+y2)/2,t_colors.start,getbackground(),"Too small area");
	}
}


// reals[] zawiera wartosc ustalajaca kolor, albo (min - 1)
int rainbow_graph::_rescale_data_point(const double reals[],long in_area[])
// zwraca -1 jesli nie w oknie
{
if(reals[0]<=s_colo.max && reals[0]>=s_colo.min)
	{
	unsigned color=(unsigned)s_colo.get(reals[0]);
	in_area[0]=color+c_range.start;
	return 0;
	}
return -1;
}

//scatter_graph & net_graph
//-----------------------------------------------------------------------
//DESTRUCTOR
scatter_graph::~scatter_graph()
{
assert(&CurrConfig!=nullptr);
if(menage_p )
		delete CurrConfig;
if(Xdata!=nullptr && menage_x)
		delete Xdata;
if(Ydata!=nullptr && menage_y)
		delete Ydata;
if(colors!=nullptr && menage_c)
		delete colors;
if(sizes!=nullptr && menage_s)
		delete sizes;
}

net_graph::~net_graph()
{
if(Sources!=nullptr && menage_so)
		delete Sources;
if(Targets!=nullptr && menage_t)
		delete Targets;
if(Arrows!=nullptr && menage_a)
		delete Arrows;
if(ArrColors!=nullptr && menage_ac)
		delete ArrColors;
}

//CONSTRUCTOR(S)
scatter_graph::scatter_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
			 data_source_base* iXdata,int imenage_x  ,//dane o X-ach
			 data_source_base* iYdata,int imenage_y,//dane o Y-ach
			 data_source_base* icolors,int imenage_c,//dane o kolorach
			 data_source_base* isizes,int imenage_s,//dane o rozmiarach
			 config_point*  ifig,int ifmenage//figura domyslna
			 ):
			 graph(ix1,iy1,ix2,iy2),
             CurrConfig(nullptr),menage_p(0),
			 Xdata(iXdata),menage_x(imenage_x),
			 Ydata(iYdata),menage_y(imenage_y),
			 colors(icolors),menage_c(imenage_c),
			 sizes(isizes),menage_s(imenage_s)
{
assert(Xdata!=nullptr);
assert(Ydata!=nullptr);
config_scat pom(ifig,ifmenage);
if(ifig!=nullptr)
	scatter_graph::configure(&pom);
	else
    scatter_graph::configure(nullptr);
}

net_graph::net_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
			 data_source_base* iXdata,int imenage_x  ,//dane o X-ach
			 data_source_base* iYdata,int imenage_y,//dane o Y-ach
             data_source_base* iSources,int imenage_so,//indeksy zrodel
             data_source_base* iTargets,int imenage_t,//indeksy celów
			 data_source_base* iColors,int imenage_c,//dane o kolorach
			 data_source_base* iSizes,int imenage_s,//dane o rozmiarach
             data_source_base* iArrows,int imenage_a,//rozmiary strzalek - 0 brak
             data_source_base* iArrColors,int imenage_ac,//Kolory strzalek
			 config_point*  ifig,int ifmenage//figura domyslna
             ):
            scatter_graph(ix1,iy1,ix2,iy2,
                            iXdata,imenage_x  ,//dane o X-ach
			                iYdata,imenage_y,//dane o Y-ach
			                iColors,imenage_c,//dane o kolorach
			                iSizes,imenage_s,//dane o rozmiarach
                            ifig,ifmenage //Domyslny moze byc brak rysowania wezlów sieci
                            ),
            Sources(iSources),Targets(iTargets),Arrows(iArrows),ArrColors(iArrColors),
            menage_so(imenage_so),menage_t(imenage_t),menage_a(imenage_a),menage_ac(imenage_ac)
{
    assert(Sources!=nullptr);
    assert(Targets!=nullptr);
}

// IMPLEMENTATION OF VIRTUAL METHODS
int scatter_graph::configure(const void* p)
//Parametr typu scatter_graph::config*
{
if(menage_p && CurrConfig!=nullptr)
		delete CurrConfig;
if(p!=nullptr)
	{
	CurrConfig=((config_scat*)p)->fig;
	menage_p=((config_scat*)p)->menage;
	}
	else
	{
    CurrConfig=nullptr;//new hash_point;
	menage_p=0;
	}
return 0;
}

int scatter_graph::setseries(size_t index,data_source_base* data,int menage)
//zwraca -1 jesli indeks za duzy
{
assert(data!=nullptr);
switch(index){
case 0:if(menage_x && Xdata!=nullptr)
		   delete Xdata;
		Xdata=data;menage_x=menage;break;
case 1:if(menage_y && Ydata!=nullptr)
		   delete Ydata;
		Ydata=data;menage_y=menage;break;
case 2:if(menage_c && colors!=nullptr)
		   delete colors;
		colors=data;menage_c=menage;break;
case 3:if(menage_s && sizes!=nullptr)
		   delete sizes;
		sizes=data;menage_s=menage;break;
default:
	return -1;}
return 0;
}

int net_graph::setseries(size_t index,data_source_base* data,int menage)
//zwraca -1 jesli indeks za duzy
{
assert(data!=nullptr);
switch(index){
case 0:if(menage_x && Xdata!=nullptr)
		   delete Xdata;
		Xdata=data;menage_x=menage;break;
case 1:if(menage_y && Ydata!=nullptr)
		   delete Ydata;
		Ydata=data;menage_y=menage;break;
case 2:if(menage_c && colors!=nullptr)
		   delete colors;
		colors=data;menage_c=menage;break;
case 3:if(menage_s && sizes!=nullptr)
		   delete sizes;
		sizes=data;menage_s=menage;break;
case 4:if(menage_so && Sources!=nullptr)
		   delete Sources;
		Sources=data;menage_so=menage;break;
case 5:if(menage_t && Targets!=nullptr)
		   delete Targets;
		Targets=data;menage_t=menage;break;
case 6:if(menage_a && Arrows!=nullptr)
		   delete Arrows;
		Arrows=data;menage_a=menage;break;
case 7:if(menage_ac && ArrColors!=nullptr)
		   delete ArrColors;
		ArrColors=data;menage_ac=menage;break;
default:
	return -1;}
return 0;
}

data_source_base* scatter_graph::getseries(size_t index)
//zwraca nullptr jesli indeks za duzy
{
switch(index){
case 0:return Xdata;
case 1:return Ydata;
case 2:return colors;
case 3:return sizes;
default:
    return nullptr;}
}

data_source_base* net_graph::getseries(size_t index)
//zwraca nullptr jesli indeks za duzy
{
switch(index){
case 0:return Xdata;
case 1:return Ydata;
case 2:return colors;
case 3:return sizes;
case 4:return Sources;
case 5:return Targets;
case 6:return Arrows;
case 7:return ArrColors;
default:
    return nullptr;}
}


int scatter_graph::_rescale_data_point(const double reals[],long in_area[])
//zwraca -1 jesli cos nie tak
{
int flaga=0;

if(reals[0]<=scale_x.max && reals[0]>=scale_x.min)
	{
	unsigned pom=(unsigned)scale_x.get(reals[0]);
	in_area[0]=pom;
	}
	else flaga=1;

if(reals[1]<=scale_y.max && reals[1]>=scale_y.min)
	{
	unsigned pom=(unsigned)scale_y.get(reals[1]);
	in_area[1]=pom;
	}
	else flaga=1;

if(reals[2]<=scale_c.max && reals[2]>=scale_c.min)
	{
	unsigned color=(unsigned)scale_c.get(reals[2]);
	in_area[2]=color+c_range.start;
	}

if(reals[3]<=scale_s.max && reals[3]>=scale_s.min)
	{
	unsigned size=(unsigned)scale_s.get(reals[3]);
	in_area[3]=size+CurrConfig->base_size();
	}

if(flaga)
	return -1;
	else
	return 0;

return -1;
}


void scatter_graph::_replot()
// Rysuje wlasciwy wykres a pod nim ewentualnie legende
{
int x1=getstartx();
int y1=getstarty();
int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
int y2=y1+getheight()-1;
size_t       num,num_X,num_Y,num_color,num_size;//Number of values in each series
double       missX,missY,missColor,missSize;    //missing values for each series
int flaga=0;

num=num_X=num_Y=num_color=num_size=0;//Abrakadabra

//Miejsce na legende, wtedy gdy jest potrzebna
unsigned height,width;
if((t_colors.start!=default_transparent && t_colors.start!=getbackground() ) ||
   (t_colors.end!=default_transparent && t_colors.end!=getbackground() && (colors || sizes) ))
	{
	height=(y2-y1)-(2*char_height('0')+3);//bedzie legenda
    if(sizes!=nullptr && t_colors.end!=getbackground())
    {
        CurrConfig->baselen=_min(this->getwidth(),this->getheight());//szacunkowo
		/*height-=char_height('0');*/height-=_max(CurrConfig->max_size(),unsigned(char_height('0')));
    }
	width=(x2-x1)-3;
    if(colors!=nullptr && t_colors.end!=getbackground())
		width-=10;//Musi byc jakis odstep
	}
	else
	{
	height=y2-y1;//NIe bedzie legendy
	width=x2-x1;
	}

{//Ustawianie struktow skalujacych
double min=0;double max=-1;//Byle co, i tak zaraz zostanie przypisane

//Do skalowania X-ow
                                        assert(Xdata!=nullptr);
Xdata->bounds(num_X,min,max);
if(num_X==0 || min==max || min>max) goto SKIP; //Seria wadliwa - PUSTA?
missX=Xdata->get_missing();
scale_x.set(min,max,width);

//Do skalowania Y-ow
                                        assert(Ydata!=nullptr);
Ydata->bounds(num_Y,min,max);
if(num_Y==0 || min==max || min>max) goto SKIP; //Seria wadliwa - PUSTA?
missY=Ydata->get_missing();
scale_y.set(min,max,height);

//Do skalowania kolorow, jesli jest seria i ma odstep miedzy min i max
if(colors!=nullptr)
{
	colors->bounds(num_color,min,max);
	missColor=colors->get_missing();
	if(num_color==0 || min==max || min>max)//Seria wadliwa - PUSTA?
	{
		num_color=num_X;//Ale zeby sie rysowalo bez tego - taki sygnal...
	}
	else
	{
		scale_c.set(min,max,c_range.end-c_range.start+0.999);//Do kolorow potrzebna poprawka 0.999 w skalowaniu bo nie bedzie bialego (max)
	}
}
else num_color=num_X;

//Do skalowania rozmiarow, jesli jest seria i ma odstep miedzy min i max
if(sizes!=nullptr)
{
	//DEBUG min=max=0;
	sizes->bounds(num_size,min,max);
	missSize=sizes->get_missing();
	if(num_size==0 || min==max  || min>max)//Seria wadliwa - PUSTA?
	{
		num_size=num_X;//Ale zeby sie rysowalo bez tego - taki sygnal...
	}
	else
	{
		CurrConfig->baselen=_min(this->getwidth(),this->getheight());
		scale_s.set(min,max,CurrConfig->max_size()-CurrConfig->base_size());
	}
}
else num_size=num_X;

}

//Rysowanie legendy dla rozmiarow
if(t_colors.end!=default_transparent && t_colors.end!=getbackground() && (sizes!=nullptr) && vis_leg4)
	{
	double min,max;
    unsigned rmin,rmax; size_t dummy;
	sizes->bounds(dummy,min,max);
    rmin=scale_s.get(min)+CurrConfig->base_size();//scale_s.get(scale_s.min)+1
    rmax=scale_s.get(max)+CurrConfig->base_size();//scale_s.get(scale_s.max)+1 ALE: (CurrConfig->max_size()-CurrConfig->base_size())/2 ???

	CurrConfig->plot(x1+rmin/2,y1+rmin/2,rmin,(c_range.end-c_range.start)/2);
	CurrConfig->plot(x2-rmax/2,y1+rmax/2,rmax,(c_range.end-c_range.start)/2);

	int x=x1+CurrConfig->base_size()+1;
	int xE=x2-CurrConfig->max_size()-1;
	int width=xE-x;
	width=print_width(x,y1,width/3,t_colors.end,getbackground(),
		"%g",scale_s.min);

    if(width==0) goto SKIP;//Nie ma miejsca?
		else x+=width;

	const char* pom=sizes->name();
	width=print_width(x,y1,(xE-x)/2,getbackground(),t_colors.end,
		" %*s",strlen(pom)>60?60:strlen(pom),pom);

	if(width==0) goto SKIP;
		else x+=width;

	print_width(x,y1,xE-x,t_colors.end,getbackground(),
		"%@R%g",scale_s.max);

	y1+=_max(CurrConfig->max_size(),unsigned(char_height('0')));
	}

SKIP:
//Wypisywanie legendy dla 1 i 2 serii
if(t_colors.start!=default_transparent && t_colors.start!=getbackground() && vis_leg1)
	{
	int swidth=0;
	int pos=x1;
	//Do drukowania max Y
	print_width(x1,y1,(x2-x1)/2,t_colors.start,getbackground(),
		 "%g",scale_y.max);

	const char* pom2=Xdata->name();
	const char* pom1=Ydata->name();

	//Min Y i seria I
	swidth=print_width(pos,y2-char_height('0'),(x2-x1)/2,t_colors.start,getbackground(),
		"%g %*s",scale_y.min,strlen(pom1)>60?60:strlen(pom1),pom1);
	pos+=swidth;

	//seria II
	swidth=print_width(pos,y2-char_height('0'),(x2-pos)/3*2,t_colors.start,getbackground(),
		",%*s",strlen(pom2)>60?60:strlen(pom2),pom2);
	if(pos==0) goto SKIP2;
	pos+=swidth;

	//Min X
	swidth=print_width(pos,y2-char_height('0'),(x2-pos)/2,t_colors.start,getbackground(),
		"<%g",scale_x.min);
	if(pos==0) goto SKIP2;
	pos+=swidth;

	//Max X
	print_width(pos,y2-char_height('0'),x2-pos,t_colors.start,getbackground(),
		"%@R,%g>",scale_x.max);

	flaga=1;
	}

SKIP2:
//...i dla trzeciej
if(t_colors.end!=default_transparent && t_colors.end!=getbackground() && (colors!=nullptr) && vis_leg2)
	{
	int swidth=(x2-x1)/2;
	//Zakres serii kolorow - najwyzej 2/3 miejsca
	int width1=print_width(x2-swidth/3,y1,swidth/3,
		c_range.end,c_range.end!=getbackground()?getbackground():c_range.start,
		"%@R,%g>",scale_c.max);

	int width2=print_width(x2-width1-swidth/3,y1,swidth/3,
	c_range.start,c_range.start!=getbackground()?getbackground():c_range.end,
		"%@R<%g",scale_c.min);

	// Nazwa serii kolorow - reszta
	const char* pom=colors->name();
	print_width((x1+x2)/2,y1,swidth-width1-width2,getbackground(),t_colors.start,
		"%@R%*s",strlen(pom)>60?60:strlen(pom),pom);

	flaga=1;
	}

//Zawerzenie po y-grekach
if(flaga)
	{
	y1+=char_height('0');
	y2-=char_height('0');
	}


//Rysowanie skali -jesli sa conajmniej dwa kolory i jest miejsce na conajmniej 2 pixele
if(t_colors.end!=default_transparent && t_colors.end!=getbackground() && colors!=nullptr &&
   x2-x1>=10 && c_range.end>c_range.start && c_range.end-c_range.start>=1 && vis_leg3)
	{
	c_range.plot(x2-5,y1,x2,y2);
	x2-=10;
	}
//NIE_DA_SIE:;

//Strzalka w prawo
if(x2-x1>10 && y2-y1>10)
	{
	scale_x.OXaxis(x1+3,y2-6,x1+3+width,y2,t_colors.start,getbackground());
	y2-=3;
	}

//Strzalka w gore
if(x2-x1>10 && y2-y1>10)
	{
	scale_y.OYaxis(x1,y1,x1+6,y2,t_colors.start,getbackground());
	x1+=3;
	}

//Rysowanie punktow
num=_min(_min((size_t)num_X,num_Y),_min((size_t)num_size,num_color));//cast for gcc

graph_core.set(x1,y1,x2,y2);//Zapamietanie gdzie lezy wlasciwy obszar rysowania

if(num>0 && x2-x1>10 && y2-y1>10)
{
    if(CurrConfig!=nullptr)//Jesli uzytkownik w ogole zyczy sobie rysowac jakies punkty (bo klasa potomna moze nie chciec!)
    {
        data_source_base::iteratorh ix=Xdata->reset();
        data_source_base::iteratorh iy=Ydata->reset();
        data_source_base::iteratorh ic=colors?colors->reset():nullptr;
        data_source_base::iteratorh is=sizes?sizes->reset():nullptr;
            if(CurrConfig!=nullptr)//Jesli uzytkownik w ogole zyczy sobie rysowac jakies punkty (bo klasa potomna moze nie chciec!)
                for(unsigned i=0;i<num;i++)
                {
                    CurrConfig->baselen=x2-x1;//Ustawiamy bazowy rozmiar obszaru rysowania
                    double    test;
                    double    r[4]={-1,-1,-1,-1};
                    long      a[4]={0,0,(c_range.end-c_range.start)/2,(CurrConfig->max_size()-CurrConfig->base_size())/2};
                    r[0]=test=Xdata->get(ix);
                    if(test==missX) continue;
                    r[1]=test=Ydata->get(iy);
                    if(test==missY) continue;
                    if(colors)
                    {
                        r[2]=test=colors->get(ic);
                        if(test==missColor) continue;
                    }
                    if(sizes)
                    {
                        r[3]=test=sizes->get(is);
                        if(test==missSize) continue;
                    }
                    _rescale_data_point(r,a);

                    CurrConfig->plot(x1+a[0],y2-a[1],a[3]+1,a[2]);
                }
                Xdata->close(ix);
                Ydata->close(iy);
                if(sizes) sizes->close(is);
                if(colors) colors->close(ic);
    }
}
else
  {
  print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CInvalid data or too small area");
  }
}

// Lokalna funkcyjka podnosząca do kwadratu.
static inline double sqr(double x) { return x*x; }

void net_graph::_replot()
// Rysuje wykres punktowy a pod nim ewentualnie legende
//a do tego laczy punkty liniami
{
    scatter_graph::_replot();

    assert(Sources!=nullptr);assert(Targets!=nullptr);

    size_t num_so,num_t,num_a,num_c,num;//Ilosc elementow w kazdej z serii i ostatecznie ilosc minimalna (num)
    double mini,maxi,mina,maxa;
    double SameSize=sqrt(sqr(graph_core._x1()-graph_core._x2())+sqr(graph_core._y1()-graph_core._y2()))*0.025;

    Sources->bounds(num_so,mini,maxi);

    Targets->bounds(num_t,mini,maxi);

    //Sprawdzenie czy jest wystarczajoca ilosc informacji o grotach
    if(Arrows)
    {
        Arrows->bounds(num_a,mina,maxa);
        if(num_a==0) goto ERROR_HAPPENED;
    }
    else
        num_a=num_so;//Jak nie ma to zeby num bylo dobrze ustawione

    //Do skalowania kolorow, jesli jest seria
    if(ArrColors!=nullptr)
    {
        ArrColors->bounds(num_c,mina,maxa);
        if(num_c==0) goto ERROR_HAPPENED;
        //if(mina==maxa){mina--;maxa++;}???
        scale_ac.set(mina,maxa,c_range.end-c_range.start);//(c_range.end-1)-c_range.start+0.999);
    }
	else num_c=num_so;//Jak nie ma to zeby num bylo dobrze ustawione

    //USTAWIANIE num NA DLUGOSC NAJKROTSZEJ SERII
    num=_min(_min(num_so,num_t),_min(num_a,num_c));

    if(num>0)
    {
		//Wszystkie serie musza udostepniac bezposrednie czytanie!!!
		//for(size_t i=0;i<num;i++)
        for(size_t j=0,i=num-1;j<num;i--,j++)//W odwrotnej kolejności!
        {
													assert(i<num);
            double Si=Sources->get(i);
            double Ti=Targets->get(i);
            if(Sources->is_missing(Si) || Targets->is_missing(Ti))
                continue;

            double Present=0;
            if(Arrows!=nullptr)
            {
                Present=Arrows->get(i);
                if( Arrows->is_missing(Present))
                    continue;
            }

            double AColor=0.5;
            if(ArrColors!=nullptr)
            {
                AColor=ArrColors->get(i);
                if( ArrColors->is_missing(AColor))
                    continue;
            }

                                                        assert(Si==size_t(Si));
            double ASize=0.05;//Dlugosc grota =5% dlugosci strzalki
            double test;

            double r[4]={-1,-1,-DBL_MAX,-DBL_MAX};
            long   a1[4]={0,0,-INT_MAX,-INT_MAX};
            long   a2[4]={0,0,-INT_MAX,-INT_MAX};

            r[0]=test=Xdata->get(size_t(Si));//Seria reprezentowana przez Si zawiera indeksy obiektow, pobieramy wspolrzedne
            if(Xdata->is_missing(test))
                goto ERROR_HAPPENED;

            r[1]=test=Ydata->get(size_t(Si));//Ta seria jest zawiera indeksy obiektow
            if(Ydata->is_missing(test))
                goto ERROR_HAPPENED;

            _rescale_data_point(r,a1);//Odziedziczone po scatter_plot
                                                    assert(Ti==size_t(Ti));
            r[0]=test=Xdata->get(size_t(Ti));//Seria reprezentowana przez Ti zawiera indeksy obiektow, pobieramy wspolrzedne
            if(Xdata->is_missing(test))
                goto ERROR_HAPPENED;

            r[1]=test=Ydata->get(size_t(Ti));//Ta seria jest zawiera indeksy obiektow
            if(Ydata->is_missing(test))
                goto ERROR_HAPPENED;

            _rescale_data_point(r,a2);//Odziedziczone po scatter_plot

            unsigned color=255;
            if(AColor<=scale_ac.max && AColor>=scale_ac.min)
            {
                color=(unsigned)scale_ac.get(AColor)+c_range.start;
            }

            if(Present>0)
            {
                if(Present<1)
                    ASize=Present;
					else
                    {
						double D=sqr(a1[0]-a2[0])+sqr(a1[1]-a2[1]);
						if(D>0)
							D=sqrt(D);
							else
							D=0;
                        if(SameSize<D/2)
                            ASize=SameSize;
						else
                            ASize=D/2;
                    }

                arrow(graph_core._x1()+a1[0],graph_core._y2()-a1[1],
                      graph_core._x1()+a2[0],graph_core._y2()-a2[1],
                      color,
                      ASize,3.27);
			}
            else
            {
               line(graph_core._x1()+a1[0],graph_core._y2()-a1[1],
                    graph_core._x1()+a2[0],graph_core._y2()-a2[1],
                    color);
            }
        }
    }
    else
    {
        goto ERROR_HAPPENED;
    }

    return;
ERROR_HAPPENED:
     print_width(this->getstartx()+1,this->getstarty()+this->getheight()/2,
                    _min(string_width("Invalid conections data"),this->getwidth()),
                    128,getbackground(),"%@CInvalid conections data");
}

/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/
