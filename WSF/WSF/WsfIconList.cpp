///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfIconList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//*****************************************************************************
CWsfIconList::CWsfIconList()
{
}

//*****************************************************************************
CWsfIconList::~CWsfIconList()
{
}

//*****************************************************************************
int CWsfIconList::AddIcons(int nBmpIdx, BOOL bMakeGrayScale)
{
	CBitmap bmp;

	ASSERT(nBmpIdx > 0);
	ASSERT(GetIconIdx(nBmpIdx,0) == -1);

	m_aBmps.GrowBy(1);
	CBitmapInfo& bi = m_aBmps.GetLast();
	bi.nBmpId = nBmpIdx;
	bi.nFirstIconIdx = GetImageCount();

	VERIFY(bmp.LoadBitmap(nBmpIdx));

   // Gray scale

   if (bMakeGrayScale)
   {
      BITMAP b;
      bmp.GetBitmap(&b);

      BITMAPINFO bi; 
      bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
      bi.bmiHeader.biPlanes = 1;
      bi.bmiHeader.biBitCount = 32;
      bi.bmiHeader.biCompression = BI_RGB;
      bi.bmiHeader.biSizeImage = 0;
      bi.bmiHeader.biClrUsed = 0;
      bi.bmiHeader.biClrImportant = 0;
      bi.bmiHeader.biHeight = b.bmHeight;
      bi.bmiHeader.biWidth = b.bmWidth;
   
      COLORREF* pData = new COLORREF[b.bmHeight*b.bmWidth];
      HDC hDC = ::GetDC(NULL);

      GetDIBits(hDC, bmp, 0, b.bmHeight, (void*)pData, &bi, DIB_RGB_COLORS);

      COLORREF rgbBackgnd = ::GetSysColor(COLOR_MENU);

      for (int i=0; i < b.bmHeight*b.bmWidth; i++)
      {
         COLORREF rgb = pData[i];
         rgb = RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb));
      
         if (rgb != RGB(0,255,255))
         {
            int n = (int)((GetRValue(rgb) * 0.299) +
               (GetGValue(rgb) * 0.587) +
               (GetBValue(rgb) * 0.114));
            double f = n/255.0/3+0.66;
            
            pData[i] = RGB(GetBValue(rgbBackgnd)*f, 
               GetGValue(rgbBackgnd)*f, 
               GetRValue(rgbBackgnd)*f);
         }
      }

      SetDIBits(NULL, bmp, 0, b.bmHeight, (void*)pData, &bi, DIB_RGB_COLORS);
      delete[] pData;
      DeleteObject(hDC);
   }

   // Add icons to the list

	VERIFY(Add(&bmp, RGB(0,255,255)) != -1);		

	return bi.nFirstIconIdx;
}

//*****************************************************************************
int CWsfIconList::GetIconIdx(int nBmpIdx, int nIconIdx)
{
	for (int i=m_aBmps.GetSize(); i--;)
	{
		if (m_aBmps[i].nBmpId == nBmpIdx) 
			return m_aBmps[i].nFirstIconIdx + nIconIdx;
	}
	return -1;
}








