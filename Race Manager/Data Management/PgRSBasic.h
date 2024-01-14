///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2004 06
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class CPgRSBasic : public CWsfPropertyPage
{
public:
	BOOL	   m_bInRace;
	CTIRS&   m_RS;

	CString	m_strName;
	BOOL		m_bQual;
	int		m_nFirstPart;
	int		m_nDistributionType;
	CString	m_strChanging;

	int		m_nPrimaryEval;
	int		m_nNumRides;
	int		m_nNumAttempts;

public:
	CPgRSBasic(CTIRS *pRS, BOOL bInRace);

	// from WSF

	virtual BOOL OnOkButton();
	virtual BOOL OnKillActive(int& nNewPage);

protected:
	void UpdateUI();

// Dialog Data
	//{{AFX_DATA(CPgRSBasic)
	enum { IDD = IDD_RS_BASIC };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgRSBasic)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPgRSBasic)
	virtual BOOL OnInitDialog();
	afx_msg void OnPartConsi();
	afx_msg void OnPartQuater();
	afx_msg void OnPartSemi();
	afx_msg void OnType();
	afx_msg void OnType2();
	afx_msg void OnPartQual();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
