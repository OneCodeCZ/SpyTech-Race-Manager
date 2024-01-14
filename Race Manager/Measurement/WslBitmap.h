/******************
*                 *  WslBitmap.h
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of World Spy Library.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#if !defined(AFX_WSLBITMAP_H__3D101C01_F2D8_11D3_B030_0000B49E3BD7__INCLUDED_)
#define AFX_WSLBITMAP_H__3D101C01_F2D8_11D3_B030_0000B49E3BD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Bitmap operations.h"

class CWslBitmap  
{
	int m_nSizeX;
	int m_nSizeY;
	int m_nBitDepth;
	HBITMAP m_hBitmap;
public:
	CWslBitmap();
	HBITMAP Load(const char *pcszFileName);
	int Paint(CDC *pDC, int nPosX, int nPosY);
	int GetSizeX() { return m_nSizeX; };
	int GetSizeY() { return m_nSizeY; };
	virtual ~CWslBitmap();

};

#endif // !defined(AFX_WSLBITMAP_H__3D101C01_F2D8_11D3_B030_0000B49E3BD7__INCLUDED_)
