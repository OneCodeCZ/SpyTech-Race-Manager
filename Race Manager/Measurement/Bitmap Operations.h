#if !defined(__BITMAP_OPS__)
#define __BITMAP_OPS__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Load BMP function
HBITMAP LoadDIBSectionFromFile( LPCTSTR lpszFileName, LPVOID *ppvBits=NULL, 
				HANDLE hSection=NULL, DWORD dwOffset=0, int *nSizeX=0, int *nSizeY=0, int *nBitDepth=0) ;

// Creates bitmap from prototype
HBITMAP CreateDIBFromPrototype( LPCTSTR lpszFileName, LPVOID *ppvBits, 
					HANDLE hSection, DWORD dwOffset, int nSizeX, int nSizeY, int nBitDepth);

// Draws DIB section
void DrawDIBSection( HDC hDC, HBITMAP hBitmap, int xDest, int yDest );

void StretchDIBSection( HDC hDC, HBITMAP hBitmap, int xDest, int yDest, int xRes, int yRes );

// Stores the DIB on the disk
BOOL WriteDIB( LPTSTR szFile, HBITMAP hBitmap);

PBITMAPINFO CreateBitmapInfoStruct(HBITMAP hBmp);

void CreateBMPFile(LPTSTR pszFile, PBITMAPINFO pbi, HBITMAP hBMP, HDC hDC);

#endif // __BITMAP_OPS__
