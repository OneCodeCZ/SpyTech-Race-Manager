///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class WSL_API CWsfToolBarComboBox : public CComboBox
{
protected:
	class CToolEdit : public CEdit
	{
	public:
		CWsfToolBarComboBox* m_pCombo;
	protected:
		virtual BOOL PreTranslateMessage(MSG* pMsg);
	};

friend CToolEdit;

protected:
	CToolEdit* m_pEdit;
	
public:
	CWsfToolBarComboBox();
	virtual ~CWsfToolBarComboBox();
	DECLARE_DYNCREATE(CWsfToolBarComboBox);

protected:
	// Called when the combo's ID is to be posted to the app
	// You should store the combo's value and call the
	// default version that will post the command
	virtual void OnPostCommand();

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfToolBarComboBox)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfToolBarComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};




