///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////
//

#include "stdafx.h"
#include "main.h"
#include "PgREStartData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPgREStartData, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgREStartData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CPgREStartData::CPgREStartData(CTIRaceEntity *pTIRE) 
	: CWsfPropertyPage(CPgREStartData::IDD), m_TIRE(*pTIRE)
{
	//{{AFX_DATA_INIT(CPgREStartData)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
void CPgREStartData::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgREStartData)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_WslText(pDX, IDC_RANKING, m_nStartRanking, 0, 1000);
}


//*****************************************************************************
BOOL CPgREStartData::OnInitDialog() 
{
	m_nStartRanking = m_TIRE.m_nStartRanking;

	if (WsfGetCWorkspace()->m_pTIRace->m_nRacePart != RACE_PART_NONE)
		WsfSetDialogReadOnly(this);

	GetDlgItem(IDC_TITLE)->SetWindowText(m_TIRE.GetTitle());

	CWsfPropertyPage::OnInitDialog();
	return TRUE;
}

//*****************************************************************************
BOOL CPgREStartData::OnKillActive(int& nNewPage) 
{
	if (!UpdateData()) return false;
	return true;
}

//*****************************************************************************
BOOL CPgREStartData::OnOkButton() 
{
	if (!CWsfPropertyPage::OnOkButton()) return false;

	m_TIRE.m_nStartRanking = m_nStartRanking;
	m_TIRE.m_nRanking = m_nStartRanking;
	
	g_pApp->SetWorkspaceModified();

	return true;
}

