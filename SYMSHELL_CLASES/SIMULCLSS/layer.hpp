//Definicja bazy i szablonow  warstwy symulacji
//////////////////////////////////////////////////////////////////////
#ifndef __LAYER_HPP__
#define __LAYER_HPP__

//#include "geombase.hpp"
#include "rectgeom.hpp"
#include "datasour.hpp"
#include "simpsour.hpp"
#include "filtsour.hpp"
#include "statsour.hpp"
#include "fifosour.hpp"
//#include "sourmngr.hpp" any_layer_base::const unsigned long FULL=UINT_MAX;

class any_layer_base
//---------------------------
{
public:
    //static const unsigned long FULL;//Najwieksza wartosc indeksu - marker nieznalezienia itp.
	enum my_full {FULL=((unsigned long)(ULONG_MAX))}; //Zamiast #define FULL albo const full
//Wirtualny destruktor
virtual	~any_layer_base(){}
//Rejestracja zrodla/zrodel w menagerze danych
//W klasach specjalizowanych mozna zdefiniowac automatyczna rejestracje
virtual int registry_sources(sources_menager_base&		Sources)
	{return 0;}//0->Brak automatycznej rejestracji. Inne oznaczaja sukces
//Zwraca wskaznik do geometri - nie wolno go z-delet-owac
virtual const geometry_base* get_geometry()=0;
//virtual void swap(size_t index1,size_t index2)=0;//Zamienia ze soba dwa elementy
//virtual void clean(size_t index)=0;//Czysci obiekt sposobem zdefiniowanym dla konkretnego typu warstwy
//TYPE& get(size_t index);//Daje dostep do elementu o indeksie obliczonym przez geometrie

//Implementacja wejscia/wyjscia. Zwracaj 1 jesli sukces!
virtual
int		implement_output(ostream& o) const=0;
virtual
int		implement_input(istream& i)=0;
//i samych operatorow strumieniowych
friend
ostream& operator << (ostream& o,const any_layer_base& w);
friend
istream& operator >> (istream& i,any_layer_base& w);
};

template<class TYPE>
class layer:public any_layer_base
{
public:
virtual TYPE& get(size_t index)=0;//Daje dostep do elementu o indeksie obliczonym przez geometrie
};

//Klasa implementujaca wlasnosci typowe dla wartswy prostokatnej
class rectangle_layer
//////////////////////////////////////////////
{
protected:
rectangle_geometry	MainGeometry;//Geometria dla operacji na warstwie
//rectangle_geometry	VisoGeometry;//Geometria dla serii danych - w celu ich wizualizacji
public:

//AKCESORY ZALEZNE OD WLASNOSCI RECTANGLE
/////////////////////////////////////////////
// Przypisanie polu/lub agentowi wartosci RGB np z bitmapy
virtual
void assign_rgb(size_t TargetX,size_t TargetY,//tez konieczne bo pure-virtual
				unsigned char Red,
				unsigned char Green,
				unsigned char Blue,
				void* user_data=0
				)=0;

int init_from_bitmap(const char* filename,void* user_data=0);//Wczytanie pliku GIF,BMP lub XBM na warstwe. Plik musi miec rozmiar wystarczajacy inaczej "foult"

void clean_line(int X1,int Y1,int X2,int Y2);   //Czyszczenie lini

void clean_horizontal(int X1,int Y1,size_t N);  //Czyszczenie wiersza punktow

void clean_circle(int X,int Y,size_t R);//Czyszczenie kola z agentów i podłoża

void clean_randomly(int how_many);              //Czyszczenie losowo wybranych elementow

virtual void clean(size_t TargetX,size_t TargetY)=0;//Czyszczenie elementu - ze sprawdzaniem zakresu, lub zawijaniem

virtual void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY)=0;//Zamiana elementow

virtual bool filled(int X,int Y)=0; //Sprawdzenie czy jest agent w tym miejscu

//Akcesory i metody ogolne
/////////////////////////////////
const rectangle_geometry* get_rect_geometry()//Wypelnienie obowiazku pure-virtual
			{return &MainGeometry;}

//virtual rectangle_source_base* make_source(const char* name)=0;//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe

//KONSTRUKTOR/DESTRUKTOR
~rectangle_layer(){}
rectangle_layer(size_t Width,
				size_t Height):
				MainGeometry(Width,Height)//,
				//VisoGeometry(Width,Height)
				{}

};

//Szablon warstwy typu skalarnego, nie wskaznikowego!!!
template<class SCALAR>
class rectangle_unilayer:public layer<SCALAR>,public rectangle_layer
//----------------------------------------------
{
wb_dynarray<SCALAR> table;
SCALAR cleaner;//"Obiekt" do zamazywania
public:
rectangle_unilayer(size_t Width,
				size_t Height,
				const SCALAR& iclean):
				rectangle_layer(Width,Height),
				table(Width*Height),//odpowiednia ilosc pol
				cleaner(iclean)
				{
				size_t N=Width*Height;
				for(size_t i=0;i<N;i++)
					table[i]=cleaner;
				}

//Metody Pure-virtual, które musza zastac zdefiniowane dla kazdej warstwy
///////////////////////////////////////////////////////////////////////////
const geometry_base* get_geometry()//Wypelnienie obowiazku pure-virtual
	{ return &MainGeometry;}

SCALAR& get(size_t index)//Daje dostep do elementu o indeksie obliczonym przez geometrie
	{ return table[index]; }

//Metody specyficzne dla warstw prostokatnych
///////////////////////////////////////////////////////////////////////////
//Bezposredni dostep do pola
SCALAR&	get(size_t X,size_t Y)
	{
        size_t lindex=MainGeometry.get(X,Y);        assert(lindex!=rectangle_geometry::FULL);//Jedyne sprawdzanie zakresow zeby nie spowalniac przetestowanej symulacji
	return table[lindex];
	}

//Czyszczenie pojedynczego pola
void clean(size_t TargetX,size_t TargetY)//Konieczne bo pure-virtual
	{ get(TargetX,TargetY)=cleaner;}//Takiej metody nie mozna uzyc bezposrednio do wskaznikow

//Zamiana elementow
void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY)
	{
	SCALAR& Target=get(TargetX,TargetY);
	SCALAR& Source=get(SourceX,SourceY);
	SCALAR& tmp=Target;
	Target=Source;
	Source=tmp;
	}

bool filled(int X,int Y)//Sprawdzenie czy jest agent w tym miejscu
{
    return true; //Domyslnie cos zawsze jest bo to skalary
}

// Przypisanie polu wartosci RGB z bitmapy - domyslnie przeksztalcone na szarosc,
void assign_rgb(size_t TargetX,size_t TargetY,//tez konieczne bo pure-virtual
				unsigned char Red,unsigned char Green,unsigned char Blue,
				void* user_data=0)
	{
	//Uproszczone - moznaby zastosowac specjalny wzor z wagami
	unsigned long pom=(unsigned long)Red+(unsigned long)Green+(unsigned long)Blue;
	pom/=3;//Srednia intensywnosc - w zakresie 0..255
	get(TargetX,TargetY)=(unsigned char)pom;
	}

//Zmiana cleanera
void set_cleaner(SCALAR icleaner)//"Obiekt" do zamazywania)
{
    cleaner=icleaner;
}

//Tworzenie zrodla do czytania danych
/////////////////////////////////////////////////////////////////////////////////////
virtual /*rectangle_source_base**/
matrix_source<SCALAR>* make_source(const char* name)//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new matrix_source<SCALAR>(name,MainGeometry.get_width(),
									 MainGeometry.get_height(),
									 table.get_ptr_val());
	}
//Implementacja wejscia/wyjscia. Zwracaj 1 jesli sukces!
int		implement_output(ostream& o) const
	{
		o<<table<<' '<<cleaner;return 1;
	}
int		implement_input(istream& i)
	{
		i>>table;
		i>>cleaner;
		if(i.fail())
			return 0;
		return 1;
	}
};

//Szablon warstwy typu strukturalnego
template<class STRUCT_T>
class rectangle_layer_of_struct:public layer<STRUCT_T>,public rectangle_layer
//----------------------------------------------
{
wb_dynarray<STRUCT_T> table;
public:
rectangle_layer_of_struct(
				size_t Width,
				size_t Height
			    ):
			rectangle_layer(Width,Height),
			table(Width*Height)//odpowiednia ilosc pol
			{
			    //Zakladamy ze wystarcza to co robi bezparametrowy konstruktor struktury
                ;
			}

//rectangle_layer_of_struct(){} //Empty constructor ???

virtual	void Reinitialise()
	{
			size_t N=table.get_size();
			for(size_t i=0;i<N;i++)
			{
				STRUCT_T pom;		//construction!!!
				table[i]=pom;	    //assign!
			}
	}

//Metody Pure-virtual, które musza zastac zdefiniowane dla kazdej warstwy
///////////////////////////////////////////////////////////////////////////
const geometry_base* get_geometry()//Wypelnienie obowiazku pure-virtual
	{ return &MainGeometry;}

STRUCT_T& get(size_t index)//Daje dostep do elementu o indeksie obliczonym przez geometrie
	{ return table[index]; }

//Metody specyficzne dla warstw prostokatnych
///////////////////////////////////////////////////////////////////////////
//Bezposredni dostep do pola
STRUCT_T&	get(size_t X,size_t Y)
	{
	size_t lindex=MainGeometry.get(X,Y);
    assert(lindex!=rectangle_geometry::FULL);//Jedyne sprawdzanie zakresow zeby nie spowalniac przetestowanej symulacji
	return table[lindex];
	}

//Czyszczenie pojedynczego pola
void clean(size_t TargetX,size_t TargetY)//Konieczne bo pure-virtual
	{
		get(TargetX,TargetY)=STRUCT_T();//Czyszczenie bezparametrowym konstruktorem
	}

//Zamiana elementow
void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY)
	{
	STRUCT_T& Target=get(TargetX,TargetY);
	STRUCT_T& Source=get(SourceX,SourceY);
	char tmp[sizeof(STRUCT_T)];
	//Powinna byc opcja rozwijania takich funkcji jak memcpy.
	//Omija mechanizm konstrukcji/destrukcji
	memcpy(tmp,&Target,sizeof(STRUCT_T));
	memcpy(&Target,&Source,sizeof(STRUCT_T));
	memcpy(&Source,&tmp,sizeof(STRUCT_T));
	}

bool filled(int X,int Y)//Sprawdzenie czy jest agent w tym miejscu
{
    return true; //Struktura jest zawsze. Jak moze nie byc to trzeba uzyc warstwy innego typu
}

//Przykrycie funkcji init_from_bitmap
typedef void (STRUCT_T::* assign_rgb_fun)(unsigned char,unsigned char,unsigned char);
struct assign_rgb_stc
{
	assign_rgb_fun AssFun;
	assign_rgb_stc(assign_rgb_fun par):AssFun(par){}
};

int init_from_bitmap(const char* filename,
					 assign_rgb_stc user_fun)
{
	return rectangle_layer::init_from_bitmap(filename,&user_fun);
}


// Przypisanie polu wartosci RGB z bitmapy - domyslnie przeksztalcone na szarosc,
void assign_rgb(size_t TargetX,size_t TargetY,//tez konieczne bo pure-virtual
				unsigned char Red,unsigned char Green,unsigned char Blue,
				void* user_data=0
				)
	{
	assign_rgb_fun AssignFun=((assign_rgb_stc*)user_data)->AssFun;
	(get(TargetX,TargetY).*AssignFun)(Red,Green,Blue);
	}


//Tworzenie zrodel do czytania danych - najlepiej gdyby to byl szablon, ale to niestandardowe C++
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual //rectangle_source_base
struct_matrix_source<STRUCT_T,short>* make_source(const char* name,short STRUCT_T::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<STRUCT_T,short>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual //rectangle_source_base
struct_matrix_source<STRUCT_T,bool>* make_source(const char* name,bool STRUCT_T::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<STRUCT_T,bool>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}


virtual //rectangle_source_base
struct_matrix_source<STRUCT_T,unsigned>* make_source(const char* name,unsigned STRUCT_T::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<STRUCT_T,unsigned>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual //rectangle_source_base
struct_matrix_source<STRUCT_T,unsigned short>* make_source(const char* name,unsigned short STRUCT_T::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<STRUCT_T,unsigned short>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual //rectangle_source_base
struct_matrix_source<STRUCT_T,unsigned char>* make_source(const char* name,unsigned char STRUCT_T::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<STRUCT_T,unsigned char>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual //rectangle_source_base
struct_matrix_source<STRUCT_T,double>* make_source(const char* name,double STRUCT_T::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<STRUCT_T,double>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual
method_matrix_source<STRUCT_T,double>* make_source(const char* name,double (STRUCT_T::* method_ptr)(void) )
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<STRUCT_T,double>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

virtual
method_matrix_source<STRUCT_T,short int>* make_source(const char* name,short int (STRUCT_T::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<STRUCT_T,short int>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

virtual
method_matrix_source<STRUCT_T,bool>* make_source(const char* name,bool (STRUCT_T::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<STRUCT_T,bool>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

virtual
method_matrix_source<STRUCT_T,int>* make_source(const char* name,int (STRUCT_T::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<STRUCT_T,int>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

virtual
method_matrix_source<STRUCT_T,long>* make_source(const char* name,long (STRUCT_T::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<STRUCT_T,long>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

virtual
method_matrix_source<STRUCT_T,unsigned>* make_source(const char* name,unsigned (STRUCT_T::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<STRUCT_T,unsigned>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

virtual method_matrix_source<STRUCT_T,unsigned long>* make_source(const char* name,unsigned long (STRUCT_T::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<STRUCT_T,unsigned long>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

//Implementacja wejscia/wyjscia. Zwracaj 1 jesli sukces!
int		implement_input(istream& i)
	{
		i>>table;

		if(i.fail())
			return 0;
		return 1;
	}

int		implement_output(ostream& o) const
	{
		o<<table;
        if(o.fail())
			return 0;
		return 1;
	}

};


//Definicja interface'u agenta, który musi byc spelniony zeby warstwy mogły obslugiwac
class agent_base
//--------------------------------------------------
{
public:
		agent_base(){}
		agent_base(const agent_base& ini){}
virtual ~agent_base(){}
virtual void clean()=0;

//agent_base* clone() const { return new agent(*this);}
//friend ostream& operator << (ostream& o, agent a)
//friend istream& operator >> (ostream& i, agent a)

};


//Szablon warstwy typu strukturalnego zgodnegoa z agent_base
template<class AGENT>
class rectangle_layer_of_agents:public layer<AGENT>,public rectangle_layer
//----------------------------------------------
{
wb_dynarray<AGENT> table;
AGENT			   cleaner;//Obiekt do zamazywania
int				   use_cleaner;//Czy potrzebne jest uzycie cleanera,
							//   czy wystarczy konstruktor bezparametrowy i clean()
public:
rectangle_layer_of_agents(
				size_t Width,
				size_t Height
			    ):
			rectangle_layer(Width,Height),
			table(Width*Height),//odpowiednia ilosc pol
			use_cleaner(0),
			cleaner()
			{
			    //Zakladamy ze wystarcza to co robi bezparametrowy konstruktor agenta
                ;
			}

rectangle_layer_of_agents(
				size_t Width,
				size_t Height,
				int   iusecleaner,
				const AGENT* iclean
                )://Obiekt czyszczacy mial byc przekazywany przez adres zeby mozna oznaczac brak, ale to ryzykowny pomysl
			rectangle_layer(Width,Height),
			table(Width*Height),//odpowiednia ilosc pol
			use_cleaner(iusecleaner),
			cleaner(*iclean)
			{
				if(use_cleaner)//Na wypadek gdy konstruktor nie wystarcza
				{
					size_t N=Width*Height;
					for(size_t i=0;i<N;i++)
						table[i]=cleaner;
				}
			}


//rectangle_layer_of_agents(){} //Empty constructor ???
	void Reinitialise()
	{
		if(use_cleaner)//Na wypadek gdy konstruktor nie wystarcza
		{
			size_t N=table.get_size();
			for(size_t i=0;i<N;i++)
				table[i]=cleaner;
		}
		else
		{
			size_t N=table.get_size();
			for(size_t i=0;i<N;i++)
			{
				AGENT pom;		//construction!!!
				table[i]=pom;	//assign!
			}
		}
	}

//Metody Pure-virtual, które musza zastac zdefiniowane dla kazdej warstwy
///////////////////////////////////////////////////////////////////////////
const geometry_base* get_geometry()//Wypelnienie obowiazku pure-virtual
	{ return &MainGeometry;}

AGENT& get(size_t index)//Daje dostep do elementu o indeksie obliczonym przez geometrie
	{ return table[index]; }

//Metody specyficzne dla warstw prostokatnych
///////////////////////////////////////////////////////////////////////////
//Bezposredni dostep do pola
AGENT&	get(size_t X,size_t Y)
	{
	size_t lindex=MainGeometry.get(X,Y);
    assert(lindex!=rectangle_geometry::FULL);//Jedyne sprawdzanie zakresow zeby nie spowalniac przetestowanej symulacji
	return table[lindex];
	}

AGENT& operator () (size_t X,size_t Y)
	{
		return get(X,Y);
	}

//Czyszczenie pojedynczego pola
void clean(size_t TargetX,size_t TargetY)//Konieczne bo pure-virtual
	{
	if(use_cleaner)
		get(TargetX,TargetY)=cleaner;
		else
		get(TargetX,TargetY).clean();
	}

//Zamiana elementow
void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY)
	{
	AGENT& Target=get(TargetX,TargetY);
	AGENT& Source=get(SourceX,SourceY);
	char tmp[sizeof(AGENT)];
	//Powinna byc opcja rozwijania takich funkcji jak memcpy.
	//Omija mechanizm konstrukcji/destrukcji
	memcpy(tmp,&Target,sizeof(AGENT));
	memcpy(&Target,&Source,sizeof(AGENT));
	memcpy(&Source,&tmp,sizeof(AGENT));
	}

bool filled(int X,int Y)//Sprawdzenie czy jest agent w tym miejscu
{
    AGENT& Target=get(X,Y);
    return Target.is_alive();
    //return true; //Domyslnie cos zawsze jest bo to skalary
}

//Prykrycie funkcji init_from_bitmap
typedef void (AGENT::* assign_rgb_fun)(unsigned char,unsigned char,unsigned char);
struct assign_rgb_stc
{
	assign_rgb_fun AssFun;
	assign_rgb_stc(assign_rgb_fun par):AssFun(par){}
};

int init_from_bitmap(const char* filename,
					 assign_rgb_stc user_fun)
{
	return rectangle_layer::init_from_bitmap(filename,&user_fun);
}


// Przypisanie polu wartosci RGB z bitmapy - domyslnie przeksztalcone na szarosc,
void assign_rgb(size_t TargetX,size_t TargetY,//tez konieczne bo pure-virtual
				unsigned char Red,unsigned char Green,unsigned char Blue,
				void* user_data=0
				)
	{
	assign_rgb_fun AssignFun=((assign_rgb_stc*)user_data)->AssFun;
	(get(TargetX,TargetY).*AssignFun)(Red,Green,Blue);
	}


//Tworzenie zrodel do czytania danych - najlepiej gdyby to byl szablon, ale to niestandardowe C++
///////////////////////////////////////////////////////////////////////////////////////////////////
virtual /*rectangle_source_base**/
struct_matrix_source<AGENT,bool>* make_source(const char* name,bool AGENT::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<AGENT,bool>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual /*rectangle_source_base**/
struct_matrix_source<AGENT,short>* make_source(const char* name,short AGENT::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<AGENT,short>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual /*rectangle_source_base**/
struct_matrix_source<AGENT,unsigned short>* make_source(const char* name,unsigned short AGENT::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<AGENT,unsigned short>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual /*rectangle_source_base**/
struct_matrix_source<AGENT,int>* make_source(const char* name,int AGENT::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<AGENT,int>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual /*rectangle_source_base**/
struct_matrix_source<AGENT,unsigned>* make_source(const char* name,unsigned AGENT::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<AGENT,unsigned>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual /*rectangle_source_base**/
struct_matrix_source<AGENT,long>* make_source(const char* name,long AGENT::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<AGENT,long>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual /*rectangle_source_base**/
struct_matrix_source<AGENT,unsigned long>* make_source(const char* name,unsigned long AGENT::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<AGENT,unsigned long>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual /*rectangle_source_base**/
struct_matrix_source<AGENT,unsigned char>* make_source(const char* name,unsigned char AGENT::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<AGENT,unsigned char>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual /*rectangle_source_base**/
struct_matrix_source<AGENT,double>* make_source(const char* name,double AGENT::* field_ptr)
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new struct_matrix_source<AGENT,double>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   field_ptr
										   );
	}

virtual
method_matrix_source<AGENT,bool>* make_source(const char* name,bool (AGENT::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<AGENT,bool>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}



virtual
method_matrix_source<AGENT,short>* make_source(const char* name,short (AGENT::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<AGENT,short int>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

virtual
method_matrix_source<AGENT,unsigned short>* make_source(const char* name,unsigned short (AGENT::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<AGENT,unsigned short>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

virtual
method_matrix_source<AGENT,int>* make_source(const char* name,int (AGENT::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
												assert(name!=NULL);
												assert(method_ptr!=NULL);
	return new method_matrix_source<AGENT,int>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

virtual
method_matrix_source<AGENT,unsigned>* make_source(const char* name,unsigned (AGENT::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<AGENT,unsigned>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

virtual
method_matrix_source<AGENT,long>* make_source(const char* name,long (AGENT::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<AGENT,long>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

virtual
method_matrix_source<AGENT,unsigned long>* make_source(const char* name,unsigned long (AGENT::* method_ptr)(void))
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<AGENT,unsigned long>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

virtual
method_matrix_source<AGENT,double>* make_source(const char* name,double (AGENT::* method_ptr)(void) )
//Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
	{
	return new method_matrix_source<AGENT,double>(
                                           name,
                                           MainGeometry,
										   table.get_ptr_val(),
										   method_ptr
										   );
	}

//Implementacja wejscia/wyjscia. Zwracaj 1 jesli sukces!
int		implement_input(istream& i)
	{
		i>>table;
		i>>cleaner;
		i>>use_cleaner;
		if(i.fail())
			return 0;
		return 1;
	}

int		implement_output(ostream& o) const
	{
		o<<table;
		o<<' '<<cleaner;
		o<<' '<<use_cleaner<<' ';
		return 1;
	}

};

//Szablon warstwy wskazników do typu strukturalnego zgodnego z agent_base
template<class AGENT>
class rectangle_layer_of_ptr_to_agents:public layer<AGENT>,public rectangle_layer
//--------------------------------------------------------------
{
    wb_dynarray<wb_ptr<AGENT> >	table;
    wb_ptr<AGENT>				initer;//Obiekt do zamazywania
    wb_ptr<AGENT>				empty_guard;//Zwracany jako reprezentant pustych pol
    int						full_allocation;//Wszystkie wskazniki maja byc pelne

public:
    rectangle_layer_of_ptr_to_agents(
        size_t Width,
        size_t Height,
        const  AGENT* iiniter=NULL,
        int    allocate_all=0
        )://Obiekt czyszczacy przekazywany przez adres zeby mozna oznaczac brak
        rectangle_layer(Width,Height),
        table(Width*Height),//odpowiednia ilosc pol
        initer((iiniter?iiniter->clone():NULL)),
        empty_guard((iiniter?iiniter->clone():NULL)),
        full_allocation(allocate_all)
        {
            assert(sizeof(wb_ptr<AGENT>)==sizeof(AGENT*));//Bedzie taki cast
            if(allocate_all)//Na wypadek gdy konstruktor nie wystarcza
                reallocate_all();//Realokuje lub klonuje wszystkie
            else
                deallocate_all();//Wpisuje wszedzie NULL dla pewnosci
        }

    void reallocate_all()
    {
        size_t N=table.get_size();
        if(!initer)
        {
            for(size_t i=0;i<N;i++)
                table[i]=new AGENT();
        }
        else
        {
            for(size_t i=0;i<N;i++)
                table[i]=initer->clone();
        }
    }

    void deallocate_all()
    {
        size_t N=table.get_size();
        for(size_t i=0;i<N;i++)
            table[i]=NULL;
    }

    void deallocate_not_OK()
    {
        size_t N=table.get_size();
        for(size_t i=0;i<N;i++)
            if(table[i])
                if(!table[i]->IsOK()) //Jesli nie jest OK to go kasuje
                    table[i]=NULL;
    }


    //Metody Pure-virtual, które musza zastac zdefiniowane dla kazdej warstwy
    ///////////////////////////////////////////////////////////////////////////
    const geometry_base* get_geometry()//Wypelnienie obowiazku pure-virtual
    {
        return &MainGeometry;
    }

    AGENT& get(size_t index)//Daje dostep do elementu o indeksie obliczonym przez geometrie
    {
        //assert(index<);Sprawdzenie zakresu
                                            assert(index!=any_layer_base::FULL);
        if(!table[index]) //wb_ptr ma NULL
            return *empty_guard;
        else
            return *table[index];
    }

    int   is_empty(const AGENT& check)
    {
        return &check == empty_guard.get_ptr_val();
    }

    //Dostep do inteligentnego wskaznika
    wb_ptr<AGENT>&	get_ptr(size_t index)
    {                                                   assert(index!=any_layer_base::FULL);
        return table[index];
    }


    //Metody specyficzne dla warstw prostokatnych
    ///////////////////////////////////////////////////////////////////////////

    //Bezposredni dostep do struktury
    AGENT&	get(size_t X,size_t Y)
    {
        size_t lindex=MainGeometry.get(X,Y);           assert(lindex!=any_layer_base::FULL);//Jedyne sprawdzanie zakresow zeby nie spowalniac przetestowanej symulacji
        return *table[lindex];
    }

	AGENT& operator () (size_t X,size_t Y)
	{
		return get(X,Y);
	}

    //Dostep do inteligentnego wskaznika
    wb_ptr<AGENT>&	get_ptr(size_t X,size_t Y)
    {
        size_t lindex=MainGeometry.get(X,Y);            assert(lindex!=any_layer_base::FULL);//Jedyne sprawdzanie zakresow zeby nie spowalniac przetestowanej symulacji
        return table[lindex];
    }

    bool filled(int X,int Y)
    {
        if(get_ptr(X,Y)!=NULL)
            return true;
        else
            return false;
    }

    //Czyszczenie pojedynczej struktury
    void clean(size_t TargetX,size_t TargetY)//Konieczne bo pure-virtual
    {
        if(full_allocation)
        {
            get_ptr(TargetX,TargetY)=initer->clone();
        }
        else
        {
            get_ptr(TargetX,TargetY)=NULL;//Automatyczna destrukcja
        }
    }

    //Zamiana elementow
    void swap(size_t TargetX,size_t TargetY,size_t SourceX,size_t SourceY)
    {
        wb_sptr<AGENT>& Target=get_ptr(TargetX,TargetY);
        wb_sptr<AGENT>& Source=get_ptr(SourceX,SourceY);
        wb_sptr<AGENT>  tmp;
        //Kopiuje wskazniki wiec
        //omija mechanizm konstrukcji/destrukcji
        tmp=Target;		//Target jest teraz pusty,
        Target=Source;	//teraz Source jest pusty
        Source=tmp;		//a śteraz tmp jest pusty, a Source i Target sa zamienione
    }

    //Przykrycie funkcji init_from_bitmap
    typedef void (AGENT::* assign_rgb_fun)(unsigned char,unsigned char,unsigned char);


    //Struktura do przechowywania (?) funkcji wczytujacej kolory
    struct assign_rgb_stc
    {
        assign_rgb_fun AssFun;
        assign_rgb_stc(assign_rgb_fun par):AssFun(par){}
    };


    int init_from_bitmap(const char* filename,
        assign_rgb_stc user_fun)
    {
        return rectangle_layer::init_from_bitmap(filename,&user_fun);
    }

    // Przypisanie polu wartosci RGB z bitmapy - domyslnie przeksztalcone na szarosc,
    void assign_rgb(size_t TargetX,size_t TargetY,//tez konieczne bo pure-virtual
        unsigned char Red,unsigned char Green,unsigned char Blue,void* user_data)
    {
        assign_rgb_fun AssignFun=((assign_rgb_stc*)user_data)->AssFun;
        wb_ptr<AGENT>& Target=get_ptr(TargetX,TargetY);

        if(!Target)
        {
            if(!initer)
                Target=new AGENT();
            else
                Target=initer->clone();
        }
        assert(Target!=NULL);
        assert(AssignFun!=NULL);
        AGENT* pom=Target.get_ptr_val();
        (pom->*AssignFun)(Red,Green,Blue);
    }

    //Tworzenie zrodel do czytania danych - najlepiej gdyby to byl szablon, ale to niestandardowe C++
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual
        ptr_to_struct_matrix_source<AGENT,unsigned>* make_source(const char* name,unsigned AGENT::* field_ptr)
        //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new ptr_to_struct_matrix_source<AGENT,unsigned>( name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            field_ptr
            );
    }

    virtual
        ptr_to_struct_matrix_source<AGENT,unsigned short>* make_source(const char* name,unsigned short AGENT::* field_ptr)
        //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new ptr_to_struct_matrix_source<AGENT,unsigned short>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            field_ptr
            );
    }

        virtual
        ptr_to_struct_matrix_source<AGENT,unsigned long>* make_source(const char* name,unsigned long AGENT::* field_ptr)
        //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new ptr_to_struct_matrix_source<AGENT,unsigned long>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            field_ptr
            );
    }

    virtual
        ptr_to_struct_matrix_source<AGENT,short>* make_source(const char* name,short AGENT::* field_ptr)
        //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new ptr_to_struct_matrix_source<AGENT,short>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            field_ptr
            );
    }


    virtual
        ptr_to_struct_matrix_source<AGENT,unsigned char>* make_source(const char* name,unsigned char AGENT::* field_ptr)
        //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new ptr_to_struct_matrix_source<AGENT,unsigned char>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            field_ptr
            );
    }

    virtual
        ptr_to_struct_matrix_source<AGENT,double>* make_source(const char* name,double AGENT::* field_ptr)
        //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new ptr_to_struct_matrix_source<AGENT,double>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            field_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,double>* make_source(const char* name,double (AGENT::* method_ptr)(void) )
        //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new method_by_ptr_matrix_source<AGENT,double>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,short>* make_source(const char* name,short (AGENT::* method_ptr)(void))
        //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new method_by_ptr_matrix_source<AGENT,short>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }


    virtual
        method_by_ptr_matrix_source<AGENT,int>* make_source(const char* name,int (AGENT::* method_ptr)(void))
        //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new method_by_ptr_matrix_source<AGENT,int>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,unsigned>* make_source(const char* name,unsigned (AGENT::* method_ptr)(void))
        //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new method_by_ptr_matrix_source<AGENT,unsigned>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,long int>* make_source(const char* name,long int (AGENT::* method_ptr)(void))
        //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new method_by_ptr_matrix_source<AGENT,long int>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    virtual
        method_by_ptr_matrix_source<AGENT,unsigned long>* make_source(const char* name,unsigned long (AGENT::* method_ptr)(void))
        //Tworzy zawsze/wielokrotnie taka sama, ale nie ta sama warstwe
    {
        return new method_by_ptr_matrix_source<AGENT,unsigned long>(name,
            MainGeometry,
            (AGENT**)table.get_ptr_val(),
            method_ptr
            );
    }

    //Implementacja wejscia/wyjscia. Zwracaj 1 jesli sukces!
    int		implement_output(ostream& o) const
    {
        o<<table<<' '<<initer<<' ';//Obiekt do zamazywania
        o<<empty_guard<<' '<<full_allocation<<' ';//Zwracany jako reprezentant pustych pol
        return 1;
    }

    int		implement_input(istream& i)
    {
        i>>table;
        i>>initer;
        i>>empty_guard;
        i>>full_allocation;
        if(i.fail())
            return 0;
        return 1;
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

