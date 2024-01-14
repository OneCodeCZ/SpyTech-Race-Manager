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

#include "stdafx.h"
#include "main.h"
#include "WsfFlatButton.h"
#include "WsfVSplitter.h"
#include "WsfHSplitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WSF_TABWND_TAB_ID							IDS_TT_OPENED_WINDOWS
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
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipNotify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfTabWnd::CWsfTabWnd(int nFlags)
{
	m_hFont = NULL;
	m_hHeaderFont = NULL;
	m_pTabCtrl = NULL;
	m_pCloseCtrl = NULL;
	m_nActiveWnd = -1;
	m_nFlags = nFlags;
}

//*****************************************************************************
CWsfTabWnd::~CWsfTabWnd()
{
	if (m_hFont) DeleteObject(m_hFont);
	if (m_hHeaderFont) DeleteObject(m_hHeaderFont);
	delete m_pTabCtrl;
	delete m_pCloseCtrl;
}

//*****************************************************************************
BOOL CWsfTabWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs)) return FALSE;

	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_OWNDC, 
		::LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_APPWORKSPACE+1), NULL);

	return TRUE;
}

//*****************************************************************************
int CWsfTabWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1) return -1;

	// Init

	m_pFrameWnd = GetParentFrame();
	ASSERT(m_pFrameWnd);
	EnableToolTips();

	// Create the close button

	m_pCloseCtrl = new CWsfFlatButton();
	m_pCloseCtrl->Create(WS_VISIBLE|WS_CHILD, &WsfZeroRect, 
		IDB_CLOSE_CROSS, IDB_CLOSE_CROSS_DISABLED,
		this, WSF_TABWND_CLOSE_BUTTON_ID);

	// Create font

	m_hFont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	ASSERT(m_hFont);

  LOGFONT lf;
  CFont f; 
	f.Attach(m_hFont);
  f.GetLogFont(&lf);
  lf.lfWeight = 1000;		
  CFont bf;
	bf.CreateFontIndirect(&lf);
	m_hHeaderFont = (HFONT) bf.Detach();

	// Init DC

	m_hDC = ::GetDC(m_hWnd);
	SelectObject(m_hDC, m_hHeaderFont);
	SetBkMode(m_hDC, TRANSPARENT);

	TEXTMETRIC tm;
	GetTextMetrics(m_hDC, &tm);
	m_nFH = tm.tmHeight;

	// Init tab-control

	m_pTabCtrl = new CTabCtrl();
	m_pTabCtrl->CreateEx(0,
		WC_TABCONTROL, NULL, 
		WS_CHILD|TCS_BOTTOM|TCS_FLATBUTTONS|TCS_BUTTONS|TCS_FORCELABELLEFT|
		TCS_FIXEDWIDTH, 
		WsfZeroRect, this, WSF_TABWND_TAB_ID, NULL);
	m_pTabCtrl->SendMessage(WM_SETFONT, (WPARAM) m_hFont, 0);
	m_pTabCtrl->SetImageList(WsfGetApp()->GetSmallIconList());
	m_pTabCtrl->SetPadding(CSize(3,3));
	m_nTH = 22;

	// Init

	m_nHH = m_nFH + 12;

	return 0;
}

//*****************************************************************************
void CWsfTabWnd::SendSelchangedNotify(CWnd *pPrev, CWnd *pNew)
{
	WSF_TABWND_SELCHANGED_INFO s;

	s.nmhdr.code = WSF_TABWND_NM_SELCHANGED;
	s.nmhdr.hwndFrom = m_hWnd;
	s.nmhdr.idFrom = GetWindowLong(m_hWnd, GWL_ID);
	s.pNewWnd = pNew;
	//s.pPrevWnd= pPrev;

	m_pFrameWnd->SendMessage(WM_NOTIFY, s.nmhdr.idFrom, (LPARAM) &s);
}

//*****************************************************************************
int CWsfTabWnd::FindWnd(CWnd *pWnd)
{
	for (int i = m_aWnds.GetSize(); i--; ) if (m_aWnds[i]->pWnd == pWnd) return i;
	return -1;
}

//*****************************************************************************
int CWsfTabWnd::FindSlaveWnd(CWnd *pWnd)
{
	for (int i = m_aWnds.GetSize(); i--; ) if (m_aWnds[i]->pSlaveWnd == pWnd) return i;
	return -1;
}

//*****************************************************************************
void CWsfTabWnd::SetActiveWnd(CWnd *pWnd, BOOL bNotify)
{
	int i = FindWnd(pWnd);
	if (i != -1) SetActiveWnd(i, bNotify, true, false);
		
	i = FindSlaveWnd(pWnd);
	if (i != -1) SetActiveWnd(i, bNotify, true, true);
}

//*****************************************************************************
void CWsfTabWnd::SetActiveWnd(int nWnd, 
															BOOL bNotify, 		
															BOOL bUpdateSwitchOrder,
															BOOL bSlave)
{
	CWnd *pPrevWnd, *pNewWnd, *p;

	ASSERT(nWnd != -1);

	if (m_nActiveWnd == nWnd) 
	{
		p = bSlave ? m_aWnds[m_nActiveWnd]->pSlaveWnd : m_aWnds[m_nActiveWnd]->pWnd;
		if (bNotify) SendSelchangedNotify(p, p);
		return;
	}
	
	// Hide the current window

	if (m_nActiveWnd != -1) 
	{
		pPrevWnd = m_aWnds[m_nActiveWnd]->pWnd;
		ASSERT_VALID(pPrevWnd);
		if (p = m_aWnds[m_nActiveWnd]->pSlaveWnd) 
		{
			p->ShowWindow(SW_HIDE);
			m_aWnds[m_nActiveWnd]->pSplitter->ShowWindow(SW_HIDE);
		}
		else pPrevWnd->ShowWindow(SW_HIDE);
	}
	else pPrevWnd = NULL;

	// Show new window

	pNewWnd = bSlave ? m_aWnds[nWnd]->pSlaveWnd : m_aWnds[nWnd]->pWnd;
	if (p = m_aWnds[nWnd]->pSlaveWnd) 
	{
		p->ShowWindow(SW_SHOW);
		m_aWnds[nWnd]->pSplitter->ShowWindow(SW_SHOW);
	}
	else pNewWnd->ShowWindow(SW_SHOW);

	// Update tab ctrl

	m_pTabCtrl->SetCurSel(nWnd);

	// Notify the parent frame of the change

	m_nActiveWnd = nWnd;		
	if (bNotify) SendSelchangedNotify(pPrevWnd, pNewWnd);

	// Update close button

	m_pCloseCtrl->EnableWindow(m_aWnds[nWnd]->bEnableCloseWindow);

	// Update switch order

	if (bUpdateSwitchOrder) MoveActiveWndToSwitchOrderTop();
	
	// Redraw

	RecalcLayout();
	RedrawWindow();
}

//*****************************************************************************
void CWsfTabWnd::MoveActiveWndToSwitchOrderTop()
{
	if (m_aWnds.GetSize() < 2) return;

	int n = m_aSwitchOrder.FindItem(m_aWnds[m_nActiveWnd]->pWnd);
	ASSERT(n!=-1);
	m_aSwitchOrder.RemoveAt(n);
	m_aSwitchOrder.InsertAt(0, m_aWnds[m_nActiveWnd]->pWnd);
}

//*****************************************************************************
void CWsfTabWnd::ActivateNextWnd(int nSwitchOrderPos)
{
	if (m_aWnds.GetSize() < 2) return;

	nSwitchOrderPos %= m_aWnds.GetSize();
	ASSERT(nSwitchOrderPos >=0 && nSwitchOrderPos < m_aWnds.GetSize());

	int n = FindWnd(m_aSwitchOrder[nSwitchOrderPos]);
	ASSERT(n!=-1);
	SetActiveWnd(n, true, false, false);
}

//*****************************************************************************
CWnd* CWsfTabWnd::GetActiveWnd()
{
	return m_nActiveWnd != -1 ? m_aWnds[m_nActiveWnd]->pWnd : NULL;
}

//*****************************************************************************
CWnd* CWsfTabWnd::GetActiveWnd(BOOL bFirstPane)
{
	if (m_nActiveWnd == -1) return NULL;
	
	if (m_aWnds[m_nActiveWnd]->bMasterFirst)
	{
		return bFirstPane ? m_aWnds[m_nActiveWnd]->pWnd : m_aWnds[m_nActiveWnd]->pSlaveWnd;
	}
	else
	{
		return !bFirstPane ? m_aWnds[m_nActiveWnd]->pWnd : m_aWnds[m_nActiveWnd]->pSlaveWnd;
	}
}

//*****************************************************************************
CWnd* CWsfTabWnd::GetActiveSlaveWnd()
{
	return m_nActiveWnd != -1 ? m_aWnds[m_nActiveWnd]->pSlaveWnd : NULL;
}

//*****************************************************************************
void CWsfTabWnd::AddWnd(CWnd *pWnd, const char *pcszTitle, 
												int nPos /*= -1*/,
												BOOL bActivate,
												BOOL bEnableCloseWindow,
												int nIconIdx)
{
	ASSERT_VALID(pWnd);
	ASSERT(nPos >= -1);
	
	// Hide the window

	pWnd->ShowWindow(SW_HIDE);

	// Create new item
	
	if (nPos == -1 || nPos > m_aWnds.GetSize()) nPos = m_aWnds.GetSize();
	m_aWnds.InsertAt(nPos);

	m_aWnds[nPos] = new CWndInfo();
	CWndInfo& s = *m_aWnds[nPos]; 

	s.sTitle = pcszTitle;
	s.nTitleLen = strlen(s.sTitle);
	s.pWnd = pWnd;
	s.bEnableCloseWindow = bEnableCloseWindow;
	s.pSplitter = NULL;
	s.pSlaveWnd = NULL;
	s.nIconIdx = nIconIdx;
	s.bMasterFirst = true;

	// Add item to the tab ctrl

	m_pTabCtrl->InsertItem(nPos, "", s.nIconIdx);

	// Show close button

	m_pCloseCtrl->ShowWindow(SW_SHOW);

	// Shift active window index if the new window
	// was inserted before it

	if (nPos <= m_nActiveWnd) m_nActiveWnd++;

	// Add the window to the switch order

	m_aSwitchOrder += pWnd;

	// Activate the window if there is only one window or if wanted

	if (bActivate || m_aWnds.GetSize() == 1) 
		SetActiveWnd(nPos, true, true, false);
	else
	{
		RecalcLayout();
	}
}

//*****************************************************************************
BOOL CWsfTabWnd::AddSlaveWnd(CWnd* pMaster, 
														 CWnd* pSlave,
														 BOOL bMasterFirst,
														 int nDefSplitterPos)

{
	ASSERT_VALID(pSlave);

	int i = FindWnd(pMaster);
	if (i == -1) return false;

	ASSERT(!m_aWnds[i]->pSlaveWnd);
	m_aWnds[i]->pSlaveWnd = pSlave;

	m_aWnds[i]->bMasterFirst = bMasterFirst;

	m_aWnds[i]->pSplitter = new CWsfHSplitter();
	m_aWnds[i]->pSplitter->Create(NULL, NULL, WS_CHILD, WsfZeroRect, this, AFX_IDW_PANE_FIRST, NULL);
	
	pMaster->SetParent(m_aWnds[i]->pSplitter);
	pSlave->SetParent(m_aWnds[i]->pSplitter);
	
	m_aWnds[i]->pSplitter->SetPane(bMasterFirst ? 0:1, pMaster, false);
	m_aWnds[i]->pSplitter->SetPane(bMasterFirst ? 1:0, pSlave, false);
	m_aWnds[i]->pSplitter->SetSplitterPos(nDefSplitterPos);

	pSlave->ShowWindow(SW_SHOW);

	if (i == m_nActiveWnd) 
	{
		RecalcLayout();
		m_aWnds[i]->pSplitter->ShowWindow(SW_SHOW);
	}

	return true;
}

//*****************************************************************************
BOOL CWsfTabWnd::RemoveWnd(CWnd *pWnd, CWnd* pNewActiveWnd)
{
	// Check master windows
	// If the master view is found, destroy even the slave window

	int i = FindWnd(pWnd);
	if (i != -1) 
	{
		RemoveWnd(i, pNewActiveWnd);
		return true;
	}
	
	// Check slave windows
	// If the slave view is found, destroy just the slave view

	i = FindSlaveWnd(pWnd);
	
	if (i != -1) 
	{
		m_aWnds[i]->pWnd->SetParent(this);
		m_aWnds[i]->pSplitter->DestroyWindow();
		delete m_aWnds[i]->pSplitter;
		m_aWnds[i]->pSplitter = NULL;
		m_aWnds[i]->pSlaveWnd = NULL;

		if (i == m_nActiveWnd) RecalcLayout();
		return true;
	}

	return false;
}

//*****************************************************************************
void CWsfTabWnd::RemoveWnd(int nWnd, CWnd* pNewActiveWnd)
{
	int nNewActiveWndIdx;

	// If the active window is being removed, activate
	// another window

	if (m_nActiveWnd == nWnd)
	{
		if (m_aWnds.GetSize() > 1) 
		{
			// New window is either given or it's the next window in
			// the switch order
			nNewActiveWndIdx = FindWnd(pNewActiveWnd ? pNewActiveWnd : m_aSwitchOrder[1]);
			ASSERT(nNewActiveWndIdx != -1);
			ASSERT(nNewActiveWndIdx != nWnd);
			SetActiveWnd(nNewActiveWndIdx, true, true, false);
		}
		else 
		{
			m_nActiveWnd = -1;
		}
	}

	// Remove form the switch list

	int n = m_aSwitchOrder.FindItem(m_aWnds[nWnd]->pWnd);
	ASSERT(n!=-1);
	m_aSwitchOrder.RemoveAt(n);

	// Destroy master, slave, splitter

	if (m_aWnds[nWnd]->pSplitter)
	{
		m_aWnds[nWnd]->pSplitter->DestroyWindow();
		delete m_aWnds[nWnd]->pSplitter;
	}
	else m_aWnds[nWnd]->pWnd->DestroyWindow();

	// Remove from the window list 

	delete m_aWnds[nWnd];
	m_aWnds.RemoveAt(nWnd);
	m_pTabCtrl->DeleteItem(nWnd);

	// Adjust the active window index

	ASSERT(m_nActiveWnd != nWnd);
	if (m_nActiveWnd > nWnd) m_nActiveWnd--;

	// Redraw

	RecalcLayout();
	RedrawWindow();

	// Notify now, if no window is active

	if (m_nActiveWnd==-1) SendSelchangedNotify(NULL, NULL);
}

//*****************************************************************************
void CWsfTabWnd::UpdateWndTitleIcon(CWnd* pWnd, 
																		const char* pcszTitle,
																		int	nIconIdx /*= -1*/)
{
	int nWnd = FindWnd(pWnd);
	if (nWnd == -1) return;

	// Update Wnd info

	m_aWnds[nWnd]->sTitle = pcszTitle;
	m_aWnds[nWnd]->nTitleLen = strlen(m_aWnds[nWnd]->sTitle);
	if (nIconIdx != -1) m_aWnds[nWnd]->nIconIdx = nIconIdx;

	// Update tab control
	
	if (m_pTabCtrl)
	{
		RecalcLayout();
		/*CString s = GetClippedTitle(pcszTitle);
		TCITEM i;
		i.pszText = (char*) (const char*) s;
		i.mask = TCIF_TEXT;
		m_pTabCtrl->SetItem(nWnd, &i);*/
	}

	// Redraw header if nWnd is an active Wnd

	if (m_nActiveWnd == nWnd) DrawHeader(true);
}

//*****************************************************************************
BOOL CWsfTabWnd::CloseAllWnds()
{
	int i = m_aWnds.GetSize();

	while (i--)
	{
		// Close the Wnd.

		m_aWnds[i]->pWnd->SendMessage(WM_CLOSE, 0,0);
		if (IsWindow(m_aWnds[i]->pWnd->m_hWnd)) return false;

		// Remove it from the lists

		m_aWnds[i]->pWnd = NULL;
		RemoveWnd(i, NULL);
	}

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

	for (l=0,i=0,n=strlen(pcszTitle); i<n; i++)
	{
		VERIFY(GetTextExtentPoint32(m_hDC, &pcszTitle[i],1,&size));
		l += size.cx;

		if (l > (nWidth-5)) 
		{
			s += "..."; 
			break;
		}
		else s += pcszTitle[i];
	}

	return s;
}

//*****************************************************************************
void CWsfTabWnd::RecalcLayout()
{
	int nCount = m_aWnds.GetSize();

	// Close button

	if (nCount > 0) 
	{
		m_pCloseCtrl->SetWindowPos(NULL, m_nWW-8-10, 6, 14, 14, SWP_NOZORDER);
		m_pCloseCtrl->ShowWindow(SW_SHOW);
	}
	else m_pCloseCtrl->ShowWindow(SW_HIDE);

	// Window

	if (nCount > 0)
	{
		ASSERT(m_nActiveWnd!=-1);
		int h = m_nWH - m_nHH - 3;
		if (nCount > 0) h -= m_nTH;

		CWnd* p = m_aWnds[m_nActiveWnd]->pSlaveWnd ? 
			m_aWnds[m_nActiveWnd]->pSplitter :
			m_aWnds[m_nActiveWnd]->pWnd;

		p->SetWindowPos(NULL, 0, m_nHH, m_nWW, h, SWP_NOZORDER);
	}

	// Tab-control

	if (nCount > 0)
	{
		m_pTabCtrl->SetWindowPos(NULL, 0, m_nWH - m_nTH, m_nWW, m_nTH, SWP_NOZORDER);
		m_pTabCtrl->ShowWindow(SW_SHOW);

		int nItemSize = (m_nWW / m_aWnds.GetSize()) - 15;
		nItemSize = min(nItemSize,140);
		if (nItemSize < 45) nItemSize = 25;
		m_pTabCtrl->SetItemSize(CSize(nItemSize, m_nTH));

		for (int i=0; i<m_aWnds.GetSize(); i++)
		{
			CString s;
			TCITEM ti;
			ti.mask = TCIF_TEXT | TCIF_IMAGE;
			ti.iImage = m_aWnds[i]->nIconIdx;

			if (nItemSize>25 || m_aWnds[i]->nIconIdx==-1)
			{
				int n = m_aWnds[i]->nIconIdx==-1 ? nItemSize : nItemSize-20;
				s = GetClippedTitle(m_aWnds[i]->sTitle, n);
				ti.pszText = (char*) (const char*) s;
			}
			else ti.pszText = NULL;

			VERIFY(m_pTabCtrl->SetItem(i, &ti));
		}
	}
	else m_pTabCtrl->ShowWindow(SW_HIDE);
}

//*****************************************************************************
void CWsfTabWnd::DrawHeader(BOOL bRedrawButton) 
{
	RECT r;
	if (m_nActiveWnd == -1) return;

	// Draw title background

	r.left = 0;
	r.top = 0;
	r.bottom = r.top + m_nHH;
	r.right = m_nWW - 0;
		
	//DrawEdge(m_hDC, &r, BDR_RAISEDINNER, BF_RECT);
	FillRect(m_hDC, &r, (HBRUSH)(COLOR_3DLIGHT+1));
	r.top += 2;
	r.bottom -= 3;
	FillRect(m_hDC, &r, (HBRUSH)(COLOR_3DSHADOW+1));
		
	// Draw the window title
	
	SetTextColor(m_hDC, RGB(255,255,255));
	TextOut(m_hDC, 4, (m_nHH-m_nFH)/2, m_aWnds[m_nActiveWnd]->sTitle, m_aWnds[m_nActiveWnd]->nTitleLen);

	// Redraw button

	if (bRedrawButton) m_pCloseCtrl->RedrawWindow();
}

//*****************************************************************************
void CWsfTabWnd::OnPaint() 
{
	CPaintDC dc(this); 
	RECT r;

	if (m_nActiveWnd != -1)
	{
		// Draw border around the window

		r.left = 0;
		r.top = m_nWH - m_nTH - 3;
		r.bottom = m_nWH;
		r.right = m_nWW;
		FillRect(m_hDC, &r, (HBRUSH)(COLOR_3DLIGHT+1));

		// Draw header
		
		DrawHeader(false);
	}
	else
	{
		r.left = 0;
		r.top = 0;
		r.bottom = m_nWH;
		r.right = m_nWW;
		DrawEdge(m_hDC, &r, BDR_SUNKENINNER|BDR_SUNKENOUTER, BF_RECT);
	}
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
	return m_nActiveWnd != -1 ? true : CWnd::OnEraseBkgnd(pDC);
}

/*****************************************************************************
*
* MESSAGES
*
*****************************************************************************/

//*****************************************************************************
BOOL CWsfTabWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR *p = (NMHDR*) lParam;

	switch (wParam)
	{

	case WSF_TABWND_TAB_ID:
		{
			switch (p->code)
			{
			case TCN_SELCHANGE:
				SetActiveWnd(m_pTabCtrl->GetCurSel(), true, true, false);
				return true;

			case NM_RCLICK:
				if (m_nFlags & WSF_TWF_CONTEXT_MENU) OnTabContextMenu();
				return true;
			}
			break;
		}
	}

	return CWnd::OnNotify(wParam, lParam, pResult);
}

//*****************************************************************************
void CWsfTabWnd::OnTabContextMenu()
{
	ASSERT(m_pTabCtrl);

	// Get clicked tab

	TCHITTESTINFO s;
	GetCursorPos(&s.pt);
	m_pTabCtrl->ScreenToClient(&s.pt);
	int nTabIdx = m_pTabCtrl->HitTest(&s);
	if (nTabIdx == -1) return;

	// Get menu

	HMENU hMenu = LoadMenu(g_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_MENUS));
	ASSERT(hMenu);
	HMENU hSubMenu = GetSubMenu(hMenu, 1);
	ASSERT(hSubMenu);
	SetMenuDefaultItem(hSubMenu, ID_WSF_WINDOW_CLOSE, false);
  
  EnableMenuItem(hSubMenu, ID_WSF_WINDOW_CLOSE, 
    MF_BYCOMMAND | (m_aWnds[nTabIdx]->bEnableCloseWindow ? MF_ENABLED:MF_DISABLED|MF_GRAYED));

	// Display context menu

	m_pTabCtrl->ClientToScreen(&s.pt);	
	int nCmdId = TrackPopupMenu(hSubMenu, 
		TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD|TPM_NONOTIFY,
		s.pt.x, s.pt.y, 0, *WsfGetFrame(), NULL);

	// Handle command

	switch (nCmdId)
	{
	case ID_WSF_WINDOW_CLOSE:
    ASSERT(m_aWnds[nTabIdx]->bEnableCloseWindow);
		// Close slave first, then master
		if (m_aWnds[nTabIdx]->pSlaveWnd) WsfGetApp()->CloseView((CView*)m_aWnds[nTabIdx]->pSlaveWnd);
		WsfGetApp()->CloseView((CView*)m_aWnds[nTabIdx]->pWnd);
		break;
	}
}

//*****************************************************************************
BOOL CWsfTabWnd::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*) pNMHDR;

	if (!(pTTT->uFlags & TTF_IDISHWND)) return false;
	int nID = ::GetDlgCtrlID((HWND)pNMHDR->idFrom);

	if (nID == WSF_TABWND_TAB_ID)
	{
		TCHITTESTINFO hti;
		GetCursorPos(&hti.pt);
		m_pTabCtrl->ScreenToClient(&hti.pt);
		int i = m_pTabCtrl->HitTest(&hti);
		if (i != -1)
		{
			pTTT->lpszText  = (char*) (const char*) m_aWnds[i]->sTitle; 
			pTTT->hinst = NULL;
			return true;
		}
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
		else TRACE1("CWsfTabWnd::OnToolTipNotify(): Tooltip text for \"%d\" not found!\n", nID);
	}
	
	return false;
}

//*****************************************************************************
void CWsfTabWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (m_nActiveWnd == -1) WsfGetFrame()->ChooseToolBars();
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

	s.code = WSF_TABWND_NM_ONCLOSE;
	s.hwndFrom = m_hWnd;
	s.idFrom = GetWindowLong(m_hWnd, GWL_ID);

	m_pFrameWnd->SendMessage(WM_NOTIFY, s.idFrom, (LPARAM) &s);
}

//*****************************************************************************
void CWsfTabWnd::OnWsfNextTabWindow()
{
	int n = m_nActiveWnd;
	if (++n >= m_aWnds.GetSize()) n = 0;
	SetActiveWnd(n, true, true, false);
}

//*****************************************************************************
void CWsfTabWnd::OnUpdateWsfNextTabWindow(CCmdUI* p)
{
	p->Enable(m_aWnds.GetSize() > 1);
}

//*****************************************************************************
void CWsfTabWnd::OnWsfPrevTabWindow()
{
	int n = m_nActiveWnd;
	if (--n < 0) n = m_aWnds.GetSize()-1;
	SetActiveWnd(n, true, true, false);
}

//*****************************************************************************
void CWsfTabWnd::OnUpdateWsfPrevTabWindow(CCmdUI* p)
{
	p->Enable(m_aWnds.GetSize() > 1);
}



