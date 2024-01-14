///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 07
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
class CWsfFrame;
#include "WsfTreeCtrl.h"

//*****************************************************************************
class WSL_API CWsfTreeItemTreeView : public CView
{
protected:
   CWsfTreeCtrl		m_TreeCtrl;
   
   // For drag & drop
   COleDropTarget	   m_dt;
   CPoint				m_LastPoint;
   HTREEITEM			m_hLastItem;
   DROPEFFECT			m_LastDropEffect;
   DWORD					m_dwTime;
   DWORD					m_dwLastKeyState;
   
public:
   CWsfTreeItemTreeView(); 
   virtual ~CWsfTreeItemTreeView();
  
   // Expands the given item
   BOOL ExpandItem(CWsfTreeItem *pItem);
   
   // Ensures that the given item is visible
   void EnsureVisible(CWsfTreeItem *pItem);
   
   // Rebuilds the tree control's tree to match that
   // given by the pItem parameter.
   // nChangeType specifies the type of change that was performed on the pItem.
   void TreeHasChanged(CWsfTreeItem *pItem, CWsfApp::ETreeChanges eChange);
   
   // Returns root item displayed in thi view
   CWsfTreeItem* GetRootItem() const { return (CWsfTreeItem*) m_TreeCtrl.GetItemData(m_TreeCtrl.GetRootItem()); }
   
protected:
   void InsertItem(CWsfTreeItem *pItem);
   void BuildTree(CWsfTreeItem *pItem, bool bSubTree);
   void RemoveDeletedChildern(CWsfTreeItem *pParent, bool bSubTree);
   void UpdateTitleIcon(CWsfTreeItem *pItem, bool bSubTree);
   void AddNewChildern(CWsfTreeItem *pParent, bool bSubTree);
   void RedrawItem(CWsfTreeItem *pItem);
   void UpdatePlusSign(CWsfTreeItem *pItem);
   
   // Returns selected tree item or NULL if none.
   CWsfTreeItem* GetSelectedTreeItem();
   
   // Displays a context menu at specified screen position.
   void OnRclick(int nX, int nY);
   
   void OnDblclk(NMHDR *,LRESULT *);
   BOOL OnBeginLabelEdit(NMTVDISPINFO *pInfo);
   BOOL OnEndLabelEdit(NMTVDISPINFO *pInfo);
   void OnItemExpanded(NMTREEVIEW *pInfo);
   
   virtual void OnDraw(CDC *pDC) {};
   
   static void __ResetHTREEITEM(CWsfTreeItem* p, void*);
   
   // Overrides
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CWsfTreeItemTreeView)
public:
   virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
   virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
   virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
   virtual void OnDragLeave();
   virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
protected:
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
   virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
   virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
   //}}AFX_VIRTUAL
   
   // Generated message map functions
protected:
   //{{AFX_MSG(CWsfTreeItemTreeView)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSetFocus(CWnd* pOldWnd);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnWsfRename();
   afx_msg void OnWsfDelete();
   afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
   afx_msg void OnDestroy();
   //}}AFX_MSG
   DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
