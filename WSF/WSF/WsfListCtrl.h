///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class WSL_API CWsfListCtrl : public CListCtrl
{
public:
   // These events are send to the list's parent 
   // in the form of a WM_NOTIFY message
   enum ENotifyEvents  
   {
      KEY_DELETE    = 5000,
      KEY_BACKSPACE = 5001,
      KEY_ENTER     = 5002
   };

public:
	CWsfListCtrl();
	virtual ~CWsfListCtrl();
	
	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfListCtrl)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfListCtrl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
