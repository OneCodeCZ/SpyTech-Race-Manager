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

#include "stdafx.h"
#include "main.h"
#include "WslDialog.h"
#include "WslPropertyPage.h"
#include "WslPropertySheet.h"

// Controls

#define _IDC_TAB					1
#define _IDC_OKNEXTDONE		2//IDOK
#define _IDC_CANCELCLOSE	3//IDCANCEL
#define _IDC_PREV					4
#define _IDC_HELP					5

// Button states

#define PREV_ENABLED			1
#define CANCEL_ENABLED		2

//*****************************************************************************
CWslPropertySheet::CWslPropertySheet(int nType, 
																 		 int nHelpBitmapId,
																		 WSL_PS_CALLBACK *pfnCallback/*= NULL*/, 
																		 void *pData/*= NULL*/)
{
	//{{AFX_DATA_INIT(CWslPropertySheet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nType = nType;
	m_pfnCallback = pfnCallback;
	m_nActivePage = -1;
	m_pCallbackData = pData;
	m_hFont = NULL;
	m_nButtonsState = PREV_ENABLED|CANCEL_ENABLED;

	m_hHelpBitmap = (HBITMAP) ::LoadImage(WslGetApp().m_hInst, 
		MAKEINTRESOURCE(nHelpBitmapId),
		IMAGE_BITMAP,
		0,0,
		LR_LOADMAP3DCOLORS);
	VERIFY(m_hHelpBitmap);
}

//*****************************************************************************
CWslPropertySheet::~CWslPropertySheet()
{
	RemoveAllPages();
	if (m_hFont) DeleteObject(m_hFont);
	DeleteObject(m_hHelpBitmap);
}

//*****************************************************************************
int CWslPropertySheet::DoModal(CWslWnd* pParent)
{
	struct
	{
		DLGTEMPLATE dt;
		WORD dw[3];
	} s;

	s.dt.style = DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU;
	s.dt.x = 0;
	s.dt.y = 0;
	s.dt.cx = 112;
	s.dt.cy = 77;
	s.dt.dwExtendedStyle = 0;
	s.dt.cdit = 0;	
	s.dw[0]=s.dw[1]=s.dw[2]=0;

	m_bModal = true;
	int n = CWslDialog::DoModal((DLGTEMPLATE*)&s, pParent);
	return n;
}

//*****************************************************************************
BOOL CWslPropertySheet::Create(CWslWnd* pParent)
{
	ASSERT(!IsWizard());

	struct
	{
		DLGTEMPLATE dt;
		WORD dw[3];
	} s;

	s.dt.style = DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU;
	s.dt.x = 0;
	s.dt.y = 0;
	s.dt.cx = 112;
	s.dt.cy = 77;
	s.dt.dwExtendedStyle = 0;
	s.dt.cdit = 0;	
	s.dw[0]=s.dw[1]=s.dw[2]=0;

	m_bModal = false;
	return CWslDialog::Create((DLGTEMPLATE*)&s, pParent);
}

//*****************************************************************************
BOOL CWslPropertySheet::OnInitDialog() 
{
	int i, nW, nH, x, y;
	RECT r;

	// Init

	ASSERT(m_aPages.GetSize() > 0);
	CWslDialog::OnInitDialog();

	// Create font

	m_hFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	ASSERT(m_hFont);

	// Create pages and get max page size

	nW = nH = 0;

	for (i=m_aPages.GetSize(); i--;)
	{
		CreatePage(m_aPages[i]);
		GetClientRect(*m_aPages[i], &r);
		if (r.right - r.left > nW) nW = r.right - r.left;
		if (r.bottom - r.top > nH) nH = r.bottom - r.top;
	}

	// Init property sheet window

	if (IsWizard())
	{
		// Set window size

		r.left = 0;
		r.top = 0;
		r.bottom = nH+15+22+10;
		r.right = nW;

		::AdjustWindowRect(&r, GetWindowLong(*this, GWL_STYLE), false);
		::SetWindowPos(m_hWnd,NULL,0,0,r.right-r.left,r.bottom-r.top, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);

		// Create controls

		y = nH + 4;
		m_LineCtrl.Create(NULL, WS_CHILD|WS_VISIBLE|SS_ETCHEDHORZ, 0, 5,y,nW-5,y+2, this, 0);
		y += 11;

		m_HelpCtrl.Create("?", WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_CENTER|BS_BITMAP, 0, 10,y,25,22, this, _IDC_HELP);
		m_HelpCtrl.SetImage(m_hHelpBitmap, IMAGE_BITMAP);

		x = nW - (70+75+70) - 10;
		m_PrevCtrl.Create("<- Zpìt", WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x,y,70,22, this, _IDC_PREV);
		x += 70;
		m_OkDoneNextCtrl.Create("", WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x,y,70,22, this, _IDC_OKNEXTDONE);
		x += 75;
		m_CancelCloseCtrl.Create("Storno", WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0, x,y,70,22, this, _IDC_CANCELCLOSE);


		m_PrevCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));
		m_OkDoneNextCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));
		m_CancelCloseCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));
		m_HelpCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));

		// Postion pages

		for (i=m_aPages.GetSize(); i--;) ::SetWindowPos(*m_aPages[i],HWND_TOP,0,0,0,0,SWP_NOSIZE);
	}
	else
	{
		// Create tab ctrl

		m_TabCtrl.Create(NULL, WS_CHILD|WS_VISIBLE|WS_TABSTOP,0, 0,0,0,0, this, _IDC_TAB);
		m_TabCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));

		// Add pages

		for (i=0; i<m_aPages.GetSize(); i++) AddPageToTabCtrl(m_aPages[i]);

		// Set tab & window size/pos

		r.left = 0;
		r.top = 0;
		r.bottom = nH;
		r.right = nW;

		m_TabCtrl.SendMessage(TCM_ADJUSTRECT, true, (LPARAM) &r);		// get tab control size
		
		x = -r.left;
		y = -r.top;
		r.left = 5;
		r.top = 5;
		r.right += x + 5;
		r.bottom += y + 5;

		::MoveWindow(m_TabCtrl, r.left, r.top, r.right-r.left, r.bottom-r.top, false); // position the tab control

		r.bottom += 5+5+22+5;			// add borders and space for controls
		r.right += 5+5;

		nH = r.bottom-r.top;			// get new window size
		nW = r.right-r.left;

		::AdjustWindowRect(&r, GetWindowLong(*this, GWL_STYLE), false);
		::SetWindowPos(m_hWnd,NULL,0,0,r.right-r.left,r.bottom-r.top, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);

		// Postion the pages to fit within the tab control

		m_nPageX = x+5;
		m_nPageY = y+5;

		for (i=m_aPages.GetSize(); i--;) 
			::SetWindowPos(*m_aPages[i],HWND_TOP,m_nPageX,m_nPageY,0,0,SWP_NOSIZE);

		// Create controls

		y = nH-22-5;
		m_HelpCtrl.Create("?", WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_CENTER|BS_BITMAP,0, 5,y,25,22, this, _IDC_HELP);
		m_HelpCtrl.SetImage(m_hHelpBitmap, IMAGE_BITMAP);

		x = nW - (75+70) - 5;

		if (m_nType != WSL_PST_CLOSE)
		{
			m_OkDoneNextCtrl.Create("OK", WS_CHILD|WS_VISIBLE|WS_TABSTOP,0, x,y,70,22, this, _IDC_OKNEXTDONE);
			m_OkDoneNextCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));
		}
				
		x += 75;

		m_CancelCloseCtrl.Create(m_nType==WSL_PST_CLOSE ? "Close" : "Storno", WS_CHILD|WS_VISIBLE|WS_TABSTOP,0, x,y,70,22, this, _IDC_CANCELCLOSE);
		m_CancelCloseCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));

		// Title

		::SetWindowText(*this, m_sTitle);
	}

	// Default button

	SendMessage(DM_SETDEFID, 
		m_nType==WSL_PST_CLOSE ? _IDC_CANCELCLOSE : _IDC_OKNEXTDONE, 
		0);

	// Set first page

	CenterWindow();
	SetActivePage(0);

	return false;
}

//*****************************************************************************
void CWslPropertySheet::SetTitle(const char* pcszTitle)
{
	ASSERT(pcszTitle);
	m_sTitle = pcszTitle;
	if (m_hWnd) SetWindowText(pcszTitle);
}

//*****************************************************************************
void CWslPropertySheet::AddPage(CWslPropertyPage *ppg)
{
	ASSERT(ppg);
	ASSERT(!m_hWnd || !IsWizard()); // You cannot add pages when the wizard is displayed!

	// Add it to the array

	m_aPages += ppg;
	ppg->m_pps = this;

	// If PS is already displayed, update it

	if (m_hWnd)
	{
		CreatePage(ppg);
		::SetWindowPos(*ppg,HWND_TOP,m_nPageX,m_nPageY,0,0,SWP_NOSIZE);
		AddPageToTabCtrl(ppg);
	}
}

//*****************************************************************************
void CWslPropertySheet::RemovePage(CWslPropertyPage* ppg)
{
	ASSERT(ppg);
	ASSERT(m_aPages.GetSize() > 1);		// there must remain at least one page 
	ASSERT(m_hWnd);					// PS must be displayed
	ASSERT(!IsWizard());			// You cannot add pages when the wizard is displayed!

	// Find page

	for (int i=m_aPages.GetSize(); i--;) if (m_aPages[i] == ppg) break;
	ASSERT(i!=-1);

	// Update active page

	if (m_nActivePage==i) SetActivePage(i>0 ? i-1 : i+1);
	if (m_nActivePage > i) m_nActivePage--;

	// Delete page

	m_TabCtrl.SendMessage(TCM_DELETEITEM, i, 0);
	ppg->DestroyWindow();
	delete m_aPages[i];
	m_aPages.RemoveAt(i);
}

//*****************************************************************************
void CWslPropertySheet::CreatePage(CWslPropertyPage* ppg)
{
	ppg->Create(ppg->m_nDialogId, this);

	DWORD dw = GetWindowLong(*ppg, GWL_STYLE);
	dw &= ~(WS_DLGFRAME|WS_SYSMENU|WS_CAPTION|WS_VISIBLE);
	dw |= WS_CHILD;
	SetWindowLong(*ppg, GWL_STYLE, dw);

	ppg->EnableWindow(false);
}

//*****************************************************************************
void CWslPropertySheet::AddPageToTabCtrl(CWslPropertyPage* ppg)
{
	ppg->GetWindowText(wsltemp, 1024);

	TCITEM tci;
	tci.pszText = wsltemp;
	tci.mask = TCIF_TEXT;

	m_TabCtrl.SendMessage(TCM_INSERTITEM, 
		m_TabCtrl.SendMessage(TCM_GETITEMCOUNT,0,0),
		(LPARAM) &tci);
}

//*****************************************************************************
void CWslPropertySheet::RemoveAllPages()
{
	for (int i=m_aPages.GetSize(); i--;) delete m_aPages[i];
	m_aPages.RemoveAll();
}	

//*****************************************************************************
CWslPropertyPage* CWslPropertySheet::GetPage(int nPage)
{
	ASSERT(nPage >=0 && nPage < m_aPages.GetSize());
	return m_aPages[nPage];
}

//*****************************************************************************
void CWslPropertySheet::SetPageTitle(CWslPropertyPage* ppg, const char* pcszTitle)
{
	ASSERT(ppg);
	ASSERT(pcszTitle);
	ASSERT(m_hWnd);

	for (int i=m_aPages.GetSize(); i--;) if (m_aPages[i] == ppg) break;
	ASSERT(i!=-1);
	ppg->SetWindowText(pcszTitle);

	if (IsWizard())
	{
		if (m_nActivePage == i)
		{
			wsprintf(wsltemp, "%s - %s", m_sTitle, pcszTitle);
			ppg->SetWindowText(wsltemp);
		}
	}
	else
	{
		if (m_TabCtrl.m_hWnd)
		{
			TCITEM tci;
			tci.mask = TCIF_TEXT;
			tci.pszText = (char*) pcszTitle;
			m_TabCtrl.SendMessage(TCM_SETITEM, i, (LPARAM) &tci);
		}
	}
}

//*****************************************************************************
BOOL CWslPropertySheet::SetActivePage(int nPage)
{
	CWslPropertyPage *ppg;

	ASSERT(nPage >=0 && nPage < m_aPages.GetSize());
	if (m_nActivePage == nPage) return true;

	// Deactivate active page

	if (m_nActivePage != -1)
	{
		if (!m_aPages[m_nActivePage]->OnKillActive(nPage)) return false;
		ASSERT(nPage>=0 && nPage<m_aPages.GetSize());  // returned page must be in range
		
		m_aPages[m_nActivePage]->ShowWindow(SW_HIDE);
		m_aPages[m_nActivePage]->EnableWindow(false);
	}

	// Active the new one

	m_nActivePage = nPage;
	ppg = m_aPages[nPage];
	ppg->ShowWindow(SW_SHOW);
	ppg->EnableWindow(true);
	ppg->SetFocus();
	ppg->OnSetActive();

	// Update PS

	if (IsWizard())
	{
		ppg->GetWindowText(wsltemp2, 1024);
		wsprintf(wsltemp, "%s - %s", m_sTitle, wsltemp2);
		SetWindowText(wsltemp);

		m_PrevCtrl.EnableWindow(nPage > 0 && (m_nButtonsState & PREV_ENABLED));
		m_OkDoneNextCtrl.SetWindowText(nPage == m_aPages.GetSize() - 1 ? "Dokonèit" : "Další ->");
	}
	else
	{
		m_TabCtrl.SendMessage(TCM_SETCURSEL, nPage, 0);
	}

	return true;
}

//*****************************************************************************
void CWslPropertySheet::EnableOkDoneNextButton(BOOL bEnable)
{
	m_OkDoneNextCtrl.EnableWindow(bEnable);
}

//*****************************************************************************
void CWslPropertySheet::EnablePrevButton(BOOL bEnable)
{
	ASSERT(IsWizard());
	if (bEnable) m_nButtonsState |= PREV_ENABLED; else m_nButtonsState &= ~PREV_ENABLED;
	m_PrevCtrl.EnableWindow(bEnable);
}

//*****************************************************************************
void CWslPropertySheet::EnableCancelButton(BOOL bEnable)
{
	if (bEnable) m_nButtonsState |= CANCEL_ENABLED; else m_nButtonsState &= ~CANCEL_ENABLED;
	m_CancelCloseCtrl.EnableWindow(bEnable);
}

//*****************************************************************************
void CWslPropertySheet::SetOkDoneNextText(const char* pcszText)
{
	ASSERT(pcszText);
	m_OkDoneNextCtrl.SetWindowText(pcszText);
}

//*****************************************************************************
void CWslPropertySheet::OnOkNextDone()
{
	if (IsWizard() && m_nActivePage < m_aPages.GetSize()-1)
	{
		SetActivePage(m_nActivePage + 1);
	}
	else
	{
		// Call pages
		
		for (int i=m_aPages.GetSize(); i--;)
		{
			if (!m_aPages[i]->OnOkButton()) return;
		}
		
		// Call callback
		
		if (!m_pfnCallback || (*m_pfnCallback)(m_pCallbackData, *this, true)) 
		{
			m_bModal ? EndDialog(IDOK) : DestroyWindow();
		}
	}
}

//*****************************************************************************
void CWslPropertySheet::OnCancelButton()
{
	// Call pages

	for (int i=m_aPages.GetSize(); i--;)
	{
		if (!m_aPages[i]->OnCancelButton()) return;
	}
	
	// Call callback

	if (!m_pfnCallback || (*m_pfnCallback)(m_pCallbackData, *this, false)) 
	{
		m_bModal ? EndDialog(IDCANCEL) : DestroyWindow();
	}
}

//*****************************************************************************
void CWslPropertySheet::OnHelp()
{
	m_aPages[m_nActivePage]->OnHelpButton();	
}

//*****************************************************************************
void CWslPropertySheet::OnPrev()
{
	SetActivePage(m_nActivePage - 1);
}

//*****************************************************************************
void CWslPropertySheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if (nID == SC_CLOSE)
	{
		if (m_nButtonsState & CANCEL_ENABLED) OnCancelButton();
	}
	else CWslDialog::WndProc(WM_SYSCOMMAND, nID, lParam);
}

//*****************************************************************************
DWORD CWslPropertySheet::OnSelchangeTab(NMHDR* pNMHDR) 
{
	int n = m_nActivePage;
	int i = m_TabCtrl.SendMessage(TCM_GETCURSEL,0,0);
	if (!SetActivePage(i)) m_TabCtrl.SendMessage(TCM_SETCURSEL,n,0);
	return 0;
}

//*****************************************************************************
void CWslPropertySheet::OnCancel()
{
	OnCancelButton();
}

//*****************************************************************************
void CWslPropertySheet::SendMessageToAllPages(int nMsg, WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_hWnd); // must be created

	for (int i=0; i<m_aPages.GetSize(); i++)
	{
		if (m_aPages[i]->m_hWnd) m_aPages[i]->SendMessage(nMsg, wParam, lParam);
	}
}

/*****************************************************************************
*
* MESSAGE HANDLING
*
*****************************************************************************/

//*****************************************************************************
BOOL CWslPropertySheet::OnCommand(int nCmdId)
{
	switch (nCmdId)
	{
	case _IDC_CANCELCLOSE: OnCancelButton(); break;
	case _IDC_OKNEXTDONE: OnOkNextDone(); break;
	case _IDC_PREV:	OnPrev(); break;
	case _IDC_HELP: OnHelp(); break;
	default: return CWslDialog::OnCommand(nCmdId);
	}
	return true;
}

//*****************************************************************************
DWORD CWslPropertySheet::OnNotify(int nId, NMHDR* pData)
{
	switch (nId)
	{
	case _IDC_TAB:
		switch (pData->code)
		{
		case TCN_SELCHANGE: return OnSelchangeTab(pData);
		default: return CWslDialog::OnNotify(nId, pData);
		}
		break;
	
	default: 
		return CWslDialog::OnNotify(nId, pData);
	}
}

//*****************************************************************************
int CWslPropertySheet::WndProc(int nMsg, DWORD wParam, DWORD lParam)
{
	switch (nMsg)
	{
	case WM_SYSCOMMAND:
		OnSysCommand(wParam, lParam);
		break;
	
	default:
		return CWslDialog::WndProc(nMsg, wParam, lParam);
	}

	return true;
}





