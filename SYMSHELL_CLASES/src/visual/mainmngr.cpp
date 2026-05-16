/// @file
/// @brief **A screen or window area manager, implemented directly based on SYMSHELL functions.** /<br>
///         _Zarządca obszarów ekranu lub okna, zaimplementowany bezpośrednio na bazie funkcji SYMSHELL-a._
/// @date 2026-05-16 (last modification)
//*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Do obsługi całego ekranu/okna SYMSHELL-a.

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
#include "viewHtml.hpp"
#include "toitoutoll.hpp"

using namespace std;
using namespace sym2;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

//template<class T>
//static inline void swap(T& a,T& b)
//{
//  T c=a;a=b;b=c;
//}

//int WB_error_enter_before_clean=1;/* For close_plot() */
ssh_menu_item_definition* TopPopup=context_menu_default; //Próba wymuszenia linkowania modułu definicji menu z biblioteki klas.

int			main_area_manager::initialized=0;
int			main_area_manager::counter=0; //StepCounter obiektów tego typu. Ma być 1
int			main_area_manager::idle_must_work; //Flaga wywoływania on_idle
wb_color	main_area_manager::color_marker=0; //Kolor do znakowania przez "user-a"
wb_pchar	main_area_manager::def_dump_name("dump" );
wb_pchar	main_area_manager::old_win_title;
size_t		main_area_manager::screen_number=0;
int			main_area_manager::screen_number_precision=7;
ssh_menu_handle	main_area_manager::sub_menu_handle=nullptr;
int			main_area_manager::how_many_areas=0;

void	main_area_manager::set_dump_name(const char* name, size_t number) // NOLINT(*-convert-member-functions-to-static)
{
    if(number!=-1)
            screen_number=number;
    if(name!=nullptr)
            def_dump_name=clone_str(name);
}

//extern "C" int dump_screen(const char* Filename); //Z symshell-a

void	main_area_manager::dump_screen() // NOLINT(*-convert-member-functions-to-static)
{
    if(!int(def_dump_name))
        return;

    wb_pchar buf;
    buf.alloc(strlen(def_dump_name.get_ptr_val())+20+1); //Numer raczej nie więcej niż 10 cyfr :)

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


void	main_area_manager::enable_background() // NOLINT(*-convert-member-functions-to-static)
//i odblokowuje
{
    idle_must_work=1;
}

int		main_area_manager::set_marker(wb_color new_mark) // NOLINT(*-convert-member-functions-to-static)
//zwraca 1 jeśli OK, lub 0
{
    if( // new_mark >= 0 && //Już nie może być ujemne!
        new_mark < 255)
        {
            color_marker=new_mark;
        return 1;
        }
    return 0;
}

//KONSTRUKTORY
//---------------
//Wywołanie więcej niż jednego konstruktora powoduje aborcje procesu!!!
main_area_manager::main_area_manager( size_t size, //Konstruktor dający zarządcę o określonym rozmiarze listy
                                      int width, int height,
                                      unsigned ibkg
                                    ):
        area_manager(size, 0, 0, width - 1, height - 1, ibkg)
{
    if(initialized || counter>0)
    {
        fprintf(stderr,"One main_area_manager already constructed!\n");
        fprintf(stderr,"Process will be aborted!\n");
        abort();
    }

    counter++;
}

/*
main_area_manager(size_t size,	//Konstruktor z lista częściowo wypełniona
                  int width,int height,
                  //bkg i frm domyślne - można zmienić potem
                  drawable_base* //first...nullptr
                  );
*/

//METODY
//--------

// Inicjacja trybu graficznego w momencie dogodnym dla projektanta aplikacji
// Zwraca 1, jeśli ok.
// Przed pomyślnym wywołaniem `start()` nie wolno bezpośrednio lub pośrednio
// wywoływać jakichkolwiek funkcji rysujących na ekranie.
int main_area_manager::start(const char* win_title, int argc, const char* argv[], int buf)
{
    ssh_menu_handle main_menu=nullptr;

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
    ::set_background(this->get_background());
    ::shell_setup(win_title, argc, argv);

    int ret=::init_plot(get_width(), get_height(), 0, 0);
    if(!ret) return 0;

    //Zmiana rozmiarów w przypadku gdy ekran jest za mały
    if(get_width() > ::screen_width() || get_height() > screen_height())
        gps_area::set(0,0,::screen_width()-1,screen_height()-1);

    //Przygotowany
    initialized=1;

    //Odnalezienie menu "Window" - żeby można było dodawać elementy
    if((main_menu=ssh_main_menu()) != nullptr)
    {
        unsigned pos=ssh_get_item_position(main_menu, "Window");
        if(pos==-1)
            pos=ssh_get_item_position(main_menu, "Okno");
        if(pos!=-1)
            sub_menu_handle=ssh_sub_menu(main_menu, pos); //I zapamiętanie do użycia w funkcji insert
        //ssh_menu_add_item(sub_menu_handle,"Test menu item",55555);
    }

    return 1;
}

const char* main_area_manager::get_title()
//Poprzedni tytuł lub nullptr jak nie był ustalony
{
    if(old_win_title)
        return old_win_title.get();
    else
        return nullptr;
}

int main_area_manager::set_main_title(const char* win_title) // NOLINT(*-convert-member-functions-to-static)
//Ustawienie tytułu okna
{   //Poniżej produkt z `clone_str` jest zapamiętywany w obiekcie, który zabezpiecza jego dealokację.
    //Więc nie ma żadnego "memory leak"!!!
    old_win_title.take( clone_str(win_title) );
    return ssh_set_window_name(win_title);
}


int    main_area_manager::insert(wb_ptr<drawable_base>	drw)
{
    const char* pom=drw->name();    assert(pom!=nullptr); //Musi być tu bo po replace drw jest juz puste!!!
    int ret=area_manager::replace(how_many_areas, drw);
    if(ret>-1)
    {
        assert(ret == how_many_areas);
        if(sub_menu_handle)
        {
            ssh_menu_add_item(sub_menu_handle, pom, SSH_FIRST_FREE_MESSAGE + ret);
            ssh_menu_mark_item(sub_menu_handle, 1, SSH_FIRST_FREE_MESSAGE + ret);
            ssh_realize_menu(sub_menu_handle);
        }
        how_many_areas++;
    }

    return ret;
}

int     main_area_manager::minimize(size_t index)
//Ukrywa pod-obszar
{
    if(!is_minimized(index) && (sub_menu_handle)  )
        ssh_menu_mark_item(sub_menu_handle, 0, SSH_FIRST_FREE_MESSAGE + index);
    return area_manager::minimize(index);
}

int     main_area_manager::restore(size_t  index)
//Odtwarza poprzednie położenie i rozmiar obszaru
{
    if(is_minimized(index) && (sub_menu_handle) )
        ssh_menu_mark_item(sub_menu_handle, 1, SSH_FIRST_FREE_MESSAGE + index);
    return area_manager::restore(index);
}

int     main_area_manager::original(const wb_dynarray<int>& lst_index)
//Odtwarza pierwotne  położenie i rozmiar obszaru
{
    return area_manager::original(lst_index);
}

int     main_area_manager::original(size_t  index)
//Odtwarza pierwotne  położenie i rozmiar obszaru
{
    if(is_minimized(index) && (sub_menu_handle) )
        ssh_menu_mark_item(sub_menu_handle, 1, SSH_FIRST_FREE_MESSAGE + index);
    return area_manager::original(index);
}

int     main_area_manager::minimize(const wb_dynarray<int>& lst_index)
//Ukrywa pod-obszar.
{
    return area_manager::minimize(lst_index);
}

int     main_area_manager::restore(/*ALL*/)
//Odtwarza poprzednie położenie i rozmiar obszaru
{
    return area_manager::restore();
}

void main_area_manager::need_confirmation_before_clean(int yes) // NOLINT(*-convert-member-functions-to-static)
//Metoda ukrywająca dostęp do `WB_error_enter_before_clean`
{
    WB_error_enter_before_clean=yes;
}

void main_area_manager::make_help_area(const char* text)
{
    if(text==nullptr)
    {
        text=                "%@C AREA MANAGER CONTROL KEYS \n"
            "%@C--------------------------------------------------------------------\n"
            "ctrl-A: do one step of background processing\n"
            "ctrl-B: start/stop background processing\n"
            "ctrl-E: exclude (hide) marked areas from window\n"
            "ctrl-H: reserved - mouse event\n"
            "ctrl-I: make/restore/minimize HELP window\n"
            "ctrl-J: replot all\n"
            "ctrl-K: mark all areas\n"
            "ctrl-L: reserved (deletion of areas not implemented)\n"
            "ctrl-M: reserved - replot event\n"
            "ctrl-O: move marked areas to original position\n"
            "ctrl-Q: exit\n"
            "ctrl-R: restore all areas to window\n"
            "ctrl-S: save screen\n"
            "ctrl-T: tile marked areas\n"
            "ctrl-U: unmark all areas\n";
    }

    int ile=0;
    //Zliczenie znaków \n w tekście
    {
        const char* pom=text;
        while(*pom++){if(*pom=='\n')ile++;}
    }

    ile+=3; //Na tytuł i pusta linie i zapas
    int maxy= toi(get_height() / 5 + ile * char_height('X'));

    text_area* pom=new text_area(
            get_width() / 4, get_height() / 5,
                    get_width() / 4 * 3, (maxy < get_height()?maxy:get_height() - 1),
            text,
            default_black, default_white, 250);

    //if(pom) //Zaalokowane OK, I NIE MOŻE BYĆ INACZEJ!
    {
        pom->set_title("HELP");
        int help_ind=insert(pom);   //Zabiera zawartość w zarząd!!!
        pom->replot();
    }
}

/// Obsługa wszelkich zdarzeń z zewnątrz
/// Wychodzi z funkcji, gdy nie ma co robić, czyi brak zdarzeń do obsługi.
void main_area_manager::process_input()
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
                cout<<"STOPPED."<<char(7)<<endl; //<<flush;
                disable_background(); //Blokuje ta procedure w pętli
           }
           else
           {
                cout<<"CONTINUE."<<char(7)<<endl; //<<flush;
                enable_background(); //Odblokowanie procedury i od razu wyjście
                return;
           } //Przerywa wewnętrzną pętlę tej funkcji
       break;
       //case 3: //ctrl-c break;
       //case 4: //ctrl-d break;
       case SSH_WINDOWS_HIDEMARKEDAREAS:
       case 5: //ctrl-e
           {
            wb_dynarray<int> list=get_marked(color_marker, 1); //Z odznaczaniem
            minimize(list);
           }
       break;
       //case 6: //ctrl-f break;
       //case 7: //ctrl-g break;
       //case 8:
        case '\b': //ctrl-h
        {
        int x_pos=0,y_pos=0,click=0; //Myszowate
        get_mouse_event(&x_pos, &y_pos, &click);

        if(on_click(x_pos, y_pos, click) == 1)
            {
            int pom=get_last_lazy_area();
            if(pom==-1) return; //Coś nie tak, ale olał...

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
                        mark(pom, color_marker);
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
                int help_index=search("HELP");
                if(help_index == -1)
                {
                    make_help_area();
                }
                else
                    if(tab[help_index].minimized)
                    {
                        original(help_index);
                    }
                    else
                    {
                        minimize(help_index);
                    }
            }
        break;
       //case 10:
        case '\n': //control-j
            replot();
        break;
        case SSH_WINDOWS_MARKALLAREAS:
        case 11://ctrl-k
            mark_all(wb_color(color_marker)); //Wszystkie widoczne
        break;

        /*
        case 12://ctrl-l
        {
        wb_dynarray<int> do_usuwania=get_marked(color_marker,1);
        //minimize(do_usuwania);
        for(int i=0;i<do_usuwania.get_size();i++)
            remove(do_usuwania[i]);
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
        //gps_area old_area(static_cast<gps_area&>(*this)); //Kopia potrzebna do późniejszych przeliczeń.
        gps_area new_area(0,0,max((ssh_natural)0,::screen_width()-1),
                              max((ssh_natural)0,::screen_height()-1));
        this->load(new_area);

        //Powinno być jeszcze przeliczenie aktualnych
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
            //rect(rx,ry,rx+rw,ry+rh,default_black); //TEST ONLY!!!
//#endif
            this->replot( are ); //Tylko cześć wnętrza. Ramki i tytułu i tak nie ma!
            }
        mouse_activity(old);
        }
        break;

        //case 14: //ctrl-n break;
        case SSH_WINDOWS_RESTORETOORGINALPOSITION:
        case 15: //ctrl-o
            original(get_marked(color_marker, 1)); //Z odznaczaniem
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
            tile(get_marked(color_marker, 1)); //Z odznaczaniem
        break;

        case SSH_WINDOWS_TILE_ALL:
            mark_all(color_marker);
            tile(get_marked(color_marker, 1));
        break;

        case SSH_WINDOWS_UNMARKALLAREAS:
        case 21: //ctrl-u
            get_marked(color_marker, 1); //Odznacza wszystkie
        break;

        default:
            if(SSH_FIRST_FREE_MESSAGE<=inp && inp< SSH_FIRST_FREE_MESSAGE + how_many_areas)
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
                if(cerr && inp != 0 ) //Zero jest dozwolonym kodem na nic nierobienie (np. kliknięcie etykiety menu)
                    cerr<<"\n\b unknown command "<<inp<<endl;
             }
            }
        }
    }
}

/// Gdy zostanie wywołane to koniec wejścia.
void main_area_manager::break_input_loop()
{
    need_break_action(); //Nie ma kontynuować!
}

/// Przechwycenie całości sterowania
/// Wychodzi z tej funkcji dopiero wtedy, gdy user "zakończy" program.
void main_area_manager::run_input_loop()
{
    need_break_action(0); //Planuje jeszcze "pokontynuować".
    do{
    process_input();
    _on_idle();
    }while(should_continue());
}


//ELASTYCZNE UCHWYTY OBSŁUGI
//---------------------------
int main_area_manager::_pre_process_input(int input_char)
//Przed obsługą domyślną. Zwraca 1, jeśli obsłużył.
{
    return 0; //nie obsłużył
}

int main_area_manager::_post_process_input(int input_char)
//Po obsłudze domyślnej. Zwraca 1, jeśli obsłużył.
{
    return 1; //Uznaj za obsłużone
}

int main_area_manager::_on_idle()
//Uruchamiane przez run_input_loop() gdy nie ma zdarzeń.
{
    delay_ms(0); //Tu nie ma nic do roboty
                 //Wiec trzeba dać szansę systemowi
    return 0;
}

//#include <alloc.h> //Tylko pod Borland-ami jest `heapcheck` (?)

main_area_manager::~main_area_manager()
//Wirtualny destruktor
{
    if(initialized)
            ::close_plot();
    counter--;														// assert( heapcheck() != _HEAPCORRUPT );
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

