/******************
*                 *  TIMeasuredEntity.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  17-AUG-2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"
#include "TIMeasuredEntity.h"
#include "PgEntityDebug.h"
#include "version.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  CONTRUCTION / DESTRUCTION
*
*******************************************************************************/

BEGIN_MESSAGE_MAP(CTIMeasuredEntity, CTIRaceEntity)
	//{{AFX_MSG_MAP(CTIRaceEntity)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/******************************************************************************/

IMPLEMENT_SERIAL(CTIMeasuredEntity, CTIRaceEntity, 1)

/******************************************************************************/

CTIMeasuredEntity::CTIMeasuredEntity()
{
	QualReset();
	int a;
	for (a=0; a<4; a++) m_pArrEvents.Append(new CEventData);
	for (a=0; a<8; a++) m_nDataAreValid[a]=FALSE;
	m_nCurrentEvent = -1;
	m_nCurrentRide = 0;
}

/******************************************************************************/

CTIMeasuredEntity::CTIMeasuredEntity(DWORD dwId) : CTIRaceEntity(dwId)
{
	QualReset();
	for (int a=0; a<4; a++) m_pArrEvents.Append(new CEventData);
	for (a=0; a<8; a++) m_nDataAreValid[a]=FALSE;
	m_nCurrentEvent = -1;
	m_nCurrentRide = 0;
}

/******************************************************************************/

CTIMeasuredEntity::~CTIMeasuredEntity()
{
	int nIdx;
	ITERATE(m_pArrEvents, nIdx) { delete m_pArrEvents[nIdx]; }
}

/******************************************************************************/

void CTIMeasuredEntity::Serialize(CArchive &ar) {
	CTIRaceEntity::Serialize(ar);

	CEventData *pEventData;
	DWORD dw, dwVersion;
	int a;

	CString s;

	// --------------------------------------------------------------------------
	// LOAD

	if (ar.IsLoading()) {
		ITERATE(m_pArrEvents, a) { delete m_pArrEvents[a]; }
		m_pArrEvents.RemoveAll();

		ar >> dwVersion;

		ar >> m_bWasAction;
		ar >> m_nCurrentEvent;
		ar >> m_dwTimeElapsed;
		for (a=0; a<8; a++) ar >> m_nDataAreValid[a];

		ar >> m_dwArrNumLaps;
		ar >> m_dwArrNumHdts;
		ar >> m_dwArrTimes;

		ar >> dw;
		for (a=0; a<dw; a++) {
			ar >> pEventData;
			m_pArrEvents.Append(pEventData);
		}

		if (dwVersion >= 0x20000827) {
			ar >> m_nCurrentRide;
		}

		if (dwVersion >= 0x20000923) {
			ar >> m_dwPrognosis;
		}
		
	}

	// --------------------------------------------------------------------------
	// STORE

	if (ar.IsStoring()) {
		ar << (DWORD) MEASUREMENT_FILES_VERSION;

		ar << m_bWasAction;
		ar << m_nCurrentEvent;
		ar << m_dwTimeElapsed;
		for (a=0; a<8; a++) ar << m_nDataAreValid[a];

		ar << m_dwArrNumLaps;
		ar << m_dwArrNumHdts;
		ar << m_dwArrTimes;

		DWORD dwNumEvents = m_pArrEvents.GetSize();
		ar << dwNumEvents;

//		s.Format("Storing %d events", dwNumEvents);
//		AfxMessageBox(s);
		for (a=0; a<dwNumEvents; a++) ar << m_pArrEvents[a];
		ar << m_nCurrentRide;
		ar << m_dwPrognosis;

//		CPropertySheet p;
//		p.SetTitle("DEBUG");
//		p.AddPage(new CPgEntityDebug(this));
//		p.DoModal();
	}
}

/*******************************************************************************
*
*  QUALIFICATION
*
*******************************************************************************/

void CTIMeasuredEntity::QualBeginTransaction() {
	m_nQualTransPointer = m_dwArrTimes.GetSize();
	m_bQualTransWasAction = m_bWasAction;
	m_nQualTransCurrentRide = m_nCurrentRide;
   m_dwQualTransTimeElapsed = m_dwTimeElapsed;
}

/******************************************************************************/

void CTIMeasuredEntity::QualRollback() {
	m_dwArrTimes.SetSize(m_nQualTransPointer);
	m_bWasAction = m_bQualTransWasAction;
	m_nCurrentRide = m_nQualTransCurrentRide;
   m_dwTimeElapsed = m_dwQualTransTimeElapsed;
}

/******************************************************************************/

void CTIMeasuredEntity::QualStoreTime(DWORD dwTime) {
	if (dwTime==0) return;
	TRACE("ENTITY: STORING TIME %d", dwTime);
	m_dwArrTimes.Append(dwTime);
	m_bWasAction=TRUE;
}

/******************************************************************************/

void CTIMeasuredEntity::QualStoreLaps(DWORD dwNumLaps, DWORD dwNumHdts)
{
	TRACE("ENTITY: STORING LAP");
	m_dwArrNumLaps.Append(dwNumLaps);
	m_dwArrNumHdts.Append(dwNumHdts);
	if (dwNumLaps || dwNumHdts) m_bWasAction=TRUE;
}

/******************************************************************************/

void CTIMeasuredEntity::QualGetBestTimes(DWORD *dw1, DWORD *dw2)
{
	int nIdx;
	*dw1=1024*1024; *dw2=1024*1024;
	ITERATE(m_dwArrTimes, nIdx) {
		if (m_dwArrTimes[nIdx] < *dw1) {
			*dw2=*dw1;
			*dw1=m_dwArrTimes[nIdx];
		}
	}
	if (*dw1==1024*1024) *dw1=0;
	if (*dw2==1024*1024) *dw2=0;
}

/******************************************************************************/

void CTIMeasuredEntity::QualGetNumLaps(DWORD *dwLaps, DWORD *dwHdts)
{
	int nIdx;
	*dwLaps=0;
	*dwHdts=0;
	ITERATE(m_dwArrNumLaps, nIdx) *dwLaps+=m_dwArrNumLaps[nIdx];
	ITERATE(m_dwArrNumHdts, nIdx) *dwHdts+=m_dwArrNumHdts[nIdx];
}

/******************************************************************************/

int CTIMeasuredEntity::GetCurrentQualRank()
{
   if (!m_bWasAction) return 0;
   CTIMeasuredRace *pRace = GetRace();
   int nIdx;
   ITERATE (pRace->m_pArrSortedEntities, nIdx) {
      if (pRace->m_pArrSortedEntities[nIdx] == this) return nIdx+1;
   }
   return 0;
}

/******************************************************************************/

int CTIMeasuredEntity::QualCompareWith(CTIMeasuredEntity *pOther, int nEvalMethod)
{
	DWORD dw1, dw2, dwo1, dwo2, t1, t2, to1, to2;
	if (pOther->m_bWasAction && !m_bWasAction) return -1;
	if (!pOther->m_bWasAction && m_bWasAction) return 1;
	if (!pOther->m_bWasAction && !m_bWasAction) return 0;
	switch (nEvalMethod) {
	case RS_EVAL_NUM_LAPS:
		QualGetNumLaps(&dw1, &dw2);
		QualGetBestTimes(&t1, &t2);
		pOther->QualGetNumLaps(&dwo1, &dwo2);
		pOther->QualGetBestTimes(&to1, &to2);

		if (t1 == 0) t1 = 1024*1024;
		if (t2 == 0) t2 = 1024*1024;
		if (to1 == 0) to1 = 1024*1024;
		if (to2 == 0) to2 = 1024*1024;

		if (dw1 > dwo1) return 1;
		if (dw1 == dwo1) {
			if (dw2 > dwo2) return 1;
			if (dw2 == dwo2) {
				if (t1 < to1) return 1;
				if (t1 == to1) {
					if (t2 < to2) return 1;
					if (t2 == to2) return 0;
					return -1;
				}
				return -1;
			}
			return -1;
		}
		return -1;
		break;
	case RS_EVAL_FIRST_TIME:
		QualGetBestTimes(&dw1, &dw2);
		pOther->QualGetBestTimes(&dwo1, &dwo2);

		if (dw1 == 0) dw1 = 10000000;
		if (dw2 == 0) dw2 = 10000000;
		if (dwo1 == 0) dwo1 = 10000000;
		if (dwo2 == 0) dwo2 = 10000000;

		if (dw1 < dwo1) return 1;
		if (dw1 == dwo1) {
			if (dw2 < dwo2) return 1;
			if (dw2 == dwo2) return 0;
			return -1;
		}
		return -1;
		break;
	}
	return FALSE;
}

/******************************************************************************/

void CTIMeasuredEntity::QualReset() {
	m_bWasAction = FALSE;
	m_dwArrTimes.SetSize(0);
	m_dwArrNumLaps.SetSize(0);
	m_dwArrNumHdts.SetSize(0);
	m_dwTimeElapsed = 0;
	m_nCurrentRide = 0;
}

/******************************************************************************/

CString CTIMeasuredEntity::QualReport(int nEvalMethod) {
	char temp[16];
	DWORD dw1, dw2;
	int a;
	switch(nEvalMethod) {
	case RS_EVAL_NUM_LAPS:
		QualGetNumLaps(&dw1, &dw2);
		wsprintf(temp, "%d.%2d", dw1, dw2);
		for (a=0; a<strlen(temp); a++) if (temp[a]==32) temp[a]='0';
		break;
	case RS_EVAL_FIRST_TIME:
		QualGetBestTimes(&dw1, &dw2);
		TimeFormat(dw1, temp, 3);
		break;
	}
	return temp;
}

/*******************************************************************************
*
*  OVERALL
*
*******************************************************************************/

void CTIMeasuredEntity::AdvanceToEvent(int nIdx) {
	m_bWasAction = FALSE;
	m_nCurrentEvent = GetEventIdx(nIdx);
	m_nDataAreValid[nIdx] = 1;
	m_pED = m_pArrEvents[m_nCurrentEvent];
	m_dwTimeElapsed=0;
	m_nCurrentOrder=9999;
//	m_dwPrognosis=0;
}

/******************************************************************************/

void CTIMeasuredEntity::Reset() {

	CTIRaceEntity::Reset();

	QualReset();
	int nIdx;
	ITERATE(m_pArrEvents, nIdx) m_pArrEvents[nIdx]->Reset();
	for (int a=0; a<8; a++) m_nDataAreValid[a]=FALSE;

	m_dwPrognosis=0;
	m_nCurrentEvent = -1;
}

/******************************************************************************/

void CTIMeasuredEntity::ResetIntoEvent(int nIdx) {
	if (nIdx == -1) Reset();
	ASSERT(nIdx<4);
	for (int i=nIdx; i<4; i++) {
		delete m_pArrEvents[i];
		m_pArrEvents[i]=new CEventData;
	}
	m_nCurrentEvent = nIdx;
}

/******************************************************************************/

void CTIMeasuredEntity::ResetBeforeRide(int nIdx) {
	int nNumSoFar = GetCurrentEvent()->m_pArrRides.GetSize();
	ASSERT(nIdx>=0 && nIdx<nNumSoFar);
	for (int i=nIdx; i<nNumSoFar; i++) {
		delete GetCurrentEvent()->m_pArrRides[i];
	}
	GetCurrentEvent()->m_pArrRides.SetSize(nIdx);
}

/******************************************************************************/

CTIMeasuredRace *CTIMeasuredEntity::GetRace() {
	CTIMeasuredRace *pRace;
	pRace = dynamic_cast<CTIMeasuredRace*>(GetParent());
	ASSERT(pRace);
	return pRace;
}

/*******************************************************************************
*
*  RACE
*
*******************************************************************************/

int CTIMeasuredEntity::RaceCompareWith(CTIMeasuredEntity *pOther) {

	ASSERT(pOther);

	// --------------------------------------------------------------------------
	// STEP ONE: Gather data

	DWORD l1 = GetCurrentEvent()->GetTotalLaps();
	DWORD h1 = GetCurrentEvent()->GetTotalHdts();
	DWORD l2 = pOther->GetCurrentEvent()->GetTotalLaps();
	DWORD h2 = pOther->GetCurrentEvent()->GetTotalHdts();

	// --------------------------------------------------------------------------
	// STEP TWO: Compare data

	if (l1>l2) return 1;
	if (l1<l2) return -1;
	if (h1>h2) return 1;
	if (h1<h2) return -1;

//	TRACE("SAME !!\n");

	return 0;
}

/******************************************************************************/

int CTIMeasuredEntity::RaceFinalCompareWith(CTIMeasuredEntity *pOther) {

	ASSERT(pOther);

	if (m_nCurrentEvent > pOther->m_nCurrentEvent) return 1;
	if (m_nCurrentEvent < pOther->m_nCurrentEvent) return -1;

	// --------------------------------------------------------------------------
	// SUBSTEP ONE: Gather data

	DWORD l1 = GetCurrentEvent()->GetTotalLaps();
	DWORD h1 = GetCurrentEvent()->GetTotalHdts();
	DWORD l2 = pOther->GetCurrentEvent()->GetTotalLaps();
	DWORD h2 = pOther->GetCurrentEvent()->GetTotalHdts();

	// --------------------------------------------------------------------------
	// SUBSTEP TWO: Compare data

	if (l1>l2) return 1;
	if (l1<l2) return -1;
	if (h1>h2) return 1;
	if (h1<h2) return -1;

	return 0;
}

/******************************************************************************/

CEventData *CTIMeasuredEntity::GetCurrentEvent() {
	ASSERT(m_nCurrentEvent >= 0);
	CEventData *pData = m_pArrEvents[m_nCurrentEvent];
	ASSERT(pData);
	return pData;
}

/*******************************************************************************
*
*  DEBUG
*
*******************************************************************************/

void CTIMeasuredEntity::AddDebugPropertySheet(CWsfPropertySheet &s) {
	CPgEntityDebug *p = new CPgEntityDebug(this);
	s.AddPage(p);
}

/*********************************************************************** eof **/
