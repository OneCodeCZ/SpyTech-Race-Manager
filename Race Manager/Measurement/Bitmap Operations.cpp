/******************
*                 *  Bitmap Operations.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of World Spy Library source code.
*     ^^     ^^   *  This file is property of SpyTech Company but is for
*                 *  anybody's free use.
*******************                                                          */

#include "stdafx.h"
#include "main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// www.codeguru.com rulez!!!

/****************************************************************************/

// LoadDIBSectionFromFile	- Creates a DIB section from BMP file
// lpszFileName			- Name of the BMP file
// ppvBits			- to receive address of bitmap bits
// hSection		- optional handle to a file mapping object
// dwOffset		- offset to the bitmap bit values within hSection
HBITMAP LoadDIBSectionFromFile( LPCTSTR lpszFileName, LPVOID *ppvBits, 
					HANDLE hSection, DWORD dwOffset, int *nSizeX, int *nSizeY, int *nBitDepth) 
{
	LPVOID lpBits;
	CFile file;
	if( !file.Open( lpszFileName, CFile::modeRead) )
		return NULL;
	
	BITMAPFILEHEADER bmfHeader;
	long nFileLen;
	
	nFileLen = file.GetLength();
	
	// Read file header
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return NULL;
	
	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
		return NULL;

	BITMAPINFO *pbmInfo;
	pbmInfo=(BITMAPINFO*)malloc(sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256);
	if (pbmInfo == NULL)	return NULL;
	
	// Read the BITMAPINFO
	file.Read( pbmInfo, sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256 );

	if (nSizeX) *nSizeX=pbmInfo->bmiHeader.biWidth;
	if (nSizeY) *nSizeY=pbmInfo->bmiHeader.biHeight;
	if (nBitDepth) *nBitDepth=pbmInfo->bmiHeader.biBitCount;
	
	BITMAPINFO &bmInfo = *pbmInfo;
	
	HBITMAP hBmp = CreateDIBSection( NULL, pbmInfo, DIB_RGB_COLORS, &lpBits, 
						hSection, dwOffset );

	LPBYTE  lpDIBBits;              // Pointer to DIB bits
	int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
				1 << bmInfo.bmiHeader.biBitCount;
	
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPBYTE)((LPDWORD)(bmInfo.bmiColors + 
			bmInfo.bmiHeader.biClrUsed) + 
			((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPBYTE)(bmInfo.bmiColors + nColors);

	int nOffset = sizeof(BITMAPFILEHEADER) + (lpDIBBits - (LPBYTE)pbmInfo);
	file.Seek( nOffset, CFile::begin);
	file.ReadHuge((LPSTR)lpBits, nFileLen - nOffset); //bmInfo.biSizeImage ); 

	free(pbmInfo);

	if( ppvBits )
		*ppvBits = lpBits;
	
	return hBmp;
}

/****************************************************************************/

// DrawDIBSection	- Draws a DIB section onto a device
// hDC		- Handle to a device context
// hBitmap	- Handle of the DIB Section
// xDest	- x-coordinate of the upper-left corner of the destination rect
// yDest	- y-coordinate of the upper-left corner of the destination rect
void DrawDIBSection( HDC hDC, HBITMAP hBitmap, int xDest, int yDest )
{
	HPALETTE hPal;

	HDC hDCMem = ::CreateCompatibleDC( hDC );

	// Create a logical palette for the bitmap
	DIBSECTION ds;
	BITMAPINFOHEADER &bmInfo = ds.dsBmih;
	if( ::GetObject(hBitmap, sizeof(ds), &ds ) == 0 )
		return;			// Not a DIB Section
	
	HGDIOBJ hBmpOld =  ::SelectObject(hDCMem, hBitmap);

	int nColors = bmInfo.biClrUsed ? bmInfo.biClrUsed : 1 << ds.dsBm.bmBitsPixel;
	
	if( ::GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE )
	{
		// Create a halftone palette if colors > 256. 
		if( nColors > 256 )
			hPal = ::CreateHalftonePalette(hDC);
		else
		{
			// Create the palette
			RGBQUAD *pRGB = new RGBQUAD[nColors];
			
			::GetDIBColorTable( hDCMem, 0, nColors, pRGB );
			
			UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
			LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
			
			pLP->palVersion = 0x300;
			pLP->palNumEntries = nColors;
			
			for( int i=0; i < nColors; i++)
			{
				pLP->palPalEntry[i].peRed = pRGB[i].rgbRed;
				pLP->palPalEntry[i].peGreen = pRGB[i].rgbGreen;
				pLP->palPalEntry[i].peBlue = pRGB[i].rgbBlue;
				pLP->palPalEntry[i].peFlags = 0;
			}
			
			hPal = ::CreatePalette( pLP );
			
			delete[] pLP;
			delete[] pRGB;
		}

		HPALETTE hPalOld = ::SelectPalette(hDC,hPal,FALSE);
		::RealizePalette(hDC);
		BitBlt(hDC,xDest,yDest,bmInfo.biWidth,bmInfo.biHeight,hDCMem,0,0,SRCCOPY);

		::SelectPalette(hDC,hPalOld,FALSE);
		// delete GDI objects
		::DeleteObject(hPal);
	}
	else
		BitBlt(hDC,xDest,yDest,bmInfo.biWidth,bmInfo.biHeight,hDCMem,0,0,SRCCOPY);

	::SelectObject(hDCMem, hBmpOld);
	::DeleteDC(hDCMem);
}

/****************************************************************************/
// StretchDIBSection	- Draws a DIB section onto a device
// hDC		- Handle to a device context
// hBitmap	- Handle of the DIB Section
// xDest	- x-coordinate of the upper-left corner of the destination rect
// yDest	- y-coordinate of the upper-left corner of the destination rect
// xRes - destination number of pixels of the bitmap
// yRes - destination number of pixels of the bitmap
void StretchDIBSection( HDC hDC, HBITMAP hBitmap, int xDest, int yDest, int xRes, int yRes )
{
	HPALETTE hPal;

	HDC hDCMem = ::CreateCompatibleDC( hDC );

	// Create a logical palette for the bitmap
	DIBSECTION ds;
	BITMAPINFOHEADER &bmInfo = ds.dsBmih;
	if( ::GetObject(hBitmap, sizeof(ds), &ds ) == 0 )
		return;			// Not a DIB Section
	
	HGDIOBJ hBmpOld =  ::SelectObject(hDCMem, hBitmap);

	int nColors = bmInfo.biClrUsed ? bmInfo.biClrUsed : 1 << ds.dsBm.bmBitsPixel;
	
	if( ::GetDeviceCaps(hDC, RASTERCAPS) & RC_PALETTE )
	{
		// Create a halftone palette if colors > 256. 
		if( nColors > 256 )
			hPal = ::CreateHalftonePalette(hDC);
		else
		{
			// Create the palette
			RGBQUAD *pRGB = new RGBQUAD[nColors];
			
			::GetDIBColorTable( hDCMem, 0, nColors, pRGB );
			
			UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
			LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];
			
			pLP->palVersion = 0x300;
			pLP->palNumEntries = nColors;
			
			for( int i=0; i < nColors; i++)
			{
				pLP->palPalEntry[i].peRed = pRGB[i].rgbRed;
				pLP->palPalEntry[i].peGreen = pRGB[i].rgbGreen;
				pLP->palPalEntry[i].peBlue = pRGB[i].rgbBlue;
				pLP->palPalEntry[i].peFlags = 0;
			}
			
			hPal = ::CreatePalette( pLP );
			
			delete[] pLP;
			delete[] pRGB;
		}

		HPALETTE hPalOld = ::SelectPalette(hDC,hPal,FALSE);
		::RealizePalette(hDC);
		BitBlt(hDC,xDest,yDest,bmInfo.biWidth,bmInfo.biHeight,hDCMem,0,0,SRCCOPY);

		::SelectPalette(hDC,hPalOld,FALSE);
		// delete GDI objects
		::DeleteObject(hPal);
	}
	else
		StretchBlt(hDC,xDest,yDest,xRes,yRes,hDCMem,0,0,bmInfo.biWidth,bmInfo.biHeight,SRCCOPY);

	::SelectObject(hDCMem, hBmpOld);
	::DeleteDC(hDCMem);
}

/****************************************************************************/

// LoadDIBSectionFromFile	- Creates a DIB section from BMP file
// lpszFileName			- Name of the BMP file
// ppvBits			- to receive address of bitmap bits
// hSection		- optional handle to a file mapping object
// dwOffset		- offset to the bitmap bit values within hSection
HBITMAP CreateDIBFromPrototype( LPCTSTR lpszFileName, LPVOID *ppvBits, 
					HANDLE hSection, DWORD dwOffset, int nSizeX, int nSizeY, int nBitDepth) 
{
	if (nBitDepth!=8) return NULL;	// unsupported type..

	LPVOID lpBits;
	CFile file;
	if( !file.Open( lpszFileName, CFile::modeRead) )
		return NULL;
	
	BITMAPFILEHEADER bmfHeader;
	long nFileLen;
	
	nFileLen = file.GetLength();
	
	// Read file header
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
		return NULL;
	
	// File type should be 'BM'
	if (bmfHeader.bfType != ((WORD) ('M' << 8) | 'B'))
		return NULL;

	BITMAPINFO *pbmInfo;
	pbmInfo=(BITMAPINFO*)malloc(sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256);
	if (pbmInfo == NULL)	return NULL;
	
	// Read the BITMAPINFO
	file.Read( pbmInfo, sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256 );

	pbmInfo->bmiHeader.biWidth=nSizeX;
	pbmInfo->bmiHeader.biHeight=nSizeY;

//	if (nSizeX) *nSizeX=pbmInfo->bmiHeader.biWidth;
//	if (nSizeY) *nSizeY=pbmInfo->bmiHeader.biHeight;
//	if (nBitDepth) *nBitDepth=pbmInfo->bmiHeader.biBitCount;
	
	BITMAPINFO &bmInfo = *pbmInfo;
	
	HBITMAP hBmp = CreateDIBSection( NULL, pbmInfo, DIB_RGB_COLORS, &lpBits, 
						hSection, dwOffset );

//	LPBYTE  lpDIBBits;              // Pointer to DIB bits
	int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
				1 << bmInfo.bmiHeader.biBitCount;
	
//	if( bmInfo.bmiHeader.biBitCount > 8 )
//		lpDIBBits = (LPBYTE)((LPDWORD)(bmInfo.bmiColors + 
//			bmInfo.bmiHeader.biClrUsed) + 
//			((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
//	else
//		lpDIBBits = (LPBYTE)(bmInfo.bmiColors + nColors);
//
//	int nOffset = sizeof(BITMAPFILEHEADER) + (lpDIBBits - (LPBYTE)pbmInfo);
//	file.Seek( nOffset, CFile::begin);
//	file.ReadHuge((LPSTR)lpBits, nFileLen - nOffset); //bmInfo.biSizeImage ); 
//
//	::GlobalFree(pbmInfo);
//
	if( ppvBits )
		*ppvBits = lpBits;
	
	return hBmp;
}

/****************************************************************************/
  
// WriteDIB		- Writes a DIB to file
// Returns		- TRUE on success
// szFile		- Name of file to write to
// hDIB			- Handle of the DIB
BOOL WriteDIB( LPTSTR szFile, HBITMAP hBitmap)
{
	CBitmap bmp;
	bmp.Attach(hBitmap);

	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;

	if (!hBitmap)
		return FALSE;

	CFile file;
	if( !file.Open( szFile, CFile::modeWrite|CFile::modeCreate) )
		return FALSE;

	lpbi = (LPBITMAPINFOHEADER)hBitmap;

	int nColors = 1 << lpbi->biBitCount;

	// Fill in the fields of the file header 
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= GlobalSize (hBitmap) + sizeof( hdr );
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
						nColors * sizeof(RGBQUAD));

	// Write the file header 
	file.Write( &hdr, sizeof(hdr) );

	// Write the DIB header and the bits 
	file.Write( lpbi, GlobalSize(hBitmap) );

	return TRUE;
}

/****************************************************************************/
  
PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp)
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap's color format, width, and height. 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
        AfxMessageBox("Buga");

    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 

     if (cClrBits != 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * (1<< cClrBits)); 

     // There is no RGBQUAD array for the 24-bit-per-pixel format. 

     else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER)); 

    // Initialize the fields in the BITMAPINFO structure. 

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    pbmi->bmiHeader.biSizeImage = (pbmi->bmiHeader.biWidth + 7) /8 
                                  * pbmi->bmiHeader.biHeight* cClrBits; 
// Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
     pbmi->bmiHeader.biClrImportant = 0; 
     return pbmi; 
 } 

/****************************************************************************/
  
void CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi, 
                  HBITMAP hBMP, HDC hDC) 
 { 
     HANDLE hf;                 // file handle 
    BITMAPFILEHEADER hdr;       // bitmap file-header 
    PBITMAPINFOHEADER pbih;     // bitmap info-header 
    LPBYTE lpBits;              // memory pointer 
    DWORD dwTotal;              // total count of bytes 
    DWORD cb;                   // incremental count of bytes 
    BYTE *hp;                   // byte pointer 
    DWORD dwTmp; 

    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits) 
		AfxMessageBox("GlobalAlloc"); 

    // Retrieve the color table (RGBQUAD array) and the bits 
    // (array of palette indices) from the DIB. 
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, 
        DIB_RGB_COLORS)) 
    {
        AfxMessageBox("GetDIBits"); 
    }

    // Create the .BMP file. 
    hf = CreateFile(pszFile, 
                   GENERIC_READ | GENERIC_WRITE, 
                   (DWORD) 0, 
                    NULL, 
                   CREATE_ALWAYS, 
                   FILE_ATTRIBUTE_NORMAL, 
                   (HANDLE) NULL); 
    if (hf == INVALID_HANDLE_VALUE) 
        AfxMessageBox("CreateFile"); 
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
    // Compute the size of the entire file. 
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
                 pbih->biSize + pbih->biClrUsed 
                 * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    // Compute the offset to the array of color indices. 
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
                    pbih->biSize + pbih->biClrUsed 
                    * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file. 
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
        (LPDWORD) &dwTmp,  NULL)) 
    {
       AfxMessageBox("WriteFile"); 
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
                  + pbih->biClrUsed * sizeof (RGBQUAD), 
                  (LPDWORD) &dwTmp, ( NULL)))
        AfxMessageBox("WriteFile"); 

    // Copy the array of color indices into the .BMP file. 
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
        AfxMessageBox("WriteFile"); 

    // Close the .BMP file. 
     if (!CloseHandle(hf)) 
           AfxMessageBox("CloseHandle"); 

    // Free memory. 
    GlobalFree((HGLOBAL)lpBits);
}

