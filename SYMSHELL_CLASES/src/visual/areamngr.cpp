// Implementacja podstawowego mengera obszarow i...
//------------------------------------------------
//  Wersja z kosmetyką XI 2012
//*///////////////////////////////////////////////////////////////////////////
#include "areamngr.hpp"

area_menager::~area_menager()  //Wirtualny destruktor
{
maximized=-1;grabbed=-1;  
}

area_menager::area_menager(size_t size, //Konstruktor dający zarządcę o określonym rozmiarze listy
				int ix1,int iy1,int ix2,int iy2,
				unsigned ibkg,
				unsigned ifrm):
	area_menager_base(ix1,iy1,ix2,iy2,ibkg,ifrm),
	maximized(-1),grabbed(-1),
	tab(size)
{
}
	
//	AKCESORY OGOLNE
//----------------------------
 size_t area_menager::get_size()
//Podaje po prostu aktualny rozmiar listy lacznie z pozycjami pustymi
 {
	return tab.get_size();
 }
 
 int    area_menager::insert(wb_ptr<drawable_base> drw)
//Dodaje obszar do listy. Zwraca pozycje albo -1(blad)
 {  
	size_t len=tab.get_size();
	for(size_t i=0;i<len;i++)
		if(!tab[i].ptr)			
			return replace(i,drw);
	return -1;
 }

 int    area_menager::replace(const char* nam,wb_ptr<drawable_base> drw)
//Wymienia na liscie. Jak nie znajdzie to zwraca -1.
 {
	int pos=search(nam);
	if(pos!=-1)
		return replace(pos,drw);
	return -1;//nie znalazl pozycji
 }

 int    area_menager::replace(size_t    index,wb_ptr<drawable_base> drw)
//Wymienia na liscie. Jak bledne parametry to zwraca -1.
 {
	if(index>=tab.get_size()) return -1;//bledny parametr
	
	tab[index].ptr=drw;//Jesli byl to wb_ptr zwalnia
	if(tab[index].ptr)
	{
		tab[index].orginal.load(*tab[index].ptr);
		tab[index].saved.load(*tab[index].ptr);
	}
	else //De facto kasowanie
	{
		if(index==maximized)
			maximized=-1;//Nie ma juz zmaksymalizowanego
	}

	return index;
 }

 int area_menager::as_orginal(size_t    index)
 {
 if(index>=tab.get_size()) return -1;//bledny parametr
	
 if(tab[index].ptr)
		tab[index].orginal.load(*tab[index].ptr);

 return 0;
 }

 int    area_menager::search(const char* nam)
//Odnajduje na liscie. Zwraca -----//----	
 {
	for(size_t i=0;i<tab.get_size();i++)
		if(tab[i].ptr && 
			strcmp(nam,tab[i].ptr->name())==0)
				return i;
	return -1;//nie znalazl
 }


// AKCESORY poszczegolnych obszarow
 drawable_base /*const*/* area_menager::get_ptr(size_t index)
//Bez mozliwosci modyfikacji i zwolnienia
 {
	if( index>=0 && index<tab.get_size() )
		return tab[index].ptr.get_ptr_val();
		else
		return NULL;
 }

 wb_ptr<drawable_base>&  area_menager::get(size_t index)
//Mozliwosci modyfikacji, ale trzeba pamietac
//ze pewne informacje sa zapisywane w zarzadcy w zwiazku z pozycja
 {	
	assert(index>=0 && index<tab.get_size());
	return tab[index].ptr;
 }

 //	REAKCJE NA ZDAZENIA
//--------------------------------
 int    area_menager::on_click(int x,int y,int click)
//Przepytuje obszary z reakcji na punkt.
//Jesli on_click() zwraca 1 to mozna sie dowiedziec, kt�ry obszar wywo�ujac:
 {
	 if(tab.get_size()==0) return -1;
	 if(maximized!=-1 && tab[maximized].ptr )//Jesli jest zmaksymalizowany to nie szukaj dalej
			{
			assert(maximized>=0 && maximized<int(tab.get_size()));
			int on_click_ret=tab[maximized].ptr->on_click(x,y,click);
			assert( on_click_ret>=0 && on_click_ret<=2 );
			if(on_click_ret==1)
					lazy=maximized;//jaki index
			return  on_click_ret;  //Czy obsluzone.
			}
	
	for(int i=tab.get_size()-1;i>=0;i--)
		if( tab[i].ptr && !tab[i].minimized )
			{
			int on_click_ret=tab[i].ptr->on_click(x,y,click);	assert( on_click_ret>=0 && on_click_ret<=2 );
			if(on_click_ret==0)
				continue; //Szukaj dalej
            lazy=i;		  //jaki index
			if(on_click_ret==2)	
				return 2; //Jest w obszarze i obsluzono
			//lazy=i;		  //jaki index
			return 1;     //Znaleziono ale nie obsluzono
			}
	
	if(this->on_margin_click(x,y,click))
				return 2; //W tym obszarze ale na niezajetych marginesach

	return 0;//Nikt w tym obszarze
 }

 int    area_menager::get_last_lazy_area()
//zwroci -1 jesli juz raz wziete, lub inny blad
 {
	int pom=lazy;
	lazy=-1;
	return pom;
 }

 int    area_menager::on_input(int input_char)
//Przepytuje obszary czy chca znak
 {
	 if(tab.get_size()==0) return -1;
	 int on_input_ret=-1;
	 
	 //Jesli jest glowny odbiorca to idzie tylko do niego
	 if(grabbed!=-1 && tab[grabbed].ptr  && !tab[grabbed].minimized)
			if((on_input_ret=tab[grabbed].ptr->on_input(input_char))==1)
				return 1;
										assert(on_input_ret!=1);
	 //Jesli nie ma glownego to odbieraja wszyscy
	 for(int i=tab.get_size()-1;i>=0;i--)
		if( tab[i].ptr  && !tab[i].minimized)			
				{
				on_input_ret=tab[i].ptr->on_input(input_char);
				}

	 return 1;//Obsluzone
 }

 

 void   area_menager::replot(const gps_area& ar)
//Odrysowuje obszary "nadepniete" przez "ar"
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

 void   area_menager::_replot()
//Odrysowuje wszystkie (widoczne) obszary
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
 }


//  MANIPULATORY
//----------------
 int    area_menager::mark(size_t index,wb_color frame)
//Zaznacza obszar 
 {
	if( index>=0 && index<tab.get_size() && !tab[index].minimized)
		{
		tab[index].mark=1;
		tab[index].org_frame=tab[index].ptr->setframe(frame);
		tab[index].ptr->replot();
		return 0;
		}
	 return -1;
 }

 int    area_menager::mark_all(wb_color frame)
 //Zaznacza wszystkie widoczne obszary 
 {
	 if(maximized!=-1)
	 {
     assert(maximized>=0 && maximized<int(tab.get_size()));
	 mark(maximized,frame);
	 return 0;
	 }
	 else
     {
	 for(size_t i=0;i<tab.get_size();i++)
		if( tab[i].ptr && !tab[i].minimized)
				mark(i,frame);
	 return 0;
	 }
 return -1;
 }

 int    area_menager::unmark(size_t index)
//i odznacza obszar
 {
	if( /*index>=0 &&*/ index<tab.get_size() )
		{
		tab[index].mark=0;
		tab[index].ptr->setframe(tab[index].org_frame);
		tab[index].ptr->replot();
		return 0;
		}
	return -1;
 }

int    area_menager::refresh(size_t index)
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


int    area_menager::is_marked(size_t index)
//Informuje czy jest zaznaczony
{
	if( /*index>=0 &&*/ index<tab.get_size() && tab[index].mark )
		{
		return 1;
		}
	return 0;
}

int    area_menager::is_minimized(size_t index)
//Informuje czy jest zaznaczony
{
	if( /*index>=0 &&*/ index<tab.get_size() && tab[index].minimized )
		{
		return 1;
		}
	return 0;
}

 wb_dynarray<int> area_menager::get_marked(wb_color what,int unm)
//Zwraca liste zaznaczonych obszarow. 
//Jesli what=default color to wszystkie zaznaczone.
// i opcjonalnie zdejmuje zaznaczenie
 {
	 wb_dynarray<int> pom(get_size());//z duzym zapasem
	 size_t ok=0;
	 //Odszukuje wszystkie zamarkowane zgodnie z what
	 //i odmarkowuje jesli trzeba
	 for(size_t i=0;i<tab.get_size();i++)
		if( tab[i].ptr && tab[i].mark)
		  if( what==default_color || tab[i].ptr->getframe()==what )
			{
			if(unm) unmark(i);
			pom[ok]=i;
			ok++;
			}

	 wb_dynarray<int> ret(ok);
	 for(size_t j=0;j<ok;j++)
		{
		ret[j]=pom[j];
		}	
	 
	 return ret;
 }

 int    area_menager::set_input(size_t index)
//Ustala obszar jako pierwszy do wejscia z klawiatury lub zdarzen menu
 {
	int pom=grabbed;
	if(index<0)
		{
		grabbed=-1; //TODO, bo to jest podejrzane. unsigned(-1) jest markerem, ale kompilator mo�e tu to wyoptymalizowa�
		return pom;
		}
		else
		if(index<tab.get_size())
			{
			grabbed=index;
			return pom;
			}
			else 
			return -1;
 }

 int    area_menager::maximize(size_t index)
//Oddaje podobszarowi caly zarzadzany obszar
 {
	if( index>=0 && index<tab.get_size() && 
		index!=maximized //Jeszcze nie jest
	   )
		if(tab[index].ptr->on_change(*this)==1)//Jesli akceptuje transformacje
			{
			tab[index].saved.load(*tab[index].ptr);
			tab[index].ptr->load(*this);//Caly obszar oddac
			tab[index].ptr->replot();//i narysowac w nowym polozeniu
			maximized=index;
			return 0;
			}
	 return -1;
 }

 int    area_menager::minimize(size_t index)
//Ukrywa podobszar
 {
	if( index>=0 && index<tab.get_size() && !tab[index].minimized )
    {
        if(index==maximized)//Jest zmaksymalizowany
        {
            maximized=-1;//Poprzednia pozycja zapamientana w maximize
        }
        else
        {
            drawable_base* pom=tab[index].ptr.get_ptr_val();            assert(pom!=NULL);
            tab[index].saved.load(*pom);//Zapamientac poprzednia pozycje
        }

        //Likwiduje obszar	
        tab[index].ptr->clear();
        tab[index].minimized=1;//Juz sie nie odrysuje
        replot(*tab[index].ptr);//odrysowuje wszystkie dotyczczas zasloniete
        
        tab[index].ptr->set(0,0,0,0);//Zeruje rozmiar, ale nie informuje o tym
								//boby to grozilo calkowita degeneracja informacji
        return 0;
    }
	return -1;
 }

 int    area_menager::restore(size_t  index)
//Odtwarza poprzednie polozenie i rozmiar obszaru
 {
	 if( index>=0 && index<tab.get_size() )
		 {
		 if(index==maximized)
			{
			if(tab[index].ptr->on_change(tab[index].saved)==1)//Jesli akceptuje transformacje
				{
				tab[index].ptr->clear();
				maximized=-1;				
				tab[index].minimized=1;//Tymczasowa deaktywacja i przekazanie ze byla flaga	
				replot(*tab[index].ptr);//odrysowuje wszystkie dotyczczas zasloniete	
				}
			}

		 if(tab[index].minimized)//Jesli byla flaga
			{
			tab[index].minimized=0;
			tab[index].ptr->load(tab[index].saved);
			tab[index].ptr->replot();
			return 0;
			}
		 }
	 return -1;
 }
 
 int    area_menager::restore()
 {
 for(size_t i=0;i<tab.get_size();i++)
	if( tab[i].ptr )
		if( i==maximized || tab[i].minimized)
				restore(i);
 return 0;
 }

 int    area_menager::orginal(size_t  index)
//Odtwarza pierwotne  polozenie i rozmiar obszaru
 {
	 if( index>=0 && index<tab.get_size() && tab[index].ptr )
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
			tab[index].minimized=1;//Tymczasowa deaktywacja i przekazanie ze byla flaga
			replot(*tab[index].ptr);//odrysowuje wszystkie dotyczczas zasloniete
			}

		tab[index].minimized=0;
		if(tab[index].ptr->on_change(tab[index].orginal)==1)
			{
			tab[index].ptr->load(tab[index].orginal);//Laduje orginalne polozenie
			tab[index].ptr->replot();
			}
		return 0;
		}

	 return -1;
 }

 int    area_menager::restore(const wb_dynarray<int>& lst)
//Robi restore dla wszystkich obszarow
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
				//Po takiej operacji nie moze zostac bez ramki
				//if(tab[i].ptr->getframe()==default_color)
		 		//	tab[i].org_frame=tab[i].ptr->setframe(default_black);
				restore(i);
				}
		}

	 return 0;
 }
 
int    area_menager::minimize(const wb_dynarray<int>& lst)
//Robi orginal dla wszystkich obszarow
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

int    area_menager::orginal(const wb_dynarray<int>& lst)
//Robi orginal dla wszystkich obszarow
 {
	if(lst.get_size()<=0)
			return -1;
	minimize(lst);

	size_t i;
	for(size_t j=0;j<lst.get_size();j++)
	  if((i=lst[j])>=0 && i<tab.get_size() )
		if( tab[i].ptr )	
				orginal(i);
	 
	return 0;
 }

static wb_dynarray<int> tworz_uzupelnienie(int beg,int end,const wb_dynarray<int>& lst)
//Tworzy list� lufcik�w uzupe�niaj�c� do danej
{
																assert(beg<end);
																assert(lst.IsOK());
wb_dynarray<int> ret( (end-beg+1)-lst.get_size() );
																assert(lst.IsOK());
int i,ind=0;
size_t j;
for(i=beg;i<=end;i++)
	{
	for(j=0;j<lst.get_size();j++)
	  if(i==lst[j]) goto JEST;
	ret[ind++]=i;
	JEST:;
	}	
																				assert(lst.IsOK());
return ret;
}

int    area_menager::tile(const wb_dynarray<int>& lst)
//...na chama, czyli po rowno, albo -1 jak nie da sie
 {  
	if(lst.get_size()<=0)
			return -1;															assert(lst.IsOK());
	{
	wb_dynarray<int> poz=tworz_uzupelnienie(0,tab.get_size()-1,lst);			assert(lst.IsOK());
	if(poz.IsOK()) //Jak s� jakie� do zminimalizowania
			minimize(poz);
	}

	size_t dzielX=1;
	size_t dzielY=1;
	//Ustalanie sposobu podzialu
	for(size_t swit=0;dzielX*dzielY<lst.get_size();swit=!swit)
		//Mysi byc tyle obszarow ile okien
		{
		if(swit==0)
			dzielX++;
			else
			dzielY++;
		}
	//Dzielenie
	size_t skokX=getwidth()/dzielX;
	size_t skokY=getheight()/dzielY;
	size_t licz=0;																assert(lst.IsOK());
	  for(size_t a=0;a<dzielX;a++)
		for(size_t b=0;b<dzielY;b++)
			{
			gps_area pom(getstartx()+a*skokX,
						 getstarty()+b*skokY,
						 getstartx()+(a+1)*skokX-1,
						 getstarty()+(b+1)*skokY-1);
			size_t index=lst[licz++];
																				assert(lst.IsOK());
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

int    area_menager::arrange(const wb_dynarray<int>& lst)
// albo zracaja -1, jesli sie nie udalo
 {
	if(lst.get_size()<=0)
			return -1;
	//wb_dynarray<int> poz;//pozostale
	wb_dynarray<int> poz=tworz_uzupelnienie(0,tab.get_size()-1,lst);
	minimize(poz);
	//....
	return -1;
 }

int    area_menager::on_change(const gps_area& ar)
//Reguje na zmiane rozmiarow lub polozenia wlasnego obszaru
 {
	gps_area old=*this;
	
	return -1;
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



