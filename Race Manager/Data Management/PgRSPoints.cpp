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
#include "PgRSPoints.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CPgRSPoints, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgRSPoints)
	ON_BN_CLICKED(IDC_ADD_BEFORE, OnAddBefore)
	ON_BN_CLICKED(IDC_ADD_AFTER, OnAddAfter)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST, OnBeginlabeleditList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST, OnEndlabeleditList)
	ON_CBN_SELCHANGE(IDC_POINTS_TYPE, OnSelchangePointsType)
	ON_BN_CLICKED(IDC_ENABLE_POINTS, OnEnablePoints)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CPgRSPoints::CPgRSPoints(CTIRS *pRS) 
	: CWsfPropertyPage(CPgRSPoints::IDD), m_RS(*pRS)
{
	//{{AFX_DATA_INIT(CPgRSPoints)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
CPgRSPoints::~CPgRSPoints()
{
}

//*****************************************************************************
void CPgRSPoints::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgRSPoints)
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_CBIndex(pDX, IDC_POINTS_TYPE, m_nPointsType);
	DDX_Check(pDX, IDC_ENABLE_POINTS, m_bEnablePoints);
	//}}AFX_DATA_MAP
}

//*****************************************************************************
BOOL CPgRSPoints::OnInitDialog() 
{
	m_nPointsType = m_RS.m_nPointsType;
	m_bEnablePoints = m_RS.m_bEnablePoints;
	if (!m_nPointsType) m_aOwnPoints = m_RS.m_aPoints;

	WsfFillComboBox((CComboBox*)GetDlgItem(IDC_POINTS_TYPE), IDS_POINT_SCHEMA_COMBO);

	CWsfPropertyPage::OnInitDialog();
	OnEnablePoints();

	// Don't allow editing when race is finished

	if (m_RS.m_bInRace && WsfGetCWorkspace()->m_pTIRace->m_nRacePart == RACE_PART_DONE) 
		WsfSetDialogReadOnly(this);

	// Init list

	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	CString s;
	s.LoadString(IDS_POINTS);
	m_List.InsertColumn(0, s, LVCFMT_LEFT, 130);
	s.LoadString(IDS_RANK);
	m_List.InsertColumn(1, s, LVCFMT_LEFT, 60);

	int nOrder[] = { 1, 0 };
	m_List.SetColumnOrderArray(2, nOrder);

	BuildList(m_RS.m_aPoints);
	return TRUE;
}

//*****************************************************************************
void CPgRSPoints::BuildList(CWslArray<int> &aPoints)
{
	m_List.DeleteAllItems();

	for (int i=0; i < aPoints.GetSize(); i++)
	{
		itoa(aPoints[i], temp, 10);
		m_List.InsertItem(i, temp);
		wsprintf(temp, "%d.", i+1);
		m_List.SetItem(i, 1, LVIF_TEXT, temp, 0,0,0,0);
	}
}

//*****************************************************************************
void CPgRSPoints::BuildArrayFromList(CWslArray<int> &aPoints)
{
	int i = m_List.GetItemCount();
	aPoints.SetSize(i);
	
	for (;i--;)
	{
		m_List.GetItemText(i, 0, temp, 1024);
		aPoints[i] = atoi(temp);
	}
}

//*****************************************************************************
BOOL CPgRSPoints::OnOkButton() 
{
	if (!CWsfPropertyPage::OnOkButton()) return false;

	m_RS.m_nPointsType = m_nPointsType;
	m_RS.m_bEnablePoints = m_bEnablePoints;
	BuildArrayFromList(m_RS.m_aPoints);

	return true;
}

//*****************************************************************************
void CPgRSPoints::OnEdit() 
{
	POSITION p = (POSITION) m_List.GetFirstSelectedItemPosition();
	if (!p) return;
	m_List.SetFocus();
	m_List.EditLabel(m_List.GetNextSelectedItem(p));
}

//*****************************************************************************
void CPgRSPoints::InsertAt(int nPos)
{
	// Update own points array

	if (!m_nPointsType) 
	{
		// Add item to the own point array
		m_aOwnPoints.InsertAt(nPos, 0);
	}
	else
	{
		// Change to own points scheme
		((CComboBox*)GetDlgItem(IDC_POINTS_TYPE))->SetCurSel(0);		
		m_nPointsType = 0;
		BuildArrayFromList(m_aOwnPoints);
	}

	// Update list ctrl

	BuildList(m_aOwnPoints);

	// Start edititng

	m_List.SetFocus();
	m_List.EditLabel(nPos);
}

//*****************************************************************************
void CPgRSPoints::OnAddAfter() 
{
	POSITION p = m_List.GetFirstSelectedItemPosition();
	int i = p ? m_List.GetNextSelectedItem(p) + 1 : m_List.GetItemCount();
	InsertAt(i);
}

//*****************************************************************************
void CPgRSPoints::OnAddBefore() 
{
	POSITION p = m_List.GetFirstSelectedItemPosition();
	int i = p ? m_List.GetNextSelectedItem(p) : 0;
	InsertAt(i);
}

//*****************************************************************************
void CPgRSPoints::OnRemove() 
{
	// Remove item from list

	POSITION p = m_List.GetFirstSelectedItemPosition();
	if (!p) return;
	int i = m_List.GetNextSelectedItem(p);

	if (!m_nPointsType) 
	{
		// Remove from own points
		m_aOwnPoints.RemoveAt(i);
	}
	else
	{
		// Change to own points scheme
		((CComboBox*)GetDlgItem(IDC_POINTS_TYPE))->SetCurSel(0);
		m_nPointsType = 0;
		BuildArrayFromList(m_aOwnPoints);
	}

	// Update list ctrl

	BuildList(m_aOwnPoints);
}

//*****************************************************************************
void CPgRSPoints::OnBeginlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CEdit *pEdit = m_List.GetEditControl();
	pEdit->ModifyStyle(0, ES_NUMBER);
	pEdit->SendMessage(EM_LIMITTEXT, 3, 0);
	*pResult = 0;
}

//*****************************************************************************
void CPgRSPoints::OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* p = (LV_DISPINFO*)pNMHDR;
	*pResult = true;
	if (p->item.pszText) m_aOwnPoints[p->item.iItem] = atoi(p->item.pszText);

	// Change to own points scheme

	if (m_nPointsType)
	{
		((CComboBox*)GetDlgItem(IDC_POINTS_TYPE))->SetCurSel(0);		
		m_nPointsType = 0;
	}
}

//*****************************************************************************
void CPgRSPoints::OnSelchangePointsType() 
{
	CWslArray<int> aPoints;

	m_nPointsType = ((CComboBox*)GetDlgItem(IDC_POINTS_TYPE))->GetCurSel();

	switch (m_nPointsType)
	{
	case 0:
		aPoints = m_aOwnPoints;
		break;

	case 1:
		aPoints += 15;
		aPoints += 12;
		aPoints += 10;
		aPoints += 8;
		aPoints += 6;
		aPoints += 5;
		aPoints += 4;
		aPoints += 3;
		aPoints += 2;
		aPoints += 1;
		break;

	case 2:
		aPoints += 25;
		aPoints += 20;
		aPoints += 16;
		aPoints += 13;
		aPoints += 11;
		aPoints += 10;
		aPoints += 9;
		aPoints += 8;
		aPoints += 7;
		aPoints += 6;
		aPoints += 5;
		aPoints += 4;
		aPoints += 3;
		aPoints += 2;
		aPoints += 1;
		break;

	case 3:
		aPoints += 16;
		aPoints += 15;
		aPoints += 14;
		aPoints += 13;
		aPoints += 12;
		aPoints += 11;
		aPoints += 10;
		aPoints += 9;
		aPoints += 8;
		aPoints += 7;
		aPoints += 6;
		aPoints += 5;
		aPoints += 4;
		aPoints += 3;
		aPoints += 2;
		aPoints += 1;
		break;
	}

	BuildList(aPoints);
}

//*****************************************************************************
void CPgRSPoints::OnEnablePoints()
{
	BOOL b = ((CButton*)GetDlgItem(IDC_ENABLE_POINTS))->GetCheck();

	m_List.EnableWindow(b);
	GetDlgItem(IDC_ADD_BEFORE)->EnableWindow(b);
	GetDlgItem(IDC_ADD_AFTER)->EnableWindow(b);
	GetDlgItem(IDC_REMOVE)->EnableWindow(b);
	GetDlgItem(IDC_EDIT)->EnableWindow(b);
	GetDlgItem(IDC_POINTS_TYPE)->EnableWindow(b);
}

//*****************************************************************************
void CPgRSPoints::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	OnEdit();
}
