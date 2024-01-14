/******************
*                 *  DlgReturn.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  01-SEP-2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of RM source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"
#include "DlgReturn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*******************************************************************************
*
*  CDlgReturn
*
*******************************************************************************/

CDlgReturn::CDlgReturn(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReturn::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgReturn)
	m_listParts = _T("");
	//}}AFX_DATA_INIT
}

/******************************************************************************/

void CDlgReturn::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgReturn)
	DDX_Control(pDX, IDC_GROUPS, m_listGroups);
	DDX_Control(pDX, IDC_RIDES, m_listRides);
	DDX_LBString(pDX, IDC_PARTS, m_listParts);
	//}}AFX_DATA_MAP
}

/******************************************************************************/

BEGIN_MESSAGE_MAP(CDlgReturn, CDialog)
	//{{AFX_MSG_MAP(CDlgReturn)
	ON_LBN_SELCHANGE(IDC_GROUPS, OnSelchangeGroups)
	ON_BN_CLICKED(IDC_WHOLE, OnWhole)
	ON_BN_CLICKED(IDC_RETURN, OnReturn)
	ON_BN_CLICKED(IDC_END, OnEnd)
	ON_BN_CLICKED(IDC_CONTINUE, OnContinue)
	ON_LBN_SELCHANGE(IDC_RIDES, OnSelchangeRides)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*******************************************************************************
*
*  MESSAGE HANDLERS
*
*******************************************************************************/

BOOL CDlgReturn::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Fill list of groups

	for (int a=0; a<=m_pRace->m_nActiveGroup; a++) {
		CString s;
		s.Format(IDS_NTH_GROUP, a+1);
		m_listGroups.AddString(s);
	}

	if (a) {
		m_listGroups.SetCurSel(0);
		FillListOfRides(0);
	}

	// Enable / disable proper buttons

	GetDlgItem(IDC_WHOLE)->EnableWindow(TRUE);
	GetDlgItem(IDC_RETURN)->EnableWindow(FALSE);
	GetDlgItem(IDC_CONTINUE)->EnableWindow(TRUE);
	GetDlgItem(IDC_END)->EnableWindow(FALSE);

	if (m_pRace->m_bPartEndedButNotAccepted) {
		GetDlgItem(IDC_END)->EnableWindow(TRUE);
		GetDlgItem(IDC_CONTINUE)->EnableWindow(FALSE);
	}

	m_bUserWantsEndRacePart = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************/

void CDlgReturn::FillListOfRides(int nGroupIdx) {
	CTIMeasuredEntity *pEntity = DC_ME(m_pRace->m_aGroup[nGroupIdx][0]);
	ASSERT(pEntity);

	m_listRides.ResetContent();
	CEventData *pData = pEntity->GetCurrentEvent();

	int nNumRides = pData->m_pArrRides.GetSize();

	for (int a=0; a<nNumRides; a++) {
		CString s;
		s.Format(IDS_RIDE_X_OF_GROUP_Y, a+1, nGroupIdx+1);
		m_listRides.AddString(s);
	}
}

/******************************************************************************/

void CDlgReturn::OnSelchangeRides() 
{
	int nRideIdx = m_listRides.GetCurSel();
	GetDlgItem(IDC_RETURN)->EnableWindow(nRideIdx != -1);
}

/******************************************************************************/

void CDlgReturn::OnSelchangeGroups() 
{
	int nGroupIdx = m_listGroups.GetCurSel();
	FillListOfRides(nGroupIdx);
	GetDlgItem(IDC_RETURN)->EnableWindow(FALSE);
}

/******************************************************************************/

void CDlgReturn::OnWhole() // The whole part again
{
	if (AfxMessageBox(IDS_OK_TO_CANCEL_RACE_PART_DATA, MB_YESNO)==IDNO) return;
	m_pRace->ResetCurrentEvent();
	m_pRace->m_bNeedRecovery = FALSE; // Standard initialization will be performed
	m_pRace->m_bPartEndedButNotAccepted = FALSE;
	EndDialog(IDOK);
}

/******************************************************************************/
// Nove Veseli, 1.9.2000
// Zdar nad Sazavou 15:13

void CDlgReturn::OnReturn() // Return to part
{
	if (AfxMessageBox(IDS_OK_TO_RESET_INTO_RACE_PART, MB_YESNO)==IDNO) return;

	int nGroupIdx = m_listGroups.GetCurSel();
	int nRideIdx = m_listRides.GetCurSel();

	// first, completely reset all racers in groups that follows currently
	// selected group.

	int a;

	int nGroupNum = m_listGroups.GetCount();

	for (a=nGroupIdx+1; a<nGroupNum; a++) {
		int nIdx;
		ITERATE(m_pRace->m_aGroup[a], nIdx) {
			m_pRace->m_aGroup[a][nIdx]->ResetIntoEvent(GetEventIdx(m_pRace->m_nRacePart));
		}
	}

	// and now, let's reset rides of racers in currently selected group.

	int nRideNum = m_listRides.GetCount();

	int nIdx;
	ITERATE(m_pRace->m_aGroup[nGroupIdx], nIdx) {
		CTIMeasuredEntity *pEntity = m_pRace->m_aGroup[nGroupIdx][nIdx];
		pEntity->ResetBeforeRide(nRideIdx);
	}

	m_pRace->m_bPartEndedButNotAccepted = FALSE;

	m_pRace->m_nActiveGroup = nGroupIdx;
	m_pRace->m_nCurrentRide = nRideIdx;

	EndDialog(IDOK);
}

/******************************************************************************/

void CDlgReturn::OnEnd() // End part
{
	ASSERT(m_pRace->m_bPartEndedButNotAccepted);
	if (AfxMessageBox(IDS_OK_TO_END_RACEPART, MB_YESNO)==IDNO) return;
	m_bUserWantsEndRacePart = TRUE;
	EndDialog(IDOK);
}

/******************************************************************************/

void CDlgReturn::OnContinue() // Continue without anything
{
	ASSERT(!m_pRace->m_bPartEndedButNotAccepted);
;	if (AfxMessageBox(IDS_OK_TO_CONTINUE_MEASUREMENT, MB_YESNO)==IDNO) return;
	m_pRace->m_bNeedRecovery = TRUE;
	m_pRace->m_bPartEndedButNotAccepted = FALSE;
	EndDialog(IDOK);
}

/******************************************************************************/

void CDlgReturn::OnReset() 
{
	if (AfxMessageBox(IDS_OK_TO_RESET_WHOLE_RACE, MB_YESNO)==IDNO) return;
	m_pRace->OnResetRace();	
	EndDialog(IDCANCEL);
}

/*********************************************************************** eof **/

void CDlgReturn::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
}
