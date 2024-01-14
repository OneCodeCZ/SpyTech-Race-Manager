///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "PgRSBasic.h"

//*****************************************************************************
class CPgRSAdvances : public CWsfPropertyPage
{
protected:
	CTIRS				&m_RS;				
	CPgRSBasic	*m_pPgBasic;

	int		m_nAdvancesQual[3];
	int		m_nAdvances[3][2];

public:
	CPgRSAdvances(CTIRS *pRS, CPgRSBasic *pPgBasic);

	// from WSF

	virtual BOOL OnOkButton();
	virtual BOOL OnKillActive(int& nNewPage);
	virtual BOOL OnSetActive();

// Dialog Data
	//{{AFX_DATA(CPgRSAdvances)
	enum { IDD = IDD_RS_ADVANCES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgRSAdvances)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPgRSAdvances)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
