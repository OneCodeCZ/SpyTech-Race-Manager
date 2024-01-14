///////////////////////////////////////////////////////////////////////////////
//
// Race Manager
// ------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "PgTeamBasic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPgTeamBasic, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgTeamBasic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CPgTeamBasic::CPgTeamBasic(CTIRaceTeam *pTeam) 
	: CWsfPropertyPage(CPgTeamBasic::IDD), m_Team(*pTeam)
{
	//{{AFX_DATA_INIT(CPgTeamBasic)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
CPgTeamBasic::~CPgTeamBasic()
{
}

//*****************************************************************************
void CPgTeamBasic::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgTeamBasic)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_WslText(pDX, IDC_NAME, m_strName, MAX_TEAM_NAME_CHARS);
}

//*****************************************************************************
BOOL CPgTeamBasic::OnInitDialog() 
{
	m_strName = m_Team.m_strName;

	CWsfPropertyPage::OnInitDialog();
	return TRUE;
}

//*****************************************************************************
BOOL CPgTeamBasic::OnKillActive(int& nNewPage) 
{
	if (!UpdateData()) return false;

	// Check for duplicities

	CTIRaceEntity *p = WsfGetCWorkspace()->m_pTIRace->FindEntity(m_strName);
	if (p && p != &m_Team)
	{
		AfxMessageBox(IDS_TEAM_EXISTS);
		WsfPrepareForEdit(GetDlgItem(IDC_NAME));
		return false;
	}

	return true;
}

//*****************************************************************************
BOOL CPgTeamBasic::OnOkButton() 
{
	if (!CWsfPropertyPage::OnOkButton()) return false;
	m_Team.m_strName = m_strName;
	return true;
}
