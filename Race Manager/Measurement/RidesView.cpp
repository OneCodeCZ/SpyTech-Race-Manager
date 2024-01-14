/******************
*                 *  RidesView.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  SEP/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"
#include "RidesView.h"
#include "DlgEventData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*******************************************************************************
*
*  CONSTRUCTION / DESTRUCTION
*
*******************************************************************************/

IMPLEMENT_DYNCREATE(CRidesView, CFormView)

CRidesView::CRidesView()
	: CFormView(CRidesView::IDD)
{
	//{{AFX_DATA_INIT(CRidesView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

#define IDC_RIDE_LIST 0x12123123

/******************************************************************************/

CRidesView::~CRidesView()
{
}

/******************************************************************************/

void CRidesView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRidesView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

/******************************************************************************/

BEGIN_MESSAGE_MAP(CRidesView, CFormView)
	//{{AFX_MSG_MAP(CRidesView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_RIDE_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*******************************************************************************
*
*  DIAGNOSTICS
*
*******************************************************************************/

#ifdef _DEBUG
void CRidesView::AssertValid() const
{
	CFormView::AssertValid();
}

void CRidesView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/*******************************************************************************
*
*  CRidesView
*
*******************************************************************************/

/******************************************************************************/

void CRidesView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	m_Resize.Add(IDC_EDIT, 1, 0, 1, 0);
	m_Resize.Add(IDC_RACE_PART_COMMENT, 0, 0, 1, 0);
	m_Resize.Add(IDC_RACE_PART, 0, 0, 1, 0);
	m_Resize.Add(IDC_RIDE_LIST, 0, 1, 0, 1);

	UpdateList(m_pRace->m_nRacePart);
}

/******************************************************************************/

void CRidesView::OnSize(UINT nType, int cx, int cy) 
{
	m_Resize.Update(cx, cy);
	CFormView::OnSize(nType, cx, cy);
}

/******************************************************************************/

int CRidesView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CString s;
	CRect rect;
	GetClientRect(LPRECT(rect));

	rect.left += 12;
	rect.top += 32;
	rect.right -= 12;
	rect.bottom -= 48;

	m_wndTblCtrl.CreateEx(WS_EX_CLIENTEDGE, NULL, NULL, WS_CHILD | WS_VISIBLE, rect, this, IDC_RIDE_LIST, NULL);
	VERIFY(s.LoadString(IDS_RANK)); m_wndTblCtrl.AddTextColumn(s,	100, CWsfTableCtrl::CColumn::RIGHT);
	VERIFY(s.LoadString(IDS_RACER)); m_wndTblCtrl.AddTextColumn(s, 200);

	for (int a=0; a<10; a++) {
		CString s;
		s.Format(IDS_SEGMENT_NUMBER_IN_HEADER, a+1);
		m_wndTblCtrl.AddTextColumn(LPCTSTR(s), 100, CWsfTableCtrl::CColumn::RIGHT);
	}

	VERIFY(s.LoadString(IDS_TRACK_SEGMENTS)); m_wndTblCtrl.AddTextColumn(s, 100, CWsfTableCtrl::CColumn::RIGHT);
	VERIFY(s.LoadString(IDS_PENALTY_ABRV)); m_wndTblCtrl.AddTextColumn(s, 100, CWsfTableCtrl::CColumn::RIGHT);
	VERIFY(s.LoadString(IDS_TOTAL)); m_wndTblCtrl.AddTextColumn(s, 100, CWsfTableCtrl::CColumn::RIGHT);

	m_wndTblCtrl.ActivateDefaultColumns();

	m_Resize.SetParent(this);

	return 0;
}

/******************************************************************************/

void CRidesView::UpdateList(int nRacePart)
{
	CEntityArray array;
	m_pRace->BuildEntitiesArray(array);

	m_wndTblCtrl.DeleteAllRows();
	
	int nIdx;
	ITERATE (array, nIdx) {
		m_wndTblCtrl.AddRow();
		m_wndTblCtrl.SetItemData(nIdx, 0, (LONG)(&(array[nIdx])));
		CString s;
		s.Format("%d.", nIdx+1);
		m_wndTblCtrl.SetItem(nIdx, 0, s);
		m_wndTblCtrl.SetItem(nIdx, 1, array[nIdx]->GetEntityName(CTIRaceEntity::NICKNAME));

		CEventData *pEvent = array[nIdx]->m_pArrEvents[GetEventIdx(nRacePart)];
		
		int nRide;

		ITERATE (pEvent->m_pArrRides, nRide) {
			CRideData *pRide = pEvent->m_pArrRides[nRide];
			// Determine whether we are in pause mode
			if (pRide->m_nTrackAssigned == -1) s.Format("P");
			else s.Format("%d (%d)", pRide->m_dwNumLaps, pRide->m_nTrackAssigned+1);
			m_wndTblCtrl.SetItem(nIdx, nRide + 2, s);
		}

		s.Format("%d", pEvent->GetTotalLaps());
		m_wndTblCtrl.SetItem(nIdx, 14, s);
		s.Format("%d", pEvent->m_nPenalization);
		m_wndTblCtrl.SetItem(nIdx, 13, s);
		s.Format("%d", pEvent->GetTotalHdts());
		m_wndTblCtrl.SetItem(nIdx, 12, s);
	}
}

/******************************************************************************/

void CRidesView::OnEdit() 
{
	if (m_wndTblCtrl.GetSelRowCount() == 0) {
		AfxMessageBox(IDS_CHOOSE_RACER_TO_EDIT);
		return;
	}

	if (m_wndTblCtrl.GetSelRowCount() > 1) {
		AfxMessageBox(IDS_TOO_MUCH_RACERS_SELECTED);
		return;
	}

	CEntityArray array;
	m_pRace->BuildEntitiesArray(array);

	POSITION pos = m_wndTblCtrl.GetFirstSelectedRowPosition();
	int nIdx = m_wndTblCtrl.GetNextSelectedRow(pos);
	CTIMeasuredEntity *pEntity = array[nIdx];

	if (pEntity->m_nCurrentEvent != GetEventIdx(m_pRace->m_nRacePart)) {
		AfxMessageBox(IDS_NO_EDIT_RACER_DIDNT_START);
		return;
	}

	CDlgEventData ed;
	ed.m_pEntity = pEntity;
	ed.nRacePart = m_pRace->m_nRacePart;
	if (ed.DoModal() == IDOK) {
		UpdateList(m_pRace->m_nRacePart);
		m_pRace->RaceSortRacers(m_pRace->m_aPartEntities);
	}

}

/******************************************************************************/

void CRidesView::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnEdit();
	*pResult = 0;
}

/*********************************************************************** eof **/
