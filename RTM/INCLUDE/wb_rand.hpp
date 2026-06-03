/** \file wb_rand.hpp
 *  @brief
 *       @PL{ PODSTAWOWE KLASY GENERATORÓW LICZB PSEUDOLOSOWYCH. }
 *       @EN{ BASIC CLASSES OF PSEUDORANDOM NUMBER GENERATORS. }
 *  @date 2026-06-03 (last modification)
 *        ======================================================
 *  \details
 *   Contents:
 *   - RandomGenerator - interface to random generators
 *   - RandSTDC	- Random generator build in standard C
 *   - RandG	- Random generator wrote in C based on "Numerical Recipes"
 * \note Like many others wbrtm files created around year 1996.
 * \warning OBSOLETE
 *   - RandBSD - Random generator from BSD UNIX
 *   - RandSVR4 - Random generator from System V UNIX
 *
 * \author Wojciech Borkowski @ Institut for Social Studies, University of Warsaw
 */

#ifndef __cplusplus
#error Only C++ supported!!!
#endif

#ifndef WB_RAND_HPP_INCLUDED_
#define WB_RAND_HPP_INCLUDED_  1
#include <cassert>
#include <climits>
#include <cstdlib>
#include <ctime>
#ifndef unix
#include <sys/timeb.h>
#endif

/**
 * @defgroup Randomize Różne klasy i funkcje związane z generowaniem pseudolosowości
 *   \brief  Unifikacja randomizacji sprzed czasów, gdy ujęto to w standard C++
 *   \details
 *           Zarówno funkcje, jak i klasy do różnorodnego stosowania. Część na podstawie
 *           rozwiązania systemowego w języku C, część z Numerical Recipies i innych
 *           podobnych źródeł.
 */
/// @{

extern "C"
{
    long    my_rand();          /**< (MUTEX-OWO PROTECTED)::rand() for multithreaded programs */
    float	randg();            /**< Numerical Recipes random number generator (TODO MUTEX inside?) */
	float	randnorm();			/**< Normalised output of randg() */
	float	randexp();			/**< Exponential output of randg() */
    void    srandg(short s);    /**< Seed setting for generator. \param s - value for seed. */
}

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

    /// \brief Random number generator class interface.
    class RandomGenerator
    {
    public:
        ///Max Value that can be returned from Rand().
        virtual unsigned long RandomMax() = 0;

        ///Returned unsigned long from 0 to RandomMax.
        virtual unsigned long Rand() = 0;

        ///Returned double from <0 to 1).
        virtual double DRand() = 0;
        
        ///Returned unsigned long from `0` to `i`.
        virtual unsigned long Random(unsigned long i) = 0;

        ///Initialisation for a well-defined repeatable sequence.
        virtual void Seed(unsigned long i) = 0;

        ///Initialisation for a random selected sequence.
        virtual void Reset() = 0;

        ///Required for abstract classes.
        virtual ~RandomGenerator() = default;
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

        /// \brief Returned `ulong` from `0` to `i`.
        unsigned long Random(unsigned long i) override
        {
            unsigned long ret = (unsigned long) (((double) (::randg)() * (i)));
            if (ret >= i)
                ret = i - 1;
            return ret;
        }

        /// \brief Returned double from <0 to 1).
        double DRand() override
        {
            return ((::randg)());
        }

        /// \brief Generation of normal distribution. Defined only for this class.
        double NormRand() { return ::randnorm(); }

        /// \brief Generation of exponential distribution. Defined only for this class.
        double ExpRand() { return ::randexp(); }

        /// \brief Initialisation for a well-defined repeatable sequence.
        void Seed(unsigned long i) override { ::srandg((short int) i); }

        /// \brief Initialisation for a random selected sequence.
        void Reset() override { ::srandg((unsigned) time(NULL)); }

        /// \brief DEFAULT CONSTRUCTOR.
        RandG()
        {
            RandG::Reset();
        }

         /// Empty destructor.
        ~RandG() override = default;
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
        unsigned long Random(unsigned long i) override
        {
            unsigned long ret=(unsigned long) (((double) (my_rand)() * (i)) / ((double) RAND_MAX + 1));
            if (ret >= i)
                ret = i - 1;
            return  ret;
        }

        /// \brief Returned double from <0 to 1)
        double DRand() override
        {
            return ((double) (my_rand)()) / (double) RAND_MAX;
        }

        /// \brief Initialisation for a well-defined repeatable sequence.
        void Seed(unsigned long i) override { (::srand)(i); }

        /// \brief Initialisation for a random-selected sequence.
        void Reset() override { (::srand)((unsigned) time(NULL)); }

        /// \brief DEFAULT CONSTRUCTOR.
        RandSTDC() { RandSTDC::Reset(); }

        /// Empty destructor.
        ~RandSTDC() override = default;
    };

} //namespace

extern wbrtm::RandG    TheRandG;             ///< ready to use generator using `randg()`.
extern wbrtm::RandSTDC TheRandSTDC;          ///< ready to use generator using `C::rand()`.

/// @}

/* ******************************************************************/
/*                      WBRTM  version 2026                         */
/* ******************************************************************/
/*            THIS CODE IS DESIGNED & COPYRIGHT BY:                 */
/*             W O J C I E C H   B O R K O W S K I                  */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif





