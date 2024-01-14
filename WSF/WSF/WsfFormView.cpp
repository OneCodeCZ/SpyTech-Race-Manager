///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 12
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfFormView.h"
#include "WsfTableCtrl2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfFormView, CFormView)
	//{{AFX_MSG_MAP(CWsfFormView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
   ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
CWsfFormView::CWsfFormView(int nDialogId) : CFormView(nDialogId)
{
	//{{AFX_DATA_INIT(CWsfFormView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

/*****************************************************************************/
CWsfFormView::~CWsfFormView()
{
}

/*****************************************************************************/
BOOL CWsfFormView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CFormView::PreCreateWindow(cs)) return false;

//   cs.dwExStyle |= WS_EX_COMPOSITED;	

   return true;
}

/*****************************************************************************/
int CWsfFormView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

	if (CFormView::OnCreate(lpCreateStruct) == -1) return -1;
   	
   m_Resizer.SetParent(this);
	return 0;
}

/*****************************************************************************/
LONG CWsfFormView::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
   UpdateDialogControls(WsfGetFrame(), (BOOL) wParam);
   return 0;
}

/*****************************************************************************/
void CWsfFormView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	m_Resizer.Update(cx,cy);
}

/*****************************************************************************/
BOOL CWsfFormView::PreTranslateMessage(MSG* pMsg) 
{
   // Allow to instances of the CWsfTableCtrl2 class to handle the ENTER key

   if (pMsg->message == WM_KEYDOWN && 
      pMsg->wParam==VK_RETURN &&
      dynamic_cast<CWsfTableCtrl2*>(CWnd::FromHandlePermanent(pMsg->hwnd)))
   {
      ASSERT(::IsChild(*this, pMsg->hwnd));
      return false;
   }

	return CFormView::PreTranslateMessage(pMsg);
}

/*****************************************************************************/
BOOL CWsfFormView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
   CWnd* pWnd = GetFocus();

   if (pWnd && IsChild(pWnd)) 
   {
      if (pWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return true;
   }

	return CFormView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/*****************************************************************************
*
* HELPERS
*
*****************************************************************************/

/*****************************************************************************/
BOOL CWsfFormView::IsEditCtrl(CWnd* pWnd)
{
  if (pWnd)
  {
     ::GetClassName(*pWnd, temp, 1024);
     if (!stricmp(temp, "EDIT")) return true;
  }
  return false;
}

/*****************************************************************************/
void CWsfFormView::HideControlBorders(int nCtrlId, BOOL bHideBottomBorder)
{
   ASSERT_VALID(this);

   CWnd* p = GetDlgItem(nCtrlId);
   ASSERT(p);
   if (!p) return;
 
   CRect r;
   p->GetWindowRect(&r);
   ScreenToClient(&r);
   r.InflateRect(2, 0, 2, bHideBottomBorder ? 2:0);
   
   p->MoveWindow(&r);
}

/*****************************************************************************
*
* EDIT COMMANDS
*
*****************************************************************************/

/*****************************************************************************/
void CWsfFormView::OnEditCopy() 
{
  GetFocus()->SendMessage(WM_COPY);	
}

void CWsfFormView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(IsEditCtrl(GetFocus()));	
}

/*****************************************************************************/
void CWsfFormView::OnEditCut() 
{
  GetFocus()->SendMessage(WM_CUT);	
}

void CWsfFormView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(IsEditCtrl(GetFocus()));	
}

/*****************************************************************************/
void CWsfFormView::OnEditPaste() 
{
  GetFocus()->SendMessage(WM_PASTE);	
}

void CWsfFormView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(IsEditCtrl(GetFocus()));	
}

/*****************************************************************************/
void CWsfFormView::OnEditSelectAll() 
{
   GetFocus()->SendMessage(EM_SETSEL, 0, -1);
}

void CWsfFormView::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(IsEditCtrl(GetFocus()));	
}


/*****************************************************************************
*
* TOOLTIPS
*
*****************************************************************************/

/*****************************************************************************/
BOOL CWsfFormView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	switch (((NMHDR*)lParam)->code)
	{
   case TTN_NEEDTEXT:
      *pResult = WsfGetFrame()->SendMessage(WM_NOTIFY, wParam, lParam);
      return true;
	
   default:	
		return CFormView::OnNotify(wParam, lParam, pResult);
	}

	return true;
}

