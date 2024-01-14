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

#include "stdafx.h"
#include "main.h"
#include "WsfVSplitter.h"
#include "WsfHSplitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WSF_TABWND_TAB_ID					IDS_TT_OPENED_WINDOWS
#define WSF_TABWND_CLOSE_BUTTON_ID		IDS_TT_CLOSE_BUTTON

BEGIN_MESSAGE_MAP(CWsfTabWnd, CWnd)
	//{{AFX_MSG_MAP(CWsfTabWnd)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_WSF_NEXT_TAB_WINDOW, OnWsfNextTabWindow)
	ON_UPDATE_COMMAND_UI(ID_WSF_NEXT_TAB_WINDOW, OnUpdateWsfNextTabWindow)
	ON_COMMAND(ID_WSF_PREVIOUS_TAB_WINDOW, OnWsfPrevTabWindow)
	ON_UPDATE_COMMAND_UI(ID_WSF_PREVIOUS_TAB_WINDOW, OnUpdateWsfPrevTabWindow)
	ON_COMMAND(WSF_TABWND_CLOSE_BUTTON_ID, OnCloseButtonClicked)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfTabWnd::CWsfTabWnd(EStyles eStyle, BOOL bShowCloseButton)
{
	m_hFont = NULL;
	m_hHeaderFont = NULL;
	m_nActiveTabIdx = -1;
   m_eStyle = eStyle;
   m_bShowCloseButton = bShowCloseButton;
}

//*****************************************************************************
CWsfTabWnd::~CWsfTabWnd()
{
	if (m_hFont) DeleteObject(m_hFont);
	if (m_hHeaderFont) DeleteObject(m_hHeaderFont);
}

//*****************************************************************************
BOOL CWsfTabWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs)) return FALSE;

	cs.style &= ~WS_BORDER;
   cs.dwExStyle |= WS_EX_STATICEDGE;
	cs.lpszClass = AfxRegisterWndClass( CS_HREDRAW|CS_VREDRAW, 
		::LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_APPWORKSPACE+1), NULL);
   cs.lpszName = "CWsfTabWnd";

	return TRUE;
}

//*****************************************************************************
int CWsfTabWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
   if (CWnd::OnCreate(lpCreateStruct) == -1) return -1;
   
   // Enable tooltips for child windows

   EnableToolTips(true);

   // Create fonts
   
   m_hFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
   ASSERT(m_hFont);
   
   CFont f; 
   f.CreateFont(-17, 0, 0, 0, FW_NORMAL, false, false, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
      CLIP_DEFAULT_PRECIS, PROOF_QUALITY, TMPF_TRUETYPE, "Verdana");
   m_hHeaderFont = (HFONT) f.Detach();
 
   // Init tab-control
   
   VERIFY(m_TabCtrl.CWnd::CreateEx(0, WC_TABCONTROL, "",
      WS_CHILD|TCS_FORCELABELLEFT, 
      WsfZeroRect, this, WSF_TABWND_TAB_ID, NULL));
   m_TabCtrl.SendMessage(WM_SETFONT, (WPARAM) m_hFont, 0);
   m_TabCtrl.SetPadding(CSize(6,4));
   m_TabCtrl.SetExtendedStyle(TCS_EX_REGISTERDROP,TCS_EX_REGISTERDROP);

   if (m_eStyle==BUTTONS)
   {
      m_TabCtrl.ModifyStyle(0,TCS_FORCELABELLEFT|TCS_FIXEDWIDTH|TCS_BUTTONS|TCS_FLATBUTTONS|TCS_MULTILINE,0);
      WsfSetBoldFont(&m_TabCtrl);
      m_TabCtrl.SetImageList(WsfGetApp()->GetNormalIconList());
   }
   else
   {
      m_TabCtrl.SetImageList(WsfGetApp()->GetSmallIconList());
   }
   
   // Title controls

   m_TitleBackgndCtrl.Create("Title Background", SS_GRAYRECT, WsfZeroRect, this);
   m_TitleCtrl.Create("Title", SS_WORDELLIPSIS|SS_CENTERIMAGE|SS_NOPREFIX, WsfZeroRect, this);
   m_TitleCtrl.SendMessage(WM_SETFONT, (WPARAM) m_hHeaderFont, 0);
   
   // Close button
   
   if (m_bShowCloseButton)
   {
      m_CloseCtrl.Create(WS_VISIBLE|WS_CHILD, &WsfZeroRect, 
         IDB_CLOSE_CROSS, IDB_CLOSE_CROSS_DISABLED,
         this, WSF_TABWND_CLOSE_BUTTON_ID);
   }

   // Toolbar

	VERIFY(m_ToolBar.CreateControl(this, 0));
   m_ToolBar.ShowWindow(SW_HIDE);
   m_ToolBar.SetOwner(NULL);
   
   // Window Z-order

   m_TitleCtrl.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
   if (m_bShowCloseButton) m_CloseCtrl.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
   m_TitleBackgndCtrl.SetWindowPos(&wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
   m_TabCtrl.SetWindowPos(&wndBottom,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

   return 0;
}

//*****************************************************************************
void CWsfTabWnd::SendSelchangedNotify(CWnd *pPrev, CWnd *pNew)
{
	CSelChangedInfo s;

   s.nmhdr.code = SEL_CHANGED;
	s.nmhdr.hwndFrom = m_hWnd;
	s.nmhdr.idFrom = GetWindowLong(m_hWnd, GWL_ID);
	s.pNewWnd = pNew;

	WsfGetFrame()->SendMessage(WM_NOTIFY, s.nmhdr.idFrom, (LPARAM) &s);
}

/*****************************************************************************/
BOOL CWsfTabWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
   if (m_nActiveTabIdx != -1 && 
      m_aTabs[m_nActiveTabIdx]->pWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
   {
      return true;
   }
	
	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/*****************************************************************************
*
* TABS
*
*****************************************************************************/

//*****************************************************************************
int CWsfTabWnd::FindWnd(CWnd *pWnd)
{
	for (int i = m_aTabs.GetSize(); i--; ) if (m_aTabs[i]->pWnd == pWnd) return i;
	return -1;
}

//*****************************************************************************
void CWsfTabWnd::SetActiveWnd(CWnd *pWnd, BOOL bNotify)
{
	int i = FindWnd(pWnd);
	if (i != -1) SetActiveWnd(i, bNotify, true);
}

//*****************************************************************************
void CWsfTabWnd::SetActiveWnd(int nWnd, 
                              BOOL bNotify, 		
                              BOOL bUpdateSwitchOrder)
{
	CWnd *pPrevWnd, *pNewWnd, *p;
	ASSERT(nWnd != -1);

   // Already active? 

	if (m_nActiveTabIdx == nWnd) 
	{
      if (bNotify)
      {
		   p = m_aTabs[m_nActiveTabIdx]->pWnd;
		   SendSelchangedNotify(p, p);
      }
		return;
	}
	
	// Hide the current window

	if (m_nActiveTabIdx != -1) 
	{
		pPrevWnd = m_aTabs[m_nActiveTabIdx]->pWnd;
		ASSERT_VALID(pPrevWnd);
		pPrevWnd->ShowWindow(SW_HIDE);
	}
	else pPrevWnd = NULL;

	// Show new window

	pNewWnd = m_aTabs[nWnd]->pWnd;
	pNewWnd->ShowWindow(SW_SHOW);

	// Update tab ctrl

	m_TabCtrl.SetCurSel(nWnd);

	// Notify the parent frame of the change

	m_nActiveTabIdx = nWnd;		
	if (bNotify) SendSelchangedNotify(pPrevWnd, pNewWnd);

	// Update close button

	if (m_bShowCloseButton) m_CloseCtrl.EnableWindow(m_aTabs[nWnd]->bEnableCloseWindow);

	// Update switch order

	if (bUpdateSwitchOrder) MoveActiveWndToSwitchOrderTop();

   // Update toolbar

   IWsfTabWndDockable* pi = dynamic_cast<IWsfTabWndDockable*>(m_aTabs[nWnd]->pWnd);
   m_ToolBar.Empty();
   m_ToolBar.SetOwner(m_aTabs[nWnd]->pWnd);
   
   if (pi)
   {
      pi->OnInitToolbar(m_ToolBar);
   }

	// Redraw

	RecalcLayout();
	RedrawWindow();
}

//*****************************************************************************
void CWsfTabWnd::MoveActiveWndToSwitchOrderTop()
{
	if (m_aTabs.GetSize() < 2) return;

	int n = m_aSwitchOrder.Find(m_aTabs[m_nActiveTabIdx]->pWnd);
	ASSERT(n!=-1);
	m_aSwitchOrder.RemoveAt(n);
	m_aSwitchOrder.InsertAt(0, m_aTabs[m_nActiveTabIdx]->pWnd);
}

//*****************************************************************************
void CWsfTabWnd::ActivateNextWnd(int nSwitchOrderPos)
{
	if (m_aTabs.GetSize() < 2) return;

	nSwitchOrderPos %= m_aTabs.GetSize();
	ASSERT(nSwitchOrderPos >=0 && nSwitchOrderPos < m_aTabs.GetSize());

	int n = FindWnd(m_aSwitchOrder[nSwitchOrderPos]);
	ASSERT(n!=-1);
	SetActiveWnd(n, true, false);
}

//*****************************************************************************
CWnd* CWsfTabWnd::GetActiveWnd()
{
	return m_nActiveTabIdx != -1 ? m_aTabs[m_nActiveTabIdx]->pWnd : NULL;
}

//*****************************************************************************
void CWsfTabWnd::OpenTab(CWnd* pMasterWnd, 
                         const char *pcszTitle, 
                         int nPos /*= -1*/,
                         BOOL bActivate,
                         BOOL bEnableCloseWindow,
                         int nIconIdx)
{
	ASSERT_VALID(pMasterWnd);
	ASSERT(nPos >= -1);
	
	// Create new item
	
	if (nPos == -1 || nPos > m_aTabs.GetSize()) nPos = m_aTabs.GetSize();
	m_aTabs.InsertAt(nPos);

	m_aTabs[nPos] = new CTab();
	CTab& s = *m_aTabs[nPos]; 

	s.pWnd = pMasterWnd;
	s.sTitle = pcszTitle;
	s.bEnableCloseWindow = bEnableCloseWindow;
	s.nIconIdx = nIconIdx;

	// Add item to the tab ctrl

	m_TabCtrl.InsertItem(nPos, "", s.nIconIdx);

	// Shift active window index if the new window
	// was inserted before it

	if (nPos <= m_nActiveTabIdx) m_nActiveTabIdx++;

	// Add the window to the switch order

	m_aSwitchOrder += pMasterWnd;

	// Activate the window if there is only one window or if wanted

	if (bActivate || m_aTabs.GetSize() == 1) 
		SetActiveWnd(nPos, true, true);
	else
	{
   	pMasterWnd->ShowWindow(SW_HIDE);
		RecalcLayout();
      RedrawWindow();
	}
}

//*****************************************************************************
BOOL CWsfTabWnd::CloseWnd(CWnd* pWnd, CWnd* pNewActiveWnd)
{
	int i = FindWnd(pWnd);
	if (i == -1) return false;

   CloseTab(i, pNewActiveWnd);
	return true;
}

//*****************************************************************************
void CWsfTabWnd::CloseTab(int nTabIdx, CWnd* pNewActiveWnd)
{
	int nNewActiveTabIdx;

	// If the active tab is being removed, activate
	// another tab

	if (m_nActiveTabIdx == nTabIdx)
	{
		if (m_aTabs.GetSize() > 1) 
		{
			// New tab is either given or it's the next tab in
			// the switch order
			nNewActiveTabIdx = FindWnd(pNewActiveWnd ? pNewActiveWnd : m_aSwitchOrder[1]);
			ASSERT(nNewActiveTabIdx != -1);
			ASSERT(nNewActiveTabIdx != nTabIdx);
			SetActiveWnd(nNewActiveTabIdx, true, true);
		}
		else 
		{
			m_nActiveTabIdx = -1;

         // Turn-off the toolbar

         m_ToolBar.Empty();
         m_ToolBar.SetOwner(NULL);
      }
	}

	// Remove form the switch list

	int n = m_aSwitchOrder.Find(m_aTabs[nTabIdx]->pWnd);
	ASSERT(n!=-1);
	m_aSwitchOrder.RemoveAt(n);

	// Close master and slave
   // Destroy splitter

   m_aTabs[nTabIdx]->pWnd->ShowWindow(SW_HIDE);
   m_aTabs[nTabIdx]->pWnd->SetParent(WsfGetFrame());

	// Remove from the tab list 

	delete m_aTabs[nTabIdx];
	m_aTabs.RemoveAt(nTabIdx);
	m_TabCtrl.DeleteItem(nTabIdx);

	// Adjust the active tab index

	ASSERT(m_nActiveTabIdx != nTabIdx);
	if (m_nActiveTabIdx > nTabIdx) m_nActiveTabIdx--;


	// Redraw

	RecalcLayout();
	RedrawWindow();

	// Notify now, if no tab is active

	if (m_nActiveTabIdx==-1) SendSelchangedNotify(NULL, NULL);
}

//*****************************************************************************
void CWsfTabWnd::SetTabTitleIcon(CWnd* pWnd, 
                                 const char* pcszTitle,
                                 int nIconIdx /*= -1*/)
{
	int nWnd = FindWnd(pWnd);
	if (nWnd == -1) return;

	// Update Wnd info

	m_aTabs[nWnd]->sTitle = pcszTitle;
	if (nIconIdx != -1) m_aTabs[nWnd]->nIconIdx = nIconIdx;

   // Update layout 
	
	RecalcLayout();
   RedrawWindow();
}

//*****************************************************************************
BOOL CWsfTabWnd::CloseAllTabs()
{
	int i = m_aTabs.GetSize();

	while (i--)
	{
		// Close the Wnd.

		m_aTabs[i]->pWnd->SendMessage(WM_CLOSE, 0,0);
		if (IsWindow(m_aTabs[i]->pWnd->m_hWnd)) return false;

		// Remove it from the lists

		m_aTabs[i]->pWnd = NULL;
		CloseTab(i, NULL);
	}

	return true;
}

/*****************************************************************************/
BOOL CWsfTabWnd::SetTabWnd(CWnd* pCurrentWnd, 
                           CWnd* pNewWnd,
                           const char* pcszTitle,
                           int nIconIdx)
{
   ASSERT(pNewWnd);
   ASSERT(this->IsChild(pNewWnd));

   int n = FindWnd(pCurrentWnd);
	if (n==-1) return false;

   m_aTabs[n]->pWnd = pNewWnd;
   m_aTabs[n]->sTitle = pcszTitle;
   m_aTabs[n]->nIconIdx = nIconIdx;

   // Update the switch order

   for (int i=0; i<m_aSwitchOrder.GetSize(); i++) 
   {
      if (m_aSwitchOrder[i]==pCurrentWnd)
      {
         m_aSwitchOrder[i] = pNewWnd;
         break;
      }
   }

   // Update toolbar

   if (m_nActiveTabIdx==n) 
   {
      SetActiveWnd(n, true, false);
      IWsfTabWndDockable* pi = dynamic_cast<IWsfTabWndDockable*>(m_aTabs[n]->pWnd);
      m_ToolBar.Empty();
      
      if (pi)
      {
         pi->OnInitToolbar(m_ToolBar);
      }

      m_ToolBar.SetOwner(m_aTabs[n]->pWnd);
   }

   // Detach the old view

   pCurrentWnd->ShowWindow(SW_HIDE);
   pCurrentWnd->SetParent(WsfGetFrame());

   // Layout & show!

   pNewWnd->SetParent(this);
   
   RecalcLayout();
   RedrawWindow();

   pNewWnd->ShowWindow(SW_SHOW);

   return true;
}

/*****************************************************************************
*
* LAYOUT & DRAWING
*
*****************************************************************************/

//*****************************************************************************
CString CWsfTabWnd::GetClippedTitle(const char* pcszTitle, int nWidth)
{
	CString s;
	int l,n,i;
	SIZE size;

	ASSERT(pcszTitle);

	// If the title is a path, display only the last part

	const char* p = strrchr(pcszTitle, '\\');
	if (p) pcszTitle = p+1;

	// Clip1!

   CDC* pDC = GetDC();

	for (l=0,i=0,n=strlen(pcszTitle); i<n; i++)
	{
		VERIFY(GetTextExtentPoint32(*pDC, &pcszTitle[i],1,&size));
		l += size.cx;

		if (l > (nWidth-5)) 
		{
			s += "..."; 
			break;
		}
		else s += pcszTitle[i];
	}

   ReleaseDC(pDC);

	return s;
}

//*****************************************************************************

#define TOOLBAR_H          30
#define TITLE_H            26
#define TAB_H              28
#define TAB_TOP_MARGIN     2
#define HDR_BOTTOM_MARGIN  2
#define BUTTON_H           40

//*****************************************************************************
void CWsfTabWnd::RecalcLayout()
{
   if (m_aTabs.GetSize()==0)
   {
      m_TabCtrl.ShowWindow(SW_HIDE);
      m_TitleCtrl.ShowWindow(SW_HIDE);
      m_TitleBackgndCtrl.ShowWindow(SW_HIDE);      
      if (m_bShowCloseButton) m_CloseCtrl.ShowWindow(SW_HIDE);
      m_ToolBar.ShowWindow(SW_HIDE);
      return;
   }

	ASSERT(m_nActiveTabIdx != -1);

   BOOL bTabVisible = m_aTabs.GetSize() > 1;
   BOOL bTopTabVisible = m_eStyle==TABS && bTabVisible;
   BOOL bBottomTabVisible = m_eStyle==BUTTONS && bTabVisible;
   
   // Header height

   m_nHH = TITLE_H + HDR_BOTTOM_MARGIN;
   if (bTopTabVisible) m_nHH += TAB_H;
   if (!m_ToolBar.IsEmpty()) m_nHH += TOOLBAR_H;
   m_nBottomTabH = bBottomTabVisible ? m_aTabs.GetSize()*BUTTON_H + TAB_TOP_MARGIN : 0;
   
	// Window

	m_aTabs[m_nActiveTabIdx]->pWnd->SetWindowPos(NULL, 
      0, m_nHH, 
      m_nWW, m_nWH - m_nBottomTabH - m_nHH, 
      SWP_NOZORDER);
	
   // Tab-control

   if (bTabVisible)
   {
	   int nItemSize;

      if (m_eStyle==TABS)
      {
	      m_TabCtrl.SetWindowPos(NULL, 
            0, TAB_TOP_MARGIN, 
            m_nWW+4,                      // +2 to hide the right border
            TAB_H + 4,                    // +4 to hide the bottom border under the title
            SWP_NOZORDER | SWP_SHOWWINDOW);

         nItemSize = (m_nWW / m_aTabs.GetSize()) - 15;
   	   nItemSize = min(nItemSize, 300);
      }
      else
      {
	      m_TabCtrl.SetWindowPos(NULL, 
            -2, m_nWH-m_nBottomTabH+TAB_TOP_MARGIN,
            m_nWW+4, m_nBottomTabH,
            SWP_NOZORDER | SWP_SHOWWINDOW);
   
         nItemSize = m_nWW - 15;
         m_TabCtrl.SetItemSize(CSize(m_nWW+4, BUTTON_H-2));
      }
      
//	   if (nItemSize < 45) nItemSize = 25;

	   for (int i=0; i<m_aTabs.GetSize(); i++)
	   {
		   CString s;
		   TCITEM ti;
		   ti.mask = TCIF_TEXT | TCIF_IMAGE;
		   ti.iImage = m_aTabs[i]->nIconIdx;

		   if (nItemSize > 25 || m_aTabs[i]->nIconIdx==-1)
		   {
			   int n = m_aTabs[i]->nIconIdx==-1 ? nItemSize : nItemSize-20;
			   s = GetClippedTitle(m_aTabs[i]->sTitle, n);
			   ti.pszText = (char*) (const char*) s;
		   }
		   else ti.pszText = NULL;

		   VERIFY(m_TabCtrl.SetItem(i, &ti));
	   }
   }
   else
   {
      m_TabCtrl.ShowWindow(SW_HIDE);
   }

   // Title & close button

   {
   CRect r;
   r.top = bTopTabVisible ? TAB_H:0;
   r.left = 0;
   r.right =  m_nWW;
   r.bottom = r.top + TITLE_H;

   m_TitleBackgndCtrl.MoveWindow(&r, true);
   m_TitleBackgndCtrl.ShowWindow(SW_SHOW);

   r.DeflateRect(4,2,22,2);
   m_TitleCtrl.MoveWindow(&r, true);
   m_TitleCtrl.SetWindowText(m_aTabs[m_nActiveTabIdx]->sTitle);
   m_TitleCtrl.ShowWindow(SW_SHOW);

	if (m_bShowCloseButton)
   {
      m_CloseCtrl.SetWindowPos(NULL, 
         m_nWW-26, r.top + (r.Height()-19)/2, 
         20, 19, SWP_NOZORDER | SWP_SHOWWINDOW);
   }
   }

   // Toolbar

   if (!m_ToolBar.IsEmpty())
   {
      CRect r;
      r.top = (bTopTabVisible ? TAB_H:0) + TITLE_H;
      r.left = 0;
      r.right =  m_nWW;
      r.bottom = r.top + TOOLBAR_H;

      m_ToolBar.MoveWindow(&r);
      m_ToolBar.ShowWindow(SW_SHOW);
   }
   else
   {
      m_ToolBar.ShowWindow(SW_HIDE);
   }
}

//*****************************************************************************
void CWsfTabWnd::OnPaint() 
{
	CPaintDC dc(this); 
}

//*****************************************************************************
void CWsfTabWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	m_nWW = cx;
	m_nWH = cy;
	
	RecalcLayout();
}

//*****************************************************************************
BOOL CWsfTabWnd::OnEraseBkgnd(CDC* pDC) 
{
   // Erase only when there is no window

   if (m_nActiveTabIdx != -1)
   {
      CRect r;
      r.SetRect(0,0,m_nWW,m_nHH);
	   FillRect(*pDC, &r, (HBRUSH)(COLOR_3DLIGHT+1));

      if (m_nBottomTabH)
      {
         r.SetRect(0,m_nWH-m_nBottomTabH, m_nWW, m_nWH-m_nBottomTabH+TAB_TOP_MARGIN);
	      FillRect(*pDC, &r, (HBRUSH)(COLOR_3DLIGHT+1));
      }
      return false;
   }
   else
   {
      return CWnd::OnEraseBkgnd(pDC);   
   }
}

//*****************************************************************************
HBRUSH CWsfTabWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
   HBRUSH hbr;

   if (nCtlColor == CTLCOLOR_STATIC)
   {
      pDC->SetTextColor(RGB(255,255,255));
      pDC->SetBkColor(GetSysColor(COLOR_BTNSHADOW));
      hbr = GetSysColorBrush(COLOR_3DSHADOW);
   }
   else
   {
      hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
   }
	return hbr;
}

/*****************************************************************************
*
* MESSAGES
*
*****************************************************************************/

//*****************************************************************************
BOOL CWsfTabWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR* pNMHDR = (NMHDR*) lParam;

	switch (pNMHDR->code)
	{
	case TCN_SELCHANGE:
      SetActiveWnd(m_TabCtrl.GetCurSel(), true, true);
		return true;

   case NM_RCLICK:
      if (wParam == WSF_TABWND_TAB_ID)
      {
   		OnTabContextMenu();
	   	return true;
      }
   	break;

   case TCN_GETOBJECT:
      {
         NMOBJECTNOTIFY* p = (LPNMOBJECTNOTIFY) lParam;
         SetActiveWnd(p->iItem, true, true);
      }
      return 0;

   case TTN_SHOW:
/*      {
         POINT pt;
         VERIFY(GetCursorPos(&pt));
         ::SetWindowPos(p->hwndFrom, NULL, pt.x,pt.y, 0,0, SWP_NOSIZE|SWP_NOOWNERZORDER|SWP_NOACTIVATE);
         *pResult = true;
      }
      return true;*/
      break;

   case TTN_NEEDTEXT:
      {
         TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*) pNMHDR;

         int nID = pTTT->uFlags & TTF_IDISHWND ? 
            ::GetDlgCtrlID((HWND)pNMHDR->idFrom) :
            pNMHDR->idFrom;

	      if (nID == WSF_TABWND_TAB_ID)
	      {
		      /*TCHITTESTINFO hti;
		      GetCursorPos(&hti.pt);
		      m_TabCtrl.ScreenToClient(&hti.pt);
		      int i = m_TabCtrl.HitTest(&hti);
		      if (i != -1)
		      {
			      pTTT->lpszText  = (char*) (const char*) m_aTabs[i]->sTitle; 
			      pTTT->hinst = NULL;
			      return true;
		      }*/
	      }
	      else
	      {
		      CString s;
		      if (s.LoadString(nID))
		      {
			      strncpy(pTTT->szText, s, 79);
			      pTTT->szText[79] = 0;
			      return true;
		      }
		      else TRACE1("CWsfTabWnd::OnNotify(): Tooltip text for \"%d\" not found!\n", nID);
         }
      }
      return true;
	}

	return CWnd::OnNotify(wParam, lParam, pResult);
}

//*****************************************************************************
void CWsfTabWnd::OnTabContextMenu()
{
	// Get clicked tab

	TCHITTESTINFO s;
	GetCursorPos(&s.pt);
	m_TabCtrl.ScreenToClient(&s.pt);
	int nTabIdx = m_TabCtrl.HitTest(&s);
	if (nTabIdx == -1) return;

	// Get menu

	HMENU hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_MENUS));
	ASSERT(hMenu);
	HMENU hSubMenu = GetSubMenu(hMenu, 1);
	ASSERT(hSubMenu);
	//SetMenuDefaultItem(hSubMenu, ID_WSF_WINDOW_CLOSE, false);
  
   EnableMenuItem(hSubMenu, ID_WSF_WINDOW_CLOSE, 
      MF_BYCOMMAND | (m_aTabs[nTabIdx]->bEnableCloseWindow ? MF_ENABLED:MF_DISABLED|MF_GRAYED));

	// Display context menu

	m_TabCtrl.ClientToScreen(&s.pt);	
	int nCmdId = TrackPopupMenu(hSubMenu, 
		TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD|TPM_NONOTIFY,
		s.pt.x, s.pt.y, 0, *WsfGetFrame(), NULL);

	// Handle command

	switch (nCmdId)
	{
	case ID_WSF_WINDOW_CLOSE:
      ASSERT(m_aTabs[nTabIdx]->bEnableCloseWindow);
		WsfGetApp()->CloseView((CView*)m_aTabs[nTabIdx]->pWnd, false);
		break;
	}
}


//*****************************************************************************
void CWsfTabWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_nActiveTabIdx == -1) WsfGetFrame()->ChooseToolBars();
}

/*****************************************************************************
*
* COMMANDS
*
*****************************************************************************/

//*****************************************************************************
void CWsfTabWnd::OnCloseButtonClicked()
{
	NMHDR s;

   s.code = CLOSE_BUTTON_CLICKED;
	s.hwndFrom = m_hWnd;
	s.idFrom = GetWindowLong(m_hWnd, GWL_ID);

	WsfGetFrame()->SendMessage(WM_NOTIFY, s.idFrom, (LPARAM) &s);
}

//*****************************************************************************
void CWsfTabWnd::OnWsfNextTabWindow()
{
	int n = m_nActiveTabIdx;
	if (++n >= m_aTabs.GetSize()) n = 0;
	SetActiveWnd(n, true, true);
}

//*****************************************************************************
void CWsfTabWnd::OnUpdateWsfNextTabWindow(CCmdUI* p)
{
	p->Enable(m_aTabs.GetSize() > 1);
}

//*****************************************************************************
void CWsfTabWnd::OnWsfPrevTabWindow()
{
	int n = m_nActiveTabIdx;
	if (--n < 0) n = m_aTabs.GetSize()-1;
	SetActiveWnd(n, true, true);
}

//*****************************************************************************
void CWsfTabWnd::OnUpdateWsfPrevTabWindow(CCmdUI* p)
{
	p->Enable(m_aTabs.GetSize() > 1);
}

   