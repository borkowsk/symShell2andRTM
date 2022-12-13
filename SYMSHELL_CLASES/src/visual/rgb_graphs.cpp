///////////////////////////////////////////////////////////////////////////////////////////
//IMPLEMENTACJA KLAS GRAFOW UZYWAJACYCH KOLORAWANIA RGB
///////////////////////////////////////////////////////////////////////////////////////////

#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

//#include "platform.hpp"

#include <cstdlib>
#include <cstdio> //sprintf!!!
#include <iostream>

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

//true color carpet_graph
//-------------------------------------------

//DESTRUCTOR
true_color_carpet_graph::~true_color_carpet_graph()
{
	if(menage[0] && data[0]) delete data[0];
	if(menage[1] && data[1]) delete data[1];
	if(menage[2] && data[2]) delete data[2];
    data[0]=data[1]=data[2]=nullptr;
	menage[0]=menage[1]=menage[2]=0;
}

//CONSTRUCTOR(S)
true_color_carpet_graph::true_color_carpet_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
												 unsigned iA,unsigned iB,			 //A-ile kolumn,B-ile wierszy
												 data_source_base* RedData,int menage_r,
												 data_source_base* GreenData,int menage_g,
												 data_source_base* BlueData,int menage_b
												 )://data-zrodlo danych o kolorach
graph(ix1,iy1,ix2,iy2),
AA(iA),BB(iB)			
{
	data[0]=RedData;menage[0]=menage_r;
	data[1]=GreenData;menage[1]=menage_g;
	data[2]=BlueData;menage[2]=menage_b;
    assert(data[0]!=nullptr || data[1]!=nullptr || data[2]!=nullptr);
	assert(AA>=2 && BB>=2);
}

true_color_carpet_graph::true_color_carpet_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru. Serie musza miec taka sama geometrie 2D			
												 data_source_base* RedData,int menage_r,
												 data_source_base* GreenData,int menage_g,
												 data_source_base* BlueData,int menage_b
												 )://data-zrodlo danych o kolorach
graph(ix1,iy1,ix2,iy2),
AA(1),BB(1)
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
		MyGeometry[posit++]=data[0]->getgeometry(); 
	if(data[1])
		MyGeometry[posit++]=data[1]->getgeometry();
	if(data[2])
		MyGeometry[posit++]=data[2]->getgeometry();

    if( MyGeometry[0]==nullptr || //Gdyby nie bylo wcale albo byla wadliwa
		MyGeometry[0]->get_dimension()<2 )
	{
		//	A=subtab[1];
		//	B=subtab[3];						
		if(!zastepnik && AA>1 && BB>1)
			zastepnik=new rectangle_geometry(AA,BB,0);//Nie torus
		else
            return nullptr; //Wadliwe dane
		aa=AA;
		bb=BB;
		return zastepnik.get_ptr_val();
	}
	else 
	{
		//	A=subtab[1];
		//	B=subtab[3];			
		geometry::view_info pom[3];
		MyGeometry[0]->get_view_info(&pom[0]);

		if(MyGeometry[1])//Sprawdzanie czy inne geometrie sa takie same
		{
			if(MyGeometry[0]->get_dimension()!=MyGeometry[1]->get_dimension())
                return nullptr;
			MyGeometry[1]->get_view_info(&pom[1]);
			if(pom[0].dia.X()!=pom[1].dia.X() ||
				pom[0].dia.Y()!=pom[1].dia.Y())
                return nullptr;//Wadliwe dane
		}
		if(MyGeometry[2])
		{
			if(MyGeometry[0]->get_dimension()!=MyGeometry[2]->get_dimension())
                return nullptr;
			MyGeometry[2]->get_view_info(&pom[2]);
			if(pom[0].dia.X()!=pom[2].dia.X() ||
				pom[0].dia.Y()!=pom[2].dia.Y())
                return nullptr;//Wadliwe dane
		}

		aa=size_t(pom[0].dia.X()*2);
		bb=size_t(pom[0].dia.Y()*2);

		return MyGeometry[0];
	}
}

int true_color_carpet_graph::setseries(size_t index,data_source_base* idata,int imenage)
//zwraca -1 jesli indeks za duzy
{
	if(index>2) return -1;//Tylko jedna seria
    assert(idata!=nullptr);
	if(menage[index]) delete data[index];
	data[index]=idata;
	menage[index]=imenage;
	return 0;
}

data_source_base* true_color_carpet_graph::getseries(size_t index)
//zwraca nullptr jesli indeks za duzy
{
	if(index>2)
        return nullptr;
	return data[index];
}

void true_color_carpet_graph::_replot()// Rysuje wlasciwy wykres a pod nim ewentualnie legende
{
	int x1=getstartx();
	int y1=getstarty();
	int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
	int y2=y1+getheight()-1;

	assert(x1<=x2);//Czy aby na pewno
	assert(y1<=y2);//sensowne okno. Moze miec zerowy rozmiar ale nie ujemny

	double min[3]={0,0,0};
	double max[3]={1,1,1};
	double missing[3];

	size_t A=1,B=1;//Lokalne A i B

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
		print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CInvalid data");
		return;
	}

	//Wartosci zwracane gdy brak danych
	if(data[0])
		missing[0]=data[0]->get_missing();
	if(data[1])
		missing[1]=data[1]->get_missing();
	if(data[2])
		missing[2]=data[2]->get_missing();

	//Do skalowania kolorow
	if(data[0])
		mm[0].set(min[0],max[0],c_range.end-c_range.start+0.999);
	if(data[1])
		mm[1].set(min[1],max[1],c_range.end-c_range.start+0.999);
	if(data[2])
		mm[2].set(min[2],max[2],c_range.end-c_range.start+0.999);

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
        if(data[0]!=nullptr)
		{
			width=print_width(x,y,(x2-x)/3*2,50,getbackground(),//50 to czerwone z palety
				"%s",data[0]->name());

			if(width==0) goto KWADRACIKI;
			else	 x+=width+1;

			width=print_width(x,y,(x2-x)/2,50,
				c_range.start!=getbackground()?getbackground():c_range.end,
				"<%g",min[0]);

			if(width==0) goto KWADRACIKI;
			else	x+=width+1;

			width=print_width(x,y,x2-x,255,//Biale na czerwonym z palety
				50,
				",%g>",max[0]);

			if(width==0) goto KWADRACIKI;
			else	x+=width+5;
		}
        if(data[1]!=nullptr)
		{
			width=print_width(x,y,(x2-x)/3*2,220,getbackground(),//220 to zielone z palety
				"%s",data[1]->name());

			if(width==0) goto KWADRACIKI;
			else	 x+=width+1;

			width=print_width(x,y,(x2-x)/2,220,
				c_range.start!=getbackground()?getbackground():c_range.end,
				"<%g",min[1]);

			if(width==0) goto KWADRACIKI;
			else	x+=width+1;

			width=print_width(x,y,x2-x,255,//Biale na zielonym z palety
				220,
				",%g>",max[1]);

			if(width==0) goto KWADRACIKI;
			else	x+=width+5;
		}
        if(data[2]!=nullptr)
		{
			width=print_width(x,y,(x2-x)/3*2,128,getbackground(),//128 to niebieski z palety
				"%s",data[2]->name());

			if(width==0) goto KWADRACIKI;
			else	 x+=width+1;

			width=print_width(x,y,(x2-x)/2,128,
				c_range.start!=getbackground()?getbackground():c_range.end,
				"<%g",min[2]);

			if(width==0) goto KWADRACIKI;
			else	x+=width+1;

			width=print_width(x,y,x2-x,255,//Biale na niebieskim z palety
				128,
				",%g>",max[2]);

			if(width==0) goto KWADRACIKI;
			else	x+=width+5;
		}
	}


	//Rysowanie kwadracikow
KWADRACIKI:

	assert(x2>x1);
	assert(y2>y1);
	if(A>=1 && B>=1 && A<=size_t(x2-x1+1) && B<=size_t(y2-y1+1))
	{
		assert(c_range.end-c_range.start>=1);
		size_t i,j;//Indeksy po wierszach i kolumnach
		int width=x2-x1+1;//Juz moga byc inne
		int height=y2-y1+1;//Niz dla calego obszaru
		int gruboscA=width/A;
		int gruboscB=height/B;
		//Musi byc kwadratowo bo inaczej jest nieladnie
		if(gruboscA>1 && gruboscB>1)
		{
			if(gruboscA>gruboscB) gruboscA=gruboscB;
			else		  gruboscB=gruboscA;	
		}
		else
		{
			gruboscA=gruboscB=1;//Pixelami panowie!!!
		}

		int offsetA=(width-gruboscA*A)/2;
		int offsetB=(height-gruboscB*B)/2;

		//Rysowanie
		data_source_base::iteratorh h=MyGeometry->make_view_iterator();
		wb_color back=getbackground();//Dla sprawdzania kiedy kolor kwadratu taki jak kolor tla.

		if(gruboscA==1)//starczy jedna sprawdzic bo kwadrat
		{//Pixelami panowie!!!			
			for(j=0;j<B;j++)
				for(i=0;i<A;i++)
                {                                   assert(h!=nullptr);
			size_t Gind=MyGeometry->get_next(h);
			double test[3]={0,0,0};
			wb_color color[3]={0,0,0};//Na razie wb_color jest unsigned wiec dziala, ale ...
            if(data[0]!=nullptr)
			{
				test[0]=data[0]->get(Gind);
				if(test[0]!=missing[0])
				{
					color[0]=mm[0].get(test[0])+c_range.start;	assert(color[0]<=c_range.end);  	          
					if(color[0]>c_range.end)	
						goto NIE_DA_SIE;//Awaria
				}
			}
            if(data[1]!=nullptr)
			{
				test[1]=data[1]->get(Gind);
				if(test[1]!=missing[1])
				{
					color[1]=mm[1].get(test[1])+c_range.start;	assert(color[1]<=c_range.end);  	          
					if(color[1]>c_range.end)	
						goto NIE_DA_SIE;//Awaria
				}
			}
            if(data[2]!=nullptr)
			{
				test[2]=data[2]->get(Gind);
				if(test[2]!=missing[2])
				{
					color[2]=mm[2].get(test[2])+c_range.start;	assert(color[2]<=c_range.end);  	          
					if(color[2]>c_range.end)	
						goto NIE_DA_SIE;//Awaria
				}
			}            								
			plot_rgb(offsetA+x1+i,offsetB+y1+j,color[0],color[1],color[2]);
			//else goto NIE_DA_SIE;
			}
		}
		else
		{//Kwadratami
			for(j=0;j<B;j++)
				for(i=0;i<A;i++)
                {                                     assert(h!=nullptr);
			size_t Gind=MyGeometry->get_next(h);//rectangle_geometry
			double test[3]={0,0,0};
			wb_color color[3]={0,0,0};//Na razie wb_color jest unsigned wiec dziala, ale ...
            if(data[0]!=nullptr)
			{
				test[0]=data[0]->get(Gind);
				if(test[0]!=missing[0])
				{
					color[0]=mm[0].get(test[0])+c_range.start;	assert(color[0]<=c_range.end);  	          
					if(color[0]>c_range.end)	
						goto NIE_DA_SIE;//Awaria
				}
			}
            if(data[1]!=nullptr)
			{
				test[1]=data[1]->get(Gind);
				if(test[1]!=missing[1])
				{
					color[1]=mm[1].get(test[1])+c_range.start;	assert(color[1]<=c_range.end);  	          
					if(color[1]>c_range.end)	
						goto NIE_DA_SIE;//Awaria
				}
			}
            if(data[2]!=nullptr)
			{
				test[2]=data[2]->get(Gind);
				if(test[2]!=missing[2])
				{
					color[2]=mm[2].get(test[2])+c_range.start;	assert(color[2]<=c_range.end);  	          
					if(color[2]>c_range.end)	
						goto NIE_DA_SIE;//Awaria
				}
			}
			/*          
			if(color==back && gruboscA>3)
			{
			rect(offsetA+x1+i*gruboscA,offsetB+y1+j*gruboscB,
			offsetA+x1+(i+1)*gruboscA-1,offsetB+y1+(j+1)*gruboscB-1,
			255!=back?255:0);
			}
			else*/
			{
				set_brush_rgb(color[0],color[1],color[2]);
				fill_rect_d(offsetA+x1+i*gruboscA,offsetB+y1+j*gruboscB,
					offsetA+x1+(i+1)*gruboscA,offsetB+y1+(j+1)*gruboscB);
			}

			}
		}
		MyGeometry->destroy_iterator(h);
	}
	else
	{
		print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CToo small area");
	}

	return;
NIE_DA_SIE:
	print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CInvalid colors found.");
	print_width(x1,(y1+y2)/2+char_height('X'),x2-x1,t_colors.start,getbackground(),"%@CProbably min or max not properly set.");
}

// reals[] zawiera jedynie wartosc ustalajaca kolor, albo cos spoza zakresu
int true_color_carpet_graph::_rescale_data_point(const double reals[3],long in_area[3])
//zwraca -1 jesli nie w oknie
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
	return (ile==0?-1:0);//Blad jesli nie bylo zadnego lub zadne nie bylo w oknie
}

// true_color_manhattan_graph
//--------------------------------------------

//DESTRUCTOR
true_color_manhattan_graph::~true_color_manhattan_graph()
{
	if(datas && d_menage) delete datas;
    datas=nullptr;
	d_menage=0;
	if(colors[0] && c_menage[0]) delete colors[0];
    colors[0]=nullptr;
	c_menage[0]=0;
	if(colors[1] && c_menage[1]) delete colors[1];
    colors[1]=nullptr;
	c_menage[1]=0;
	if(colors[2] && c_menage[2]) delete colors[2];
    colors[2]=nullptr;
	c_menage[2]=0;
}

//CONSTRUCTOR(S)
true_color_manhattan_graph::true_color_manhattan_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
													   unsigned iA,unsigned iB,
													   data_source_base* idatas,int menage_d,//datas-dane o wysokosciach
													   data_source_base* RedData,int menage_r,
													   data_source_base* GreenData,int menage_g,
													   data_source_base* BlueData,int menage_b,
													   int zero_mod, //tryb wyswietlania
													   double iHoffs,		//Ulamek szerokosci przeznaczony na perspektywe
													   double	iVoffs		//Ulamek wysokosci  przeznaczony na perspektywe
													   ):
graph(ix1,iy1,ix2,iy2),AA(iA),BB(iB),
datas(idatas),d_menage(menage_d),
mode(zero_mod),c_offset(0),wire(getbackground()),
Hoffs(iHoffs),Voffs(iVoffs)
{
	colors[0]=RedData;c_menage[0]=menage_r;
	colors[1]=GreenData;c_menage[1]=menage_g;
	colors[2]=BlueData;c_menage[2]=menage_b;
    assert(datas!=nullptr);
	assert(AA!=0);
	assert(BB!=0);
	assert(Voffs>0 && Voffs<1);
	assert(Hoffs>0 && Hoffs<1);
}

true_color_manhattan_graph::true_color_manhattan_graph(int ix1,int iy1,int ix2,int iy2,//Polozenie obszaru
													   data_source_base* idatas,int menage_d,//datas-dane o wysokosciach
													   data_source_base* RedData,int menage_r,
													   data_source_base* GreenData,int menage_g,
													   data_source_base* BlueData,int menage_b,
													   int zero_mod, //tryb wyswietlania
													   double iHoffs,		//Ulamek szerokosci przeznaczony na perspektywe
													   double	iVoffs		//Ulamek wysokosci  przeznaczony na perspektywe
													   ):
graph(ix1,iy1,ix2,iy2),AA(1),BB(1),
datas(idatas),d_menage(menage_d),
mode(zero_mod),c_offset(0),wire(getbackground()),
Hoffs(iHoffs),Voffs(iVoffs)
{
	colors[0]=RedData;c_menage[0]=menage_r;
	colors[1]=GreenData;c_menage[1]=menage_g;
	colors[2]=BlueData;c_menage[2]=menage_b;
    assert(datas!=nullptr);
	assert(AA!=0);
	assert(BB!=0);
	assert(Voffs>0 && Voffs<1);
	assert(Hoffs>0 && Hoffs<1);
}

// IMPLEMENTATION OF VIRTUAL METHODS oF true_color_manhattan_graph
//-----------------------------------------------------
const geometry_base* true_color_manhattan_graph::read_dim(size_t& aa,size_t& bb)
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
		//Sprawdzenie czy serie sie nie zgadzaja - czy maja dokladnie ta sama geometrie
        if(colors[0]!=nullptr)
		{
			geometry_base* ColGeom=colors[0]->getgeometry();
			if(*MyGeometry!=*ColGeom)
			{//jesli nie to pozbywany sie skladowej
				if(c_menage[0]) delete colors[0];
                colors[0]=nullptr;
				c_menage[0]=0;
			}
		}
        if(colors[1]!=nullptr)
		{
			geometry_base* ColGeom=colors[1]->getgeometry();
			if(*MyGeometry!=*ColGeom)
			{//jesli nie to pozbywany sie skladowej
				if(c_menage[1]) delete colors[1];
                colors[1]=nullptr;
				c_menage[1]=0;
			}
		}
        if(colors[2]!=nullptr)
		{
			geometry_base* ColGeom=colors[2]->getgeometry();
			if(*MyGeometry!=*ColGeom)
			{//jesli nie to pozbywany sie skladowej
				if(c_menage[2]) delete colors[2];
                colors[2]=nullptr;
				c_menage[2]=0;
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


int true_color_manhattan_graph::setseries(size_t index,data_source_base* idata,int imenage)
//zwraca -1 jesli indeks za duzy
{
	if(index>4) return -1;//Tylko dwie serie
    assert(idata!=nullptr);
	if(index==0)
	{
		if(d_menage) delete datas;
		datas=idata;
		d_menage=imenage;
	}
	else
	{
		if(c_menage[index-1]) delete colors[index-1];
		colors[index-1]=idata;
		c_menage[index-1]=imenage;
	}
	return 0;
}

data_source_base* true_color_manhattan_graph::getseries(size_t index)
//zwraca nullptr jesli indeks za duzy
{
	if(index>4)
        return nullptr;
	if(index==0)
		return datas;
	else	
		return colors[index-1];
}


void true_color_manhattan_graph::_replot()// Rysuje wlasciwy wykres a pod nim ewentualnie legende
{
	int x1=getstartx();
	int y1=getstarty();
	int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
	int y2=y1+getheight()-1;
	double min,max,minc[3],maxc[3];
	double miss,missingc[3];
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
    if(colors[0]!=nullptr)
	{
		size_t num_color;
		colors[0]->bounds(num_color,minc[0],maxc[0]);
		s_colo[0].set(minc[0],maxc[0],c_range.end-c_range.start+0.999);
		missingc[0]=colors[0]->get_missing();
	}
    if(colors[1]!=nullptr)
	{
		size_t num_color;
		colors[1]->bounds(num_color,minc[1],maxc[1]);
		s_colo[1].set(minc[1],maxc[1],c_range.end-c_range.start+0.999);
		missingc[1]=colors[1]->get_missing();
	}
    if(colors[2]!=nullptr)
	{
		size_t num_color;
		colors[2]->bounds(num_color,minc[2],maxc[2]);
		s_colo[2].set(minc[2],maxc[2],c_range.end-c_range.start+0.999);
		missingc[2]=colors[2]->get_missing();
	}

	//Wypisywanie legendy dla 1 serii
	if(t_colors.start!=getbackground())
	{
		print_width(x1,y1,((x2-x1)/5)*4,t_colors.start,getbackground(),"%g",max);
		const char* pom=datas->name();
		print_width(x1,y2-char_height('0'),((x2-x1)/4),t_colors.start,getbackground(),
			"%g %*s    ",min,strlen(pom)>60?60:strlen(pom),pom);
		flaga=1;
	}

	//...i dla składowych koloru
	{int width,x=(x2-x1)/4,y=y2-char_height('0');
    if(colors[0]!=nullptr)
	{
		width=print_width(x,y,(x2-x)/3*2,50,getbackground(),//50 to czerwone z palety
			"%s",colors[0]->name());

		if(width==0) goto RYSOWANIE;
		else	 x+=width+1;

		width=print_width(x,y,(x2-x)/2,50,
			c_range.start!=getbackground()?getbackground():c_range.end,
			"<%g",minc[0]);

		if(width==0) goto RYSOWANIE;
		else	x+=width+1;

		width=print_width(x,y,x2-x,255,//Biale na czerwonym z palety
			50,
			",%g>",maxc[0]);

		if(width==0) goto RYSOWANIE;
		else	x+=width+5;
	}
    if(colors[1]!=nullptr)
	{
		width=print_width(x,y,(x2-x)/3*2,220,getbackground(),//220 to zielone z palety
			"%s",colors[1]->name());

		if(width==0) goto RYSOWANIE;
		else	 x+=width+1;

		width=print_width(x,y,(x2-x)/2,220,
			c_range.start!=getbackground()?getbackground():c_range.end,
			"<%g",minc[1]);

		if(width==0) goto RYSOWANIE;
		else	x+=width+1;

		width=print_width(x,y,x2-x,255,//Biale na zielonym z palety
			220,
			",%g>",maxc[1]);

		if(width==0) goto RYSOWANIE;
		else	x+=width+5;
	}
    if(colors[2]!=nullptr)
	{
		width=print_width(x,y,(x2-x)/3*2,128,getbackground(),//128 to niebieski z palety
			"%s",colors[2]->name());

		if(width==0) goto RYSOWANIE;
		else	 x+=width+1;

		width=print_width(x,y,(x2-x)/2,128,
			c_range.start!=getbackground()?getbackground():c_range.end,
			"<%g",minc[2]);

		if(width==0) goto RYSOWANIE;
		else	x+=width+1;

		width=print_width(x,y,x2-x,255,//Biale na niebieskim z palety
			128,
			",%g>",maxc[2]);

		if(width==0) goto RYSOWANIE;
		else	x+=width+5;
	}
	}
RYSOWANIE:
	//Zawerzenie po y-grekach
	if(flaga)
	{
		y1+=char_height('0');
		y2-=char_height('0');
	}
	flaga=0;
	x2-=3;
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

		for(unsigned Bpos=0;Bpos<B;Bpos++)//W ktorym wierszu
			for(unsigned Apos=0;Apos<A;Apos++)//W ktorej kolumnie
			{
				double test;
				int X,Y,Y2;		  //Robocze pozycje	
				double r[4]={-1,-1,-1,-1};
                long   a[4]={0,0,0,0};
				size_t Gind=MyGeometry->get_next(h);//czyta index
				r[0]=test=datas->get(Gind);//i czyta wartosc

				if(test==miss)//Jesli missing
					continue; //Nie rysuj

                if(colors[0]!=nullptr)
				{
					r[1]=test=colors[0]->get(Gind);
					if(test==missingc[1])
						goto NIE_DA_SIE;//Awaria - missingi muza sie pokrywac
				}
                if(colors[1]!=nullptr)
				{
					r[2]=test=colors[1]->get(Gind);
					if(test==missingc[1])
						goto NIE_DA_SIE;//Awaria - missingi muza sie pokrywac
				}
                if(colors[2]!=nullptr)
				{
					r[3]=test=colors[2]->get(Gind);
					if(test==missingc[2])
						goto NIE_DA_SIE;//Awaria - missingi muza sie pokrywac
				}

				_rescale_data_point(r,a);

				X=x2-(Bpos+1)*offsetH-(A-Apos)*gruboscH;
				Y=zero-(B-(1+Bpos))*gruboscV;
				Y2=y2-(B-(1+Bpos))*gruboscV-a[0];//Wysokosc od min

				if(Y2>Y)
					::swap(Y,Y2);

				//Slupek 3D RGB
				bar3dRGB(X,Y,Y-Y2,a[1],a[2],a[3],2);
			}	

			MyGeometry->destroy_iterator(h);
	}
	else
	{
		printc(x1,(y1+y2)/2,t_colors.start,getbackground(),"Too small area");
	}

	return;
NIE_DA_SIE:
	print_width(x1,(y1+y2)/2,x2-x1,t_colors.start,getbackground(),"%@CInvalid colors found. Probably");
	print_width(x1,(y1+y2)/2+char_height('X'),x2-x1,t_colors.start,getbackground(),"%@C min or max is not set properly");
}

// reals[] zawiera  wysokosc slupka i wartosc ustalajaca kolor, albo -1
int true_color_manhattan_graph::_rescale_data_point(const double reals[],long in_area[])
//zwraca -1 jesli nie w oknie
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



/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/



