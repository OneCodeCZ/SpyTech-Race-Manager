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
class CPgRSQual : public CWsfPropertyPage
{
protected:
	CTIRS				&m_RS;
	CPgRSBasic	*m_pPgBasic;

	int			m_nPrimaryEval;
	int			m_nNumRides;
	int			m_nNumAttempts;

public:
	CPgRSQual(CTIRS *pRS, CPgRSBasic *pPgBasic);

	// from WSF

	virtual BOOL OnOkButton();
	virtual BOOL OnKillActive(int& nNewPage);
	virtual BOOL OnSetActive();

// Dialog Data
	//{{AFX_DATA(CPgRSQual)
	enum { IDD = IDD_RS_QUAL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgRSQual)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPgRSQual)
	virtual BOOL OnInitDialog();
	afx_msg void OnType();
	afx_msg void OnType2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
