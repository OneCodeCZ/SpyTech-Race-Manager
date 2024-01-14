// DlgRemapSingleTrack.cpp : implementation file
//

#include "stdafx.h"
#include "main.h"
#include "DlgRemapSingleTrack.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*******************************************************************************
*
*  CDlgRemapSingleTrack
*
*******************************************************************************/

CDlgRemapSingleTrack::CDlgRemapSingleTrack(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRemapSingleTrack::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRemapSingleTrack)
	m_sChallenge = _T("");
	//}}AFX_DATA_INIT
}

/******************************************************************************/

void CDlgRemapSingleTrack::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRemapSingleTrack)
	DDX_Text(pDX, IDC_CHALLENGE, m_sChallenge);
	//}}AFX_DATA_MAP
}

/******************************************************************************/

BEGIN_MESSAGE_MAP(CDlgRemapSingleTrack, CDialog)
	//{{AFX_MSG_MAP(CDlgRemapSingleTrack)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*******************************************************************************
*
*  Handlers
*
*******************************************************************************/

BOOL CDlgRemapSingleTrack::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UpdateData(TRUE);
   CString t; t.LoadString(IDS_GO_OVER_SENSOR_X);
	CString s; s.Format(t, m_nVirtualTrack + 1);
	m_sChallenge = s;
	UpdateData(FALSE);

	BOOL bOK = TRUE;

	// Retrieve settings from RM settings
	monitor.RetrieveSettings();
	if (monitor.Connect()) {
		monitor.ResetVariables();
		if (!monitor.MeasureAll()) bOK = FALSE;
		monitor.SetClock(0);
		monitor.SetControlsEx(CONTROLS_POWER | CONTROLS_CLOCK);
	}
	else bOK = FALSE;

	if (!bOK) {
		AfxMessageBox(IDS_ERROR_OPENING_STM2008);
		monitor.Disconnect();
		EndDialog(IDCANCEL);
	}
	
	SetTimer(0, 100, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************/

void CDlgRemapSingleTrack::OnDestroy() 
{
	CDialog::OnDestroy();
	
	monitor.SetControlsEx(0);
	monitor.Disconnect();
	KillTimer(0);
}

/******************************************************************************/

void CDlgRemapSingleTrack::OnTimer(UINT nIDEvent) 
{
	
	if (!monitor.MeasureAll()) {
		MessageBeep(0xffffffff);
		return;
	}
	for (int a=0; a<NUMTRACKS; a++) {
		if (monitor.m_bArrWasMotion[a]) {
			m_nPhysicalTrack = a;

         CString t; t.LoadString(IDS_LANE_X_IS_SENSOR_Y);
			CString s;
			s.Format(t, m_nVirtualTrack + 1, m_nPhysicalTrack + 1);
			m_sChallenge=s;
			UpdateData(FALSE);

//			Sleep(1500);
			EndDialog(IDOK);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

/*********************************************************************** eof **/
