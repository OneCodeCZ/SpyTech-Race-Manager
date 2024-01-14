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
class CPgSetupSTM : public CWsfPropertyPage
{
protected:
	CApp						&m_App;
	int							m_nComPort;
	int							m_nVirtualTrackToSet;
	int							m_TrackMap[8];			// values: 1..8
	bool						m_bUpdateForRemap;

public:
	CPgSetupSTM(CApp *pApp);

	void UpdateControls();
	void HandleKillFocus();

	// from WSF

	virtual BOOL OnOkButton();
	virtual BOOL OnKillActive(int& nNewPage);

// Dialog Data
	//{{AFX_DATA(CPgSetupSTM)
	enum { IDD = IDD_SETUP_STM };
	BOOL	m_bTrackMappingEnabled;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPgSetupSTM)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPgSetupSTM)
	virtual BOOL OnInitDialog();
	afx_msg void OnRemap();
	afx_msg void OnSetfocusPhysical1();
	afx_msg void OnSetfocusPhysical2();
	afx_msg void OnSetfocusPhysical3();
	afx_msg void OnSetfocusPhysical4();
	afx_msg void OnSetfocusPhysical5();
	afx_msg void OnSetfocusPhysical6();
	afx_msg void OnSetfocusPhysical7();
	afx_msg void OnSetfocusPhysical8();
	afx_msg void OnTrackMappingEnabled();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
