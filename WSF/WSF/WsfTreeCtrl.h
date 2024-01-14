///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 02 25
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

// Notify messages

#define	WSF_TREECTRL_NM_DELETE			5000
#define WSF_TREECTRL_NM_ENTER				5001


//*****************************************************************************
class WSL_API CWsfTreeCtrl : public CTreeCtrl
{
public:
	CWsfTreeCtrl();
	virtual ~CWsfTreeCtrl();

	// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfTreeCtrl)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfTreeCtrl)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

