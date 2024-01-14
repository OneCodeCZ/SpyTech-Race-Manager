///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 11
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//*****************************************************************************
class WSL_API CWsfFrame : public CFrameWnd
{
friend CWsfApp;
friend CWsfToolBar;
friend CWsfTabWnd;
   
protected:  
   CWsfVSplitter*	m_pSplitterWnd;
   CWsfTabWnd*		m_pLeftPane;
   CWsfTabWnd*		m_pRightPane;
   
public:
   CWsfStatusBar	m_StatusBarWnd;
   
private:
   BOOL				m_bLeftPaneVisible;
   
public:
   CWsfFrame();
   virtual ~CWsfFrame();
   
   // --------------------------------------------------------------------------
   // Panes
   // --------------------------------------------------------------------------
   
   // Shows or hides the tab window with tree views.
   void ShowLeftPane(BOOL bShow);
   
   // ....
   inline BOOL IsLeftPaneVisible() const { return m_bLeftPaneVisible; }
   
   // Activates the given pane. If there is now window
   // in the pane, it does nothing. If the left pane is hidden
   // and should be activatedm it does nothing.
   void ActivatePane(BOOL bRight);
   
   // Returns the desired pane.
protected: 
   CWsfTabWnd* GetPane(BOOL bLeft) { return bLeft ? m_pLeftPane : m_pRightPane; }
public:
   
   // --------------------------------------------------------------------------
   // Various
   // --------------------------------------------------------------------------
   
   // Returns true, if command is enabled in current context 
   // (selection, doc, view, etc.)
   BOOL IsCmdEnabled(int nCmdId);
   
   // Called when title of the frame window should reflect the current 
   // active view's document name.
   virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
   
   // Returns active document -> document of the selected window in workplace
   // tab window.
   virtual CDocument* GetActiveDocument();
   
   // Translate some default IDs
   virtual void GetMessageString(UINT nID, CString& rMessage) const;
   
protected:
   
   // --------------------------------------------------------------------------
   // Various 
   // --------------------------------------------------------------------------
   
   // Activates document with the given index in the doc list. 
   // If bSwap is TRUE the document will be moved to the top of the list.
   // Index is put in boundaries using "% m_aDocs.GetSize()"
   void ActivateNextDoc(int nDocIdx, BOOL bSwap);
   
   // Helper stuff
   void OnWsfSelchanged(NMHDR *pNmhdr);
   
   // Displays menu to choose to show/hide toolbars
   void ChooseToolBars();
   
   // WSF Messages Handlers
   afx_msg LRESULT OnWsfWmDbHasChanged(WPARAM wParam, LPARAM lParam);
   
   // --------------------------------------------------------------------------
   // Pane changing
   // --------------------------------------------------------------------------
   
   // Activates next or previous pane. bDir is TRUE -> next.
   void ChangePane(BOOL bDir);
   
   // Command handling.
   void OnWsfNextPane();
   void OnUpdateWsfNextPane(CCmdUI* pCmdUI);
   void OnWsfPreviousPane();
   void OnUpdateWsfPreviousPane(CCmdUI* pCmdUI);
   
   // Overrides
   
   // ClassWizard generated virtual function overrides
   //{{AFX_VIRTUAL(CWsfFrame)
public:
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
protected:
   virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
   //}}AFX_VIRTUAL
   
   virtual BOOL OnToolTipNeedTextA(UINT,NMHDR *,LRESULT *);
   virtual BOOL OnToolTipNeedTextW(UINT,NMHDR *,LRESULT *);

protected:
   
   // Generated message map functions
   
   //{{AFX_MSG(CWsfFrame)
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnWsfTreeView();
   afx_msg void OnUpdateWsfTreeView(CCmdUI* pCmdUI);
   afx_msg void OnDestroy();
   afx_msg void OnWsfWindowClose();
   afx_msg void OnUpdateWsfWindowClose(CCmdUI* pCmdUI);
   afx_msg void OnClose();
   afx_msg void OnViewLargeicon();
   afx_msg void OnUpdateViewLargeicon(CCmdUI* pCmdUI);
   afx_msg void OnViewList();
   afx_msg void OnUpdateViewList(CCmdUI* pCmdUI);
   afx_msg void OnViewSmallicon();
   afx_msg void OnUpdateViewSmallicon(CCmdUI* pCmdUI);
   afx_msg void OnViewReport();
   afx_msg void OnUpdateViewReport(CCmdUI* pCmdUI);
   afx_msg void OnWindowCloseall();
   afx_msg void OnUpdateWindowCloseall(CCmdUI* pCmdUI);
   afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
   afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
   afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
   //}}AFX_MSG
   
   DECLARE_MESSAGE_MAP()
};


