/******************
*                 *  SrcRaceGroupResult.cpp
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

CSrcRaceGroupResults::CSrcRaceGroupResults(CWslWatchSource *pOwner, int nNumRacers, int nNumRides)
{
	SE(C_WHITE, 3, W_ALIGN_LEFT, C_BLUE1, C_WHITE, TRUE);
	RW(NEW_TEXT(" ", 0, 88, 800, 392, this));

	m_pSrcTable=new CSrcNiceTable(this);
	m_pSrcTable->SetParams(1, 120, 798, 360, 35, 0);
	m_pSrcTable->RegisterColumn(8, W_ALIGN_RIGHT, W_TYPE_DUMMY, 0, 0, 0, FALSE);         // 1
	m_pSrcTable->RegisterColumn(32, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 4, FALSE);  // 9     rank
	m_pSrcTable->RegisterColumn(12, W_ALIGN_RIGHT, W_TYPE_DUMMY, 0, 0, 0, FALSE);        // 53
	m_pSrcTable->RegisterColumn(260, W_ALIGN_LEFT, W_TYPE_TEXT, C_WHITE, 0, 4, FALSE);   // 65    name
	m_pSrcTable->RegisterColumn(36, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 1, FALSE);  // 9
	m_pSrcTable->RegisterColumn(36, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 1, FALSE);  // 9
	m_pSrcTable->RegisterColumn(36, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 1, FALSE);  // 9
	m_pSrcTable->RegisterColumn(36, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 1, FALSE);  // 9
	m_pSrcTable->RegisterColumn(36, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 1, FALSE);  // 9
	m_pSrcTable->RegisterColumn(36, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 1, FALSE);  // 9
	m_pSrcTable->RegisterColumn(36, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 1, FALSE);  // 9
	m_pSrcTable->RegisterColumn(36, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 1, FALSE);  // 9
	m_pSrcTable->RegisterColumn(36, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 1, FALSE);  // 9
	m_pSrcTable->RegisterColumn(36, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 1, FALSE);  // 9
	m_pSrcTable->RegisterColumn(110, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 3, FALSE); // 9     total
	m_pSrcTable->RegisterColumn(500, W_ALIGN_RIGHT, W_TYPE_DUMMY,  C_WHITE, 0, 2, TRUE); // 9
	m_pSrcTable->BuildFields(10);

/*
	m_pSrcTable->GetTextField(0, 0)->SetText("1.");
	m_pSrcTable->GetTextField(0, 1)->SetText("Jmeno Prijmeni");

	char temp[1024];
	for (int b=0; b<10; b++) {
		m_pSrcTable->GetTextField(b, 0)->SetText(itoa(b+1, temp, 10));
		for (int a=0; a<10; a++) {
			if (a+b==8 || a+b==9)
				m_pSrcTable->GetTextField(b, 2+a)->SetText("–");
			else
				m_pSrcTable->GetTextField(b, 2+a)->SetText(rand()%1000);
		}
		m_pSrcTable->GetTextField(b, 2+10)->SetText(rand()%100000);
	}
*/

	RW(m_pSrcTable);
}

/******************************************************************************/

void CSrcRaceGroupResults::SetRacerData(int nLine, int nRank, const char *pcszRacer) {
	char temp[16];
	if (nRank!=-1)	wsprintf(temp, "%d.", nRank);
	else wsprintf(temp, " ");
	m_pSrcTable->GetTextField(nLine, 0)->SetText(temp);
	m_pSrcTable->GetTextField(nLine, 1)->SetText(pcszRacer);
}

/******************************************************************************/

void CSrcRaceGroupResults::SetLaps(int nLine, int nCol, int nNum) {
	if (nNum!=-1)
		m_pSrcTable->GetTextField(nLine, 2+10-m_nNumColsUsed+nCol)->SetText(nNum);
	else
		m_pSrcTable->GetTextField(nLine, 2+10-m_nNumColsUsed+nCol)->SetText("—");
}

/******************************************************************************/

void CSrcRaceGroupResults::SetRacerResults(int nLine, int nTotNumLaps, int nTotNumHdts, int nFmt) {
	char temp[128];
	m_pSrcTable->GetTextField(nLine, 12)->SetText(LapsFormat(nTotNumLaps, nTotNumHdts, nFmt, temp));
}

/******************************************************************************/

void CSrcRaceGroupResults::SetNumColsUsed(int nNum) {
	m_nNumColsUsed=nNum;
}

/******************************************************************************/

void CSrcRaceGroupResults::Reset(int nLine) {
	for (int a=0; a<12; a++) {
		m_pSrcTable->GetTextField(nLine, a)->SetText(" ");
	}
}

/******************************************************************************/

void CSrcRaceGroupResults::SetLines(CTIMeasuredRace *pRace) {
	for (int a=0; a<10; a++) Reset(a);
	int nIdx, nRide;
	ITERATE(pRace->m_pArrSortedEntities, nIdx) {
		CTIMeasuredEntity *pEntity = DC_ME(pRace->m_pArrSortedEntities[nIdx]);
		SetRacerData(nIdx, nIdx+1, pEntity->GetEntityName(CTIRaceEntity::NICKNAME));
		CEventData *pEvent = pEntity->GetCurrentEvent();
		SetNumColsUsed(pEvent->m_pArrRides.GetSize());
		pEvent->GetTotalLaps();
		SetRacerResults(nIdx, pEvent->GetTotalLaps(), pEvent->GetTotalHdts(), pRace->m_nDivisionType);
		ITERATE(pEvent->m_pArrRides, nRide) {
			int nToReport = pEvent->m_pArrRides[nRide]->m_nTrackAssigned == -1 
				? -1 : pEvent->m_pArrRides[nRide]->m_dwNumLaps;
			SetLaps(nIdx, nRide, nToReport);
		}
	}
}

/******************************************************************************/

CSrcRaceGroupResults::~CSrcRaceGroupResults()
{
}

/*********************************************************************** eof **/
