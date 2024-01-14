// PgEntityDebug.cpp : implementation file
//

#include "stdafx.h"
#include "main.h"
#include "TIMeasuredEntity.h"
#include "PgEntityDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPgEntityDebug property page

CPgEntityDebug::CPgEntityDebug() : CWsfPropertyPage(CPgEntityDebug::IDD)
{
}

CPgEntityDebug::CPgEntityDebug(CTIMeasuredEntity *pEntity) : CWsfPropertyPage(CPgEntityDebug::IDD)
{
	m_pEntity = pEntity;
}

CPgEntityDebug::~CPgEntityDebug()
{
}

void CPgEntityDebug::DoDataExchange(CDataExchange* pDX)
{
	CWsfPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPgEntityDebug)
	DDX_Control(pDX, IDC_LIST, m_wndList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPgEntityDebug, CWsfPropertyPage)
	//{{AFX_MSG_MAP(CPgEntityDebug)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPgEntityDebug message handlers

BOOL CPgEntityDebug::OnInitDialog() 
{
	CWsfPropertyPage::OnInitDialog();

	m_wndList.AddString(CString("Entity name: ") + m_pEntity->GetEntityName(CTIRaceEntity::NICKNAME));

	CString s;
	int a;

	s = "Valid data: ";
	for (a=0; a<8; a++) {
		if (m_pEntity->m_nDataAreValid[a]) s+="v ";
		else s+="- ";
	}
	m_wndList.AddString(s);

	s.Format("Num event objects: %d", m_pEntity->m_pArrEvents.GetSize());
	m_wndList.AddString(s);

	s.Format("Current event: %d", m_pEntity->m_nCurrentEvent);
	m_wndList.AddString(s);

	for (a=0; a<=m_pEntity->m_nCurrentEvent; a++) {
		m_wndList.AddString(" ");
		s.Format("Event: %d", a);
		m_wndList.AddString(s);
		CEventData *pEvent = m_pEntity->m_pArrEvents[a];
		ASSERT(pEvent);
		int nIdx;
		ITERATE(pEvent->m_pArrRides, nIdx) {
			CRideData *pRide = pEvent->m_pArrRides[nIdx];
			s.Format("Ride %d. best=%d last=%d laps=%d hdts=%d track=%d", 
				nIdx,
				pRide->m_dwBestTime,
				pRide->m_dwLastTime,
				pRide->m_dwNumLaps,
				pRide->m_dwNumHdts,
				pRide->m_nTrackAssigned);
			m_wndList.AddString(s);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
