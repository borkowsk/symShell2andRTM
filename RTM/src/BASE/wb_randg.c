/********************************************************************
 * \file
 * \brief  randg generuje liczby losowe o rozkładzie prostym z przedziału (0,1)
 *====================================================================================================================
 *  \details Flaga dla "wb_rtm" kończenia z zapytaniem.
 *           Created by borkowsk before 2000.
 *  \copyright ....
 *  \date 2022-10-12 (last modification)
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#define USES_RANDG
#include "random.h"
static short int di=-1;

#ifdef __cplusplus
extern "C" {
#endif

float randg();
void  srandg(short int srandg_init_val);
float randnorm();
float randexp();

#ifdef __cplusplus
} //extern C
#endif

/// \note TODO - MUTEXOWE ZABEZPIECZENIE ::rand() na wielowątkowość ALE JAK TO ZROBIC W STD C ???
long my_rand()
{
    return rand();
}

/*INT_MAX musi byc 0x7fffffff*/
typedef long int32; /*Wg ANSI C long ma zawsze 32bity */

/// \details Parametr jest traktowany ABS i zmieniany w liczbę ujemną.
void srandg(short int srandg_init_val)
{                                                                                            assert(srandg_init_val!=0);
	di=-abs(srandg_init_val);
}

/// \note TODO - MUTEXOWE ZABEZPIECZENIE ::randg() na wielowątkowość. ALE JAK TO ZROBIĆ W STD C ???
float randg()
{
  static short int        inext, inextp;
  static int32            ma[55];
  int32                   mj, mk, mbig = 1000000000;
  short int               i, k;

  if (di < 0)
  {
    mj = 161803398 - abs(di);
    mj %= mbig;
    ma[54] = mj;
    mk = 1;
    for (i=1; i <= 54; i++)
    {
      k = (21 * i) % 55;
      ma[k - 1] = mk;
      mk = mj - mk;
      if (mk < 0)
	mk += mbig;
      mj = ma[k - 1];
    };
    for (k = 1; k <= 4; k++)
      for (i = 1; i <= 55; i++)
      {
	ma[i - 1] -= ma[(i + 30) % 55];
	if (ma[i - 1] < 0)
	  ma[i - 1] += mbig;
      };
    inext = 0;
    inextp = 31;
    di= 1;
  };
  inext++;
  if (inext == 56)
    inext = 1;
  inextp++;
  if (inextp == 56)
    inextp = 1;
  mj=ma[inext - 1] - ma[inextp - 1];
  if (mj < 0)
    mj += mbig;
  ma[inext - 1] = mj;
  return (mj * 1.e-9f);/* CZY MOZE BYC 1.e-9f czy winno BYC DOUBLE */
}

float randnorm()
{
	//float ran1(long *idum);
	static int iset=0;
	static float gset;
	float fac,rsq,v1,v2;
   
	//if (*idum < 0) iset=0;
    if  (iset == 0) 
    {
        do {
            v1=2.0f*randg()-1.0f;
            v2=2.0f*randg()-1.0f;
            rsq=v1*v1+v2*v2;
        } while (rsq >= 1.0 || rsq == 0.0);

        fac=(float)sqrt(-2.0*log(rsq)/rsq);
        gset=v1*fac;
        iset=1;
        return v2*fac;
    } 
    else 
    {
        iset=0;
        return gset;
    }
}

float randexp()
{
	float dum;
   
	do
	 dum=randg();
	while (dum == 0.0);

	return (float)( -log(dum) );
}



/* *******************************************************************/
/*			 WBRTM  version 2006 - renovation 2022                   */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiów Społecznych Uniwersytetu Warszawskiego        */
/*        WWW:  ...                                                  */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/