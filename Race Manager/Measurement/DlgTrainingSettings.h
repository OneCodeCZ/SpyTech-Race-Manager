/******************
*                 *  DlgTrainingSettings.h
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  08/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of RM source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#pragma once

/*******************************************************************************
*
*  CDlgTrainingSettings
*
*******************************************************************************/

class CDlgTrainingSettings : public CDialog
{
// Construction
public:
	CDlgTrainingSettings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTrainingSettings)
	enum { IDD = IDD_TRAINING_SETTINGS };
	int		m_nMode;
	CString	m_sTitle;
	int		m_nNumRides;
	CString	m_sBeforeRide;
	CString	m_sPauseDuration;
	CString	m_sRideDuration;
	int		m_nClearAfter;
	//}}AFX_DATA

	CTimeSpan	m_PauseTime;
	CTimeSpan	m_RideTime;
	CTimeSpan   m_AnteRide;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTrainingSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Backdoor implementation (And also method OnLButtonDblClk)
	int m_nBkDoor;

	// Generated message map functions
	//{{AFX_MSG(CDlgTrainingSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnModeManual();
	afx_msg void OnModeSemiauto();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/*********************************************************************** eof **/
