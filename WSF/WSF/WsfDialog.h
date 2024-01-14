///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 05
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "WsfChildernResizer.h"

/*****************************************************************************/
class WSL_API CWsfDialog : public CDialog
{
protected:
   CWsfChildrenResizer m_Resizer;

public:
   CWsfDialog();
	CWsfDialog(int nTemplateId);

   DECLARE_DYNCREATE(CWsfDialog)

   // Creates the dialog that will behave as a control inside another dialog.
   // If a template window ID is specified (!=0), the dialog
   // will get its position, size, and z-order. The template window will be destroyed.
   BOOL CreateAsControl(int nDialogTemplateId, CWnd* pParentWnd, int nPositionTemplateId);

   // Uses the dialog template specified in the constructor.
   BOOL CreateAsControl(CWnd* pParentWnd, int nPositionTemplateId);

   // Sets all the dialog's controls read-only
   void SetControlsReadOnly();

   // Sets the given control to use a bold font
   void SetControlBoldFont(int nID);

   //---------------------------------------------------------------------------
   // INTERNALS
   //---------------------------------------------------------------------------
private:
   BOOL IsEditCtrl(CWnd* pWnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfDialog)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfDialog)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};
