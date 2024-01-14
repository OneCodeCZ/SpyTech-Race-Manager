///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class CPgRSTimes : public CWsfPropertyPage
{
protected:
	CTIRS				&m_TIRS;
	CTimeSpan		m_Times[5][3];		
	
public:
	CPgRSTimes(CTIRS *pTIRS);

	// from WSF

	virtual BOOL OnOkButton();
	virtual BOOL OnKillActive(int& nNewPage);

// Dialog Data
	//{{AFX_DATA(CPgRSTimes)
	enum { IDD = IDD_RS_TIMES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgRSTimes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CPgRSTimes)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
