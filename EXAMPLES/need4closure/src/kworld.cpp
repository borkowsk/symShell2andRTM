#include <string.h>
#include <math.h>
#include "krand.h"
#include "kworld.h"
#include "wb_ptrio.h"

extern const RAMKA=4;
extern const char* SYMULATION_NAME;

//Statyczne pola kagentow
////////////////////////////////////////////////////////////////
short	kagent::max_sila=256;//Maksymalna sila agenta
int     kagent::treshold=256;//Granica domkniecia pogladu
double  kagent::Majority=0.10;//Poczatkowa ilosc czarnych - lewych
double  kagent::Minority=0.05;//Poczatkowa ilosc bialych - prawych 
double	kagent::NoiseLevel=0;//Prawd. spontanicznej zmiany

//KONSTRUKCJA	SWIATA
////////////////////////////////////
extern unsigned internal_log;
extern unsigned spatial_correlation_mode;

kworld::kworld(size_t Width,		//Szerokosc torusa macierzy agentow
               char* log_name,	//Nazwa pliku do zapisywania histori
               char* mapl_name,	//Nazwa (bit)mapy inicjujacej "skladowe"
               char* mapp_name,	//Nazwa (bit)mapy inicjujacej "sily"
               char* live_mask,	//Czarne w tej mapie sa kasowane
               double noise,		//Szum informacyjny
               short	max_sila,	//Maksymalna sila agenta
                             
               short	ile_sasiad, //8==Gestosc losowania sasiedztwa
               double how_use_self,//Z jaka waga ma brac siebie
               double need_for_closure,//Glowny parametr 
               bool	synchronicly,
               
               short treshold,
               double spontanic,
               
               double fill,
               double migrprob,
               double majority,
               double minority
               ):	
world(log_name,50),		
MaplName(clone_str(mapl_name)),//Nazwa (bit)mapy 1. inicjujacej agentow					
MappName(clone_str(mapp_name)),//Nazwa (bit)mapy 2. inicjujacej agentow					
MaskName(clone_str(live_mask)),//Nazwa bitmapy maskujacej (kasujacej agentow)
//Sub-obiekty wlasciwe dla tej symulacji
MyWidth(Width),
Agenci(Width,Width,NULL),//Initer == NULL wiec tworzone przez konstruktor a nie klonowanie
MaxSila(max_sila),	//Maksymalna sila agenta
Treshold(treshold), //Prog sily
IleSasiad(ile_sasiad),	//8==Gestosc sasiedztwa
Noise(noise),
Fill(fill),
Migr(migrprob),
WeightOfSelf(how_use_self),//Z jaka waga brac siebie pod uwage (0..1
NeedForClosure(need_for_closure),//Z jaka waga brani s¹ inni domyslnie 1
Synchronic(synchronicly),

//Wskazniki do podstawowych seri danych
Firsts(NULL),
Seconds(NULL),
Powers(NULL),//,Classif(NULL)
ForRight(NULL),
ForLeft(NULL),
ptrStres(NULL),
ptrClsSize(NULL),
ptrLastChanged(NULL),
ptrLastMigration(NULL),
CountCh(0),
CountMig(0)
{//!!!Niewiele mozna zrobic bo nie mozna tu jeszcze polegac na wirtualnych metodach klasy swiat
    kagent::Majority=majority;
    kagent::Minority=minority;
    set_simulation_name(SYMULATION_NAME);
    kagent::max_sila=MaxSila;//Maksymalna sila agenta
    kagent::NoiseLevel=spontanic;
}


//AKCJE SYMULACYJNE
//////////////////////
//////////////////////
void kworld::after_read_from_image()
//actions after read state from file. Aktualizacja pol static kagent'a!!!
{   
    //...
}

// stan startowy symulacji
void kworld::initialize_layers()
//-------------------------------------
{
    kagent::max_sila=MaxSila;//Maksymalna sila agenta
    
    static first=1;//EWENTUALNE WYLACZENIE WYDRUKOW JESLI SYMULACJA 
    
    //...wydruk wartosci parametrow symulacji
    if(first)
        Log.GetStream()
            <<"SIMULATION:"<<SYMULATION_NAME
            <<"\nMax Power="<<Log.separator()<<MaxSila
        //		<<"\nTresh of Power="<<Log.separator()<<TrsSila
        //		<<"\nNum of Kl="<<Log.separator()<<IleKate
            <<"\nNoise %="<<Log.separator()<<Noise*100
        //		<<"\nSelf="<<Log.separator()<<WeightOfSelf
            <<"\nNforC="<<Log.separator()<<NeedForClosure
        //		<<"\nNaighborhood="<<Log.separator()<<IleSasiad<<"/("<<(1+2*OdlSasiad)<<"*"<<(1+2*OdlSasiad)<<")\n"
            <<endl;
    
    //			USTALANIE STANÓW AGENTÓW 
    //Wczytuje uzywajac konstruktora lub klonowania gdy niema, wiec inicjuje reszte pól.
    int from1= Agenci.init_from_bitmap(MappName.get_ptr_val(),kagent::assignPow);
    int from2= Agenci.init_from_bitmap(MaplName.get_ptr_val(),kagent::assign_curr);
    //   int from3= Agenci.init_from_bitmap(MaplName.get_ptr_val(),kagent::assign_prev);
    
    //Jesli nie zainicjowane z pliku to prowizoryczna inicjacja przez konstruktory lub klonowanie
    if(from1!=1 && from2!=1)
        Agenci.reallocate_all();
    
    //Zabija agenta, jesli w masce jest czarny kolor
    if(Agenci.init_from_bitmap(MaskName.get_ptr_val(),kagent::killBlack)==1 )
        Agenci.deallocate_not_OK();
    
    if(Fill<1)//Dealokacja nadmiarow
    {
        size_t how_many=(1-Fill)*sqr(MyWidth);
        Agenci.clean_randomly(how_many);
    }

    first=0;//Koniec pierwszego wywolania - WYLACZA WYPISYWANIE PARAMETROW W KOLEJNYCH POWTORZENIACH SYMULACJI
}

//Pojedynczy krok symulacji
void kworld::simulate_one_step()
//---------------------------------------
{   
    CountCh=CountMig=0; //Zerowanie licznikow dynamizmu
    const rectangle_geometry* MyGeom=dynamic_cast<const rectangle_geometry*>(Agenci.get_geometry());    assert(MyGeom!=NULL);
    
    if(Synchronic)//Jesli synchronicznie to inna petla niz przy monte-carlo
    {//Idziemy po agentach pelnym iteratorem a stan agentow zmieniamy dopiero potem
        
        iteratorh Iglobal=MyGeom->make_global_iterator();//Alokujemy iterator po wszystkich agentach
        while(Iglobal)
        {
            size_t index=MyGeom->get_next(Iglobal);//Uzyskujemy index  agenta	
            
            assert(index!=MyGeom->FULL);				//... tutaj nie powinno sie zdarzyc
            
            kagent& CenterAgent=*(Agenci.get_ptr(index).get_ptr_val());// Uzyskujemy referencje do agenta omijajac asercje na NULL
            
            if(Agenci.is_empty(CenterAgent))	// Sprawdzamy czy nie jest to pusta komórka (NULL)
                continue;						// bo wtedy robic dalej by³oby bez sensu.
            
            if(CenterAgent.DurCh)
                continue;                       //Ten juz byl sprawdzany, tylko ze sie przeniosl
            
            
            CheckChange(MyGeom,index,CenterAgent); //Sprawdzamy zmiane stanu
            
        }        
        MyGeom->destroy_iterator(Iglobal);// upewniamy sie ze iterator zostanie usuniety
        
        
        Iglobal=MyGeom->make_global_iterator();//Tworzymy nowy iterator i iterujemy od poczatku
        while(Iglobal)
        {
            size_t index=MyGeom->get_next(Iglobal);//Uzyskujemy index  agenta	
            
            assert(index!=MyGeom->FULL);				//... tutaj nie powinno sie zdarzyc
            
            kagent& CenterAgent=*(Agenci.get_ptr(index).get_ptr_val());// Uzyskujemy referencje do agenta omijajac asercje na NULL
            
            if(Agenci.is_empty(CenterAgent))	// Sprawdzamy czy nie jest to pusta komórka (NULL)
                continue;
            
            CenterAgent.update();  //Tu dopiero NADAJEMY nowy stan agentowi
        }	                
        MyGeom->destroy_iterator(Iglobal);// upewniamy sie ze iterator zostanie usuniety        
    }
    else
    {
        iteratorh Monte=MyGeom->make_random_global_iterator();	//Alokujemy iterator Monte-Carlo
        while(Monte)//Idziemy po agentach iteratorem Monte-Carlo. Niektórzy moga sie powtórzyc
        {	
            size_t index=MyGeom->get_next(Monte);//Uzyskujemy index losowo wybranego agenta	
            
            assert(index!=MyGeom->FULL);				//... tutaj nie powinno sie zdarzyc
            
            kagent& CenterAgent=*(Agenci.get_ptr(index).get_ptr_val());// Uzyskujemy referencje do agenta omijajac asercje na NULL
            if(Agenci.is_empty(CenterAgent))	// Sprawdzamy czy nie jest to pusta komórka (NULL)
                continue;						// bo wtedy robic dalej by³oby bez sensu.
            
            CheckChange(MyGeom,index,CenterAgent);//Czy zaszla zmiana stanu
            
            CenterAgent.update();//Tu NADAJEMY nowy stan agentowi				
        }
        MyGeom->destroy_iterator(Monte);// upewniamy sie ze iterator zostanie usuniety
    }
    
    //Wpisujemy wartosci licznikow do zrodel eksportujacych je dla wizualizacji
    ptrLastChanged->change_ptr(&CountCh); //Alternatywna metoda dla oznacznia braku/obecnosci policzonych danych
    ptrLastMigration->change_ptr(&CountMig);//Alternatywna metoda dla oznacznia braku policzonych danych
}

int kworld::DoMigration(const rectangle_geometry* MyGeom, //Ta procedura jest napisana nie-ogolnie, tj. w uzaleznieniu od prostokatnego typu geometrii
                        size_t index,
                        kagent& CenterAgent
                        )
{
    size_t SouX,SouY,TarX,TarY;
    MyGeom->WhatCoordinates(index,SouX,SouY);//Nie ma co sprawdzac czy dobrze bo przeciez bylo dobrze
    
    do{
        TarX=RANDOM(MyGeom->get_width());
        TarY=RANDOM(MyGeom->get_height());
    }while(Agenci.filled(TarX,TarY));//Dopuki nie znajdzie pustego
    
    Agenci.swap(TarX,TarY,SouX,SouY);//Zamienia miejsce
    return MyGeom->get(TarX,TarY);//Nowa pozycja w postaci liniowej 
}

int kworld::CheckChange(const rectangle_geometry* MyGeom,
                        size_t index,
                        kagent& CenterAgent
                        )//KOD NA SZUKANIE ZMIAN
{ 
    int testowanie=0;
    // Alokujemy iterator sasiedztwa o boku 2*NeedForClosure, zawierajacy "IleSasiad" losowych s¹siadów.
    iteratorh Neigh=MyGeom->make_random_neighbour_iterator(index,NeedForClosure,IleSasiad);
    
    while(Neigh)
    {
         size_t index2=MyGeom->get_next(Neigh);//Uzyskujemy index sasiada		
         if(index2==MyGeom->FULL || index2==index)	//Jesli poza obszarem symulacji lub w 
                  continue;				//centrum obszaru to dalej byloby bez sensu.
         
         kagent& SecAgent=*(Agenci.get_ptr(index2).get_ptr_val());//Uzyskujemy referencje do sasiada omijajac asercje na NULL
         if(Agenci.is_empty(SecAgent))		//Sprawdzamy czy nie jest to pusta komórka (NULL)
                    continue;					   // bo wtedy robic dalej by³oby bez sensu.                  
    
         double distance=MyGeom->get_distance(index,index2);                        assert(distance>=1);
         
         double impactfact=pow(SecAgent.Power,NeedForClosure)*1/sqrt(distance);
         double ownimpact=pow(CenterAgent.Power,NeedForClosure)*WeightOfSelf;

         double leftimpact=0,leftbase=0;
         double rightimpact=0,rightbase=0;

         if(SecAgent.First<=0)
         {
            leftimpact=impactfact*SecAgent.ForLeft;
            leftbase=ownimpact*CenterAgent.ForLeft;
            CenterAgent.ForLeft=pow(leftbase+leftimpact,1./(2.*NeedForClosure));    
         }
         
         if(SecAgent.First>=0)
         {
            rightimpact=impactfact*SecAgent.ForRight; 
            rightbase=ownimpact*CenterAgent.ForRight;
            CenterAgent.ForRight=pow(rightbase+rightimpact,1./(2.*NeedForClosure));     
         }         
    }


    //Tu z pewnym prawdopodobienstwem moglaby zajsc zmiana postawy
    if(abs(CenterAgent.ForRight-CenterAgent.ForLeft)>Treshold)    
        {
            if(CenterAgent.ForRight>CenterAgent.ForLeft)
                CenterAgent.new_attitude(1);
            else
                CenterAgent.new_attitude(-1);
            CountCh++;
        }
        else
        CenterAgent.new_attitude(CenterAgent.First);//nic nie trzeba zmieniac wiec nic nie zmieniamy (faktycznie)

    return 0;
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

  /*
    if(DRAND()<=kagent::NoiseLevel)//Rzadka, spontaniczna zmiana pogladu
    {
    int atti=RANDOM(IleKate);
    assert(0<=atti && atti<IleKate);
    
      CenterAgent.Second=atti;			//zmieniamy w agencie centralnym
      return 1;
      }
      
        
          //TABLICA POMOCNICZA
          wb_dynarray<int> Firsts(IleKate);
          assert(Firsts.IsOK());
          //Czyszczenie licznika
          memset(Firsts.get_ptr_val(),0,sizeof(int)*IleKate);
          
            // Alokujemy iterator sasiedztwa
            ::iteratorh Neigh=NULL;
            
              if(BierzWszystko)
              {
              Neigh=MyGeom->make_neighbour_iterator(index,OdlSasiad);
              }
              else
              {
              Neigh=MyGeom->make_random_neighbour_iterator(index,OdlSasiad,IleSasiad);
              }
              
                //iteratorh Neigh=MyGeom->make_neighbour_iterator(index,OdlSasiad);
                unsigned zliczanie=0;//Zliczanie sasiadów
                
                  while(Neigh)
                  {
                  size_t index2=MyGeom->get_next(Neigh);//Uzyskujemy index sasiada		
                  if(index2==FULL || index2==index)	//Jesli poza obszarem symulacji lub w 
                  continue;				//centrum obszaru to dalej byloby bez sensu.
                  
                    kagent& PeryfAgent=*(Agenci.get_ptr(index2).get_ptr_val());//Uzyskujemy referencje do sasiada omijajac asercje na NULL
                    if(Agenci.is_empty(PeryfAgent))		//Sprawdzamy czy nie jest to pusta komórka (NULL)
                    continue;					   // bo wtedy robic dalej by³oby bez sensu.
                    
                      zliczanie++;
                      //Dodawanie sil sasiadow do licznikow w tablicach
                      Firsts[PeryfAgent.First]+=unsigned(PeryfAgent.Power*NeedForClosure);//W za³o¿eniu to nie sa duze liczby
                      }
                      
                        MyGeom->destroy_iterator(Neigh);	// upewniamy sie ze iterator zostanie usuniety
                        //Zlicza wylosowanych agentow
                        testowanie++;	
                        
                          //Dodawanie wlasnych sil do licznikow w tablicach o ile WeightOfSelf>0
                          if(WeightOfSelf)
                          {
                          Firsts[CenterAgent.First]+=unsigned(CenterAgent.Power*WeightOfSelf);//W za³o¿eniu to nie sa duze liczby
                          }
                          
                            //Szukanie maksimow
                            int maxF=0,indF=-1;
                            
                              int offset=RANDOM(IleKate);                     assert(0<=offset && offset<IleKate);//Jak IleKate==2 to 0 albo 1 itd..
                              
                                for(int g=0;g<IleKate;g++)
                                {
                                int h=(g+offset)%IleKate;
                                assert(h>=0 && h<IleKate);
                                //Dodawanie szumu
                                if(Firsts[h]>0)
                                Firsts[h]+=long(DRAND()*Noise*(4.5*MaxSila));
                                
                                  //Testowanie
                                  if(Firsts[h]>maxF)
                                  {
                                  maxF=Firsts[h];
                                  indF=h;
                                  }
                                  
                                    }
                                    
                                      assert(indF!=-1);//Czy jest maksimum 
                                      
                                        //Trzeba cos zrobic, ale co?
                                        if(Migr!=0 &&  Fill<1 && (Migr==1 || DRAND()<Migr)  )//Decyzja 
                                        { //Migracja
                                        if(CenterAgent.First!=indF) //Ale tylko gdy trzebaby zmienic poglad
                                        {
                                        CenterAgent.Press=maxF;//Specjalny znacznik 
                                        CenterAgent.new_attitude(CenterAgent.First); //wiec nic nie zmieniamy w jego pogladach
                                        DoMigration(MyGeom,index,CenterAgent);//za to zmieniamy pozycje w swiecie
                                        CountMig++;
                                        return 0;//Trzeba wyjsc bo "index" jest nieaktualny i zasygnalizowac to wyzej
                                        }
                                        else
                                        {
                                        CenterAgent.Press=0;
                                        CenterAgent.new_attitude(CenterAgent.First);//nie potrzeba migrowac wiec nic nie zmieniamy        
                                        return 0;
                                        }
                                        }
                                        else 
                                        { //Konformizm
                                        if(CenterAgent.First!=indF) //Jesli znaleziono cos nowego
                                        {
                                        CenterAgent.Press=maxF;
                                        CenterAgent.new_attitude(indF);		//zmieniamy w agencie centralnym            
                                        CountCh++;
                                        return 1;
                                        }
                                        else                                    //nie znaleziono
                                        {
                                        CenterAgent.Press=0;
                                        CenterAgent.new_attitude(CenterAgent.First);//wiec nic nie zmieniamy        
                                        return 0;
                                        }
                                        }
*/