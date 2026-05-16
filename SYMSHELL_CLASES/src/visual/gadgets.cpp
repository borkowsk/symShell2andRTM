/// @file
/// @brief **IMPLEMENTATION OF DIFFERENT TYPES OF NON-GRAPH AREAS** /<br>
///         _IMPLEMENTACJA ROŻNYCH TYPóW OBSZARóW NIE BĘDĄCYCH GRAFAMI._
/// @date 2026-05-16 (last modification)
//*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wersja z kosmetyką XI 2012 i późniejszymi zmianami.

#include "wb_clone.hpp"
#include "symshell.h"
#include "drawable.hpp"
#include "gadgets.hpp"
#include "textarea.hpp"
#include "toitoutoll.hpp"

using namespace sym2;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

//`void rect(int x1,int y1,int x2,int y2,unsigned int frame_c);`

//template<class T>
//static inline void swap(T& a,T& b)
//{
//    T c=a;a=b;b=c;
//}

int gadget::on_click(int x,int y,int click)
//Jeśli klik "inside" to rysuje w inwersji, ale zwraca wynik funkcji `is_inside`.
{
    int ins=is_inside(x,y);
    if(ins==1)
        {
        draw_color= set_background(draw_color); //Na draw_color stara wartość background
        replot();
        draw_color= set_background(draw_color); //i z powrotem
        ins=_on_click(x,y,click); //Może zmienić wynik
        }
    return ins;
}

void empty_area::_replot()
//Rysuje przekreślenie obszaru użytkownika
{
    int x1= get_start_x();
    int y1= get_start_y();
    int x2= x1 + get_width() - 1; //-1, bo width obejmuje pierwszy piksel
    int y2= y1 + get_height() - 1;
    line(x1,y1,x2,y2,draw_color);
    line(x1,y2,x2,y1,draw_color);
}

void button::_replot()
{
    int x1= get_start_x();
    int y1= get_start_y();
    int x2= x1 + get_width() - 1; //-1, bo width obejmuje pierwszy piksel
    int y2= y1 + get_height() - 1;
    int xr=x2-3;
    int yr=y2-3;

    if(xr>x1 && yr>y1 )
        {
        line(x1, yr, xr, yr, get_frame());
        line(xr, y1, xr, yr, get_frame());
        x2=xr;
        y2=yr;
        }

    const char* text=name(); //Podręczny wskaźniczek.
    if(vh_mode == 0) //Vertical mode
        {
        if(x2-x1<char_width('X')) return; //Za malo miejsca
        int height=toi(char_height('X'));
        while(*text!='\0' && (y2-y1)>=height)
            {
            printc(x1, y1, draw_color, get_background(), "%c", *text);
            y1+=height;
            text++;
            }
        return; //Wykonane
        }
    if(vh_mode == 1)
        {
        int height=toi(string_height(text));
        if(y2-y1<height) return; //Za malo miejsca
        int y_start= (y1 + y2) / 2 - height / 2; //Środek
        if(y_start < y1) y_start=y1; //jak wylezie za wysoko
        print_width(x1, y_start, x2 - x1, draw_color, get_background(), text); //samo rysowanie
        return;
        }
}


void arrow_button::_replot()
{
    int x1= get_start_x();
    int y1= get_start_y();
    int x2= x1 + get_width() - 1; //-1, bo width obejmuje pierwszy piksel
    int y2= y1 + get_height() - 1;
    int r1=x2-x1;
    int r2=y2-y1;
    wb_color bck= this->get_background();
    if(bck==0)
      bck=256; //W palecie 512
    if(draw_color==0)
      draw_color=256;
    if(bck==255)
      bck=511; //W palecie 512
    if(draw_color==255)
      draw_color=511;
    int	  ow=toi(line_width(mode==0?2:0));				/* Ustala szerokość linii — może być kosztowne. Zwraca stan poprzedni */
    int	  os=line_style(SSH_LINE_SOLID); /* Ustala styl rysowania linii: SSH_LINE_SOLID, SSH_LINE_DOTTED, SSH_LINE_DASHED */
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
        for(int /*y=(y1+y2)/2,*/i=1,end=toi(r1*0.2);i<=end;i++)
        {
            wb_color cur_col=(bck < draw_color?int(draw_color - (double(i) / end) * 255):int(draw_color + (double(i) / end) * 255));
            circle((x1+x2)/2,(y1+y2)/2,r1/2-i, cur_col);
        }
        break;
        case 1 :// print up arrow
        line((x1+x2)/2,y2-2,(x1+x2)/2,y1,draw_color);
        for(int /*y=(y1+y2)/2,*/i=1,end=toi(r1*0.4);i<=end;i++)
        {
            wb_color cur_col=(bck < draw_color?int(draw_color - (double(i) / end) * 255):int(draw_color + (double(i) / end) * 255));
            line((x1+x2)/2, y1,(x1+x2)/2-i,y2-r2/2, cur_col);
            line((x1+x2)/2, y1,(x1+x2)/2+i,y2-r2/2, cur_col);
            line((x1+x2)/2-i/2,y2-2,(x1+x2)/2-i/2,y1+r2/2, cur_col);
            line((x1+x2)/2+i/2,y2-2,(x1+x2)/2+i/2,y1+r2/2, cur_col);
        }
        break;
        case 2 :// print left arrow
        line(x1,(y1+y2)/2,x2-2,(y1+y2)/2,draw_color);
        for(int y=(y1+y2)/2,i=1,end=toi(r2*0.45);i<=end;i++)
        {
            wb_color cur_col=(bck < draw_color?int(draw_color - (double(i) / end) * 255):int(draw_color + (double(i) / end) * 255));
            line(x1, y,x2-r1/2,y-i, cur_col);
            line(x1, y,x2-r1/2,y+i, cur_col);
            line(x2-r1/2,y-i/2,x2-2,y-i/2, cur_col);
            line(x2-r1/2,y+i/2,x2-2,y+i/2, cur_col);
        }
        break;
        case 3 :// print down arrow
        line((x1+x2)/2,y2,(x1+x2)/2,y1+2,draw_color);
        for(int /*y=(y1+y2)/2,*/i=1,end=toi(r1*0.4);i<=end;i++)
        {
            wb_color cur_col=(bck < draw_color?int(draw_color - (double(i) / end) * 255):int(draw_color + (double(i) / end) * 255));
            line((x1+x2)/2, y2,(x1+x2)/2-i,y1+r2/2, cur_col);
            line((x1+x2)/2, y2,(x1+x2)/2+i,y1+r2/2, cur_col);
            line((x1+x2)/2-i/2,y2-r2/2,(x1+x2)/2-i/2,y1+2, cur_col);
            line((x1+x2)/2+i/2,y2-r2/2,(x1+x2)/2+i/2,y1+2, cur_col);
        }
        break;
        case 4 ://print right arrow
        line(x1+2,(y1+y2)/2,x2,(y1+y2)/2,draw_color);
        for(int y=(y1+y2)/2,i=1,end=toi(r2*0.45);i<=end;i++)
        {
            wb_color cur_col=(bck < draw_color?int(draw_color - (double(i) / end) * 255):int(draw_color + (double(i) / end) * 255));
            line(x2, y,x2-r1/2,y-i, cur_col);
            line(x2, y,x2-r1/2,y+i, cur_col);
            line(x1+2,y-i/2,x2-r1/2,y-i/2, cur_col);
            line(x1+2,y+i/2,x2-r1/2,y+i/2, cur_col);
        }
        break;
        default:
        break;
        }
    line_width(ow);
    line_style(os);
}


left_right_button::left_right_button(int ix1, int iy1, int ix2, int iy2):
    empty_area(ix1,iy1,ix2,iy2,::background(),::background(),::background())
{
    int a=(ix1+ix2)/2;
    int b=toi(char_height('X'));
    left	=new arrow_button(ix1+1	,iy1+b,a-1,	iy2-1,		2/*mode left*/	);
    right	=new arrow_button(a+1,iy1+b,ix2-1,iy2-1,		4/*mode right*/);
    set_title("Left-Right control");
}

void left_right_button::_replot()
//Odrysowuje składowe
{
    //	empty_area::_replot(); //Niepotrzebne - tylko przekreślenie
    left->replot(0);
    right->replot(0);
}

steering_wheel::steering_wheel( int ix1,int iy1,int ix2,int iy2,
                               wb_dynarray<rectangle_source_base*>&  idat) //Pamięć żadnej z seri nie jest tu zarządzana
: empty_area(ix1,iy1,ix2,iy2,::background(),::background(),::background())
{
    size_t size=idat.get_size();
    data.alloc(size);                                 assert(data.IsOK());

    //Przepisuje, bo nie wiadomo co to za tablica
    for(size_t i=0;i<size;i++)
    {                                                 assert(idat[i]->valid_memory(this));
        rectangle_source_base* tmp=idat[i];           assert(tmp->valid_memory(tmp));
        cerr<<tmp->name()<<endl;
        data[i]=tmp;
    }

    //USTAWIANIE
    int a=ix2-ix1;
    int b=iy2-iy1;
    resizing=new arrow_button(ix1+a/4,	 iy1+1,	   ix1+a/4*3,	iy1+b/2,	0/*mode target*/);
    up		=new arrow_button(ix1+1,	 iy1+b/2+1,ix2-a/2,	iy2-b/4,	1/*mode up*/);
    down	=new arrow_button(ix1+a/2+1,iy1+b/2+1,ix2,		iy2-b/4,	3/*mode down*/);
    left	=new arrow_button(ix1+1	,iy2-b/4+1,ix2-a/2,	iy2,		2/*mode left*/	);
    right	=new arrow_button(ix1+a/2+1,iy2-b/4+1,ix2,		iy2,		4/*mode right*/);
    set_title("STEERING WHEEL");
}


steering_wheel::steering_wheel( 
                    rectangle_source_base*     i_dat, //Pamięć seri nigdy nie jest tu zarządzana
                    wb_ptr<drawable_base>      i_res, //Pamięć dla pod-obszarów jest
                    wb_ptr<drawable_base>       i_up, // z a w s z e
                    wb_ptr<drawable_base>     i_left, //zarządzana.
                    wb_ptr<drawable_base>     i_down, //Współrzędne obszaru "steering wheel" są
                    wb_ptr<drawable_base>    i_right  //ustalane ze współrzędnych jego składowych.
                    ):			//Tlo jest takie, jakie dla całego okna!
        empty_area(0,0,0,0,::background(),::background(),::background()),
        resizing(i_res), left(i_left), right(i_right), up(i_up), down(i_down)
{
    data.alloc(1);                                 assert(data.IsOK() );
    data[0]=i_dat;

    //ŁADOWANIE WSPÓŁRZĘDNYCH
    load(*resizing);
    add(*up);
    add(*left);
    add(*down);
    add(*right);
    set_title("STEERING WHEEL");
}

steering_wheel::steering_wheel(
                    wb_dynarray<rectangle_source_base*>&  i_dat, // Pamięć żadnej z seri nie jest tu zarządzana
                    wb_ptr<drawable_base> i_res,  //Pamięć dla pod-obszarów jest
                    wb_ptr<drawable_base> i_up,   // z a w s z e
                    wb_ptr<drawable_base> i_left, //zarządzana.
                    wb_ptr<drawable_base> i_down, //Współrzędne obszaru steering wheel są
                    wb_ptr<drawable_base> i_right //ustalane ze współrzędnych jego składowych.
                    ):			//Tlo jest takie, jakie dla całego okna!
        empty_area(0,0,0,0,::background(),::background(),::background()),
        resizing(i_res), left(i_left), right(i_right), up(i_up), down(i_down)
{
    data.alloc(i_dat.get_size());                                 assert(data.IsOK() );

    //Przepisuje, bo nie wiadomo co to za tablica
    for(size_t i=0; i < i_dat.get_size(); i++)
                    data[i]=i_dat[i];

    assert(data.IsOK());
    //ŁADOWANIE WSPÓŁRZĘDNYCH
    load(*resizing);
    add(*up);
    add(*left);
    add(*down);
    add(*right);
    set_title("STEERING WHEEL");
}

//steering_wheel( int ix1,int iy1,int ix2,int iy2,
//					rectangle_source_base*  idat);//Pamięć Żadnej z seri nie jest tu zarządzana


int left_right_button::on_change(const gps_area& new_ar)
//Musi przesunąć współrzędne składowych razem ze swoimi.
{
    float trans[6];
    get_transform_to(new_ar,trans);
    gps_area check(static_cast<gps_area&>(*this)); //Potrzebujemy kopii danych.
    check.transform(trans);
    if(!(check==new_ar))
    {
#ifndef _NDEBUG
        fprintf(stderr,"left_right_button warning: rescaling will loss precision.\n");
#endif
    }

    left->transform(trans);
    right->transform(trans);

    return 1;
}

int steering_wheel::on_change(const gps_area& new_ar)
{
    float trans[6];
    gps_area check(static_cast<gps_area&>(*this)); //Potrzebujemy kopii danych.
    get_transform_to(new_ar,trans);
    check.transform(trans);

    if(!(check==new_ar))
    {
#ifndef _NDEBUG
        fprintf(stderr,"Steering_wheel warning:rescaling will loss precision.\n");
#endif
    }

    resizing->transform(trans);
    up->transform(trans);
    left->transform(trans);
    down->transform(trans);
    right->transform(trans);

    return 1;
}

void steering_wheel::_replot()
//Odrysowuje składowe
{
    resizing->replot(0);
    up->replot(0);
    left->replot(0);
    down->replot(0);
    right->replot(0);
}

int left_right_button::_user_action(int left_or_right, int /*ignoruje click*/)
/*Jeśli lewo to -1, a jeśli prawo to 1*/
{
    return 2; //Obsłużone, choć nie zrobione
}


int left_right_button::on_click(int x, int y, int click)
        //Przepytuje składowe i jeśli któraś została trafiona to ...
{
    if(!is_inside(x, y))//sprawdzenie dla całości
        return 0;  //"Nie moja sprawa!"

    int retval = 0;

    if(left->on_click(x, y, click))
    {
        retval = _user_action(-1, click);
    } else if(right->on_click(x, y, click))
    {
        retval = _user_action(1, click);
    } else
    {
        //Gdzieś w tytuł lub tło
        //replot();
        retval = 1;
    }

    if(retval != 1)
    {
        delay_ms(128);
        set_char('\r'); //Wymuszenie replot-u w następnym nawrocie pętli czytającej
    }

    return retval;
}

int steering_wheel::on_click(int x,int y,int click)
        //Przepytuje składowe i jeśli któraś została trafiona, to
        //adekwatnie zmienia serie za pomocą metody `sub`
        //oraz wymusza odnowienie ekranu
{
    if(!is_inside(x,y)) //sprawdzenie dla całości
        return 0;  //"Nie moja sprawa!"

    int dim=0;
    int multiplier=1;
    geometry::view_info subtab; //Struktura na informacje o "kamerze".
    geometry::MD_info	md;
    geometry_base* geom= data[0]->get_geometry();

    if(geom==nullptr) goto ERROR;
    dim=geom->get_dimension();
    if(dim<2) goto ERROR;
    if(geom->get_view_info(&subtab)==nullptr)
                            goto ERROR; //Wypełnianie struktury informacja o "kamerze"
    if(geom->get_info(&md)==nullptr)
                            goto ERROR; //Wypełnianie struktury informacja
    if(subtab.dia.X()>20*subtab.sst.X() &&
                subtab.dia.Y()>20*subtab.sst.Y() )
        multiplier=10;
    if(up->on_click(x,y,click)==1)
    {
        subtab.pos.Y()-= subtab.sst.Y() * multiplier;
    }
    else if(down->on_click(x,y,click)==1)
    {
        subtab.pos.Y()+= subtab.sst.Y() * multiplier;
    }
    else if(left->on_click(x,y,click)==1)
    {
        subtab.pos.X()-= subtab.sst.X() * multiplier;
    }
    else if(right->on_click(x,y,click)==1)
    {
        subtab.pos.X()+= subtab.sst.X() * multiplier;
    }
    else if(resizing->on_click(x,y,click)==1)
    {
        if(click==1)
            {
                //Przy małych zakresach co 1, przy dużych dwukrotnie
                if(subtab.dia.X()<10*subtab.sst.X()
                    && subtab.dia.Y()<10*subtab.sst.Y() )
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
            if(subtab.dia.X()<15*subtab.sst.X()
                && subtab.dia.Y()<15*subtab.sst.Y() )
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
            subtab.dispose(); //Need be nullptr for resetting
        }
    */
    }
    else //Żaden z pod-obiektów
    return is_inside(x,y);//Wychodzi się.

    //Ustawia tak samo pozostałym zarządzanym seriom
    size_t i;
    for(i=1;i<data.get_size();i++)
        {
        rectangle_source_base* pom=data[i];                    assert(pom!=nullptr);

        geometry_base* sec_geom= pom->get_geometry();
        if(sec_geom==nullptr)
                    continue;
        if(*geom!=*sec_geom)
                    continue;
        if(sec_geom->set_view_info(&subtab)==-1)
                        goto ERROR;
        }
    //I ustawia seri podstawowej
    geom->set_view_info(&subtab);

    set_char('\r'); //Informacja ze trzeba odrysować — prowizoryczna!!!
    return 2;

ERROR://Nie zaimplementowano koniecznej operacja lub inny błąd
    {
    wb_color bf= get_frame(); //Użyte jako tymczasowa
        set_frame(254); //Jasny ale nie bialy
    replot();
        set_frame(bf);
    }

    return 0; //Nie przyznaje się do punktu
}

/*
int text_area::on_click(int x,int y,int click)
//Jeśli "inside" to rysuje w inwersji, ale zwraca 1.
{
int ins=is_inside(x,y);
if(ins==1)
    {
    ins=_on_click(x,y,click);//Może zmienić wynik
    }
return ins;
}
*/

text_area::text_area(int ix1,int iy1,int ix2,int iy2,
            const char* i_text,		//TEXT inicjujący
            unsigned i_color,	//=default_black,
            unsigned i_background,	//=default_white,
            unsigned i_frame,	//=128,
            size_t   buff_size//=-1	//Inicjalny rozmiar bufora.
            ):
            drawable_base(ix1, iy1, ix2, iy2, i_background, i_frame),
            curr_col(i_color),
            user_size(buff_size),
            index(0)
{
    if(user_size!=size_t(-1)) //Tylko jak `user_size` już ustalony
        linie.alloc(user_size);
    add_text(i_text);
}


void text_area::_replot()
{
    int x1= get_start_x();
    int y1= get_start_y();
    int x2= x1 + get_width() - 1; //-1, bo width obejmuje pierwszy piksel
    int y2= y1 + get_height() - 1;
    //int r1=x2-x1;
    //int r2=y2-y1;
    int start=toi(y1+index*char_height('X'));
    if(start>y2)
            start=y2;
    for(int i=toi(index)-1;i>=0;i--)
        {
        start-=toi(char_height('X'));
        if(start>=y1)
            print_width(x1, start,x2-x1, curr_col, get_background(), linie[i].get_ptr_val());
        }
}

void text_area::clean()
{
    linie.dispose();//Usuwa stare
    index=0;		//Umożliwia alokowanie linii od początku.
    if(user_size!=size_t(-1))//Tylko gdy user_size ustalone
        linie.alloc(user_size);// Alokuje nowe
}

int text_area::add_line(const char* ini) //ret 1 jeśli OK
{
    wb_dynarray<char> Ini;
    if(ini==nullptr)
        return 0;

    char* pom=clone_str(ini); //Kopia łańcucha

   // if(pom==nullptr) return 0; //Wpadka na braku pamięci podobno sie już nie zdarza.
   // else
        Ini.set_dynamic_ptr_val(pom,strlen(pom)+1);


    if(index<linie.get_size())
        //Jest jeszcze miejsce. Czyli tablica niezerowego rozmiar
        {
            linie[index]=Ini;//Jeśli przepisze to nie zwolni
            index++;
        }
    else
        {
            linie[0].dispose();
            size_t len=linie.get_size();
            if(len>=2)
                {
                    len=(len-1)*sizeof(linie[0]);
                    // Przesuwamy w pamięci, żeby uniknąć sekwencji konstruktor-destruktor przy kopiowaniu.
                    memmove( (void*)linie.get_ptr_val(),
                             (void*)linie.get_ptr_val(1),
                             len);
                }
            size_t outsize;
            linie[index-1].give_dynamic_ptr_val(outsize); //Zabiera mu z zarządu
            linie[index-1]=Ini;//Jeśli przepisze to nie zwolni
        }

    return 1;
}


int text_area::add_text(const char* ini)
{
    if(ini==nullptr)
        return 0;

    char* pom=clone_str(ini);           // assert(pom!=nullptr);

    /// Bardziej elegancki uchwyt dla kopii łańcucha źródłowego, usuwanej w momencie wyjścia z metody.
    wb_dynarray<char> Ini;
    Ini.set_dynamic_ptr_val(pom,strlen(pom)+1); ///< Bo tu mu dajemy w zarząd.

    if(linie.get_size()==0) //Trzeba zaalokować jakieś linie
        {
        size_t licznik=0; //StepCounter końców linii.
        char* iter=pom;   //assert(iter);
        // Zliczanie końców linii.
        while(*iter!='\0') //To na pewno nie będzie null!!!
            {
            if(*iter=='\n') licznik++;
            iter++;
            }
        linie.alloc(licznik+1); //Jeśli nie ma końców linii to i tak jest jedna
        //cerr<<"linii: "<<licznik<<"in text:"<<endl<<ini<<endl;
        }

    char* iter=pom;   //assert(iter);
    bool flaga=false; //Czy koniec pętli
    do{
      if(	*iter=='\n'
         || (flaga=(*iter=='\0'))
         )
            {
            *iter='\0'; //Koniec poprzedniej linii
            if(add_line(pom)!=1)
                return 0;
            pom=iter+1; //Początek następnej linii
            }
        iter++;
       }while(!flaga);

    return 1;
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


