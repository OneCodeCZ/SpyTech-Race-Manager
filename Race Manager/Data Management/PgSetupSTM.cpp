///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "PgSetupSTM.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPgSetupSTM, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgSetupSTM)
	ON_BN_CLICKED(IDC_REMAP, OnRemap)
	ON_EN_SETFOCUS(IDC_PHYSICAL_1, OnSetfocusPhysical1)
	ON_EN_SETFOCUS(IDC_PHYSICAL_2, OnSetfocusPhysical2)
	ON_EN_SETFOCUS(IDC_PHYSICAL_3, OnSetfocusPhysical3)
	ON_EN_SETFOCUS(IDC_PHYSICAL_4, OnSetfocusPhysical4)
	ON_EN_SETFOCUS(IDC_PHYSICAL_5, OnSetfocusPhysical5)
	ON_EN_SETFOCUS(IDC_PHYSICAL_6, OnSetfocusPhysical6)
	ON_EN_SETFOCUS(IDC_PHYSICAL_7, OnSetfocusPhysical7)
	ON_EN_SETFOCUS(IDC_PHYSICAL_8, OnSetfocusPhysical8)
	ON_BN_CLICKED(IDC_TRACK_MAPPING_ENABLED, OnTrackMappingEnabled)
	//}}AFX_MSG_MAP
	ON_EN_KILLFOCUS(IDC_PHYSICAL_1, HandleKillFocus)
	ON_EN_KILLFOCUS(IDC_PHYSICAL_2, HandleKillFocus)
	ON_EN_KILLFOCUS(IDC_PHYSICAL_3, HandleKillFocus)
	ON_EN_KILLFOCUS(IDC_PHYSICAL_4, HandleKillFocus)
	ON_EN_KILLFOCUS(IDC_PHYSICAL_5, HandleKillFocus)
	ON_EN_KILLFOCUS(IDC_PHYSICAL_6, HandleKillFocus)
	ON_EN_KILLFOCUS(IDC_PHYSICAL_7, HandleKillFocus)
	ON_EN_KILLFOCUS(IDC_PHYSICAL_8, HandleKillFocus)
END_MESSAGE_MAP()

//*****************************************************************************
CPgSetupSTM::CPgSetupSTM(CApp *pApp) 
	: CWsfPropertyPage(CPgSetupSTM::IDD), m_App(*pApp)
{
	//{{AFX_DATA_INIT(CPgSetupSTM)
	//}}AFX_DATA_INIT
}

//*****************************************************************************
void CPgSetupSTM::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgSetupSTM)
	DDX_Check(pDX, IDC_TRACK_MAPPING_ENABLED, m_bTrackMappingEnabled);
	//}}AFX_DATA_MAP

	DDX_CBIndex(pDX, IDC_COM, m_nComPort);

	if (!pDX->m_bSaveAndValidate || !m_bUpdateForRemap)
	{
		for (int i=0; i<8; i++) DDX_WslText(pDX, IDC_PHYSICAL_1+i, m_TrackMap[i], 1, 8);
	}
}

//*****************************************************************************
BOOL CPgSetupSTM::OnInitDialog() 
{
	m_nComPort = m_App.m_nComPort;
	m_bTrackMappingEnabled = m_App.m_bTrackMappingEnabled;
	m_nVirtualTrackToSet = -1;
	for (int a=0; a<8; a++) m_TrackMap[a] = m_App.m_TrackMap[a] + 1;
		
	WsfSetBoldFont(this, IDC_STATIC1);
	WsfSetBoldFont(this, IDC_STATIC2);

	CWsfPropertyPage::OnInitDialog();
	UpdateControls();

	return TRUE;
}

//*****************************************************************************
BOOL CPgSetupSTM::OnKillActive(int& nNewPage) 
{
	m_bUpdateForRemap = false;
	if (!UpdateData()) return false;

	// Check for duplicities

	for (int a=0; a<8; a++) 
	{
		for (int b=0; b<8; b++) 
		{
			if (a!=b && m_TrackMap[a] == m_TrackMap[b]) 
			{
				AfxMessageBox(IDS_TWO_LANES_SAME_SENSOR);
				return false;
			}
		}
	}
	
	return true;
}

//*****************************************************************************
BOOL CPgSetupSTM::OnOkButton() 
{
	if (!CWsfPropertyPage::OnOkButton()) return false;
	m_App.m_nComPort = m_nComPort;
	m_App.m_bTrackMappingEnabled = m_bTrackMappingEnabled;
	for (int a=0; a<8; a++)  m_App.m_TrackMap[a] = m_TrackMap[a] - 1;
	return true;
}

//*****************************************************************************
void CPgSetupSTM::OnRemap() 
{
	if (m_nVirtualTrackToSet == -1) return;

	// Activate current settings, but remember previous
	// values. This is needed because MAPI_Remap... uses
	// values from setup.

	m_bUpdateForRemap = true;
	if (!UpdateData()) return;

	int nComPort = m_App.m_nComPort;
	m_App.m_nComPort = m_nComPort;

	// Get sensor

	int nResult = MAPI_RemapSingleTrack(m_nVirtualTrackToSet);
	if (nResult != -1) 
	{
		m_TrackMap[m_nVirtualTrackToSet] = nResult + 1;
		UpdateData(false);
	}

	// Restore previous settings

	m_App.m_nComPort = nComPort;

	// Return to the sensor edit line

	GetDlgItem(IDC_PHYSICAL_1 + m_nVirtualTrackToSet)->SetFocus();
}

//*****************************************************************************
void CPgSetupSTM::HandleKillFocus()
{
	if (GetWindowLong(*GetFocus(), GWL_ID) != IDC_REMAP)
	{
		m_nVirtualTrackToSet = -1; 
		UpdateControls();
	}
}

//*****************************************************************************
void CPgSetupSTM::OnSetfocusPhysical1() { m_nVirtualTrackToSet = 0; UpdateControls(); }
void CPgSetupSTM::OnSetfocusPhysical2() { m_nVirtualTrackToSet = 1; UpdateControls(); }
void CPgSetupSTM::OnSetfocusPhysical3() { m_nVirtualTrackToSet = 2; UpdateControls(); }
void CPgSetupSTM::OnSetfocusPhysical4() { m_nVirtualTrackToSet = 3; UpdateControls(); }
void CPgSetupSTM::OnSetfocusPhysical5() { m_nVirtualTrackToSet = 4; UpdateControls(); }
void CPgSetupSTM::OnSetfocusPhysical6() { m_nVirtualTrackToSet = 5; UpdateControls(); }
void CPgSetupSTM::OnSetfocusPhysical7() { m_nVirtualTrackToSet = 6; UpdateControls(); }
void CPgSetupSTM::OnSetfocusPhysical8() { m_nVirtualTrackToSet = 7; UpdateControls(); }

//*****************************************************************************
void CPgSetupSTM::OnTrackMappingEnabled() 
{
	UpdateControls();
}

//*****************************************************************************
void CPgSetupSTM::UpdateControls()
{
	CButton *p = (CButton*)GetDlgItem(IDC_TRACK_MAPPING_ENABLED);
	BOOL b = p->GetCheck();
	
	GetDlgItem(IDC_PHYSICAL_1)->EnableWindow(b);
	GetDlgItem(IDC_PHYSICAL_2)->EnableWindow(b);
	GetDlgItem(IDC_PHYSICAL_3)->EnableWindow(b);
	GetDlgItem(IDC_PHYSICAL_4)->EnableWindow(b);
	GetDlgItem(IDC_PHYSICAL_5)->EnableWindow(b);
	GetDlgItem(IDC_PHYSICAL_6)->EnableWindow(b);
	GetDlgItem(IDC_PHYSICAL_7)->EnableWindow(b);
	GetDlgItem(IDC_PHYSICAL_8)->EnableWindow(b);
	GetDlgItem(IDC_REMAP)->EnableWindow(b && m_nVirtualTrackToSet != -1);
}









