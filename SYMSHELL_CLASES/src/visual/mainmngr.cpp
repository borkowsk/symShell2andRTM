//	Zarządca obszaru ekranu zaimplementowany bezpośrednio na bazie funkcji SYMSHELL'a
//   do obsługi całego ekranu/okna SYMSHELL'a.
//*/////////////////////////////////////////////////////////////////////////////////////////


//#include "INCLUDE/platform.hpp"

#include <cstdlib>
#include <cstdio> //sprintf!!!
#include <iostream>

#include "symshell.h"
#include "sshutils.hpp"
#include "sshmenuf.h"
#include "textarea.hpp"
#include "mainmngr.hpp"
#include "amngrcmd.h"
//#include "wbminmax.hpp"
#include "viewHtml.hpp"

using namespace std;

template<class T>
static inline void swap(T& a,T& b)
{
T c=a;a=b;b=c;
}

//int WB_error_enter_before_clean=1;/* For close_plot() */

int		main_area_menager::initialized=0;
int		main_area_menager::counter=0;//Licznik obiektow tego typu. Ma byc 1
int		main_area_menager::idle_must_work;//Flaga wywolywania on_idle
wb_color main_area_menager::Marker=0;//Kolor do znakowania przez usera
wb_pchar main_area_menager::def_dump_name( "dump" );
wb_pchar main_area_menager::old_win_title;

size_t	main_area_menager::screen_number=0;
int main_area_menager::screen_number_precision=7;
ssh_menu_handle    main_area_menager::WindowMenu=nullptr;
int main_area_menager::HowManyAreas=0;

void	 main_area_menager::set_dump_name(const char* name,size_t number)
{
if(number!=-1)
		screen_number=number;
if(name!=nullptr)
		def_dump_name=clone_str(name);
}

//extern "C" int  dump_screen(const char* Filename);//Z symshell'a

void main_area_menager::dump_screen()
{
    if(!int(def_dump_name))
        return;

    wb_pchar buf;
    buf.alloc(strlen(def_dump_name.get_ptr_val())+20+1);//Numer raczej nie wiecej niz 10 cyfr :)

	//sprintf(buf.get_ptr_val(),"%s_%0*d", def_dump_name, screen_number_precision, screen_number);
    buf.prn("%s_%0*d", def_dump_name.get_ptr_val(), screen_number_precision, screen_number);

    if(::dump_screen(buf.get_ptr_val())!=-1)
    {
        screen_number++;
    }
    else
    {
        perror("dump screen failed");
    }
}


void main_area_menager::enable_background()
//i odblokowuje
{
idle_must_work=1;
}

int  main_area_menager::set_marker(wb_color newmark)
//zwraca 1 jesli OK, lub 0
{
if(newmark>=0 && newmark<255)
	{
	Marker=newmark;
	return 1;
	}
return 0;
}

//KONSTRUKTORY
//---------------
//Wywolanie wiecej niz jednego konstruktora powoduje aborcje procesu!!!
main_area_menager::main_area_menager(size_t size, //Konstruktor dajacy zarzadce o okreslonym rozmiarze listy
				                    int width,int height,
				                    unsigned ibkg
				                    ):
				area_menager(size,0,0,width-1,height-1,ibkg)
{
if(initialized || counter>0)
	{
	fprintf(stderr,"One main_area_menager already constructed!\n");
	fprintf(stderr,"Process will be aborted!\n");
	abort();
	}
counter++;
}

/*
main_area_menager(size_t size,//Konstruktor z lista czesciowo wypelniona
				  int width,int height,
				  //bkg i frm domyslne - mozna zmienic potem
                  drawable_base* //first...nullptr
				  );
*/

//METODY
//--------

//Inicjacja trybu graficznego w momencie dogodnym dla projektanta aplikacji
//Zwraca 1 jesli ok
//Przed pomyslnym wywolaniem start() nie wolno bezposrednio lub posrednio wywolywac
//jakichkolwiek funkcji rysujacych po ekranie.
int main_area_menager::start(const char* wintitle,int argc,const char* argv[],int buf)
{
    ssh_menu_handle mainmenu=nullptr;

    if(initialized)
    {
        fprintf(stderr,"Graphix output already started!\n");
        fprintf(stderr,"Process will be aborted!\n");
        abort();
    }

    if(buf!=-1)
        ::buffering_setup(buf);

    ::mouse_activity(1);
    ::fix_size(0);
    ::set_background(this->getbackground());
    ::shell_setup(wintitle,argc,argv);

    int ret=::init_plot(getwidth(),getheight(),0,0);
    if(!ret) return 0;

    //Zmiana rozmiarow w przypadku gdy ekran jest za maly
	if(getwidth()>::screen_width() || getheight()>screen_height())
		gps_area::set(0,0,::screen_width()-1,screen_height()-1);

    //Przygotowany
    initialized=1;

    //Odnalezienie menu "Window" - zeby mozna bylo dodawac elementy
    if((mainmenu=ssh_main_menu())!=nullptr)
    {
        unsigned pos=ssh_get_item_position(mainmenu,"Window");
        if(pos==-1)
            pos=ssh_get_item_position(mainmenu,"Okno");
        if(pos!=-1)
            WindowMenu=ssh_sub_menu(mainmenu,pos); //I zapamientanie do uzycia w funkcji insert
        //ssh_menu_add_item(WindowMenu,"Test menu item",55555);
    }

    return 1;
}

const char* main_area_menager::gettitle()
//Poprzedni tytuł lub nullptr jak nie był ustalony
{
	if(old_win_title)
		return old_win_title.get();
	else
        return nullptr;
}

int main_area_menager::settitle(const char* wintitle)
//Ustawienie tytułu okna
{
	old_win_title.take(clone_str(wintitle));
  	return ssh_set_window_name(wintitle);
}


int    main_area_menager::insert(wb_ptr<drawable_base>	drw)
{
    const char* pom=drw->name();    assert(pom!=nullptr);//Musi byc tu bo po replace drw jest juz puste!!!
    int ret=area_menager::replace(HowManyAreas,drw);
    if(ret>-1)
    {
        assert(ret==HowManyAreas);
        if(WindowMenu)
        {
            ssh_menu_add_item(WindowMenu,pom,SSH_FIRST_FREE_MESSAGE+ret);
            ssh_menu_mark_item(WindowMenu,1,SSH_FIRST_FREE_MESSAGE+ret);
            ssh_realize_menu(WindowMenu);
        }
        HowManyAreas++;
    }

    return ret;
}

int     main_area_menager::minimize(size_t index)
//Ukrywa podobszar
{
    if(!is_minimized(index) && (WindowMenu)  )
        ssh_menu_mark_item(WindowMenu,0,SSH_FIRST_FREE_MESSAGE+index);
    return area_menager::minimize(index);
}

int     main_area_menager::restore(size_t  index)
//Odtwarza poprzednie polozenie i rozmiar obszaru
{
	if(is_minimized(index) && (WindowMenu) )
		ssh_menu_mark_item(WindowMenu,1,SSH_FIRST_FREE_MESSAGE+index);
	return area_menager::restore(index);
}

int     main_area_menager::orginal(const wb_dynarray<int>& lstindex)
//Odtwarza pierwotne  polozenie i rozmiar obszaru
{
	return area_menager::orginal(lstindex);
}

int     main_area_menager::orginal(size_t  index)
//Odtwarza pierwotne  polozenie i rozmiar obszaru
{
	if(is_minimized(index) && (WindowMenu) )
		ssh_menu_mark_item(WindowMenu,1,SSH_FIRST_FREE_MESSAGE+index);
	return area_menager::orginal(index);
}

int     main_area_menager::minimize(const wb_dynarray<int>& lstindex)
//Ukrywa podobszar
{
	return area_menager::minimize(lstindex);
}

int     main_area_menager::restore(/*ALL*/)
//Odtwarza poprzednie polozenie i rozmiar obszaru
{
	return area_menager::restore();
}

void main_area_menager::need_confirmation_before_clean(int yes)
//Metoda ukrywajaca dostep do WB_error_enter_bofore_clean
{
WB_error_enter_before_clean=yes;
}

void main_area_menager::make_help_area(const char* text)
{
if(text==nullptr)
	{
	text=                "%@C AREA MENAGER CONTROL KEYS \n"
		"%@C--------------------------------------------------------------------\n"
        "ctrl-A: do one step of backround processing\n"
		"ctrl-B: start/stop background processing\n"
		"ctrl-E: exclude (hide) marked areas from window\n"
		"ctrl-H: reserved - mouse event\n"
		"ctrl-I: make/restore/minimize HELP window\n"
		"ctrl-J: replot all\n"
		"ctrl-K: mark all areas\n"
		"ctrl-L: reserved (deletion of areas not implemented)\n"
		"ctrl-M: reserved - replot event\n"
		"ctrl-O: move marked areas to orginal position\n"
		"ctrl-Q: exit\n"
		"ctrl-R: restore all areas to window\n"
		"ctrl-S: save screen\n"
		"ctrl-T: tile marked areas\n"
		"ctrl-U: unmark all areas\n";
	}

int ile=0;
//Zliczenie znakow \n w tekscie
{
const char* pom=text;
while(*pom++){if(*pom=='\n')ile++;}
}

ile+=3;//Na tytul i pusta linie i zapas
int maxy=getheight()/5+ile*char_height('X');

text_area* pom=new text_area(
				getwidth()/4,getheight()/5,
				getwidth()/4*3,(maxy<getheight()?maxy:getheight()-1),
				text,
				default_black,default_white,250);

if(pom)//Zaalokowane OK
	{
	pom->settitle("HELP");
	int helpind=insert(pom);   //Zabiera zawartość w zarzad!!!
	pom->replot();
	}
}

/// Obsługa wszelkich zdarzeń z zewnątrz
/// Wychodzi z funkcji, gdy nie ma co robić, czyi brak zdarzeń do obsługi.
void main_area_menager::process_input()
{
int inp;
while((!background_enabled()) || input_ready())
   {
   inp=get_char();
#ifndef NDEBUG
   //cerr<<inp<<' '<<char(inp)<<'\n';
#endif
   if(_pre_process_input(inp))
		continue;
   switch(inp)
		{
	   //case 0: //ctrl-@ break;
	   case SSH_ONESTEP:
	   case 1: //ctrl-a
		   {
			   cout<<" STEP ";
			   cout.flush();
			   disable_background(); //Procedure będzie zablokowana po powrocie
		   }
		   return; //break; niepotrzebne raczej
	   case SSH_STARTSTOP:
	   case 2: //ctrl-b
		   if(background_enabled())
		   {
				cout<<"STOPPED."<<char(7)<<endl;//<<flush;
				disable_background(); //Blokuje ta procedure w pętli
		   }
			else
			{
				cout<<"CONTINUE."<<char(7)<<endl;//<<flush;
				enable_background(); //Odblokowanie procedury i od razu wyjście
			    return;
			} //Przerywa wewnętrzną pętlę tej funkcji
	   break;
	   //case 3: //ctrl-c break;
	   //case 4: //ctrl-d break;
       case SSH_WINDOWS_HIDEMARKEDAREAS:
	   case 5: //ctrl-e
           {
            wb_dynarray<int> list=get_marked(Marker,1); //Z odmarkowywaniem
		    minimize(list);
           }
	   break;
	   //case 6: //ctrl-f break;
	   //case 7: //ctrl-g break;
	   //case 8:
		case '\b': //ctrl-h
		{
		int xpos=0,ypos=0,click=0; //Myszowate
		get_mouse_event(&xpos,&ypos,&click);

		if(on_click(xpos,ypos,click)==1)
			{
			int pom=get_last_lazy_area();
			if(pom==-1) return; //Cos nie tak ale olal...

			if(click==1)
			{
			if(pom==get_maximized())
				restore(pom);
				else
				maximize(pom);
			}
			else
			if(click==2)
				{
				if(is_marked(pom))
					unmark(pom);
					else
					mark(pom,Marker);
				}

			}
		}
		break;
        case SSH_HELP_AUTHORSWWWPAGE:
            {
                ViewHtml( std::string("http://iss.uw.edu.pl/") );
            }
        break;
        case SSH_HELP_SHORTCUTHELP:
	    case 9://ctrl-i
            {
                int helpind=search("HELP");
				if(helpind==-1)
				{
                    make_help_area();
                }
                else
                    if(tab[helpind].minimized)
                    {
                        orginal(helpind);
                    }
                    else
                    {
                        minimize(helpind);
                    }
            }
		break;
	   //case 10:
		case '\n'://ctrk-j
			replot();
		break;
        case SSH_WINDOWS_MARKALLAREAS:
		case 11://ctrl-k
			mark_all(wb_color(Marker)); //Wszystkie widoczne
		break;

        /*
		case 12://ctrl-l
		{
		wb_dynarray<int> dousuwania=get_marked(Marker,1);
		//minimize(dousuwania);
		for(int i=0;i<dousuwania.get_size();i++)
			remove(dousuwania[i]);
		replot();
		}
		break;
		*/

		case '\r': //ctrl-m
		{
        ssh_coordinate rx,ry;
        ssh_natural    rw,rh;
        int            maxim;

		//Reakcja na zmianę rozmiarów okna
		gps_area old_area(*this); //Potrzebna do późniejszych przeliczeń.
        gps_area new_area(0,0,max((ssh_natural)0,::screen_width()-1),
                              max((ssh_natural)0,::screen_height()-1));
		this->load(new_area);

		//Powinno byc jeszcze przeliczenie aktualnych
		if((maxim=get_maximized())>=0)
        {
			get(maxim)->load(new_area);
        }
        else
        {
				//Nie ma jednego. Nie tak prosto!
        }

		//Właściwe odrysowanie
		int old=mouse_activity(0);
		int ret=repaint_area(&rx,&ry,&rw,&rh);
		if(ret==-1) //Nie wiadomo co dokładnie, więc lepiej wszystko
		  this->replot( *this);
		  else
		  if(ret==0) //Wiadomo, że tylko określony obszar
			{
			gps_area are(rx,ry,rx+rw,ry+rh);
//#ifndef NDEBUG
			//rect(rx,ry,rx+rw,ry+rh,default_black);//TEST ONLY!!!
//#endif
			this->replot( are ); //Tylko cześć wnętrza. Ramki i tytułu i tak nie ma!
			}
		mouse_activity(old);
		}
		break;

		//case 14: //ctrl-n break;
        case SSH_WINDOWS_RESTORETOORGINALPOSITION:
		case 15: //ctrl-o
			orginal(get_marked(Marker,1)); //Z odmarkowywaniem
		break;

		//case 16: //ctrl-p
        case SSH_FILE_EXIT:
		case 17: //ctrl-Q
		case EOF:
			need_break_action();
		return; //Nie ma już kontynuować!

        case SSH_WINDOWS_UNCOVERHIDDENAREAS:
		case 18: //ctrl-r //restore_all
			restore();
		break;

        case SSH_FILE_DUMPSCREEN:
		case 19: //ctrl-s
			dump_screen();
		break;

        case SSH_WINDOWS_TILEMARKEDAREAS:
		case 20: //ctrl-t //tile
			tile(get_marked(Marker,1));//Z odmarkowywaniem
		break;

        case SSH_WINDOWS_TILE_ALL:
			mark_all(Marker);
			tile(get_marked(Marker,1));
        break;

        case SSH_WINDOWS_UNMARKALLAREAS:
		case 21: //ctrl-u
			get_marked(Marker,1);//Odznacza wszystkie
		break;

		default:
            if(SSH_FIRST_FREE_MESSAGE<=inp && inp<SSH_FIRST_FREE_MESSAGE+HowManyAreas)
            {
                int ind=inp-SSH_FIRST_FREE_MESSAGE;
                if(is_minimized(ind))
                {
                    restore(ind);
                }
                else
                {
                    minimize(ind);
                }
            }
			else
			{
			 if(_post_process_input(inp)!=1)
			 {
				if(cerr)
					cerr<<"\b unknown command"<<endl;
             }
			}
		}
    }
}

/// gdy zostanie wywołane to koniec wejścia
void main_area_menager::break_input_loop()
{
    need_break_action(); //Nie ma kontynuowac!
}

/// Przechwycenie całości sterowania
/// Wychodzi z tej funkcji dopiero gdy user "zakończy" program.
void main_area_menager::run_input_loop()
{
    need_break_action(0); //Planuje pokontynuować.
    do{
    process_input();
    _on_idle();
    }while(should_continue());
}


//ELASTYCZNE UCHWYTY OBSŁUGI
//---------------------------
int main_area_menager::_pre_process_input(int input_char)
//Przed obsluga domyslna. Zwraca 1 jesli obsluzyl.
{
	return 0; //nie obsluzyl
}

int main_area_menager::_post_process_input(int input_char)
//Po obsludze domyslnej. Zwraca 1 jesli obsluzyl.
{
	return 1; //Uznaj za obsluzone
}

int main_area_menager::_on_idle()
//Uruchamiane przez run_input_loop() gdy nie ma zdarzen.
{
    delay_ms(0);//Tu nie ma nic do roboty
               //Wiec trzeba dac szanse systemowi
	return 0;
}

//#include <alloc.h> //Tylko pod borlandami jest heapcheck()

main_area_menager::~main_area_menager()
//Wirtualny destruktor
{
if(initialized)
		::close_plot();
counter--;																		//assert( heapcheck() != _HEAPCORRUPT );
}


/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*                                                                  */
/*      Instytut Studiow Spolecznych Uniwersytetu Warszawskiego     */
/*                                                                  */
/*        WWW:  http://www.iss.uw.edu.pl/~borkowsk                  */
/*        MAIL: borkowsk@samba.iss.uw.edu.pl                        */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/

