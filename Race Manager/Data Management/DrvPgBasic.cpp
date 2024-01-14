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
#include "DrvPgBasic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPgDrvBasic, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgDrvBasic)
	ON_BN_CLICKED(IDC_MEMBER_OF_UNION, OnMemberOfUnion)
	ON_EN_CHANGE(IDC_SURNAME, OnChangeNameSurname)
	ON_EN_KILLFOCUS(IDC_NICKNAME, OnKillfocusNickname)
	ON_EN_CHANGE(IDC_NAME, OnChangeNameSurname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CPgDrvBasic::CPgDrvBasic(CDbDrv *pDrv, BOOL bInRace, CWsfTreeItem* pTIDrv) 
	: CWsfPropertyPage(CPgDrvBasic::IDD)
{
	ASSERT(m_pTIDrv);
	m_pDrv = pDrv;
	m_TempDrv = *pDrv;
	m_bInRace = bInRace;
	m_pTIDrv = pTIDrv;

	//{{AFX_DATA_INIT(CPgDrvBasic)
	//}}AFX_DATA_INIT
}

//*****************************************************************************
void CPgDrvBasic::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgDrvBasic)
	//}}AFX_DATA_MAP

	DDX_WslText(pDX, IDC_NAME, m_TempDrv.m_szName, DB_DRV_NAME_CHARS);
	DDX_WslText(pDX, IDC_SURNAME, m_TempDrv.m_szSurname, DB_DRV_SURNAME_CHARS);
	DDX_WslText(pDX, IDC_NICKNAME, m_TempDrv.m_szNickname, DB_DRV_NICKNAME_CHARS);
	DDX_WslText(pDX, IDC_CLUB, m_TempDrv.m_szClubCountry, DB_DRV_CLUB_CHARS, !m_TempDrv.m_bMemberOfAssoc);
	DDX_WslText(pDX, IDC_LICENCE_NUMBER, m_TempDrv.m_szLicence, DB_DRV_LICENCE_CHARS);
}

//*****************************************************************************
BOOL CPgDrvBasic::OnInitDialog() 
{
	// Init the club/country combo box with values from all other
	// drivers in the database

	CComboBox* pCombo = (CComboBox*) GetDlgItem(IDC_CLUB);
	CWslArray<CWsfTreeItem*> a;
	WsfGetCApp()->m_pTIDbDrvFolder->EnumChildItems(a, true);

	for (int i=0; i<a.GetSize(); i++)
	{
		CTIDriver* pDrv = dynamic_cast<CTIDriver*>(a[i]);
		if (pDrv && 
			pDrv->m_Drv.m_szClubCountry[0] &&
			pCombo->FindStringExact(-1, pDrv->m_Drv.m_szClubCountry)==CB_ERR)
		{
			pCombo->AddString(pDrv->m_Drv.m_szClubCountry);
		}
	}

	// Member of assoc check box

	GetDlgItem(IDC_LICENCE_NUMBER)->EnableWindow(m_TempDrv.m_bMemberOfAssoc);
	((CButton*)GetDlgItem(IDC_MEMBER_OF_UNION))->SetCheck(m_TempDrv.m_bMemberOfAssoc);

	// Update UI

	if (m_bInRace && WsfGetCWorkspace()->m_pTIRace->m_nRacePart == RACE_PART_DONE) 
		WsfSetDialogReadOnly(this);

	CWsfPropertyPage::OnInitDialog();
	OnKillfocusNickname();

	return TRUE;
}

//*****************************************************************************
BOOL CPgDrvBasic::OnKillActive(int& nNewPage) 
{
	if (!UpdateData()) return false;

	if (!m_bInRace)
	{
		// Check for name, surname and nickname duplicity
		// in the driver's folder
		
		CTIDrvFolder* pFolder = (CTIDrvFolder*) m_pTIDrv->GetParent();
		CTIDriver* pDrv = pFolder->FindDriver(m_TempDrv.m_szName, 
			m_TempDrv.m_szSurname, m_TempDrv.m_szNickname, false);

		if (pDrv && &pDrv->m_Drv != m_pDrv) 
		{
			AfxMessageBox(IDS_DUPLICATE_DRIVER);
			WsfPrepareForEdit(GetDlgItem(IDC_NAME));
			return false;
		}
	}
	else
	{
		// Check for nickname duplicity
		
		CTIRaceEntity* pRE = WsfGetCWorkspace()->m_pTIRace->FindEntity(CString(m_TempDrv.m_szNickname));

		if (pRE && pRE != m_pTIDrv)
		{
			AfxMessageBox(IDS_DUPLICATE_NICKNAME);
			WsfPrepareForEdit(GetDlgItem(IDC_NICKNAME));
			return false;
		}
	}

	return true;
}

//*****************************************************************************
BOOL CPgDrvBasic::OnOkButton()
{
	if (!CWsfPropertyPage::OnOkButton()) return false;

	// Copy the changes

	*m_pDrv = m_TempDrv;

	return true;
}

//*****************************************************************************
void CPgDrvBasic::OnMemberOfUnion() 
{
	m_TempDrv.m_bMemberOfAssoc ^= true;	
	GetDlgItem(IDC_LICENCE_NUMBER)->EnableWindow(m_TempDrv.m_bMemberOfAssoc);
}

//*****************************************************************************
void CPgDrvBasic::OnChangeNameSurname() 
{
	CString a,b;

	if (m_bTheSame) 
	{
		GetDlgItem(IDC_NAME)->GetWindowText(a);
		GetDlgItem(IDC_SURNAME)->GetWindowText(b);
		GetDlgItem(IDC_NICKNAME)->SetWindowText(a+' '+b);
	}
}

//*****************************************************************************
void CPgDrvBasic::OnKillfocusNickname() 
{
	CString a,b,c;

	GetDlgItem(IDC_NAME)->GetWindowText(a);
	GetDlgItem(IDC_SURNAME)->GetWindowText(b);
	GetDlgItem(IDC_NICKNAME)->GetWindowText(c);

	m_bTheSame = c.IsEmpty() || (a+' '+b==c);
}

