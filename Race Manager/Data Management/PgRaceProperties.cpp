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
#include "PgRaceProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPgRaceProperties, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgRaceProperties)
	ON_CBN_SELCHANGE(IDC_RACE_TYPE, OnSelchangeRaceType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CPgRaceProperties::CPgRaceProperties(CTIRace *pRace)
	: CWsfPropertyPage(CPgRaceProperties::IDD), m_Race(*pRace)
{
	//{{AFX_DATA_INIT(CPgRaceProperties)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
void CPgRaceProperties::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgRaceProperties)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_WslText(pDX, IDC_NUMBER, m_strNumber, 32, true);
	DDX_WslText(pDX, IDC_NAME, m_strName, RACE_NAME_MAX_CHARS, false);
	DDX_WslText(pDX, IDC_ORGANIZER, m_strOrganizer, RACE_ORGANIZER_MAX_CHARS, true);
	DDX_WslText(pDX, IDC_DATE, m_Date);
	DDX_WslText(pDX, IDC_PLACE, m_strPlace, 64, true);
	DDX_WslText(pDX, IDC_JUDGE, m_strJudge, 64, true);

	DDX_CBIndex(pDX, IDC_RACE_TYPE, (int)m_bTeams);
   DDX_WslText(pDX, IDC_MIN_MEMBERS, m_nMinMembers, 1, 100);
	DDX_WslText(pDX, IDC_MAX_MEMBERS, m_nMaxMembers, 1, 100);

	DDX_WslText(pDX, IDC_MIN_TIME, m_nLapMinTime, 0, 10000);
}

//*****************************************************************************
BOOL CPgRaceProperties::OnInitDialog() 
{
	m_strName = m_Race.m_strName;
	m_strOrganizer = m_Race.m_strOrganizer;
	m_bTeams = m_Race.m_bTeams;
	m_Date = m_Race.m_Date;
	m_nLapMinTime = m_Race.m_nLapMinTime;
	m_nMinMembers = m_Race.m_nMinMembers;
	m_nMaxMembers = m_Race.m_nMaxMembers;

	m_strJudge = m_Race.m_strJudge;
	m_strNumber = m_Race.m_strNumber;
	m_strPlace = m_Race.m_strPlace;

	// Set race type radio buttons read only, 
	// if we DON'T edit it for the first time

	if (!m_strName.IsEmpty()) WsfSetReadOnly(*GetDlgItem(IDC_RACE_TYPE));

	// Don't allow editing when race is finished

	if (m_Race.m_nRacePart == RACE_PART_DONE) WsfSetDialogReadOnly(this);

	WsfSetBoldFont(this, IDC_STATIC1);
	WsfSetBoldFont(this, IDC_STATIC2);
	WsfSetBoldFont(this, IDC_STATIC3);

	// Fill race_type combo

	WsfFillComboBox(((CComboBox*)GetDlgItem(IDC_RACE_TYPE)), IDS_RACE_TYPE_COMBO);

	
	CWsfPropertyPage::OnInitDialog();
	OnSelchangeRaceType();
	return TRUE;
}

//*****************************************************************************
void CPgRaceProperties::OnSelchangeRaceType()
{
	int i = ((CComboBox*)GetDlgItem(IDC_RACE_TYPE))->GetCurSel();
	GetDlgItem(IDC_MIN_MEMBERS)->EnableWindow(i == 1);
	GetDlgItem(IDC_MAX_MEMBERS)->EnableWindow(i == 1);
}

//*****************************************************************************
BOOL CPgRaceProperties::OnKillActive() 
{
	if (!UpdateData()) return false;

	if (m_bTeams)
	{
		if (m_nMinMembers > m_nMaxMembers)
		{
			AfxMessageBox(IDS_MIN_LESS_THAN_MAX);
			WsfPrepareForEdit(GetDlgItem(IDC_MIN_MEMBERS));
			return false;
		}

		// Check if there is no team with more drivers

		CWsfTreeItem *pItem = WsfGetCWorkspace()->m_pTIRace->GetChild();
		while (pItem)
		{
			if (pItem->IsKindOf(RUNTIME_CLASS(CTIRaceTeam)))
			{
				int n = ((CTIRaceTeam*)pItem)->GetDrvCount();
				if (n > m_nMaxMembers)
				{
					AfxMessageBox(IDS_TEAM_WITH_MORE_DRVS_EXISTS);
					WsfPrepareForEdit(GetDlgItem(IDC_MAX_MEMBERS));
					return false;
				}
			}
			pItem = pItem->GetNextSibling();
		}
	}

	return true;
}

//*****************************************************************************
BOOL CPgRaceProperties::OnOkButton() 
{
	if (!CWsfPropertyPage::OnOkButton()) return false;

	m_Race.m_strName = m_strName;
	m_Race.m_strOrganizer = m_strOrganizer;
	m_Race.m_bTeams = m_bTeams;
	m_Race.m_Date = m_Date;
	m_Race.m_nLapMinTime = m_nLapMinTime;
	m_Race.m_nMinMembers = m_nMinMembers;
	m_Race.m_nMaxMembers = m_nMaxMembers;

	m_Race.m_strJudge = m_strJudge;
	m_Race.m_strNumber = m_strNumber;
	m_Race.m_strPlace = m_strPlace;

	g_pApp->SetWorkspaceName(m_strName);

	return true;
}



