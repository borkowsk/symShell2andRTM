#include <ctype.h> //Typy znaków - http://www.cppreference.com/wiki/string/character_classes
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream> //Standardowe wejscie/wyjscie na konsole
#include <fstream>  //Bedzie wejscie z pliku dyskowego

#include "TabDelimited.hpp"
//#include "INCLUDE/wbminmax.hpp"


using namespace std; // Łatwe użycie nazw z przestrzeni "std"
using wbrtm::TabelaTabDelimited;

/// \brief Dostęp do nie zmienialnej treści komórki
const char* TabelaTabDelimited::operator ()(int w,int k) const
       {  //...Jakies sprawdzenia???
		  if(w<IleWierszy()&& k<IleKolumn())
		  {
			unsigned index=(w*Wiersz)+k;
			return Tresc[index].c_str();
		  }
		  else
		  {
			if(Opisowo>0)
				cerr<<"Wadliwy indeks tabeli danych (od 0!) - wiersz: "<<w<<" kolumna: "<<k<<endl;
			if(errno=ERANGE,!AktualnaFunkcjaBledu(1,*this,cerr)) //Czy próba kontynuowania
				return "???";
			else
			exit(1);
		  }
       }

//Pe³ny dostêp do zawartosci komórki
string& TabelaTabDelimited::operator () (int w,int k)
       {
		  static string dummy("???");

		  //...Jakies sprawdzenia???
		  if(w<IleWierszy() && k<IleKolumn())
		  {
			unsigned index=(w*Wiersz)+k;
			return Tresc[index];
		  }
		  else
		  {
			if(Opisowo>0)
				cerr<<"Wadliwy indeks tabeli danych (od 0!) - wiersz: "<<w<<" kolumna: "<<k<<endl;
			if(errno=ERANGE,!AktualnaFunkcjaBledu(1,*this,cerr)) //Czy próba kontynuowania
				return dummy;
			else
			exit(1);
		  }
	   }

string   TabelaTabDelimited::ZmienNazwe(const char* nowa)
{
	string stara=NazwaPliku;
	NazwaPliku=nowa;
	return stara;
}

const char*  TabelaTabDelimited::JakaNazwa() const
{
	return NazwaPliku.c_str();
}

void  TabelaTabDelimited::UstalRozmiar(int w,int k)
//Decyzja ustalaj¹ca rozmiary
{
    if(Tresc!=nullptr)
		delete []Tresc;//Jesli cos bylo
    Rozmiar=w*k;
    Wiersz=k;//Dlugos wiersza czyli ile kolumn
    Tresc=new string[Rozmiar];
}

TabelaTabDelimited::~TabelaTabDelimited()
//Destruktor musi byæ bo s¹ wewn¹trz obiekty
{
 if(Tresc!=nullptr)
          delete [/*Rozmiar*/]Tresc;
#ifndef _NDEBUG
if(this->Opisowo>0)
 cerr<<"Sukces dealokacji w destruktorze dla "<<NazwaPliku<<endl;
 //cin.get();
#endif
 Tresc=nullptr;
 Wiersz=0;Rozmiar=0;
}

void TabelaTabDelimited::PrzekopiujZ(const TabelaTabDelimited& Zrodlo,
						unsigned startw,//=0, //Pocz¹tkowy wiersz
						unsigned startk,//=0,  //Poczatkowa kolumna
						unsigned endw,//=-1, //Koncowy wiersz albo do konca
						unsigned endk,//=-1  //Koncowa kolumna albo do konca
						unsigned celw,//=0,  //Pierwsza komorka docelowa - numer wiersza
						unsigned celk//=0	  //Pierwsza komorka docelowa - numer kolumny
						)
 {
	 if(endw>Zrodlo.IleWierszy())
		 endw=Zrodlo.IleWierszy()-1;
	 if(endk>Zrodlo.IleKolumn())
		 endk=Zrodlo.IleKolumn()-1;

	 if(IleKolumn()==0 || IleWierszy()==0) //Jak jeszcze nie ustalony rozmiar tablicy docelowej
		this->UstalRozmiar(endw-startw+1,endk-startk+1);//To alokacja tablicy odpowiedniej wielkoœci

	 for(unsigned i=startw;i<=endw;i++)
		 for(unsigned j=startk;j<=endk;j++)
			{
			 if(Opisowo>3)
				 cerr<<NazwaPliku<<"("<<celw+(i-startw)<<","<<celk+(j-startk)<<")="<<Zrodlo(i,j)<<endl;
			 unsigned a=celw+(i-startw);
			 unsigned b=celk+(j-startk);
			 (*this)( a , b )=Zrodlo(i,j);
		    }
 }

//Funkcja wy³apywania b³êdów. Jak zwróci 0 to kontynuujemy!
int TabelaTabDelimited::PodstawowaFunkcjaBledu(int kod,const TabelaTabDelimited& self,std::ios& s)
//Wersja domyslna funkcji reakcji na b³¹d
{
	cerr<<"Kod: "<<kod<<" ";
	if(errno!=0)    //extern int errno; OLD C!
		perror(self.JakaNazwa());
	else
		cerr<<self.JakaNazwa();
	return kod; //Przerywamy niezw³ocznie
}

TabelaTabDelimited::funkcja_bledu* TabelaTabDelimited::AktualnaFunkcjaBledu=TabelaTabDelimited::PodstawowaFunkcjaBledu; //I wskaznik do aktualnie stosowanej funkcji

bool TabelaTabDelimited::ZapiszDoPliku(const char* _Nazwa,char Delimiter)
//Zapisuje do pliku. Bierze domyslny delimiter, chyba ¿e podano
{
	char old_delim=this->JakiDelimiter();
	if(Delimiter!=-1)
		this->ZmienDelimiter(Delimiter);
	if(*_Nazwa!='\0') //Podano jak¹œ
		this->ZmienNazwe(_Nazwa);

	//Wlasciwe zapisywanie
	ofstream oplik(this->JakaNazwa());
	if(!oplik.is_open() && AktualnaFunkcjaBledu(-1,*this,oplik))
			return false;
	/////////////////////
	oplik<<*this;
	/////////////////////
	oplik.close();

	if(Delimiter!=-1)//Przywraca stary delimiter
		this->ZmienDelimiter(old_delim);

	//Sprawdza ewentualnie b³êdny stan strumienia
	if(oplik.bad() && AktualnaFunkcjaBledu(-1,*this,oplik))
		return false;
	else
		return true;
}

void TabelaTabDelimited::pomin_puste(istream& str)
//Definicja funkcji, która usuwa "bia³e" znaki
{   //Sprawdza czy nie koniec i czy nastêpny bia³y
    while(!str.eof() && isspace(str.peek()) )
                     str.get(); //Wczytuje czyli usuwa kolejny znak
}

void TabelaTabDelimited::wczytaj_do_delim(std::istream& str,std::string& buf)
//I pobierania znaków do delimitera
{	//Sprawdza czy nie koniec i czy nastêpny jest Delimiter'em
	buf="";//Usuwa star¹ zawartoœæ
	while(!str.eof() && str.peek()!='\n' && str.peek()!=Delimiter )
				buf+=str.get();//Wczytuje czyli te¿ usuwa ze strumienia kolejny znak
}

bool TabelaTabDelimited::parse_string(const string& bufek,unsigned w,unsigned& k)
//Tnie buf na delimiterach i fragmenty umieszcza w komórkach
//Trzeba uwa¿aæ bo string mo¿e byæ w/tstringiem i mieæ niewidoczne znaki!!!
//wrrrr....
{
	unsigned len=bufek.length();
	if(len==0)
		return false;

	char* tmpbufor=new char[len+1];
#ifdef _MSC_VER
	strncpy_s(tmpbufor,len+1,bufek.c_str(),len+1);
#else
	strncpy(tmpbufor,bufek.c_str(),len+1);
#endif

	const char* start=tmpbufor;
	for(char* curr=tmpbufor;*curr!='\0';curr++)
	{
		if(*curr==Delimiter)
		{
			*curr='\0';
			(*this)(w,k)=start;
			if(Opisowo>2)
				cerr<<(*this)(w,k)<<Delimiter;
			k++;
			start=curr;
			start++;
		}
	}

	if(*start!='\0')//Jesli ostatni nie by³ pusty!!!
	{
		(*this)(w,k)=start;
		if(Opisowo>2)
			cerr<<(*this)(w,k)<<Delimiter<<endl;
		k++;
	}

	delete []tmpbufor;
	return true;
}

static //Sprawdza ile znaków danego typu jest w stringu. Np. delimiterów
unsigned ZliczZnaki(char Znak,const string& Tekst)
{
	unsigned licznik=0;
	for(unsigned i=0;i<Tekst.length();i++)
		if(Tekst[i]==Znak)
			licznik++;
	return licznik;
}

bool TabelaTabDelimited::WczytajZPliku(const char* _Nazwa,char _Delimiter,unsigned spW/*=-1*/,unsigned spK/*=-1*/)
//Wczytuje z pliku. Dotychczasowa zawartosc weg
//Mo¿na podaæ rozmiar, wtedy nie sprawdza.
{
   NazwaPliku=_Nazwa;
   Delimiter=_Delimiter;
   ifstream mojplik(_Nazwa); //Utworzenie obiektu skojarzonego z plikiem na dysku i od razu otwarcie
   if(!mojplik.is_open() && AktualnaFunkcjaBledu(-1,*this,mojplik)) //Sprawdzenie czy otwarcie dostêpu siê uda³o
	   {if(Opisowo>0) cerr<<"Nieudane otwarcie pliku.";return false;} //Jak siê nie uda otworzyæ to konczymy z b³êdem
	 else
		if(Opisowo>0) cout<<"/* Czytam z pliku "<<_Nazwa<<" ."<<endl;

   string pom;//Na wczytanie zawartoœci komorki
   unsigned ilek=0;
   unsigned ilew=0;
   char cc;

  //Zliczanie wierszy czyli linii pliku
  if(spW==-1 || spK==-1)//Nie wiadomo ile wierszy lub kolumn
  {
	if(Opisowo>1) cerr<<endl<<"/*\tUstalanie liczby kolumn i/lub wierszy tabeli"<<endl<<endl;
	while(!mojplik.eof())
	{
	  pom="";//Pusty
	  getline(mojplik,pom,'\n');//Mo¿e sobie "zawieœæ", jak koniec pliku, a nie linii, ale nie mo¿e byæ "zly"
	  if(mojplik.bad()&& AktualnaFunkcjaBledu(-2,*this,mojplik))
			{
				if(Opisowo>0)cerr<<endl<<"Wiersz:"<<ilew+1<<' '<<endl;return false;
			}

	  if(pom[0]=='\0')//Koncowka pliku ...
	  {
		//cerr<<"Pusty wiersz "<<ilew<<endl;
		continue;
	  }

	  ilew++;
	  if(Opisowo>3) cerr<<pom<<"|"<<endl;

	  unsigned Delimiterow=ZliczZnaki(Delimiter,pom)+1;//Na koncu linii powinien byæ \n. A nawet jak jest \t\n to jedna kolumna wiêcej bardzo nie zaszkodzi
	  unsigned pomlen=pom.length();
	  if(pom[pomlen-1]=='\t' ) //&& pom[pomlen-1]=='\n')
	  		Delimiterow--;
	  if(Delimiterow>ilek)
	  {
		ilek=Delimiterow;
		if(Opisowo>0 && ilew>1)
			cerr<<"!!! W wierszu "<<ilew<<" wykryto jeszcze wiecej kolumn: "<<Delimiterow<<endl;
	  }
	}
	//if(pom.length()==0)ilew--;// Jeœli ostatnia linia pusta //To nie bêdzie potrzebna - zrobione wczeœniej
  }

  ilew=max(ilew, (spW!=-1?spW:0) );
  ilek=max(ilek, (spK!=-1?spK:0) );

  if(Opisowo>0)
  {
	cout<<endl;
	cout<<"/*\tKolumn jest "<<ilek<<" w tej tabeli"<<" ."<<endl;
	cout<<"/*\toraz linii "<<ilew<<" w tej tabeli"<<" ."<<endl;
  }
					assert(ilek>0);
  if(ilew==0)	//To siê mo¿e zdarzyæ
	  return false; //Ale wtedy nie ma czego czytaæ!

  UstalRozmiar(ilew,ilek); //Alokacja potrzebnej pamiêci

  //Z powrotem na pocz¹tek
  mojplik.clear();//Bo jesli by³ ju¿ na koncu pliku to mia³ pewnie fail()
  mojplik.seekg( 0, ios_base::beg );
  if((mojplik.fail() || mojplik.bad())&& AktualnaFunkcjaBledu(-3,*this,mojplik))
		{if(Opisowo>1) cerr<<endl<<"Resetowanie pozycji. "<<endl;return false;}

  //Wlaœciwe czytanie
  unsigned ik=0,jw=0;//Indeksy komorek //split? parse?
  do{
	   if(mojplik.eof())
	   {
		   if(Opisowo<0) cerr<<endl<<"Niespodziewany koniec pliku w wierszu "<<jw;
			errno=ERANGE;
		   if(AktualnaFunkcjaBledu(-2,*this,mojplik))return false;
	   }

	   pom="";//Pusty
	   getline(mojplik,pom,'\n');//Mo¿e sobie "zawieœæ", jak koniec pliku a nie linii, ale nie mo¿e byæ "zly"

	   if(mojplik.bad())
	   {
			if(Opisowo>0)cerr<<endl<<"Wiersz:"<<jw<<' '<<endl;
			if(AktualnaFunkcjaBledu(-2,*this,mojplik))return false;
	   }//Mo¿e pójœc dalej choc to raczej bez sensu

	   if(!parse_string(pom,jw,ik) && pom.length()!=0)//Mo¿e byæ pusta linia, ale inne b³êdy nie wchodz¹
	   {
		   if(Opisowo>0) cerr<<endl<<"Nie udany parsing wiersza "<<jw<<endl;
		   if(AktualnaFunkcjaBledu(-3,*this,mojplik))return false;
	   }//Mo¿e pójœc dalej choc to raczej bez sensu

   jw++;//Skonczylismy t¹ linie
   ik=0;//Zaczynamy now¹
   if(Opisowo>1)
		cerr<<endl;

  }while(jw<ilew);

  return true; //Jesli dotarl tutaj do czytanie sie uda³o
}

bool TabelaTabDelimited::Znajdz(const char* Czego,unsigned& pozw,unsigned pozk) const
{
  while(strcmp((*this)(pozw,pozk),Czego)!=0) //Powtarzaj dopuki nie znajdziesz lub nie dojdziesz do koñca
  {
	 if(this->IleKolumn() > pozk+1) pozk++;
	 else
	 {
		 if(this->IleWierszy() > pozw+1) pozw++;
		 else
		 return false; //Koniec pliku, i nadal nic
		 pozk=0;//Pocz¹tek wiersza
	 }
	// clog<<pozw<<' '<<pozk<<';';
  }
  return true;
}

//friend
/// \details Operacja wyrzucania na dowolny strumien. Uzywa ustalonego Delimitera
std::ostream& operator << (std::ostream &o, const TabelaTabDelimited &self)
{
    unsigned ilew=self.IleWierszy();
    unsigned ilek=self.IleKolumn();

    for(unsigned w=0;w<ilew;w++)
    {
        for(unsigned k=0;;k++)
        {
            o<<self(w,k);
            if(o.bad() && self.AktualnaFunkcjaBledu(-1,self,o)) goto NIE_DA_SIE_DALEJ;//Powinien skonczyæ, ale mo¿e siê uda?
            if(k<ilek-1)
                o<<self.JakiDelimiter();
            else break; //Przerywa pêtle wypisywania tego wiersza

        }
        //o<<'|';//DEBUG
        o<<endl; //Koniec tego wiersza
        if(o.bad() && self.AktualnaFunkcjaBledu(-1,self,o)) goto NIE_DA_SIE_DALEJ;//Powinien skonczyæ, ale mo¿e siê uda?
    }

    NIE_DA_SIE_DALEJ:
    return o;
}

// ///////////////////////////////////////////////////////////////
//  Wojciech Borkowski
//  Instytut Studiów Spo³ecznych Uniwersytet Warszawski
//  http://borkowski.iss.uw.edu.pl
// ///////////////////////////////////////////////////////////////

	//Wstêpne zliczanie kolumn
/*   if(spK==-1)//Czyli nie wiadomo ile kolumn jest naprawdê
   {
   do{
	   cc=mojplik.get();
	   if((mojplik.fail() || mojplik.bad())&& AktualnaFunkcjaBledu(-2,*this,mojplik))
				{if(Opisowo>0)cerr<<endl<<pom<<" '"<<cc<<"' "<<endl;return false;}

	   if(cc==Delimiter)//Nie by³o pola
	   {
		  if(Opisowo>2) cout<<"./";
		  ilek++;
	   }
	   else
	   {
		  mojplik.unget();//Poczatek pola albo konca
		  cc=mojplik.peek();
		  if(cc=='\n'||cc=='\r')
			 {
				if(Opisowo>0)cerr<<"??? wiersz 1 kolumna:"<<ilek+1<<" PUSTA/EMPTY?";break;
			 }//To siê nie powinno zdarzac, chyba ¿e pusta linia

		 // mojplik>>pom;//Uwaga - to dzia³a dobrze jak delimiter jest "bia³y"!!!
		  wczytaj_do_delim(mojplik,pom);

		  if((mojplik.fail() || mojplik.bad())&& AktualnaFunkcjaBledu(-2,*this,mojplik))
				{if(Opisowo>0)cerr<<"'"<<pom<<"'"<<endl; return false;}

		  ilek++;
		  if(Opisowo>2)	cerr<<pom.c_str()<<"|";

		  cc=mojplik.get();//Po stringu coœ jest. Albo tabulacja i dalej, albo koniec linii

		  if((mojplik.fail() || mojplik.bad())&& AktualnaFunkcjaBledu(-2,*this,mojplik))
			{if(Opisowo>0)cerr<<endl<<pom<<" '"<<cc<<"' "<<endl;return false;}

		  if(cc=='\n')//||cc=='\r'
		  {
			  cc=mojplik.peek();
			  break;
		  }
	   }
	}while(1);

   ilew=1;//Jeden wiersz juz jest
   if(Opisowo>0)cerr<<endl;
   }
   else ilek=spK;  */

/*
cc=mojplik.get();
	   if((mojplik.fail() || mojplik.bad()) && AktualnaFunkcjaBledu(-3,*this,mojplik))
		{if(Opisowo>1) cerr<<endl<<"Wiersz: "<<jw<<"Item: "<<ik<<' '<<pom<<' ';return false;}

	   if(cc==Delimiter)//Nie by³o pola
	   {
		  if(Opisowo>1)
		    cerr<<"?/";
		  ik++;
	   }
	   else
	   {
		  mojplik.unget();//Poczatek pola albo konca
		  cc=mojplik.peek();
		  if(cc=='\n'||cc=='\r')
			 { if(Opisowo>1)cerr<<"???";break;}//To siê nie powinno zdarzac, chyba ¿e pusta linia

		  //mojplik>>pom;//Uwaga - to dzia³a dobrze jak delimiter jest "bia³y"!!!
		  wczytaj_do_delim(mojplik,pom);

		  if((mojplik.fail() || mojplik.bad()) && AktualnaFunkcjaBledu(-3,*this,mojplik))
			{if(Opisowo>1) cerr<<endl<<"Wiersz: "<<jw<<"Item: "<<ik<<' '<<pom<<' ';return false;}

		  (*this)(jw,ik)=pom;
		  ik++;

		  if(Opisowo>1)
			cerr<<pom.c_str()<<"|";

		  cc=mojplik.get();//Po stringu coœ jest. Albo tabulacja i dalej, albo koniec linii

		  if((mojplik.fail() || mojplik.bad())&& AktualnaFunkcjaBledu(-3,*this,mojplik))
			{if(Opisowo>1) cerr<<endl<<"Wiersz: "<<jw<<"Item: "<<ik<<' '<<pom<<' ';return false;}

		  if(cc=='\n')//||cc=='\r'
		  {
			  cc=mojplik.peek();
			  break;
		  }


		  */
