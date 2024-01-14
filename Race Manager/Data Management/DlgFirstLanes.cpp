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
#include "DlgFirstLanes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CDlgFirstLanes, CDialog)
	//{{AFX_MSG_MAP(CDlgFirstLanes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CDlgFirstLanes::CDlgFirstLanes(CTIRace *pRace)
	: CDialog(CDlgFirstLanes::IDD, NULL), m_Race(*pRace)
{
	//{{AFX_DATA_INIT(CDlgFirstLanes)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//*****************************************************************************
void CDlgFirstLanes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFirstLanes)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_CBIndex(pDX, IDC_ENTITY1, m_nLane[0]);
	DDX_CBIndex(pDX, IDC_ENTITY2, m_nLane[1]);
	DDX_CBIndex(pDX, IDC_ENTITY3, m_nLane[2]);
	DDX_CBIndex(pDX, IDC_ENTITY4, m_nLane[3]);
	DDX_CBIndex(pDX, IDC_ENTITY5, m_nLane[4]);
	DDX_CBIndex(pDX, IDC_ENTITY6, m_nLane[5]);
	DDX_CBIndex(pDX, IDC_ENTITY7, m_nLane[6]);
	DDX_CBIndex(pDX, IDC_ENTITY8, m_nLane[7]);
	DDX_CBIndex(pDX, IDC_ENTITY9, m_nLane[8]);
	DDX_CBIndex(pDX, IDC_ENTITY10, m_nLane[9]);
}

//*****************************************************************************
BOOL CDlgFirstLanes::OnInitDialog() 
{
	int nNumPausing, nNumEntities, t, i, nNumLanes;
	CString s;

	CEntityArray &aGroup = m_Race.m_aGroup[m_Race.m_nActiveGroup];
	nNumEntities = aGroup.GetSize();
	nNumLanes = m_Race.GetNumLanes();

	// Init combo boxes

	CComboBox *p = (CComboBox*) GetDlgItem(IDC_ENTITY1);
	nNumPausing = nNumEntities - m_Race.GetNumLanes();

	for (i=0; i<10; i++)
	{
		if (i < nNumEntities)
		{
			for (t=0; t < nNumLanes; t++) p->AddString(itoa(t+1, temp,10));
			if (nNumPausing > 0) p->AddString("Pauza");
			m_nLane[i] = i < nNumLanes ? i : nNumLanes;
		}
		else p->EnableWindow(false);

		p = (CComboBox*) GetNextDlgGroupItem(p);
	}

	// Init names

	CWnd *p2 = GetDlgItem(IDC_NAME1);

	for (i=0; i<10; i++)
	{
		if (i < nNumEntities)
		{
			s = aGroup[i]->GetEntityName(CTIRaceEntity::SURNAME_NAME);
			s += ':';
			p2->SetWindowText(s);
		}
		else p2->EnableWindow(false);

		p2 = GetNextDlgGroupItem(p2);
	}

	// --------

	GetDlgItem(IDCANCEL)->EnableWindow(false);
	CDialog::OnInitDialog();
	return TRUE;
}

//*****************************************************************************
void CDlgFirstLanes::OnOK() 
{
	int nNumEntities, i, t, nNumPausing ,nPausingCount, n;
	int nNumLanes;

	if (!UpdateData()) return;

	// Check lanes assignment

	nNumEntities = m_Race.m_aGroup[m_Race.m_nActiveGroup].GetSize();
	nNumPausing = nNumEntities - m_Race.GetNumLanes();
	nNumLanes = m_Race.GetNumLanes();
	nPausingCount = 0;

	for (i=0; i < nNumEntities; i++) // Not nNumEntities-1 because of pausing counting
	{
		n = m_nLane[i];
		
		if (n < nNumLanes)
		{
			for (t=i+1; t<nNumEntities; t++)
			{
				if (m_nLane[t] == n)
				{
					AfxMessageBox(IDS_TWO_HAS_SAME_LANE);
					return;
				}
			}
		}
		else if (++nPausingCount > nNumPausing)
		{
			CString s;
			s.Format(nNumPausing == 1 ? IDS_PAUSE_CAN_ONE_DRIVER : IDS_PAUSE_CAN_X_DRIVERS, nNumPausing);
			AfxMessageBox(s);
			return;
		}
	}

	// -----

	EndDialog(IDOK);
}

//*****************************************************************************
void CDlgFirstLanes::GetLanes(CWslArray<int> &a)
{
	int nNumEntities = m_Race.m_aGroup[m_Race.m_nActiveGroup].GetSize();
	a.SetSize(nNumEntities);

	for (int i=nNumEntities; i--;)
	{
		a[i] = m_nLane[i] == m_Race.GetNumLanes() ? LANE_PAUSE : m_nLane[i];
	}
}











