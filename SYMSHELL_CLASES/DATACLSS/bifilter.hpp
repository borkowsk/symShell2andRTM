/// @file
/// @brief Klasa bazowa filtra zależnego od dwu źródeł.
/// @date 2026-05-04 (modified)
// ********************************************************************************************************************
//
#ifndef __bifilter_hpp__
#define __bifilter_hpp__

#include "datasour.hpp"

//Źródło filtrujace dane z innego źródła
class bi_filter_source_base : public filter_source_base
//-----------------------------------------------------------------
{

protected:
    data_source_base *Source2;
    double source2_miss;

    virtual int check_version()
    //Sprawdza, czyi jak zmienily się dane w zrodlach
    //W filtrach cache'ujacych może powodowac oproznienie
    //lub ponowne napełnienie. Zwraca 1, jeśli zmienilo wersje.
    //0, jeśli wersje się zgadzaja.
    {
        int update2 = update_version_from(Source2); //Tu ewentualnie może zmienić wersje
        int update1 = update_version_from(Source); //Tylko żeby dac zrodlu szanse updatu
        assert(update1 == 0); //NIe powinno już zmienić wersji

        //Aktualizacja "missing values"
        source_miss = Source->get_missing();
        source2_miss = Source2->get_missing();
        return update2;
    }

//Wewnętrzna struktura dla indeksowania
    struct indexes
    {
        iteratorh first;
        iteratorh second;

        indexes(iteratorh ifirst, iteratorh isecond) :
                first(ifirst), second(isecond)
        {}
    };

public:
/// Constructor.
    bi_filter_source_base(data_source_base *ini1 = NULL,
                          data_source_base *ini2 = NULL,
                          const char *format = "F(%s,%s)") :
            filter_source_base(ini1, format)
    {
        set_second_source(ini2);
    }

    void set_second_source(data_source_base *ini)
    {
        Source2 = ini;
        source2_miss = Source2->get_missing();
    }

    void set_first_source(data_source_base *ini)
    {
        Source = ini;
        source_miss = Source->get_missing();
    }

    data_source_base *get_second_source()	//Zwraca wskaźnik do drugiej seri zrodlowej
    {
        source2_miss = Source2->get_missing();
        return Source2;
    }

    data_source_base *get_first_source()	//Zwraca wskaźnik do pierwszej seri zrodlowej
    {
        source_miss = Source->get_missing();
        return Source;
    }

//Sposob tworzenia nazwy musi być przedefiniowany
    virtual const char *name(); //Musi zwracać nazwe serii albo "" - NIE NULL!!!

    virtual geometry_base *getgeometry()
//Powinien zwracać wskaźnik do obowiazujacej geometrii danych
//domyślnie nie można ustalic geometrii bo są dwie alternatywne,
//ale klasy potomne mogą zadecydowac ktora wziac.
    {
        return NULL;
    }

//DOSTĘP DO DANYCH:
//UWAGA: Zlozona iteracja wykonuje się do konca krotszego ze źródeł.
//--------------------------------------------------------------------
    virtual void bounds(size_t &N, double &min, double &max)
    //Ile elementów,wartość minimalna i maksymalna
    //Być może wartości te trzeba przekonwertowac
    {
        check_version();

        size_t N1 = 0, N2 = 0;
        double min1 = 0, min2 = 0, max1 = 0, max2 = 0;
        Source->bounds(N1, min1, max1);
        Source2->bounds(N2, min2, max2);
        N = (N1 < N2?N1:N2);
        _bounds(min1, max1, min2, max2, min, max);
    }

    virtual iteratorh reset()
    //Umozliwia czytanie od poczatku - iteratorh jest uchwytem iteratora zlozonego
    {
        check_version();  //Żeby źródła mialo szanse na update

        //Tworzenie zlozonego indeksu:
        indexes *pom = new indexes(Source->reset(), Source2->reset());

        return pom;
    }


    virtual double get(iteratorh &I)
    //Daje następną z N liczb!!! Po N-tej obiekt zrodlowy zwalnia iterator!
    {
        indexes *pom = (indexes *) I;
        double val1 = Source->get(pom->first);
        double val2 = Source2->get(pom->second);
        if(pom->first == NULL || pom->second == NULL)	//Ktores źródło się wyczerpalo
        {
            Source->close(pom->first);
            Source2->close(pom->second);
            delete pom;
            I = NULL; //Informacja zwrotna
        }
        return _get(val1, val2);
    }


    virtual double get(size_t index_from_geometry)
    //Przetwarza index uzyskany z geometrii
    //na wartość z serii, o ile jest możliwe czytanie losowe
    //(domyślnie nie jest, bo nie wiadomo, czyta sama geometriia)
    //i uzywamy _get do interpretacji pary wartości
    {
        double val1 = Source->get(index_from_geometry);
        double val2 = Source2->get(index_from_geometry);
        return _get(val1, val2);
    }

    virtual void close(iteratorh &I)
    //Obiekt  zwalnia iteratory wewnetrzne i cały iterator, jeśli nie został
    //zwolniony przez get(N)
    {
        indexes *pom = (indexes *) I;
        if(pom == NULL) return;
        Source->close(pom->first);
        Source2->close(pom->second);
        delete pom;
        I = NULL; //Informacja zwrotna
    }

//Implementacja konkretnej operacji - musi sprawdzac, czy
//któraś z wartości lub obie nie są "missing"
protected:
    virtual double _get(double val1, double val2) = 0;

//implementacja decyzji o wartości minimalnej i maksymalnej
    virtual void _bounds(double &min1, double &max1,
                         double &min2, double &max2,
                         double &min, double &max) = 0;

//Uzupełnienie o sprawdzanie, czyktores get nie dalo missing
    virtual int FromFirstIsMissing(double val)
    {
        if(val == source_miss) return 1;
        else return 0;
    }

    virtual int FromSecondIsMissing(double val)
    {
        if(val == source2_miss) return 1;
        else return 0;
    }

};


inline
const char *bi_filter_source_base::name()
//Musi zwracać nazwe serii albo "" - NIE NULL!!!
{
    const char *pom1 = Source->name();
    const char *pom2 = Source2->name();
    if(
            !_name.IsOK() ||
            strstr(_name.get_ptr_val(), pom1) == NULL ||
            strstr(_name.get_ptr_val(), pom2) == NULL
            )    //jeśli jeszcze nie ma albo zmienilo się którymś obiekcie źródła
    {
        _name.alloc(strlen(title_util::name()) + strlen(pom1) + strlen(pom2) + 1);
        sprintf(_name.get_ptr_val(), title_util::name(), pom1, pom2);
    }
    return _name.get_ptr_val();
}

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




