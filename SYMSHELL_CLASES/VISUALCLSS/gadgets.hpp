//				POMOCNICZE TYPY OBSZAROW
//--------------------------------------------------------------------
#ifndef __GADGETS_HPP__
#define __GADGETS_HPP__
#ifndef __cplusplus
#error C++ required
#endif

#include "drawable.hpp"
#include "datasour.hpp"

// Sluza glownie jako typy bazowe dla klas specjalizowanych, wykonujacych
// jakies akcje w metodzie is_inside() i ewentualnie podobnych.
class gadget:public drawable_base
//-------------------------------------
{
protected:
wb_color draw_color;
public:
	gadget(int ix1,int iy1,int ix2,int iy2,
				wb_color icolor,
				wb_color ibkg=default_white,
				wb_color ifr=default_white):
			drawable_base(ix1,iy1,ix2,iy2,ibkg,ifr),draw_color(icolor)
				{}
virtual
int  on_click(int x,int y,int click=0); //Jesli "inside" to rysuje w inwersji, ale zwraca wynik _on_click() lub 0
virtual
int _on_click(int /*x*/,int /*y*/,int /*click*/)//Prawdziwa akcja klasy potomnej jesli w srodku tej metody
			{return 1;}							//Domyslnie zwraca 1=="nie obsluzono"
virtual
void _replot()=0;//Rysuje - np X. Do reimplementacji w klasach potomnych
};

class sensitive_area:public gadget
//--------------------------------------------
{
public:
	sensitive_area(int ix1,int iy1,int ix2,int iy2,
				unsigned icolor,
				unsigned ibkg=default_white,
				unsigned ifr=default_transparent):
			gadget(ix1,iy1,ix2,iy2,icolor,ibkg,ifr)
				{}

//int _on_click(int /*x*/,int /*y*/,int /*click*/)//Prawdziwa akcja klasy potomnej powinna byc w srodku tej metody
//			{return 2;}							  //i zwracac 2 jesli "obsluzono".
virtual
void _replot();//Rysuje - np X. Do reimplementacji w klasach potomnych
};

typedef sensitive_area empty_area;


class button:public sensitive_area
//---------------------------------------------
{
protected:
unsigned vhmode:1;// vert(0)/hor(1) - tryb wyswietlania
unsigned reserved:15;//conajmniej 15 bitow i tak bedzie zajete
public:
	button(int ix1,int iy1,int ix2,int iy2,
			const char* iprompt,
			unsigned imode=1, //1 -> horizontal print mode
						//0 -> vertical print mode
			unsigned icolor=default_black,
			unsigned ibackground=default_white,
			unsigned iframe=128):		
		sensitive_area(ix1,iy1,ix2,iy2,icolor,ibackground,iframe),vhmode(imode)
		{
		settitle(iprompt);//Prompt jest pamietany jako tytul okna, ale wyswietlany recznie
		settitlecolor(default_transparent);//Wiec domyslnie
		settitleback(default_transparent);//jest niewidoczny
		}

//int _on_click(int /*x*/,int /*y*/,int /*click*/)//Prawdziwa akcja klasy potomnej powinna byc w srodku tej metody
//			{return 2;}							  //i zwracac 2 jesli "obsluzono".
virtual		
void _replot();
};

class arrow_button:public sensitive_area
//---------------------------------------------
{
protected:
int mode;//tryb wyswietlania
public:
	arrow_button(int ix1,int iy1,int ix2,int iy2,
			int imode=0,// 0 - print target (-(+)-)
					   // 1 - print up arrow	
					   // 2 - print left arrow
					   // 3 - print down arrow
					   // 4 - print right arrow
			unsigned icolor=default_black,
			unsigned ibackground=default_white,
			unsigned iframe=default_transparent):
		sensitive_area(ix1,iy1,ix2,iy2,icolor,ibackground,iframe),
		mode(imode)
		{}

//int _on_click(int /*x*/,int /*y*/,int /*click*/)//Prawdziwa akcja klasy potomnej powinna byc w srodku tej metody
//			{return 2;}							  //i zwracac 2 jesli "obsluzono".

void _replot();
};

class leftrigt_button:public sensitive_area
//==============================================
{
protected:
wb_ptr<drawable_base> left;
wb_ptr<drawable_base> right;
public:
	leftrigt_button(int ix1,int iy1,int ix2,int iy2);
	int on_change(const gps_area&);//Musi przesunac wspolrzedne skladowych razem ze swoimi.
	void _replot(); //Odrysowuje skladowe
virtual int _user_action(int leftorright/*Jeœli lewo to -1, a jesli prawo to 1*/,int click/* mo¿e mieæ znaczenie który przycisk*/);
	int on_click(int x,int y,int click=0);
		//Przepytuje skladowe i jesli ktoras zostala trafiona to 
		//wywo³uje _user_action() z odpowiednim parametrem
};

class steering_wheel:public sensitive_area
//----------------------------------------
{
//wskaznik do danych
wb_dynarray<rectangle_source_base*> data;
//wskazniki do elementow sterowania 
wb_ptr<drawable_base> resizing;
wb_ptr<drawable_base> left;
wb_ptr<drawable_base> up;
wb_ptr<drawable_base> right;
wb_ptr<drawable_base> down;
public:
	steering_wheel( rectangle_source_base*     idat,//Pamiec seri nigdy nie jest tu zarzadzana
					wb_ptr<drawable_base> ires,//Pamiec dla pod-obszarow jest
					wb_ptr<drawable_base> iup, // z a w s z e
					wb_ptr<drawable_base> ileft,//zarz¹dzana.
					wb_ptr<drawable_base> idown,//Wspolrzedne obszaru steeering wheel sa
					wb_ptr<drawable_base> iright //ustalane z wspolrzednych jego skladowych.
					);			//Tlo jest takie jakie dla calego okna!
	steering_wheel( wb_dynarray<rectangle_source_base*>&  idat,//Pamiec zadnej z seri nie jest tu zarzadzana										
					wb_ptr<drawable_base> ires,//Pamiec dla pod-obszarow jest
					wb_ptr<drawable_base> iup,// z a w s z e
					wb_ptr<drawable_base> ileft,  //zarz¹dzana.
					wb_ptr<drawable_base> idown,//Wspolrzedne obszaru steeering wheel sa
					wb_ptr<drawable_base> iright //ustalane z wspolrzednych jego skladowych.
					);			//Tlo jest takie jakie dla calego okna!
	steering_wheel( int ix1,int iy1,int ix2,int iy2,
					rectangle_source_base*  idat);//Pamiec zadnej z seri nie jest tu zarzadzana										
	steering_wheel( int ix1,int iy1,int ix2,int iy2,
					wb_dynarray<rectangle_source_base*>&  idat);//Pamiec zadnej z seri nie jest tu zarzadzana										
int on_change(const gps_area&);//Musi przesunac wspolrzedne skladowych razem ze swoimi.
void _replot(); //Odrysowuje skladowe
int on_click(int x,int y,int click=0);
		//Przepytuje skladowe i jesli ktoras zostala trafiona to 
		//adekwatnie zmienia serie za pomoca metody sub()
		//oraz wymusza odnowienie ekranu
};

template<class NUMBER>
class knob_for_value:public leftrigt_button
{
protected:
	NUMBER min,max,step;
	NUMBER* valptr;
public:
	knob_for_value(int ix1,int iy1,int ix2,int iy2,const char* Title="knob",NUMBER* valptr=NULL,NUMBER min=0,NUMBER max=100,double iprop=0.01):
		leftrigt_button(ix1,iy1,ix2,iy2)
		{										assert(min<max);
			settitle(Title);
			this->valptr=valptr;
			this->max=max;
			this->min=min;
			set_step_proportionaly(iprop);
		}

	int _user_action(int leftorright/*Jeœli lewo to -1, a jesli prawo to 1*/,int click/* mo¿e mieæ znaczenie który przycisk*/)
	{
		if(valptr==NULL) 
			return 0;

		if(leftorright>0)
		{
		   if(click==1)
			*valptr+=step;
			else if(click==2)
				*valptr+=step/10;

			if(*valptr>max)
				*valptr=max;
		}
		else
		if(leftorright<0)
		{
		   if(click==1)
			*valptr-=step;
			else if(click==2)
				*valptr-=step/10;
			if(*valptr<min)
				*valptr=min;
		}

		return 2;//Nie 1!!!
	}

	NUMBER* get_ptr() { return valptr;}

virtual void change_ptr(NUMBER* ivalptr)
	{
		valptr=ivalptr;
		if(*valptr<min || max<*valptr)
			*valptr=(min+max)/2;
	}

virtual void set_step(NUMBER istep)
	{
		if((max-min)<istep)
			step=istep;
	}

virtual void set_step_proportionaly(double proportion)
	{
		if(0<proportion && proportion<=0.5)//Conajmniej 2 kroki!!!
			step=(max-min)*proportion;
	}
};

#endif
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/

