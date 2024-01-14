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
#include "WsfToolBar.h"
#include "WsfCmdUI.h"
#include "WsfAppPlugin.h"
#include "WsfCmd.h"
#include "WsfIconList.h"

// Child windows IDs

#define WSF_MF_RIGHT_PANE			1001
#define WSF_MF_LEFT_PANE			1002

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CWsfFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_WSF_TREE_VIEW, OnWsfTreeView)
	ON_UPDATE_COMMAND_UI(ID_WSF_TREE_VIEW, OnUpdateWsfTreeView)
	ON_WM_DESTROY()
	ON_COMMAND(ID_WSF_WINDOW_CLOSE, OnWsfWindowClose)
	ON_UPDATE_COMMAND_UI(ID_WSF_WINDOW_CLOSE, OnUpdateWsfWindowClose)
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_LARGEICON, OnViewLargeicon)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LARGEICON, OnUpdateViewLargeicon)
	ON_COMMAND(ID_VIEW_LIST, OnViewList)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIST, OnUpdateViewList)
	ON_COMMAND(ID_VIEW_SMALLICON, OnViewSmallicon)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SMALLICON, OnUpdateViewSmallicon)
	ON_COMMAND(ID_VIEW_DETAILS, OnViewReport)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DETAILS, OnUpdateViewReport)
	ON_COMMAND(ID_WSF_WINDOW_CLOSEALL, OnWindowCloseall)
	ON_UPDATE_COMMAND_UI(ID_WSF_WINDOW_CLOSEALL, OnUpdateWindowCloseall)
	ON_WM_NCRBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MEASUREITEM()
	ON_WM_DRAWITEM()

   ON_COMMAND(ID_WSF_NEXT_PANE, OnWsfNextPane)
	ON_UPDATE_COMMAND_UI(ID_WSF_NEXT_PANE, OnUpdateWsfNextPane)

	ON_COMMAND(ID_WSF_PREVIOUS_PANE, OnWsfPreviousPane)
	ON_UPDATE_COMMAND_UI(ID_WSF_PREVIOUS_PANE, OnUpdateWsfPreviousPane)

	ON_MESSAGE(WSF_WM_DB_HAS_CHANGED, OnWsfWmDbHasChanged)

   // Both messages must be specified to disable the CFrameWnd's 
   // tooltip text implementation
	ON_NOTIFY_EX(TTN_NEEDTEXTW, 0, OnToolTipNeedTextW)
	ON_NOTIFY_EX(TTN_NEEDTEXTA, 0, OnToolTipNeedTextA)

   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//*****************************************************************************
CWsfFrame::CWsfFrame()
{
	m_pSplitterWnd = new CWsfVSplitter();
   m_pLeftPane = new CWsfTabWnd(CWsfTabWnd::BUTTONS, false);
   m_pRightPane = new CWsfTabWnd(CWsfTabWnd::TABS, true);

	ASSERT(m_pSplitterWnd);
	ASSERT(m_pLeftPane);
	ASSERT(m_pRightPane);
}

//*****************************************************************************
CWsfFrame::~CWsfFrame()
{
	delete m_pSplitterWnd;
	delete m_pLeftPane;
	delete m_pRightPane;
}

//*****************************************************************************
BOOL CWsfFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs)) return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
//   cs.dwExStyle |= WS_EX_COMPOSITED;	

	// :NOTE: Don't set windows class, because the default one has icon set.

	return TRUE;
}

//*****************************************************************************
int CWsfFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CWsfApp::CSetup& s = WsfGetApp()->m_Setup;

	EnableToolTips();

	// If this assert fails, the user created this frame 
	// before CWsfApp::LoadSetup was called.

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;

	// Don't erase the background for flicker free resizing.

	SetClassLong(m_hWnd, GCL_HBRBACKGROUND, NULL);

	// Create splitter window
	// ID=AFX_IDW_PANE_FIRST -> MFC manages sizing

	if (!m_pSplitterWnd->Create(NULL, NULL, WS_VISIBLE|WS_CHILD, WsfZeroRect, this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("CWsfFrame: Failed to create splitter.\n");
		return -1;
	}

	m_pSplitterWnd->SetSplitterPos(s.nSplitter);

	// Create the workplace tab window

	if (!m_pRightPane->Create(NULL, NULL, WS_VISIBLE|WS_CHILD, WsfZeroRect, m_pSplitterWnd, WSF_MF_RIGHT_PANE, NULL))
	{
		TRACE0("CWsfFrame: Failed to create workplace window.\n");
		return -1;
	}

   m_pSplitterWnd->SetPane(CWsfVSplitter::RIGHT, m_pRightPane);

	// Create the tab window for tree views

	if (!m_pLeftPane->Create(NULL, NULL, WS_CHILD, WsfZeroRect, m_pSplitterWnd, WSF_MF_LEFT_PANE, NULL))
	{
		TRACE0("CWsfFrame: Failed to create tree view tab window.\n");
		return -1;
	}

	m_bLeftPaneVisible = s.bLeftPaneVisible;
	ShowLeftPane(m_bLeftPaneVisible);

	// Init status bar

	if (!m_StatusBarWnd.Create(this)) { ASSERT(false); return -1; }

	// Init stuff

	EnableDocking(CBRS_ALIGN_ANY);
   WsfGetApp()->ProcessMenu(*GetMenu());

	return 0;
}

//*****************************************************************************
void CWsfFrame::GetMessageString(UINT nID, CString& rMessage) const
{
	CView *pView;

	switch (nID)
	{
	case AFX_IDS_IDLEMESSAGE:
		pView = GetActiveView();
		if (!pView || !pView->SendMessage(WSF_WM_UPDATE_MESSAGE_TEXT,(WPARAM)&rMessage,0))
			CFrameWnd::GetMessageString(nID, rMessage);
		break;

	default:
		CFrameWnd::GetMessageString(nID, rMessage);
		break;
	}
}

//*****************************************************************************
void CWsfFrame::OnDestroy() 
{
	WINDOWPLACEMENT wp;
	CWsfApp::CSetup& s = WsfGetApp()->m_Setup;
	
	// Save UI parameters

	GetWindowPlacement(&wp);

	s.bLeftPaneVisible = m_bLeftPaneVisible;
	s.nSplitter = m_pSplitterWnd->GetSplitterPos();

	s.nShowType = wp.showCmd;
	s.nX = wp.rcNormalPosition.left;
	s.nY = wp.rcNormalPosition.top;
	s.nW = wp.rcNormalPosition.right - s.nX;
	s.nH = wp.rcNormalPosition.bottom - s.nY;

	// End

	CFrameWnd::OnDestroy();
}

//*****************************************************************************
CDocument* CWsfFrame::GetActiveDocument()
{
	ASSERT_VALID(this);
	CView* pView = (CView*) m_pRightPane->GetActiveWnd();

	return pView ? pView->GetDocument() : NULL;
}

//*****************************************************************************
void CWsfFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	ASSERT(GetStyle() & FWS_ADDTOTITLE);
	if (!bAddToTitle) return;

	char temp[256 + MAX_PATH];
	CWsfApp& a = *WsfGetApp();
	CString s;
	CDocument* pWorkspaceDoc = a.GetWorkspaceDoc();
	CView* pView = (CView*) GetPane(false)->GetActiveWnd();

   // No workspace
	if (!pWorkspaceDoc || a.GetWorkspaceName().IsEmpty())
	{
		if (pView)
		{
			pView->GetWindowText(s);
			wsprintf(temp, "%s - %s%s", 
				m_strTitle, 
            s, 
				pView->GetDocument()->IsModified() ? " *":"");
		}
		else strcpy(temp, m_strTitle);
	}
	else
   // Workspace
	{
		if (pView)
		{
			pView->GetWindowText(s);
			wsprintf(temp, "%s%s - %s%s - %s", 
				a.GetWorkspaceName(),
				a.IsWorkspaceModified() ? " *":"",
            s, 
				pView->GetDocument() != a.GetWorkspaceDoc() && pView->GetDocument()->IsModified() ? " *":"",
            m_strTitle
            );
		}
		else
		{
			wsprintf(temp, "%s%s - %s", 
				a.GetWorkspaceName(),
				a.IsWorkspaceModified() ? " *":"",
				m_strTitle);
		}
	}

	SetWindowText(temp);
}

//*****************************************************************************
void CWsfFrame::ActivateNextDoc(int nDocIdx, BOOL bSwap)
{
/*	int n = WsfGetApp()->m_aDocs.GetSize();
	if (n)
	{
		nDocIdx %= n;
		m_pRightPane->SetActiveWnd(WsfGetApp()->m_aDocs[nDocIdx].pView, false);
		SetActiveView(WsfGetApp()->m_aDocs[nDocIdx].pView);
		OnUpdateFrameTitle(true);
		if (bSwap) WsfGetApp()->MoveDocToTop(WsfGetApp()->m_aDocs[nDocIdx].pDoc);
	}*/
}


//*****************************************************************************
void CWsfFrame::ActivatePane(BOOL bRight)
{
	if (bRight)
	{
		CView* p = (CView*) m_pRightPane->GetActiveWnd();
		if (p) SetActiveView(p);
	}
	else if (m_pLeftPane->IsWindowVisible())
	{
		CView* p = (CView*) m_pLeftPane->GetActiveWnd();
		if (p) SetActiveView(p);
	}
}

//*****************************************************************************
void CWsfFrame::ShowLeftPane(BOOL bShow)
{
	if (bShow)
	{
		if (m_pLeftPane->GetTabCount())
		{
			m_pLeftPane->ShowWindow(SW_SHOW);
			m_pSplitterWnd->SetPane(CWsfVSplitter::LEFT, m_pLeftPane);

			// If no doc view is active, activate the folder list view

			if (!GetActiveView()) SetActiveView((CView*)m_pLeftPane->GetActiveWnd());
		}
	}

	else
	
	{
		// Deactive the tree view if active. Activate opened doc, if any.

		if (GetActiveView() == (CView*) m_pLeftPane->GetActiveWnd()) 
		{
			SetActiveView((CView*) m_pRightPane->GetActiveWnd());	// If GetActiveWnd returns NULL, its OK.
		}

		m_pLeftPane->ShowWindow(SW_HIDE);
		m_pSplitterWnd->SetPane(CWsfVSplitter::LEFT, NULL);
	}
}

//*****************************************************************************
BOOL CWsfFrame::IsCmdEnabled(int nCmdId)
{
	CWsfTestCmdUI s(nCmdId);
	OnCmdMsg(nCmdId, CN_UPDATE_COMMAND_UI, &s, NULL);
	return s.IsEnabled();
}

//*****************************************************************************
BOOL CWsfFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// Send it to the frame/view/doc stuff

	if (CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) 
		return true;

	// Send it to the pane
	// :TODO: do it better, this is not 100% certain

	CView* pView = GetActiveView();
	if (pView)
	{
		if (dynamic_cast<CWsfTabWnd*>(pView->GetParent()))
			if (pView->GetParent()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) 
				return true;
	}

	// Send it to the app plug-ins

	const CWsfPluginsArray& aPlugins = WsfGetApp()->GetPlugins();
	
	for (int i=aPlugins.GetSize(); i--;)
	{
		if (aPlugins[i]->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return true;
	}

   // Send it to the workspace doc

   CDocument* pDoc = WsfGetWorkspace();

   if (pDoc && (!pView || pView->GetDocument()!=pDoc))
   {
	   if (pDoc->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) 
		   return true;
   }

	// No handler 

	return false;
}

//*****************************************************************************
void CWsfFrame::OnWsfTreeView() 
{
	ASSERT(m_bLeftPaneVisible==0 || m_bLeftPaneVisible==1);
	m_bLeftPaneVisible ^= 1;
	ShowLeftPane(m_bLeftPaneVisible);
}

//*****************************************************************************
void CWsfFrame::OnUpdateWsfTreeView(CCmdUI* pCmdUI) 
{
	BOOL b = m_pLeftPane->IsCloseButtonVisible() && (m_pLeftPane->GetTabCount() > 0);
	pCmdUI->Enable(b);
	pCmdUI->SetCheck(b && m_bLeftPaneVisible);
}

//*****************************************************************************
void CWsfFrame::OnWsfWindowClose() 
{
   // Close the view.
   // When the view is removed from m_pRightPaneWnd and a new view is set as
   // active in m_pRightPaneWnd, m_pRightPaneWnd sends the WSF_TABWND_NM_SELCHANGED
   // message. In the handler SetActiveView is called.
   
   ASSERT(m_pRightPane->CanBeActiveWndClosed());
   
   CView* p = (CView*) m_pRightPane->GetActiveWnd();
   ASSERT_VALID(p);
   WsfGetApp()->CloseView(p, false);
}

//*****************************************************************************
void CWsfFrame::OnUpdateWsfWindowClose(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pRightPane->CanBeActiveWndClosed());
}

//*****************************************************************************
void CWsfFrame::OnWsfSelchanged(NMHDR *pNmhdr)
{
	ASSERT(pNmhdr);
   CWsfTabWnd::CSelChangedInfo& si = *(CWsfTabWnd::CSelChangedInfo*) pNmhdr;

	// If there is no doc view, activate folder list

	if (!si.pNewWnd && m_bLeftPaneVisible)
	{
		SetActiveView((CView*) m_pLeftPane->GetActiveWnd());
	}
	else SetActiveView((CView*) si.pNewWnd);

	// Update frame title

	OnUpdateFrameTitle(true);
}

//*****************************************************************************
void CWsfFrame::ChooseToolBars()
{
	HMENU hMenu;
	POINT pt;
	CControlBar *pcb;

	// Build list of all registered toolbars

	hMenu = CreatePopupMenu();
	ASSERT(hMenu);

	for (int i = 0; i<WsfGetApp()->m_aControlBars.GetSize(); i++)
	{
		pcb = WsfGetApp()->m_aControlBars[i];
		pcb->GetWindowText(temp, 1024);
		AppendMenu(hMenu, MF_STRING | (pcb->IsWindowVisible() ? MF_CHECKED:0), i+1, temp);
	}

	// Display context menu

	GetCursorPos(&pt);

	i = TrackPopupMenu(hMenu, 
		TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RIGHTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,
		pt.x, pt.y, 0, *this, NULL);

	if (i)
	{
		pcb = WsfGetApp()->m_aControlBars[i-1];
		ShowControlBar(pcb, !pcb->IsWindowVisible(), true);
	}

	// End

	DestroyMenu(hMenu);
}

//*****************************************************************************
BOOL CWsfFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR &s = *(NMHDR*) lParam;

	switch (s.code)
	{
   case CWsfTabWnd::CLOSE_BUTTON_CLICKED:
      switch (s.idFrom)
      {
      case WSF_MF_RIGHT_PANE:
			OnWsfWindowClose();
			break;
			
      case WSF_MF_LEFT_PANE:
			OnWsfTreeView();
			break;
      }
      break;

	case CWsfTabWnd::SEL_CHANGED:
      {
         CWsfTabWnd::CSelChangedInfo& si = *(CWsfTabWnd::CSelChangedInfo*)lParam;		

         switch (s.idFrom)
         {
         case WSF_MF_RIGHT_PANE:
				WsfGetApp()->OnTabWindowSelChange(false, NULL, (CView*)si.pNewWnd); 
				OnWsfSelchanged(&s);
				break;
				
      	case WSF_MF_LEFT_PANE:
				WsfGetApp()->OnTabWindowSelChange(true, NULL, (CView*)si.pNewWnd); 
				SetActiveView((CView*)si.pNewWnd);
				break;
         }
      }
      break;
     
	case NM_RCLICK:
		ChooseToolBars();
		break;

   default:
     	return CFrameWnd::OnNotify(wParam, lParam, pResult);
	}
	
   return true;
}

//*****************************************************************************
BOOL CWsfFrame::OnToolTipNeedTextW(UINT,NMHDR *,LRESULT *)
{
   // CFrameWnd handles this message so we must disable it
   // to be able to provide own tooltips
   return false;
}

//*****************************************************************************
BOOL CWsfFrame::OnToolTipNeedTextA(UINT, NMHDR* pNMHDR, LRESULT *)
{
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*) pNMHDR;

   if (!(pTTT->uFlags & TTF_IDISHWND))
   {
      const CWsfCmd* pCmd = WsfGetApp()->GetCmd(pTTT->hdr.idFrom);
      if (pCmd)
      {
			strncpy(pTTT->szText, pCmd->GetToolTip(), 78);
			pTTT->szText[79] = 0;
      }
   }

   return true;
}


//*****************************************************************************
void CWsfFrame::OnClose() 
{
	CWsfApp* pApp = WsfGetApp();

	// Save & close docs
 
	if (!pApp->SaveAllModified()) return;
	pApp->HideApplication();
	pApp->CloseAllDocuments(true);

	// there are cases where destroying the documents may destroy the
	//  main window of the application.
	if (!afxContextIsDLL && pApp->m_pMainWnd == NULL)
	{
		AfxPostQuitMessage(0);
		return;
	}

	// Save control bars

	SaveBarState(WSF_CONTROL_BARS_REG_KEY);

	// Destroy the window

	DestroyWindow();
}

//*****************************************************************************
void CWsfFrame::OnViewLargeicon() 
{
	WsfGetApp()->m_Setup.nListViewType = 0;
	WsfGetApp()->UpdateAllDocs(NULL, WSF_UV_LIST_VIEW_TYPE_CHANGED, NULL);
}

void CWsfFrame::OnViewList() 
{
	WsfGetApp()->m_Setup.nListViewType = LVS_LIST;
	WsfGetApp()->UpdateAllDocs(NULL, WSF_UV_LIST_VIEW_TYPE_CHANGED, NULL);
}

void CWsfFrame::OnViewSmallicon() 
{
	WsfGetApp()->m_Setup.nListViewType = LVS_SMALLICON;
	WsfGetApp()->UpdateAllDocs(NULL, WSF_UV_LIST_VIEW_TYPE_CHANGED, NULL);
}

void CWsfFrame::OnViewReport() 
{
	WsfGetApp()->m_Setup.nListViewType = LVS_REPORT;
	WsfGetApp()->UpdateAllDocs(NULL, WSF_UV_LIST_VIEW_TYPE_CHANGED, NULL);
}

void CWsfFrame::OnUpdateViewLargeicon(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(WsfGetApp()->m_Setup.nListViewType == 0);
}

void CWsfFrame::OnUpdateViewList(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(WsfGetApp()->m_Setup.nListViewType == LVS_LIST);
}

void CWsfFrame::OnUpdateViewSmallicon(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(WsfGetApp()->m_Setup.nListViewType == LVS_SMALLICON);
}

void CWsfFrame::OnUpdateViewReport(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(WsfGetApp()->m_Setup.nListViewType == LVS_REPORT);
}

//*****************************************************************************
void CWsfFrame::OnWindowCloseall()
{
	// Don't close workspace!
	if (WsfGetApp()->SaveAllModifiedOrdinaryDocs()) WsfGetApp()->CloseAllPaneViews(false);
}

//*****************************************************************************
void CWsfFrame::OnUpdateWindowCloseall(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_pRightPane->GetTabCount() > 0);
}

//*****************************************************************************
void CWsfFrame::OnNcRButtonDown(UINT nHitTest, CPoint point) 
{
	if (nHitTest == HTMENU) 
		ChooseToolBars();
	else
		CFrameWnd::OnNcRButtonDown(nHitTest, point);
}

//*****************************************************************************
void CWsfFrame::OnRButtonDown(UINT nFlags, CPoint point) 
{
	ChooseToolBars();
}

/*****************************************************************************
*
* WSF MESSAGES HANDLERS
*
*****************************************************************************/

//*****************************************************************************
LRESULT CWsfFrame::OnWsfWmDbHasChanged(WPARAM wParam, LPARAM lParam)
{
	CWsfDbEvent* s = (CWsfDbEvent*) wParam;
	ASSERT(s);

	WsfGetApp()->UpdateAllDocs(NULL, WSF_UV_DB_HAS_CHANGED, (CObject*) s);
	
	delete s;
	return 0;
}

/*****************************************************************************
*
* PANE CHANGING
*
*****************************************************************************/

//*****************************************************************************
void CWsfFrame::ChangePane(BOOL bDir)
{
   CView* p = GetActiveView() == (CView*)m_pLeftPane->GetActiveWnd() ? 
      (CView*)m_pRightPane->GetActiveWnd() : 
      (CView*)m_pLeftPane->GetActiveWnd();

   SetActiveView(p);
}

//*****************************************************************************
void CWsfFrame::OnWsfNextPane()
{
	ChangePane(true);
}

//*****************************************************************************
void CWsfFrame::OnUpdateWsfNextPane(CCmdUI* pCmdUI)
{
	BOOL b = m_pLeftPane->GetTabCount() && m_pRightPane->GetTabCount();
	pCmdUI->Enable(b);
}

//*****************************************************************************
void CWsfFrame::OnWsfPreviousPane()
{
	ChangePane(false);
}

//*****************************************************************************
void CWsfFrame::OnUpdateWsfPreviousPane(CCmdUI* pCmdUI)
{
	BOOL b = m_pLeftPane->GetTabCount() && m_pRightPane->GetTabCount();
	pCmdUI->Enable(b);
}

/*****************************************************************************
*
* MENU DRAWING
*
*****************************************************************************/

/*****************************************************************************/
void CWsfFrame::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	if (lpMeasureItemStruct->CtlType == ODT_MENU)
	{
		lpMeasureItemStruct->itemWidth = 10;
		lpMeasureItemStruct->itemHeight = 18;
	}
	CFrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

/*****************************************************************************/
void CWsfFrame::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (lpDrawItemStruct->CtlType != ODT_MENU) return;
	
	const CWsfCmd* pCmd = (CWsfCmd*) lpDrawItemStruct->itemData;
	ASSERT(pCmd);
	ASSERT(pCmd->m_nImgIdx != -1);

	CRect r(0,0,20,20);
	HDC hDC = lpDrawItemStruct->hDC;

	switch (lpDrawItemStruct->itemState & (ODS_SELECTED|ODS_CHECKED))
	{
	case ODS_SELECTED:
		FillRect(hDC, &r, (HBRUSH) GetSysColorBrush(COLOR_HIGHLIGHT));
		break;

	case ODS_CHECKED:
	case ODS_SELECTED | ODS_CHECKED:
		FillRect(hDC, &r, (HBRUSH) GetSysColorBrush(COLOR_HIGHLIGHT));
		FrameRect(hDC, &r, (HBRUSH) GetSysColorBrush(COLOR_3DSHADOW));
		break;
	}

   if (lpDrawItemStruct->itemState & ODS_GRAYED)
   {
      ImageList_Draw(*WsfGetApp()->GetGraySmallIconList(), pCmd->m_nImgIdx, hDC, 2, 2, ILD_NORMAL);
   }
	else
   {
      ImageList_Draw(*WsfGetApp()->GetSmallIconList(), pCmd->m_nImgIdx, hDC, 2, 2, ILD_NORMAL);
   }
}
