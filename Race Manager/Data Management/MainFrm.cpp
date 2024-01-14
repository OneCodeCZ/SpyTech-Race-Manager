///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "DlgAbout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CMainFrame, CWsfFrame)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_NEW_GLOBAL_RS, OnNewGlobalRs)
	ON_COMMAND(ID_NEW_GLOBAL_TEAM, OnNewGlobalTeam)
	ON_UPDATE_COMMAND_UI(ID_NEW_GLOBAL_TEAM, OnUpdateNewGlobalTeam)
	ON_COMMAND(ID_EXERCISE, OnExercise)
	ON_UPDATE_COMMAND_UI(ID_EXERCISE, OnUpdateExercise)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT nIndicators[] =
{
	ID_SEPARATOR
};

//*****************************************************************************
#ifdef _DEBUG
void CMainFrame::Dump(CDumpContext& dc) const
{
	CWsfFrame::Dump(dc);
}

#endif

//*****************************************************************************
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWsfFrame::OnCreate(lpCreateStruct) == -1) return -1;

	EnableToolTips();
//	((CStatusBar*)GetMessageBar())->SetIndicators(nIndicators, sizeof(nIndicators)/sizeof(UINT));

	m_StatusBarWnd.SetIndicators(nIndicators, sizeof(nIndicators)/sizeof(UINT));
	m_StatusBarWnd.GetStatusBarCtrl().SetMinHeight(20);
	
	m_StatusBarWnd.SetPaneInfo(0, ID_SEPARATOR, SBPS_STRETCH, 0);
//	m_StatusBarWnd.SetPaneText(1, "STM/2008", TRUE);

	m_nStatusPhase = 0;
	m_nLastStatus = -2;
//	SetTimer(0, 1000, 0);

	// Set title with version.
	// By default the title is read from resources.

	m_strTitle = WsfGetCApp()->m_pszAppName;
	OnUpdateFrameTitle(true);

	return 0;
}

//*****************************************************************************
void CMainFrame::OnAppAbout() 
{
	CDlgAbout d;
	d.DoModal();
}

//*****************************************************************************
void CMainFrame::OnNewGlobalRs() 
{
	g_pApp->m_pTIDbRSFolder->OnCmdMsg(ID_NEW_RS, CN_COMMAND, NULL, NULL);
}

//*****************************************************************************
void CMainFrame::OnNewGlobalTeam() 
{
	ASSERT(WsfGetApp()->IsWorkspaceOpened());
	WsfGetCWorkspace()->m_pTIRace->OnCmdMsg(ID_NEW_TEAM, CN_COMMAND, NULL, NULL);
}

//*****************************************************************************
void CMainFrame::OnUpdateNewGlobalTeam(CCmdUI* pCmdUI) 
{
	if (WsfGetApp()->IsWorkspaceOpened())
	{
		pCmdUI->Enable(WsfGetCWorkspace()->m_pTIRace->m_bTeams);
	}
	else pCmdUI->Enable(false);
}

//*****************************************************************************
void CMainFrame::OnExercise() 
{
	MAPI_RunTraining();
}

//*****************************************************************************
void CMainFrame::OnUpdateExercise(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

//*****************************************************************************
DWORD WINAPI GetStatusProc(LPVOID lpParam)
{
	CMainFrame *pFrame = (CMainFrame*) lpParam;
	pFrame->m_nLastStatus = MAPI_Stm2008Probe();
	return 1;
}

//*****************************************************************************
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	CString s;
	CMainFrame *pFrame = this;

	switch((m_nStatusPhase % 2)) 
	{
	case 1:
//		CreateThread(NULL, 0, &GetStatusProc, this, 0, &id);
		GetStatusProc(this);
		break;
	case 0:
		switch(m_nLastStatus) 
		{
		case -1: s.LoadString(IDS_STM_UNKNOWN_STATE); break;
		case 0 : s.LoadString(IDS_STM_NOT_CONNECTED); break;
		case 1 : s.LoadString(IDS_STM_TRACK_IS_OFF); break;
		case 2 : s.LoadString(IDS_STM_TRACK_IS_ON); break;
		}
		pFrame->m_StatusBarWnd.SetPaneText(1, s, TRUE);
		break;
	}
	m_nStatusPhase++;
	CWsfFrame::OnTimer(nIDEvent);
}

//*****************************************************************************
BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	return CWsfFrame::PreTranslateMessage(pMsg);
}
