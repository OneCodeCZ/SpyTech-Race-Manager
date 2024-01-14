/******************
*                 *  SrcRaceMain.cpp
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
*  CSrcRaceMain
*
*******************************************************************************/

void CSrcRaceMain::SetRacerData(int nLine, int nTrack, COLORREF crColor, const char *pcszRacer)
{
	if (nTrack) m_pSrcTable->GetTextField(nLine, 0)->SetText(nTrack);
	else m_pSrcTable->GetTextField(nLine, 0)->SetText(" ");
	m_pSrcTable->GetColorBox(nLine, 1)->SetColor(crColor);
	m_pSrcTable->GetTextField(nLine, 2)->SetText(pcszRacer);
}

/******************************************************************************/

void CSrcRaceMain::SetRacerResults(int nLine, int nLastLap, int nBestLap, int nNumLaps, int nTotNumLaps, int nLoss, int nPrognosis, int nOrder, int nCurrentLane, bool bEbUpdate)
{
   if ((nCurrentLane != -1) && bEbUpdate) CallEBUpdateLaps(nCurrentLane+1, nTotNumLaps);

	char temp[256];
	if (nLastLap!=-1) {
		TimeFormat(nLastLap, temp, 3);
		m_pSrcTable->GetTextField(nLine, 3)->SetText(temp);
	}
	if (nBestLap!=-1) {
		if (nBestLap == 0) {
			m_pSrcTable->GetTextField(nLine, 4)->SetText("–");
		}
		else {
			TimeFormat(nBestLap, temp, 3);
			m_pSrcTable->GetTextField(nLine, 4)->SetText(temp);
		}
	}
	if (nNumLaps!=-1)m_pSrcTable->GetTextField(nLine, 5)->SetText(nNumLaps);
	if (nTotNumLaps!=-1)m_pSrcTable->GetTextField(nLine, 6)->SetText(nTotNumLaps);
	if (nLoss!=-1)m_pSrcTable->GetTextField(nLine, 7)->SetText(-1*nLoss);
	if (nPrognosis!=-1)m_pSrcTable->GetTextField(nLine, 8)->SetText(nPrognosis);
	wsprintf(temp, "%d.", nOrder);
	if (nOrder!=-1) m_pSrcTable->GetTextField(nLine, 9)->SetText(temp);
}

/******************************************************************************/

CSrcRaceMain::CSrcRaceMain(CWslWatchSource *pOwner, int nNumRacers)
{
	SE(C_WHITE, 3, W_ALIGN_LEFT, C_BLUE1, C_WHITE, TRUE);
	RW(NEW_TEXT(" ", 0, 88, 800, 392, this));

	SE(C_WHITE, 1, W_ALIGN_LEFT, C_BLUE1, C_WHITE, FALSE);
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_LANE),      9+8,  92,  54, 26, this));
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_RACER),    65+8,  92, 284, 26, this));
	SE(C_WHITE, 1, W_ALIGN_RIGHT, C_BLUE1, C_WHITE, FALSE);
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_LAST_LAP),   349+8-70,  92,  70, 26, this));
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_BEST_LAP),   419+8-70,  92,  70, 26, this));
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_NUM_LAPS),   489+8-70,  92,  70, 26, this));
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_AVGSPD),     559+8-70,  92,  70, 26, this));
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_GAP),        629+8-70,  92,  70, 26, this));
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_PROGNOSIS),  699+8-70,  92,  70, 26, this));
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_POSITION),   769+8-70,  92,  70, 26, this));

	m_pSrcTable=new CSrcNiceTable(this);
	m_pSrcTable->SetParams(1, 120, 798, 360, 35, 0);
	m_pSrcTable->RegisterColumn(8, W_ALIGN_RIGHT, W_TYPE_DUMMY, 0, 0, 0, FALSE);         // 1
	m_pSrcTable->RegisterColumn(24, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 3, FALSE);  // 9
	m_pSrcTable->RegisterColumn(6, W_ALIGN_RIGHT, W_TYPE_DUMMY, 0, 0, 0, FALSE);         // 33
	m_pSrcTable->RegisterColumn(24, W_ALIGN_RIGHT, W_TYPE_COLORBOX, 0, 0, 0, FALSE);     // 39
	m_pSrcTable->RegisterColumn(12, W_ALIGN_RIGHT, W_TYPE_DUMMY, 0, 0, 0, FALSE);        // 53
	m_pSrcTable->RegisterColumn(214, W_ALIGN_LEFT, W_TYPE_TEXT, C_WHITE, 0, 4, FALSE);   // 65
	m_pSrcTable->RegisterColumn(70, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);   // 349
	m_pSrcTable->RegisterColumn(70, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);   // 419
	m_pSrcTable->RegisterColumn(70, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);   // 489
	m_pSrcTable->RegisterColumn(70, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);   // 559
	m_pSrcTable->RegisterColumn(70, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);   // 629
	m_pSrcTable->RegisterColumn(70, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);   // 699
	m_pSrcTable->RegisterColumn(70, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);   // 699
	m_pSrcTable->RegisterColumn(500, W_ALIGN_LEFT, W_TYPE_DUMMY, 0, 0, 0, TRUE);
	m_pSrcTable->BuildFields(10);

   RW(m_pSrcTable);
}

/******************************************************************************/

CSrcRaceMain::~CSrcRaceMain()
{
}

/******************************************************************************/

void CSrcRaceMain::Reset(int nLine) {
	m_pSrcTable->GetTextField(nLine, 0)->SetText(" ");
	m_pSrcTable->GetColorBox(nLine, 1)->SetColor((COLORREF)-1);
	m_pSrcTable->GetTextField(nLine, 2)->SetText(" ");
	m_pSrcTable->GetTextField(nLine, 3)->SetText(" ");
	m_pSrcTable->GetTextField(nLine, 4)->SetText(" ");
	m_pSrcTable->GetTextField(nLine, 5)->SetText(" ");
	m_pSrcTable->GetTextField(nLine, 6)->SetText(" ");
	m_pSrcTable->GetTextField(nLine, 7)->SetText(" ");
	m_pSrcTable->GetTextField(nLine, 8)->SetText(" ");
}

void CSrcRaceMain::ResetLaps(int nLine, BOOL bClear)
{
   if (bClear) {
   	m_pSrcTable->GetTextField(nLine, 3)->SetText(" ");
	   m_pSrcTable->GetTextField(nLine, 4)->SetText(" ");
	   m_pSrcTable->GetTextField(nLine, 5)->SetText(" ");
   }
   else {
	   m_pSrcTable->GetTextField(nLine, 3)->SetText(" ");
	   m_pSrcTable->GetTextField(nLine, 4)->SetText("–");
	   m_pSrcTable->GetTextField(nLine, 5)->SetText("0");
   }
}

/******************************************************************************/

void CSrcRaceMain::PrepareForNextRide(CTIMeasuredRace *pRace)
{
	pRace->m_pWnd->m_pSrcRaceMain->SetLines(pRace, TRUE);
}

/******************************************************************************/

void CSrcRaceMain::SetLines(CTIMeasuredRace *pRace, BOOL bCalledFromPNR) {

	CApp *pApp = WsfGetCApp();

	CEntityArray *pArray = &pRace->m_pArrSortedGroup;
	CTIMeasuredEntity *pEntity;

	int a;
   if (bCalledFromPNR) for (a=0; a<10; a++) {
      pEntity = FALSE;
      if (a < pArray->GetSize()) pEntity = DC_ME((*pArray)[a]);
      if (pEntity) ResetLaps(a, FALSE);
      else ResetLaps(a, TRUE);
   }
   else {
      for (a=0; a<10; a++) Reset(a);
	   for (a=0; a<pArray->GetSize(); a++) {
   		pEntity = DC_ME((*pArray)[a]);
		   SetRacerData(
   			a, 
			   pEntity->m_nCurrentTrack+1, 
			   pEntity->m_nCurrentTrack+1 == 0 ? -1 : pApp->GetLaneColor(/*pRace->m_nLaneColor[*/pEntity->m_nCurrentTrack/*]*/),
			   pEntity->GetEntityName(CTIRaceEntity::NICKNAME)
		   ); 
		   UpdateRacerResults(pEntity);
	   }
   }
}

/******************************************************************************/

void CSrcRaceMain::UpdateRacerResults(CTIMeasuredEntity *pEntity)
{
	CTIMeasuredRace *pRace = dynamic_cast<CTIMeasuredRace*>(pEntity->GetParent());
	ASSERT(pRace);
	CEventData *pEvent = pEntity->GetCurrentEvent();

	CRideData *pRide;
	if (pRace->m_bPauseBetweenRides) pRide = pEvent->GetPreviousRide();
	else pRide = pEvent->GetCurrentRide();

	int nLine = pEntity->m_nDisplayLine;

	if (pRide) {
		SetRacerResults
		(  nLine, 
			pRide->m_dwLastTime, 
			pRide->GetBestTime(), 
			pRide->m_dwNumLaps, 
			pEvent->GetTotalLaps(), 
			0,
			0,
			0,
         pEntity->m_nCurrentTrack,
         1
		);
	}
	else {
		SetRacerResults(nLine, 0, 0, 0, 0, 0, 0, 0, pEntity->m_nCurrentTrack, 0);
	}
}

/******************************************************************************/

void CSrcRaceMain::UpdatePosition(CTIMeasuredRace *pRace)
{
	// Get the size
	int nNum = pRace->m_aPartEntities.GetSize();
	// Just for feeling that something has been done...
	if (nNum == 1) {
		SetRacerResults(0, -1, -1, -1, -1, -1, -1, 1, 0, 0);
		return;
	}

	CEntityArray array = pRace->m_aPartEntities;
	for (int a=0; a<array.GetSize()-1; a++) {
		for (int b=1; b<array.GetSize(); b++) {
			CTIMeasuredEntity *p1 = array[b-1];
			CTIMeasuredEntity *p2 = array[b];
			if (p1->m_dwPrognosis < p2->m_dwPrognosis) {
				array[b-1] = p2;
				array[b] = p1;
			}
		}
	}

	ITERATE(array, a) {
		CString s;
		s.Format("%s -- %d\n", array[a]->GetEntityName(CTIRaceEntity::NAME_SURNAME), array[a]->m_dwPrognosis);
		TRACE1("%s", s);
	}

	int nIdx;
	ITERATE(array, nIdx) {
		int nNum;
		if (array[nIdx]->m_dwPrognosis) nNum = nIdx;
		else nNum = 0;
		array[nIdx]->m_nCurrentOrder = nNum + 1;
	}

/*
   // If there two racers with the same position, show the lower number
   for (nIdx = array.GetSize()-1; nIdx > 0; nIdx--) {
      if (array[nIdx]->m_dwPrognosis == array[nIdx-1]->m_dwPrognosis) {
         array[nIdx - 1]->m_nCurrentOrder = array[nIdx]->m_nCurrentOrder;
      }
   }
*/
	// If there are two racers with the same pos, show the higher number
   for (nIdx = 0; nIdx < array.GetSize()-1; nIdx++) {
      if (array[nIdx]->m_dwPrognosis == array[nIdx+1]->m_dwPrognosis) {
         array[nIdx+1]->m_nCurrentOrder = array[nIdx]->m_nCurrentOrder;
      }
   }

	// STEP TWO: DISPLAY THE NUMBERS

	CEntityArray *pArray = &pRace->m_pArrSortedGroup;
	int nNumEntities = pArray->GetSize();

	CTIMeasuredEntity *pEntity;

	for (a=0; a<nNumEntities; a++) {
		pEntity = (*pArray)[a];
		char temp[256];
		if (pEntity->m_dwPrognosis!=0) wsprintf(temp, "%d", pEntity->m_nCurrentOrder);
		else strcpy(temp, "–");
		m_pSrcTable->GetTextField(a, 9)->SetText(temp);
	}
}

/******************************************************************************/

void CSrcRaceMain::UpdatePrognosis(CTIMeasuredRace *pRace, CTIMeasuredEntity *pEntity, int nCurrentTime)
{

	CEventData *pEvent = pEntity->GetCurrentEvent();
	CRideData *pRide = pEvent->GetCurrentRide();

	int nLine = pEntity->m_nDisplayLine;

	// --------------------------------------------------------------------------
	// STEP ONE: Calc stuff

   int nEffectiveNumRides = max(pRace->GetNumRides(), pRace->GetNumLanes());

	// Total time available for given racer in the event
	double fTotalTime = (double)
//		pRace->GetRS()->GetTimeSettings(pRace->m_nRacePart, 1) * pRace->GetNumRides();
		pRace->GetRS()->GetTimeSettings(pRace->m_nRacePart, 1) * nEffectiveNumRides;

	// Total time already elapsed
	double fElapsedTime = (double)
		(pRace->GetRS()->GetTimeSettings(pRace->m_nRacePart, 1) * pRace->m_nCurrentRide) + nCurrentTime;

   // Total time can't be less than the total time.
   fElapsedTime = min(fElapsedTime, fTotalTime);

	// Get laps gained
	double fGainedLaps = (double)
		pEvent->GetTotalLaps();

	// Assume laps
	double fLapsAssumed;

	if (fElapsedTime == 0.0f) fLapsAssumed = 0.0f;
	else fLapsAssumed = (fTotalTime * fGainedLaps) / fElapsedTime;

	pEntity->m_dwPrognosis = (DWORD)(fLapsAssumed + 0.5);

	// --------------------------------------------------------------------------
	// STEP TWO: Display stuff

	SetRacerResults(nLine, -1, -1, -1, -1, -1, (DWORD)fLapsAssumed, -1, pEntity->m_nCurrentTrack, 0);

}

/******************************************************************************/

void CSrcRaceMain::UpdateLoss(CTIMeasuredRace *pRace) {

	CEntityArray *pArray = &pRace->m_pArrSortedGroup;
	int nNumEntities = pArray->GetSize();
	
	// -------------------------------------------------------------------------
	// STEP ONE: Decide, who's best for now

	DWORD dwMaxLaps = 0;
	CTIMeasuredEntity *pEntity;
	CTIMeasuredEntity *pBest;
	int a;

	for (a=0; a<nNumEntities; a++) {
		pEntity = (*pArray)[a];
		DWORD dwNumLaps = pEntity->GetCurrentEvent()->GetTotalLaps();
		if (dwMaxLaps < dwNumLaps) {
			dwMaxLaps = dwNumLaps;
			pBest = pEntity;
		}
	}

	// -------------------------------------------------------------------------
	// STEP TWO: Display the numbers

	for (a=0; a<nNumEntities; a++) {
		pEntity = (*pArray)[a];
		int nLoss = dwMaxLaps - pEntity->GetCurrentEvent()->GetTotalLaps();
		m_pSrcTable->GetTextField(a, 7)->SetText(nLoss);
	}

}

/*********************************************************************** eof **/
