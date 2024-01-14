/******************
*                 *  WslBitmap.cpp
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

#include "stdafx.h"
#include "main.h"
#include "WslBitmap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  CWslBitmap
*
*******************************************************************************/

CWslBitmap::CWslBitmap()
{
	m_nBitDepth=0;
	m_nSizeX=0;
	m_nSizeY=0;
	m_hBitmap=NULL;
}

/******************************************************************************/

CWslBitmap::~CWslBitmap()
{

}

/******************************************************************************/

HBITMAP CWslBitmap::Load(const char *pcszFileName) {
   if (m_hBitmap) {
      ASSERT(FALSE);
      return m_hBitmap;
   }
	m_hBitmap = LoadDIBSectionFromFile(pcszFileName, NULL, NULL, 0, &m_nSizeX, &m_nSizeY, &m_nBitDepth);
	return m_hBitmap;
}

/******************************************************************************/

int CWslBitmap::Paint(CDC *pDC, int nPosX, int nPosY) {

	if (m_hBitmap) {
		DrawDIBSection(pDC->m_hDC, m_hBitmap, nPosX, nPosY);
		return 1;
	}
	else return 0;
}

/*********************************************************************** eof **/
