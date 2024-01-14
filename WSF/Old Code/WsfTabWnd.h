///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 10
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

// Flags

#define WSF_TWF_CONTEXT_MENU						1

// Notification messages sent to the parent frame window

#define	WSF_TABWND_NM_ONCLOSE						1
#define WSF_TABWND_NM_SELCHANGED				2

class CWsfFlatButton;

//*****************************************************************************
struct WSF_TABWND_SELCHANGED_INFO
{
	NMHDR nmhdr;
	//CWnd* pPrevWnd;				// Previously active window
	CWnd* pNewWnd;				// Current active window
};

//*****************************************************************************
class WSL_API CWsfTabWnd : public CWnd
{
protected:
	struct CWndInfo
	{
		CWnd*					pWnd;
		CWnd*					pSlaveWnd;
		CString				sTitle;
		int						nIconIdx;
		int						nTitleLen;
		BOOL					bEnableCloseWindow;
		class CWsfHSplitter*	pSplitter;
		BOOL					bMasterFirst;
	};

protected:
	CWslArray<CWndInfo*>	m_aWnds;
	CWslArray<CWnd*>			m_aSwitchOrder;		// order for CTRL+TAB stuff

	CTabCtrl*				m_pTabCtrl;
	HFONT						m_hFont, m_hHeaderFont;
	int							m_nWW, m_nWH, m_nTH, m_nHH, m_nFH;
	int							m_nActiveWnd;
	CFrameWnd*			m_pFrameWnd;
	CWsfFlatButton*	m_pCloseCtrl;
	HDC							m_hDC;
	int							m_nFlags;

public:
	CWsfTabWnd(int nFlags);
	virtual ~CWsfTabWnd();

	// Adds a new master window at the specified pos. 
	// If pos is -1, the it's added at the end.
	void AddWnd(CWnd* pWnd, const char* pcszTitle, 
		int nPos,
		BOOL bActivate,
		BOOL bEnableCloseWindow, 
		int nIconIdx);

	// Removes a window. The CWnd object is NOT deleted.
	// It can be a master or slave window.
	// Returns TRUE if the window was removed.
	BOOL RemoveWnd(CWnd* pWnd, CWnd* pNewActiveWnd = NULL);

	// Sets master window's slave window.
	// Returns FALSE, when the master doesn't exists.
	BOOL AddSlaveWnd(CWnd* pMaster, 
		CWnd* pSlave, 
		BOOL bMasterFirst,
		int nDefSplitterPos);

	// Attempts to close all Wnds. If some Wnd refuse to close
	// FALSE is returned. The Wnd objects are deleted.
	BOOL CloseAllWnds();

	// Activates the given window.
	// If bNotify is TRUE, it sends WSF_TABWND_NM_SELCHANGED message 
	// to the frame window.
	void SetActiveWnd(CWnd* pWnd,	BOOL bNotify = true);

	// Returns an active master window.
	// NULL if there is no master window.
	CWnd* GetActiveWnd();

	// Returns active master or slave window according the given position.
	// NULL if there is no window.
	CWnd* GetActiveWnd(BOOL bFirstPane);

	// Returns an active slave window.
	// NULL if there is no slave window.
	CWnd* GetActiveSlaveWnd();

	// Updates title and icon of the given window.
	void UpdateWndTitleIcon(CWnd* pWnd, const char* pcszTitle, int nIconIdx = -1);

	// Returns number of managed windows
	inline int GetWndCount() const { return m_aWnds.GetSize(); }

	// Moves the active window to the top of the switch order.
	void MoveActiveWndToSwitchOrderTop();

	// Activates the window at specified switch order pos.
	// The following operation is performed on nWndPos: pos = pos % num_of_wnds
	// The switch order is not changed.
	void ActivateNextWnd(int nSwitchOrderPos);

	// Returns TRUE if the given window has been added as a master.
	BOOL CheckWnd(CWnd* p) { return FindWnd(p) != -1; }

	// Returns TRUE if the given window has been added as a slave.
	BOOL CheckSlaveWnd(CWnd* p) { return FindSlaveWnd(p) != -1; }

	// Returns TRUE if there is a master/slave window active
	inline BOOL IsMasterWndActive() const { return m_nActiveWnd != -1; }
	inline BOOL IsSlaveWndActive() const { return m_nActiveWnd != -1 && m_aWnds[m_nActiveWnd]->pSlaveWnd != NULL; }

  // Returns TRUE if the current view can be closed, if any
  inline BOOL CanBeActiveWndClosed() const {  return m_nActiveWnd != -1 && m_aWnds[m_nActiveWnd]->bEnableCloseWindow; }

protected:
	void RecalcLayout();
	void DrawHeader(BOOL bRedrawButton);
	void OnCloseButtonClicked();
	void RemoveWnd(int nWnd, CWnd* pNewActiveWnd);  //pNewActiveWnd can be NULL
	void SetActiveWnd(int nWnd, BOOL bNotify, BOOL bUpdateSwitchOrder, BOOL bSlave);
	int FindWnd(CWnd* pWnd);
	void SendSelchangedNotify(CWnd* pPrev, CWnd* pNew);
	BOOL OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
	void OnTabContextMenu();

	// Returns name of the window clipped to defined width.
	CString GetClippedTitle(const char* pcszTitle, int nWidth);

	// Finds an index of the given slave window.
	int FindSlaveWnd(CWnd* pWnd);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfTabWnd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	// Generated message map functions
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
	//}}AFX_MSG


	DECLARE_MESSAGE_MAP()
};

