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
#include "PgRSQual.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPgRSQual, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgRSQual)
	ON_BN_CLICKED(IDC_TYPE, OnType)
	ON_BN_CLICKED(IDC_TYPE2, OnType2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CPgRSQual::CPgRSQual(CTIRS *pRS, CPgRSBasic *pPgBasic) 
	: CWsfPropertyPage(CPgRSQual::IDD), m_RS(*pRS)
{
	ASSERT_VALID(pRS);
	m_pPgBasic = pPgBasic;

	//{{AFX_DATA_INIT(CPgRSQual)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
void CPgRSQual::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgRSQual)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_Radio(pDX, IDC_TYPE, m_nPrimaryEval);
	
	if (m_nPrimaryEval == RS_EVAL_NUM_LAPS)
	{
		if (!pDX->m_bSaveAndValidate) OnType();
		DDX_WslText(pDX, IDC_DATA, m_nNumRides, 1, 4);
	}
	else
	{
		if (!pDX->m_bSaveAndValidate) OnType2();
		DDX_WslText(pDX, IDC_DATA, m_nNumAttempts, 1, 2);
	}
}

//*****************************************************************************
BOOL CPgRSQual::OnInitDialog() 
{
	m_nPrimaryEval = m_RS.m_nPrimaryEval;
	m_nNumRides = m_RS.m_nNumRides;
	m_nNumAttempts = m_RS.m_nNumAttempts;

	if (m_pPgBasic->m_bInRace)
	{
		if (WsfGetCWorkspace()->m_pTIRace->m_nRacePart == RACE_PART_DONE) 
			WsfSetDialogReadOnly(this);
	}

	CWsfPropertyPage::OnInitDialog();
	return TRUE;
}

//*****************************************************************************
void CPgRSQual::OnType() 
{
	GetDlgItem(IDC_TEXT)->SetWindowText(CString((char*)IDS_NUM_RIDES));
}

//*****************************************************************************
void CPgRSQual::OnType2() 
{
	GetDlgItem(IDC_TEXT)->SetWindowText(CString((char*)IDS_NUM_ATTEMPTS));
}

//*****************************************************************************
BOOL CPgRSQual::OnSetActive() 
{
	BOOL b = m_pPgBasic->m_bQual;

	GetDlgItem(IDC_TYPE)->EnableWindow(b /*false*/);
	GetDlgItem(IDC_TYPE2)->EnableWindow(b);
	GetDlgItem(IDC_DATA)->EnableWindow(b);

	return CWsfPropertyPage::OnSetActive();
}

//*****************************************************************************
BOOL CPgRSQual::OnKillActive(int& nNewPage) 
{
	return UpdateData();
}

//*****************************************************************************
BOOL CPgRSQual::OnOkButton() 
{
	if (!CWsfPropertyPage::OnOkButton()) return false;
	m_RS.m_nPrimaryEval = m_nPrimaryEval;
	m_RS.m_nNumRides = m_nNumRides;
	m_RS.m_nNumAttempts = m_nNumAttempts;
	
	return true;
}







