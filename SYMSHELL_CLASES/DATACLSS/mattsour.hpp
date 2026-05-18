/// @file
/// @brief __Specific source types - access to data in two-dimensional arrays.__ /<br>
///         _Konkretne klasy źródeł — dostęp do danych w tablicach dwuwymiarowych._
/// @date 2026-05-18 (modified)
// ********************************************************************************************************************
//
#ifndef SYMSHELL2_MATRIX_SOUR_HPP_INCLUDED_
#define SYMSHELL2_MATRIX_SOUR_HPP_INCLUDED_

#include "datasour.hpp"
#include "geombase.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace data {
    using namespace sym2::shell;

/// @brief @EN{ . }
///        @PL{ Klasa udostępniająca dowolną tablicę prostokątną oraz jej wycinki (albo i nie?). }
/// @details ...
///     Jeśli zdefiniowany wycinek wykracza poza tablicę źródłową, to funkcja `get` zwraca wartość "miss" podawana w konstruktorze.
///     Alternatywnie wycinek może realizować geometrie torusa i wtedy `miss` nie jest potrzebne.
///     ...
///     ...
template<class T>
class matrix_source : public rectangle_source_base
{
    T *arra;        ///< Wskaźnik do tablicy.

public:
    /// @name Constructors and destructors.
    /// @{

    /// 1.
    /// \param i_tit to tytuł, czyli nazwa serii danych.
    /// \param iA,iB to wymiary macierzy.
    /// \param i_array wskaźnik do tablicy, która jest tylko czytana przez ten obiekt.
    /// \param i_torus określa, czy włączyć geometrie torusa. Default, bo wtedy nie trzeba "miss".
    /// \param i_section ustala wycinek tablicy: `{start_X, len_X, start_Y, len_Y}`.
    /// \param i_miss to wartość podawana przy braku danych, czyli skanowaniu wycinka wychodzącego poza macierz.
    /// @note Możliwość ograniczenia tablicy do sekcji chyba nie jest zaimplementowana!
    matrix_source(const char     *i_tit,
                  size_t iA, size_t  iB,
                  void         *i_array,
                  int           i_torus = 1,
                  const int    *i_section = NULL,
                  double         i_miss = default_missing<double>()
                  )
    : rectangle_source_base(i_tit, iA, iB, i_torus, i_section, i_miss), arra((T *) i_array)
    {}

    /// 2.
    /// \param iA,iB to wymiary macierzy.
    /// \param i_array wskaźnik do tablicy, która jest tylko czytana przez ten obiekt.
    /// \param i_torus określa, czy włączyć geometrie torusa. Default, bo wtedy nie trzeba "miss".
    /// \param i_tit to tytuł, czyli nazwa serii danych.
    matrix_source(size_t iA, size_t iB,
                  void *i_array,
                  const char *i_tit,
                  int i_torus
                 )
    : rectangle_source_base(i_tit, iA, iB, i_torus, NULL, default_missing<double>()), arra((T *) i_array)
    {}

    /// 3. OBSOLETE
    /// \param i_tit to tytuł, czyli nazwa serii danych.
    /// \param geom to geometria danych.
    /// \param i_array wskaźnik do tablicy, która jest tylko czytana przez ten obiekt.
    /// \param i_section ustala wycinek tablicy: `{start_X, len_X, start_Y, len_Y}`.
    /// \param i_miss to wartość podawana przy braku danych, czyli skanowaniu wycinka wychodzącego poza macierz.
    matrix_source(const char *i_tit,
                  rect_geometry &geom,
                  void *i_array,
                  const int *i_section = NULL,
                  double i_miss = default_missing<double>()
                 )
    : rectangle_source_base(i_tit, geom, i_miss,i_section),//rectangle_source_base(i_tit, geom, i_section, i_miss),
      arra(i_array)
    {}

    /// Destruktor nic nie robi jawnie.
    ~matrix_source() override
    {
#ifndef NDEBUG
        cerr << "~matrix_source():" << name() << '\n'; //                ???
#endif
    }
    /// @}

// REIMPLEMENTACJE:
//=================

    /// Na podstawie wewnętrznej geometrii wylicza liczbę wartości, a potem określa minimum i maksimum.
    /// Żeby uniknąć obliczania można wymusić minimum i maksimum metodą `set_min_max`.
    void bounds(size_t &num, double &min, double &max) override;

    /// Daje następną z la*lb liczb, odczytaną z tablicy.
    double get(iterator_h &p) override
    {
        //assert(p!=NULL);
        if(p == NULL) return miss;
        size_t pom = _next(p);
        if(pom != ULONG_MAX)
            return arra[pom];
        else
            return miss;
    }

    /// Przetwarza index uzyskany z geometrii dwuwymiarowej.
    /// Podaje wartość serii z tablicy `arra`.
    double get(size_t index) override
    {
        assert(index < get_rect_geometry()->get_size());
        return arra[index];
    }

};

/// @brief @EN{ . }
///        @PL{ Klasa czytająca z dowolnej prostokątnej tablicy struktur za pomocą wskaźników do składowych. }
/// @details ...
///          ...
template<class STRUCT_T, class FIELD_T>
class struct_matrix_source : public rectangle_source_base
//------------------------------------------------
{
    typedef FIELD_T STRUCT_T::* TYP_POLA;
    STRUCT_T     *arra; ///< Tablica struktur
    TYP_POLA component; ///< Wskaźnik do składowej indywiduum

public:
    /// @name Constructors and destructor
    /// @{

    /// 1.
    /// \param i_tit to tytuł, czyli nazwa serii danych.
    /// \param iA,iB to wymiary macierzy.
    /// \param i_array wskaźnik do tablicy, która jest tylko czytana przez ten obiekt.
    /// \param i_component wskaźnik do pola struktury.
    /// \param i_torus określa, czy włączyć geometrie torusa. Default, bo wtedy nie trzeba "miss".
    /// \param i_section ustala wycinek tablicy: `{start_X, len_X, start_Y, len_Y}`.
    /// \param i_miss to wartość podawana przy braku danych, czyli skanowaniu wycinka wychodzącego poza macierz.
    struct_matrix_source(const char     *i_tit,
                         size_t iA,  size_t iB,
                         STRUCT_T     *i_array,
                         TYP_POLA  i_component,
                         int          i_torus = 1,
                         const int *i_section = NULL,
                         double        i_miss = default_missing<double>()
    )
    : rectangle_source_base(i_tit, iA, iB, i_torus, i_section, i_miss),//rectangle_source_base(iA,iB,i_tit,i_torus,i_section,i_miss),
      arra(i_array), component(i_component)
    {
        assert(component != NULL);
    }

    /// 2.
    /// \param i_tit to tytuł, czyli nazwa serii danych.
    /// \param geom to geometria danych.
    /// \param i_array wskaźnik do tablicy, która jest tylko czytana przez ten obiekt.
    /// \param i_component wskaźnik do pola struktury.
    /// \param i_section ustala wycinek tablicy: `{start_X, len_X, start_Y, len_Y}`.
    /// \param i_miss to wartość podawana przy braku danych, czyli skanowaniu wycinka wychodzącego poza macierz.
    struct_matrix_source(const char     *i_tit,
                         rect_geometry   &geom,
                         STRUCT_T     *i_array,
                         TYP_POLA  i_component,
                         const int *i_section = NULL,
                         double        i_miss = default_missing<double>()
                        )
    : rectangle_source_base(i_tit, geom, i_miss,i_section),
      arra(i_array), component(i_component)
    { assert(arra!=NULL); }

    /// Destructor.
    ~struct_matrix_source() override // NOLINT(modernize-use-equals-default)
    {
        //cerr<<"~struct_matrix_source(): "<<name()<<'\n';
    }
    /// @}

// METHODS:
//=======

    /// Daje następną z `la*lb` liczb w iteracji.
    double get(iterator_h &p) override
    {
        assert(p != NULL);
        size_t pom = _next(p);
        if(pom < this->get_rect_geometry()->get_size())
        {
            double val = arra[pom].*component;
            return val;
        }
        else
        {
            return miss;
        }
    }

    /// Przetwarza index uzyskany z geometrii na wartość komponentu struktury z tablicy.
    double get(size_t index) override
    {
        assert(component != NULL);
        assert(arra!=NULL);
        if(index < get_rect_geometry()->get_size())
        {
            double val = arra[index].*component;
            return val;
        }
        else
        {
            return miss;
        }
    }

    /// Na podstawie wewnętrznej geometrii wylicza liczbę wartości, a potem określa minimum i maksimum.
    /// Żeby uniknąć obliczania można wymusić minimum i maksimum metodą `set_min_max`.
    void bounds(size_t &num, double &min, double &max) override;

};

/// @brief @EN{ . }
///        @PL{ Klasa czytająca z dowolnej prostokątnej tablicy wskaźników do struktur za pomocą wskaźników do składowych. }
/// @details ...
/// Czyli dostęp `wskaźnik tablicy->wskaźnik struktury->wskaźnik` komponentu.
/// ...
template<class STRUCT_T, class FIELD_T>
class ptr_to_struct_matrix_source : public rectangle_source_base
//------------------------------------------------
{
    typedef FIELD_T STRUCT_T::* TYP_POLA;
    STRUCT_T    **arra; ///< Tablica wskaźników do struktur.
    TYP_POLA component; ///< Wskaźnik do składowej struktury.

public:
    /// @name Constructors and destructor.
    /// \param i_component wskaźnik do pola struktury.
    /// @{

    /// 1.
    ptr_to_struct_matrix_source(const char     *i_tit,
                                size_t iA, size_t  iB,
                                STRUCT_T    **i_array,
                                TYP_POLA  i_component,
                                int          i_torus = 1,
                                const int *i_section = NULL,
                                double        i_miss = default_missing<double>()
                                )
    : rectangle_source_base(i_tit, iA, iB, i_torus, i_section, i_miss),
      arra(i_array), component(i_component)
    {}

    /// 2.
    ptr_to_struct_matrix_source(const char     *i_tit,
                                rect_geometry   &geom,
                                STRUCT_T    **i_array,
                                TYP_POLA  i_component,
                                const int *i_section = NULL,
                                double        i_miss = default_missing<double>()
                                )
    : rectangle_source_base(i_tit, geom, i_miss,i_section),
      arra(i_array), component(i_component)
    {}

    /// Destruktor.
    ~ptr_to_struct_matrix_source() override // NOLINT(modernize-use-equals-default)
    {
        //cerr<<"~ptr_to_struct_matrix_source(): "<<name()<<'\n';
    }
    /// @}

// METHODS:
//========

    /// Daje następną z `la*lb` liczb w iteracji.
    double get(iterator_h &p) override
    {
        assert(p != NULL);
        size_t pom1 = _next(p);
        if(pom1 < this->get_rect_geometry()->get_size())
        {
            STRUCT_T *pom2 = arra[pom1];
            if(pom2)
                return pom2->*component;
        }
        //Nie udało się, bo coś tam.
        return miss;
    }

    /// Przetwarza index uzyskany z geometrii na wartość komponentu struktury wskazywanej przez komórkę tablicy.
    double get(size_t index) override
    {
        size_t s = get_rect_geometry()->get_size();
        assert(index < get_rect_geometry()->get_size());
        if(index < s /*&& index!=ULONG_MAX*/)
        {
            STRUCT_T *pom2 = arra[index];
            if(pom2)
                return pom2->*component;
        }
        //Nie udało się, bo coś tam.
        return miss;
    }

    /// Na podstawie wewnętrznej geometrii wylicza liczbę wartości, a potem określa minimum i maksimum.
    /// Żeby uniknąć obliczania można wymusić minimum i maksimum metodą `set_min_max`.
    void bounds(size_t &num, double &min, double &max) override;

};

/// @brief @EN{ . }
///        @PL{ Klasa czytająca z dowolnej prostokątnej tablicy struktur za pomocą wskaźnika do metody.. }
/// @details ...
template<class STRUCT_T, class METHOD_T>
class method_matrix_source : public rectangle_source_base
//------------------------------------------------
{
    typedef METHOD_T (STRUCT_T::* TYP_METODY)();

    STRUCT_T      *arra; ///< Tablica struktur
    TYP_METODY   method; ///< Wskaźnik do składowej metody struktury/klasy.

    wb_dynarray<METHOD_T> cache; ///< Ostatnio policzone dane.
    int cache_ok; ///< Flaga, czy `cache` aktualny.

public:
    /// @name Constructors and destructor.
    /// \param i_method wskaźnik do metody struktury zwracającej jakąś liczbę.
    /// @{

    /// Constructor 1.
    method_matrix_source(const char     *i_tit,
                         size_t iA, size_t  iB,
                         STRUCT_T     *i_array,
                         TYP_METODY   i_method,
                         int          i_torus = 1,
                         const int *i_section = NULL,
                         double        i_miss = default_missing<double>()
                        )
    : rectangle_source_base(i_tit, iA, iB, i_torus, i_section, i_miss),//rectangle_source_base(iA, iB, i_tit, i_torus, i_section, i_miss),
      arra(i_array),method(i_method),
      cache(iA * iB),cache_ok(0)
    {}

    /// Constructor 2.
    method_matrix_source(const char         *i_tit,
                         rect_geometry       &geom,
                         STRUCT_T         *i_array,
                         TYP_METODY       i_method,
                         const int     *i_section = NULL,
                         double            i_miss = default_missing<double>()
                        )
    : rectangle_source_base(i_tit, geom, i_miss,i_section),
      arra(i_array), method(i_method),
      cache(geom.get_height() * geom.get_width()), cache_ok(0)
    {}

    /// Destructor.
    ~method_matrix_source() override // NOLINT(modernize-use-equals-default)
    {
#ifndef NDEBUG
        cerr << "~method_matrix_source(): " << name() << '\n';
#endif
    }
    /// @}

// METHODS:
//========

    /// Sięganie do wyniku wskazywanej metody (tu bezparametrowej, ale w klasach potomnych niekoniecznie?).
    /// Raz wyciągnięte dane są trzymane w `cache`, chyba że zostanie on oznaczony jako "nieważny"/"nieaktualny".
    double _get(size_t i)
    {
        double pom;
        if(cache_ok)
            pom = cache[i];
        else
            pom = cache[i] = (arra[i].*method)();
        return pom;
    }

    /// Daje następną z `la*lb` liczb korzystając z `_get`.
    double get(iterator_h &p) override
    {
        double ret = 0;
        assert(p != NULL);
        size_t pom = _next(p);

        if(pom != ULONG_MAX)
            ret = _get(pom);
        else
            ret = miss;

        if(p == NULL) //Koniec danych
            cache_ok = 1; //Więc już wszystkie w cachu

        return ret;
    }

    /// Przetwarza `index` uzyskany z geometrii na daną, ale używając `_get`;
    double get(size_t index) override
    {
        assert(index < get_rect_geometry()->get_size());
        return _get(index);
    }

    /// Na podstawie wewnętrznej geometrii wylicza liczbę wartości, a potem określa minimum i maksimum.
    /// Żeby uniknąć obliczania można wymusić minimum i maksimum metodą `set_min_max`.
    void bounds(size_t &num, double &min, double &max) override;

    /// Ustalanie informacji o wersji danych z oznaczeniem nieaktualności `cache`.
    void new_data_version(int change = 1, unsigned increment = 1) override
    {
        rectangle_source_base::new_data_version(change, increment);
        cache_ok = 0; //Cache już nieaktualny
    }


};

/// @brief @EN{ . }
///        @PL{ Klasa czytająca z dowolnej prostokątnej tablicy wskaźników do struktur za pomocą wskaźnika do metody. }
/// @details ...
template<class STRUCT_T, class METHOD_T>
class method_by_ptr_matrix_source : public rectangle_source_base
//------------------------------------------------
{
    typedef METHOD_T (STRUCT_T::* TYP_METODY)();

    STRUCT_T   **arra; ///< Tablica wskaźników do struktur/obiektów.
    TYP_METODY method; ///< Wskaźnik do metody.

    wb_dynarray<METHOD_T> cache; ///< Ostatnio policzone dane.
    int cache_ok;

public:
    /// @name Constructors and destructor.
    /// \param i_method wskaźnik do metody struktury zwracającej jakąś liczbę.
    /// @{

    /// Constructor 1.
    method_by_ptr_matrix_source(const char     *i_tit,
                                size_t iA, size_t  iB,
                                STRUCT_T    **i_array,
                                TYP_METODY   i_method,
                                int          i_torus = 1,
                                const int *i_section = NULL,
                                double        i_miss = default_missing<double>()
                               )
    : rectangle_source_base(i_tit, iA, iB, i_torus, i_section, i_miss),
      arra(i_array), method(i_method),
      cache(iA * iB), cache_ok(0)
    {}

    /// Constructor 2.
    method_by_ptr_matrix_source(const char     *i_tit,
                                rect_geometry   &geom,
                                STRUCT_T    **i_array,
                                TYP_METODY   i_method,
                                const int *i_section = NULL,
                                double        i_miss = default_missing<double>()
                               )
    : rectangle_source_base(i_tit, geom, i_miss,i_section),
      arra(i_array),method(i_method),
      cache(geom.get_height() * geom.get_width()), cache_ok(0)
    {}

    /// Destructor.
    ~method_by_ptr_matrix_source() override // NOLINT(modernize-use-equals-default)
    {
#ifndef NDEBUG
        //cerr<<"~method_by_ptr_matrix_source(): "<<name()<<'\n';
#endif
    }
    /// @}

// METHODS:
//========

    /// Wyliczanie danych. Sięga do wyniku funkcji (tu bezparametrowej, ale w klasach potomnych niekoniecznie).
    double _get(size_t i)
    {
        double pom;
        if(cache_ok)
        {
            if(arra[i] != NULL)	//Troche to zmniejsza wartość cache'u
                pom = cache[i];
            else
                pom = miss;
        } else
        {
            if(arra[i] != NULL)
                pom = cache[i] = (arra[i]->*method)();
            else
                pom = miss;
        }

        return pom;
    }

    /// Przetwarza `index` uzyskany z geometrii na daną, ale używając `_get`;
    double get(size_t index) override
    {
        assert(index < get_rect_geometry()->get_size());
        return _get(index);
    }

    /// Daje następną z `la*lb` liczb korzystając z `_get`.
    double get(iterator_h &p) override
    {
        double ret = 0;
        assert(p != NULL);
        size_t pom = _next(p);

        if(pom != ULONG_MAX)
            ret = _get(pom);
        else
            ret = miss;

        if(p == NULL) //Koniec danych
            cache_ok = 1; //Więc już wszystkie w cachu

        return ret;
    }

    /// Na podstawie wewnętrznej geometrii wylicza liczbę wartości, a potem określa minimum i maksimum.
    /// Żeby uniknąć obliczania można wymusić minimum i maksimum metodą `set_min_max`.
    void bounds(size_t &num, double &min, double &max) override;

    /// Ustalanie informacji o wersji danych z oznaczeniem nieaktualności `cache`.
    void new_data_version(int change = 1, unsigned increment = 1) override
    {
        rectangle_source_base::new_data_version(change, increment);
        cache_ok = 0; //Cache już nieaktualny
    }

};

/*
class file_source:public data_source_base;
*/

// IMPLEMENTACJE:
//===============

template<class T> inline
void matrix_source<T>::bounds(size_t &num, double &min, double &max)
{
    num = get_rect_geometry()->get_width() * get_rect_geometry()->get_height();
    if(y_min < y_max)	//Są już dane
    {
        min = y_min;
        max = y_max;
        return;
    }

    //Nie są dane, więc próbkujemy — co trochę kosztuje
    min = wbrtm::limit<T>::Max();
    max = wbrtm::limit<T>::Min();
    iterator_h iterator = get_rect_geometry()->make_global_iterator();
    while(iterator)
    {
        size_t i = get_rect_geometry()->get_next(iterator);
        if(min > arra[i]) min = arra[i];
        if(max < arra[i]) max = arra[i];
    }
    get_rect_geometry()->destroy_iterator(iterator);
}

template<class STRUCT_T, class FIELD_T>
void struct_matrix_source<STRUCT_T, FIELD_T>::bounds(size_t &num, double &min, double &max)
{
    num = get_rect_geometry()->get_width() * get_rect_geometry()->get_height();
    if(y_min < y_max)	//Sa dane
    {
        min = y_min;
        max = y_max;
        return;
    }

    //Nie są dane, więc próbkujemy, co trochę kosztuje.
    min = wbrtm::limit<FIELD_T>::Max();
    max = wbrtm::limit<FIELD_T>::Min();
    iterator_h iterator = get_rect_geometry()->make_global_iterator();
    while(iterator)
    {
        size_t i = get_rect_geometry()->get_next(iterator);
        double pom = arra[i].*component;
        if(min > pom) min = pom;
        if(max < pom) max = pom;
    }
    get_rect_geometry()->destroy_iterator(iterator);
}

template<class STRUCT_T, class FIELD_T>
void ptr_to_struct_matrix_source<STRUCT_T, FIELD_T>::bounds(size_t &num, double &min, double &max)
{
    num = get_rect_geometry()->get_width() * get_rect_geometry()->get_height();
    if(y_min < y_max)	//Sa dane
    {
        min = y_min;
        max = y_max;
        return;
    }

    //Nie są dane, więc próbkujemy — co trochę kosztuje
    min = wbrtm::limit<FIELD_T>::Max();
    max = wbrtm::limit<FIELD_T>::Min();
    iterator_h iterator = get_rect_geometry()->make_global_iterator();
    while(iterator)
    {
        size_t i = get_rect_geometry()->get_next(iterator);
        STRUCT_T *pom1 = arra[i];
        if(pom1 == NULL)
            continue; //Nie ma szans na wartość, więc ignorujemy
        double pom = pom1->*component;
        if(min > pom) min = pom;
        if(max < pom) max = pom;
    }
    get_rect_geometry()->destroy_iterator(iterator);
}

template<class STRUCT_T, class METHOD_T>
void method_matrix_source<STRUCT_T, METHOD_T>::bounds(size_t &num, double &min, double &max)
{
    num = get_rect_geometry()->get_width() * get_rect_geometry()->get_height();
    if(y_min < y_max)	//Sa dane
    {
        min = y_min;
        max = y_max;
        return;
    }

    //Nie są dane, więc próbkujemy — co trochę kosztuje
    min = wbrtm::limit<METHOD_T>::Max();
    max = wbrtm::limit<METHOD_T>::Min();
    iterator_h iterator = get_rect_geometry()->make_global_iterator();

    //assert(miss!=???);
    //missing(); //Aktualizacja wartości "miss" ???? DEBUG THIS

    while(iterator)
    {
        size_t i = get_rect_geometry()->get_next(iterator);
        double pom = _get(i); //Dostęp liniowy
        if(!is_missing(pom))
        {
            if(min > pom) min = pom;
            if(max < pom) max = pom;
        }
    }

    cache_ok = 1; //Wszystkie przeszły więc cache pełny
    get_rect_geometry()->destroy_iterator(iterator);
}

template<class STRUCT_T, class METHOD_T>
void method_by_ptr_matrix_source<STRUCT_T, METHOD_T>::bounds(size_t &num, double &min, double &max)
{
    num = get_rect_geometry()->get_width() * get_rect_geometry()->get_height();
    if(y_min < y_max)	//Sa dane
    {
        min = y_min;
        max = y_max;
        return;
    }

    //Nie są dane, więc próbkujemy — co trochę kosztuje
    min = wbrtm::limit<METHOD_T>::Max();
    max = wbrtm::limit<METHOD_T>::Min();
    iterator_h iterator = get_rect_geometry()->make_global_iterator();

    get_missing(); //Aktualizacja wartości "miss"
    while(iterator)
    {
        size_t i = get_rect_geometry()->get_next(iterator);
        double pom = _get(i); //Dostęp liniowy
        if(!is_missing(pom))
        {
            if(min > pom) min = pom;
            if(max < pom) max = pom;
        }
    }
    cache_ok = 1; //Wszystkie przeszły więc cache pełny
    get_rect_geometry()->destroy_iterator(iterator);
}

}} // end-of-namespaces sym2::data

#pragma clang diagnostic pop
/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roslin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk               */
/*        MAIL: borkowsk@iss.uw.edu.pl                                */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
#endif //SYMSHELL2_MATRIX_SOUR_HPP_INCLUDED_




