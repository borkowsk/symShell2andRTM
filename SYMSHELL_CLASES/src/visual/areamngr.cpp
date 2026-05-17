/// @file
/// @brief **Implementation of the base area manager** /<br>
///         _Implementacja bazowego zarządcy obszarów._
/// @date 2026-05-17 (modified)
//---------------------------------------------------------------------------
//  Wersja z kosmetyką XI 2012
//======================================================================================================================

#include "areamngr.hpp"
#include "toitoutoll.hpp"

using namespace sym2;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

area_manager::~area_manager()  //Wirtualny destruktor
{
    maximized=-1;grabbed=-1;
}

area_manager::area_manager(size_t size, //Konstruktor dający zarządcę o określonym rozmiarze listy
                int ix1, int iy1, int ix2, int iy2,
                           unsigned i_bkg,
                           unsigned i_frm)
: area_manager_base(ix1, iy1, ix2, iy2, i_bkg, i_frm), maximized(-1), grabbed(-1), lazy(-1), tab(size)
{}

//	AKCESORY OGÓLNE:
//------------------

 size_t area_manager::get_size()
//Podaje po prostu aktualny rozmiar listy łącznie z pozycjami pustymi (a mogą być takie? Chyba że na końcu...).
 {
    return tab.get_size();
 }
 
 int    area_manager::insert(wb_ptr<drawable_base> drw)
//Dodaje obszar do listy. Zwraca pozycje albo -1(błąd)
 {  
    size_t len=tab.get_size();
    for(size_t i=0;i<len;i++)
        if(!tab[i].ptr)
            return replace(i,drw);
    return -1;
 }

 int    area_manager::replace(const char* nam, wb_ptr<drawable_base> drw)
//Wymienia na liście. Jak nie znajdzie, to zwraca -1.
 {
    int pos=search(nam);
    if(pos!=-1)
        return replace(pos,drw);
    return -1; //nie znalazł pozycji
 }

 int    area_manager::replace(size_t    index, wb_ptr<drawable_base> drw)
//Wymienia na liście. Jak błędne parametry to zwraca -1.
 {
    if(index>=tab.get_size()) return -1; //błędny parametr.

    tab[index].ptr=drw; //Jeśli był to `wb_ptr` zwalnia.
    if(tab[index].ptr)
    {
        tab[index].orig_pos.load(*tab[index].ptr);
        tab[index].saved.load(*tab[index].ptr);
    }
    else //De facto kasowanie
    {
        if(index==maximized)
            maximized=-1; //Nie ma już zmaksymalizowanego.
    }

    return toi(index);
 }

 int area_manager::as_original(size_t    index)
 {
 if(index>=tab.get_size()) return -1; //błędny parametr

 if(tab[index].ptr)
        tab[index].orig_pos.load(*tab[index].ptr);

 return 0;
 }

 int    area_manager::search(const char* nam)
//Odnajduje na liście. Zwraca -----//----
 {
    for(size_t i=0;i<tab.get_size();i++)
        if(tab[i].ptr &&
            strcmp(nam,tab[i].ptr->name())==0)
                return toi(i);

    return -1; //nie znalazł
 }


// AKCESORY poszczególnych obszarów
//---------------------------------

 drawable_base /*const*/* area_manager::get_ptr(size_t index)
//Bez możliwości modyfikacji i zwolnienia
 {
    if( //index >= 0 && //Zaszłość
        index<tab.get_size() )
        return tab[index].ptr.get_ptr_val();
    else
        return NULL;
 }

 wb_ptr<drawable_base>&  area_manager::get(size_t index)
//Możliwość modyfikacji, ale trzeba pamiętać,
//że pewne informacje są zapisywane w zarządcy w związku z pozycją.
 {
    assert(index>=0 && index<tab.get_size());
    return tab[index].ptr;
 }

//	REAKCJE NA ZDARZENIA
//---------------------

 int    area_manager::on_click(int x, int y, int click)
//Przepytuje obszary z reakcji na punkt.
//Jeśli on_click() zwraca 1 to można się dowiedzieć, który obszar wywołując...
 {
     if(tab.get_size()==0) return -1;
     if(maximized!=-1 && tab[maximized].ptr ) //Jeśli jest zmaksymalizowany to nie szukaj dalej
            {
            assert(maximized>=0 && maximized<int(tab.get_size()));
            int on_click_ret=tab[maximized].ptr->on_click(x,y,click);
            assert( on_click_ret>=0 && on_click_ret<=2 );
            if(on_click_ret==1)
                    lazy=maximized; //jaki index
            return  on_click_ret;  //Czy obsłużone?
            }

    for(int i=toi(tab.get_size())-1;i>=0;i--)
        if( tab[i].ptr && !tab[i].minimized )
            {
            int on_click_ret=tab[i].ptr->on_click(x,y,click);	assert( on_click_ret>=0 && on_click_ret<=2 );
            if(on_click_ret==0)
                continue; //Szukaj dalej
            lazy=i;		  //jaki index
            if(on_click_ret==2)
                return 2; //Jest w obszarze i obsłużono

            return 1;     //Znaleziono, ale nie obsłużono
            }

    if(this->on_margin_click(x,y,click))
                return 2; //W tym obszarze, ale na nie zajętych marginesach

    return 0; //Nikt w tym obszarze
 }

 int    area_manager::get_last_lazy_area()
//zwróci -1, jeśli już raz wzięte, lub inny błąd
 {
    int pom=lazy;
    lazy=-1;
    return pom;
 }

 int    area_manager::on_input(int input_char)
//Przepytuje obszary czy chcą znak
 {
     if(tab.get_size()==0) return -1;
     int on_input_ret=-1;

     //Jeśli jest główny odbiorca to idzie tylko do niego
     if(grabbed!=-1 && tab[grabbed].ptr  && !tab[grabbed].minimized)
            if((on_input_ret=tab[grabbed].ptr->on_input(input_char))==1)
                return 1;
                                        assert(on_input_ret!=1);
     //Jeśli nie ma głównego, to odbierają wszyscy
     for(int i=toi(tab.get_size())-1;i>=0;i--)
        if( tab[i].ptr  && !tab[i].minimized)
        {
            on_input_ret=tab[i].ptr->on_input(input_char);
        }

     return 1; //Obsłużone
 }

 

 void   area_manager::replot(const gps_area& ar)
//Odrysowuje obszary "nadepnięte" przez "ar"
 {
     if(maximized!=-1)
     {
         assert(maximized>=0 && maximized<int(tab.get_size()));
         tab[maximized].ptr->replot(0);
     }
     else
     {
     size_t N=tab.get_size();
     for(size_t i=0;i<N;i++)
        if( tab[i].ptr && !tab[i].minimized)
             if(tab[i].ptr->is_overlapped(ar) )
                    tab[i].ptr->replot(0);
     }

    flush();
 }

 /// Odrysowuje wszystkie (widoczne) obszary
 void   area_manager::_replot()
 {
   if(maximized!=-1)
   {
      assert(maximized>=0 && maximized<int(tab.get_size()));
      tab[maximized].ptr->replot(0);
   }
   else
   {
     for(size_t i=0;i<tab.get_size();i++)
        if( tab[i].ptr && !tab[i].minimized)
                tab[i].ptr->replot(0);
   }

     flush_plot();
 }


//  MANIPULATORY
//----------------


 int    area_manager::mark(size_t index, wb_color frame)
 //Zaznacza obszar
 {
    if( //index>=0 &&
        index<tab.get_size() && !tab[index].minimized)
    {
        tab[index].mark=true; //one bit `int` przechowuje tylko 0 i -1 ???
        tab[index].org_frame= tab[index].ptr->set_frame(frame);
        tab[index].ptr->replot();
        return 0;
    }

    return -1;
 }

 int    area_manager::mark_all(wb_color frame)
 //Zaznacza wszystkie widoczne obszary 
 {
     if(maximized != -1)
     {
         assert(maximized >= 0 && maximized < int(tab.get_size()));
         mark(maximized, frame);
         return 0;
     }
     else
     {
         for(size_t i = 0; i < tab.get_size(); i++)
             if(tab[i].ptr && !tab[i].minimized)
                 mark(i, frame);
         return 0;
     }
    //return -1;
 }

 int    area_manager::unmark(size_t index)
//i odznacza obszar
 {
    if( /*index>=0 &&*/ index<tab.get_size() )
        {
        tab[index].mark=false;
            tab[index].ptr->set_frame(tab[index].org_frame);
        tab[index].ptr->replot();
        return 0;
        }
    return -1;
 }

int    area_manager::refresh(size_t index)
//i odznacza obszar
 {
    if( /*index>=0 &&*/ index<tab.get_size() )
        {
        if(maximized==index || tab[index].minimized)
                    tab[index].ptr->replot();
        return 0;
        }
    return -1;
 }


int    area_manager::is_marked(size_t index)
//Informuje czy jest zaznaczony
{
    if( /*index>=0 &&*/ index<tab.get_size() && tab[index].mark )
        {
        return 1;
        }
    return 0;
}

int    area_manager::is_minimized(size_t index)
//Informuje czy jest zaznaczony
{
    if( /*index>=0 &&*/ index<tab.get_size() && tab[index].minimized )
        {
        return 1;
        }
    return 0;
}

 wb_dynarray<int> area_manager::get_marked(wb_color what, int unm)
//Zwraca listę zaznaczonych obszarów.
//Jeśli `what == default color` to wszystkie zaznaczone.
//Wtedy opcjonalnie zdejmuje zaznaczenie (???).
 {
     wb_dynarray<int> pom(get_size()); //z dużym zapasem
     size_t ok=0;
     //Odszukuje wszystkie zamarkowane zgodnie z what
     //i odznacza, jeśli trzeba.
     for(size_t i=0;i<tab.get_size();i++)
        if( tab[i].ptr && tab[i].mark)
          if( what==default_color || tab[i].ptr->get_frame() == what )
            {
                if(unm)
                    unmark(i);
                pom[ok]=toi(i);
                ok++;
            }

     wb_dynarray<int> ret(ok);
     for(size_t j=0;j<ok;j++)
        {
        ret[j]=pom[j];
        }

     return ret;
 }

 int    area_manager::set_input(size_t index)
//Ustala obszar jako pierwszy do wejścia z klawiatury lub zdarzeń menu
 {
    int pom=grabbed;
//    if(index<0) //To się już nie zdarza.
//        {
//        grabbed=-1; //TODO, bo to jest podejrzane. unsigned(-1) jest markerem, ale kompilator może tu to zoptymalizować!
//        return pom;
//        }
//        else
    if(index<tab.get_size())
    {
        grabbed=int(index);
        return pom;
    }
    else
        return -1;
 }

 int    area_manager::maximize(size_t index)
//Oddaje pod-obszarowi cały zarządzany obszar
 {
    if( //index>=0 &&
        index<tab.get_size() &&
        index!=maximized //Jeszcze nie jest
       )
        if(tab[index].ptr->on_change(*this)==1) //Jeśli akceptuje transformacje
            {
            tab[index].saved.load(*tab[index].ptr);
            tab[index].ptr->load(*this); //Cały obszar oddać
            tab[index].ptr->replot(); //i narysować w nowym położeniu
            maximized=toi(index);
            return 0;
            }
     return -1;
 }

 int    area_manager::minimize(size_t index)
//Ukrywa pod-obszar
 {
    if( //index>=0 &&
        index<tab.get_size() && !tab[index].minimized )
    {
        if(index==maximized) //Jest zmaksymalizowany
        {
            maximized=-1; //Czyszczona poprzednia pozycja zapamiętana w `maximized`.
        }
        else
        {
            drawable_base* pom=tab[index].ptr.get_ptr_val();            assert(pom!=NULL);
            tab[index].saved.load(*pom); //Zapamiętać poprzednią pozycję.
        }

        //Likwiduje obszar
        tab[index].ptr->clear();
        tab[index].minimized=true; //Już się nie odrysuje
        replot(*tab[index].ptr); //odrysowuje wszystkie dotychczas zasłonięte

        tab[index].ptr->set(0,0,0,0); //Zeruje rozmiar, ale nie informuje o tym
                                //bo by to groziło całkowitą degeneracja informacji! :-)
        return 0;
    }
    return -1;
 }

 int    area_manager::restore(size_t  index)
//Odtwarza poprzednie położenie i rozmiar obszaru
 {
     if( //index>=0 &&
         index<tab.get_size() )
         {
         if(index==maximized)
            {
            if(tab[index].ptr->on_change(tab[index].saved)==1) //Jeśli akceptuje transformacje
                {
                tab[index].ptr->clear();
                maximized=-1;
                tab[index].minimized=true; //Tymczasowa deaktywacja i przekazanie, że była flaga
                replot(*tab[index].ptr); //odrysowuje wszystkie dotychczas zasłonięte
                }
            }

         if(tab[index].minimized) //Jeśli była ustawiona flaga
            {
            tab[index].minimized=false;
            tab[index].ptr->load(tab[index].saved);
            tab[index].ptr->replot();
            return 0;
            }
         }
     return -1;
 }

 int    area_manager::restore()
 {
     for(size_t i=0;i<tab.get_size();i++)
        if( tab[i].ptr )
            if( i==maximized || tab[i].minimized)
                restore(i);
     return 0;
 }

 int    area_manager::original(size_t  index)
//Odtwarza pierwotne  położenie i rozmiar obszaru
 {
     if( //index>=0 &&
         index<tab.get_size() && tab[index].ptr )
        {
        if(tab[index].minimized)
            {
            tab[index].ptr->load(tab[index].saved);
            }
        else
            {
            if(index==maximized)
                    maximized=-1;

            tab[index].ptr->clear();
            tab[index].minimized=true; //Tymczasowa deaktywacja i przekazanie, że była flaga
            replot(*tab[index].ptr); //odrysowuje wszystkie dotychczas zasłonięte
            }

        tab[index].minimized=false;
        if(tab[index].ptr->on_change(tab[index].orig_pos) == 1)
            {
            tab[index].ptr->load(tab[index].orig_pos); //Ładuje oryginalne położenie
            tab[index].ptr->replot();
            }
        return 0;
        }

     return -1;
 }

 int    area_manager::restore(const wb_dynarray<int>& lst)
//Robi restore dla wszystkich obszarów
 {
     if(lst.get_size()<=0)
            return -1;
     size_t i;
     for(size_t j=0;j<lst.get_size();j++)
        {
        i=lst[j];
        if(i<tab.get_size() )
          if( tab[i].ptr )
            if(i==maximized && tab[i].minimized)
                {
                //Po takiej operacji nie może zostać bez ramki
                //if(tab[i].ptr->get_frame()==default_color)
                //	tab[i].org_frame=tab[i].ptr->set_frame(default_black);
                restore(i);
                }
        }

     return 0;
 }

int    area_manager::minimize(const wb_dynarray<int>& lst)
//Robi original dla wszystkich obszarów
 {
    if(lst.get_size()<=0)
            return -1;
    size_t i;
    for(size_t j=0;j<lst.get_size();j++)
      if((i=lst[j])>=0 && i<tab.get_size() )
        if( tab[i].ptr )
                minimize(i);

    return 0;
 }

int    area_manager::original(const wb_dynarray<int>& lst)
//Robi original dla wszystkich obszarów
 {
    if(lst.get_size()<=0)
            return -1;
    minimize(lst);

    size_t i;
    for(size_t j=0;j<lst.get_size();j++)
      if((i=lst[j])>=0 && i<tab.get_size() )
        if( tab[i].ptr )
            original(i);

    return 0;
 }

static wb_dynarray<int> create_complement(int beg, int end, const wb_dynarray<int>& lst)
//Tworzy listę obszarów uzupełniającą do zadanej.
{																						assert(beg<end);assert(lst.IsOK());
wb_dynarray<int> ret( (end-beg+1)-lst.get_size() );										assert(lst.IsOK());
int i,ind=0;
size_t j;
for(i=beg;i<=end;i++)
    {
    for(j=0;j<lst.get_size();j++)
      if(i==lst[j]) goto JEST;
    ret[ind++]=i;
    JEST:;
    }																					assert(lst.IsOK());
return ret;
}

int    area_manager::tile(const wb_dynarray<int>& lst)
//Na chama, czyli po równo, albo -1 jak nie da się.
 {
    if(lst.get_size()<=0)
            return -1;																	assert(lst.IsOK());
    {
    wb_dynarray<int> poz= create_complement(0, toi(tab.get_size()) - 1, lst);			assert(lst.IsOK());
    if(poz.IsOK()) //Jak są jakieś do zminimalizowania
            minimize(poz);
    }

    size_t dzielX=1;
    size_t dzielY=1;
    //Ustalanie sposobu podziału
    for(size_t swit=0;dzielX*dzielY<lst.get_size();swit=!swit)
        //Mysi być tyle obszarów ile okien
        {
        if(swit==0)
            dzielX++;
        else
            dzielY++;
        }
    //Dzielenie
    size_t skokX= get_width() / dzielX;
    size_t skokY= get_height() / dzielY;
    size_t licz=0;																			assert(lst.IsOK());
    for(size_t a=0;a<dzielX;a++)
        for(size_t b=0;b<dzielY;b++)
            {
            gps_area pom( toi(get_start_x() + a * skokX),
                          toi(get_start_y() + b * skokY),
                          toi(get_start_x() + (a + 1) * skokX) - 1,
                          toi(get_start_y() + (b + 1) * skokY) - 1);
            size_t index=lst[licz++];														assert(lst.IsOK());
                                                                                        assert(index<tab.get_size());
            if(tab[index].ptr)
                {
                if(tab[index].ptr->on_change(pom)==1)
                    tab[index].ptr->clear(0);
                else
                    minimize(index);
                tab[index].ptr->load(pom);
                }
            if(licz>=lst.get_size())
                goto KONIEC;
            }
KONIEC:
    drawable_base::replot();
    return 0;
 }

int    area_manager::arrange(const wb_dynarray<int>& lst)
// albo zwracają -1, jeśli się nie udało.
 {
    if(lst.get_size()<=0)
            return -1;
    //wb_dynarray<int> pozos; //pozostałe
    wb_dynarray<int> poz= create_complement(0, toi(tab.get_size()) - 1, lst);
    minimize(poz);
    //....
    return -1;
 }

int    area_manager::on_change(const gps_area& ar)
//Reaguje na zmianę rozmiarów lub położenia własnego obszaru
 {
    //gps_area old=*this;
    //Ta wersja nie ma nic do roboty.
    return -1;
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



