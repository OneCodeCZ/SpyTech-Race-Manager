///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "WslButton.h"
#include "WslStatic.h"
#include "WslTabCtrl.h"

class CWslPropertyPage;
class CWslPropertySheet;

// Called when OK, DONE, CANCEL button is pressed.
// Return true if the PS can be closed.

typedef BOOL WSL_PS_CALLBACK(void *pData, CWslPropertySheet& s, BOOL bOk);

// Sheet types
#define WSL_PST_OKCANCEL			0	// false
#define WSL_PST_WIZARD				1	// true
#define WSL_PST_CLOSE					2
#define WSL_PST_OKCANCELAPPLY	3

//*****************************************************************************
class CWslPropertySheet : public CWslDialog
{
friend CWslPropertyPage;

protected:
	CWslArray<CWslPropertyPage*> m_aPages;
	WSL_PS_CALLBACK *m_pfnCallback;

	int					m_nType;
	BOOL				m_bModal;
	CWslString	m_sTitle;
	int					m_nActivePage;
	CWslButton	m_OkDoneNextCtrl, m_CancelCloseCtrl, m_HelpCtrl, m_PrevCtrl;
	CWslStatic	m_LineCtrl;
	CWslTabCtrl	m_TabCtrl;
	void				*m_pCallbackData;
	HFONT				m_hFont;
	int					m_nPageX, m_nPageY;		// Left corner of the page
	int					m_nButtonsState;			// The user states of buttons (enable/disable)
	HBITMAP			m_hHelpBitmap;

public:
	CWslPropertySheet(int nType,
		int nHelpBitmapId,
		WSL_PS_CALLBACK* pfnCallback = NULL, 
		void* pData = NULL);

	virtual ~CWslPropertySheet();
	
	//---------------------------------------------------------------------------
	// API
	//---------------------------------------------------------------------------

	// Creates the sheet in modal mode.
	int DoModal(CWslWnd* pParent);

	// Creates modeless property sheet,
	BOOL Create(CWslWnd* pParent);

	// Sets the title of the PS
	void SetTitle(const char *pcszTitle);

	// Adds a page to the PS. If the PS is already displayed,
	// pages can be added only if it's not a wizard!
	void AddPage(CWslPropertyPage *ppg);

	// Removes and DELETES the given page. Pages can removed only
	// if the dialog is displayed and it's not a wizard.
	void RemovePage(CWslPropertyPage* ppg);

	// Removes and DELETES all pages.
	void RemoveAllPages();

	// Returns the page at the given index. 
	CWslPropertyPage* GetPage(int nPage);
	
	// Returns number of pages.
	inline int GetPageCount() const { return m_aPages.GetSize(); }

	// Sets the active pages.
	BOOL SetActivePage(int nPage);

	// Returns the type of PS.
	inline BOOL IsWizard() const { return m_nType == WSL_PST_WIZARD; }

	// Sets page title. The PS must be displayed.
	void SetPageTitle(CWslPropertyPage* ppg, const char* pcszTitle);

	// Buttons management
	void EnableOkDoneNextButton(BOOL bEnable);
	void EnablePrevButton(BOOL bEnable);
	void EnableCancelButton(BOOL bEnable);
	void SetOkDoneNextText(const char* pcszText);

	// Sends a message to all pages
	void SendMessageToAllPages(int nMsg, WPARAM wParam, LPARAM lParam);

  //---------------------------------------------------------------------------
	// Internal Stuff
	//---------------------------------------------------------------------------

protected:
	virtual BOOL OnInitDialog();
	virtual DWORD OnNotify(int nId, NMHDR* pData);
	virtual BOOL OnCommand(int nCmdId);
	virtual int WndProc(int nMsg, DWORD wParam, DWORD lParam); 

	void OnOkNextDone();
	void OnCancelButton();
	void OnHelp();
	void OnPrev();
	void OnCancel();

	void OnSysCommand(UINT nID, LPARAM lParam);
	DWORD OnSelchangeTab(NMHDR* pNMHDR);

	void CreatePage(CWslPropertyPage* ppg);
	void AddPageToTabCtrl(CWslPropertyPage* ppg);
};









