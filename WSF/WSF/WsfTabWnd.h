///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 11
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "WsfToolBar.h"
#include "WsfFlatButton.h"

/*****************************************************************************/
class WSL_API IWsfTabWndDockable
{
public:
   virtual void OnInitToolbar(CWsfToolBar& tb) = 0;
};


//*****************************************************************************
class WSL_API CWsfTabWnd : public CWnd
{
public:
   enum ENotifyEvents
   {
      CLOSE_BUTTON_CLICKED  = 1,
      SEL_CHANGED           = 2
   };

   enum EStyles 
   { 
      TABS,             // tabs on the top 
      BUTTONS          // vertical stack of buttons on the bottom (Outlook 2003)
   };

   class CSelChangedInfo
   {
   public:
	   NMHDR nmhdr;
	   //CWnd* pPrevWnd;				// Previously active window
	   CWnd* pNewWnd;				// Current active window
   };

protected:
	class CTab
	{
   public:
		CWnd*		pWnd;
		CString	sTitle;
		int		nIconIdx;
		BOOL		bEnableCloseWindow;
	};


protected:
	CWslArray<CTab*>	m_aTabs;
	CWslArray<CWnd*>	m_aSwitchOrder;		// order for CTRL+TAB stuff

	CTabCtrl 			m_TabCtrl;
	HFONT					m_hFont, m_hHeaderFont;
	int					m_nActiveTabIdx;
	CWsfFlatButton 	m_CloseCtrl;
   CStatic           m_TitleCtrl, m_TitleBackgndCtrl;
   CWsfToolBar       m_ToolBar;

   // Styles
   EStyles           m_eStyle;
   BOOL              m_bShowCloseButton;

   // Layout 
	int					m_nWW, m_nWH;
   int               m_nHH;
   int               m_nBottomTabH;

public:
	CWsfTabWnd(EStyles eStyle, BOOL bShowCloseButton);
	virtual ~CWsfTabWnd();

   BOOL IsCloseButtonVisible() { return m_CloseCtrl.m_hWnd != NULL; }

   //---------------------------------------------------------------------------
   // OPENING/CLOSING TAB
   //---------------------------------------------------------------------------

	// Adds a new master window at the specified pos. 
	// If pos is -1, the it's added at the end.
	void OpenTab(CWnd* pMasterWnd, 
      const char* pcszTitle, 
		int nPos,
		BOOL bActivate,
		BOOL bEnableCloseWindow, 
		int nIconIdx);

	// Removes a window. The CWnd object is NOT destroyed and deleted.
	// Returns TRUE if the window was removed.
	BOOL CloseWnd(CWnd* pWnd, CWnd* pNewActiveWnd = NULL);

	// Attempts to close all Wnds. If some Wnd refuse to close
	// FALSE is returned. The Wnd objects are deleted.
	BOOL CloseAllTabs();

   //---------------------------------------------------------------------------
   // MODIFYING TAB
   //---------------------------------------------------------------------------

	// Updates title and icon of the given window.
	void SetTabTitleIcon(CWnd* pWnd, const char* pcszTitle, int nIconIdx = -1);

   // Replaces the given window with a new window. 
   // The old window is NOT destroyed and is hidden.
   BOOL SetTabWnd(CWnd* pCurrentWnd, 
      CWnd* pNewWnd,
      const char* pcszTitle,
      int nIconIdx);

   //---------------------------------------------------------------------------
   // QUERYTING
   //---------------------------------------------------------------------------

	// Returns TRUE if the given window has been added as a master.
	BOOL CheckWnd(CWnd* p) { return FindWnd(p) != -1; }

	// Returns number of managed windows
	inline int GetTabCount() const { return m_aTabs.GetSize(); }

   //---------------------------------------------------------------------------
   // ACTIVE WINDOW
   //---------------------------------------------------------------------------

	// Activates the given window.
	// If bNotify is TRUE, it sends WSF_TABWND_NM_SELCHANGED message 
	// to the frame window.
	void SetActiveWnd(CWnd* pWnd,	BOOL bNotify = true);

	// Returns an active master window.
	// NULL if there is no master window.
	CWnd* GetActiveWnd();

	// Moves the active window to the top of the switch order.
	void MoveActiveWndToSwitchOrderTop();

	// Activates the window at specified switch order pos.
	// The following operation is performed on nWndPos: pos = pos % num_of_wnds
	// The switch order is not changed.
	void ActivateNextWnd(int nSwitchOrderPos);

  // Returns TRUE if the current view can be closed, if any
  inline BOOL CanBeActiveWndClosed() const {  return m_nActiveTabIdx != -1 && m_aTabs[m_nActiveTabIdx]->bEnableCloseWindow; }

  //---------------------------------------------------------------------------
  // INTERNALS
  //---------------------------------------------------------------------------
protected:
	void RecalcLayout();
	void DrawHeader(BOOL bRedrawButton);
	void OnCloseButtonClicked();
	void CloseTab(int nTabIdx, CWnd* pNewActiveWnd);  //pNewActiveWnd can be NULL
	void SetActiveWnd(int nWnd, BOOL bNotify, BOOL bUpdateSwitchOrder);
	int FindWnd(CWnd* pWnd);
	void SendSelchangedNotify(CWnd* pPrev, CWnd* pNew);
	void OnTabContextMenu();

	// Returns name of the window clipped to defined width.
	CString GetClippedTitle(const char* pcszTitle, int nWidth);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfTabWnd)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CWsfTabWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnWsfNextTabWindow();
	afx_msg void OnUpdateWsfNextTabWindow(CCmdUI* p);
	afx_msg void OnWsfPrevTabWindow();
	afx_msg void OnUpdateWsfPrevTabWindow(CCmdUI* p);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

