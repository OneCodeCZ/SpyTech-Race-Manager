///////////////////////////////////////////////////////////////////////////////
//
// SpyTech Application Framework 
// -----------------------------
// 
// Copyright (C)2000 Radek Tetik 
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class WSL_API CWsfStatusBar : public CStatusBar
{
protected:
	CMapWordToPtr m_Map;

public:
	CWsfStatusBar();
	virtual ~CWsfStatusBar();

	// Creates controls in the specified pane

	CStatic *SetPaneIcon(int nPane, HICON hIcon, int nWidth = -1);
	CAnimateCtrl *SetPaneAnimation(int nPane, int nId, int nWidth = -1);

	// Removes control from the given pane

	void RemovePaneCtrl(int nPane);

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfStatusBar)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfStatusBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
