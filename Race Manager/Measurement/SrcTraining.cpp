/******************
*                 *  SrcTraining.cpp
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

CSrcTraining::CSrcTraining(CWslWatchSource *pOwner)
{
	SE(C_WHITE, 3, W_ALIGN_LEFT, C_BLUE1, C_WHITE, TRUE);
	RW(NEW_TEXT(" ", 0, 88, 800, 392, this));

	SE(C_WHITE, 1, W_ALIGN_LEFT, C_BLUE1, C_WHITE, FALSE);
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_LANE),       9+8,  92,  54, 26, this));
	SE(C_WHITE, 1, W_ALIGN_RIGHT, C_BLUE1, C_WHITE, FALSE);
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_TOTAL),  66+8,  92,  50, 26, this));
	SE(C_WHITE, 1, W_ALIGN_LEFT, C_BLUE1, C_WHITE, FALSE);
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_BEST_LAP),  148+8,  92,  60, 26, this));
	SE(C_WHITE, 1, W_ALIGN_LEFT, C_BLUE1, C_WHITE, FALSE);
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_MAIN_LAST_LAPS),  238+8,  92,  80, 26, this));

	m_pSrcTable=new CSrcNiceTable(this);
	m_pSrcTable->SetParams(1, 120, 798, 360, 35, 0);
	m_pSrcTable->RegisterColumn(8, W_ALIGN_RIGHT, W_TYPE_DUMMY, 0, 0, 0, FALSE);
	m_pSrcTable->RegisterColumn(24, W_ALIGN_RIGHT, W_TYPE_TEXT,  C_WHITE, 0, 3, FALSE);
	m_pSrcTable->RegisterColumn(6, W_ALIGN_RIGHT, W_TYPE_DUMMY, 0, 0, 0, FALSE);
	m_pSrcTable->RegisterColumn(24, W_ALIGN_RIGHT, W_TYPE_COLORBOX, 0, 0, 0, FALSE);
	m_pSrcTable->RegisterColumn(12, W_ALIGN_RIGHT, W_TYPE_DUMMY, 0, 0, 0, FALSE);
	m_pSrcTable->RegisterColumn(50, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);
	m_pSrcTable->RegisterColumn(80, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);
	m_pSrcTable->RegisterColumn(12, W_ALIGN_RIGHT, W_TYPE_DUMMY, 0, 0, 0, FALSE);
	m_pSrcTable->RegisterColumn(78, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);
	m_pSrcTable->RegisterColumn(78, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);
	m_pSrcTable->RegisterColumn(78, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);
	m_pSrcTable->RegisterColumn(78, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);
	m_pSrcTable->RegisterColumn(78, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);
	m_pSrcTable->RegisterColumn(78, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);
	m_pSrcTable->RegisterColumn(78, W_ALIGN_RIGHT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);
	m_pSrcTable->RegisterColumn(500, W_ALIGN_LEFT, W_TYPE_DUMMY, 0, 0, 0, TRUE);
	m_pSrcTable->BuildFields(10);

	RW(m_pSrcTable);

	for (int a=0; a<8; a++) {
		ResetLine(a);
	}
}

/******************************************************************************/

void CSrcTraining::SetLaneData(int nLine, int nTrack, COLORREF crColor)
{
	int nNumLanes = WsfGetCApp()->GetNumLanes();
	if (nLine >= nNumLanes) return; 

	if (nTrack) m_pSrcTable->GetTextField(nLine, 0)->SetText(nTrack);
	else m_pSrcTable->GetTextField(nLine, 0)->SetText(" ");
	m_pSrcTable->GetColorBox(nLine, 1)->SetColor(crColor);
}

/******************************************************************************/

void CSrcTraining::DisplayLap(int nLine, DWORD dwTime)
{
	// Let's make us sure that no other than connected lanes are available
	int nNumLanes = WsfGetCApp()->GetNumLanes();
	if (nLine >= nNumLanes) return; 

	if (dwNumLaps[nLine]>1) {
		// Shift
		for (int a=15; a>=1; a--) dwTimes[nLine][a]=dwTimes[nLine][a-1];
		// Store
		dwTimes[nLine][0] = dwTime;
		// Store best lap
		if (dwBestLaps[nLine]==0) dwBestLaps[nLine]=dwTime;
		else dwBestLaps[nLine] = min(dwBestLaps[nLine], dwTime);
		// Call the updating member
		UpdateWatches(nLine);
	}
}

/******************************************************************************/

void CSrcTraining::ResetLine(int nLine)
{
	dwBestLaps[nLine]=0;
	for (int a=0; a<16; a++) dwTimes[nLine][a]=0;
	dwNumLaps[nLine]=0;
   CallEBUpdateLaps(nLine+1, 0);
	UpdateWatches(nLine);
}

/******************************************************************************/

void CSrcTraining::CountLap(int nLine)
{
	int nNumLanes = WsfGetCApp()->GetNumLanes();
	if (nLine >= nNumLanes) return; 

	dwNumLaps[nLine]++;
   CallEBUpdateLaps(nLine+1, dwNumLaps[nLine]);
	UpdateWatches(nLine);
}

/******************************************************************************/

void CSrcTraining::UpdateWatches(int nLine)
{
	int nNumLanes = WsfGetCApp()->GetNumLanes();

	char temp[1024];
	if (dwNumLaps[nLine]) m_pSrcTable->GetTextField(nLine, 2)->SetText(dwNumLaps[nLine]);
	else {
		if (nLine >= nNumLanes) m_pSrcTable->GetTextField(nLine, 2)->SetText(" ");
		else m_pSrcTable->GetTextField(nLine, 2)->SetText("0");
	}

	CWslWatchText *pText=m_pSrcTable->GetTextField(nLine, 3);
	if (pText) {
		if (dwBestLaps[nLine])
		{
			TimeFormat(dwBestLaps[nLine], temp, 3);
			pText->SetText(temp);
		}
		else pText->SetText(" ");
	}

	for (int a=0; a<16; a++) {
		CWslWatchText *pText=m_pSrcTable->GetTextField(nLine, 4+a);
		if (pText) {
			if (dwTimes[nLine][a]) {
				TimeFormat(dwTimes[nLine][a], temp, 3);
				pText->SetText(temp);
			}
			else pText->SetText(" ");
		}
	}
}

/******************************************************************************/

void CSrcTraining::SetLines() {
	CApp *pApp = WsfGetCApp();
	int a;
	int nNumLanes = pApp->GetNumLanes();

	for (a=0; a<nNumLanes; a++) ResetLine(a);

	for (a=0; a<nNumLanes; a++) {
		SetLaneData(a, a+1, pApp->GetLaneColor(a));
	}
}

/******************************************************************************/

CSrcTraining::~CSrcTraining()
{
}

/*********************************************************************** eof **/
