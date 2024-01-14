/******************
*                 *  TIResults.cpp
*   __|           *  ___________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  08/2000
*       \    '\   *  ___________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  Implementation of ArtEdit, a control for use by software
*     ^^     ^^   *  of the SpyTech company. This file is property of the
*                 *  SpyTech company.
*******************                                                           */

#include "stdafx.h"
#include "main.h"
#include "TIResults.h"
#include "version.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTIResults, CWsfTreeItem)
	//{{AFX_MSG_MAP(CTIResults)
	ON_UPDATE_COMMAND_UI(ID_WSF_OPEN, OnUpdateWsfOpen)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_COMMAND(ID_WSF_OPEN, OnWsfOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CTIResults, CWsfTreeItem, 1)

/*******************************************************************************
*
*  JUST A TINY MACROLANGUAGE
*
*******************************************************************************/

#ifdef DOC
#undef DOC
#endif // ifdef DOC

#define DOC pDoc->

#define INS_PAR             DOC InsertPar()
#define INS_TAB(x)          DOC InsertTable(x)
#define SET_TAB_COL(i, w)   DOC SetTableColWidth(i, w, UNIT_PERCENT)
#define ASC                 DOC ContextAscent()
#define DSC_TAB(i)          DOC ContextTableDescent(i)
#define DSC                 DOC ContextDescent()
#define WRT(x)              DOC Write(x)
#define LFD                 DOC LF()
#define WRT_TAB(i, x)       DSC_TAB(i); WRT(x); ASC
#define ROW                 DOC AddTableRow();

/*******************************************************************************
*
*  CONSTRUCTION / DESTRUCTION
*
*******************************************************************************/

CTIResults::CTIResults() 
: CWsfTreeItem(IDR_CONTEXT_MENUS,0, IDB_SMALL_ICONS,0, IDB_SMALL_ICONS,0, 0)
{
	m_nRacePart = 0;
	m_nViewID = 0;
}

/******************************************************************************/

CTIResults::CTIResults(int nRacePart)
: CWsfTreeItem(IDR_CONTEXT_MENUS,0, IDB_SMALL_ICONS,0, IDB_SMALL_ICONS,0, 0)
{
	// Remember, what part of the race we've been writing
	m_nRacePart = nRacePart;
	m_nViewID = 0;
}

/******************************************************************************/

void CTIResults::Serialize(CArchive &ar)
{
	CWsfTreeItem::Serialize(ar);

	DWORD dwVersion;
	DWORD dw;
	int nIdx;

	if (ar.IsLoading()) {
		ar >> dwVersion;
		ar >> m_nRacePart;
		if (dwVersion >= 0x20000921) {
			ar >> m_nType;
		}
		else m_nType = RESULTS_TYPE_RACE_PART_RESULTS;
		if (dwVersion >= 0x20000922) {
			ar >> m_sGroupName;
		}

		m_nArrOrder.SetSize(0);
		ar >> dw;
		while (dw != -1) {
			m_nArrOrder.Append(dw);
			ar >> dw;
		}

		m_nArrGroupBorders.SetSize(0);
		ar >> dw;
		while (dw != -1) {
			m_nArrGroupBorders.Append(dw);
			ar >> dw;
		}
	}

	if (ar.IsStoring()) {
		ar << (DWORD) MEASUREMENT_FILES_VERSION;
		ar << m_nRacePart;
		ar << m_nType;
		ar << m_sGroupName;

		ITERATE(m_nArrOrder, nIdx) ar << m_nArrOrder[nIdx];
		ar << (int) -1;

		ITERATE(m_nArrGroupBorders, nIdx) ar << m_nArrGroupBorders[nIdx];
		ar << (int) -1;
	}
}

/******************************************************************************/

void CTIResults::GatherData()
{
	m_nType = RESULTS_TYPE_RACE_PART_RESULTS;

	CWsfTreeItem *pItem = GetParent();
	pItem = pItem->GetParent();

	CTIMeasuredRace *pRace = dynamic_cast<CTIMeasuredRace*>(pItem);

	int nIdx;

	TRACE0("void CTIResults::GatherData() – ");
	int n;
//	ITERATE(pRace->m_pArrSortedEntities, n) {
	// Change made: m_pArrSortedEntities into m_aPartEntities
	ITERATE(pRace->m_aPartEntities, n) {
		nIdx = pRace->GetEntityIdx(pRace->m_aPartEntities[n]);
		TRACE1("%d ", nIdx);
		if (nIdx!=-1) m_nArrOrder.Append(nIdx);
	}
	TRACE0("\n");
}

/******************************************************************************/

void CTIResults::GatherDataPartEntities()
{
	m_nRacePart = 0;
	m_nType = RESULTS_TYPE_INITIAL_LIST;

	CWsfTreeItem *pItem = GetParent();
	pItem = pItem->GetParent();
	CTIMeasuredRace *pRace = dynamic_cast<CTIMeasuredRace*>(pItem);
	int nIdx, n;
	ITERATE(pRace->m_aPartEntities, n) {
		nIdx = pRace->GetEntityIdx(pRace->m_aPartEntities[n]);
		m_nArrOrder.Append(nIdx);
	}
}

/******************************************************************************/

void CTIResults::GatherDataGroups(CTIMeasuredRace *pRace)
{
	int nIdx, n;

	m_nRacePart = pRace->m_nRacePart;
	m_nType = RESULTS_TYPE_GROUP_CFG;
	m_nArrGroupBorders.SetSize(0);
	m_nArrOrder.SetSize(0);

	int nNumGroups = pRace->m_nNumGroups;
	int nCounter = 0;
	for (int nGroup=0; nGroup<nNumGroups; nGroup++) {
		ITERATE(pRace->m_aGroup[nGroup], n) {
			nIdx = pRace->GetEntityIdx(pRace->m_aGroup[nGroup][n]);
			m_nArrOrder.Append(nIdx);
			nCounter++;
		}
		m_nArrGroupBorders.Append(nCounter);
	}
}

/******************************************************************************/

void CTIResults::GatherDataGroupRide(CTIMeasuredRace *pRace, int nGroupIdx)
{
	int nIdx, n;
	m_nRacePart = pRace->m_nRacePart;
	m_nType = RESULTS_TYPE_GROUP;
	m_nArrGroupBorders.SetSize(0);
	m_nArrOrder.SetSize(0);

//  BUGA: Taking the groups from the initial group settings
//  DATE OF DISCOVERY: 23-sep-2000, after semifinal B group :-)
//	int nGroup = pRace->m_nActiveGroup;
//	ITERATE(pRace->m_aGroup[nGroup], n) {
//		nIdx = pRace->GetEntityIdx(pRace->m_aGroup[nGroup][n]);
//		m_nArrOrder.Append(nIdx);
//	}

//  Replaced piece of code
	int nGroup;
	if (nGroupIdx == -1) nGroup = pRace->m_nActiveGroup;
	else nGroup = nGroupIdx;

	ITERATE(pRace->m_pArrSortedEntities, n) {
		nIdx = pRace->GetEntityIdx(pRace->m_pArrSortedEntities[n]);
		m_nArrOrder.Append(nIdx);
	}

	m_sGroupName = GetGroupName(nGroup + 1, pRace->m_nNumGroups);
}

/******************************************************************************/

void CTIResults::GatherDataFinalResults(CTIMeasuredRace *pRace)
{
	int nIdx, n;
	m_nType = RESULTS_TYPE_FINAL_RESULTS;
	m_nArrOrder.SetSize(0);
	m_nArrGroupBorders.SetSize(0);

	CEntityArray array;
	pRace->BuildEntitiesArray(array);

	for (int a=0; a<array.GetSize()-1; a++) {
		for (int b=1; b<array.GetSize(); b++) {
			CTIMeasuredEntity *p1 = array[b-1];
			CTIMeasuredEntity *p2 = array[b];
			if (p1->RaceFinalCompareWith(p2) == -1) {
				array[b]=p1;
				array[b-1]=p2;
			}
		}
	}

	ITERATE(array, n) {
		nIdx = pRace->GetEntityIdx(array[n]);
		m_nArrOrder.Append(nIdx);
		TRACE2("%d %s\n", array[nIdx]->m_nCurrentEvent, array[nIdx]->GetEntityName(CTIRaceEntity::NAME_SURNAME));
	}
}

/******************************************************************************/

CString CTIResults::GetTitle() const
{
	CString sTitle;

	switch (m_nType) {
	case RESULTS_TYPE_GROUP_CFG:
		sTitle.LoadString(IDS_SHEET_TREE_GROUP_SEPARATION);
		break;
	case RESULTS_TYPE_INITIAL_LIST:
		sTitle.LoadString(IDS_SHEET_TREE_LIST_OF_RACERS);
		break;
	case RESULTS_TYPE_GROUP:
		sTitle.Format(IDS_SHEET_TREE_GROUP, GetRacePartName(m_nRacePart), m_sGroupName);
		break;
	case RESULTS_TYPE_RACE_PART_RESULTS:
		sTitle.Format(IDS_SHEET_TREE_RACE_PART_RESULTS, GetRacePartName(m_nRacePart));
		break;
	case RESULTS_TYPE_FINAL_RESULTS:
		sTitle.LoadString(IDS_SHEET_TREE_FINAL_RESULTS);
		break;
	default:
		sTitle = "name error";
		break;
	}

	return sTitle;
}


/******************************************************************************/

DWORD m_dwTempFileNameSuffix = 0;

void CTIResults::OnWsfOpen() 
{
   /*
	int a=0;
	a++;

	CArticle *pLog = new CArticle;
	pLog->CreateSimplePage();

	// --------------------------------------------------------------------------
	// MAKE QUALIFICATION REPORT

	CTIMeasuredRace *pRace = dynamic_cast<CTIMeasuredRace*>(GetParent()->GetParent());
	
	switch (m_nType) {
	case RESULTS_TYPE_INITIAL_LIST:
		CreateInitialListReport(pLog, pRace);
		break;
	case RESULTS_TYPE_RACE_PART_RESULTS:
		if (m_nRacePart == RACE_PART_QUAL) {
			if (pRace->GetRS()->m_nPrimaryEval == RS_EVAL_NUM_LAPS) CreateQualificationReportNumLaps(pLog, pRace);
			else CreateQualificationReportNumLaps(pLog, pRace);
		}
		else CreateRacePartReport(pLog, pRace);
		break;
	case RESULTS_TYPE_GROUP:
		CreateGroupReport(pLog, pRace);
		break;
	case RESULTS_TYPE_FINAL_RESULTS:
		CreateFinalResultList(pLog, pRace);
		break;
	}

	// UI

	if (!WsfGetApp()->ActivateView(m_nViewID))
	{
		CArtEditView *pArtEdit = new CArtEditView();
		pLog->m_bReadOnly = TRUE;
		
		WsfGetApp()->OpenDoc(pLog);
		m_nViewID = WsfGetApp()->OpenView(pArtEdit, pLog, GetTitle());
		ASSERT(m_nViewID);
	}
   */

   CString sDocument, sReportName;

   // Get race
   CTIMeasuredRace *pRace = dynamic_cast<CTIMeasuredRace*>(GetParent()->GetParent());

   // Get title for the HTML header
   CString sRacePartName;
   if (m_nType == RESULTS_TYPE_INITIAL_LIST) sRacePartName = "List of Registered Racers";
   else if (m_nType == RESULTS_TYPE_FINAL_RESULTS) sRacePartName = "Race Results";
   else sRacePartName = pRace->GetRacePartName(m_nRacePart);

   // Write HTML opening
   WriteHtmlHeader(pRace, sDocument, sRacePartName);
   sReportName = WriteHtmlStandardHeader(pRace, sDocument);
   sDocument += "<br>";

   // Write the appropriate HTML body
	switch (m_nType) {
	case RESULTS_TYPE_INITIAL_LIST:
		CreateHtmlInitialListReport(pRace, sDocument);
		break;
	case RESULTS_TYPE_RACE_PART_RESULTS:
		if (m_nRacePart == RACE_PART_QUAL) {
			if (pRace->GetRS()->m_nPrimaryEval == RS_EVAL_NUM_LAPS) CreateHtmlQualificationReportNumLaps(pRace, sDocument);
			else CreateHtmlQualificationReportBestTime(pRace, sDocument);
		}
		else CreateHtmlRacePartReport(pRace, sDocument);
		break;
	case RESULTS_TYPE_GROUP:
		CreateHtmlGroupReport(pRace, sDocument);
		break;
	case RESULTS_TYPE_FINAL_RESULTS:
		CreateHtmlFinalResultList(pRace, sDocument);
		break;
   }

   sDocument += "<br>";

   // Write Footer
   WriteHtmlFooter(sDocument);

   ShowHtmlPage(sDocument, sReportName);
}

/******************************************************************************/

void CTIResults::ShowHtmlPage(CString sDocument, CString sReportName)
{
   // Write temporary file:
   // Get the temporary file name
   m_dwTempFileNameSuffix++;
   CString sFileTitle; sFileTitle.Format("temp%04x", m_dwTempFileNameSuffix);

   // Create temporary file
   CWslFile file;
   file.CreateTemporary(sFileTitle);
   CString sFileName = file.GetPath() + file.GetName(); // Remember its name
   file.Write(sDocument, sDocument.GetLength()); // Write the HTML document content
   file.Close(); // Close the file

   // Invoke the HTML document
   m_nViewID = WsfGetApp()->OpenHtmlFile(sFileName, sReportName, 
      IDB_SMALL_ICONS,0, TRUE, FALSE);
}

/******************************************************************************/

void CTIResults::OnUpdateWsfOpen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

/******************************************************************************/

void CTIResults::OnFilePrint() 
{
	OnWsfOpen();
	CWsfViewInfo* pvi = WsfGetCApp()->GetViewInfo(m_nViewID);
	ASSERT(pvi);
	pvi->pView->PostMessage(WM_COMMAND, ID_FILE_PRINT, 0);
}

/******************************************************************************/

void CTIResults::OnUpdateFilePrint(CCmdUI* pCmdUI) {
	pCmdUI->Enable(TRUE);
}

/******************************************************************************/

CTIResults::~CTIResults()
{
}

/*********************************************************************** eof **/
