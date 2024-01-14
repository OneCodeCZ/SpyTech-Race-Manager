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
#include "PgSetupTrack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPgSetupTrack, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgSetupTrack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CPgSetupTrack::CPgSetupTrack(CSetupTrack *pRM, BOOL bReadOnly) 
	: CWsfPropertyPage(CPgSetupTrack::IDD), m_RM(*pRM)
{
	m_bReadOnly = bReadOnly;

	//{{AFX_DATA_INIT(CPgSetupTrack)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
void CPgSetupTrack::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgSetupTrack)
	//}}AFX_DATA_MAP

  DDX_Control(pDX, IDC_LANE1, m_Lane[0]);
  DDX_Control(pDX, IDC_LANE2, m_Lane[1]);
  DDX_Control(pDX, IDC_LANE3, m_Lane[2]);
  DDX_Control(pDX, IDC_LANE4, m_Lane[3]);
  DDX_Control(pDX, IDC_LANE5, m_Lane[4]);
  DDX_Control(pDX, IDC_LANE6, m_Lane[5]);
  DDX_Control(pDX, IDC_LANE7, m_Lane[6]);
  DDX_Control(pDX, IDC_LANE8, m_Lane[7]);

	DDX_WslText(pDX, IDC_OWNER, m_strOwner, RACE_ORGANIZER_MAX_CHARS);
	DDX_WslText(pDX, IDC_TRACK_NAME, m_strTrackName, RACE_TRACK_MAX_CHARS);
	DDX_WslText(pDX, IDC_TRACK_LENGTH, m_nTrackLength, 1, 1000*100);
	DDX_CBIndex(pDX, IDC_NUM_LANES, m_nNumLanes);
	DDX_CBIndex(pDX, IDC_DIVISION_TYPE, m_nDivisionType);

	DDX_CBIndex(pDX, IDC_LANE1, m_nLaneColor[0]);
	DDX_CBIndex(pDX, IDC_LANE2, m_nLaneColor[1]);
	DDX_CBIndex(pDX, IDC_LANE3, m_nLaneColor[2]);
	DDX_CBIndex(pDX, IDC_LANE4, m_nLaneColor[3]);
	DDX_CBIndex(pDX, IDC_LANE5, m_nLaneColor[4]);
	DDX_CBIndex(pDX, IDC_LANE6, m_nLaneColor[5]);
	DDX_CBIndex(pDX, IDC_LANE7, m_nLaneColor[6]);
	DDX_CBIndex(pDX, IDC_LANE8, m_nLaneColor[7]);
}

//*****************************************************************************
BOOL CPgSetupTrack::OnInitDialog() 
{
	m_strOwner = m_RM.m_strOwner;
	m_strTrackName = m_RM.m_strTrackName;
	m_nNumLanes = m_RM.m_nNumLanes;
	m_nTrackLength = m_RM.m_nTrackLength;
	m_nDivisionType = m_RM.m_nDivisionType;
	memcpy(&m_nLaneColor, &m_RM.m_nLaneColor, sizeof(m_nLaneColor));

	if (m_bReadOnly) WsfSetDialogReadOnly(this);
	
	WsfSetBoldFont(this, IDC_STATIC1);
	WsfSetBoldFont(this, IDC_STATIC2);
	
	WsfFillComboBox((CComboBox*)GetDlgItem(IDC_DIVISION_TYPE), IDS_DIVISION_UNITS_COMPO);

	CWsfPropertyPage::OnInitDialog();
	return TRUE;
}

//*****************************************************************************
BOOL CPgSetupTrack::OnOkButton() 
{
	if (!CWsfPropertyPage::OnOkButton()) return false;
	if (!m_bReadOnly)
	{
		m_RM.m_strOwner = m_strOwner;
		m_RM.m_strTrackName = m_strTrackName;
		m_RM.m_nNumLanes = m_nNumLanes;
		m_RM.m_nTrackLength = m_nTrackLength;
		m_RM.m_nDivisionType = m_nDivisionType;
		memcpy(&m_RM.m_nLaneColor, &m_nLaneColor, sizeof(m_nLaneColor));
	}
	
	return true;
}






