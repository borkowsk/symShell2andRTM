/** \file TabDelimited.cpp
 *  \author borkowsk
 *  \brief Implementation of wbrtm::TabDelimited class
 *  \date 2022-10-11 (last modification)
 */
#include <ctype.h> //Typy znaków - http://www.cppreference.com/wiki/string/character_classes
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream> //Standardowe wejście/wyjście na konsole
#include <fstream>  //Używane wejście z pliku dyskowego

#include "TabDelimited.hpp"

using namespace std; // Łatwe użycie nazw z przestrzeni "std"

namespace wbrtm {

/// \details Wersja domyślna funkcji reakcji na błąd.
///          Jak funkcja wyłapywania błędów zwróci 0 to kontynuujemy, ale domyślna tego nie robi.
int TabelaTabDelimited::PodstawowaFunkcjaBledu(int kod,const TabelaTabDelimited& self,std::ios& s)
{
    cerr<<"Kod: "<<kod<<" ";
    if(errno!=0)    //extern int errno; OLD C!
        perror(self.JakaNazwa());
    else
        cerr<<self.JakaNazwa();
    return kod; //Przerywamy niezwłocznie raczej.
}

/// \detail To wskaźnik do aktualnie stosowanej funkcji błędu.
TabelaTabDelimited::funkcja_bledu* TabelaTabDelimited::AktualnaFunkcjaBledu=TabelaTabDelimited::PodstawowaFunkcjaBledu;

/// \details Dostęp do nie modyfikowalnej treści komórki.
const char* TabelaTabDelimited::operator ()(int w,int k) const
{  //...Jakieś sprawdzenia???
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

/// \details Pełny dostęp do zawartości komórki.
string& TabelaTabDelimited::operator () (int w,int k)
       {
		  static string dummy("???");

		  //...Jakieś sprawdzenia???
		  if(w<IleWierszy() && k<IleKolumn())
		  {
			unsigned index=(w*Wiersz)+k;
			return Tresc[index];
		  }
		  else
		  {
			if(Opisowo>0)
				cerr<<"Wadliwy indeks tabeli danych (od 0!) - wiersz: "<<w<<" kolumna: "<<k<<endl;
			if(errno=ERANGE,!AktualnaFunkcjaBledu(1,*this,cerr)) //Czy próba kontynuowania?
				return dummy;
			else
			exit(1);
		  }
	   }

/// \details Zmiana domyślnej nazwy pliku.
///          Nie wpływa na realne pliki zanim nazwa nie zostanie uzyta w jakiejś innej funkcji.
string   TabelaTabDelimited::ZmienNazwe(const char* nowa)
{
	string stara=NazwaPliku;
	NazwaPliku=nowa;
	return stara;
}

/// \details Po prostu zwraca nazwę pliku przypisaną do tej tabeli.
const char*  TabelaTabDelimited::JakaNazwa() const
{
	return NazwaPliku.c_str();
}

/// \details Decyzja ustalająca rozmiary tabeli.
/// \note    Jeśli tabela miała już jakąś 'treść' to zostanie ona usunięta!
void  TabelaTabDelimited::UstalRozmiar(int w,int k)
{
    if(Tresc!=nullptr)
		delete []Tresc; //Jeśli coś tam było

    Rozmiar=w*k;
    Wiersz=k; //Długość wiersza, czyli ile kolumn.

    Tresc=new string[Rozmiar]; //alokacja tablicy stringów
}

/// \details Destruktor musi być bo są wewnątrz obiekty.
///          Poza dealokacją zeruje Treść,Wiersz i Rozmiar, bo może być widoczne w debugger-ze, ale nie musi,
///          bo nowe debugger-y same wiedzą co jest już dealokowane.
TabelaTabDelimited::~TabelaTabDelimited()
{
 if(Tresc!=nullptr)
          delete [/*Rozmiar*/]Tresc;
#ifndef _NDEBUG
 if(this->Opisowo>0)
    cerr<<"Sukces dealokacji w destruktorze dla "<<NazwaPliku<<endl;
 //cin.get();
#endif
 Tresc=nullptr; //Dla pewności i debugingu.
 Wiersz=0;Rozmiar=0;
}

/// \details Implementacja kopiowania tabeli lub jej fragmentu.
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

	 if(IleKolumn()==0 || IleWierszy()==0) //Jak jeszcze nieustalony rozmiar tablicy docelowej
		this->UstalRozmiar(endw-startw+1,endk-startk+1);//To alokacja tablicy odpowiedniej wielkości.

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

/// \details Zapisuje do pliku. Bierze domyślny delimiter, chyba że podano inny.
bool TabelaTabDelimited::ZapiszDoPliku(const char* _Nazwa,char Delimiter)
{
	if(*_Nazwa!='\0') //Podano jakąś nazwę
		this->ZmienNazwe(_Nazwa);

	// Właściwe zapisywanie
	ofstream oplik(this->JakaNazwa()); // otwarcie strumienia
	if(!oplik.is_open() && AktualnaFunkcjaBledu(-1,*this,oplik)) // sprawdzenie strumienia
			return false;

    char old_delim=this->JakiDelimiter();
    if(Delimiter!=-1)
        this->ZmienDelimiter(Delimiter);

	// ///////////////////
	oplik<<*this;   // WŁAŚCIWY ZAPIS!
	// ///////////////////

	if(Delimiter!=-1) //Przywraca stary delimiter
		this->ZmienDelimiter(old_delim);

	//Sprawdza ewentualnie błędny stan strumienia
	if(oplik.bad() && AktualnaFunkcjaBledu(-1,*this,oplik))
		return false;
	else
		return true;

    //oplik.close(); //zamkniecie strumienia odbywa się w destruktorze
}

/// \details Własna funkcja, która usuwa "białe" znaki, bo różnie bywało z dostępnością bibliotecznej.
void TabelaTabDelimited::pomin_puste(istream& str)
{
    while(!str.eof() && isspace(str.peek()) ) //Sprawdza, czy nie koniec i czy następny jest "biały"
                     str.get(); // Wczytuje, czyli usuwa kolejny znak ze strumienia
}

/// \details Własna funkcja pobierania znaków do delimitera.
void TabelaTabDelimited::wczytaj_do_delim(std::istream& str,std::string& buf)
{
	buf="";//Usuwa star¹ zawartoœæ
	while(!str.eof() && str.peek()!='\n' && str.peek()!=Delimiter ) // Sprawdza, czy nie koniec i czy następny jest delimiter'em
				buf+=str.get() ;//Wczytuje dodając do bufora i usuwa ze strumienia kolejny znak.
}

/// \details Tnie buf na delimiterach i fragmenty umieszcza w komórkach.
///          Trzeba uważać bo string może być w/t-stringiem i mieć niewidoczne znaki!!!
bool TabelaTabDelimited::parse_string(const string& bufek,unsigned w,unsigned& k)
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

	if(*start!='\0') //Jeśli ostatni nie był pusty!!!
	{
		(*this)(w,k)=start;
		if(Opisowo>2)
			cerr<<(*this)(w,k)<<Delimiter<<endl;
		k++;
	}

	delete []tmpbufor;
	return true;
}

/// \brief Lokalna pomocnicza.
/// \details Sprawdza ile znaków danego typu jest w string-u. Np. delimiterów.
static
unsigned ZliczZnaki(char Znak,const string& Tekst)
{
	unsigned licznik=0;
	for(unsigned i=0;i<Tekst.length();i++)
		if(Tekst[i]==Znak)
			licznik++;
	return licznik;
}

/// \details Wczytuje dane z pliku. Dotychczasowa zawartość jest tracona.
///          Można podać rozmiar, wtedy nie musi sprawdzać.
bool TabelaTabDelimited::WczytajZPliku(const char* _Nazwa,char _Delimiter,unsigned spW/*=-1*/,unsigned spK/*=-1*/)
{
   NazwaPliku=_Nazwa;
   Delimiter=_Delimiter;

   ifstream mojplik(_Nazwa); //Utworzenie obiektu skojarzonego z plikiem na dysku i od razu otwarcie
   if(!mojplik.is_open() && AktualnaFunkcjaBledu(-1,*this,mojplik)) //Sprawdzenie, czy otwarcie dostępu się udało
   {
       if(Opisowo>0) cerr<<"Nieudane otwarcie pliku.";
       return false; // Jak się nie uda otworzyć, to kończymy z błędem.
   }
   else
		if(Opisowo>0)
            cout<<"/* Czytam z pliku "<<_Nazwa<<" ."<<endl;

   string pom; //Na wczytanie zawartości komórki
   unsigned ilek=0;
   unsigned ilew=0;
   char cc;

  // Zliczanie wierszy, czyli linii pliku
  if(spW==-1 || spK==-1) //Tylko gdy nie wiadomo ile wierszy lub kolumn.
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

	  if(pom[0]=='\0') //Koncowka pliku ...
	  {
		//cerr<<"Pusty wiersz "<<ilew<<endl;
		continue;
	  }

	  ilew++;
	  if(Opisowo>3) cerr<<pom<<"|"<<endl;

	  unsigned Delimiterow=ZliczZnaki(Delimiter,pom)+1; //Na końcu linii powinien być '\n'.
                                                                   // A nawet jak jest '\t\n' to jedna kolumna więcej
                                                                   // tak bardzo nie zaszkodzi.
	  unsigned pomlen=pom.length();
	  if(pom[pomlen-1]=='\t' ) //&& pom[pomlen-1]=='\n')
	  		Delimiterow--;

	  if(Delimiterow>ilek)
	  {
		ilek=Delimiterow;
		if(Opisowo>0 && ilew>1)
			cerr<<"!!! W wierszu "<<ilew<<" wykryto jeszcze więcej kolumn: "<<Delimiterow<<endl;
	  }
	}
	//if(pom.length()==0)ilew--;// Jeœli ostatnia linia pusta //To nie bêdzie potrzebna - zrobione wczeœniej
  }

  ilew=max(ilew, (spW!=-1?spW:0) ); //Ostatecznie ile jest tych wierszy
  ilek=max(ilek, (spK!=-1?spK:0) ); //... i kolumn.

  if(Opisowo>0)
  {
	cout<<endl;
	cout<<"/*\tKolumn jest "<<ilek<<" w tej tabeli"<<" ."<<endl;
	cout<<"/*\toraz linii "<<ilew<<" w tej tabeli"<<" ."<<endl;
  }

  if(ilew==0)	//To się może zdarzyć
	  return false; //Tylko że wtedy nie ma czego czytać!
                                                                                                         assert(ilek>0);
  UstalRozmiar(ilew,ilek); //Alokacja potrzebnej pamięci

  // Z powrotem na początek pliku!
  mojplik.clear(); //Bo jeśli był już na końcu pliku to miał pewnie fail()
  mojplik.seekg( 0, ios_base::beg );

  if((mojplik.fail() || mojplik.bad())&& AktualnaFunkcjaBledu(-3,*this,mojplik))
		{if(Opisowo>1) cerr<<endl<<"Resetowanie pozycji. "<<endl;return false;}

  // Właściwe czytanie
  unsigned ik=0,jw=0; //Indeksy komórek //split? parse?
  do{
	   if(mojplik.eof())
	   {
		   if(Opisowo<0) cerr<<endl<<"Niespodziewany koniec pliku w wierszu "<<jw;
           errno=ERANGE;
		   if(AktualnaFunkcjaBledu(-2,*this,mojplik)) return false;
	   }

	   pom=""; //Pusty na początek.
	   getline(mojplik,pom,'\n'); //Może sobie "zawieść", jak koniec pliku, a nie linii, ale nie może być "zły"

	   if(mojplik.bad())
	   {
			if(Opisowo>0)cerr<<endl<<"Wiersz:"<<jw<<' '<<endl;
			if(AktualnaFunkcjaBledu(-2,*this,mojplik))return false;
	   }//Mo¿e pójœc dalej choc to raczej bez sensu

	   if(!parse_string(pom,jw,ik) && pom.length()!=0)//Mo¿e byæ pusta linia, ale inne b³êdy nie wchodz¹
	   {
		   if(Opisowo>0) cerr<<endl<<"Nie udany parsing wiersza "<<jw<<endl;
		   if(AktualnaFunkcjaBledu(-3,*this,mojplik))return false;
	   } //Może pójść dalej, choć to raczej bez sensu

   jw++; //Skończyliśmy tą linią
   ik=0; //Zaczynamy nową
   if(Opisowo>1)
		cerr<<endl;

  }while(jw<ilew);

  return true; //Jeśli dotarł tutaj, to czytanie się udało
}

/// \details przeszukiwanie zawartości komórek
bool wbrtm::TabelaTabDelimited::Znajdz(const char* Czego,unsigned& pozw,unsigned pozk) const
{
  while(strcmp((*this)(pozw,pozk),Czego)!=0) //Powtarzaj, dopóki nie znajdziesz lub nie dojdziesz do końca
  {
	 if(this->IleKolumn() > pozk+1) pozk++;
	 else
	 {
		 if(this->IleWierszy() > pozw+1) pozw++;
		 else
		 return false; //Koniec tabeli, i nadal nic
		 pozk=0; //Początek wiersza
	 }
	// clog<<pozw<<' '<<pozk<<';';
  }
  return true;
}

/// \details Operacja wyrzucania na dowolny strumień. Używa ustalonego Delimiter-a.
///          'friend' klasy.
std::ostream& operator << (std::ostream &o, const TabelaTabDelimited &self)
{
    unsigned ilew=self.IleWierszy();
    unsigned ilek=self.IleKolumn();

    for(unsigned w=0;w<ilew;w++)
    {
        for(unsigned k=0;;k++)
        {
            o<<self(w,k);
            if(o.bad() && self.AktualnaFunkcjaBledu(-1,self,o))
                goto NIE_DA_SIE_DALEJ; //Powinien skończyć, ale może się uda?
            if(k<ilek-1)
                o<<self.JakiDelimiter();
            else break; //Przerywa pętle wypisywania tego wiersza

        }
        //o<<'|';//DEBUG
        o<<endl; //Koniec tego wiersza
        if(o.bad() && self.AktualnaFunkcjaBledu(-1,self,o))
            goto NIE_DA_SIE_DALEJ; //Powinien skończyć, ale może się uda?
    }

    NIE_DA_SIE_DALEJ:
    return o;
}

} //namespace wbrtm
/* *******************************************************************/
/*	       WBRTM  version 2006 - renovated in 2022                   */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  htt...                                               */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/