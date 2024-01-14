///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 07 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class CPgDrvBasic : public CWsfPropertyPage
{
protected:
	CWsfTreeItem*	m_pTIDrv;			// TI of the driver, either DB or in race
	CDbDrv				m_TempDrv, *m_pDrv;
	char					m_szLicenceNum[DB_DRV_LICENCE_CHARS+1];
	BOOL					m_bInRace;
	BOOL					m_bTheSame;

public:
	CPgDrvBasic(CDbDrv *pDrv, BOOL bInRace, CWsfTreeItem* pTIDrv);

protected:
	afx_msg void OnChangeNameSurname();

	// From WSF

	virtual BOOL OnOkButton();
	virtual BOOL OnKillActive(int& nNewPage);

	// Dialog Data

	//{{AFX_DATA(CPgDrvBasic)
	enum { IDD = IDD_DRV_BASIC };
	//}}AFX_DATA

	// Overrides

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPgDrvBasic)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation

protected:
	// Generated message map functions
	//{{AFX_MSG(CPgDrvBasic)
	virtual BOOL OnInitDialog();
	afx_msg void OnMemberOfUnion();
	afx_msg void OnKillfocusNickname();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
