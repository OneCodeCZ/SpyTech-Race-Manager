///////////////////////////////////////////////////////////////////////////////
//
// Race Manager
// ------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class CPgRaceProperties : public CWsfPropertyPage
{
protected:
	CTIRace		&m_Race;
	
	CString		m_strName;
	CString		m_strOrganizer;
	CString   m_strJudge;
	CString   m_strNumber;
	CString   m_strPlace;

	BOOL			m_bTeams;
	CWslDate	m_Date;
	int				m_nLapMinTime;
	int				m_nMinMembers;
	int				m_nMaxMembers;

public:
	CPgRaceProperties(CTIRace *pRace);

	// from WSF

	virtual BOOL OnOkButton();

// Dialog Data
	//{{AFX_DATA(CPgRaceProperties)
	enum { IDD = IDD_RACE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPgRaceProperties)
	public:
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPgRaceProperties)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeRaceType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
