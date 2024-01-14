/******************
*                 *  SrcTime.cpp
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
//#include "WndRace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/******************************************************************************/

CSrcTime::CSrcTime(CWslWatchSource *pOwner) :CWslWatchSource(pOwner) {

	SE(C_WHITE, 2, W_ALIGN_CENTER, C_GREY1, C_WHITE, TRUE);
	RW(NEW_TEXT(" ", 0, 488, 396, 80, this));
			
	SE(C_WHITE, 3, W_ALIGN_LEFT, C_GREY1, C_WHITE, FALSE);
	RW(NEW_TEXT(" ", 12, 496, 200, 24, this));
			
	SE(-1, 6, W_ALIGN_RIGHT);
	RW(NEW_TEXT(" ", 60, 516, 320, 40, this));

   m_nLastTimeSeconds = -1;
   m_nLastTimeTotalMs = -1;
   m_nLastCountMode = -1;
}

/******************************************************************************/

void CSrcTime::SetMode(int nMode, int nFormat, int nCountMode) {

	SetCaption(TIME_CAPTION_ZEROES);

	switch(nMode) {
	case TIME_MODE_SMALL:
		GetText(0)->ChangePos(0, 488, 396, 80);
		GetText(1)->ChangePos(12, 496, 300, 24);
		GetText(2)->ChangePos(20, 520, 360, 38);
		GetText(2)->ChangeFont(6);
		break;
	case TIME_MODE_LARGE:
		GetText(0)->ChangePos(0, 408, 246, 160);
		GetText(1)->ChangePos(12, 416, 200, 24);
		GetText(2)->ChangePos(60, 440, 170, 125);
		GetText(2)->ChangeFont(9);
		break;
	}
	m_nMode=nMode;
	m_nFormat=nFormat;
	m_nCountMode=nCountMode;
}

/******************************************************************************/

void CSrcTime::SetCaption(int nCaption) {
	switch (nCaption) {
	case TIME_CAPTION_TIME_TO_START:
		GetText(1)->SetText(GetIds(IDS_RACE_CAPTION_TIME_TOSTART));
		break;
	case TIME_CAPTION_TIME_TO_END:
		GetText(1)->SetText(GetIds(IDS_RACE_CAPTION_TIME_TOEND));
		break;
	case TIME_CAPTION_TIME_TO_WARMUP:
		GetText(1)->SetText(GetIds(IDS_RACE_CAPTION_TIME_TOWARMUP));
		break;
	case TIME_CAPTION_TRAINING_TIME_ELAPSED:
		GetText(1)->SetText(GetIds(IDS_RACE_CAPTION_TIME_PRACTICE_ELAPSED));
		break;
	case TIME_CAPTION_TIME_TO_END_WARMUP:
		GetText(1)->SetText(GetIds(IDS_RACE_CAPTION_TIME_WUP_TOEND));
		break;
	case TIME_CAPTION_END:
		GetText(1)->SetText(" ");
		GetText(2)->SetText("00:00:00");
		break;
	case TIME_CAPTION_ATTEMPT_END:
		GetText(1)->SetText(" ");
		GetText(2)->SetText(GetIds(IDS_RACE_CAPTION_TIME_ENDATTEMPT));
		break;
	case TIME_CAPTION_QUAL_LEAVE:
		GetText(1)->SetText(" ");
		GetText(2)->SetText(GetIds(IDS_RACE_CAPTION_TIME_ENDQUAL));
		break;
	case TIME_CAPTION_NOTHING:
		GetText(1)->SetText(" ");
		GetText(2)->SetText(" ");
		break;
	case TIME_CAPTION_ZEROES:
		GetText(1)->SetText(" ");
		GetText(2)->SetText("00:00:00");
		break;
	}
}

/******************************************************************************/

void CSrcTime::SetTime(int nTimeMs, int nTimeTotalMs)
{
	char temp[1024];
	int nDisp;

	switch (m_nCountMode) {
	case TIME_MODE_COUNTUP:
		nDisp = nTimeMs;
		break;
	case TIME_MODE_COUNTDOWN:
		if (nTimeMs >= nTimeTotalMs) nDisp = 0;
		else nDisp = min (nTimeTotalMs, (int)nTimeTotalMs - (int)nTimeMs + 1000);
		break;
	}

	switch (m_nFormat) {
	case TIME_FORMAT_HMS:
		TimeFormat(nDisp, temp, 1);
		break;
	case TIME_FORMAT_S:
		TimeFormat(nDisp, temp, 4);
		break;
	}
	GetText(2)->SetText(temp);

   int whole = nTimeMs / 1000;
   int part = nTimeMs % 1000;

   if (whole != m_nLastTimeSeconds || nTimeTotalMs != m_nLastTimeTotalMs || m_nCountMode != m_nLastCountMode)
   {
      int h = whole / 3600;
      int m = (whole / 60) % 60;
      int s = whole % 60;
      CallEBUpdateTime(h, m, s, 0);
   }

   m_nLastTimeSeconds = whole;
   m_nLastTimeTotalMs = nTimeTotalMs;
   m_nLastCountMode = m_nCountMode;
}

/******************************************************************************/

CSrcTime::~CSrcTime() {
	CWslWatchSource::~CWslWatchSource();
}

/******************************************************************************/
