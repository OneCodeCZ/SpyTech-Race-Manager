///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfToolBar.h"
#include "WsfCmd.h"
#include "WsfToolBarComboBox.h"
#include "WsfIconList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*****************************************************************************
*
* CToolCmdUI
*
*****************************************************************************/

/*****************************************************************************/
CWsfToolBar::CToolCmdUI::CToolCmdUI()
{
   m_bEnableChanged = false;
   m_bCheckChanged = false;
}

//*****************************************************************************
void CWsfToolBar::CToolCmdUI::Enable(BOOL bOn)
{
	m_bEnableChanged = TRUE;
   m_bEnabled = bOn;
}

//*****************************************************************************
void CWsfToolBar::CToolCmdUI::SetCheck(int nCheck)
{
	ASSERT(nCheck >= 0 && nCheck <= 1); // 0=>off, 1=>on, 2=>indeterminate
   m_bCheckChanged = true;
   m_nCheck = nCheck;
}

//*****************************************************************************
void CWsfToolBar::CToolCmdUI::SetText(LPCTSTR)
{
	ASSERT(false);
}

/*****************************************************************************/
void CWsfToolBar::CToolCmdUI::PerformChanges()
{
	CWsfToolBar* pToolBar = (CWsfToolBar*) m_pOther;
	ASSERT(pToolBar);

	if (m_bEnableChanged)
   {
      pToolBar->SendMessage(TB_ENABLEBUTTON, m_nID, MAKELONG(m_bEnabled, 0));
      
      if (!m_bEnabled)
      {
         m_bCheckChanged = true;
         m_nCheck = false;
      }
   }

   if (m_bCheckChanged)
   	pToolBar->SendMessage(TB_CHECKBUTTON, m_nID, MAKELONG(m_nCheck, 0));

   m_bEnableChanged = false;
   m_bCheckChanged = false;
}


/*****************************************************************************
*
* CWindowToolCmdUI
*
*****************************************************************************/

//*****************************************************************************
void CWsfToolBar::CWindowToolCmdUI::PerformChanges()
{
	m_bEnableChanged = true;
	ASSERT(m_pOther);

   if (m_bEnableChanged) m_pOther->EnableWindow(m_bEnabled);
   
   m_bEnableChanged = false;
   m_bCheckChanged = false;
}


/*****************************************************************************
*
* CWsfToolBar
*
*****************************************************************************/

BEGIN_MESSAGE_MAP(CWsfToolBar, CControlBar)
	//{{AFX_MSG_MAP(CWsfToolBar)
	ON_WM_CREATE()
	ON_WM_NCPAINT()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(TBN_DROPDOWN, OnDropDown)
END_MESSAGE_MAP()

//*****************************************************************************
CWsfToolBar::CWsfToolBar()
{
}

//*****************************************************************************
CWsfToolBar::~CWsfToolBar()
{
	// Delete all child window objects

	for (int i=m_aCmds.GetSize(); i--;) 
	{
		if (m_aCmds[i].pObject) 
		{
			ASSERT_VALID(m_aCmds[i].pObject);
			delete m_aCmds[i].pObject;
		}
	}
}

//*****************************************************************************
BOOL CWsfToolBar::Create(CWnd *pParent, DWORD dwStyle, int nID)
{
	dwStyle |= CCS_NOPARENTALIGN|CCS_NOMOVEY|CCS_NODIVIDER|CCS_NORESIZE;
	dwStyle |= TBSTYLE_LIST;
   
   //if (WsfGetApp()->m_eWinVersion != CWsfApp::WINXP)
   {
   	dwStyle |= TBSTYLE_FLAT;
   }
	
	SetBorders(CRect(4,3,4,3));   // control bar borders

	return CWnd::Create(TOOLBARCLASSNAME, NULL, dwStyle, WsfZeroRect, pParent, nID);
}


/*****************************************************************************/
BOOL CWsfToolBar::CreateControl(CWnd* pParent, int nId)
{
   ASSERT(pParent);
   CWnd* p;
   CRect r;

   if (nId)
   {
      p = pParent->GetDlgItem(nId);
      ASSERT(p);
      p->GetWindowRect(&r);
      pParent->ScreenToClient(&r);
   }

   BOOL b = Create(pParent, WS_CHILD|WS_VISIBLE|TBSTYLE_TOOLTIPS, nId);
   if (!b) return false;

   SetCmdBitmaps();
   SetOwner(pParent);

   // Put the table at the same position in the children order
   // as the original window

   if (nId) 
   {
      SetWindowPos(p, r.left, r.top, r.Width(), r.Height(), 0);
      p->DestroyWindow();
   }

   return true;
}

//*****************************************************************************
int CWsfToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CControlBar::OnCreate(lpCreateStruct) == -1) return -1;
	
	DefWindowProc(TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	DefWindowProc(TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
	VERIFY(DefWindowProc(TB_SETBITMAPSIZE, 0, MAKELONG(16,16)));
	
   //DWORD x = DefWindowProc(TB_GETPADDING, 0, 0);
	//VERIFY(DefWindowProc(TB_SETPADDING, 0, MAKELPARAM(x>>16,0)));
  
   //EnableToolTips(true);

	return 0;
}

//*****************************************************************************
void CWsfToolBar::SetOwner(CWnd* pOwnerWnd)
{
	ASSERT_VALID(this);
	if (m_hWnd != NULL)
	{
		ASSERT(::IsWindow(m_hWnd));
		DefWindowProc(TB_SETPARENT, (WPARAM)pOwnerWnd->GetSafeHwnd(), 0);
	}
	CControlBar::SetOwner(pOwnerWnd);
}

//*****************************************************************************
void CWsfToolBar::SetCmdBitmaps()
{
	DefWindowProc(TB_SETIMAGELIST, 0, (LPARAM) (HIMAGELIST) *WsfGetApp()->GetSmallIconList());
	DefWindowProc(TB_SETDISABLEDIMAGELIST, 0, (LPARAM) (HIMAGELIST) *WsfGetApp()->GetGraySmallIconList());
}

//*****************************************************************************
void CWsfToolBar::AddCmd(int nCmdId, EShow eShowText, int nPos)
{
	const CWsfCmd* pCmd;
	int nTextIdx;
	TBBUTTON b;
	TBBUTTONINFO tbi;
	RECT r;

	if (nPos == -1) nPos = GetButtonCount();

	// Init toolbar command

	pCmd = WsfGetApp()->GetCmd(nCmdId);
	ASSERT(pCmd);
   if (!pCmd) return;

	m_aCmds.InsertAt(nPos);
	CToolBarCmd& tbcmd = m_aCmds[nPos];

	tbcmd.pCmd = pCmd;
	tbcmd.pObject = NULL;
   
	// Add command text to the toolbar control if needed

	if (eShowText==YES || (eShowText==DEFAULT && pCmd->m_bShowText))
	{
		nTextIdx = DefWindowProc(TB_ADDSTRING, NULL, (LPARAM) (const char*)pCmd->GetText());
		ASSERT(nTextIdx != -1);
	}
	else nTextIdx = -1;

	// Insert toolbar control item that represents the command

	switch (pCmd->m_nType)
	{
	case CWsfCmd::COMBO_BOX:
		{
			tbcmd.nCurCmdId = nCmdId;

			// Create combo box

			tbcmd.pObject = pCmd->m_pClass->CreateObject();
			ASSERT(tbcmd.pObject);

			CWsfToolBarComboBox* pWnd = (CWsfToolBarComboBox*) tbcmd.pObject;
			ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CWsfToolBarComboBox)));

			pWnd->Create(WS_CHILD, WsfZeroRect, this, pCmd->m_nId);

			// Insert separator that will be turned into a combo box

			b.fsState = 0;
			b.fsStyle = BTNS_SEP;
			b.idCommand = nCmdId;
			b.iBitmap = pCmd->m_nWidth + 8;			// means width for separator
			VERIFY(DefWindowProc(TB_INSERTBUTTON, nPos, (LPARAM) &b));

			// Repos and resize the window

			VERIFY(GetButtonRect(nPos, &r));
			pWnd->SetWindowPos(NULL, r.left + 4, r.top, pCmd->m_nWidth, r.bottom - r.top - 4+200, SWP_NOZORDER);
			pWnd->ShowWindow(SW_SHOW);
		}
		break;

	case CWsfCmd::DROP_DOWN_MENU:
		{
			const CWsfCmd* p = WsfGetApp()->GetCmd(pCmd->m_nDefCmdId);
			ASSERT(p);
			tbcmd.nCurCmdId = p->m_nId;

			b.fsState = 0;
			b.fsStyle = BTNS_DROPDOWN | (nTextIdx != -1 ? BTNS_AUTOSIZE : 0);
			b.idCommand = p->m_nId;
			b.iBitmap = p->m_nImgIdx;
			b.iString = nTextIdx;
			VERIFY(DefWindowProc(TB_INSERTBUTTON, nPos, (LPARAM) &b));
		}
		break;

	case CWsfCmd::DROP_DOWN_MENU_ONLY:
		{
			tbcmd.nCurCmdId = 0;
			b.fsState = TBSTATE_ENABLED;
			b.fsStyle = BTNS_DROPDOWN | (nTextIdx != -1 ? BTNS_AUTOSIZE : 0) | BTNS_WHOLEDROPDOWN;
			b.idCommand = nCmdId;
         b.iBitmap = pCmd->m_nImgIdx != -1 ? pCmd->m_nImgIdx : I_IMAGENONE ;
			b.iString = nTextIdx;
			VERIFY(DefWindowProc(TB_INSERTBUTTON, nPos, (LPARAM) &b));
		}
		break;

	case CWsfCmd::DROP_DOWN_CUSTOM:
		{
			b.fsState = 0;
			b.fsStyle = BTNS_DROPDOWN | (nTextIdx != -1 ? BTNS_AUTOSIZE : 0);
			b.idCommand = nCmdId;
			b.iBitmap = pCmd->m_nImgIdx;
			b.iString = nTextIdx;
			VERIFY(DefWindowProc(TB_INSERTBUTTON, nPos, (LPARAM) &b));

			// Correct button size
			if (nTextIdx == -1)
			{	
				tbi.cbSize = sizeof(tbi);
				tbi.dwMask = TBIF_SIZE;
				tbi.cx = 24 + 12;
				VERIFY(DefWindowProc(TB_SETBUTTONINFO, nCmdId, (LPARAM) &tbi));
			}

			tbcmd.nCurCmdId = nCmdId;
		}
		break;

	case CWsfCmd::SIMPLE:
		{
			b.fsState = 0;
			b.fsStyle = TBSTYLE_BUTTON | (nTextIdx != -1 ? BTNS_AUTOSIZE : 0);
			b.idCommand = nCmdId;
			b.iBitmap = pCmd->m_nImgIdx;
			b.iString = nTextIdx;
			VERIFY(DefWindowProc(TB_INSERTBUTTON, nPos, (LPARAM) &b));

			// Correct button size
			if (nTextIdx == -1)
			{	
				tbi.cbSize = sizeof(tbi);
				tbi.dwMask = TBIF_SIZE;
				tbi.cx = 24;
				VERIFY(DefWindowProc(TB_SETBUTTONINFO, nCmdId, (LPARAM) &tbi));
			}

			tbcmd.nCurCmdId = nCmdId;
		}
		break;
	}

	RecalcChildLayout(nPos+1);
}

//*****************************************************************************
void CWsfToolBar::AddSeparator(int nPos /*= -1*/)
{
	TBBUTTON b;

	if (nPos == -1) nPos = GetButtonCount();

	m_aCmds.GrowBy(1);
	CToolBarCmd& tbcmd = m_aCmds.GetLast();
	
	tbcmd.pCmd = NULL;
	tbcmd.pObject = NULL;

	b.fsState = 0;
	b.fsStyle = BTNS_SEP;
	b.idCommand = 0;
	b.dwData = 0;
	b.iBitmap = 0;

	VERIFY(DefWindowProc(TB_INSERTBUTTON, nPos, (LPARAM) &b));

	RecalcChildLayout(nPos+1);
}

/*****************************************************************************/
int CWsfToolBar::IsEmpty()
{
   return GetButtonCount()==0;
}

/*****************************************************************************/
void CWsfToolBar::Empty()
{
   for (int i=GetButtonCount(); i--;)
   {
      DefWindowProc(TB_DELETEBUTTON, (WPARAM) i, 0);
   }
}

/*****************************************************************************/
void CWsfToolBar::RecalcChildLayout(int nFromIdx)
{
	for (int i=nFromIdx, n=GetButtonCount(); i<n; i++)
	{
		if (m_aCmds[i].pCmd && m_aCmds[i].pCmd->m_nType == CWsfCmd::COMBO_BOX)
		{
			RECT r;
			VERIFY(GetButtonRect(i, &r));
			CWnd* pWnd = (CWnd*) m_aCmds[i].pObject;
			pWnd->SetWindowPos(NULL, 
				r.left + 4, r.top, 
				m_aCmds[i].pCmd->m_nWidth, r.bottom - r.top - 4+200, 
				SWP_NOZORDER);
		}
	}
}

//*****************************************************************************
void CWsfToolBar::OnDropDown(NMHDR *p, LRESULT *pResult)
{
	int i, nCmdId;
	NMTOOLBAR *pNMTB = (NMTOOLBAR*) p;
	RECT r;

	*pResult = TBDDRET_DEFAULT;

	// Get command index

	i = CmdToIdx(pNMTB->iItem);
	ASSERT(i != -1);

	// Get position for drop down

	VERIFY(GetButtonRect(i, &r));
	ClientToScreen(&r);
	r.top += 20;

	// Display drop down window

	switch (m_aCmds[i].pCmd->m_nType)
	{
	case CWsfCmd::DROP_DOWN_MENU_ONLY:
	case CWsfCmd::DROP_DOWN_MENU:
		{
			HMENU h = m_aCmds[i].pCmd->m_hMenu;
			ASSERT(h);
			SetMenuDefaultItem(h, m_aCmds[i].nCurCmdId, false);

			// Display menu

			nCmdId = TrackPopupMenu(h, 
				TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD,
				r.left, r.top , 0, WsfGetFrame()->m_hWnd, NULL);

			if (!nCmdId) return;
			
			// Update default command

/*			if (m_aCmds[i].pCmd->m_bSeparateArrow)
			{
				m_aCmds[i].nCurCmdId = nCmdId;
				VERIFY(DefWindowProc(TB_SETCMDID, i, nCmdId));

				const CWsfCmd* p = WsfGetApp()->GetCmd(nCmdId);
				ASSERT(p);
				ASSERT(p->m_nImgIdx != -1);

				VERIFY(DefWindowProc(TB_CHANGEBITMAP, nCmdId, MAKELPARAM(p->m_nImgIdx,0)));
			}*/

			// Issue the command

			WsfGetFrame()->PostMessage(WM_COMMAND, nCmdId, 0);
		}
		break;

	case CWsfCmd::DROP_DOWN_CUSTOM:
		{
			CWnd* pWnd = (CWnd*) m_aCmds[i].pCmd->m_pClass->CreateObject();
			ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CWnd)));

			r.bottom = r.top + 20;
			r.right = r.left + 100;
			VERIFY(pWnd->Create(NULL, NULL, 0, r, WsfGetFrame(), 0, NULL));

			::SetWindowPos(*pWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);

			// NOTE: Window must auto delete itself!!!
		}
		break;
	}
}

//*****************************************************************************
CSize CWsfToolBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	CSize s;
	RECT r;
	CRect rect;

	// Get toolbar dimensions

	s.cx = 0;

	for (int i = GetButtonCount(); i--;)
	{
		VERIFY(GetButtonRect(i, &r));
		s.cx += r.right - r.left;
   	s.cy = r.bottom+1;
	}

	// Add borders

	rect.SetRect(0,0,s.cx,s.cy);
	CControlBar::CalcInsideRect(rect, (m_dwStyle & CBRS_ORIENT_HORZ) != 0);

	s.cx += rect.left + (s.cx-rect.right);
	s.cy += rect.top + (s.cy-rect.bottom);

	return s;
}

//*****************************************************************************
CSize CWsfToolBar::CalcDynamicLayout( int nLength, DWORD dwMode)
{
	return CalcFixedLayout(false, false);
};

//*****************************************************************************
void CWsfToolBar::OnNcPaint() 
{
   // Draws the gripper and borders
	EraseNonClient();
}

//*****************************************************************************
void CWsfToolBar::OnPaint() 
{
   Default();
}

//*****************************************************************************
BOOL CWsfToolBar::OnEraseBkgnd(CDC* pDC) 
{
	return (BOOL) Default();
}

//*****************************************************************************
LRESULT CWsfToolBar::OnNcHitTest(CPoint point) 
{
	return HTCLIENT;
}

//*****************************************************************************
void CWsfToolBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	// Set client area

	CRect r = lpncsp->rgrc[0];

	CalcInsideRect(r, (m_dwStyle & CBRS_ORIENT_HORZ) != 0);

	lpncsp->rgrc[0] = r;
}

//*****************************************************************************
void CWsfToolBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)
{
   // Toolbar's owner handles the UI

   CWnd* pContextWnd = GetOwner();

   // Init 

	CToolCmdUI s;
	s.m_pOther = this;
	s.m_nIndexMax = m_aCmds.GetSize();
	s.m_nIndex = 0;

	CWindowToolCmdUI s2;
	s2.m_nIndexMax = m_aCmds.GetSize();
	s2.m_nIndex = 0;

	// Update all tools on the toolbar

	for (int i=m_aCmds.GetSize(); i--;)
	{
		if (!m_aCmds[i].pCmd) continue;  // no cmd at this index?

		// Init update structure according the CWsfCmd type

   	CToolCmdUI* pCmdUI;

		switch (m_aCmds[i].pCmd->m_nType)
		{
		case CWsfCmd::COMBO_BOX: 
			s2.m_pOther = (CWnd*) m_aCmds[i].pObject;
			pCmdUI = &s2; 
			break;

		default: 
			pCmdUI = &s; 
			break;
		}

		pCmdUI->m_nID = m_aCmds[i].nCurCmdId;
	
		// Just dropdown, no command?

		if (m_aCmds[i].pCmd->m_nType == CWsfCmd::DROP_DOWN_MENU_ONLY)
		{
			//pCmdUI->Enable();
			continue;
		}

		// allow reflections

		//b = CWnd::OnCmdMsg(0, MAKELONG((int)CN_UPDATE_COMMAND_UI, WM_COMMAND+WM_REFLECT_BASE), pCmdUI, NULL);
		//if (b) continue;

		// allow the toolbar itself to have update handlers

		//b = CWnd::OnCmdMsg(pCmdUI->m_nID, CN_UPDATE_COMMAND_UI, pCmdUI, NULL);
		//if (b) continue;

		// Allow the owner to process the update


      pCmdUI->DoUpdate(pContextWnd, bDisableIfNoHndler);
      pCmdUI->PerformChanges();
	}

	//update the dialog controls added to the toolbar
	//UpdateDialogControls(pTarget, bDisableIfNoHndler);
}

//*****************************************************************************
int CWsfToolBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	CRect r;
	TBBUTTON tb;
	CWsfToolBar* pBar = (CWsfToolBar*) this;
	int i;

	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	// Check child windows first by calling CControlBar

	i = CControlBar::OnToolHitTest(point, pTI);
	if (i != -1) return i;

	// Now hit test against ToolBar buttons

	for (i = pBar->GetButtonCount(); i--;)
	{
		VERIFY(pBar->GetButtonRect(i, (RECT*)r));
		r.bottom++; 
		r.right++;
		if (!r.PtInRect(point)) continue;

		VERIFY(pBar->GetButton(i, &tb));
		if (tb.fsStyle & BTNS_SEP) continue;

		if (pTI != NULL)
		{
			pTI->hwnd = m_hWnd;
			pTI->rect = r;
			pTI->uId = tb.idCommand;
			pTI->lpszText = LPSTR_TEXTCALLBACK;
		}

		return tb.idCommand;
	}

	return -1;
}

//*****************************************************************************
void CWsfToolBar::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	// not necessary to invalidate the borders
	DWORD dwStyle = m_dwStyle;
	m_dwStyle &= ~(CBRS_BORDER_ANY);
	CControlBar::OnWindowPosChanging(lpwndpos);
	m_dwStyle = dwStyle;

	// If we can resize while floating
	if (dwStyle & CBRS_SIZE_DYNAMIC)
	{
		// And we are resizing
		if (lpwndpos->flags & SWP_NOSIZE)
			return;

		// Then redraw the buttons
		Invalidate();
	}
}

