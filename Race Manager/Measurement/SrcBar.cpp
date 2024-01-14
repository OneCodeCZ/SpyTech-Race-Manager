/******************
*                 *  SrcBar.cpp
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

/******************************************************************************/

CSrcBar::CSrcBar(CWslWatchSource *pOwner) : CWslWatchSource(pOwner) {
/*
	// --------------------------------------------------------------------------
	// Create caption
	SE(C_GREY2, 0, W_ALIGN_LEFT, C_GREY1, C_WHITE, FALSE);
	m_nStatFirst=
		RW(NEW_TEXT("  STM/2008", 0, 576, 768, 12, this));
		RW(NEW_TEXT("  Calibration",       0, 588, 768, 12, this));

	// --------------------------------------------------------------------------
	// Create value displays
	SE(C_WHITE, 0, W_ALIGN_RIGHT, -1, C_WHITE);
	m_nStatTime=2;
		for (int a=0; a<8; a++) RW(NEW_TEXT("0", 316-32+a*60, 576, 60, 12, this, TRUE));
	m_nStatDelta=10;
		for (a=0; a<8; a++) RW(NEW_TEXT("0", 316-32+a*60, 588, 60, 12, this, TRUE));
//		for (a=0; a<8; a++) RW(NEW_TEXT("0", 316+a*60, 588, 30, 12, this, TRUE));
//	m_nStatCount=18;
//		for (a=0; a<8; a++) RW(NEW_TEXT("0", 346+a*60, 588, 30, 12, this, TRUE));

	// --------------------------------------------------------------------------
	// Create status watches
	SE(C_GREY2, 0, W_ALIGN_RIGHT, -1, C_WHITE);
		RW(NEW_TEXT("Clock", 80, 576, 60, 12, this));
		RW(NEW_TEXT("Timer", 140, 576, 60, 12, this));

	SE(C_WHITE, 0, W_ALIGN_RIGHT, -1, C_WHITE);
	m_nStatClock=
		RW(NEW_TEXT("0", 80, 588, 60, 12, this));
	m_nStatTimer=
		RW(NEW_TEXT("0", 140, 588, 60, 12, this));

	SE(C_GREY2, 0, W_ALIGN_RIGHT, -1, C_WHITE);
		RW(NEW_TEXT("Power", 220, 576, 40, 12, this));
		RW(NEW_TEXT("Sensors", 220, 588, 40, 12, this));
	
	SE(C_WHITE, 0, W_ALIGN_LEFT, -1, C_WHITE);
//	m_nStatPower=
//		RW(NEW_TEXT("On", 284, 576, 40, 12, this));

	m_nStatPower=
		RegisterWatch(new CWslWatchColorBox(RGB(0x00, 0x00, 0x00), 264, 579, 8, 8, this));

	m_nStatInverted=
		RW(NEW_TEXT("Inv", 264, 588, 40, 12, this));
*/
	// --------------------------------------------------------------------------
	// Create the progress bar as the last watch in this source
	m_nStatProgress=
		RW(NEW_PROGRESS(0, 1024, 4, 0, 576, 768, 24, this));

	m_nStatPowerLed=
		RegisterWatch(new CWslWatchColorBox(RGB(0x00, 0x00, 0x00), 776, 576, 24, 24, this));

	// --------------------------------------------------------------------------
	// Remember total number of watches
	m_nNumStatWatches=m_apWatches.GetSize();

	// --------------------------------------------------------------------------
	// Initialy, the monitor is hidden and the progress is shown
	SetMode(BAR_PROGRESS);
}

/******************************************************************************/

CSrcBar::~CSrcBar() {
	CWslWatchSource::~CWslWatchSource();
}

/******************************************************************************/

void CSrcBar::SetMode(int nMode) {
	int a;

	switch(m_nMode) {
	case BAR_PROGRESS:
		for (a=0; a<m_nNumStatWatches-2; a++) GetWatch(a)->Visibility(SW_HIDE);
		GetWatch(m_nStatProgress)->Visibility(SW_SHOW);
		GetWatch(m_nStatPowerLed)->Visibility(SW_SHOW);
		break;
	case BAR_MONITOR:
		for (a=0; a<m_nNumStatWatches-2; a++) GetWatch(a)->Visibility(SW_SHOW);
		GetWatch(m_nStatProgress)->Visibility(SW_HIDE);
		GetWatch(m_nStatPowerLed)->Visibility(SW_SHOW);
		break;
	}

	m_nMode=nMode;
}

/******************************************************************************/

void CSrcBar::SetTime(int nElapsed, int nTotal, int nDiv) {
	CWslWatchProgress *pProgress = (CWslWatchProgress*) GetWatch(m_nStatProgress);
	pProgress->SetCurrent(nElapsed, 0, nTotal, nDiv);
}

/******************************************************************************/

void CSrcBar::SetMonitor(CSlotTrackMonitor *pMonitor, BOOL bWillPause) {
/*
	char temp[32];
	for (int nIdx=0; nIdx<NUMTRACKS; nIdx++) {
		if (pMonitor->m_bArrWasIllegal[nIdx]) wsprintf(temp, "err %d", pMonitor->m_dwArrLastValues[nIdx]);
		else wsprintf(temp, "%d", pMonitor->m_dwArrLastValues[nIdx]);

		GetText(m_nStatTime+nIdx)->SetText(temp);
//		GetText(m_nStatCount+nIdx)->SetText(pMonitor->m_dwArrNumPulses[nIdx]);
		GetText(m_nStatDelta+nIdx)->SetText(pMonitor->m_dwArrLastAccepted[nIdx]);
//		GetText(m_nStatDelta+nIdx)->SetText(pMonitor->m_dwArrDelta[nIdx]);
	}
	GetText(m_nStatClock)->SetText(pMonitor->m_dwClock);
	GetText(m_nStatTimer)->SetText(pMonitor->m_dwTimer);

//	if (pMonitor->m_bPower) GetText(m_nStatPower)->SetText("On");
//	else GetText(m_nStatPower)->SetText("Off");

	CWslWatchColorBox* pWatch=(CWslWatchColorBox*)GetWatch(m_nStatPower);
*/
	CWslWatchColorBox* pWatch2=(CWslWatchColorBox*)GetWatch(m_nNumStatWatches-1);

   BOOL bBlackGate = FALSE;
   if (bWillPause) 
   {
      // if ((GetTickCount() / 500) % 2 == 0) bBlackGate = TRUE;
      // else bBlackGate = FALSE;
      bBlackGate = TRUE;
   }

	if (pMonitor->m_bPower) {
      if (bBlackGate) pWatch2->SetColor(RGB(255, 128, 0));
      else pWatch2->SetColor(RGB(0, 192, 64));
	}
	else {
      if (bBlackGate) pWatch2->SetColor(RGB(255, 128, 0));
      else pWatch2->SetColor(RGB(255, 32, 0));
	}

/*
	if (pMonitor->m_bInvert) GetText(m_nStatInverted)->SetText("INV");
	else GetText(m_nStatInverted)->SetText("NORM");
*/
}

/******************************************************************************/
