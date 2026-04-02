/* ***************************************************************************************/
// Implementacja odczytywania  BITMAP uzupelniajaca biblioteke GD na bazie  kodu bibl.PIC
/* ***************************************************************************************/


#include <cstdio>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <csetjmp>
#include <cerrno>

#include <malloc.h> //!!!

#include <iostream>

//#include "INCLUDE/platform.hpp"
#include "../gd.h" //Adopted library  GD

#ifndef SIMPLE_ERROR_HANDLING //Hide exceptions or pseudoexceptions
#include "errorhan.hpp"
#include "excpbase.hpp"
#endif

/* THIS CODE IS BASED ON BMPDECODER FROM PICT LIBRARY
/--------------------------------------------------------------------
|
|      BMPDEC.CPP            Windows Bitmap Decoder Class
|
|      Windows bitmap file decoder. Decodes 1, 4, 8, and 24 bpp
|      bitmap files (compressed and uncompressed) and returns a 32
|      bpp DIB.
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/// Copyright (c) 1996-1998 Ulrich von Zadow

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef long LONG;
typedef int  BOOL;
const BOOL FALSE=0;
const BOOL TRUE=1;

typedef struct tagWINRGBQUAD
{
  BYTE    rgbBlue;
  BYTE    rgbGreen;
  BYTE    rgbRed;
  BYTE    rgbReserved;
} WINRGBQUAD;

typedef struct tagWINBITMAPFILEHEADER
{
  WORD    bfType;
  LONG    bfSize;
  WORD    bfReserved1;
  WORD    bfReserved2;
  LONG    bfOffBits;
} WINBITMAPFILEHEADER;

typedef struct tagWINBITMAPINFOHEADER
{
  LONG   biSize;
  LONG   biWidth;
  LONG   biHeight;
  WORD   biPlanes;
  WORD   biBitCount;
  LONG   biCompression;
  LONG   biSizeImage;
  LONG   biXPelsPerMeter;
  LONG   biYPelsPerMeter;
  LONG   biClrUsed;
  LONG   biClrImportant;
} WINBITMAPINFOHEADER;

typedef struct tagWINBITMAPINFO
{
  WINBITMAPINFOHEADER bmiHeader;
  WINRGBQUAD          bmiColors[1];
} WINBITMAPINFO;

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define TRACE( _P_ )	trace(1,_P_)
#define ASSERT		assert

// Error codes
#define ERR_OK              0            // No error
#define ERR_WRONG_SIGNATURE 1            // Expected file signature
                                         // not found.
#define ERR_FORMAT_UNKNOWN  2            // Sanity check failed.

#define ERR_PATH_NOT_FOUND  3            // <--
#define ERR_FILE_NOT_FOUND  4            // <-- Problems with the file
#define ERR_ACCESS_DENIED   5            // <-- system.

#define ERR_FORMAT_NOT_SUPPORTED 6       // Known but unsupported
                                         // format.
#define ERR_INTERNAL        7            // Internal error of some sort.
#define ERR_UNKNOWN_FILE_TYPE    8       // Couldn't recognize the
                                         // file type.
#define ERR_DIB_TOO_LARGE   9            // Maximum size for 1 bmp was
                                         // exceeded.
#define ERR_NO_MEMORY      10            // Out of memory.

// Define the pixel format for RGB-bitmaps. Change this if you change
// the order of the color components. At the moment, PIXEL_SIZE must
// be 4. The decoders depend on it.
#define PIXEL_SIZE  4
typedef LONG RGBAPIXEL;

#define RGBA_BLUE   0
#define RGBA_GREEN  1
#define RGBA_RED    2
#define RGBA_ALPHA  3

const int m_DestBPP=8;

//! Windows bitmap file decoder. Decodes 1, 4, 8
//! bitmap files (compressed and uncompressed). Returns an 8 bpp bitmap.
class MyBmp256Decoder
{
gdImagePtr im;
FILE*      finput;
RGBAPIXEL   * m_pPal;  //!< Pointer to palette used in file.
static jmp_buf Jamper; //!< Nie ma lokalnych obiektów do zwalniania wiec można skoczyć.

void raiseError( int Code,const char * pszErr)
  {
#ifndef SIMPLE_ERROR_HANDLING
    wbrtm::error_handling::Error(wbrtm::TextException(pszErr,0));//Warning only
#else
  cerr<<"SORRY:"<<pszErr<<"["<<Code<<"]"<<endl;
#endif
  longjmp(Jamper,Code); //Wskakuje do DoDecode - jesli nie ma mechanizmu exceptions
  }

public:
  //! Creates a decoder
  MyBmp256Decoder
    (FILE* finp)
  {
  m_pPal=NULL;//CO Z ALOKACJA!
  im=NULL;
  ASSERT(finp);
  finput=finp;
  }

  //! Destroys a decoder
  ~MyBmp256Decoder
    ()
  {
  if(im)
    gdImageDestroy(im);
  if(m_pPal!=NULL)
    delete m_pPal;
  }

  gdImagePtr give()
  {
  gdImagePtr pom=im;
  im=NULL;
  return pom;
  }

  //! Does the actual decoding of bitmap data.
  int DoDecode
    ();

protected:
//Read important information about current Image
int GetWidth
    ()
{
  assert(im!=NULL);

  return gdImageSY(im);//Na abarot bo tak ma bitmapa

}


int GetHeight
    ()
{
  assert(im!=NULL);

  return gdImageSX(im);

}


int GetNumColors
    ()
{
  assert(im!=NULL);

  return 256;
}

BYTE ** GetLineArray
  ()
{
  assert(im!=NULL);

  return im->pixels;
}

  // routines for reading file

  BYTE * ReadNBytes
    ( size_t n
    );

  BYTE * Read1Byte
    ();

  BYTE * Read2Bytes
    ();

  BYTE * Read4Bytes
    ();

  void Skip
    ( int n
    );

  BYTE ReadByte
    ();

  // Machine-independent routines for byte-order conversion.

  WORD ReadIWord
    ();

  WORD ReadMWord
    ();

  LONG ReadILong
    ();

  LONG ReadMLong
    ();

  static void trace
    ( int TraceLevel,
      const char * pszMessage
    );

private:

  // Decodes the bitmap file & info headers
  WINBITMAPINFOHEADER * getInfoHeader
    ( BYTE ** ppBits
    );

  // Decodes a 2-color bitmap. Ignores the palette & just uses
  // black & white as 'colors'
  void decode1bpp( BYTE* ppBits );

  // Decodes an uncompressed 16-color-bitmap.
  void decode4bpp( BYTE* ppBits );

  // Decodes an uncompressed 256-color-bitmap.
  void decode8bpp( BYTE* ppBits );

  // Decodes a compressed 16-color-bitmap.
  void decodeRLE4( BYTE* ppBits );

  // Decodes a compressed 256-color-bitmap.
  void decodeRLE8( BYTE* ppBits );

  // Decodes two 4-bit pixels using a palette & puts them in pDest
  // and pDest+4.

  void decode2Nibbles
    ( BYTE * pDest,
      BYTE SrcByte
    );

  // Makes a private copy of the palette in the file and sets the
  // alpha channel.
  void readPalette
    ( WINBITMAPINFOHEADER * pBMI  // Pointer to bitmapinfoheader in file.
    );

  void SetPalette
    ( RGBAPIXEL * pPal,
    int len
    );

};

inline BYTE * MyBmp256Decoder::ReadNBytes
    ( size_t n
    )
{
  BYTE* bufor=new BYTE[n];
  size_t ret=fread(bufor,sizeof(BYTE),n,finput);
//	if(ret!=n)
//		errh::Error(TextException("decoding bmp:File shorter than header declare",0));

//  m_pData += n;
//  return m_pData-n;
  return bufor;
}


inline BYTE * MyBmp256Decoder::Read1Byte
    ()
{
  static BYTE bufor;
  bufor=getc(finput);
//  m_pData++;
//  return m_pData-1;
  return &bufor;
}


inline BYTE * MyBmp256Decoder::Read2Bytes
    ()
{
  static BYTE bufor[2];
  bufor[0]=getc(finput);
  bufor[1]=getc(finput);
//  m_pData += 2;
//  return m_pData-2;
  return bufor;
}


inline BYTE * MyBmp256Decoder::Read4Bytes
    ()
{
  static BYTE bufor[4];
  fread(bufor,sizeof(BYTE),4,finput);
//  m_pData += 4;
//  return m_pData-4;
  return bufor;
}


inline void MyBmp256Decoder::Skip
    ( int n
    )
{
  //m_pData += n;
  fseek(finput,n,SEEK_CUR);
}


inline BYTE MyBmp256Decoder::ReadByte
    ()
{
  return *(Read1Byte());
}


inline WORD MyBmp256Decoder::ReadIWord
    ()
{
  BYTE * pData = Read2Bytes ();
  // This should work regardless of the destination byte order ;-)
  return pData[0] + (pData[1]<<8);
}


inline WORD MyBmp256Decoder::ReadMWord
    ()
{
  BYTE * pData = Read2Bytes ();
  return pData[1] + (pData[0]<<8);
}


inline LONG MyBmp256Decoder::ReadILong
    ()
{
  BYTE * pData = Read4Bytes ();
  return pData[0] + (pData[1]<<8) + (pData[2]<<16) + (pData[3]<<24);
}

inline LONG MyBmp256Decoder::ReadMLong
    ()
{
  BYTE * pData = Read4Bytes ();
  return pData[3] + (pData[2]<<8) + (pData[1]<<16) + (pData[0]<<24);
}

void MyBmp256Decoder::trace
  ( int //TraceLevel,
    ,const char * //pszMessage
  )
{                                                                               assert("Not implemented"==nullptr);
//  clog<<"LEV:"<<TraceLevel<<":"<<pszMessage<<"\n"<<flush;
}

void MyBmp256Decoder::SetPalette
  ( RGBAPIXEL * pPal
    ,int NumColors)
{																				assert( pPal	);
                                        assert(im!=NULL);
  //memcpy (m_pClrTab, pPal, 256*sizeof(RGBAPIXEL));    TODO ?
  //Skopiowac palete z naglowka na palete w im
  //im->red[] ... itd
  if(NumColors==2)
  {
  im->blue[0]=im->green[0]=im->red[0]=0;
  im->blue[1]=im->green[1]=im->red[1]=255;
  im->open[0] = im->open[1] = 0;
  }
  else
  for (int i=0; i<NumColors; i++)
  {
  im->blue[i]=*(((BYTE *)m_pPal)+i*4+RGBA_BLUE);
    im->green[i]=*(((BYTE *)m_pPal)+i*4+RGBA_GREEN);
    im->red[i]=*(((BYTE *)m_pPal)+i*4+RGBA_RED);
  im->open[i] =  0;
  }
  im->colorsTotal=NumColors;
}

int MyBmp256Decoder::DoDecode
  ()
{
  WINBITMAPINFOHEADER * pBMI;  // Pointer to bitmapinfoheader of the file.
  BYTE * pBits = NULL;
  BYTE ret = 1;

  pBMI = getInfoHeader (&pBits);

  if(pBMI==NULL)
  { ret=0; goto FAILED;}

  im=gdImageCreate(pBMI->biHeight,pBMI->biWidth);
  if(im==NULL)
  { ret=0; goto FAILED;}

  //Ustawianie palety
  int NumColors;
  if (pBMI->biClrUsed == 0)
  NumColors = 1<<(pBMI->biBitCount);
  else
    NumColors = pBMI->biClrUsed;
  if(m_pPal!=NULL) //Jesli jest paleta -np BMP true-color nie maja
  SetPalette (m_pPal,NumColors);


  if(setjmp(Jamper)!=0)
  { ret=0; goto FAILED;}

  switch (pBMI->biBitCount)
  {
    case 1:
      decode1bpp (pBits );
      break;
    case 4:
      if (pBMI->biCompression == BI_RGB)
        decode4bpp (pBits  );
       else
        decodeRLE4 (pBits  );
      break;
    case 8:
      if (pBMI->biCompression == BI_RGB)
        decode8bpp (pBits  );
       else
        decodeRLE8 (pBits  );
      break;
    default:
      // This is not a standard bmp file.
      raiseError (ERR_FORMAT_UNKNOWN,
                  "Decoding bmp: Illegal bpp value.");
  }

goto END;//Jest OK

FAILED:
 if(im)
  {
  gdImageDestroy(im);
  im=NULL;
  }
END:
  delete pBMI;
  delete pBits;
  trace (3, "Decoding finished.\n");
  return ret;
}

WINBITMAPINFOHEADER * MyBmp256Decoder::getInfoHeader
    ( BYTE ** ppBits     // Used to return location of bitmap bits.
    )
    // Decodes the bitmap file & info headers
{
  WINBITMAPFILEHEADER BFH;

  BFH.bfType = ReadIWord ();
  BFH.bfSize = ReadILong ();
  BFH.bfReserved1 = ReadIWord ();
  BFH.bfReserved2 = ReadIWord ();
  BFH.bfOffBits = ReadILong ();

  // Check for bitmap file signature: First 2 bytes are 'BM'
  if (BFH.bfType != 0x4d42)
  raiseError (ERR_WRONG_SIGNATURE,
        "Bitmap decoder: This isn't a bitmap.");

  trace (2, "Bitmap file signature found\n");

  WINBITMAPINFOHEADER *pBMI = new WINBITMAPINFOHEADER;

  pBMI->biSize = ReadILong ();
  pBMI->biWidth = ReadILong ();
  pBMI->biHeight = ReadILong ();
  pBMI->biPlanes = ReadIWord ();
  pBMI->biBitCount = ReadIWord ();
  pBMI->biCompression = ReadILong ();
  pBMI->biSizeImage = ReadILong ();
  pBMI->biXPelsPerMeter = ReadILong ();
  pBMI->biYPelsPerMeter = ReadILong ();
  pBMI->biClrUsed = ReadILong ();
  pBMI->biClrImportant = ReadILong ();

  // Do sanity check
  if (pBMI->biSize != sizeof (WINBITMAPINFOHEADER))
    raiseError (ERR_FORMAT_UNKNOWN,
                "Bitmap decoder: BITMAPINFOHEADER has wrong size.");

  trace (2, "Bitmap header is ok.\n");

  // Read palette if 8 bpp or less.
  if (pBMI->biBitCount <= 8)
    readPalette (pBMI);
 //Czytanie samych bitow - troche bez sensu - trzeba prawie dwa razy tyle pamieci
  *ppBits = ReadNBytes
                 (BFH.bfSize-
                  sizeof (WINBITMAPFILEHEADER)-
                  sizeof (WINBITMAPINFOHEADER));

  return pBMI;
}

void MyBmp256Decoder::decode1bpp
    ( BYTE* pBits
    )
    // Decodes a 2-color bitmap. Ignores the palette & just uses
    // black & white as 'colors' if decoding to 32 bit
{
  int i;
  int y;                           // Current row
  int x;                           // Current column

  BYTE * pSrcLine = pBits;         // Start of current row in file.
  BYTE * pDest;                    // Current destination.
  BYTE * pSrc;                     // Current position in file.
  BYTE   BTable[8];                // Table of bit masks.
  BYTE   SrcByte;                  // Source byte cache.
  int    XSize = GetWidth(); // Width of bitmap in pixels.
  int    LineLen = ((XSize+7)/8 + 3) & ~3;
                                   // Width of source in bytes
                                   //   (DWORD-aligned).
  BYTE ** pLineArray = GetLineArray();
                                   // Pointers to dest. lines.

  int    OpaqueBlack = 0x00000000;
  *(((BYTE*)&OpaqueBlack)+RGBA_ALPHA) = 0xFF;

  trace (2, "Decoding 1 bit per pixel bitmap.\n");

  // Initialize bit masks.
  for (i=0; i<8; i++)
  {
    BTable[i] = 1<<i;
  }

  for (y=0; y< GetHeight(); y++)
  { // For each line...
    pSrc = pSrcLine;
    pDest = pLineArray[y/*GetHeight()-y-1*/];
    for (x=0; x<XSize/8; x++)
    { // For each source byte...
      SrcByte = *(pSrc);
      for (i=7; i>=0; i--)
      { // For each bit...

          if (SrcByte & BTable[i]) // Test if bit i is set
            *pDest = 0x01;
           else
            *pDest = 0x00;
          pDest++;

      }
      pSrc++;
    }

    // Last few bits in line...
    SrcByte = *(pSrc);
    for (i=7; i>7-(XSize & 7); i--)
    { // For each bit...

        if (SrcByte & BTable[i]) // Test if bit i is set
          *pDest = 0x01;
         else
          *pDest = 0x00;
        pDest++;

    }
    pSrcLine += LineLen;
  }
}

void MyBmp256Decoder::decode4bpp
    (   BYTE* pBits
    )
    // Decodes an uncompressed 16-color-bitmap.
{
  int y;                            // Current row
  int x;                            // Current column

  BYTE * pSrcLine = pBits;          // Start of current row in file.
  BYTE * pDest;                     // Current destination.
  BYTE * pSrc;                      // Current position in file.
  BYTE   SrcByte;                   // Source byte cache.
  int    XSize = GetWidth();// Width of bitmap in pixels.
  int    LineLen = ((XSize+1)/2 + 3) & ~3;
                                    // Width of source in bytes
                                    //   (DWORD-aligned).
  BYTE ** pLineArray = GetLineArray();
                                   // Pointers to dest. lines.

  trace (2, "Decoding uncompressed 4 bit per pixel bitmap.\n");

  for (y=0; y<GetHeight(); y++)
  { // For each line...
    pSrc = pSrcLine;
    pDest = pLineArray[y/*GetHeight()-y-1*/];
    for (x=0; x<XSize/2; x++)
    { // For each source byte...
      SrcByte = *(pSrc);

        *pDest = SrcByte>>4;
        pDest++;
        *pDest = SrcByte & 15;
        pDest++;

      pSrc++;
   }

    // Last nibble in line if line length is odd.
    if (XSize & 1)
    {

        *pDest = (*(pSrc))>>4;
        pDest++;

    }

    pSrcLine += LineLen;
  }
}


void MyBmp256Decoder::decode8bpp
    (  BYTE* pBits
    )
    // Decodes an uncompressed 256-color-bitmap.
{
  int y;                            // Current row
  int x;                            // Current column

  BYTE * pSrcLine = pBits;          // Start of current row in file.
  BYTE * pDest;                     // Current destination.
  BYTE * pSrc;                      // Current position in file.
  int    XSize = GetWidth();// Width of bitmap in pixels.
  int    LineLen = (XSize + 3) & ~3;
                                    // Width of source in bytes
                                    //   (DWORD-aligned).
  BYTE ** pLineArray = GetLineArray();
                                   // Pointers to dest. lines.

  trace (2, "Decoding uncompressed 8 bit per pixel bitmap.\n");

  for (y=0; y< GetHeight(); y++)
  { // For each line...
    pSrc = pSrcLine;
    pDest = pLineArray[y/*GetHeight()-y-1*/];
    for (x=0; x<XSize; x++)
    { // For each source byte...

        *pDest = *pSrc;
        pDest++;

      pSrc++;
    }
    pSrcLine += LineLen;
  }
}


void MyBmp256Decoder::decodeRLE4
    (   BYTE* pBits
    )
    // Decodes a compressed 16-color-bitmap.
{
  int y;                              // Current row

  BYTE * pDest;                       // Current destination.
  BYTE * pSrc = pBits;                // Current position in file.
  int    XSize = GetWidth();  // Width of bitmap in pixels.
  BYTE   SrcByte;                     // Source byte cache.

  BYTE   RunLength;    // Length of current run.
  BOOL   bOdd;         // TRUE if current run has odd length.

  BOOL   bEOL;         // TRUE if end of line reached.
  BOOL   bEOF=FALSE;   // TRUE if end of file reached.

  BYTE * pLineBuf;     // Current line as uncompressed nibbles.
  BYTE * pBuf;         // Current position in pLineBuf.
  BYTE ** pLineArray = GetLineArray();
                                   // Pointers to dest. lines.

  trace (2, "Decoding RLE4-compressed bitmap.\n");

  // Allocate enough memory for DWORD alignment in original 4 bpp
  // bitmap.
  pLineBuf = new BYTE [XSize*4+28];

  for (y=0; y<GetHeight() && !bEOF; y++)
  { // For each line...
    pBuf = pLineBuf;
    bEOL=FALSE;
    while (!bEOL)
    { // For each packet do
      RunLength = *pSrc;
      pSrc++;
      if (RunLength==0)
      { // Literal or escape.
        RunLength = *pSrc;
        pSrc++;
        switch (RunLength)
        {
          case 0: // End of line escape
            bEOL = TRUE;
            break;
          case 1: // End of file escape
            bEOF = TRUE;
            bEOL = TRUE;
            break;
          case 2: // Delta escape.
            // I have never seen a file using this.
            delete pLineBuf;
            raiseError (ERR_FORMAT_NOT_SUPPORTED,
                        "Encountered delta escape.");
            break;
          default:
            // Literal packet
            bOdd = (RunLength & 1);
            RunLength /= 2; // Convert pixels to bytes.
            for (int i=0; i<RunLength; i++)
            { // For each source byte...
              decode2Nibbles (pBuf, *pSrc );
              pBuf += 2;
              pSrc++;
            }
            if (bOdd)
            {
                *pBuf = (*(pSrc))>>4;
                pBuf++;

              pSrc++;
            }
            // Word alignment at end of literal packet.
            if (RunLength & 1) pSrc++;
        }
      }
      else
      { // Encoded packet:
        // RunLength 4 bpp pixels with 2 alternating
        // values.
        SrcByte = *pSrc;
        pSrc++;
        for (int i=0; i<RunLength/2; i++)
        {
          decode2Nibbles (pBuf, SrcByte );
          pBuf += 2;
        }
        if (RunLength & 1)
        {
            *pBuf = (*(pSrc))>>4;
            pBuf++;
        }
      }
    }
    pDest = pLineArray[y/*GetHeight()-y-1*/];

    memcpy (pDest, pLineBuf, XSize);
  }
  delete pLineBuf;
}


void MyBmp256Decoder::decodeRLE8
    (  BYTE* pBits
    )
    // Decodes a compressed 256-color-bitmap.
{
  int y;                              // Current row

  BYTE * pDest;                       // Current destination.
  BYTE * pSrc = pBits;                // Current position in file.
//  int    XSize =GetWidth();  // Width of bitmap in pixels.

  BYTE   RunLength;                   // Length of current run.

  BOOL   bEOL;                        // TRUE if end of line reached.
  BOOL   bEOF=FALSE;                  // TRUE if end of file reached.
  BYTE ** pLineArray = GetLineArray();
                                      // Pointers to dest. lines.

  trace (2, "Decoding RLE8-compressed bitmap.\n");

  for (y=0; y<GetHeight() && !bEOF; y++)
  { // For each line...
    pDest = pLineArray[y/*GetHeight()-y-1*/];
    bEOL=FALSE;
    while (!bEOL)
    { // For each packet do
      RunLength = *pSrc;
      pSrc++;
      if (RunLength==0)
      { // Literal or escape.
        RunLength = *pSrc;
        pSrc++;
        switch (RunLength)
        {
          case 0: // End of line escape
            bEOL = TRUE;
            break;
          case 1: // End of file escape
            bEOF = TRUE;
            bEOL = TRUE;
            break;
          case 2: // Delta escape.
            // I have never seen a file using this...
            raiseError (ERR_FORMAT_NOT_SUPPORTED,
                        "Encountered delta escape.");
            bEOL = TRUE;
            bEOF = TRUE;
            break;
          default:
            // Literal packet

              memcpy (pDest, pSrc, RunLength);
              pDest += RunLength;
              pSrc += RunLength;

            // Word alignment at end of literal packet.
            if (RunLength & 1) pSrc++;
        }
      }
      else
      { // Encoded packet:
        // RunLength pixels, all with the same value

          memset (pDest, *pSrc, RunLength);
          pSrc++;
          pDest += RunLength;

      }
    }
  }
}


void MyBmp256Decoder::decode2Nibbles
    ( BYTE * pDest,
      BYTE SrcByte
    )
    // Decodes two 4-bit pixels.
{

    *pDest = SrcByte>>4;
    *(pDest+1) = SrcByte & 15;

}

void MyBmp256Decoder::readPalette
    ( WINBITMAPINFOHEADER * pBMI     // Pointer to bitmapinfoheader in file.
    )
    // Assumes 8 bpp or less.
{
  trace (3, "Reading palette.\n");
  int i;

  int NumColors;
  if (pBMI->biClrUsed == 0)
    NumColors = 1<<(pBMI->biBitCount);
   else
    NumColors = pBMI->biClrUsed;

  WINRGBQUAD * pPal = (WINRGBQUAD *) ReadNBytes
                                    (NumColors*sizeof (WINRGBQUAD));
  m_pPal = new RGBAPIXEL  [256];

  // Correct the byte ordering & copy the data.
  for (i=0; i<NumColors; i++)
  {
    *(((BYTE *)m_pPal)+i*4+RGBA_BLUE) = pPal[i].rgbBlue;
    *(((BYTE *)m_pPal)+i*4+RGBA_GREEN) = pPal[i].rgbGreen;
    *(((BYTE *)m_pPal)+i*4+RGBA_RED) = pPal[i].rgbRed;
    *(((BYTE *)m_pPal)+i*4+RGBA_ALPHA) = 0xFF;
  }
}


extern "C"
gdImagePtr gdImageCreateFromBmp(FILE *f)//Implementacja
{
MyBmp256Decoder Work(f);
Work.DoDecode();
return Work.give();
}


jmp_buf  MyBmp256Decoder::Jamper;

/* ****************************************************************** */
/*			    Adopted to WBRTM  version 2006                        */
/* ****************************************************************** */
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                   */
/*            W O J C I E C H   B O R K O W S K I                     */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego         */
/*        WWW:  http://wwww.iss.uw.edu.pl/~borkowsk/                  */
/*                                                                    */
/*                               (Don't change or remove this note)   */
/* ****************************************************************** */



