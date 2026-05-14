/// @file
/// @brief DATA SERIES MANAGER CLASS/KLASA ZARZĄDCY SERI DANYCH.
/// @date 2026-05-14 (modified)
//*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#ifndef __SOURMNGR_HPP__
#define __SOURMNGR_HPP__

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nullptr"


#include "datasour.hpp"
#include "graphs.hpp"

namespace sym2 { namespace data {

//+using namespace symshell2;
//using ssh2 = symshell2; //C++17

/// Interface zarządcy danych.
class sources_manager_base
//------------------------
{
    //using symshell2::config_point;
public:
    virtual ~sources_manager_base() = default;         ///< Destruktor wirtualny z uwagi na klasy potomne

    /// @name AKCESORY.
    /// @{
    virtual size_t get_size() = 0;        ///< Podaje po prostu rozmiar.

    /// Dodaje serie do listy. @return pozycja albo -1 (gdy error).
    virtual int insert(data_source_base *ser, int not_men = 0) = 0;

    /// Wymienia serie na pozycji `pos`, ale jak indeks niepoprawny to zwraca -1.
    virtual int replace(size_t pos, data_source_base *ser = NULL, int not_men = 0) = 0;

    /// Wymienia serie wg. nazwy. Jak nie znajdzie, to zwraca -1.
    virtual int replace(const char *nam, data_source_base *ser = NULL, int not_men = 0) = 0;

    /// Odnajduje na liście. @return pozycja albo -1 (gdy error).
    virtual int search(const char *nam) = 0;

    /// Zapamiętuje informacje o wizualizacji serii.
    virtual int set_info(size_t index, //!< Pozycja na liście.
                         wb_color ico = default_color, //!< Kolor ustalony lub domyślny.
                         symshell2::config_point *fig = NULL //!< Obiekt rysujący punkty serii.
    ) = 0;

    /// Udostępnianie samych seri. Bez możliwości modyfikacji i zwolnienia.
    virtual data_source_base /*const*/*get(size_t index) = 0;
    /// @}

    /// @name Metody, które tworzą dynamicznie tablice serii — potrzebne dla złożonych wykresów.
    /// @{
    virtual wb_dynarray<symshell2::graph::series_info> make_series_info(int start, .../* ostatnia -1*/) = 0; //Z listy parametrów
    virtual wb_dynarray<symshell2::graph::series_info>
    make_series_info(wb_dynarray<int>) = 0;                //Z tablicy dynamicznej
    //wb_dynarray<graph::series_info> make_series_info(const int* first/*ostatnia -1*/); //Z tablicy typu C zakonczonej -1
    /// @}

    //reserved:
    //virtual
    //int	eval(istream& script); //Wykonuje skrypt łączenia danych.
    //int	save(ostream& script); //Zapisuje się w formacie skryptu łączenia danych.
};

/// @brief NAJPROSTSZY ZARZĄDCA DANYCH.
/// @details
///     Zakłada pełne panowanie nad "włożonymi" do niego seriami, które
///     muszą być utworzone w pamięci dynamicznej, chyba że w metodach
///     `insert()` lub `replace()` podano inaczej, tzn. `not_menage==1` (czyli że obiekt statyczny).
///     Normalnie zwalnianie wykonuje zarządca w swoim destruktorze.
///     Ręcznie można użyć do tego metody `replace()` z parametrem NULL.
class sources_manager : public sources_manager_base
//-----------------------------------------------
{
private:
    /// Wewnętrzna klasa reprezentująca źródło danych.
    class internal
        //------------
    {
    private:
        data_source_base *dat;   //!< Wskaźnik na serie.
        unsigned not_menage: 1;   //!< Określa, czyzwalniać.
    public:
        wb_color col;   //!< Ewentualny kolor ustalony.
        wb_ptr<symshell2::config_point> fig;   //!< Obiekt wizualizujący punkty danych.

        /// @name MANIPULATORY
        /// @{
        void set(data_source_base *d, int not_men)
        {
            clear();
            dat = d;
            not_menage = not_men;
        } ///< setter.
        data_source_base *get()
        { return dat; } ///< a read-only reader.
        void clear()
        {
            if(not_menage == 0 && dat != NULL)
            {
#ifndef NDEBUG
                fprintf(stderr, "%s\n", dat->name());
#endif
                delete dat;
            }

            dat = NULL;
            not_menage = 0;
        }
        /// @}

        /// @name KONSTRUKTÓRY i DESTRUKTÓRY
        /// @{
        internal() : not_menage(1u), col(default_color), dat(NULL), fig(NULL)
        {}

        //internal(const internal& ini):col(ini.col),dat(ini.dat),fig(ini.fig),not_menage(ini.not_menage){}
        ~internal()
        { clear(); }
        /// @}
    };

    wb_dynarray<internal> tab; ///< Tablica z danymi o seriach.

public:
    /// Konstruktor tylko na rozmiar.
    explicit sources_manager(size_t N);

    /// Konstruktor z inicjująca lista zakończoną przez NULL.
    sources_manager(size_t N, data_source_base *.../*NULL*/);

    /// Destruktor wirtualny z zasady, że mamy funkcje wirtualne.
    ~sources_manager() override;

    /// @name AKCESORY
    /// @{

    /// Podaje po prostu rozmiar, łącznie z pozycjami pustymi.
    size_t get_size() override;

    /// Dodaje serie do listy. Zwraca pozycje albo -1(błąd).
    int insert(data_source_base *ser, int not_men = 0) override;

    int replace(size_t pos, data_source_base *ser = NULL, int not_men = 0) override;

    /// Wymienia na liście. Jak nie znajdzie, to zwraca -1.
    int replace(const char *nam, data_source_base *ser = NULL, int not_men = 0) override;

    /// Odnajduje na liście. Zwraca pozycje albo -1(błąd).
    int search(const char *s_name) override;

    /// Zapamiętuje informacje o wizualizacji serii. @return -1, gdy jakiś błąd.
    int set_info(size_t index,
                 wb_color ico = default_color, //!< Czy kolor ustalony.
                 symshell2::config_point *fig = NULL      //!< Obiekt rysujący punkty.
    ) override;

    /// Udostępnianie samych seri. Nie wolno zwalniać !!!
    data_source_base/*const*/*get(size_t index) override;
    /// @}

    /// @name Funkcje tworzące dynamicznie tablice serii — potrzebne dla wykresów.
    /// @details Nie jest jasne, dlaczego są wirtualne, gdy pewnie mogłyby być `static`!
    /// @{
    wb_dynarray<symshell2::graph::series_info>
    make_series_info(int start, .../* ostatnia -1*/) override;    ///< Z listy parametrów.
    wb_dynarray<symshell2::graph::series_info>
    make_series_info(wb_dynarray<int>) override;                ///< Z tablicy dynamicznej.
    //wb_dynarray<graph::series_info> make_series_info(const int* first/*ostatnia -1*/); ///< Z tablicy typu C zakończonej -1.
    /// @}

    //reserved:
    //virtual
    //int	eval(istream& script); //Wykonuje skrypt łączenia danych.
    //int	save(ostream& script); //Zapisuje się w formacie skryptu łączenia danych.

    /// Ustalanie informacji o wersji obsługiwanych danych.
    /// Pierwsza serie na sile, potem w pętli najpierw sprawdza, czysamo się zmieni,
    /// , jeśli się nie zgadza, to wymusza.
    void new_data_version(int change = 1, unsigned increment = 1);

    /// Zeruje informacje o wersjach danych.
    void restart_data_version();
};

}} // end of namespaces sym2::data

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
#endif

