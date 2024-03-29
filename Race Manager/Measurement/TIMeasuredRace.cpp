/******************
*                 *  TIMeasuredRace.cpp
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
#include "TIMeasuredRace.h"
#include "ScreenCfgCodes.h"
#include "version.h"
#include "DlgReturn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  INSTANCE OF THREAD VARIABLES
*
*******************************************************************************/

UINT g_dwThreadCmd;
UINT g_dwThreadKey;
HANDLE g_hEventKeyPressed;

/*******************************************************************************
*
*  CONSTRUCTORS; MFC AND WSF STUFF
*
*******************************************************************************/

BEGIN_MESSAGE_MAP(CTIMeasuredRace, CTIRace)
	//{{AFX_MSG_MAP(CTIMeasuredRace)
//	ON_COMMAND(WM_SUCCESS, OnSuccess)
//	ON_COMMAND(WM_INTERRUPTED, OnInterrupt)
	ON_COMMAND(65432, OnFinalResultsList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CTIMeasuredRace, CTIRace, 1)


/******************************************************************************/

CTIMeasuredRace::CTIMeasuredRace()
{
	CreateEvents();
	m_bNeedRecovery = FALSE;
	m_bPartEndedButNotAccepted = FALSE;
	m_nActiveGroup = 0;
	m_bPauseBetweenRides = 0;

//	AfxMessageBox("Dear programmer,\n\nI really admire that you dare to call this constructor\nbut now please apologize me, I'm gonna crash.");
//	ASSERT(FALSE);
}

/******************************************************************************/

CTIMeasuredRace::CTIMeasuredRace(CSetupTrack *pSetupTrack) : CTIRace(pSetupTrack)
{
	CreateEvents();

/*	m_nDivisionType = pRM->m_nDivisionType; */
//	for (int a=0; a<8; a++) {
//		m_nLaneColor[a] = pRM->GetLaneColor(a);
//		m_nLaneColor[a] = pSetupTrack->m_nLaneColor[a];
//	}
/*	m_sTrackName = pRM->m_strTrackName;
	m_sTrackOwner = pRM->m_strOwner;
	m_bNeedRecovery = FALSE;*/

	m_bPartEndedButNotAccepted = FALSE;
}

/******************************************************************************/

CTIMeasuredRace::~CTIMeasuredRace()
{
	DestroyEvents();
}

/******************************************************************************/

void CTIMeasuredRace::Serialize(CArchive &ar) {

	CTIRace::Serialize(ar);
	DWORD dwVersion;

	// --------------------------------------------------------------------------

	if (ar.IsStoring()) {
		ar << (DWORD) MEASUREMENT_FILES_VERSION;
		ar << m_bNeedRecovery;
		ar << m_bPartEndedButNotAccepted;
		ar << m_nQualRacer;
		ar << m_nQualRide;
		ar << m_nQualAttempt;
		ar << m_nCurrentRide;
	}

	// --------------------------------------------------------------------------

	if (ar.IsLoading()) {
		ar >> dwVersion;
		ar >> m_bNeedRecovery;
		ar >> m_bPartEndedButNotAccepted;
		ar >> m_nQualRacer;
		ar >> m_nQualRide;
		ar >> m_nQualAttempt;
		ar >> m_nCurrentRide;
	}
}

/*******************************************************************************
*
*  COMMON STUFF
*
*******************************************************************************/

CString CTIMeasuredRace::GetRacePartResultSheetTitle(int nRacePart) const
{
	CString s;

	switch (nRacePart)
	{
	case RACE_PART_NONE		: s = "Tvorba";                 break;
	case RACE_PART_QUAL		: s = "Qualification Results";   break;
	case RACE_PART_CONSI	: s = "Results of Heats";      break;
	case RACE_PART_QUATER	: s = "Quarterfinal Results";   break;
	case RACE_PART_SEMI		: s = "Semifinal Results";    break;
	case RACE_PART_FINAL	: s = "Final Results";        break;
	case RACE_PART_DONE		: s = "Ukon�en";                break;
	}

	

	return s;
}

/******************************************************************************/

int CTIMeasuredRace::GetEntityIdx(CTIMeasuredEntity *pEntity) {
	int nIdx=0;
	CTIMeasuredEntity *p = DC_ME(GetFirstEntity());
	while (p && p != pEntity) {
		p = DC_ME(p->GetNextSibling());
		nIdx++;
	}
	if (!p) return -1;
	return nIdx;
}

/******************************************************************************/

CTIMeasuredEntity* CTIMeasuredRace::GetIdxEntity(int nIdx) {
	ASSERT(nIdx>=0);
	int i=0;
	CTIMeasuredEntity *p = DC_ME(GetFirstEntity());
	while (i < nIdx && p) {
		p = DC_ME(p->GetNextSibling());
		i++;
	}
	return p;
}

/******************************************************************************/

CWsfTreeItem *CTIMeasuredRace::GetResultsFolder() {
	CWsfTreeItem *pItem = GetChild();
	return pItem;
//	CWsfTreeItem *pRF = pItem->GetNextSibling();
//	return pRF;
}

/******************************************************************************/

void CTIMeasuredRace::AddResultChild(CTIResults *pResults)
{
	CWsfTreeItem *pItem = GetResultsFolder();
	CWsfTreeItem *pExisting = pItem->FindChildItem(pResults->GetTitle(), false);
	if (pExisting) pExisting->RemoveFromTree();
	pItem->AddChild(pResults);
}

/******************************************************************************/

CWsfTreeItem *CTIMeasuredRace::GetFirstEntity() {
	CWsfTreeItem *pItem = GetChild();
	while (!DC_ME(pItem) && pItem) {
		pItem = pItem->GetNextSibling();
	}
	return pItem;
}

/******************************************************************************/

void CTIMeasuredRace::OnRacePartFinished() {
		// Gather data
	if (m_nRacePart != RACE_PART_NONE) {
		CTIResults *pResults = new CTIResults(m_nRacePart);
//		GetResultsFolder()->AddChild(pResults);
		AddResultChild(pResults);
		pResults->GatherData();

		WsfGetApp()->TreeHasChanged(this, CWsfApp::ETreeChanges::GENERAL);
	}

	// Reset state variables
	m_bNeedRecovery = FALSE;
	m_bPartEndedButNotAccepted = FALSE;

	int nOldPart = m_nRacePart;

	// Call Radek
	CTIRace::OnRacePartFinished();

	if (nOldPart == RACE_PART_NONE) {
		CTIResults *pResults = new CTIResults();
		AddResultChild(pResults);
		pResults->GatherDataPartEntities();
		WsfGetApp()->TreeHasChanged(this, CWsfApp::ETreeChanges::GENERAL);
	}

	// Reset all the prognosis variables for all racers in the race.
	CEntityArray array;
	BuildEntitiesArray(array);
	int nIdx;
	ITERATE(array, nIdx) {
		array[nIdx]->m_dwPrognosis=0;
	}

	m_nActiveGroup = 0;

	WsfGetCApp()->OnRaceSave();
}

/******************************************************************************/

BOOL CTIMeasuredRace::OnRaceStart() {

	// perform some checks here and then return true if everything is ok.
	// Well, so far, everything is ok... so... where is the problem, man?

	return CTIRace::OnRaceStart();
}

/******************************************************************************/

BOOL CTIMeasuredRace::OnRaceNextPart() {

	// Call Radek's function
	BOOL bResult = CTIRace::OnRaceNextPart();
	if (!bResult) return false;

	// Close rides view before meassuring
	WsfGetApp()->CloseView(g_nRidesViewID, FALSE);

	// Flash correction during the European Championship in Zlin, 24.9.2000
	if (m_bPartEndedButNotAccepted && !m_bNeedRecovery) {
		m_bPartEndedButNotAccepted = FALSE;
	}

	// Incorrect configuration check.
	ASSERT(!(!m_bNeedRecovery && m_bPartEndedButNotAccepted));

	// If there is no need for recovery, we might launch standard question.
	if (!m_bNeedRecovery) {
		if (AfxMessageBox(IDS_OK_TO_BEGIN_NEXT_PART,
			MB_ICONQUESTION|MB_YESNO) != IDYES) return false;
	}

	// If there is a need for recovery or acceptance of the race part,
	// a dedicated dialog will appear allowing user to do many things
	if (m_bNeedRecovery || m_bPartEndedButNotAccepted) {
		if (m_nRacePart > RACE_PART_QUAL) {
			CDlgReturn hd;
			hd.m_pRace = this;
			// If the CANCEL button has been pressed, do nothing and quit.
			if (hd.DoModal()==IDCANCEL) return false;
			// If the user wanted to end race that received flag
			// m_bPartEndedButNotAccepted, call what has not been called.
			if (hd.m_bUserWantsEndRacePart) {
				OnSuccess(FALSE);
				return false;
			}
		}
//		else AfxMessageBox("Reverting to qualification");
	}

	// NOW: The variables has been changed (or left unchanged) by the dialog

	if (!m_bNeedRecovery) {
		
		// ---------------------------------------------------------------------
		// Init racers
		
		if (m_nRacePart == RACE_PART_QUAL) {
			m_nQualRacer = 0;
			m_nQualRide = 0;
			m_nQualAttempt = 0;
		}
		
		int nIdx;
		
		if (m_nRacePart > RACE_PART_QUAL) {
			// Warning: Bug source: The AdvanceToEvent is writtn
			ITERATE(m_aPartEntities, nIdx) {
				m_aPartEntities[nIdx]->AdvanceToEvent(m_nRacePart);
			}

//			CString ss;
//			ss.Format("Erasing group %d", m_nActiveGroup);

//			ITERATE(m_aGroup[m_nActiveGroup], nIdx) {
//				m_aGroup[m_nActiveGroup][nIdx]->m_dwPrognosis = 0;
//			}
		}
	}

	// -------------------------------------------------------------------------
	// Call the correct function

	if (m_nRacePart == RACE_PART_QUAL) OnRunQualification();
	else OnRunRace();

	return true;
}

/******************************************************************************/

void CreateEvents() {
	g_hEventKeyPressed = CreateEvent(NULL, FALSE, FALSE, "KBD");
}

/******************************************************************************/

void CTIMeasuredRace::DestroyEvents() {
}

/*******************************************************************************
*
*  QUALIFICATION
*
*******************************************************************************/

int CTIMeasuredRace::QualSortRacers(int nMode) {

	if (nMode == 0) {
		m_pArrSortedEntities = m_aPartEntities;
		// Retrieve RS settings
		CTIRS *pTIRS = GetRS();
		// Std. bubble sort :-)
		int nNum=m_pArrSortedEntities.GetSize();
		for (int a=0; a<nNum-1; a++) {
			for (int b=0; b<nNum-1; b++) {
				CTIMeasuredEntity *e1 = static_cast<CTIMeasuredEntity*>(m_pArrSortedEntities[b]);
				CTIMeasuredEntity *e2 = static_cast<CTIMeasuredEntity*>(m_pArrSortedEntities[b+1]);
				if (e1->QualCompareWith(e2, GetRS()->m_nPrimaryEval)==-1) {
					CTIMeasuredEntity *pSwap=m_pArrSortedEntities[b];
					m_pArrSortedEntities[b]=m_pArrSortedEntities[b+1];
					m_pArrSortedEntities[b+1]=pSwap;
				}
			}
		}
	}
	
	else {
		// Retrieve RS settings
		CTIRS *pTIRS = GetRS();
		// Std. bubble sort :-)
		int nNum=m_aPartEntities.GetSize();
		for (int a=0; a<nNum-1; a++) {
			for (int b=0; b<nNum-1; b++) {
				CTIMeasuredEntity *e1 = static_cast<CTIMeasuredEntity*>(m_aPartEntities[b]);
				CTIMeasuredEntity *e2 = static_cast<CTIMeasuredEntity*>(m_aPartEntities[b+1]);
				if (e1->QualCompareWith(e2, GetRS()->m_nPrimaryEval)==-1) {
					CTIMeasuredEntity *pSwap=m_aPartEntities[b];
					m_aPartEntities[b]=m_aPartEntities[b+1];
					m_aPartEntities[b+1]=pSwap;
				}
			}
		}
	}
	return 1;
}

/******************************************************************************/

void CTIMeasuredRace::OnRunQualification() {
	ShowFullScreen();
	SpawnQualificationThread();
		
}

/******************************************************************************/

void CTIMeasuredRace::QualLoadPosition() {
	m_nQualRacer = m_nQualRacerHld;
	m_nQualAttempt = m_nQualAttemptHld;
	m_nQualRide = m_nQualRideHld;
}

/******************************************************************************/

void CTIMeasuredRace::QualSavePosition() {
	m_nQualRacerHld = m_nQualRacer;
	m_nQualAttemptHld = m_nQualAttempt;
	m_nQualRideHld = m_nQualRide;
}

/******************************************************************************/

int CTIMeasuredRace::QualAdvance()
{
	CTIRS *pTIRS=GetRS();
	int nToReturn=1;
	switch (pTIRS->m_nPrimaryEval) {
	// --------------------------------------------------------------------------
	// Primary evalution method is number of laps
	case RS_EVAL_NUM_LAPS:
		// Increment ride index
		m_nQualRide++;
		// Try whether another ride is available
		if (m_nQualRide >= pTIRS->m_nNumRides) {
			// If not, advance to the next racer
			m_nQualRide=0;
			m_nQualRacer++;
			nToReturn=2;
			// .. but try whether there is any racer available
			if (m_nQualRacer >= m_aPartEntities.GetSize()) return 0;
		}
		return nToReturn;
		break;
	// --------------------------------------------------------------------------
	// Primary evalution method is the best time
	case RS_EVAL_FIRST_TIME:
		do {
			m_nQualRacer++;
			if (m_nQualRacer >= m_aPartEntities.GetSize()) {
				m_nQualRacer=0;
				m_nQualAttempt++;
				if (m_nQualAttempt >= pTIRS->m_nNumAttempts) return 0;
			}

			CTIMeasuredEntity *pEntity = static_cast<CTIMeasuredEntity*>(m_aPartEntities[m_nQualRacer]);

//			if (pEntity->m_dwTimeElapsed < pTIRS->m_Times[0][1].GetSecond()*1000) return 2;
			if (pEntity->m_dwTimeElapsed < pTIRS->GetTimeSettings(RACE_PART_QUAL , 1)) return 2;
		}
		// And thou shalst repeat this until thou findest the answer to live,
		// universe and everything.
		while (42);
		break;
	}
	return 0;
}

/******************************************************************************/

void CTIMeasuredRace::QualGetNextRacers(CTIRaceEntity** ppNextEntities, DWORD dwNumEntities)
{
	QualSavePosition();

	int nIdx;
	BOOL bEnabled=TRUE;
	CTIRaceEntity *pLastEntity=NULL;
	CTIRaceEntity *pEntity=NULL;

	for (nIdx=0; nIdx<dwNumEntities; nIdx++) ppNextEntities[nIdx]=NULL;
	for (nIdx=0; nIdx<dwNumEntities; nIdx++) {
		if (bEnabled) {
			pEntity = m_aPartEntities[m_nQualRacer];
			ppNextEntities[nIdx]=m_aPartEntities[m_nQualRacer];
			int nResult = QualAdvance();
			while (nResult == 1) {
				nResult = QualAdvance();
			}
			if (nResult == 0) bEnabled=FALSE;
		}
	}

	QualLoadPosition();
}

/*******************************************************************************
*
*  REPORT FUNCTIONS
*
*******************************************************************************/

BOOL GetUniqueFileName(CString &s, const char *pcszMoreInfo) {
//	char pszBuffer[128];

//	m_TestBitmap.Load(WsfGetAppPath() + "/Data/sponsor.bmp");

   try {
	   CTime time = CTime::GetCurrentTime();
	   s = WsfGetApp()->GetAppDataPath() + "Logs/" + time.Format("log-%Y%m%d-%H%M");
   }
   catch (CException *pE)
   {
      WsfReportError(pE, "GetUniqueFileName for Logs could not resolve the Application Data Path", TRUE);
      return FALSE;
   }
}

/******************************************************************************/

void WriteString(CFile &file, const char *pcszString) {
	file.Write(pcszString, strlen(pcszString));
}

/******************************************************************************/

void WriteLn(CFile &file) {
	file.Write("\r\n", 2);
}

/******************************************************************************/

BOOL CTIMeasuredRace::ReportInit(const char *pcszReportName) {

	// Write
	CString s;
	if (!GetUniqueFileName(s, pcszReportName)) return FALSE;
	file.Open(s, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary);
}

/******************************************************************************/

void CTIMeasuredRace::ReportDone() {
	try {
		file.Close();
	}
	catch(...) {
	};
}

/******************************************************************************/

void CTIMeasuredRace::QualReportRide(CTIMeasuredEntity *pEntity) {
	DWORD dwTime = GetTickCount();
	
	CString s;
	char t1[32], t2[32], t3[32];

	DWORD dw1, dw2, dw3, dw4;
	pEntity->QualGetBestTimes(&dw1, &dw2);
	pEntity->QualGetNumLaps(&dw3, &dw4);

	s.Format("%32s %8s %8s %8s", 
		pEntity->GetEntityName(CTIRaceEntity::NICKNAME),
		TimeFormat(dw1, t1, 3),
		TimeFormat(dw2, t2, 3),
		LapsFormat(dw3, dw4, GetRS()->m_nPrimaryEval, t3));

	WriteString(file, s);
	WriteLn(file);
}

/******************************************************************************/

BOOL CTIMeasuredRace::QualReportResults()
{
	if (!ReportInit()) return FALSE;

	WriteString(file, "*** VYSLEDKY KVALIFIKACE ***");
	WriteLn(file); WriteLn(file);

	int nIdx;
	ITERATE(m_aPartEntities, nIdx) {
		CString s;
		s.Format("%3d.", nIdx+1);
		WriteString(file, s);
		QualReportRide(m_aPartEntities[nIdx]);
	}

	ReportDone();
}

/******************************************************************************/

BOOL CTIMeasuredRace::RaceReportGroup(CEntityArray &rArray, BOOL bLastLap, const char *pcszReportName)
{
   if (!ReportInit()) return FALSE;

	WriteString(file, pcszReportName);
	WriteLn(file);
	WriteLn(file);

	CString s;

	if (bLastLap) {
		s.Format("%3s %28s %8s %8s %8s",
			" ",
			"zavodnik",
			"laps",
			"celkem",
			"best lap");
	}
	else {
		s.Format("%3s %28s %8s %8s",
			" ",
			"zavodnik",
			"celkem",
			"best lap");
	}

	WriteString(file, s);
	WriteLn(file);

	int nIdx;
	ITERATE(rArray, nIdx) 
	{
		CTIMeasuredEntity *pEntity = DC_ME(rArray[nIdx]);
		if (pEntity) {

			char t1[32], t2[32], t3[32];

			CEventData *pEvent = pEntity->GetCurrentEvent();
			CRideData *pData = pEvent->GetCurrentRide();

			DWORD dwLapsNow = pData->m_dwNumLaps;
			DWORD dwHdtsNow = pData->m_dwNumHdts;

			DWORD dwLapsTotal = pEvent->GetTotalLaps();
			DWORD dwHdtsTotal = pEvent->GetTotalHdts();

			DWORD dwBestLap = pEvent->GetBestLap();

			LapsFormat(dwLapsNow, dwHdtsNow, 0, t1);
			LapsFormat(dwLapsTotal, dwHdtsTotal, 0, t2);
			TimeFormat(dwBestLap, t3, 3);

			if (bLastLap) {
				s.Format("%3d %28s %8s %8s %8s",
					nIdx + 1,
					pEntity->GetEntityName(CTIRaceEntity::NICKNAME),
					t1,
					t2,
					t3
				);
			}

			else {
				s.Format("%3d %28s %8s %8s",
					nIdx + 1,
					pEntity->GetEntityName(CTIRaceEntity::NICKNAME),
					t2,
					t3
				);
			}

			WriteString(file, s);
			WriteLn(file);
		}
	}

	ReportDone();

}

/******************************************************************************/

void CTIMeasuredRace::CreateFinalResultsList()
{
	CTIResults *pResults = new CTIResults();
//	CArticle *pDoc = new CArticle;
	pResults->GatherDataFinalResults(this);
	AddResultChild(pResults);
	WsfGetApp()->TreeHasChanged(this, CWsfApp::ETreeChanges::GENERAL);
}

/******************************************************************************/

void CTIMeasuredRace::OnFinalResultsList()
{
	CTIResults results;
//	CArticle *pDoc = new CArticle;

   CString sDocument;
   CString s; 
	s.LoadString(IDS_SHEET_FINAL_RESULTS);

	results.GatherDataFinalResults(this);

   results.WriteHtmlHeader(this, sDocument, s);
   results.WriteHtmlStandardHeader(this, sDocument);
   results.CreateHtmlFinalResultList(this, s);
   results.WriteHtmlFooter(sDocument);

   results.ShowHtmlPage(sDocument, s);
}

/*******************************************************************************
*
*  DISPLAY
*
*******************************************************************************/

void CTIMeasuredRace::ShowFullScreen() {

	m_pWnd = new CRaceUIWnd;
	m_pWnd->m_pRace = this;
	m_pWnd->Create(WsfGetFrame(), FALSE, NULL);
	m_pWnd->ShowWindow(SW_SHOW);
}

void CTIMeasuredRace::HideFullScreen() {

	m_pWnd->ShowWindow(SW_SHOW);
	m_pWnd->DestroyWindow();
	delete m_pWnd;

}

/*******************************************************************************
*
*  RACE
*
*******************************************************************************/

void CTIMeasuredRace::InitGroupRacers() {
	int nIdx;
	
	// For each entity remember the track it rides
	m_pArrSortedGroup.SetSize(0);
	ITERATE(m_aTrackMappedEntities, nIdx) {
		if (m_aTrackMappedEntities[nIdx]) {
			CTIMeasuredEntity *pEntity = DC_ME(m_aTrackMappedEntities[nIdx]);
			pEntity->m_nCurrentTrack = nIdx;
			m_pArrSortedGroup.Append(pEntity);
		}
	}

	// Sort them according to their results so far
	RaceSortRacers(m_pArrSortedGroup);

	// If there is less entities with tracks than total num entities in segment
	if (m_pArrSortedGroup.GetSize() < m_aGroup[m_nActiveGroup].GetSize()) {
		ITERATE(m_aGroup[m_nActiveGroup], nIdx) {
			int nPos = m_pArrSortedGroup.FindItem(m_aGroup[m_nActiveGroup][nIdx]);
			if (nPos == -1) {
				// Append also those that do not have any track assigned.
				// Mark that those entities have no track using -1.
				m_aGroup[m_nActiveGroup][nIdx]->m_nCurrentTrack = -1;
				m_pArrSortedGroup.Append(m_aGroup[m_nActiveGroup][nIdx]);
			}
		}
	}

	// Prepare all the entities for the ride that is about to begin
	ITERATE(m_pArrSortedGroup, nIdx) {
		m_pArrSortedGroup[nIdx]->
			GetCurrentEvent()->
				AppendNewRide(m_pArrSortedGroup[nIdx]->m_nCurrentTrack);
		m_pArrSortedGroup[nIdx]->m_nDisplayLine = nIdx;
	}
}

/******************************************************************************/

void CTIMeasuredRace::OnRunRace() {
	ShowFullScreen();
	SpawnRaceThread();
}

/******************************************************************************/

void CTIMeasuredRace::OnResetRace() {

	// Let Radek to reset its part of the race

	CTIRace::OnResetRace();

	// First, let's erase the content of the result folder

	CWsfTreeItem *pResults = GetResultsFolder();
	CWsfTreeItem *p1 = pResults->GetChild();
	CWsfTreeItem *p2;
	while (p1) {
		p2 = p1->GetNextSibling();
		p1->RemoveFromTree();
		delete p1;
		p1 = p2;
	}

	// Now, let's erase all the entities

	CTIMeasuredEntity *pEntity = DC_ME(GetFirstEntity());
	while (pEntity) {
		pEntity->Reset();
		pEntity = DC_ME(pEntity->GetNextSibling());
	}

	m_nQualAttempt = 0;
	m_nQualRacer = 0;
	m_nQualRide = 0;
	m_nCurrentRide = 0;

	// No recovery is needed, since the race is in a consistent state

	m_bNeedRecovery = FALSE;
	m_bPartEndedButNotAccepted = FALSE;

	// And now, let's modify the tree

	WsfGetApp()->TreeHasChanged(this, CWsfApp::ETreeChanges::GENERAL);
}

/******************************************************************************/

void CTIMeasuredRace::ResetCurrentEvent() {

	// If now it is qualification, reset the whole race
	if (m_nRacePart == RACE_PART_QUAL) OnResetRace();

	// Reset status variables

	m_nActiveGroup=0;
	m_nCurrentRide=0;

	// Reset results

	/*

   Was too tired to do this...

	CWslArray <CWsfTreeItem *> pArr;

	CWsfTreeItem *pItem = GetResultsFolder();
	pItem = pItem->GetChild();
	while (pItem) {
		pArr.Append(pItem);
		pItem=pItem->GetNextSibling();
	}

	int nIdx;
	ITERATE(pArr, nIdx) {
		CTIResults *pResults = dynamic_cast<CTIResults*>(pArr[nIdx]) {
			pArr[nIdx]->
		}
	}

	*/

	// Reset entities

	int nIdx;
	ITERATE(m_aPartEntities, nIdx) {
		m_aPartEntities[nIdx]->ResetIntoEvent(GetEventIdx(m_nRacePart));
	}

}

/******************************************************************************/

void CTIMeasuredRace::RaceSortRacers(CEntityArray &rArray) {
	int nNum = rArray.GetSize();
	int a,b;

	if (nNum > 1) {
		for (a=0; a<nNum-1; a++) {
			for (b=1; b<nNum; b++) {

				CTIMeasuredEntity *p1 = rArray[b-1];
				CTIMeasuredEntity *p2 = rArray[b];
				if (p1->RaceCompareWith(p2)==-1) {
					rArray[b-1] = p2;
					rArray[b] = p1;
				}
			}
		}
	}

	TRACE0("RaceSortRacers ...\n");
	ITERATE(rArray, nNum) {
		TRACE1("    %s\n", rArray[nNum]->GetEntityName(CTIRaceEntity::NICKNAME));
	}
}

/******************************************************************************/

int CTIMeasuredRace::SetActiveGroup(int nGroup, CWslArray<int> *paFirstLanes) {

	return CTIRace::SetActiveGroup(nGroup, paFirstLanes);
}

/******************************************************************************/

void CTIMeasuredRace::AssignLanes(int nRide) {
	CTIRace::AssignLanes(nRide);
}

/******************************************************************************/

void CTIMeasuredRace::ForgetLastRide() {
	int nIdx;
	ITERATE(m_aGroup[m_nActiveGroup], nIdx) {
		m_aGroup[m_nActiveGroup][nIdx]->GetCurrentEvent()->ForgetCurrentRide();
	}
}

/*******************************************************************************
*
*  CONTROL
*
*******************************************************************************/

void CTIMeasuredRace::OnSuccess(BOOL bHideScreen) {
//	AfxMessageBox("Success. That means, I will end the part here...");
//	AfxMessageBox("Me�en� ��sti z�vodu bylo dokon�eno.\n");
	
	if (bHideScreen) HideFullScreen();

	// Here was "STORE THE DATA" sequence.
	// But this is no longer needed since there is separate command to finnish
	// the race part.

	m_bPartFinished = TRUE;
	m_bPartInProgress = FALSE;

}

/******************************************************************************/

void CTIMeasuredRace::OnInterrupt(BOOL bHideScreen) {
	AfxMessageBox(IDS_PART_INTERRUPTED_CONTINUE_POSSIBLE);
	if (bHideScreen) HideFullScreen();
}

/******************************************************************************/

void CTIMeasuredRace::OnIncorrect(BOOL bHideScreen) {
	if (bHideScreen) HideFullScreen();
}

/******************************************************************************/

void CTIMeasuredRace::OnGroupSuccess()
{
	HideFullScreen();

	CTIResults *pResults = new CTIResults();
	pResults->GatherDataGroupRide(this);
	AddResultChild(pResults);
	WsfGetApp()->TreeHasChanged(this, CWsfApp::ETreeChanges::GENERAL);

	BOOL bThisIsLastGroup = (m_nActiveGroup + 1 == m_nNumGroups);
	if (bThisIsLastGroup) {
		if (AfxMessageBox(IDS_IS_RACE_PART_CORRECTLY_MEASURED, MB_YESNO)==IDYES) {
			OnSuccess(FALSE);
		}
		else {
			m_bPartEndedButNotAccepted = TRUE;
			m_bNeedRecovery = TRUE;
			OnIncorrect(FALSE);
		}
	}
	else {
		m_nActiveGroup++;
	}
}

/*******************************************************************************
*
*  THREAD SPAWNERS
*
*******************************************************************************/

void CTIMeasuredRace::SpawnQualificationThread() {
	m_hThread = CreateThread(NULL, 0, &QualThreadProc, this, 0, &m_dwThreadID);
}

/******************************************************************************/

void CTIMeasuredRace::SpawnRaceThread() {
	m_hThread = CreateThread(NULL, 0, &RaceThreadProcEx, this, 0, &m_dwThreadID);
}

/*********************************************************************** eof **/
