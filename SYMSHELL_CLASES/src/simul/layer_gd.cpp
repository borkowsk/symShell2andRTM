// Implementacja bazy warstwy symulacji
//*////////////////////////////////////////////////////////////////////
#include <limits.h>
#include "../CONTRLIBS/GD2/gd.h"

#include "compatyb.h"
#define USES_STDC_RAND
#include "random.h"

#include "layer.hpp"

extern "C" gdImagePtr gdImageCreateFromBmp(FILE *f); //Uzupelnienie dla BMP
//const unsigned long any_layer_base::FULL=UINT_MAX;

//Wczytanie pliku GIF lub BMP
int rectangle_layer::init_from_bitmap(const char* filename,void* user_data)
{
size_t Width=0,i=0,Height=0,j=0;//Rozmiary i indeksy petli
if(filename==nullptr || filename[0]=='\0')
			return 0;
FILE* file=fopen(filename,"rb");
if(!file)
{
	perror(filename);
	return 0;
}

gdImagePtr mapa=nullptr;
if(strcmp(strrchr(filename,'.'),".gif")==0)
	mapa=gdImageCreateFromGif(file);//Uchwyt do mapy
	else
	mapa=gdImageCreateFromBmp(file);

if(!mapa)
	return 0;//Raczej nie wywoluje bo wczesniej wypada

fclose(file);

//Ustalenie co jest większe
if(gdImageSX(mapa)>MainGeometry.get_width())
		Width=MainGeometry.get_width();
		else
		Width=gdImageSX(mapa);

if(gdImageSY(mapa)>MainGeometry.get_height())
		Height=MainGeometry.get_height();
		else
		Height=gdImageSY(mapa);

//Wczytywanie
int background=gdImageGetTransparent(mapa);
for(i=0;i<Width;i++)
	for(j=0;j<Height;j++)
		{
		int color=gdImageGetPixel(mapa,i,j);
		if(color!=background)
			assign_rgb(
				i,j,
				gdImageRed(mapa,color),
				gdImageGreen(mapa,color),
				gdImageBlue(mapa,color),
				user_data);
		}
return 1;
}

