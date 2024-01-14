// DlgEbTest.cpp : implementation file
//

#include "stdafx.h"
#include "main.h"
#include "DlgEbTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgEbTest dialog


CDlgEbTest::CDlgEbTest(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEbTest::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgEbTest)
	m_sLane = _T("");
	m_sLaps = _T("");
	m_sLights = _T("");
	m_sState = _T("");
	//}}AFX_DATA_INIT
}


void CDlgEbTest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgEbTest)
	DDX_Text(pDX, IDC_LANE, m_sLane);
	DDX_Text(pDX, IDC_LAPS, m_sLaps);
	DDX_Text(pDX, IDC_LIGHTS, m_sLights);
	DDX_Text(pDX, IDC_STATE, m_sState);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgEbTest, CDialog)
	//{{AFX_MSG_MAP(CDlgEbTest)
	ON_BN_CLICKED(IDC_PING, OnPing)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_UPDATE_LIGHTS, OnUpdateLights)
	ON_BN_CLICKED(IDC_UPDATE_LAPS, OnUpdateLaps)
	ON_BN_CLICKED(IDC_UPDATE_STATE, OnUpdateState)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgEbTest message handlers

void CDlgEbTest::OnPing() 
{
   CallEBPing();
}

void CDlgEbTest::OnReset() 
{
   CallEBReset();
}

void CDlgEbTest::OnUpdateLights() 
{
   UpdateData(TRUE);
   int l = atoi(m_sLights);
   CallEBUpdateLights(l);
}

void CDlgEbTest::OnUpdateLaps() 
{
   UpdateData(TRUE);
   int lane = atoi(m_sLane);
   int laps = atoi(m_sLaps);
   CallEBUpdateLaps(lane, laps);
}

void CDlgEbTest::OnUpdateState() 
{
   UpdateData(TRUE);
   int new_state = atoi(m_sState);
   CallEBOnRaceEvent(new_state);
}
