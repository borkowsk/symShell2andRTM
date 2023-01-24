/** \file wb_bits.h
 *  \brief Podstawowe operacje na 8 bitach bajtu
 *         --------------------------------------
 *  \details ... ...
 *           ... ...
 *  \author Wojciech Borkowski @ Institut for Social Studies, University of Warsaw
 *  \date   2023-01-23 (last modification)
 *  \note   Like many others wbrtm files created around year 1996.
 *  \ingroup OBSOLETE
 */
//  (W.Borkowski)
//#/////////////////////////////////////////////////////
#ifndef __WB_BITS_H_INCLUDED__
#define __WB_BITS_H_INCLUDED__

#include <limits.h>

#ifndef BITSPERBYTE //Jakby taki komentarz :-)
#define BITSPERBYTE (CHAR_BIT)  //!!!!
#endif

inline unsigned bits(unsigned char u)
{
#if BITSPERBYTE!=8 //NIETYPOWA SYTUACJA, ALE MOZLIWA
#error NOT TESTED IN SUCH SITUATION!!! YOU CAN TRY BY COMMENT THIS LINE.
	unsigned sum=0;
	for(unsigned i=0;i<(BITSPERBYTE*sizeof(u));i++)
		{
		if(u&1)
			sum++;
		u>>=1;
		}
	return sum;
#else           //TYPOWA SYTUACJA DLA PC i WIEKSZOSCI WORKSTATION
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

inline void byte2bits(unsigned char u,unsigned char* out)
{
#if BITSPERBYTE!=8 //NIETYPOWA SYTUACJA, ALE MOŻLIWA

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

inline int highest_bit_index(unsigned value)
// this can be done as a binary search, reducing complexity of
//O(N) (for an N-bit word) to O(log(N)).
//http://stackoverflow.com/questions/9041837/find-the-index-of-the-highest-bit-set-of-a-32-bit-number-without-loops-obviously
{									//assert(sizeof(unsigned)==4);
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
#endif  //__WB_BITS_H_INCLUDED__
