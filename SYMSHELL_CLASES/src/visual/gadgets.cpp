// IMPLEMENTACJA ROZNYCH TYPOW OBSZAROW NIE BEDACYCH GRAFAMI
//==============================================================================
// Wersja z kosmetyką XI 2012
//*//////////////////////////////////////////////////////////////////////////////
#include "wb_clone.hpp"
#include "symshell.h"
#include "drawable.hpp"
#include "gadgets.hpp"
#include "textarea.hpp"

//void rect(int x1,int y1,int x2,int y2,unsigned int frame_c);

template<class T>
static inline void swap(T& a,T& b)
{
T c=a;a=b;b=c;
}

int gadget::on_click(int x,int y,int click)
//Jesli "inside" to rysuje w inwersji, ale zwraca 1.
{
int ins=is_inside(x,y);
if(ins==1)
	{
	draw_color=setbackground(draw_color);//Na draw_color stara wartosc background
	replot();
	draw_color=setbackground(draw_color);//i z powrotem
	ins=_on_click(x,y,click);//Moze zmienic wynik
	}
return ins;
}

void empty_area::_replot()
//Rysuje przekreslenie obszaru uzytkownika
{
int x1=getstartx();
int y1=getstarty();
int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
int y2=y1+getheight()-1;
line(x1,y1,x2,y2,draw_color);
line(x1,y2,x2,y1,draw_color);
}

void button::_replot()
{
int x1=getstartx();
int y1=getstarty();
int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
int y2=y1+getheight()-1;
int xr=x2-3;
int yr=y2-3;

if(xr>x1 && yr>y1 )
	{
	line(x1,yr,xr,yr,getframe());
	line(xr,y1,xr,yr,getframe());
	x2=xr;
	y2=yr;
	}

const char* text=name();//Podreczny wskazniczek
if(vhmode==0) //Vertical mode
	{
	if(x2-x1<char_width('X')) return;//Za malo miejsca
	int height=char_height('X');
	while(*text!='\0' && (y2-y1)>=height)
		{
		printc(x1,y1,draw_color,getbackground(),"%c",*text);
		y1+=height;
		text++;
		}
	return;//Wykonane	
	}
if(vhmode==1)
	{
	int height=string_height(text);
	if(y2-y1<height) return;//Za malo miejsca
	int ystart=(y1+y2)/2-height/2;//Srodek
	if(ystart<y1) ystart=y1;//jak wylezie za wysoko
	print_width(x1,ystart,x2-x1,draw_color,getbackground(),text);//samo rysowanie
	return;
	}	
}


void arrow_button::_replot()
{
int x1=getstartx();
int y1=getstarty();
int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
int y2=y1+getheight()-1;
int r1=x2-x1;
int r2=y2-y1;
wb_color bck=this->getbackground();
if(bck==0)
  bck=256;//W palecie 512
if(draw_color==0)
  draw_color=256;
if(bck==255)
  bck=511;//W palecie 512
if(draw_color==255)
  draw_color=511;
int	  ow=line_width(mode==0?2:0);		    /* Ustala szerokosc lini - moze byc kosztowne. Zwraca stan poprzedni */
int	  os=line_style(SSH_LINE_SOLID); /* Ustala styl rysowania lini: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED */
switch(mode){
/*
	//int ow=line_width(2);
	case 0:
	line(x1,(y1+y2)/2,x2,(y1+y2)/2,draw_color);
	line((x1+x2)/2,y2,(x1+x2)/2,y1,draw_color);
	if(r1>r2) r1=r2;
	circle((x1+x2)/2,(y1+y2)/2,r1/2,draw_color);
	circle((x1+x2)/2,(y1+y2)/2,r1/4,draw_color);
	break;
	case 1 :// print up arrow
	line((x1+x2)/2,y2,(x1+x2)/2,y1,draw_color);
	line((x1+x2)/2,y1,(x1+x2)/2-r1/4,y2-r2/2,draw_color);
	line((x1+x2)/2,y1,(x1+x2)/2+r1/4,y2-r2/2,draw_color);
	break;
	case 2 :// print left arrow
	line(x1,(y1+y2)/2,x2,(y1+y2)/2,draw_color);
	line(x1,(y1+y2)/2,x2-r1/2,(y1+y2)/2-r2/4,draw_color);
	line(x1,(y1+y2)/2,x2-r1/2,(y1+y2)/2+r2/4,draw_color);
	break;
	case 3 :// print down arrow
	line((x1+x2)/2,y2,(x1+x2)/2,y1,draw_color);
	line((x1+x2)/2,y2,(x1+x2)/2-r1/4,y1+r2/2,draw_color);
	line((x1+x2)/2,y2,(x1+x2)/2+r1/4,y1+r2/2,draw_color);
	break;
	case 4 ://print right arrow
	line(x1,(y1+y2)/2,x2,(y1+y2)/2,draw_color);
	line(x2,(y1+y2)/2,x2-r1/2,(y1+y2)/2-r2/4,draw_color);
	line(x2,(y1+y2)/2,x2-r1/2,(y1+y2)/2+r2/4,draw_color);
	break;
*/
	case 0:
	line(x1-1,(y1+y2)/2,x2,(y1+y2)/2,draw_color);
	line((x1+x2)/2,y2,(x1+x2)/2,y1,draw_color);
	if(r1>r2) r1=r2;
	//circle((x1+x2)/2,(y1+y2)/2,r1/2,draw_color);
	//circle((x1+x2)/2,(y1+y2)/2,r1/4,draw_color);
	for(int y=(y1+y2)/2,i=1,end=unsigned(r1*0.2);i<=end;i++)
	{
		wb_color curcol=(bck<draw_color?int(draw_color-(double(i)/end)*255):int(draw_color+(double(i)/end)*255));
		circle((x1+x2)/2,(y1+y2)/2,r1/2-i,curcol);
	}
	break;
	case 1 :// print up arrow
	line((x1+x2)/2,y2-2,(x1+x2)/2,y1,draw_color);
	for(int y=(y1+y2)/2,i=1,end=unsigned(r1*0.4);i<=end;i++)
	{
		wb_color curcol=(bck<draw_color?int(draw_color-(double(i)/end)*255):int(draw_color+(double(i)/end)*255));
		line((x1+x2)/2,y1,(x1+x2)/2-i,y2-r2/2,curcol);
		line((x1+x2)/2,y1,(x1+x2)/2+i,y2-r2/2,curcol);
		line((x1+x2)/2-i/2,y2-2,(x1+x2)/2-i/2,y1+r2/2,curcol);
		line((x1+x2)/2+i/2,y2-2,(x1+x2)/2+i/2,y1+r2/2,curcol);
	}
	break;
	case 2 :// print left arrow
	line(x1,(y1+y2)/2,x2-2,(y1+y2)/2,draw_color);
	for(int y=(y1+y2)/2,i=1,end=unsigned(r2*0.45);i<=end;i++)
	{
		wb_color curcol=(bck<draw_color?int(draw_color-(double(i)/end)*255):int(draw_color+(double(i)/end)*255));
		line(x1,y,x2-r1/2,y-i,curcol);
		line(x1,y,x2-r1/2,y+i,curcol);
		line(x2-r1/2,y-i/2,x2-2,y-i/2,curcol);
		line(x2-r1/2,y+i/2,x2-2,y+i/2,curcol);
	}
	break;
	case 3 :// print down arrow
	line((x1+x2)/2,y2,(x1+x2)/2,y1+2,draw_color);
	for(int y=(y1+y2)/2,i=1,end=unsigned(r1*0.4);i<=end;i++)
	{
		wb_color curcol=(bck<draw_color?int(draw_color-(double(i)/end)*255):int(draw_color+(double(i)/end)*255));
		line((x1+x2)/2,y2,(x1+x2)/2-i,y1+r2/2,curcol);
		line((x1+x2)/2,y2,(x1+x2)/2+i,y1+r2/2,curcol);
		line((x1+x2)/2-i/2,y2-r2/2,(x1+x2)/2-i/2,y1+2,curcol);
		line((x1+x2)/2+i/2,y2-r2/2,(x1+x2)/2+i/2,y1+2,curcol);
	}
	break;
	case 4 ://print right arrow
	line(x1+2,(y1+y2)/2,x2,(y1+y2)/2,draw_color);
	for(int y=(y1+y2)/2,i=1,end=unsigned(r2*0.45);i<=end;i++)
	{
		wb_color curcol=(bck<draw_color?int(draw_color-(double(i)/end)*255):int(draw_color+(double(i)/end)*255));
		line(x2,y,x2-r1/2,y-i,curcol);
		line(x2,y,x2-r1/2,y+i,curcol);
		line(x1+2,y-i/2,x2-r1/2,y-i/2,curcol);
		line(x1+2,y+i/2,x2-r1/2,y+i/2,curcol);
	}
	break;
	default:
	break;
	}	
line_width(ow);
line_style(os);
}


leftrigt_button::leftrigt_button(int ix1,int iy1,int ix2,int iy2):
	empty_area(ix1,iy1,ix2,iy2,::background(),::background(),::background())
{
int a=(ix1+ix2)/2;
int b=char_height('X');
left	=new arrow_button(ix1+1	,iy1+b,a-1,	iy2-1,		2/*mode left*/	);
right	=new arrow_button(a+1,iy1+b,ix2-1,iy2-1,		4/*mode right*/);
settitle("Left-Right control");
}

void leftrigt_button::_replot()
//Odrysowuje skladowe
{
//	empty_area::_replot();//Niepotrzebne - tylko przekreslenie
	left->replot(0);
	right->replot(0);
}

steering_wheel::steering_wheel( int ix1,int iy1,int ix2,int iy2,
							   wb_dynarray<rectangle_source_base*>&  idat)://Pamiec zadnej z seri nie jest tu zarzadzana										
empty_area(ix1,iy1,ix2,iy2,::background(),::background(),::background())
{
data.alloc(idat.get_size());
//Przepisuje bo nie wiadomo co to za tablica
for(size_t i=0;i<idat.get_size();i++)
				data[i]=idat[i];						
assert(data.IsOK());
//USTAWIANIE
int a=ix2-ix1;
int b=iy2-iy1;
resizing=new arrow_button(ix1+a/4,	 iy1+1,	   ix1+a/4*3,	iy1+b/2,	0/*mode target*/);
up		=new arrow_button(ix1+1,	 iy1+b/2+1,ix2-a/2,	iy2-b/4,	1/*mode up*/);
down	=new arrow_button(ix1+a/2+1,iy1+b/2+1,ix2,		iy2-b/4,	3/*mode down*/);
left	=new arrow_button(ix1+1	,iy2-b/4+1,ix2-a/2,	iy2,		2/*mode left*/	);
right	=new arrow_button(ix1+a/2+1,iy2-b/4+1,ix2,		iy2,		4/*mode right*/);
settitle("STEERING WHEELL");
}


steering_wheel::steering_wheel( 
					rectangle_source_base*     idat,//Pamiec seri nigdy nie jest tu zarzadzana
					wb_ptr<drawable_base> ires,//Pamiec dla pod-obszarow jest
					wb_ptr<drawable_base> iup,// z a w s z e
					wb_ptr<drawable_base> ileft,  //zarządzana.
					wb_ptr<drawable_base> idown,//Wspolrzedne obszaru steeering wheel sa
					wb_ptr<drawable_base> iright //ustalane z wspolrzednych jego skladowych.
					):			//Tlo jest takie jakie dla calego okna!
        empty_area(0,0,0,0,::background(),::background(),::background()),
        data(1,idat,nullptr), //nullptr dla zakończenia listy i ewentualnego odróżnienia konstruktorów
		resizing(ires),left(ileft),right(iright),up(iup),down(idown)
{
assert(data.IsOK()	);
//LADOWANIE WSPOLRZEDNYCH
load(*resizing);
add(*up);
add(*left);
add(*down);
add(*right);
settitle("STEERING WHEELL");
}

steering_wheel::steering_wheel(
					wb_dynarray<rectangle_source_base*>&  idat,//Pamiec zadnej z seri nie jest tu zarzadzana													
					wb_ptr<drawable_base> ires,//Pamiec dla pod-obszarow jest
					wb_ptr<drawable_base> iup,// z a w s z e
					wb_ptr<drawable_base> ileft,  //zarządzana.
					wb_ptr<drawable_base> idown,//Wspolrzedne obszaru steeering wheel sa
					wb_ptr<drawable_base> iright //ustalane z wspolrzednych jego skladowych.
					):			//Tlo jest takie jakie dla calego okna!
        empty_area(0,0,0,0,::background(),::background(),::background()),
		//data(idat),
		resizing(ires),left(ileft),right(iright),up(iup),down(idown)
{
data.alloc(idat.get_size());
//Przepisuje bo nie wiadomo co to za tablica
for(size_t i=0;i<idat.get_size();i++)
				data[i]=idat[i];						
assert(data.IsOK());
//LADOWANIE WSPOLRZEDNYCH
load(*resizing);
add(*up);
add(*left);
add(*down);
add(*right);
settitle("STEERING WHEELL");
}

//steering_wheel( int ix1,int iy1,int ix2,int iy2,
//					rectangle_source_base*  idat);//Pamiec zadnej z seri nie jest tu zarzadzana										


int leftrigt_button::on_change(const gps_area& new_ar)
//Musi przesunac wspolrzedne skladowych razem ze swoimi.
{
	float trans[6];
	get_transform_to(new_ar,trans);
	gps_area check(*this);
	check.transform(trans);
	if(!(check==new_ar))
	{
	//fprintf(stderr,"leftrigt_button warning:rescaling will loss precission.\n");
	}
	
	left->transform(trans);
	right->transform(trans);

	return 1;
}

int steering_wheel::on_change(const gps_area& new_ar)
{
	float trans[6];
	get_transform_to(new_ar,trans);

	gps_area check(*this);
	check.transform(trans);
	if(!(check==new_ar))
	{
		//fprintf(stderr,"Steering_wheel warning:rescaling will loss precission.\n");
	}

	resizing->transform(trans);
	up->transform(trans);
	left->transform(trans);
	down->transform(trans);
	right->transform(trans);

	return 1;
}

void steering_wheel::_replot()
//Odrysowuje skladowe
{
resizing->replot(0);
up->replot(0);
left->replot(0);
down->replot(0);
right->replot(0);
}

int leftrigt_button::_user_action(int leftorright,int /*ingnoruje click*/)
/*Jeśli lewo to -1, a jesli prawo to 1*/
{
	return 2;//Obsluzone, choć nie zrobione
}


int leftrigt_button::on_click(int x,int y,int click)
		//Przepytuje skladowe i jesli ktoras zostala trafiona to ... 
{
	if(!is_inside(x,y))//sprawdzenie dla calosci
		return 0;  //"Nie moja sprawa!"
	
	int retval=0;

	if(left->on_click(x,y,click))
	{	
	  retval=_user_action(-1,click);
	}
	else
	if(right->on_click(x,y,click))
	{
      retval=_user_action(1,click);
	}
	else
	{
		//Gdzieś w tytuł lub tło
		//replot();
		retval=1;
	}
	
	if(retval!=1)
	{
		delay_ms(128);
		set_char('\r');//Wymuszenie replotu w następnym nawrocie pętli czytającej
	}

	return retval;
}

int steering_wheel::on_click(int x,int y,int click)
		//Przepytuje skladowe i jesli ktoras zostala trafiona to 
		//adekwatnie zmienia serie za pomoca metody sub()
		//oraz wymusza odnowienie ekranu
{
	if(!is_inside(x,y))//sprawdzenie dla calosci
		return 0;  //"Nie moja sprawa!"

	int dim=0;	
	int Mnoznik=1;
	geometry::view_info subtab;//Struktura na informacje o kamerze	
	geometry::MD_info	md;
	geometry_base* geom=data[0]->getgeometry();

    if(geom==nullptr) goto ERROR;
	dim=geom->get_dimension();
	if(dim<2) goto ERROR;
    if(geom->get_view_info(&subtab)==nullptr)
							goto ERROR;//Wypelnianie struktury informacja o "kamerze"	
    if(geom->get_info(&md)==nullptr)
							goto ERROR;//Wypelnianie struktury informacja
	if(subtab.dia.X()>20*subtab.sst.X() &&
				subtab.dia.Y()>20*subtab.sst.Y() )			
					Mnoznik=10;
	if(up->on_click(x,y,click)==1)
	{
		subtab.pos.Y()-=subtab.sst.Y()*Mnoznik;
	}
	else if(down->on_click(x,y,click)==1)
	{
		subtab.pos.Y()+=subtab.sst.Y()*Mnoznik;
	}
	else if(left->on_click(x,y,click)==1)
	{
		subtab.pos.X()-=subtab.sst.X()*Mnoznik;
	}
	else if(right->on_click(x,y,click)==1)
	{
		subtab.pos.X()+=subtab.sst.X()*Mnoznik;
	}
	else if(resizing->on_click(x,y,click)==1)
	{	
		if(click==1)
			{
			//Przy malych zakresach co 1, przy duzych dwukrotnie
			if(subtab.dia.X()<10*subtab.sst.X() &&
				subtab.dia.Y()<10*subtab.sst.Y() )
				{
				subtab.dia.X()-=subtab.sst.X();
				subtab.dia.Y()-=subtab.sst.Y();
				}
				else
				{
				subtab.dia.X()/=2;
				subtab.dia.Y()/=2;					
				}
			}
			else if(click==2)
			{
			if(subtab.dia.X()<15*subtab.sst.X() &&
				subtab.dia.Y()<15*subtab.sst.Y() )
				{			
				subtab.dia.X()+=subtab.sst.X();
				subtab.dia.Y()+=subtab.sst.Y();
				}
				else
				{			
				subtab.dia.X()*=2;
				if(subtab.dia.X()*2>md.max.X()-md.min.X())
						subtab.dia.X()=(md.max.X()-md.min.X())/2;
				subtab.dia.Y()*=2;
				if(subtab.dia.Y()*2>md.max.Y()-md.min.Y())
						subtab.dia.Y()=(md.max.Y()-md.min.Y())/2;
				}
			}
	/*
		else
		{
            subtab.dispose();//Need be nullptr for reseting
		}
	*/	
	}
	else //Zaden z podobiektów
	return is_inside(x,y);//Wychodzi sie.		
		
//Ustawia tak samo pozostalym zarzadzanym seriom
size_t i;
for(i=1;i<data.get_size();i++)
	{		
	geometry_base* sec_geom=data[i]->getgeometry();
    if(sec_geom==nullptr)
				continue;
	if(*geom!=*sec_geom)
				continue;
	if(sec_geom->set_view_info(&subtab)==-1) 
					goto ERROR;
	}
//I ustawia seri podstawowej
geom->set_view_info(&subtab);

set_char('\r');//Informacja ze trzeba odrysowac - prowizoryczna!!!
return 2;
ERROR://Niezaimplemtowano koniecznej operacja lub inny blad
{
int bf=getframe();//Uzyte jako tymczas
setframe(254);//Jasny ale nie bialy
replot();
setframe(bf);
}
return 0;//NIe przyznaje sie do punktu
}

/*
int text_area::on_click(int x,int y,int click)
//Jesli "inside" to rysuje w inwersji, ale zwraca 1.
{
int ins=is_inside(x,y);
if(ins==1)
	{
	ins=_on_click(x,y,click);//Moze zmienic wynik
	}
return ins;
}
*/

text_area::text_area(int ix1,int iy1,int ix2,int iy2,
			const char* itext,		//TEXT inicjujacy
			unsigned icolor,//=default_black,
			unsigned ibackground,//=default_white,
			unsigned iframe,//=128,
			size_t   buffsize//=-1	//Inicjalny rozmiar bufora.
			):
			drawable_base(ix1,iy1,ix2,iy2,ibackground,iframe),
			curr_col(icolor),
			user_size(buffsize),
			index(0)
		{
                if(user_size!=size_t(-1))//Tylko jak user_size już ustalony
			linie.alloc(user_size);
		add_text(itext);
		}


void text_area::_replot()
{
int x1=getstartx();
int y1=getstarty();
int x2=x1+getwidth()-1;//-1 bo width obejmuje pierwszy pixel
int y2=y1+getheight()-1;
int r1=x2-x1;
int r2=y2-y1;
int start=y1+index*char_height('X');
if(start>y2) 
		start=y2;
for(int i=index-1;i>=0;i--)
	{
	start-=char_height('X');
	if(start>=y1)
		print_width(x1,start,x2-x1,curr_col,getbackground(),linie[i].get_ptr_val());
	}
}

void text_area::clean()
{
linie.dispose();//Usuwa stare
index=0;		//Umozliwia alokowanie lini od poczatku
if(user_size!=size_t(-1))//Tylko gdy user_size ustalone
	linie.alloc(user_size);// Alokuje nowe
}

int text_area::add_line(const char* ini)//ret 1 jesli OK
{
wb_dynarray<char> Ini;
if(ini==nullptr)
	return 0;
char* pom=clone_str(ini);//Kopia lancucha
if(pom!=nullptr)
	Ini.set_dynamic_ptr_val(pom,strlen(pom)+1);
	else
	return 0;//Wpadka na braku pamieci

if(index<linie.get_size())
	//Jest jeszcze miejsce. Czyli tablica niezerowego rozmiar
	{
	linie[index]=Ini;//Jesli przepisze to nie zwolni
	index++;
	}
	else
	{
	linie[0].dispose();
	size_t len=linie.get_size();
	if(len>=2)
		{			
		len=(len-1)*sizeof(linie[0]);
		memmove(linie.get_ptr_val(),
			linie.get_ptr_val(1),
			len);
		}
	size_t outsize;
	linie[index-1].give_dynamic_ptr_val(outsize);//Zabiera mu z zarzadu
	linie[index-1]=Ini;//Jesli przepisze to nie zwolni
	}

return 1;
}


int text_area::add_text(const char* ini)//ret 1 jesli OK
{
wb_dynarray<char> Ini;
char* pom=nullptr;

if(ini!=nullptr && (pom=clone_str(ini))==nullptr)
	return 0;

//Bradziej elegancki uchwyt do kopi lancucha zrodlowego
//usuwanej w momencie wyjscia z metody.
Ini.set_dynamic_ptr_val(pom,strlen(pom)+1);

if(linie.get_size()==0)//Trzeba zaalokowac
	{
	size_t licznik=0;
	char* iter=pom;
	while(*iter!='\0')
		{
		if(*iter=='\n') licznik++;
		iter++;
		}	
	linie.alloc(licznik+1);//Jesli nie ma koncow linii to i tak jest jedna
	//cerr<<"lini: "<<licznik<<"in text:"<<endl<<ini<<endl;
	}
	
char* iter=pom;
bool flaga=0;//Czy koniec petli
do{
  if(	*iter=='\n'
	 || (flaga=(*iter=='\0'))
	 )
		{
		*iter='\0';//Koniec poprzedniej lini		
		if(add_line(pom)!=1)
			return 0;
		pom=iter+1;//Poczatek nastepnej lini
		}
	iter++;
   }while(!flaga);

return 1;
}





/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://www.iss.uw.edu.pl/borkowski                  */
/*        MAIL: wborkowski@uw.edu.pl                                */
/*                               (Don't change or remove this note) */
/********************************************************************/


