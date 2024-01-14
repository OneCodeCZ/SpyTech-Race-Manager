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
#include "PgRSAdvances.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPgRSAdvances, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgRSAdvances)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CPgRSAdvances::CPgRSAdvances(CTIRS *pRS, CPgRSBasic *pPgBasic) 
	: CWsfPropertyPage(CPgRSAdvances::IDD), m_RS(*pRS)
{
	ASSERT_VALID(pRS);
	m_pPgBasic = pPgBasic;

	//{{AFX_DATA_INIT(CPgRSAdvances)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
void CPgRSAdvances::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgRSAdvances)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_WslText(pDX, IDC_QUAL_QUATER, m_nAdvancesQual[0], 0, MAX_ENTITIES);
	DDX_WslText(pDX, IDC_QUAL_SEMI, m_nAdvancesQual[1], 0, MAX_ENTITIES);
	DDX_WslText(pDX, IDC_QUAL_FINAL, m_nAdvancesQual[2], 0, MAX_ENTITIES);

	DDX_WslText(pDX, IDC_CONSI_BEST, m_nAdvances[0][0], 1, MAX_ENTITIES);
	DDX_WslText(pDX, IDC_CONSI_GROUP, m_nAdvances[0][1], 0, MAX_ENTITIES);

	DDX_WslText(pDX, IDC_QUATER_BEST, m_nAdvances[1][0], 1, MAX_ENTITIES);
	DDX_WslText(pDX, IDC_QUATER_GROUP, m_nAdvances[1][1], 0, MAX_ENTITIES);

	DDX_WslText(pDX, IDC_SEMI_BEST, m_nAdvances[2][0], 1, MAX_ENTITIES);
	DDX_WslText(pDX, IDC_SEMI_GROUP, m_nAdvances[2][1], 0, MAX_ENTITIES);
}

//*****************************************************************************
BOOL CPgRSAdvances::OnInitDialog() 
{
	memcpy(&m_nAdvancesQual, m_RS.m_nAdvancesQual, sizeof(m_nAdvancesQual));
	memcpy(&m_nAdvances, m_RS.m_nAdvances, sizeof(m_nAdvances));

	if (m_pPgBasic->m_bInRace)
	{
		if (WsfGetCWorkspace()->m_pTIRace->m_nRacePart == RACE_PART_DONE) 
			WsfSetDialogReadOnly(this);
	}

	CWsfPropertyPage::OnInitDialog();
	return TRUE;
}

//*****************************************************************************
BOOL CPgRSAdvances::OnSetActive() 
{
	BOOL b, b1, b2, b3;

	// Qual;

	b = m_pPgBasic->m_bQual;
	b1 = m_pPgBasic->m_nFirstPart < RACE_PART_QUATER;
	b2 = m_pPgBasic->m_nFirstPart < RACE_PART_SEMI;
	b3 = m_pPgBasic->m_nFirstPart < RACE_PART_FINAL;

	GetDlgItem(IDC_QUAL_QUATER)->EnableWindow(b && b1);
	GetDlgItem(IDC_QUAL_SEMI)->EnableWindow(b && b2);
	GetDlgItem(IDC_QUAL_FINAL)->EnableWindow(b && b3);

	// Other parts

	b = m_pPgBasic->m_nFirstPart <= RACE_PART_CONSI;
	b1 = m_pPgBasic->m_nFirstPart <= RACE_PART_QUATER;
	b2 = m_pPgBasic->m_nFirstPart <= RACE_PART_SEMI;

	GetDlgItem(IDC_CONSI_BEST)->EnableWindow(b);
	GetDlgItem(IDC_CONSI_GROUP)->EnableWindow(b);
	
	GetDlgItem(IDC_QUATER_BEST)->EnableWindow(b1);
	GetDlgItem(IDC_QUATER_GROUP)->EnableWindow(b1);

	GetDlgItem(IDC_SEMI_BEST)->EnableWindow(b2);
	GetDlgItem(IDC_SEMI_GROUP)->EnableWindow(b2);
	
	return CWsfPropertyPage::OnSetActive();
}

//*****************************************************************************
BOOL CPgRSAdvances::OnKillActive(int& nNewPage) 
{
	int nPart, nEntities, nAdvance;

	if (!UpdateData()) return false;

	// Check the total entities in parts.
	// There must be at most the same number of entities in a part than 
	// in the previous part.

	nPart = RACE_PART_QUATER;

	for (int i = 1; i < 3;i++)
	{
		if (nPart > m_pPgBasic->m_nFirstPart)
		{
			nAdvance = m_nAdvances[i][0];
			nEntities = m_nAdvances[i-1][0];
			if (m_pPgBasic->m_bQual) nEntities += m_nAdvancesQual[i-1];

			if (nAdvance > nEntities)
			{
				AfxMessageBox(IDS_MORE_ADVANCES_THAN_COUNT);
				return false;
			}
		}
	}

	return true;
}

//*****************************************************************************
BOOL CPgRSAdvances::OnOkButton() 
{
	if (!CWsfPropertyPage::OnOkButton()) return false;

	memcpy(&m_RS.m_nAdvancesQual, m_nAdvancesQual, sizeof(m_nAdvancesQual));
	memcpy(&m_RS.m_nAdvances, m_nAdvances, sizeof(m_nAdvances));
	
	return true;
}
