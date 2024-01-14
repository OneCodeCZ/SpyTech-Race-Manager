///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "WsfListCtrl.h"

//*****************************************************************************
class WSL_API CWsfTreeItemListView : public CView
{
protected:
	CWsfTreeItem*			m_pRootTreeItem;
	CWsfListCtrl			m_ListCtrl;
	CWslArray<CWsfTreeItem*>	m_aSelectedItems;

	// For drag & drop
	COleDropTarget		m_dt;
	CPoint						m_LastPoint;
	int								m_nLastItem;
	DROPEFFECT				m_LastDropEffect;
	DWORD							m_dwLastKeyState;

public:
	CWsfTreeItemListView(CWsfTreeItem* pItem);
	virtual ~CWsfTreeItemListView();

	// --------------------------------------------------------------------------
	// API
	// --------------------------------------------------------------------------

	// Sets new tree item and updates the view.
	// The pSelectItem is selected. If NULL, the first child is selected.
	void SetTreeItem(CWsfTreeItem* pItem, CWsfTreeItem* pSelectItem = NULL);

protected:
	// --------------------------------------------------------------------------
	// Internal various
	// --------------------------------------------------------------------------

	// Builds the list form m_pRootTreeItem's childs.
	// pSelectItem will be selected. 
	// pSelectItem == NULL -> first item is selected
	void BuildList(CWsfTreeItem* pSelectItem);

	// No drawing is neccesary
	virtual void OnDraw(CDC *pDC) {};

	// Sets the message string on the status bar
	void SetMessageString(bool bEmpty = false);

	// Stuff
	void OnRclick(int x, int y);
	BOOL OnBeginLabelEdit(NMLVDISPINFO *pInfo);
	BOOL OnEndLabelEdit(NMLVDISPINFO *pInfo);
	void OnItemChanged(NMLISTVIEW &lv);

	// --------------------------------------------------------------------------
	// Selection of tree items
	// --------------------------------------------------------------------------

	// Adds tree item to the selection.
	void SelectTreeItem(CWsfTreeItem* pItem);

	// Removes current selection and selects the given tree item.
	void SelectOneTreeItem(CWsfTreeItem* pItem); 

	// Removes tree item from the selection.
	void UnselectTreeItem(CWsfTreeItem* pItem);

	// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfTreeItemListView)
	public:
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfTreeItemListView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWsfDelete();
	afx_msg void OnWsfRename();
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg LRESULT OnWSF_WM_UPDATE_MESSAGE_TEXT(WPARAM wParam, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

