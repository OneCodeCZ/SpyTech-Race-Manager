///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 09
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class CWsfTableCtrl;
class CWsfQueryDoc;

//*****************************************************************************
class WSL_API CWsfQueryView : public CView
{
friend CWsfQueryDoc;

protected:
	struct CChangeInfo
	{
		int	nChangeType;
		int nPrimaryKey;
	};

protected:
	CWsfTableCtrl*	m_pList;
	BOOL						m_bInOnCmdMsg;
	BOOL						m_bUseFilter;
	CWslArray<CChangeInfo>	m_aChanges;		// Queue of changes issued during handling of a command
	BOOL						m_bRequery;		
		
public:
	CWsfQueryView();
	virtual ~CWsfQueryView();

  //---------------------------------------------------------------------------
	// User API
	//---------------------------------------------------------------------------

	inline void EnableFilter(BOOL bEnable) { m_bUseFilter=bEnable; }

  //---------------------------------------------------------------------------
	// Internal view drawing
	//---------------------------------------------------------------------------
protected:
	void BuildList(BOOL bRememberPos);

	// Sets the message string on the status bar
	void SetMessageString(BOOL bEmpty = false);

	// Loads the data for the given row from the database.
	// If the row is already in the list, it's updated.
	void UpdateRow(int nPrimaryKey);

	// Adds/updates the current row in the recorset to/in the list.
	// If nListItem is -1, the row is added, otherwise the list item
	// is updated.
	void AddUpdateRow(CWslDbRecordset &rs, 
		int nListItem,
		BOOL bDrawRow) throw (CWslException*);

	// Returns query. If nPrimaryKey != -1 then only the row with
	// the given primary key is returned by the query.
	// Sorting is also taken into account.
	// Calls CWsfQueryDoc::GetQuery method.
	void GetQuery(CString &sQuery, int nPrimaryKey);
	
	// Performs all changes in the m_aChanges array.
	void PerformChanges();

	// Displays the context menu on the specified position
	void OnRclick(int nX, int nY);

	// Returns the document
	inline CWsfQueryDoc& GetDoc() const { return *(CWsfQueryDoc*) GetDocument(); };

	// Saves the profile of the table control using 
	// CWsfQueryDoc::OnSaveProfile() method.
	void SaveProfile();

	//---------------------------------------------------------------------------
	//  MFC
	//---------------------------------------------------------------------------

	// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfQueryView)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnInitialUpdate();
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CWsfQueryView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnWSF_WM_UPDATE_MESSAGE_TEXT(WPARAM wParam, LPARAM lParam);
	afx_msg void OnWsfToogleFilter();
	afx_msg void OnWsfFilterDlg();
	afx_msg void OnUpdateWsfToogleFilter(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWsfFilterDlg(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnWsfRefresh();
	afx_msg void OnUpdateWsfRefresh(CCmdUI* pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
