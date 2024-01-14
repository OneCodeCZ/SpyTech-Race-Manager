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
class CDlgFirstLanes : public CDialog
{
protected:
	CTIRace	&m_Race;
	int		m_nLane[10];

public:
	CDlgFirstLanes(CTIRace *pRace);

	// Fills the array with first lanes. They are zero based numbers.
	// LANE_PAUSE for pausing.

	void GetLanes(CWslArray<int> &a);

// Dialog Data
	//{{AFX_DATA(CDlgFirstLanes)
	enum { IDD = IDD_FIRST_LANES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFirstLanes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgFirstLanes)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
