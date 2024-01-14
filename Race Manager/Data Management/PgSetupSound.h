///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2004 06
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

/*****************************************************************************/
class CPgSetupSound : public CWsfPropertyPage
{
public:
	CPgSetupSound();
	~CPgSetupSound();

// Dialog Data
	//{{AFX_DATA(CPgSetupSound)
	enum { IDD = IDD_SETUP_SOUND };
	CComboBox	m_SelectSoundCard;
	//}}AFX_DATA

	// from WSF

	virtual BOOL OnOkButton();
	virtual BOOL OnKillActive(int& nNewPage);


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPgSetupSound)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPgSetupSound)
	virtual BOOL OnInitDialog();
	afx_msg void OnTestSound();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
