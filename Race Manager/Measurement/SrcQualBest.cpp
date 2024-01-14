/******************
*                 *  SrcQualBest.cpp
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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  CSrcBest
*
*******************************************************************************/

CSrcBest::CSrcBest(CWslWatchSource *pOwner, int nMode, int nNumRacers) : CWslWatchSource(pOwner)
{
	m_pSrcTable=NULL;
	// SetMode(nMode, nNumRacers);
}

/******************************************************************************/

void CSrcBest::SetMode(int nMode, int nNumRacers)
{
	if (m_pSrcTable) DestroyAll();
	m_pSrcTable=new CSrcNiceTable(this);

	int nGenerated=0;
	int nFont;

	switch (nMode) {
	case QUAL_BEST_SMALL:

		m_pSrcTable->SetParams(405, 128, 394, 264, 25, 0);

		SE(C_WHITE, 2, W_ALIGN_LEFT, C_BLUE1, C_WHITE, TRUE);
		RW(NEW_TEXT(" ", 404, 88, 396, 304, this));

		SE(C_WHITE, 3, -1, -1, -1, FALSE);
		RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_CURRENT_TOP10), 416, 96, 200, 24, this));

		m_pSrcTable->RegisterColumn(512, W_ALIGN_LEFT, W_TYPE_DUMMY, 0, 0, 0, TRUE);
		m_pSrcTable->RegisterColumn(32, W_ALIGN_RIGHT, W_TYPE_TEXT, C_BLUE3, 0, 3, FALSE);
		m_pSrcTable->RegisterColumn(8, W_ALIGN_LEFT, W_TYPE_DUMMY, 0, 0, 0, FALSE);
		m_pSrcTable->RegisterColumn(256, W_ALIGN_LEFT, W_TYPE_TEXT, C_WHITE, 0, 3, FALSE);
		m_pSrcTable->RegisterColumn(64, W_ALIGN_RIGHT, W_TYPE_TEXT, C_BLUE3, 0, 3, FALSE);
		m_pSrcTable->RegisterColumn(512, W_ALIGN_LEFT, W_TYPE_DUMMY, 0, 0, 0, TRUE);
		m_pSrcTable->BuildFields(10);

		break;

	case QUAL_BEST_LARGE:

		SE(C_WHITE, 2, W_ALIGN_LEFT, C_BLUE1, C_WHITE, TRUE);
		RW(NEW_TEXT(" ", 0, 88, 800, 312, this));

		m_pSrcTable->SetParams(2, 89, 796, 310, 20, 0);

		m_pSrcTable->RegisterColumn(16, W_ALIGN_LEFT, W_TYPE_DUMMY, 0, 0, 0, FALSE);
		m_pSrcTable->RegisterColumn(32, W_ALIGN_RIGHT, W_TYPE_TEXT, C_BLUE3, 0, 2, FALSE);
		m_pSrcTable->RegisterColumn(8, W_ALIGN_LEFT, W_TYPE_DUMMY, 0, 0, 0, FALSE);
		m_pSrcTable->RegisterColumn(800, W_ALIGN_LEFT, W_TYPE_TEXT, C_WHITE, 0, 2, TRUE);
		m_pSrcTable->RegisterColumn(64, W_ALIGN_RIGHT, W_TYPE_TEXT, C_BLUE3, 0, 2, FALSE);
		m_pSrcTable->RegisterColumn(32, W_ALIGN_LEFT, W_TYPE_DUMMY, 0, 0, 0, FALSE);
		nGenerated=m_pSrcTable->BuildFields(nNumRacers);

		break;

	case QUAL_BEST_RESULTS:
		SE(C_WHITE, 1, W_ALIGN_LEFT, C_BLUE1, C_WHITE, TRUE);
		RW(NEW_TEXT(" ", 0, 88, 800, 512 - 32, this));

		if (nNumRacers <= 30) {
			m_pSrcTable->SetParams(2, 89, 796, 510 - 32, 20, 0);
			nFont = 2;
		}
		else {
			m_pSrcTable->SetParams(2, 89, 796, 510 - 32, 17, 0);
			nFont = 1;
		}

		m_pSrcTable->RegisterColumn(16, W_ALIGN_LEFT, W_TYPE_DUMMY, 0, 0, 0, FALSE);
		m_pSrcTable->RegisterColumn(32, W_ALIGN_RIGHT, W_TYPE_TEXT, C_BLUE3, 0, nFont, FALSE);
		m_pSrcTable->RegisterColumn(8, W_ALIGN_LEFT, W_TYPE_DUMMY, 0, 0, 0, FALSE);
		m_pSrcTable->RegisterColumn(800, W_ALIGN_LEFT, W_TYPE_TEXT, C_WHITE, 0, nFont, TRUE);
		m_pSrcTable->RegisterColumn(64, W_ALIGN_RIGHT, W_TYPE_TEXT, C_BLUE3, 0, nFont, FALSE);
		m_pSrcTable->RegisterColumn(32, W_ALIGN_LEFT, W_TYPE_DUMMY, 0, 0, 0, FALSE);
		nGenerated=m_pSrcTable->BuildFields(nNumRacers);

		break;
	}

	m_nMode = nMode;


	RegisterWatch(m_pSrcTable);
}

/******************************************************************************/

CSrcBest::~CSrcBest() {
}

/******************************************************************************/

void CSrcBest::SetLine(int nIdx, const char *pcszRacerName, CString s) {
	CWslWatchText *pText;

	if (nIdx>=m_pSrcTable->m_nMaxNumItems) return;

	pText=m_pSrcTable->GetTextField(nIdx, 0);
	if (!strlen(pcszRacerName)) pText->SetText("");
	else pText->SetText(nIdx+1);
	pText=m_pSrcTable->GetTextField(nIdx, 1);
	pText->SetText(pcszRacerName);
	pText=m_pSrcTable->GetTextField(nIdx, 2);
	pText->SetText(LPCTSTR(s));
}

/******************************************************************************/

void CSrcBest::QualSetLines(CTIMeasuredRace *pTIRace) {

	char info[256];
	int nMax;
	BOOL bDispActionedOnly;

	switch(m_nMode) {
	case QUAL_BEST_SMALL:
		nMax = min(10, pTIRace->m_pArrSortedEntities.GetSize());
		bDispActionedOnly = TRUE;
		break;
	case QUAL_BEST_LARGE:
		nMax = pTIRace->m_pArrSortedEntities.GetSize();
		bDispActionedOnly = FALSE;
		break;
	case QUAL_BEST_RESULTS:
		nMax = pTIRace->m_pArrSortedEntities.GetSize();
		bDispActionedOnly = FALSE;
		break;
	}

	CTIMeasuredEntity *pEntity;
	DWORD dw1, dw2;

	for (int a=0; a<m_pSrcTable->m_nMaxNumItems; a++) {
		SetLine(a, "  ", CString(""));

		pEntity = NULL;
		if (a<nMax) pEntity = dynamic_cast<CTIMeasuredEntity*>(pTIRace->m_pArrSortedEntities[a]);

		BOOL bAllowed = pEntity && ((a<nMax) || (pEntity->m_bWasAction && bDispActionedOnly) || !bDispActionedOnly);
		if (bAllowed) {

			if (pEntity->m_bWasAction && bDispActionedOnly || !bDispActionedOnly) {
				switch (pTIRace->GetRS()->m_nPrimaryEval) {
				case RS_EVAL_NUM_LAPS:
					pEntity->QualGetNumLaps(&dw1, &dw2);
					LapsFormat(dw1, dw2, pTIRace->m_nDivisionType, info);
					break;
				case RS_EVAL_FIRST_TIME:
					pEntity->QualGetBestTimes(&dw1, &dw2);
					TimeFormat(dw1, info, 3);
					break;
				}
				SetLine(a, pTIRace->m_pArrSortedEntities[a]->GetEntityName(CTIRaceEntity::NICKNAME), info);
			}
		}
	}
}

/*******************************************************************************
*
*  CSrcNextRacers
*
*******************************************************************************/

CSrcNextRacers::CSrcNextRacers(CWslWatchSource *pOwner) : CWslWatchSource(pOwner){
	SE(C_WHITE, 2, W_ALIGN_LEFT, C_BLUE1, C_WHITE, TRUE);
	RW(NEW_TEXT(" ", 404, 400, 396, 80, this));

	SE(C_WHITE, 3, W_ALIGN_LEFT, C_BLUE1, C_BLUE2, FALSE);
	RW(NEW_TEXT(GetIds(IDS_RACE_CAPTION_NEXT_RACER), 416, 408, 200, 24, this));

	SE(C_WHITE, 3, W_ALIGN_LEFT, C_BLUE1, C_BLUE2, FALSE);

	int a;
	for (a=0; a<10; a++) RW(NEW_TEXT("", 416, 232, 1, 1, this));
	for (a=0; a<10; a++) RW(new CWslWatchColorBox((COLORREF)-1, 416, 232, 1, 1, this));
}

/******************************************************************************/

CSrcNextRacers::~CSrcNextRacers() {
	CWslWatchSource::~CWslWatchSource();
}

/******************************************************************************/

void CSrcNextRacers::SetLine(int nIdx, const char *pcszRacerName) {
	GetText(nIdx+2)->SetText(pcszRacerName);
}

/******************************************************************************/

void CSrcNextRacers::Reset() {
	for (int a=0; a<10; a++) SetLine(a, " ");
}

/******************************************************************************/

void CSrcNextRacers::SetMode(int nNewMode) {
	int a;
	switch (nNewMode) {
	case NEXT_MODE_SMALL:
		GetText(1)->SetText(GetIds(IDS_RACE_CAPTION_NEXT_RACER));
		GetText(0)->ChangePos(404, 400, 396, 80);
		GetText(1)->ChangePos(416, 408, 200, 24);
		for (a=2; a<22; a++) {
			GetText(a)->ChangePos(405,401,1,1);
			GetText(a)->Visibility(SW_HIDE);
		}
		GetText(2)->ChangeAlignMode(W_ALIGN_RIGHT);
		GetText(2)->ChangePos(416, 432, 372, 40);
		GetText(2)->Visibility(SW_SHOW);
		GetText(2)->ChangeFont(5);
		break;
	case NEXT_MODE_MORE:
		GetText(1)->SetText(GetIds(IDS_RACE_CAPTION_NEXT_RACERS));
		GetText(0)->ChangePos(254, 408, 546, 160);
		GetText(1)->ChangePos(268, 416, 200, 24);
		for (a=2; a<22; a++) {
			GetText(a)->ChangePos(255,409,1,1);
			GetText(a)->Visibility(SW_HIDE);
		}
		for (a=2; a<7; a++) {
			GetText(a)->ChangePos(288, 444+(a-2)*22, 249, 22);
			GetText(a)->Visibility(SW_SHOW);
			GetText(a)->ChangeAlignMode(W_ALIGN_LEFT);
			GetText(a)->ChangeFont(3);
		}
		for (a=2; a<7; a++) {
			GetText(a+5)->ChangePos(537, 444+(a-2)*22, 256, 22);
			GetText(a+5)->Visibility(SW_SHOW);
			GetText(a+5)->ChangeAlignMode(W_ALIGN_LEFT);
			GetText(a+5)->ChangeFont(3);
		}
		break;
	case NEXT_MODE_GROUP:
		GetText(1)->SetText(GetIds(IDS_RACE_CAPTION_RACERS_NEXT_GROUP));
		GetText(0)->ChangePos(254, 408, 546, 160);
		GetText(1)->ChangePos(268, 416, 200, 24);
		for (a=2; a<7; a++) {
			GetText(a)->ChangePos(288, 444+(a-2)*22, 229, 22);
			GetText(a)->Visibility(SW_SHOW);
			GetText(a)->ChangeAlignMode(W_ALIGN_LEFT);
			GetText(a)->ChangeFont(3);

			GetBox(a+10)->ChangePos(268, 444+(a-2)*22+6, 16, 16);
			GetBox(a+10)->Visibility(SW_SHOW);
		}
		for (a=2; a<7; a++) {
			GetText(a+5)->ChangePos(537, 444+(a-2)*22, 229, 22);
			GetText(a+5)->Visibility(SW_SHOW);
			GetText(a+5)->ChangeAlignMode(W_ALIGN_LEFT);
			GetText(a+5)->ChangeFont(3);

			GetBox(a+15)->ChangePos(517, 444+(a-2)*22+6, 16, 16);
			GetBox(a+15)->Visibility(SW_SHOW);
		}
		break;
	}
}

/******************************************************************************/

void CSrcNextRacers::SetLines(int nNewMode, CTIMeasuredRace *pTIRace) {
	CTIRaceEntity *pEntities[10];
	char temp[1024];
	int a;
	switch (nNewMode) {
	case NEXT_MODE_SMALL:
		pTIRace->QualGetNextRacers(pEntities, 4);
		if (pEntities[1]) {
			wsprintf(temp, "%s", pEntities[1]->GetEntityName(CTIRaceEntity::NICKNAME));
			GetText(2)->SetText(temp);
		}
		else GetText(2)->SetText("—");
		break;
	case NEXT_MODE_MORE:
		pTIRace->QualGetNextRacers(pEntities, 10);
		for (a=0; a<10; a++) {
			if (pEntities[a]) {
				wsprintf(temp, "%s", pEntities[a]->GetEntityName(CTIRaceEntity::NICKNAME));
				SetLine(a, temp);
			}
			else SetLine(a, " ");
		}
		break;
	case NEXT_MODE_GROUP:
		break;
	}
}

/*********************************************************************** eof **/
