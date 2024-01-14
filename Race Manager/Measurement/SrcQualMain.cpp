/******************
*                 *  SrcQualMain.cpp
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

CSrcQualMain::CSrcQualMain(CWslWatchSource *pOwner, CTIMeasuredRace *pRace):CWslWatchSource(pOwner)
{

	SE(C_WHITE, 3, W_ALIGN_LEFT, C_BLUE1, C_WHITE, TRUE);
	RW(NEW_TEXT(" ", 0, 88, 396, 392, this));

	SE(-1, -1, -1, -1, -1, FALSE);
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_LAST_LAP), 12,  96, 300,  24, this));
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_BEST_LAP), 12, 226, 300,  24, this));
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_NUM_LAPS), 12, 360, 180,  24, this));

   if (pRace) {
      CTIRS *rs = pRace->GetRS();
      switch (rs->m_nPrimaryEval)
      {
      case RS_EVAL_NUM_LAPS: RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_AVGSPD),  198, 360, 180,  24, this)); break;
      case RS_EVAL_FIRST_TIME: RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_CURRENT_RANK), 198, 360, 180,  24, this)); break;
      default: ASSERT(FALSE);
      }
   }
   else {
      RW(NEW_TEXT("DUMMY",  198, 360, 180,  24, this));
   }

	SE(C_BLUE3, 8, W_ALIGN_RIGHT);
	m_nLapTime=RW(NEW_TEXT("",              60, 124, 320,  80, this));
	SE(C_WHITE);
	m_nBestTime=RW(NEW_TEXT("",              60, 258, 320,  80, this));

	SE(C_BLUE3, 7, W_ALIGN_RIGHT);
	m_nLapCount=RW(NEW_TEXT("",                    30, 396, 160,  80, this));
	m_nPlacement=RW(NEW_TEXT("",                 220, 396, 160,  80, this));
}

/******************************************************************************/

CSrcQualMain::~CSrcQualMain() {
	CWslWatchSource::~CWslWatchSource();
}

/******************************************************************************/

void CSrcQualMain::UpdateDisplays(int nLapTime, int nBestTime, int nLapCount, int nPlacement)
{
	char temp[256];
	TimeFormat(nLapTime, temp, 3);
	if (nLapTime>0) GetText(m_nLapTime)->SetText(temp);
	TimeFormat(nBestTime, temp, 3);
	if (nBestTime>0) GetText(m_nBestTime)->SetText(temp);
	if (nLapCount>0) GetText(m_nLapCount)->SetText(nLapCount);
	if (nPlacement>0) GetText(m_nPlacement)->SetText(nPlacement);

	if (nLapTime==0) GetText(m_nLapTime)->SetText("0.000");
	if (nBestTime==0) GetText(m_nBestTime)->SetText("0.000");
	if (nLapCount==0) GetText(m_nLapCount)->SetText("0");
	if (nPlacement==0) GetText(m_nPlacement)->SetText("–");
}

/******************************************************************************/

void CSrcQualMain::ResetDisplays(CTIMeasuredEntity *pEntity)
{
	GetText(m_nLapTime)->SetText("0.000");
	GetText(m_nBestTime)->SetText("0.000");
	GetText(m_nLapCount)->SetText("0");

   int nRank = pEntity->GetCurrentQualRank();
   if (nRank) GetText(m_nPlacement)->SetText(nRank);
   else GetText(m_nPlacement)->SetText("–");
}

/******************************************************************************/