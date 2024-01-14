/******************
*                 *  <NAME>
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  revised 06/05/04
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of <PROJECT> source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "resource.h"
#include "main.h"
#include "DlgTrainingSettings.h"
#include "Measurement API.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*******************************************************************************
*
*  CDlgTrainingSettings
*
*******************************************************************************/

CDlgTrainingSettings::CDlgTrainingSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTrainingSettings::IDD, pParent)
{
	m_nMode = -1;
   m_nNumRides = WsfGetCApp()->GetProfileInt("Settings\\Training", "num_rides", 3);
   m_nClearAfter = WsfGetCApp()->GetProfileInt("Settings\\Training", "clear_after", 60);
   m_sBeforeRide = WsfGetCApp()->GetProfileString("Settings\\Training", "before_ride", "0:00:15");
   m_sRideDuration = WsfGetCApp()->GetProfileString("Settings\\Training", "ride_duration", "0:03:00");
   m_sPauseDuration = WsfGetCApp()->GetProfileString("Settings\\Training", "pause_duration", "0:03:00");
}

/******************************************************************************/

void CDlgTrainingSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTrainingSettings)
	DDX_Radio(pDX, IDC_MODE_MANUAL, m_nMode);
	DDX_Text(pDX, IDC_NUM_RIDES, m_nNumRides);
	DDX_Text(pDX, IDC_BEFORE_RIDE, m_sBeforeRide);
	DDX_Text(pDX, IDC_PAUSE_DURATION, m_sPauseDuration);
	DDX_Text(pDX, IDC_RIDE_DURATION, m_sRideDuration);
	DDX_Text(pDX, IDC_CLEAR_AFTER, m_nClearAfter);
	//}}AFX_DATA_MAP
}

/******************************************************************************/

BEGIN_MESSAGE_MAP(CDlgTrainingSettings, CDialog)
	//{{AFX_MSG_MAP(CDlgTrainingSettings)
	ON_BN_CLICKED(IDC_MODE_MANUAL, OnModeManual)
	ON_BN_CLICKED(IDC_MODE_SEMIAUTO, OnModeSemiauto)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*******************************************************************************
*
*  MESSAGE HANDLERS
*
*******************************************************************************/

BOOL CDlgTrainingSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
//	WsfSetBoldFont(this, IDC_MODE_MANUAL);
//	WsfSetBoldFont(this, IDC_MODE_SEMIAUTO);

	m_nMode = GetProfileInt("Settings\\Training", "mode", 0);
   if (m_nMode == 0) OnModeManual(); 
   else OnModeSemiauto(); 
	m_sTitle.LoadString(IDS_DEFAULT_PRACTICE_CAPTION);
	UpdateData(FALSE);

	m_nBkDoor = 0;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************/

void CDlgTrainingSettings::OnModeManual() 
{
	GetDlgItem(IDC_CLEAR_AFTER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BEFORE_RIDE)->EnableWindow(FALSE);
	GetDlgItem(IDC_RIDE_DURATION)->EnableWindow(FALSE);
	GetDlgItem(IDC_PAUSE_DURATION)->EnableWindow(FALSE);
	GetDlgItem(IDC_NUM_RIDES)->EnableWindow(FALSE);
}

/******************************************************************************/

void CDlgTrainingSettings::OnModeSemiauto() 
{
	GetDlgItem(IDC_CLEAR_AFTER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BEFORE_RIDE)->EnableWindow(TRUE);
	GetDlgItem(IDC_RIDE_DURATION)->EnableWindow(TRUE);
	GetDlgItem(IDC_PAUSE_DURATION)->EnableWindow(TRUE);
	GetDlgItem(IDC_NUM_RIDES)->EnableWindow(TRUE);
}

/*******************************************************************************
*
*  BACKDOOR IMPLEMENTATION
*
*******************************************************************************/

void CDlgTrainingSettings::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   /*
	CRect rect;
	GetClientRect(LPRECT(rect));

	int nLimitX = rect.right / 2;
	int nLimitY = rect.bottom / 2;

	switch(m_nBkDoor) {
	case 0:
		SetTimer(0, 5000, NULL);
		if (point.y < nLimitY) m_nBkDoor = 1;
		else m_nBkDoor = 0;
		break;
	case 1:
		if (point.y > nLimitY) m_nBkDoor = 2;
		else m_nBkDoor = 0;
		break;
	case 2:
		if (point.y < nLimitY) m_nBkDoor = 3;
		else m_nBkDoor = 0;
		break;
	case 3:
		if (point.y < nLimitY) m_nBkDoor = 4;
		else m_nBkDoor = 0;
		break;
	case 4:
		if (point.y > nLimitY) {
			AfxMessageBox("Backdoor open");
			MAPI_EditRides(WsfGetCWorkspace()->m_pTIRace);
//			WsfGetFrame()->PostMessage(32849);
		}
		else m_nBkDoor = 0;
		break;
	}
   */

	CDialog::OnLButtonDblClk(nFlags, point);
}

/******************************************************************************/

void CDlgTrainingSettings::OnTimer(UINT nIDEvent) 
{
	m_nBkDoor = 0;	
	KillTimer(0);
	CDialog::OnTimer(nIDEvent);
}

/******************************************************************************/

void CDlgTrainingSettings::OnOK() 
{
	UpdateData(TRUE);
	if (m_nMode == 1 && m_nNumRides <= 0) {
		AfxMessageBox(IDS_TRAINING_NOT_ENOUGH_RIDES);
		return;
	}

   CWslDateTime d;
   if (!d.Set("1.1.2001 " + m_sBeforeRide)) {
      AfxMessageBox(IDS_IMPROPER_TIME_SYNTAX);
      GetDlgItem(IDC_BEFORE_RIDE)->SetFocus();
      return;
   }

   if (!d.Set("1.1.2001 " + m_sRideDuration)) {
      AfxMessageBox(IDS_IMPROPER_TIME_SYNTAX);
      GetDlgItem(IDC_RIDE_DURATION)->SetFocus();
      return;
   }

   if (!d.Set("1.1.2001 " + m_sPauseDuration)) {
      AfxMessageBox(IDS_IMPROPER_TIME_SYNTAX);
      GetDlgItem(IDC_PAUSE_DURATION)->SetFocus();
      return;
   }

   WsfGetCApp()->WriteProfileInt("Settings\\Training", "num_rides", m_nNumRides);
   WsfGetCApp()->WriteProfileInt("Settings\\Training", "clear_after", m_nClearAfter);
   WsfGetCApp()->WriteProfileString("Settings\\Training", "before_ride", m_sBeforeRide);
   WsfGetCApp()->WriteProfileString("Settings\\Training", "ride_duration", m_sRideDuration);
   WsfGetCApp()->WriteProfileString("Settings\\Training", "pause_duration", m_sPauseDuration);

   CDialog::OnOK();
}

/*********************************************************************** eof **/
