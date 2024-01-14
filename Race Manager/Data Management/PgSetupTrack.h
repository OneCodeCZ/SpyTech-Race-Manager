///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ComboColorPicker.h"

//*****************************************************************************
class CPgSetupTrack : public CWsfPropertyPage
{
protected:
	CSetupTrack &m_RM;
	CComboColorPicker m_Lane[8];
	BOOL		m_bReadOnly;

	CString	m_strOwner;
	CString	m_strTrackName;
	int			m_nTrackLength;		// in CM
	int			m_nNumLanes;
	int			m_nLaneColor[8];
	int			m_nDivisionType;

public:
	CPgSetupTrack(CSetupTrack *pRM, BOOL bReadOnly);

	// from WSF

	virtual BOOL OnOkButton();

// Dialog Data
	//{{AFX_DATA(CPgSetupTrack)
	enum { IDD = IDD_SETUP_TRACK };
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPgSetupTrack)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPgSetupTrack)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
