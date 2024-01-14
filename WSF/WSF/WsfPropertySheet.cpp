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

#include "stdafx.h"
#include "main.h"
#include "WsfPropertyPage.h"
#include "WsfPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Controls
#define _IDC_TAB					1
#define _IDC_OKNEXTDONE		   IDOK
#define _IDC_CANCEL				IDCANCEL
#define _IDC_PREV					4
#define _IDC_HELP					5

// Button states
#define PREV_ENABLED			1
#define CANCEL_ENABLED		2

#define IsShiftDown()	(GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1)))
#define IsCtrlDown()	(GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1)))

CWsfPropertySheet* CWsfPropertySheet::m_pps = NULL;

// Dimensions

#define BUTTON_H              22
#define BUTTON_W              90
#define BUTTON_DISTANCE       5

#define LEFT_MARGIN   5
#define RIGHT_MARGIN  5
#define TOP_MARGIN    5
#define BOTTOM_MARGIN 5


BEGIN_MESSAGE_MAP(CWsfPropertySheet, CDialog)
	//{{AFX_MSG_MAP(CWsfPropertySheet)
	ON_WM_SYSCOMMAND()
	ON_NOTIFY(TCN_SELCHANGE, _IDC_TAB, OnSelchangeTab)
	ON_WM_DESTROY()
	ON_COMMAND(_IDC_CANCEL, OnCancelButton)
	ON_COMMAND(_IDC_OKNEXTDONE, OnOkNextDone)
	ON_COMMAND(_IDC_PREV, OnPrev)
	ON_COMMAND(_IDC_HELP, OnHelp)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfPropertySheet::CWsfPropertySheet(BOOL bWizard, 
                                     WSF_PS_CALLBACK* pfnCallback/*= NULL*/, 
                                     void* pData/*= NULL*/) 
	: CDialog(IDD_PROPERTY_SHEET, NULL)
{
	//{{AFX_DATA_INIT(CWsfPropertySheet)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

   m_eMode = bWizard ? WIZARD : NORMAL;
   m_eState = INIT;
   
	m_pfnCallback = pfnCallback;
	m_nActivePage = -1;
	m_pCallbackData = pData;
	m_hFont = NULL;
//	m_hHelpBitmap = NULL;
	m_nButtonsState = PREV_ENABLED|CANCEL_ENABLED;
	m_hHook = NULL;
}

//*****************************************************************************
CWsfPropertySheet::~CWsfPropertySheet()
{
	RemoveAllPages();
	if (m_hFont) DeleteObject(m_hFont);
//	if (m_hHelpBitmap) DeleteObject(m_hHelpBitmap);
}

/*****************************************************************************/
BOOL CWsfPropertySheet::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CDialog::PreCreateWindow(cs)) return false;

//   cs.dwExStyle |= WS_EX_COMPOSITED;	

   return true;
}

/*****************************************************************************/
LRESULT CALLBACK CWsfPropertySheet::GetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_pps);
	if (code < 0) return CallNextHookEx(m_pps->m_hHook, code, wParam, lParam);

	MSG* m = (MSG*) lParam;
	
	if (m->message == WM_KEYDOWN && ::IsChild(*m_pps, m->hwnd))
	{
		switch (m->wParam)
		{
		case VK_PRIOR:
			// Selects the previous page
			if (!m_pps->IsWizard() && IsCtrlDown())
			{
				int i = m_pps->m_nActivePage>0 ? m_pps->m_nActivePage-1 : m_pps->m_aPages.GetSize()-1;
				m_pps->SetActivePage(i);
				m->message = 0;			// dirty?
			}
			break;

		case VK_NEXT:
			// Selects the next page
			if (!m_pps->IsWizard() && IsCtrlDown())
			{
				int i = m_pps->m_nActivePage==m_pps->m_aPages.GetSize()-1 ? 0 : m_pps->m_nActivePage+1;
				m_pps->SetActivePage(i);
				m->message = 0;			// dirty?
			}
			break;

		case VK_RETURN:
			// NOTE: This should be handled by IsDialogMessage() by default,
			// but it doesn't work somehow in multi-line edit controls.
			// NOTE: Ctrl+Enter is a line break in multi-line edit control!
         if (!IsCtrlDown())
			{
				m_pps->PostMessage(WM_COMMAND, IDOK, 0);
				m->message = 0;			// dirty?
			}
			break;

		case VK_ESCAPE:
			// NOTE: This should be handled by IsDialogMessage() by default,
			// but it doesn't work somehow in multi-line edit controls.
			if (m_pps->m_nButtonsState & CANCEL_ENABLED) 
			{
				m_pps->PostMessage(WM_COMMAND, IDCANCEL, 0);
				m->message = 0;			// dirty?
			}
			break;
		}
	}

	return CallNextHookEx(m_pps->m_hHook, code, wParam, lParam);
}

/*****************************************************************************/
void CWsfPropertySheet::CreateAsControl(CWnd* pParent, int nPositionTemplateId)
{
   ASSERT(m_eState==INIT);

   m_eMode = CONTROL;
	m_nButtonsState = 0;

   ASSERT(pParent);
   CWnd* p = pParent->GetDlgItem(nPositionTemplateId);
   ASSERT(p);

   CRect r;
   p->GetWindowRect(&r);
   pParent->ScreenToClient(&r);

   VERIFY(CDialog::Create(IDD_PROPERTY_SHEET_EMBEDDED, pParent));
   ASSERT(GetStyle() & WS_CHILD);
   ASSERT(GetExStyle() & WS_EX_CONTROLPARENT);
   
   MoveWindow(r);
   SetWindowLong(*this, GWL_ID, nPositionTemplateId);

   // Put the PS at the same position in the children order
   // as the original window

   VERIFY(SetWindowPos(p, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE));
   p->DestroyWindow();
}

/*****************************************************************************/
void CWsfPropertySheet::OnDestroy() 
{
	if (!IsControl()) 
   {
      m_pps = NULL;
	   if (m_hHook) UnhookWindowsHookEx(m_hHook);
   }

   CDialog::OnDestroy();
   m_eState = DESTROYED;
}

//*****************************************************************************
BOOL CWsfPropertySheet::OnInitDialog() 
{
   ASSERT(m_eState==INIT);
	ASSERT(GetExStyle() & WS_EX_CONTROLPARENT);

	// Set hook for special hot-keys handling
	// NOTE: For hook processing -> there can be only one property sheet
	// at the same time.

   if (!IsControl())
   {
	   ASSERT(!m_pps);
	   m_pps = this;

	   m_hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, NULL, GetCurrentThreadId());
	   ASSERT(m_hHook);
   }

	// Init

	ASSERT(m_aPages.GetSize() > 0);
	CDialog::OnInitDialog();

	m_hFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	ASSERT(m_hFont);

   if (!IsControl())
   {
//	   m_hHelpBitmap = WsfLoadBitmap(IDB_HELP, true);
//	   ASSERT(m_hHelpBitmap);
   }

	// Create pages and get max page size

   int nW = 0, nH = 0;
   CRect r;

	for (int i=m_aPages.GetSize(); i--;)
	{
		if (!CreatePage(m_aPages[i]))
      {
         EndDialog(IDCANCEL);
         return true;
      }

		m_aPages[i]->GetClientRect(&r);
		if (r.right - r.left > nW) nW = r.right - r.left;
		if (r.bottom - r.top > nH) nH = r.bottom - r.top;
	}

	// Init by mode

	if (IsWizard())
	{
		m_LineCtrl.Create(NULL, WS_CHILD|WS_VISIBLE|SS_ETCHEDHORZ, WsfZeroRect, this);

		//m_HelpCtrl.Create("?", WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_CENTER|BS_BITMAP, CRect(10,y,10+25,y+22), this, _IDC_HELP);
		//m_HelpCtrl.SetBitmap(m_hHelpBitmap);
		//m_HelpCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));

   	CString s;
		s.LoadString(IDS_BACK);
		m_PrevCtrl.Create(s, WS_CHILD|WS_VISIBLE|WS_TABSTOP, WsfZeroRect, this, _IDC_PREV);
		m_PrevCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));

		m_OkDoneNextCtrl.Create("", WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON, WsfZeroRect, this, _IDC_OKNEXTDONE);
		m_OkDoneNextCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));

      s.LoadString(IDS_CANCEL);
		m_CancelCtrl.Create(s, WS_CHILD|WS_VISIBLE|WS_TABSTOP,WsfZeroRect, this, _IDC_CANCEL);
		m_CancelCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));
	}
	else
	{
		// Create tab ctrl

		m_TabCtrl.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP/*|TCS_FIXEDWIDTH*/, CRect(0,0,0,0), this, _IDC_TAB);
		m_TabCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));
      WsfSetBoldFont(&m_TabCtrl);
      //m_TabCtrl.SetItemSize(CSize(140,22));

		for (int i=0; i<m_aPages.GetSize(); i++) AddPageToTabCtrl(m_aPages[i]);

      if (!IsControl())
      {
		   //m_HelpCtrl.Create("?", WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_CENTER|BS_BITMAP, CRect(5,y,5+25,y+22), this, _IDC_HELP);
		   //m_HelpCtrl.SetBitmap(m_hHelpBitmap);
		   //m_HelpCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));

         m_OkDoneNextCtrl.Create("OK", WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_DEFPUSHBUTTON, WsfZeroRect, this, _IDC_OKNEXTDONE);
		   m_OkDoneNextCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));

      	CString s;
         s.LoadString(IDS_CANCEL);
		   m_CancelCtrl.Create(s, WS_CHILD|WS_VISIBLE|WS_TABSTOP, WsfZeroRect, this, _IDC_CANCEL);
		   m_CancelCtrl.SendMessage(WM_SETFONT, (WPARAM)m_hFont, MAKELPARAM(false, 0));

		   // Title
   		SetWindowText(m_sTitle);
      }

      // Move the TAB on the bottom in order to get this order:
      // PG1 ... PGn OK CANCEL TAB

      ::SetWindowPos(m_TabCtrl, HWND_BOTTOM,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	}

	// Init

	SetActivePage(0);
   if (!IsControl()) SetDefID(_IDC_OKNEXTDONE);
   m_eState = RUNNING;

   // Layout

   GetSheetSizeFromPageSize(nW, nH);
   r.SetRect(0,0,nW,nH);
   AdjustWindowRectEx(&r, GetStyle(), false, GetExStyle());
   SetWindowPos(NULL,0,0,r.Width(),r.Height(),SWP_NOMOVE|SWP_NOZORDER);

   if (!IsControl()) CenterWindow();

	return false;
}

//*****************************************************************************
void CWsfPropertySheet::OnSize(UINT nType, int cx, int cy) 
{
   RecalcLayout(cx, cy);
	CDialog::OnSize(nType, cx, cy);
   RedrawWindow();
}

//*****************************************************************************
BOOL CWsfPropertySheet::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
   if (IsControl() && m_nActivePage != -1)
   {
      if (m_aPages[m_nActivePage]->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return true;
   }

	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/*****************************************************************************/
void CWsfPropertySheet::SetWizardMode(BOOL bWizard)
{
   ASSERT(m_eState==INIT);
   m_eMode = bWizard ? WIZARD : NORMAL;
}

//*****************************************************************************
void CWsfPropertySheet::SetTitle(const char *pcszTitle)
{
	ASSERT(pcszTitle);
	m_sTitle = pcszTitle;
}

/*****************************************************************************/
void CWsfPropertySheet::SetTitle(int nID)
{
	CString s;
	VERIFY(s.LoadString(nID));
	SetTitle(s);
}

/*****************************************************************************
*
* LAYOUT ENGINE
*
*****************************************************************************/

/*****************************************************************************/
void CWsfPropertySheet::RecalcLayout(int nW, int nH)
{
   if (m_eState != RUNNING) return;

   switch (m_eMode)
   {
   case CONTROL:
      {
      // Size the tab

      {
      CRect r(0,0,nW+3,29);
      m_TabCtrl.MoveWindow(&r, true);
      }

      // Get tab's display area

      CRect r(0,0,nW,nH);
	   m_TabCtrl.AdjustRect(false, &r);
      r.top += 3;
      r.left = 0;
      r.right = nW;
      r.bottom = nH;
   
      // Size the pages

	   for (int i=0; i<m_aPages.GetSize(); i++) 
      {
		   m_aPages[i]->MoveWindow(&r, false);
      }
      }
      break;
   
   case NORMAL:
      {
      CRect r;
      r.left = LEFT_MARGIN;
      r.right = nW - RIGHT_MARGIN;
      r.top = TOP_MARGIN;
      r.bottom = nH - BOTTOM_MARGIN - BUTTON_H - BOTTOM_MARGIN;

      if (m_aPages.GetSize() > 1)
      {
         m_TabCtrl.ShowWindow(SW_SHOW);
         m_TabCtrl.MoveWindow(&r);
		   m_TabCtrl.AdjustRect(false, &r);
      }
      else
      {
         m_TabCtrl.ShowWindow(SW_HIDE);
      }

      // Position pages

		for (int i=m_aPages.GetSize(); i--;) 
      {
		   m_aPages[i]->MoveWindow(&r, false);
      }

      // Position controls

      int y = nH - BOTTOM_MARGIN - BUTTON_H;
      int x = nW - RIGHT_MARGIN - BUTTON_W;
      m_CancelCtrl.MoveWindow(x,y,BUTTON_W,BUTTON_H,false);

      x = x - BUTTON_DISTANCE - BUTTON_W;
      m_OkDoneNextCtrl.MoveWindow(x,y,BUTTON_W,BUTTON_H,false);
      }
      break;
   
   case WIZARD:
      {
         // Position controls

         int y = nH - BOTTOM_MARGIN - BUTTON_H;
         int x = nW - RIGHT_MARGIN - BUTTON_W;
         m_CancelCtrl.MoveWindow(x,y,BUTTON_W,BUTTON_H,false);

         x = x - 4*BUTTON_DISTANCE - BUTTON_W;
         m_OkDoneNextCtrl.MoveWindow(x,y,BUTTON_W,BUTTON_H,false);

         x = x - BUTTON_DISTANCE - BUTTON_W;
         m_PrevCtrl.MoveWindow(x,y,BUTTON_W,BUTTON_H,false);

         y = nH - BOTTOM_MARGIN - BUTTON_H - 2*BOTTOM_MARGIN;
         m_LineCtrl.MoveWindow(0,y,nW,2,false);

         // Size and position the pages

         CRect r;
         r.top = 0;
         r.left = 0;
         r.right = nW;
         r.bottom = nH - BOTTOM_MARGIN - BUTTON_H - 2*BOTTOM_MARGIN;

	      for (int i=0; i<m_aPages.GetSize(); i++) 
         {
		      m_aPages[i]->MoveWindow(&r, false);
         }
      }
      break;
   }

}

/*****************************************************************************/
void CWsfPropertySheet::GetSheetSizeFromPageSize(int& nW, int& nH)
{
   ASSERT(m_eState==RUNNING);

   switch (m_eMode)
   {
   case NORMAL:
      nW += LEFT_MARGIN + RIGHT_MARGIN;
      nH += TOP_MARGIN + BOTTOM_MARGIN + BUTTON_H + BOTTOM_MARGIN;

      if (GetPageCount() > 1)
      {
         CRect r;
         r.SetRectEmpty();
         m_TabCtrl.AdjustRect(true, &r);
         nH += r.Height();
         nW += r.Width();
      }
      break;

   case WIZARD:
      nW += LEFT_MARGIN + RIGHT_MARGIN;
      nH += TOP_MARGIN + BOTTOM_MARGIN + BUTTON_H + BOTTOM_MARGIN + BOTTOM_MARGIN;
      break;

   case CONTROL:
      {
         CRect r;
         m_TabCtrl.AdjustRect(false, &r);
         nH += r.Height();
         nW += r.Width();
      }
      break;
   }
}

/*****************************************************************************
*
* PAGES
*
*****************************************************************************/

//*****************************************************************************
BOOL CWsfPropertySheet::AddPage(CWsfPropertyPage *ppg)
{
   ASSERT(m_eState != DESTROYED);
	ASSERT(ppg);
	ASSERT(!m_hWnd || !IsWizard()); // You cannot add pages when the wizard is displayed!

	// If PS is already displayed, update it

	if (m_hWnd)
	{
		if (!CreatePage(ppg)) return false;
		::SetWindowPos(*ppg,HWND_TOP,m_nPageX,m_nPageY,0,0,SWP_NOSIZE);
		AddPageToTabCtrl(ppg);
	}

	// Add it to the array

	m_aPages += ppg;

   return true;
}

//*****************************************************************************
void CWsfPropertySheet::RemovePage(CWsfPropertyPage* ppg)
{
	ASSERT(ppg);
	ASSERT(m_aPages.GetSize() > 1);		// there must remain at least one page 
	ASSERT(m_hWnd);					// PS must be displayed
	ASSERT(!IsWizard());			// You cannot add pages when the wizard is displayed!

	// Find page
   
   int i;
	for (i=m_aPages.GetSize(); i--;) if (m_aPages[i] == ppg) break;
	ASSERT(i!=-1);

	// Update active page

	if (m_nActivePage==i) SetActivePage(i>0 ? i-1 : i+1);
	if (m_nActivePage > i) m_nActivePage--;

	// Delete page

	m_TabCtrl.DeleteItem(i);
	ppg->DestroyWindow();
	delete m_aPages[i];
	m_aPages.RemoveAt(i);
}

//*****************************************************************************
BOOL CWsfPropertySheet::CreatePage(CWsfPropertyPage* ppg)
{
	if (!ppg->Create(this)) return false;

   ppg->ShowWindow(SW_HIDE);
	ppg->EnableWindow(false);

   return true;
}

//*****************************************************************************
void CWsfPropertySheet::AddPageToTabCtrl(CWsfPropertyPage* ppg)
{
	ppg->GetWindowText(temp, 1024);
	m_TabCtrl.InsertItem(m_TabCtrl.GetItemCount(), temp, -1);
}

//*****************************************************************************
void CWsfPropertySheet::RemoveAllPages()
{
	for (int i=m_aPages.GetSize(); i--;) delete m_aPages[i];
	m_aPages.RemoveAll();
}	

//*****************************************************************************
CWsfPropertyPage* CWsfPropertySheet::GetPage(int nPage)
{
	ASSERT(nPage >=0 && nPage < m_aPages.GetSize());
	return m_aPages[nPage];
}

//*****************************************************************************
CWsfPropertyPage* CWsfPropertySheet::GetPage(const CRuntimeClass* pClass)
{
	for (int i=0; i<m_aPages.GetSize(); i++) 
   {
      if (m_aPages[i]->IsKindOf(pClass)) return m_aPages[i];
   }
   return NULL;
}
//*****************************************************************************
void CWsfPropertySheet::SetPageTitle(CWsfPropertyPage* ppg, const char* pcszTitle)
{
	ASSERT(ppg);
	ASSERT(pcszTitle);
	ASSERT(m_hWnd);

   int i;
	for (i=m_aPages.GetSize(); i--;) if (m_aPages[i] == ppg) break;
	ASSERT(i!=-1);
	ppg->SetWindowText(pcszTitle);

	if (IsWizard())
	{
		if (m_nActivePage == i)
		{
			wsprintf(temp, "%s - %s", m_sTitle, pcszTitle);
			SetWindowText(temp);
		}
	}
	else
	{
		if (m_TabCtrl.m_hWnd)
		{
			TCITEM tci;
			tci.mask = TCIF_TEXT;
			tci.pszText = (char*) pcszTitle;
			m_TabCtrl.SetItem(i, &tci);
		}
	}
}

//*****************************************************************************
BOOL CWsfPropertySheet::SetActivePage(int nPage)
{
	CWsfPropertyPage *ppg;

	ASSERT(nPage >=0 && nPage < m_aPages.GetSize());
	if (m_nActivePage == nPage) return true;

	// Deactivate the active page

	if (m_nActivePage != -1)
	{
      // Call OnKillActive()
      // Wizard: call only when going forward

      if (!IsWizard() || (IsWizard() && m_nActivePage < nPage))
      {
		   if (!m_aPages[m_nActivePage]->OnKillActive(nPage)) return false;
		   ASSERT(nPage>=0 && nPage<m_aPages.GetSize());
      }
		
		m_aPages[m_nActivePage]->ShowWindow(SW_HIDE);
		m_aPages[m_nActivePage]->EnableWindow(false);
	}

	// Active the new one

	m_nActivePage = nPage;
	ppg = m_aPages[nPage];
	ppg->ShowWindow(SW_SHOW);
	ppg->EnableWindow();
	ppg->SetFocus();
	ppg->OnSetActive();

	// Update PS

	if (IsWizard())
	{
      CString s = m_sTitle;

      if (m_aPages.GetSize() > 1)
      {
         CString s2;
		   ppg->GetWindowText(s2);

         if (!s2.IsEmpty())
         {
            s += " - ";
            s += s2;
         }
      }
      
      SetWindowText(s);

		m_PrevCtrl.EnableWindow(nPage > 0 && (m_nButtonsState & PREV_ENABLED));
		VERIFY(s.LoadString(nPage == m_aPages.GetSize() - 1 ? IDS_FINISH:IDS_NEXT));
		m_OkDoneNextCtrl.SetWindowText(s);
	}
	else
	{
		m_TabCtrl.SetCurSel(nPage);
	}

   // Update the navigation history

   m_aNavHistory += nPage;

	return true;
}

/*****************************************************************************
*
* BUTTONS
*
*****************************************************************************/

//*****************************************************************************
void CWsfPropertySheet::EnableOkDoneNextButton(BOOL bEnable)
{
	m_OkDoneNextCtrl.EnableWindow(bEnable);
}

//*****************************************************************************
void CWsfPropertySheet::EnablePrevButton(BOOL bEnable)
{
	ASSERT(IsWizard());
	if (bEnable) m_nButtonsState |= PREV_ENABLED; else m_nButtonsState &= ~PREV_ENABLED;
	m_PrevCtrl.EnableWindow(bEnable);
}

//*****************************************************************************
void CWsfPropertySheet::EnableCancelButton(BOOL bEnable)
{
	if (bEnable) m_nButtonsState |= CANCEL_ENABLED; else m_nButtonsState &= ~CANCEL_ENABLED;
	m_CancelCtrl.EnableWindow(bEnable);
}

//*****************************************************************************
void CWsfPropertySheet::SetOkDoneNextText(const char* pcszText)
{
	ASSERT(pcszText);
	m_OkDoneNextCtrl.SetWindowText(pcszText);
}

/*****************************************************************************
*
* ACTIONS
*
*****************************************************************************/

/*****************************************************************************/
BOOL CWsfPropertySheet::Apply()
{
   ASSERT(!IsWizard());
	
	// Deactive the current page to save the data

	int nPage = -1;
	if (!m_aPages[m_nActivePage]->OnKillActive(nPage)) return false;

	// Call OK handlers
	
	for (int i=0; i<m_aPages.GetSize(); i++)
	{
		if (!m_aPages[i]->OnOkButton()) return false;
	}
	
	// Call callback
	
	if (m_pfnCallback && !(*m_pfnCallback)(m_pCallbackData, *this, true))  return false;

   return true;
}

/*****************************************************************************
*
* COMMANDS
*
*****************************************************************************/

//*****************************************************************************
void CWsfPropertySheet::OnOkNextDone()
{
	if (IsWizard() && m_nActivePage < m_aPages.GetSize()-1)
	{
		SetActivePage(m_nActivePage + 1);
	}
	else
	{
		// Deactive the current page

		int nPage = -1;
		if (!m_aPages[m_nActivePage]->OnKillActive(nPage)) return;

		// Call OK handlers in pages
		
		for (int i=0; i<m_aPages.GetSize(); i++)
		{
			if (!m_aPages[i]->OnOkButton()) return;
		}
		
		// Call callback
		
		if (!m_pfnCallback || (*m_pfnCallback)(m_pCallbackData, *this, true)) 
			EndDialog(IDOK);
	}
}

//*****************************************************************************
void CWsfPropertySheet::OnCancelButton()
{
	// Call pages

	for (int i=m_aPages.GetSize(); i--;)
	{
		if (!m_aPages[i]->OnCancelButton()) return;
	}
	
	// Call callback

	if (!m_pfnCallback || (*m_pfnCallback)(m_pCallbackData, *this, false)) 
		EndDialog(IDCANCEL);
}

//*****************************************************************************
void CWsfPropertySheet::OnHelp()
{
	m_aPages[m_nActivePage]->OnHelpButton();	
}

//*****************************************************************************
void CWsfPropertySheet::OnPrev()
{
   // Get the previous page from the navigation history

   ASSERT(m_aNavHistory.GetSize() >= 2);
   int nPage = m_aNavHistory[m_aNavHistory.GetSize()-2];
   m_aNavHistory.ShrinkBy(2);
   
   // Navigate!

	SetActivePage(nPage);
}

//*****************************************************************************
void CWsfPropertySheet::OnSysCommand(UINT nID, LPARAM lParam) 
{
	if (nID == SC_CLOSE)
	{
		if (m_nButtonsState & CANCEL_ENABLED) OnCancelButton();
	}
	else CDialog::OnSysCommand(nID, lParam);
}

//*****************************************************************************
void CWsfPropertySheet::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int n = m_nActivePage;
	if (!SetActivePage(m_TabCtrl.GetCurSel())) m_TabCtrl.SetCurSel(n);
}

//*****************************************************************************
void CWsfPropertySheet::OnCancel()
{
	OnCancelButton();
}

