// IMPLEMENTACJA CACHE-U ALOKACJI ITERATORÓW
//*////////////////////////////////////////////////////////////////////////////

//#include "INCLUDE/platform.hpp"

#include <iostream>
#include <cstdlib>
#include <cstdio>
using namespace std;

#include "geombase.hpp"

const
size_t tab_size=8;
size_t cur_size=0;
size_t max_size=0;

unsigned long hit_num=0;
unsigned long cal_num=0;

static void*	bufory[tab_size];
static size_t	rozmiary[tab_size];

class menager_bufora_alokacji_iteratorow
{
public:
~menager_bufora_alokacji_iteratorow()
{
#ifndef NDEBUG
	if(cal_num>0)
	{
	//if(cerr.good()&& (!cerr.eof()))//Może już go nie być, ale to sprawdzenie i tak nie za bradzo dzaiała!!!
	//	cerr<<"Iterator's allocator buffer:\n"
	//	<<"max len.="<<max_size<<"\n"
	//	<<"hit rat.="<<hit_num/double(cal_num)<<endl;
        fprintf(stderr,"%s\n%s %lu\n%s %g\n","Iterator's allocator buffer:","max len.=",max_size,"hit rat.=",double(hit_num/double(cal_num)));
	}
	else
	{
	//if(cerr.good()&& (!cerr.eof()))//Może już go nie być (!!!), ale to sprawdzenie i tak nie za bradzo dzaiała!!!
	//	cerr<<"Iterator's allocator buffer never been used.\n";
		fprintf(stderr,"%s\n","Iterator's allocator buffer never been used.");
	}
#endif
											assert(max_size<=tab_size);//max_size>=0 zawsze bo unsigned
	for(size_t i=0;i<tab_size;i++)
	{
		if(bufory[i]!=NULL)
		{                                   assert(i<cur_size);
			char* pc=(char*)bufory[i];      assert(rozmiary[i]!=0);
#ifdef _USE_ALLOCATORS_
			delete [rozmiary[i]] pc;//Gdyby kiedys obslugiwano alokatory tablicowe
#else
			delete [] pc;
#endif
		}
	}
}

} __menager_bufora_alokacji_iteratorow;

//Zakladam ze tablice sa inicjowane na 0!!


#define USE_OPTYMIZ_ALLOC

void* geometry_base::iterator_base::operator new (size_t s)
{
																		assert(s>0);
	cal_num++;
	if(cal_num==0)//Przekrecony (???)
	{
		hit_num=0;
		cal_num=1;
	}
#ifdef USE_OPTYMIZ_ALLOC
    //Przeszukiwanie tablicy ostatnio uzywanych i zwolnionych iteratorow
	if(cur_size!=0)
	{
	for(size_t i=0;i<cur_size;i++)//szukam czy nie ma wolnego bufora
		if(rozmiary[i]==s)//Znalazlem
		{
			hit_num++;
			void* pom=bufory[i];
			assert(pom!=NULL);

			bufory[i]=NULL;
			rozmiary[i]=0;//Usuwam z listy wolnych

			if(i==cur_size-1)
			{
				cur_size--;
				assert(cur_size!=-1);
			}

			return pom;
		}
	}
#endif
	//Nie znaleziono slotu z takim blokiem - zwykla alokacja
	return new char[s];
}

void  geometry_base::iterator_base::operator delete (void* p, size_t s)
{
	char* pc=(char*)p;

#ifdef USE_OPTYMIZ_ALLOC
	for(size_t i=0;i<tab_size;i++)
	{

		assert(p!=bufory[i]);

		if(rozmiary[i]==0)//Wolny slot
		{
			assert(i<=cur_size);//najwyzej o 1 wiecej
			assert(bufory[i]==NULL);

			bufory[i]=p;//Zapamientuje
			rozmiary[i]=s;

			if(i==cur_size)
			{
				cur_size++;
				if(cur_size>max_size)
					max_size=cur_size;
			}

			return;
		}
	}
#endif

	//Nie znaleziono tego slotu dla tego bloku, zwykle zwolnienie
#ifdef _USE_ALLOCATORS_
	delete [s] pc;      //choć "s" będzie zignorowane
#else
    delete [] pc;
#endif
}
