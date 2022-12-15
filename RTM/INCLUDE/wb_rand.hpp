/** \file wb_rand.hpp
*  \brief BASIC CLASSES OF PSEUDORANDOM NUMBER GENERATORS
*  \author Wojciech Borkowski @ Institut for Social Studies, University of Warsaw
*  \details
*   Contents:
*   - RandomGenerator - interface to random generators
*   - RandSTDC	- Random generator build in standard C
*   - RandG	- Random generator wrote in C based on "Numerical Recipes"
* \note Like many others wbrtm files created around year 1996.
* \warning OBSOLETE
*   - RandBSD - Random generator from BSD UNIX
*   - RandSVR4 - Random generator from System V UNIX
*  \date 2022-12-15 (last modification)
*/

#ifndef __cplusplus
#error Only C++ supported!!!
#endif

#ifndef _WB_RAND_HPP_INCLUDED_
#define _WB_RAND_HPP_INCLUDED_  1
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#ifndef unix
#include <sys/timeb.h>
#endif

/**
 * @defgroup Randomize Różne klasy i funkcje związane z generowaniem pseudolosowości
 *   \brief  Unifikacja randomizacji sprzed czasów gdy ujeto to w standard C++
 *   \details
 *           Zarówno funkcje jak i klasy do różnorodnego stosowania. Część w oparciu
 *           o rozwiązania systemowe w jezyku C, część z Numerical Recipies i innych
 *           podobnych źródeł.
 */
/// @{

extern "C"
{
    long    my_rand();          /**< (MUTEX-OWO PROTECTED) :: rand () for multithreaded programs */
    float	randg();            /**< Numerical Recipes random number generator (TODO MUTEX inside?) */
	float	randnorm();			/**< Normalised output of randg() */
	float	randexp();			/**< Exponential output of randg() */
    void    srandg(short s);    /**< Seed setting for generator. \param s - value for seed. */
}

///\namespace wbrtm \brief WOJCIECH BORKOWSKI RUN TIME LIBRARY
namespace wbrtm {

    /// \brief Random number generator class interface.
    class RandomGenerator
    {
    public:
        ///Max Value that can be returned from Rand().
        virtual unsigned long RandomMax() = 0;

        ///Returned unsigned long from 0 to RandomMax.
        virtual unsigned long Rand() = 0;

        ///Returned unsigned long from 0 to i.
        virtual unsigned long Random(unsigned long i) = 0;

        ///Returned double from <0 to 1).
        virtual double DRand() = 0;

        ///Initialisation for well defined repeatable sequence.
        virtual void Seed(unsigned long i) = 0;

        ///Initialisation for random selected sequence.
        virtual void Reset() = 0;

        ///Required for abstract classes.
        virtual ~RandomGenerator() {}
    };

    /// \brief Random generator specialization using the randg() function.
    /// \details randg() should be (?) secured with a mutex.
    class RandG : public RandomGenerator
    {
    public:
        /// \brief Max Value that can be returned from Rand().
        unsigned long RandomMax() override { return (INT_MAX); }

        /// \brief Returned ulong from 0 to RandomMax.
        unsigned long Rand() override { return ((int) ((::randg)() * INT_MAX)); }

        /// \brief Returned ulong from 0 to i.
        unsigned long Random(unsigned long i) override
        {
            unsigned long ret = (unsigned long) (((double) (::randg)() * (i)));
            if (ret >= i) ret = i - 1;
            return ret;
        }

        /// \brief Returned double from <0 to 1).
        double DRand() override { return ((::randg)()); }

        /// \brief Generation of normal distribution. Defined only for this class.
        double NormRand() { return ::randnorm(); }

        /// \brief Generation of exponential distribution. Defined only for this class.
        double ExpRand() { return ::randexp(); }

        /// \brief Initialisation for well defined repeatable sequence.
        void Seed(unsigned long i) override { ::srandg((short int) i); }

        /// \brief Initialisation for random selected sequence.
        void Reset() override { ::srandg((unsigned) time(NULL)); }

        /// \brief DEFAULT CONSTRUCTOR.
        RandG() { RandG::Reset();}
        ~RandG() override {}
    };

    /// \brief A random generator specialization that uses the standard C language generator.
    class RandSTDC : public RandomGenerator
    {
    public:
        /// \brief Max Value that can be returned from Rand()
        unsigned long RandomMax() override { return (RAND_MAX); }

        /// \brief Returned ulong from 0 to RandomMax
        unsigned long Rand() override { return my_rand(); }

        /// \brief Returned ulong from 0 to i
        unsigned long Random(unsigned long i) override { return (int) (((double) (my_rand)() * (i)) / ((double) RAND_MAX + 1)); }

        /// \brief Returned double from <0 to 1)
        double DRand() override { return ((double) (my_rand)()) / (double) RAND_MAX; }

        /// \brief Initialisation for well defined repeatable sequence
        void Seed(unsigned long i) override { (::srand)(i); }

        /// \brief Initialisation for random selected sequence
        void Reset() override { (::srand)((unsigned) time(NULL)); }

        /// \brief DEFAULT CONSTRUCTOR.
        RandSTDC() { RandSTDC::Reset(); }
        ~RandSTDC() override {}
    };

} //namespace

extern wbrtm::RandG    TheRandG;             ///< ready to use generator using randg()
extern wbrtm::RandSTDC TheRandSTDC;          ///< ready to use generator using C ::rand()

/// @}

/* ******************************************************************/
/*                WBRTM  version 2022 for GuestXR                   */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif





