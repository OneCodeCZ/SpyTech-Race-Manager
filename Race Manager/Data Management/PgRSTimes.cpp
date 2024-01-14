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
#include "PgRSTimes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPgRSTimes, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgRSTimes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*****************************************************************************
CPgRSTimes::CPgRSTimes(CTIRS *pTIRS)
	: CWsfPropertyPage(CPgRSTimes::IDD), m_TIRS(*pTIRS)
{
	//{{AFX_DATA_INIT(CPgRSTimes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
void CPgRSTimes::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgRSTimes)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	CTimeSpan MinT(0, 0,0,15), MaxT(0, 23,59,59);
	CTimeSpan MinT2(0, 0,0,15);

	DDX_WslTime(pDX, IDC_QUAL_2, m_Times[0][1], MinT2, MaxT);
	DDX_WslTime(pDX, IDC_QUAL_3, m_Times[0][2], MinT, MaxT);

	DDX_WslTime(pDX, IDC_CONSI_1, m_Times[1][0], MinT2, MaxT);
	DDX_WslTime(pDX, IDC_CONSI_2, m_Times[1][1], MinT2, MaxT);
	DDX_WslTime(pDX, IDC_CONSI_3, m_Times[1][2], MinT, MaxT);

	DDX_WslTime(pDX, IDC_QUATER_1, m_Times[2][0], MinT2, MaxT);
	DDX_WslTime(pDX, IDC_QUATER_2, m_Times[2][1], MinT2, MaxT);
	DDX_WslTime(pDX, IDC_QUATER_3, m_Times[2][2], MinT, MaxT);

	DDX_WslTime(pDX, IDC_SEMI_1, m_Times[3][0], MinT2, MaxT);
	DDX_WslTime(pDX, IDC_SEMI_2, m_Times[3][1], MinT2, MaxT);
	DDX_WslTime(pDX, IDC_SEMI_3, m_Times[3][2], MinT, MaxT);

	DDX_WslTime(pDX, IDC_FINAL_1, m_Times[4][0], MinT2, MaxT);
	DDX_WslTime(pDX, IDC_FINAL_2, m_Times[4][1], MinT2, MaxT);
	DDX_WslTime(pDX, IDC_FINAL_3, m_Times[4][2], MinT, MaxT);
}

//*****************************************************************************
BOOL CPgRSTimes::OnInitDialog() 
{
	for (int i = 0; i<5; i++) for (int t = 0; t<3; t++) m_Times[i][t] = m_TIRS.m_Times[i][t];

	if (m_TIRS.m_bInRace && WsfGetCWorkspace()->m_pTIRace->m_nRacePart == RACE_PART_DONE)
		WsfSetDialogReadOnly(this);

	CWsfPropertyPage::OnInitDialog();
	return TRUE;
}

//*****************************************************************************
BOOL CPgRSTimes::OnKillActive(int& nNewPage) 
{
	if (!UpdateData()) return false;
	return true;
}

//*****************************************************************************
BOOL CPgRSTimes::OnOkButton() 
{
	if (!CWsfPropertyPage::OnOkButton()) return false;
	for (int i = 0; i<5; i++) for (int t = 0; t<3; t++) m_TIRS.m_Times[i][t] = m_Times[i][t];
	return true;
}





















