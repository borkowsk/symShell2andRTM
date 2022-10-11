//KLASA DO OPEROWANIU NA TABLICACH TAB-DELIMITED
///////////////////////////////////////////////////////////////////////
#include <cassert>
#include <string>
#include <iostream>
#include <errno.h>

namespace wbrtm {


    class TabelaTabDelimited {
    private:
//Konstruktor kopiuj�cy - bardzo kosztowny albo niebezpieczny, wi�c dlatego prywatny.
        TabelaTabDelimited(const TabelaTabDelimited &);//Zabroniony tak�e dla kompilatora

        std::string *Tresc;    //W�a�ciwa tre�c w postaci wektora "zawijanego" dlugoscia wiersza
    protected:
        std::string NazwaPliku;//Nazwa pliku, jak trzeba zapisa� albo poda� na ekran
        char Delimiter; //Znak delimituj�cy - domy�lnie '\t' - i tylko to przetestowane
        unsigned Wiersz;  //D�ugosc wiersza
        unsigned Rozmiar; //Ile kom�rek

        void pomin_puste(std::istream &str); //Rodzinna funkcja do usuwania smieci - ci�g�w bia�ych znak�w
        void wczytaj_do_delim(std::istream &str, std::string &buf); //I pobierania znak�w do delimitera
        bool parse_string(const std::string &buf, unsigned w,
                          unsigned &k); //Tnie 'buf' na delimiterach i fragmenty umieszcza w kom�rkach
    public:
        virtual ~TabelaTabDelimited();//Destruktor musi by�, i to wirtualny bo s� wewn�trz obiekty dynamiczne
        TabelaTabDelimited() ://Konstruktor domyslny, bez rozmiar�w - czeka na pozniejsze decyzje
                NazwaPliku("<TabelaTabDelimited>"), Delimiter('\t'), Tresc(NULL), Wiersz(0), Rozmiar(0), Opisowo(1) {}

        TabelaTabDelimited(int w, int k) ://Tworzy od razu tabele o okreslonym rozmiarze
                NazwaPliku("<TabelaTabDelimited>"), Delimiter('\t'), Tresc(NULL), Wiersz(0), Rozmiar(0),
                Opisowo(1) { UstalRozmiar(w, k); }

        void UstalRozmiar(int w, int k);//Decyzja ustalaj�ca rozmiary tabeli. Nie zachowuje poprzedniej zawartosci
        bool WczytajZPliku(const char *_Nazwa, char Delimiter = '\t', unsigned spW = -1,
                           unsigned spK = -1);//Wczytuje z pliku. Mo�na poda� rozmiar, wtedy nie sprawdza.
        bool ZapiszDoPliku(const char *_Nazwa = "\0",
                           char Delimiter = -1);//Zapisuje do pliku. Bierze domyslny delimiter, chyba �e podano

        //Zrobienie kopi ca�o�ci lub wycinka. Kosztowne, wi�c tylko jawne
        void PrzekopiujZ(const TabelaTabDelimited &Zrodlo,
                         unsigned startw = 0, //Pocz�tkowy wiersz
                         unsigned startk = 0,  //Poczatkowa kolumna
                         unsigned endw = -1, //Koncowy wiersz albo do konca
                         unsigned endk = -1,  //Koncowa kolumna albo do konca
                         unsigned celw = 0,  //Pierwsza komorka docelowa - numer wiersza
                         unsigned celk = 0      //Pierwsza komorka docelowa - numer kolumny
        );

        bool Znajdz(const char *Czego, unsigned &pozw,
                    unsigned pozk) const;//Szuka kom�rki r�wnej tekstowi Czego, true jak znajdzie

        //Dost�p do nie zmienialnej tresci kom�rki
        const char *operator()(int w, int k) const;

        //Pe�ny dost�p do zawartosci kom�rki
        std::string &operator()(int w, int k);

        //Akcesory - inlinowany (lub nie) dost�p do atrybut�w obiektu
        /////////////////////////////////////////////////////////////////////////
        char ZmienDelimiter(char nowy) {
            char stary = Delimiter;
            Delimiter = nowy;
            return stary;
        }

        char JakiDelimiter() const { return Delimiter; }

        unsigned IleWierszy() const { if (Wiersz != 0) return Rozmiar / Wiersz; else return 0; }

        unsigned IleKolumn() const { return Wiersz; }

        std::string ZmienNazwe(const char *nowa);

        const char *JakaNazwa() const;

        friend //Operacja wyrzucania na dowolny strumien. Uzywa ustalonego Delimitera
        std::ostream &operator << (std::ostream &o, const TabelaTabDelimited &self);

        //Obsluga b��d�w wej�cia-wyj�cia
    public://Domyslnie w przypadku b��du czytania czy pisania akcja jest przerywana, ale mo�na to zmienic np. rzucaj�c wyj�tkiem
        typedef int funkcja_bledu(int kod, const TabelaTabDelimited &self,
                                  std::ios &s); //Funkcja wy�apywania b��d�w. Jak zwr�ci 0 to kontynuujemy!
        unsigned Opisowo; //Jak >0 to na konsoli jest sledzenie wczytywania. Wi�cej niz 3 - pe�ne echo
        static int PodstawowaFunkcjaBledu(int kod, const TabelaTabDelimited &self,
                                          std::ios &s);//Wersja domyslna funkcji reakcji na b��d
        static funkcja_bledu *AktualnaFunkcjaBledu; //I wskaznik do aktualnie stosowanej funkcji
    };

} //namespace wbrtm

// ///////////////////////////////////////////////////////////////
//  Wojciech Borkowski
//  Instytut Studi�w Spo�ecznych Uniwersytet Warszawski
//  http://borkowski.iss.uw.edu.pl
// ///////////////////////////////////////////////////////////////