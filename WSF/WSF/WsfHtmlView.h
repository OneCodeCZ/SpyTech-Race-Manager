///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 04
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>

/*****************************************************************************/
class WSL_API CWsfHtmlView : public CHtmlView, public IWsfTabWndDockable
{
public:
	CWsfHtmlView();
	virtual ~CWsfHtmlView();

   //---------------------------------------------------------------------------
   // From IWsfView
   //---------------------------------------------------------------------------

   virtual void OnInitToolbar(CWsfToolBar& tb);

	//{{AFX_DATA(CWsfHtmlView)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfHtmlView)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CWsfHtmlView)
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnEditFind();
	afx_msg void OnUpdateEditFind(CCmdUI* pCmdUI);
	afx_msg void OnWsfRefresh();
	afx_msg void OnUpdateWsfRefresh(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnWsfBrowseBack();
	afx_msg void OnUpdateWsfBrowseBack(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
