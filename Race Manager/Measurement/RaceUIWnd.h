/******************
*                 *  RaceUIWnd.h
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#if !defined(AFX_WslBlankWnd_H__18054DC1_D797_11D2_B567_444553540000__INCLUDED_)
#define AFX_WslBlankWnd_H__18054DC1_D797_11D2_B567_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/*******************************************************************************
*
*  CRaceUIWnd
*
*******************************************************************************/

class CTIMeasuredRace;

class CRaceUIWnd : public CWnd
{
public:
	CRgn m_rgnLast;
	int m_nWidth;
	LOGBRUSH m_logbrush;
	LOGBRUSH m_logbrushBlack;

	CWslWatchSource *m_pSource;
	CWslWatchText *m_pText;

	CTIMeasuredRace *m_pRace;

	HANDLE m_hThread;

	CSrcRacePartTitle    *m_pSrcRacePartTitle;     // title screen

	CSrcHeader           *m_pSrcHeader;            // header
	CSrcBar              *m_pSrcBar;               // debug bar
	CSrcTime             *m_pSrcTime;              // time showing watches

	CSrcSponsorSemaphore *m_pSrcSponsorSemaphore;  // semaphores and sponsor bitmap

	CSrcNextRacers       *m_pSrcNextRacers;	     // list of next racers
	CSrcBest             *m_pSrcBest;              // best racers
	CSrcQualMain         *m_pSrcQualMain;          // main qualification displays
	CSrcRaceMain         *m_pSrcRaceMain;          // main race displays
	CSrcTraining         *m_pSrcTraining;          // training mode
	CSrcRaceGroupResults *m_pSrcRaceGroupResults;  // result of groups

	CSrcBarComments      *m_pSrcBarComments;       // comments

	CWslWatchSource m_GlobalSource;

	CDlgTrainingSettings  m_TrS;

public:
	CRaceUIWnd();

	// Create the window. Parameters are:
	//   pParentWnd .. pointer to parent window;
	//   bMakeTopMost (default=FALSE) .. specify, whether the window is topmost
	//   *rect (default=NULL) .. pointer to RECT structure containing required
	//                           extent of the window
	virtual BOOL Create(CWnd* pParentWnd, BOOL bMakeTopMost=FALSE, RECT *rect=NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRaceUIWnd)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRaceUIWnd();

	// Creates all sources of watches
	void InitScreen();

	// Forces all registered sources to hide
	void HideAllSources();

	// Set up the sreen to contain the correct combination of sources
	void SetUpScreen(int nMode, int nSubmode=0);

	// Generated message map functions
protected:
	//{{AFX_MSG(CRaceUIWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSuccess();
	afx_msg void OnInterruption();
	afx_msg void OnIncorrect();
	afx_msg void OnSimpleEnd();
	afx_msg void OnGroupSuccess();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/******************************************************************************/

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WslBlankWnd_H__18054DC1_D797_11D2_B567_444553540000__INCLUDED_)
