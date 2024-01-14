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
class CPgTeamBasic : public CWsfPropertyPage
{
public:
	CTIRaceTeam		&m_Team;
	CString				m_strName;

public:
	CPgTeamBasic(CTIRaceTeam *pTeam);
	~CPgTeamBasic();

	// from WSF

	virtual BOOL OnOkButton();
	virtual BOOL OnKillActive(int& nNewPage);

// Dialog Data
	//{{AFX_DATA(CPgTeamBasic)
	enum { IDD = IDD_TEAM_BASIC };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPgTeamBasic)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPgTeamBasic)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};






\