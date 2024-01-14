///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "WsfTableCtrl2.h"

//*****************************************************************************
class WSL_API CWsfTableView : public CView
{
public:
	// Use CDocument::UpdateAllView() method to send these control
	// messages.
	enum EUVCodes 
	{ 
		REFRESH				= 30600,		// refreshes the view
		//RESET					= 30601, 	// Resets the view: loads profile, redefines columns
		SAVE_LAYOUT		   = 30602		// Saves the layout of the table control
	};


protected:
	CWsfTableCtrl2*  m_pTbl;
   CWsfToolBar*     m_pToolBar; 
   int              m_nW, m_nH;

public:
   // The view deletes the table when being destroyed
	CWsfTableView(CWsfTableCtrl2* pTbl);
	virtual ~CWsfTableView();
	DECLARE_DYNAMIC(CWsfTableView)

   void EnableToolBar(BOOL bEnable = true);

   CWsfTableCtrl2* GetTbl() { return m_pTbl; }
   CWsfToolBar* GetToolBar() { return m_pToolBar; }

protected:
   void RecalcLayout();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfTableView)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfTableView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
