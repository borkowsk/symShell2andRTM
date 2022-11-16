/**
* \file
* \brief   KLASA DO OPEROWANIU NA TABLICACH z plików TAB-DELIMITED
*        -------------------------------------------------------------
* \details
*          Contents:
*          - class wbrtm::TabelaTabDelimited;
*
* \date    2022-10-12 (last modification)
* \author  borkowsk
*/

#ifndef TAB_DELIMITED_HPP
#define TAB_DELIMITED_HPP

#include <cassert>
#include <string>
#include <iostream>
#include <errno.h>

namespace wbrtm {

    /// \brief KLASA DO OPEROWANIU NA TABLICACH z plików TAB-DELIMITED
    class TabelaTabDelimited
    {
    private:
        /// \brief Konstruktor kopiujący - bardzo kosztowny albo niebezpieczny, więc dlatego prywatny.
        TabelaTabDelimited(const TabelaTabDelimited &); ///< Zabroniony także dla kompilatora.

        std::string *Tresc;     ///< Właściwa treść w postaci wektora "zawijanego" długością wiersza

    protected:
        std::string NazwaPliku; ///< Nazwa pliku, jak trzeba zapisać albo podać na ekran
        char Delimiter;         ///< Znak delimitujący - domyślnie '\t' - i tylko to przetestowane
        unsigned Wiersz;        ///< Długość wiersza
        unsigned Rozmiar;       ///< Ile w ogóle komórek?

        void pomin_puste(std::istream &str); ///< Własna funkcja do usuwania śmieci - ciągów białych znaków.
        void wczytaj_do_delim(std::istream &str, std::string &buf); ///< Funkcja pobierania znaków aż do delimitera.

        ///\brief Metoda tnie \p 'buf' na delimiterach i fragmenty umieszcza w kolejnych komórkach
        bool parse_string(const std::string &buf, unsigned w, unsigned &k);

    public:
        /// \brief Destruktor musi być i to wirtualny bo są wewnątrz obiekty dynamiczne.
        virtual ~TabelaTabDelimited();

        /// \brief Konstruktor domyślny, bez rozmiarów. Czeka na późniejsze decyzje.
        TabelaTabDelimited() :
                NazwaPliku("<TabelaTabDelimited>"), Delimiter('\t'), Tresc(NULL), Wiersz(0), Rozmiar(0), Opisowo(1) {}

        /// \brief   Konstruktor z podaniem rozmiaru.
        /// \note   Tworzy od razu tabele o określonym rozmiarze i pustych komórkach.
        TabelaTabDelimited(int w, int k) :
                NazwaPliku("<TabelaTabDelimited>"), Delimiter('\t'), Tresc(NULL), Wiersz(0), Rozmiar(0),
                Opisowo(1) { UstalRozmiar(w, k); }

        /// \brief Decyzja ustalająca rozmiary tabeli. \note Nie zachowuje poprzedniej zawartości!
        void UstalRozmiar(int w, int k);

        /// \brief Wczytywanie z pliku. \note Można podać spodziewany rozmiar, wtedy nie musi dwa razy czytać.
        bool WczytajZPliku(const char *_Nazwa, char Delimiter = '\t', unsigned spW = -1,
                           unsigned spK = -1);

        /// \brief Zapisuje do pliku. Bierze domyślny delimiter, chyba że podano inny.
        bool ZapiszDoPliku(const char *_Nazwa = "\0",
                           char Delimiter = -1);

        /// \brief Zrobienie kopi całości lub wycinka. \note Bardzo kosztowne, więc tylko jawne.
        void PrzekopiujZ(const TabelaTabDelimited &Zrodlo, ///< tabela z której bierzemy zawartość
                         unsigned startw = 0,  ///< Początkowy wiersz
                         unsigned startk = 0,  ///< Poczatkowa kolumna
                         unsigned endw = -1,   ///< Koncowy wiersz albo do konca
                         unsigned endk = -1,   ///< Koncowa kolumna albo do konca
                         unsigned celw = 0,    ///< Pierwsza komorka docelowa - numer wiersza
                         unsigned celk = 0     ///< Pierwsza komorka docelowa - numer kolumny
        );

        /// \brief Szuka komórki równej tekstowi "Czego". Zwraca 'true' jak znajdzie.
        bool Znajdz(const char *Czego, unsigned &pozw,
                    unsigned pozk) const;

        // Akcesory - inlinowany (lub nie) dostęp do atrybutów obiektu
        // ///////////////////////////////////////////////////////////////////////

        /// \brief Pełny dostęp do zawartości komórki
        std::string &operator()(int w, int k);

        /// \brief Dostęp do nie zmienialnej treści komórki
        const char *operator()(int w, int k) const;

        /// \brief Zmiana domyślnego delimiter-a
        char ZmienDelimiter(char nowy)
        {
            char stary = Delimiter;
            Delimiter = nowy;
            return stary;
        }

        /// \brief Aktualny delimiter
        char JakiDelimiter() const { return Delimiter; }

        /// \brief Ile wierszy
        unsigned IleWierszy() const { if (Wiersz != 0) return Rozmiar / Wiersz; else return 0; }

        /// \brief Ile kolumn.
        unsigned IleKolumn() const { return Wiersz; }

        /// \brief Zmiana nazwy tabeli (?)
        std::string ZmienNazwe(const char *nowa);

        /// \brief Jaka nazwa tabeli (?)
        const char *JakaNazwa() const;

        // Obsługa wejścia-wyjścia
        // //////////////////////////////

        /// \brief Operacja wyprowadzania na dowolny strumień. Używa ustalonego wczesniej Delimiter-a.
        friend
        std::ostream &operator << (std::ostream &o, const TabelaTabDelimited &self);

        /// \brief Typ funkcji obsługi błędu czytania czy pisania.
        /// \return Jak zwróci 0 to czytanie będzie kontynuowane, w przeciwnym wypadku jest przerywane.
        typedef int funkcja_bledu(int kod, const TabelaTabDelimited &self, std::ios &s);

        /// \brief Wersja domyślna funkcji reakcji na błąd.
        /// \note  Ta funkcja zawsze sygnalizuje kod błędu (nie 0), więc że akcja czytania jest przerywana,
        ///        ale można to zmienić podstawiając własną funkcję.
        static int PodstawowaFunkcjaBledu(int kod, const TabelaTabDelimited &self, std::ios &s);

        /// \brief Statyczny wskaźnik do aktualnie stosowanej funkcji błędu.
        static funkcja_bledu *AktualnaFunkcjaBledu;

        /// \brief Jak >0 to na konsoli jest śledzenie wczytywania. Gdy wartość większa niż 3 to pełne echo.
        unsigned Opisowo;
    };

} //namespace wbrtm

/* *******************************************************************/
/*               SYMSHELLLIGHT  version 2022                         */
/* *******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*             W O J C I E C H   B O R K O W S K I                   */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*     WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*     GITHUB: https://github.com/borkowsk                           */
/*                                                                   */
/*                                (Don't change or remove this note) */
/* *******************************************************************/
#endif //TAB_DELIMITED_HPP