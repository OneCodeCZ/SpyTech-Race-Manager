///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class CWsfPropertyPage;
class CWsfPropertySheet;

//*****************************************************************************
class WSL_API CWsfPropertySheet : public CDialog
{
public:
	// Called when OK, DONE, CANCEL button is pressed.
	// Return true if the PS can be closed.
	typedef BOOL WSF_PS_CALLBACK(void *pData, CWsfPropertySheet& s, BOOL bOk);

protected:
   enum EModes    { NORMAL, WIZARD, CONTROL };
   enum EStates   { INIT, RUNNING, DESTROYED };

protected:
   EModes   m_eMode;
   EStates  m_eState;
   
   CWslArray<CWsfPropertyPage*> m_aPages;
	WSF_PS_CALLBACK* m_pfnCallback;
	CString	m_sTitle;
	int		m_nActivePage;
	CButton	m_OkDoneNextCtrl, m_CancelCtrl, m_PrevCtrl;
	//CButton	m_HelpCtrl;
	CStatic	m_LineCtrl;
	CTabCtrl	m_TabCtrl;
	void*    m_pCallbackData;
	HFONT		m_hFont;
	//HBITMAP	m_hHelpBitmap;
	int		m_nPageX, m_nPageY;		// Left corner of the page
	int		m_nButtonsState;			// The user states of buttons (enable/disable)
	HHOOK		m_hHook;

	// NOTE: For hook processing -> there can be only one standalone property sheet
	// at the same time.
	static CWsfPropertySheet*	m_pps;

   // Page navigation history. Used in wizard.
   CWslArray<int>    m_aNavHistory;

public:
	CWsfPropertySheet(BOOL bWizard, 
      WSF_PS_CALLBACK *pfnCallback = NULL, 
      void *pData = NULL);

	~CWsfPropertySheet();

   // Creates the PS supposing it will be used as a control in a dialog.
   // If the nId control already exists. The PS will have 
   // its size and position, and the control will be destroyed.
   void CreateAsControl(CWnd* pParent, int nPositionTemplateId);

	// Sets the title of the PS
	void SetTitle(const char* pcszTitle);
	void SetTitle(int nStringId);

   // Mode
   void SetWizardMode(BOOL bWizard);
	inline BOOL IsWizard() const { return m_eMode == WIZARD; }
   inline BOOL IsControl() const { return m_eMode == CONTROL; }

   //---------------------------------------------------------------------------
	// PAGES
	//---------------------------------------------------------------------------

	// Adds a page to the PS. If the PS is already displayed,
	// pages can be added only if it's not a wizard!
	BOOL AddPage(CWsfPropertyPage* ppg);

	// Removes and DELETES the given page. Pages can removed only
	// if the dialog is displayed and it's not a wizard.
	void RemovePage(CWsfPropertyPage* ppg);

	// Removes and DELETES all pages.
	void RemoveAllPages();

	// Returns the page at the given index or the page of the given type
	// If the page is not found, NULL is returned.
	CWsfPropertyPage* GetPage(int nPage);
   CWsfPropertyPage* GetPage(const CRuntimeClass* pClass);

	// Returns number of pages.
	inline int GetPageCount() const { return m_aPages.GetSize(); }

	// Sets the active pages.
	BOOL SetActivePage(int nPage);

	// Sets page title. The PS must be displayed.
	void SetPageTitle(CWsfPropertyPage* ppg, const char* pcszTitle);

   // Returns an index of the active page
   int GetActivePage() const { return m_nActivePage; }

   //---------------------------------------------------------------------------
   // BUTTONS
   //---------------------------------------------------------------------------

	void EnableOkDoneNextButton(BOOL bEnable);
	void EnablePrevButton(BOOL bEnable);
	void EnableCancelButton(BOOL bEnable);
	void SetOkDoneNextText(const char* pcszText);

   //---------------------------------------------------------------------------
   // ACTIONS
   //---------------------------------------------------------------------------

   // Updates data on the active page, calls OnOKButton() of all pages, and 
   // then calls the PS's callback.
   BOOL Apply();

   //---------------------------------------------------------------------------
	// INTERNALS
	//---------------------------------------------------------------------------
protected:

   // Recalcs layout of controls depending on the sheet size
   void RecalcLayout(int nW, int nH);

   // Given a page size, calculates the sheet size of the page
   void GetSheetSizeFromPageSize(int& nW, int& nH);

	void OnOkNextDone();
	void OnCancelButton();
	void OnHelp();
	void OnPrev();
	BOOL CreatePage(CWsfPropertyPage* ppg);
	void AddPageToTabCtrl(CWsfPropertyPage* ppg);

	// Hook
	static LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam);

// Dialog Data
	//{{AFX_DATA(CWsfPropertySheet)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWsfPropertySheet)
	public:
	virtual void OnCancel();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CWsfPropertySheet)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/*****************************************************************************/
template<class T> T* WsfGetProperySheetPage(CWsfPropertySheet& s)
{
	for (int i=0; i<s.GetPageCount(); i++)
	{
		T* p = dynamic_cast<T*>(s.GetPage(i));
		if (p) return p;
	}
	return NULL;
}


