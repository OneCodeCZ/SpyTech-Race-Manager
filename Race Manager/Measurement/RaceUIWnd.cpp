/******************
*                 *  RaceUIWnd.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of World Spy Library.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"
#include "RaceUIWnd.h"
#include "ScreenCfgCodes.h"
#include "DlgTrainingSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*******************************************************************************
*
*  CONSTRUCTION / DESTRUCTION
*
*******************************************************************************/

CRaceUIWnd::CRaceUIWnd(){
	m_pRace=NULL;
   gdi.Init();
}

/******************************************************************************/

CRaceUIWnd::~CRaceUIWnd(){
}

/******************************************************************************/

BEGIN_MESSAGE_MAP(CRaceUIWnd, CWnd)
	//{{AFX_MSG_MAP(CRaceUIWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_COMMAND(WM_SUCCESS, OnSuccess)
	ON_COMMAND(WM_INTERRUPTED, OnInterruption)
	ON_COMMAND(WM_INCORRECT, OnIncorrect)
	ON_COMMAND(WM_SIMPLE_END, OnSimpleEnd)
	ON_COMMAND(WM_GROUP_SUCCESS, OnGroupSuccess)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*******************************************************************************
*
*  CREATION
*
*******************************************************************************/

BOOL CRaceUIWnd::Create(CWnd* pParentWnd, BOOL bMakeTopMost, RECT *rect) 
{

   if (GetProfileInt("Settings\\Screen", "resolution_changes", 1) == 0)
   {
      gdi.SaveScreenResolution();
      gdi.ChangeScreenResolution(800, 600);
   }

	const char *pClassName;
	RECT local_rect;
	if(rect)memcpy(&local_rect, rect, sizeof(RECT));
	else{
		local_rect.left=0;
		local_rect.top=0;
//		local_rect.right=::GetSystemMetrics(SM_CXSCREEN);
//		local_rect.bottom=::GetSystemMetrics(SM_CYSCREEN);
		local_rect.right=800;
		local_rect.bottom=600;
	}
	pClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW);
			//::LoadCursor(AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_NULLCURSOR)));
	BOOL b = CreateEx(bMakeTopMost?WS_EX_TOPMOST:0,
		pClassName,
		WsfGetApp()->m_pszAppName,
		WS_VISIBLE|WS_POPUP /*| WS_CAPTION*/, 
		local_rect.left,
		local_rect.top,
		local_rect.right,
		local_rect.bottom,
		*pParentWnd,
		NULL,
		NULL);

	return b;
}

/******************************************************************************/

int CRaceUIWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Disable the main application window
	GetParent()->EnableWindow(false);

	InitScreen();
	Watch_CreateThread(this);
	Watch_SetSource(NULL);

	return 0;
}

/******************************************************************************/

void CRaceUIWnd::OnDestroy() 
{
	if (!Watch_KillThread()) {
		AfxMessageBox("System error:\r\nThe screen thread failed to die.");
	}

   if (GetProfileInt("Settings\\Screen", "resolution_changes", 1))
   {
      gdi.RestoreScreenResolution();
   }

	// Enable the main application window and 
	// activate it
	GetParent()->EnableWindow(true);
	GetParent()->SetFocus();

	CWnd::OnDestroy();
}

/*******************************************************************************
*
*  PAINTING
*
*******************************************************************************/

void CRaceUIWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	m_rgnLast.DeleteObject();
	m_rgnLast.CreateRectRgn(0,0,0,0);
	CBrush brush(RGB(0,0,0));
	CRect rect;
	GetClientRect(rect);
	dc.FillRect(&rect, &brush);
	int nWidth = m_nWidth * rect.Width() + ::GetSystemMetrics(SM_CXSCREEN)/2;
	nWidth = nWidth/::GetSystemMetrics(SM_CXSCREEN);

	Watch_RedrawAll();
}

/*******************************************************************************
*
*  KEYBOARD
*
*******************************************************************************/

void CRaceUIWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Just only the first keystroke !

	BOOL bRepeated = nFlags & (1 << 14);
	if (!bRepeated) {
		g_dwThreadKey = nChar;
		SetEvent(g_hEventKeyPressed);
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

/*******************************************************************************
*
*  SCREENS
*
*******************************************************************************/

void CRaceUIWnd::InitScreen() {
	m_pSrcBest=new CSrcBest(&m_GlobalSource, QUAL_BEST_LARGE, 80);
	m_GlobalSource.RegisterWatch(m_pSrcBest);
		
	m_GlobalSource.SetEnvironment(C_WHITE, 1, W_ALIGN_CENTER, C_BLUE1, C_WHITE, FALSE);

	m_pSrcHeader=new CSrcHeader(&m_GlobalSource);
	m_GlobalSource.RegisterWatch(m_pSrcHeader);
		
	m_pSrcTime=new CSrcTime(&m_GlobalSource);
	m_GlobalSource.RegisterWatch(m_pSrcTime);
		
	m_pSrcBar=new CSrcBar(&m_GlobalSource);
	m_GlobalSource.RegisterWatch(m_pSrcBar);
	m_pSrcBar->SetTime(0, 10000, 2);
	m_pSrcBar->SetMode(BAR_PROGRESS);
		
	m_pSrcSponsorSemaphore=new CSrcSponsorSemaphore(&m_GlobalSource);
	m_GlobalSource.RegisterWatch(m_pSrcSponsorSemaphore);
		
	m_pSrcQualMain=new CSrcQualMain(&m_GlobalSource, m_pRace);
	m_GlobalSource.RegisterWatch(m_pSrcQualMain);
		
	m_pSrcNextRacers=new CSrcNextRacers(&m_GlobalSource);
	m_pSrcNextRacers->SetMode(NEXT_MODE_MORE);
	m_GlobalSource.RegisterWatch(m_pSrcNextRacers);

	m_pSrcRaceMain=new CSrcRaceMain(&m_GlobalSource, 10);
	m_GlobalSource.RegisterWatch(m_pSrcRaceMain);

	m_pSrcRaceGroupResults=new CSrcRaceGroupResults(&m_GlobalSource, 10, 10);
	m_GlobalSource.RegisterWatch(m_pSrcRaceGroupResults);
		
	m_pSrcTraining=new CSrcTraining(&m_GlobalSource);
	m_GlobalSource.RegisterWatch(m_pSrcTraining);
		
	m_pSrcRacePartTitle=new CSrcRacePartTitle();
	m_GlobalSource.RegisterWatch(m_pSrcRacePartTitle);

	HideAllSources();
}

/******************************************************************************/

void CRaceUIWnd::HideAllSources() {
	m_pSrcRacePartTitle->Visibility(SW_HIDE);

	m_pSrcHeader->Visibility(SW_HIDE);
	m_pSrcBar->Visibility(SW_HIDE);
	m_pSrcTime->Visibility(SW_HIDE);

	m_pSrcSponsorSemaphore->Visibility(SW_HIDE);
	m_pSrcBest->Visibility(SW_HIDE);

	m_pSrcNextRacers->Visibility(SW_HIDE);
	m_pSrcQualMain->Visibility(SW_HIDE);
	m_pSrcRaceMain->Visibility(SW_HIDE);
	m_pSrcTraining->Visibility(SW_HIDE);
	m_pSrcRaceGroupResults->Visibility(SW_HIDE);
}

/******************************************************************************/

void CRaceUIWnd::SetUpScreen(int nMode, int nSubmode) {

	// --------------------------------------------------------------------------
	// First hide all

	Watch_SetSource(NULL);

	HideAllSources();

	// --------------------------------------------------------------------------
	// Then, gather data

	CTIMeasuredEntity *pEntity;
	CTIRS *pTIRS;

	if (m_pRace) {
		pTIRS = m_pRace->GetRS();
	}

	// --------------------------------------------------------------------------
	// And then light up appropriate only

	switch(nMode) {

		// -----------------------------------------------------------------------

	case CFG_NONE:
		break;

		// -----------------------------------------------------------------------

	case CFG_TITLE:
		m_pSrcRacePartTitle->Visibility(SW_SHOW);
		m_pSrcRacePartTitle->SetText(m_pRace->m_strName, GetRacePartName(m_pRace->m_nRacePart), "", "");
		if (m_pRace->m_nRacePart >= RACE_PART_CONSI) {
			// groups are indexed from zero, but the function requires 1-based idx.
			m_pSrcRacePartTitle->SetGroupName(m_pRace->m_nActiveGroup+1, m_pRace->m_nNumGroups);
		}
		break;

		// -----------------------------------------------------------------------

	case CFG_QUAL_LIST_CURRENT:
		pEntity = m_pRace->QualGetCurrentEntity();
		m_pSrcTime->SetMode(TIME_MODE_LARGE, TIME_FORMAT_S, TIME_MODE_COUNTDOWN);
		m_pSrcTime->SetTime(0, 0);
		m_pSrcTime->SetCaption(TIME_CAPTION_TIME_TO_START);
		m_pSrcHeader->SetCaptions("Qualification", "Current results", " ");
		m_pSrcBar->SetMode(BAR_PROGRESS);
		m_pSrcBar->SetTime(0, 4000, 1);
		m_pSrcNextRacers->Reset();
		m_pSrcNextRacers->SetMode(NEXT_MODE_MORE);
		m_pSrcNextRacers->SetLines(NEXT_MODE_MORE, m_pRace);
		m_pSrcBest->SetMode(QUAL_BEST_LARGE, m_pRace->m_aPartEntities.GetSize() /* m_pRace->m_aPartEntities.GetSize() */);
		m_pSrcBest->QualSetLines(m_pRace);

		m_pSrcTime->Visibility(SW_SHOW);
		m_pSrcHeader->Visibility(SW_SHOW);
		m_pSrcBar->Visibility(SW_SHOW);
		m_pSrcNextRacers->Visibility(SW_SHOW);
		m_pSrcBest->Visibility(SW_SHOW);
		break;

	case CFG_QUAL_MAIN:
		pEntity = m_pRace->QualGetCurrentEntity();
		m_pSrcTime->SetMode(TIME_MODE_SMALL, TIME_FORMAT_HMS, TIME_MODE_COUNTDOWN);
		if (nSubmode==0) m_pSrcTime->SetCaption(TIME_CAPTION_TIME_TO_END);
		else m_pSrcTime->SetCaption(TIME_CAPTION_TIME_TO_START);
		m_pSrcBar->SetMode(BAR_PROGRESS);
		m_pSrcBar->SetTime(0, pTIRS->GetTimeSettings(m_pRace->m_nRacePart, 1), 4);
		m_pSrcQualMain->ResetDisplays(pEntity);
		m_pSrcBest->SetMode(QUAL_BEST_SMALL, -1);
		m_pSrcBest->QualSetLines(m_pRace);
		m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SEMAPHORE);
		m_pSrcNextRacers->SetMode(NEXT_MODE_SMALL);
		m_pSrcNextRacers->SetLines(NEXT_MODE_SMALL, m_pRace);
		m_pSrcHeader->SetCaptionQualification(m_pRace, pEntity);
		
		m_pSrcBest->Visibility(SW_SHOW);
		m_pSrcTime->Visibility(SW_SHOW);
		m_pSrcBar->Visibility(SW_SHOW);
		m_pSrcQualMain->Visibility(SW_SHOW);
		m_pSrcSponsorSemaphore->Visibility(SW_SHOW);
		m_pSrcNextRacers->Visibility(SW_SHOW);
		m_pSrcHeader->Visibility(SW_SHOW);
		break;

	case CFG_QUAL_FINAL:
		m_pSrcHeader->SetCaptions("Qualification", "Results", " ");
		m_pSrcBar->SetMode(BAR_PROGRESS);
		m_pSrcBest->SetMode(QUAL_BEST_RESULTS, m_pRace->m_aPartEntities.GetSize() /* m_pRace->m_pArrSortedEntities.GetSize() */);
		m_pSrcBest->QualSetLines(m_pRace);

		m_pSrcHeader->Visibility(SW_SHOW);
		m_pSrcBar->Visibility(SW_SHOW);
		m_pSrcBest->Visibility(SW_SHOW);
		break;

		// -----------------------------------------------------------------------

	case CFG_RACE_MAIN:
		m_pSrcHeader->SetCaptionRacePart(m_pRace, nSubmode == 0);
		m_pSrcBar->SetMode(BAR_PROGRESS);
		m_pSrcTime->SetMode(TIME_MODE_SMALL, TIME_FORMAT_HMS, TIME_MODE_COUNTDOWN);
		m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SEMAPHORE);
		m_pSrcRaceMain->SetLines(m_pRace, FALSE);

		m_pSrcHeader->Visibility(SW_SHOW);
		m_pSrcBar->Visibility(SW_SHOW);
		m_pSrcTime->Visibility(SW_SHOW);
		m_pSrcSponsorSemaphore->Visibility(SW_SHOW);
		m_pSrcRaceMain->Visibility(SW_SHOW);
		break;

	case CFG_RACE_GROUP_RESULTS:
		m_pSrcHeader->SetCaptionRacePart(m_pRace, 0);
		m_pSrcBar->SetMode(BAR_PROGRESS);
		m_pSrcTime->SetMode(TIME_MODE_SMALL, TIME_FORMAT_HMS, TIME_MODE_COUNTDOWN);
		m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SPONSOR);
		m_pSrcRaceGroupResults->SetLines(m_pRace);

		m_pSrcHeader->Visibility(SW_SHOW);
		m_pSrcBar->Visibility(SW_SHOW);
		m_pSrcTime->Visibility(SW_SHOW);
		m_pSrcSponsorSemaphore->Visibility(SW_SHOW);
		m_pSrcRaceGroupResults->Visibility(SW_SHOW);
		break;

	case CFG_RACE_FINAL:
		break;

		// -----------------------------------------------------------------------

	case CFG_TRAINING:
		m_pSrcHeader->SetCaptions("Practice", "Mode: Manual", " ");
		m_pSrcBar->SetMode(BAR_PROGRESS);
		m_pSrcTime->SetMode(TIME_MODE_SMALL, TIME_FORMAT_HMS, TIME_MODE_COUNTUP);
		m_pSrcTime->SetCaption(TIME_CAPTION_TRAINING_TIME_ELAPSED);
		m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SPONSOR);
		m_pSrcTraining->SetLines();

		m_pSrcHeader->Visibility(SW_SHOW);
		m_pSrcBar->Visibility(SW_SHOW);
		m_pSrcTime->Visibility(SW_SHOW);
		m_pSrcSponsorSemaphore->Visibility(SW_SHOW);
		m_pSrcTraining->Visibility(SW_SHOW);
		break;

		// -----------------------------------------------------------------------

	case CFG_CALIBRATION:
		break;
	}

	Watch_SetSource(&m_GlobalSource);
	Watch_RedrawAll();
}

/******************************************************************************/

void CRaceUIWnd::OnSuccess() {
	m_pRace->OnSuccess();
}

/******************************************************************************/

void CRaceUIWnd::OnInterruption() {
	m_pRace->OnInterrupt();
}

/******************************************************************************/

void CRaceUIWnd::OnIncorrect() {
	m_pRace->OnIncorrect();
}

/******************************************************************************/

void CRaceUIWnd::OnSimpleEnd() {
	ShowWindow(SW_HIDE);
	DestroyWindow();
	delete this;
}

/******************************************************************************/

void CRaceUIWnd::OnGroupSuccess()
{
	m_pRace->OnGroupSuccess();
}

/*********************************************************************** eof **/

void CRaceUIWnd::OnClose() 
{
   // Do nothing
}
