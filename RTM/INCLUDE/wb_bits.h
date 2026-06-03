/** \file
 * @brief
 *       @PL{ Podstawowe operacje na 8 bitach bajtu. }
 *       @EN{  }
 * @date 2026-06-03 (last modification)
 *       ---------------------------------------
 *  \details ... ...
 *           ... ...
 *  \author Wojciech Borkowski @ Institut for Social Studies, University of Warsaw
 *
 *  \note   Like many others wbrtm files created around year 1996.
 *          Cała biblioteka WB_RTM to jest nieco odremontowane muzeum kodu z wieku XX.
 *  \ingroup OBSOLETE
 */
//#////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef WB_BITS_H_INCLUDED_
#define WB_BITS_H_INCLUDED_

#include <climits>

#ifndef BITS_PER_BYTE //Jakby taki komentarz :-)
#define BITS_PER_BYTE (CHAR_BIT)  //!!!
#endif

/// @brief @PL{ Suma jedynek w ciągu bitowym. } @EN{    }
inline unsigned bits(unsigned char u)
{
#if BITS_PER_BYTE!=8 //NIETYPOWA SYTUACJA, ALE TEORETYCZNIE MOŻLIWA
#error NOT TESTED IN SUCH SITUATION!!! YOU CAN TRY BY COMMENT THIS LINE.
    unsigned sum=0;
    for(unsigned i=0;i<(BITS_PER_BYTE*sizeof(u));i++)
        {
        if(u&1)
            sum++;
        u>>=1;
        }
    return sum;
#else           //TYPOWA SYTUACJA DLA PC i WIEKSZOŚCI WORKSTATION
    unsigned sum=0;
    if((u & 1)!=0) sum++;
    u>>=1;
    if((u & 1)!=0) sum++;
    u>>=1;
    if((u & 1)!=0) sum++;
    u>>=1;
    if((u & 1)!=0) sum++;
    u>>=1;
    if((u & 1)!=0) sum++;
    u>>=1;
    if((u & 1)!=0) sum++;
    u>>=1;
    if((u & 1)!=0) sum++;
    u>>=1;
    if((u & 1)!=0) sum++;
    return sum;
#endif
}

/// @brief @PL{ Rozwijanie bajtu w tablicę zer i jedynek. } @EN{    }
inline void byte2bits(unsigned char u,unsigned char* out)
{
#if BITS_PER_BYTE!=8 //NIETYPOWA SYTUACJA, ALE MOŻLIWA

#error Function not implemented for non 8 bits bytes

#else               //TYPOWA SYTUACJA DLA PC i WIEKSZOSCI WORKSTATION
    out[0]=(u & 0x01)!=0;
    out[1]=(u & unsigned(0x01<<1))!=0;
    out[2]=(u & unsigned(0x01<<2))!=0;
    out[3]=(u & unsigned(0x01<<3))!=0;
    out[4]=(u & unsigned(0x01<<4))!=0;
    out[5]=(u & unsigned(0x01<<5))!=0;
    out[6]=(u & unsigned(0x01<<6))!=0;
    out[7]=(u & unsigned(0x01<<7))!=0;
#endif
}

///  @brief @PL{ Jaki jest najcięższy ustawiony bit w danej zmiennej typu "unsigned short". } @EN{    }
/// @warning @PL{ Obawiam się, że to było zaprojektowane dla typu @c uint16_t !!! A do tego bardzo niewydajne. }
///          @EN{    }
// this can be done as a binary search, reducing complexity of
//O(N) (for an N-bit word) to O(log(N)).
//http://stackoverflow.com/questions/9041837/find-the-index-of-the-highest-bit-set-of-a-32-bit-number-without-loops-obviously
inline uint8_t highest_bit_index_bsearch(unsigned  value)
{                                                                     static_assert(sizeof value == 4, "only 16 bits!");
  if(value == 0) return 0;
  int depth = 0;
  int exponent = 16;

  while(exponent > 0)
  {
    int shifted = value >> (exponent);
    if(shifted > 0)
    {
      depth += exponent;
      if(shifted == 1) return depth + 1;
      value >>= exponent;
    }
    exponent /= 2;
  }

  return depth + 1;
}

/// @brief @PL{ Funkcja pomocnicza dla 'highest_bit_index'. } @EN{    }
inline void propagateBits_(uint32_t *n) {
    *n |= *n >> 1;
    *n |= *n >> 2;
    *n |= *n >> 4;
    *n |= *n >> 8;
    *n |= *n >> 16;
}

///  @brief @PL{ Jaki jest najcięższy ustawiony bit w danej zmiennej typu "unsigned 32-bit". NIETESTOWANE! }
///         @EN{    }
// Solution using lookup table and magic number multiplication (probably the best AFAIK)
// First you use the following function to clear all the bits except the highest one:
//http://stackoverflow.com/questions/9041837/find-the-index-of-the-highest-bit-set-of-a-32-bit-number-without-loops-obviously
inline uint8_t highest_bit_index(uint32_t b)
{
    static const uint32_t Magic = (uint32_t) 0x07C4ACDD;

    static const int BitTable[32] = {
            0,  9,  1, 10, 13, 21,  2, 29,
            11, 14, 16, 18, 22, 25,  3, 30,
            8, 12, 20, 28, 15, 17, 24,  7,
            19, 27, 23,  6, 26,  5,  4, 31,
    };
    propagateBits_(&b);

    return BitTable[(b * Magic) >> 27];
}

/* ******************************************************************/
/*                      WBRTM  version 2026                         */
/* ******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
/*    GITHUB: https://github.com/borkowsk                           */
/*                                                                  */
/*                               (Don't change or remove this note) */
/* ******************************************************************/
#endif  //WB_BITS_H_INCLUDED_
