/// @file
/// @brief __IMPLEMENTATION OF ITERATOR ALLOCATION CACHE__ /<br>
///         _IMPLEMENTACJA CACHE-U ALOKACJI ITERATORÓW._
/// @date 2026-05-19 (modified)
//=============================================================================

//#include "INCLUDE/platform.hpp"
//#include <cstdlib>
#include <cstdio>
#include <iostream>

using namespace std;

#include "geombase.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "modernize-use-nullptr"
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
// --checks=-google-default-arguments.
#pragma ide diagnostic ignored "google-default-arguments"

namespace sym2 { namespace shell
{
        const
        size_t tab_size = 8;
        size_t cur_size = 0;
        size_t max_size = 0;

        unsigned long hit_num = 0;
        unsigned long cal_num = 0;

        static void *bufory[tab_size];
        static size_t rozmiary[tab_size];

        /// @brief @PL{ Bufor alokacji iteratorów. } @EN{ Iterator allocation buffer. }
        class manager_bufora_alokacji_iter_base
        {
        public:
            ~manager_bufora_alokacji_iter_base()
            {
#ifndef NDEBUG
                if(cal_num > 0)
                {
                    //if(cerr.good()&& (!cerr.eof())) //Może już go nie być, ale to sprawdzenie i tak nie za bardzo działa!!!
                    //	cerr<<"Iterator's allocator buffer:\n"
                    //	<<"max len.="<<max_size<<"\n"
                    //	<<"hit rat.="<<hit_num/double(cal_num)<<endl;
                    fprintf(stderr, "%s\n%s %lu\n%s %g\n", "Iterator's allocator buffer:", "max len.=", max_size,
                            "hit rat.=", double(hit_num) / double(cal_num));
                } else
                {
                    //if(cerr.good()&& (!cerr.eof())) //Może już go nie być (!!!), ale to sprawdzenie i tak nie za bardzo działa!!!
                    //	cerr<<"Iterator's allocator buffer never been used.\n";
                    fprintf(stderr, "%s\n", "Iterator's allocator buffer has never been used.");
                }
#endif
                assert(max_size <= tab_size); //max_size>=0 zawsze bo unsigned
                for(size_t i = 0; i < tab_size; i++)
                {
                    if(bufory[i] != NULL)
                    {
                        assert(i < cur_size);
                        char *pc = (char *) bufory[i];
                        assert(rozmiary[i] != 0);
#ifdef _USE_ALLOCATORS_
                        delete [rozmiary[i]] pc; //Gdyby kiedyś obsługiwano alokatory tablicowe
#else
                        delete[] pc;
#endif
                    }
                }
            }

        } manager_bufora_alokacji_dla_iterator_base_;

//Zakładam, że tablice są inicjowane na 0!!


#define USE_OPTIMIZED_ALLOC

        void *geometry_base::iterator_base::operator new(size_t s) noexcept
        {
            assert(s > 0);
            cal_num++;
            if(cal_num == 0) //Przekręcony (???) teoretycznie istnieje taka możliwość...
            {
                hit_num = 0;
                cal_num = 1;
            }
#ifdef USE_OPTIMIZED_ALLOC
            //Przeszukiwanie tablicy ostatnio używanych i zwolnionych iteratorów
            if(cur_size != 0)
            {
                for(size_t i = 0; i < cur_size; i++) //szukam czy nie ma wolnego bufora
                    if(rozmiary[i] == s) //Znalazłem
                    {
                        hit_num++;
                        void *pom = bufory[i];
                        assert(pom != NULL);

                        bufory[i] = NULL;
                        rozmiary[i] = 0; //Usuwam z listy wolnych

                        if(i == cur_size - 1)
                        {
                            cur_size--;
                            assert(cur_size != -1);
                        }

                        return pom;
                    }
            }
#endif
            //Nie znaleziono slotu z takim blokiem — zwykła alokacja, zazwyczaj na początku programu.
            return new char[s];
        }

        void geometry_base::iterator_base::operator delete(void *p, size_t s) noexcept
        {
            char *pc = (char *) p;

#ifdef USE_OPTIMIZED_ALLOC
            for(size_t i = 0; i < tab_size; i++)
            {
                assert(p != bufory[i]);

                if(rozmiary[i] == 0) //Wolny slot
                {
                    assert(i <= cur_size); //najwyżej o 1 więcej
                    assert(bufory[i] == NULL);

                    bufory[i] = p; //Zapamiętuje
                    rozmiary[i] = s;

                    if(i == cur_size)
                    {
                        cur_size++;
                        if(cur_size > max_size)
                            max_size = cur_size;
                    }

                    return;
                }
            }
#endif

            //Nie znaleziono tego slotu dla tego bloku, zwykle zwolnienie
#ifdef _USE_ALLOCATORS_
            delete [s] pc;      //choć "s" będzie zignorowane
#else
            delete[] pc;       // Czy to się w ogóle zdarza?
#endif
        }

}} //namespaces

#pragma clang diagnostic pop
/* ****************************************************************** */
/*               SYMSHELL2  version 2006/2022/2026                    */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT BY:                    */
/*            W O J C I E C H   B O R K O W S K I                     */
/*  Zakład Systematyki i Geografii Roślin Uniwersytetu Warszawskiego  */
/*  & Instytut Studiów Społecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://iss.uw.edu.pl/wojciech-borkowski/              */
/*        MAIL:  wborkowski@uw.edu.pl                                 */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */
