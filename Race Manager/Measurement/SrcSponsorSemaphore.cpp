/******************
*                 *  SrcSponsorSemaphore.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"
// #include "WndRace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  CSrcSponsorSemaphore
*
*******************************************************************************/

CSrcSponsorSemaphore::CSrcSponsorSemaphore(CWslWatchSource *pOwner)
:CWslWatchSource(pOwner)
{
	SE(C_WHITE, 3, W_ALIGN_LEFT, C_BLUE1, C_WHITE, TRUE);
//	RW(NEW_TEXT("TEXT", 404, 488, 396, 80, this));

   /*
	RW(new CWslWatchColorCircle(C_RED1, 404-40, 492, 72, 72, this));
	RW(new CWslWatchColorCircle(C_RED1, 485-40, 492, 72, 72, this));
	RW(new CWslWatchColorCircle(C_RED1, 566-40, 492, 72, 72, this));
	RW(new CWslWatchColorCircle(C_RED1, 647-40, 492, 72, 72, this));
	RW(new CWslWatchColorCircle(C_GREEN1, 728-40, 492, 72, 72, this));
   */

	RW(new CWslWatchBitmap(&gdi.m_SponsorBitmap, 404, 488, 396, 80, this));
//	RW(NEW_TEXT("???", 404, 488, 100, 20, this));
	RW(new CWslWatchBitmap(&gdi.m_RedDim,   400-52, 488, this));
	RW(new CWslWatchBitmap(&gdi.m_RedDim,   485-52, 488, this));
	RW(new CWslWatchBitmap(&gdi.m_RedDim,   570-52, 488, this));
	RW(new CWslWatchBitmap(&gdi.m_RedDim,   655-52, 488, this));
	RW(new CWslWatchBitmap(&gdi.m_GreenDim, 740-52, 488, this));

	SetLights(0);
	SetMode(SPSE_MODE_SEMAPHORE);
}

/******************************************************************************/

CSrcSponsorSemaphore::~CSrcSponsorSemaphore() {
	CWslWatchSource::~CWslWatchSource();
}

/******************************************************************************/

void CSrcSponsorSemaphore::SetMode(int nNewMode) {
	int nIdx;
	switch (nNewMode) {
	case SPSE_MODE_SPONSOR:
		m_nLastLit = 0;
		GetWatch(0)->Visibility(SW_SHOW);
		GetWatch(1)->Visibility(SW_HIDE);
		for (nIdx=1; nIdx<6; nIdx++) {
			GetWatch(nIdx)->Visibility(SW_HIDE);
		}
		break;
	case SPSE_MODE_SEMAPHORE:
		m_nLastLit = 0;
//		SetLights(0);
		GetWatch(0)->Visibility(SW_HIDE);
		GetWatch(1)->Visibility(SW_HIDE);
		for (nIdx=2; nIdx<6; nIdx++) {
			GetWatch(nIdx)->Visibility(SW_SHOW);
		}
		break;
	}
}

/******************************************************************************/

void CSrcSponsorSemaphore::SetLights(int nNumLit) {
   switch (nNumLit) {
   case 0: CallEBUpdateLights(0); break;
   case 2: CallEBUpdateLights(8); break;
   case 3: CallEBUpdateLights(8+4); break;
   case 4: CallEBUpdateLights(8+4+2); break;
   case 5: CallEBUpdateLights(1); break;
   }
	for (int a=2; a<6; a++) 
   {
      /*
		CWslWatchColorCircle *pCircle=(CWslWatchColorCircle*)GetWatch(a);
		if (a!=5) pCircle->SetColor(a<=nNumLit ? C_RED2 : C_RED1);
		else pCircle->SetColor(a<=nNumLit ? C_GREEN2 : C_GREEN1);
      */
      CWslWatchBitmap *pBitmap = (CWslWatchBitmap*)GetWatch(a);
      if (a!=5) pBitmap->SetBitmap(a<=nNumLit ? &gdi.m_RedLit : &gdi.m_RedDim);
      else pBitmap->SetBitmap(a<=nNumLit ? &gdi.m_GreenLit : &gdi.m_GreenDim);
	}
}

/******************************************************************************/

void CSrcSponsorSemaphore::SetTimeRemains(int nNumMs, CSoundStream &snd) {
	int nNewLit = 0;
	if (nNumMs <= 0) nNewLit = 5;
	if (nNumMs > 0) nNewLit = 4;
	if (nNumMs > 1000) nNewLit = 3;
	if (nNumMs > 2000) nNewLit = 2;
	if (nNumMs > 3000) nNewLit = 0;

	if (nNewLit != m_nLastLit) {
		SetLights(nNewLit);
	}
	m_nLastLit = nNewLit;
}

/*********************************************************************** eof **/
