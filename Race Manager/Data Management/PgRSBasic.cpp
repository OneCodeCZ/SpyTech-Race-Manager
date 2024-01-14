///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2004 06
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "PgRSBasic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPgRSBasic, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgRSBasic)
	ON_BN_CLICKED(IDC_PART_CONSI, OnPartConsi)
	ON_BN_CLICKED(IDC_PART_QUATER, OnPartQuater)
	ON_BN_CLICKED(IDC_PART_SEMI, OnPartSemi)
	ON_BN_CLICKED(IDC_TYPE, OnType)
	ON_BN_CLICKED(IDC_TYPE2, OnType2)
	ON_BN_CLICKED(IDC_PART_QUAL, OnPartQual)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CPgRSBasic::CPgRSBasic(CTIRS *pRS, BOOL bInRace) 
	: CWsfPropertyPage(CPgRSBasic::IDD), m_RS(*pRS)
{
	ASSERT_VALID(pRS);
	m_bInRace = bInRace;

	//{{AFX_DATA_INIT(CPgRSBasic)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
void CPgRSBasic::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgRSBasic)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_WslText(pDX, IDC_NAME, m_strName, DB_RS_NAME_MAX_CHARS);
	DDX_Check(pDX, IDC_PART_QUAL, m_bQual);
	DDX_CBIndex(pDX, IDC_DISTRIBUTION_TYPE, m_nDistributionType);
	DDX_WslText(pDX, IDC_CHANGING, m_strChanging, MAX_LANES, false, true);

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
BOOL CPgRSBasic::OnInitDialog() 
{
	m_strName = m_RS.m_strName;
	m_nFirstPart = m_RS.m_nFirstPart;
	m_bQual = m_RS.m_bQual;
	m_nDistributionType = m_RS.m_nDistributionType;
	m_strChanging = m_RS.m_strChanging;

	m_nPrimaryEval = m_RS.m_nPrimaryEval;
	m_nNumRides = m_RS.m_nNumRides;
	m_nNumAttempts = m_RS.m_nNumAttempts;

	if (m_bInRace)
	{
		if (WsfGetCWorkspace()->m_pTIRace->m_nRacePart == RACE_PART_DONE) 
			WsfSetDialogReadOnly(this);
		else
			WsfSetReadOnly(*GetDlgItem(IDC_NAME), true);
	}

   WsfSetBoldFont(this, IDC_STATIC1);
   WsfSetBoldFont(this, IDC_STATIC2);

	// Final part is compulsory

	CButton *p = (CButton*)GetDlgItem(IDC_PART_FINAL);
	p->SetCheck(true);
	WsfSetReadOnly(*p);

	// Dividing combo-box

	WsfFillComboBox((CComboBox*)GetDlgItem(IDC_DISTRIBUTION_TYPE), IDS_DIVIDING_COMBO);

   // Let's go!

	CWsfPropertyPage::OnInitDialog();
	UpdateUI();
	return TRUE;
}

//*****************************************************************************
void CPgRSBasic::UpdateUI()
{
	CButton *p;

   // Parts check-boxes

	p = (CButton*) GetDlgItem(IDC_PART_CONSI);
	p->SetCheck(m_nFirstPart <= RACE_PART_CONSI);

	p = (CButton*) GetDlgItem(IDC_PART_QUATER);
	p->SetCheck(m_nFirstPart <= RACE_PART_QUATER);

	p = (CButton*) GetDlgItem(IDC_PART_SEMI);
	p->SetCheck(m_nFirstPart <= RACE_PART_SEMI);

   // Qual controls

	p = (CButton*) GetDlgItem(IDC_PART_QUAL);
   BOOL b = p->GetCheck();

	GetDlgItem(IDC_TYPE)->EnableWindow(b);
	GetDlgItem(IDC_TYPE2)->EnableWindow(b);
	GetDlgItem(IDC_DATA)->EnableWindow(b);
}

/*****************************************************************************/
void CPgRSBasic::OnPartQual() 
{
   UpdateUI();
}

//*****************************************************************************
void CPgRSBasic::OnPartConsi() 
{
	if (m_nFirstPart <= RACE_PART_CONSI)
		m_nFirstPart = RACE_PART_QUATER;
	else
		m_nFirstPart = RACE_PART_CONSI;
	UpdateUI();
}

//*****************************************************************************
void CPgRSBasic::OnPartQuater() 
{
	if (m_nFirstPart <= RACE_PART_QUATER)
		m_nFirstPart = RACE_PART_SEMI;
	else
		m_nFirstPart = RACE_PART_QUATER;
	UpdateUI();
}

//*****************************************************************************
void CPgRSBasic::OnPartSemi() 
{	
	if (m_nFirstPart <= RACE_PART_SEMI)
		m_nFirstPart = RACE_PART_FINAL;
	else
		m_nFirstPart = RACE_PART_SEMI;
	UpdateUI();
}

//*****************************************************************************
BOOL CPgRSBasic::OnKillActive(int& nNewPage) 
{
	if (!UpdateData()) return false;
	
	// Check for duplicity in name

	if (!m_bInRace)
	{
		CTIRS *pItem = g_pApp->m_pTIDbRSFolder->FindRS(m_strName);

		if (pItem && pItem != &m_RS) 
		{
			AfxMessageBox(IDS_RS_EXISTS);
			WsfPrepareForEdit(GetDlgItem(IDC_NAME));
			return false;
		}
	}

	// Check changing string syntax
   // No lane twice, only valid lanes

	bool b[MAX_LANES];
	int nLen = m_strChanging.GetLength();
	for (int i=0; i<MAX_LANES; i++) b[i] = false;

	for (i=0; i<nLen; i++)
	{
		int n = m_strChanging[i];
		if (n < '1' || n > '0'+MAX_LANES) break;
		n -= '1';
		if (b[n]) break;
		b[n] = true;
	}
	
	if (i < nLen)
	{
		AfxMessageBox(IDS_BAD_CHANGING_FORMAT);
		WsfPrepareForEdit(GetDlgItem(IDC_CHANGING));
		return false;
	}

	return true;
}

//*****************************************************************************
void CPgRSBasic::OnType() 
{
	GetDlgItem(IDC_TEXT)->SetWindowText(CString((char*)IDS_NUM_RIDES));
}

//*****************************************************************************
void CPgRSBasic::OnType2() 
{
	GetDlgItem(IDC_TEXT)->SetWindowText(CString((char*)IDS_NUM_ATTEMPTS));
}

//*****************************************************************************
BOOL CPgRSBasic::OnOkButton() 
{
	if (!CWsfPropertyPage::OnOkButton()) return false;

	m_RS.m_strName = m_strName;
	m_RS.m_nFirstPart = m_nFirstPart;
	m_RS.m_bQual = m_bQual;
	m_RS.m_nDistributionType = m_nDistributionType;
	m_RS.m_strChanging = m_strChanging;
	
	m_RS.m_nPrimaryEval = m_nPrimaryEval;
	m_RS.m_nNumRides = m_nNumRides;
	m_RS.m_nNumAttempts = m_nNumAttempts;

	return true;
}

