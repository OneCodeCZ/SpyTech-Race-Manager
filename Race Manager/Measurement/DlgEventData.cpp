// DlgEventData.cpp : implementation file
//

#include "stdafx.h"
#include "main.h"
#include "DlgEventData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEventData dialog


CDlgEventData::CDlgEventData(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEventData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEventData)
	m_sRide1 = _T("");
	m_sRide2 = _T("");
	m_sRide3 = _T("");
	m_sRide4 = _T("");
	m_sRide5 = _T("");
	m_sRide6 = _T("");
	m_sRide7 = _T("");
	m_sRide8 = _T("");
	m_sRide9 = _T("");
	m_sRide10= _T("");
	m_sHdts = _T("");
	m_sLapsElapsed = _T("");
	m_sPenalization = _T("");
	m_sTotalLapsElapsed = _T("");
	//}}AFX_DATA_INIT
}


void CDlgEventData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEventData)
	DDX_Control(pDX, IDC_CAPTION2, m_wndCaption2);
	DDX_Control(pDX, IDC_CAPTION1, m_wndCaption1);
	DDX_Text(pDX, IDC_RIDE_1, m_sRide1);
	DDX_Text(pDX, IDC_RIDE_2, m_sRide2);
	DDX_Text(pDX, IDC_RIDE_3, m_sRide3);
	DDX_Text(pDX, IDC_RIDE_4, m_sRide4);
	DDX_Text(pDX, IDC_RIDE_5, m_sRide5);
	DDX_Text(pDX, IDC_RIDE_6, m_sRide6);
	DDX_Text(pDX, IDC_RIDE_7, m_sRide7);
	DDX_Text(pDX, IDC_RIDE_8, m_sRide8);
	DDX_Text(pDX, IDC_RIDE_9, m_sRide9);
	DDX_Text(pDX, IDC_RIDE_10, m_sRide10);
	DDX_Text(pDX, IDC_HDTS, m_sHdts);
	DDX_Text(pDX, IDC_LAPS_ELAPSED, m_sLapsElapsed);
	DDX_Text(pDX, IDC_PENALIZATION, m_sPenalization);
	DDX_Text(pDX, IDC_TOTAL_LAPS_ELAPSED, m_sTotalLapsElapsed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEventData, CDialog)
	//{{AFX_MSG_MAP(CDlgEventData)
	ON_EN_UPDATE(IDC_RIDE_1, OnUpdateRide1)
	ON_EN_UPDATE(IDC_RIDE_10, OnUpdateRide10)
	ON_EN_UPDATE(IDC_RIDE_2, OnUpdateRide2)
	ON_EN_UPDATE(IDC_RIDE_3, OnUpdateRide3)
	ON_EN_UPDATE(IDC_RIDE_4, OnUpdateRide4)
	ON_EN_UPDATE(IDC_RIDE_5, OnUpdateRide5)
	ON_EN_UPDATE(IDC_RIDE_6, OnUpdateRide6)
	ON_EN_UPDATE(IDC_RIDE_7, OnUpdateRide7)
	ON_EN_UPDATE(IDC_RIDE_8, OnUpdateRide8)
	ON_EN_UPDATE(IDC_RIDE_9, OnUpdateRide9)
	ON_EN_UPDATE(IDC_PENALIZATION, OnUpdatePenalization)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEventData message handlers

void CDlgEventData::UpdateDependentVariables()
{
	UpdateData(TRUE);

	CEventData *pEvent = m_pEntity->m_pArrEvents[GetEventIdx(nRacePart)];
	int nNumLaps = 0;
	int nNumRides = pEvent->m_pArrRides.GetSize();

	int nIdx;

	for (nIdx = 0; nIdx < nNumRides; nIdx++) {
		if (pEvent->m_pArrRides[nIdx]->m_nTrackAssigned != -1) {
			nNumLaps += atoi(*m_pArrStrings[nIdx]);
		}
	}

	int nPenalization = atoi(m_sPenalization);

	m_sLapsElapsed.Format("%d", nNumLaps);
	m_sTotalLapsElapsed.Format("%d", nNumLaps - nPenalization);
	UpdateData(FALSE);
}

/******************************************************************************/

BOOL CDlgEventData::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_pArrStrings.Append(&m_sRide1);
	m_pArrStrings.Append(&m_sRide2);
	m_pArrStrings.Append(&m_sRide3);
	m_pArrStrings.Append(&m_sRide4);
	m_pArrStrings.Append(&m_sRide5);
	m_pArrStrings.Append(&m_sRide6);
	m_pArrStrings.Append(&m_sRide7);
	m_pArrStrings.Append(&m_sRide8);
	m_pArrStrings.Append(&m_sRide9);
	m_pArrStrings.Append(&m_sRide10);

	m_pArrEdits.Append(GetDlgItem(IDC_RIDE_1));
	m_pArrEdits.Append(GetDlgItem(IDC_RIDE_2));
	m_pArrEdits.Append(GetDlgItem(IDC_RIDE_3));
	m_pArrEdits.Append(GetDlgItem(IDC_RIDE_4));
	m_pArrEdits.Append(GetDlgItem(IDC_RIDE_5));
	m_pArrEdits.Append(GetDlgItem(IDC_RIDE_6));
	m_pArrEdits.Append(GetDlgItem(IDC_RIDE_7));
	m_pArrEdits.Append(GetDlgItem(IDC_RIDE_8));
	m_pArrEdits.Append(GetDlgItem(IDC_RIDE_9));
	m_pArrEdits.Append(GetDlgItem(IDC_RIDE_10));

	WsfSetBoldFont(this, IDC_CAPTION1);
	WsfSetBoldFont(this, IDC_CAPTION2);

	CEventData *pEvent = m_pEntity->m_pArrEvents[GetEventIdx(nRacePart)];

	int nNumRides = pEvent->m_pArrRides.GetSize();
	int nIdx;

	for (nIdx = 0; nIdx < nNumRides; nIdx++) {
		if (pEvent->m_pArrRides[nIdx]->m_nTrackAssigned != -1) m_pArrStrings[nIdx]->Format("%d", pEvent->m_pArrRides[nIdx]->m_dwNumLaps);
		else {
			m_pArrStrings[nIdx]->Format("–");
			m_pArrEdits[nIdx]->EnableWindow(FALSE);
		}
	}

	for (nIdx = nNumRides; nIdx < 10; nIdx++) {
		m_pArrEdits[nIdx]->EnableWindow(FALSE);
	}
	
	m_sHdts.Format("%d", pEvent->GetTotalHdts());
	m_sPenalization.Format("%d", pEvent->m_nPenalization);
	UpdateData(FALSE);

	UpdateDependentVariables();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************/

void CDlgEventData::OnOK() 
{
	UpdateData(TRUE);
	CEventData *pEvent = m_pEntity->m_pArrEvents[GetEventIdx(nRacePart)];

	int nNumHdts = atoi(m_sHdts);
	int nIdx;

	for (nIdx = 0; nIdx < 10; nIdx++) {
		if (m_pArrEdits[nIdx]->IsWindowEnabled()) {
			pEvent->m_pArrRides[nIdx]->m_dwNumLaps = atoi(*m_pArrStrings[nIdx]);
		}
	}

	ITERATE(pEvent->m_pArrRides, nIdx) {
		pEvent->m_pArrRides[nIdx]->m_dwNumHdts = 0;
		if (IS_LAST(pEvent->m_pArrRides, nIdx)) {
			pEvent->m_pArrRides[nIdx]->m_dwNumHdts = nNumHdts;
		}
	}

	pEvent->m_nPenalization = atoi(m_sPenalization);

	CDialog::OnOK();
}

/******************************************************************************/

void CDlgEventData::OnUpdateRide1() { UpdateDependentVariables(); }
void CDlgEventData::OnUpdateRide2() { UpdateDependentVariables(); }
void CDlgEventData::OnUpdateRide3() { UpdateDependentVariables(); }
void CDlgEventData::OnUpdateRide4() { UpdateDependentVariables(); }
void CDlgEventData::OnUpdateRide5() { UpdateDependentVariables(); }
void CDlgEventData::OnUpdateRide6() { UpdateDependentVariables(); }
void CDlgEventData::OnUpdateRide7() { UpdateDependentVariables(); }
void CDlgEventData::OnUpdateRide8() { UpdateDependentVariables(); }
void CDlgEventData::OnUpdateRide9() { UpdateDependentVariables(); }
void CDlgEventData::OnUpdateRide10() { UpdateDependentVariables(); }
void CDlgEventData::OnUpdatePenalization() { UpdateDependentVariables(); }
