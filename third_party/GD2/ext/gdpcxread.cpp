/////////////////PCX LOAD FOR GD//////////////////////
//extern "C" gdImagePtr gdImageCreateFromPcx(FILE *fd);
//////////////////////////////////////////////////////
/*
Developed by Wojciech Borkowski based on :
** Demonstration of loading a PCX file and displaying it on screen
**
**  by Steven H Don
**
**
**
** This support type 5, 256 colour PCX files with
** For questions, feel free to e-mail me.
**    shd@earthling.net
**    http://shd.home.ml.org
**
****************************************************************************/
/* #define PLOT_DEBUG  - for detailed load trace on screen */

//#include <dos.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <assert.h>

#include "../gd.h"
#include "symshell.h"
#include "wb_bits.h"


extern "C" gdImagePtr gdImageCreateFromPcx(FILE *fd);

//OBSLUGA BLEDOW LADOWANIA
static jmp_buf Jamper; //Nie ma lokalnych obiektow do zwalnianie wiec mozna
static void raiseError( int Code,char * pszErr)
{
#ifndef SIMPLE_ERROR_HANDLING
	wbrtm::errh::Error(wbrtm::TextException(pszErr,0));//Warning only
#else
  cerr<<pszErr<<"["<<Code<<"] ";
#endif
  if(Code!=0)
  {
	cerr<<" SORRY!"<<endl;
	longjmp(Jamper,Code);//Wskakuje z czytania
  }
  else
	cerr<<" WARNING ONLY!"<<endl;
}

/*
static void VgaScreen ()
{
	//Initialisation. Nothing to do
}

static void TextScreen ()
{
	//Closing. Nothing to do.
}
*/
static void SetDAC (unsigned char DAC, unsigned char R, unsigned char G,unsigned char B)
{
	set_rgb(DAC,R,G,B);
}


/*PCX Header structure*/
#pragma pack(1)
 struct PCXHeader {
     char Manufacturer, Version, Encoding, BitsPerPixel;
     unsigned short int xMin, yMin, xMax, yMax;
     char Other[116];
 };

/*Required for decoding and palette*/
static PCXHeader		Header;
static unsigned char	DataByte;
static unsigned char	HowMany;
static unsigned char	Palette[256][3];

static void _Load8bitPCX(FILE *PCXFile,gdImagePtr Image)
{
   short int x, y;
   int		 xlen=0;
   short ColorExists[256]; //Indeksy poszczegolnych kolorow
   memset(ColorExists,0xff,sizeof(ColorExists)); //Musza byc zainicjowane
   assert(ColorExists[0]==-1); //Czy wlasciwa inicjacja

 /*Load in the palette*/
   fseek (PCXFile, -769, SEEK_END);

   /*Read in identifier*/
   fread (&DataByte, 1, 1, PCXFile);
   if (DataByte!=12) {
     /*If there is no palette, don't display*/
	   raiseError(0,"Palette not present!");
   }
	else fread (&Palette, 768, 1, PCXFile);//Tu czytamy palete, ale ustawiamy na koncu jak wiadomo co jest uzyte!

 /*Go back to start of graphic data*/
   fseek (PCXFile, 128, SEEK_SET);
   y=Header.yMin;
   x=Header.xMin;
   //Header.xMax++;
   //Header.yMax++;
   xlen=Header.xMax-Header.xMin+1;
   if(xlen%2!=0) //Nieparzysta dlugosc lini
	   xlen++;	 //Pamietana jako parzysta, dodatkowa kolumna "niewidoczna"

   /*Decode and display graphics*/
   while (y<=Header.yMax) {
	   int color=0;
	   /*Read next byte*/
	   fread (&DataByte, 1, 1, PCXFile);

	   /*Reset counter*/
	   HowMany=1;

	   /*If it is encoded, extract the count information and read in
	   the colour byte*/
	   if ((DataByte & 0xC0)==0xC0) {
		   HowMany = (DataByte & 0x3F);
		   fread (&DataByte, 1, 1, PCXFile);
	   }

	   //Transformacja indeksu koloru
	   assert(int(DataByte)<=255);
	   if(ColorExists[DataByte]==-1) //Jeszcze nie bylo
	   {
		   color=gdImageColorAllocate(Image,Palette[DataByte][0], Palette[DataByte][1], Palette[DataByte][2]);//Alokacja nowego koloru
		   ColorExists[DataByte]=color;
		   if(color==-1)
			   raiseError(3,"To many colors during load PCX.");
#ifndef NDEBUG
		   cerr<<"PCX color \t"<<int(DataByte)<<"("
			   <<int(Palette[DataByte][0])<<','
			   <<int(Palette[DataByte][1])<<','
			   <<int(Palette[DataByte][2])<<") using index "<<color<<endl;
#endif
	   }
	   else
		   color=ColorExists[DataByte];
	   assert(0<=color && color<256);

	   /*Display it*/
	   for (int rep=1; rep<=HowMany; rep++) {

		   //plot(x,y,color);
		   gdImageSetPixel(Image, x, y, color);

		   x++;
		   /*If End of Line reached, next line*/
		   if (x==Header.xMin+xlen)
		   {
			   y++;
			   x=Header.xMin;
		   }
	   }
   }
}

#ifdef PLOT_DEBUG
#define PLOT( _P1 , _P2 , _P3 )  plot((_P1),(_P2),(_P3))
#else
#define PLOT( _P1 , _P2 , _P3 )
#endif

static void _Load1bitPCX(FILE *PCXFile,gdImagePtr Image)
{
   short int x, y;
   int		 xlen=0;
   int Black=gdImageColorAllocate(Image,0,0,0);//Alokacja czarnego koloru
   int White=gdImageColorAllocate(Image,255,255,255);//Alokacja bialego koloru
   SetDAC(Black,0,0,0);
   SetDAC(White,255,0,255);

   /*Go back to start of graphic data*/
   fseek (PCXFile, 128, SEEK_SET);
   y=Header.yMin;
   x=Header.xMin;
   //Header.xMax++;
   //Header.yMax++;
   xlen=Header.xMax-Header.xMin+1;

   if(xlen%16!=0) //"Nieodpowiednia" dlugosc lini
	   xlen=(xlen/16+1)*16;	//Dodatkowe kolumny "niewidoczne"
   assert(xlen%16==0);
/*
   if(xlen%8!=0) //"Nieodpowiednia" dlugosc lini
	   xlen=(xlen/8+1)*8;	//Dodatkowe kolumny "niewidoczne"

	if(xlen%2!=0) //Nieparzysta dlugosc lini
	   xlen++;	 //Pamietana jako parzysta, dodatkowa kolumna "niewidoczna"
*/
   /*Decode and display graphics*/
   while (y<=Header.yMax) {
	   /*Read next byte*/
	   if(fread (&DataByte, 1, 1, PCXFile)==0)
	   {
		   raiseError(0,"Unexpected end of data in file");
		   break;
	   }

	   /*Reset counter*/
	   HowMany=1;


	   //If it is encoded, extract the count information and read in the colour byte
	   if ((DataByte & 0xC0)==0xC0) {
		   HowMany = (DataByte & 0x3F);
		   //cerr<<int(HowMany)<<' ';
		   fread (&DataByte, 1, 1, PCXFile);
	   }


	   //Display it - mem it
	   unsigned char color[8];
	   byte2bits(DataByte,color);

	   for (int rep=1; rep<=HowMany; rep++)
	   {

		   PLOT(x,y,color[7]);
		   gdImageSetPixel(Image, x++, y, color[7]?White:Black);
		   if(x>=Header.xMin+xlen)
		   {	y++; x=Header.xMin;break;}//If End of Line reached, next line

		   PLOT(x,y,color[6]);
		   gdImageSetPixel(Image, x++, y, color[6]?White:Black);
		   if(x>=Header.xMin+xlen)
		   {	y++; x=Header.xMin;break;}//If End of Line reached, next line

		   PLOT(x,y,color[5]);
		   gdImageSetPixel(Image, x++, y, color[5]?White:Black);
		   if(x>=Header.xMin+xlen)
		   {	y++; x=Header.xMin;break;}//If End of Line reached, next line

		   PLOT(x,y,color[4]);
		   gdImageSetPixel(Image, x++, y, color[4]?White:Black);
		   if(x>=Header.xMin+xlen)
		   {	y++; x=Header.xMin;break;}//If End of Line reached, next line

		   PLOT(x,y,color[3]);
		   gdImageSetPixel(Image, x++, y, color[3]?White:Black);
		   if(x>=Header.xMin+xlen)
		   {	y++; x=Header.xMin;break;}//If End of Line reached, next line

		   PLOT(x,y,color[2]);
		   gdImageSetPixel(Image, x++, y, color[2]?White:Black);
		   if(x>=Header.xMin+xlen)
		   {	y++; x=Header.xMin;break;}//If End of Line reached, next line

		   PLOT(x,y,color[1]);
		   gdImageSetPixel(Image, x++, y, color[1]?White:Black);
		   if(x>=Header.xMin+xlen)
		   {	y++; x=Header.xMin;break;}//If End of Line reached, next line

		   PLOT(x,y,color[0]);
		   gdImageSetPixel(Image, x++, y, color[0]?White:Black);
		   if(x>=Header.xMin+xlen)
		   {	y++; x=Header.xMin;break;}//If End of Line reached, next line

	   }
   }
}


gdImagePtr gdImageCreateFromPcx(FILE *PCXFile)
//This loads in the actual PCX-file displaing it for debugging purpose.
{
   gdImagePtr Image=NULL; //Image handle
#if 0 //ndef NDEBUG
   cerr<<endl;
#endif
   if(setjmp(Jamper)!=0){ //Control point for read failure
		cerr<<"Ups!"<<endl;
		goto FAILED;
	}

   /*Read in header information*/
   fread (&Header, 128, 1, PCXFile);
   /*Check to see whether we can display it*/
   if (Header.Version != 5) {
     /*If other version than 5 don't LOAD*/
	 raiseError(0,"Can't load PCX other then version 5");
   }
/*
   if(Header.xMin!=0 || Header.yMin!=0)
			raiseError(0,"Can't load because xMin or yMin is non 0.");
*/
	Image=gdImageCreate(Header.xMax-Header.xMin+1,Header.yMax-Header.yMin+1);

	if(Header.Encoding==1 && Header.BitsPerPixel==8)
		_Load8bitPCX(PCXFile,Image);
	else
	if(Header.Encoding==1 && Header.BitsPerPixel==1)
		_Load1bitPCX(PCXFile,Image);
	else
	{
		cerr<<"Manufacturer:"<<Header.Manufacturer<<" Version:"<<Header.Version<<endl;
		cerr<<"Encoding:"<<Header.Encoding<<" BitsPerPixel:"<<Header.BitsPerPixel<<endl;
		raiseError(1,"Unsupported PCX version");
	}

   //SUCCESS!
	return Image;

FAILED:
   if(Image){
	gdImageDestroy(Image);
	}
   return NULL;
}


/********************************************************************/
/*			    Adopted to WBRTM  version 2006                      */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
