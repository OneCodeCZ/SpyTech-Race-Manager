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
#include <db\WslDb.h>
#include "WsfTableCtrl.h"
#include "WsfCmdUI.h"
#include "WsfTableView.h"
#include "WsfPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfTableView, CView)
	//{{AFX_MSG_MAP(CWsfTableView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CWsfTableView, CView)

//*****************************************************************************
CWsfTableView::CWsfTableView(CWsfTableCtrl2* pTbl)
{
   ASSERT(m_pTbl);
   m_pTbl = pTbl;
   m_pToolBar = NULL;
}

//*****************************************************************************
CWsfTableView::~CWsfTableView()
{
   delete m_pTbl;
   delete m_pToolBar;
}

/*****************************************************************************/
BOOL CWsfTableView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CView::PreCreateWindow(cs)) return false;

	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, NULL, NULL, NULL);
	return true;
}

//*****************************************************************************
int CWsfTableView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1) return -1;

	VERIFY(m_pTbl->CreateEx(0, NULL, NULL, 
      WS_CHILD | WS_VISIBLE | HDS_DRAGDROP, WsfZeroRect, this, 1001, NULL));

	return 0;
}

/*****************************************************************************/
void CWsfTableView::EnableToolBar(BOOL bEnable)
{
   ASSERT(m_hWnd);

   if (bEnable)
   {
      if (!m_pToolBar)
      {
         m_pToolBar = new CWsfToolBar();
         m_pToolBar->CreateControl(this, 0);
         m_pTbl->ModifyStyleEx(0,WS_EX_CLIENTEDGE);
         m_pToolBar->SetOwner(this);
         RecalcLayout();
      }
   }
   else
   {
      if (m_pToolBar)
      {
         m_pToolBar->DestroyWindow();
         delete m_pToolBar;
         m_pToolBar = NULL;
         m_pTbl->ModifyStyleEx(WS_EX_CLIENTEDGE,0);
         RecalcLayout();
      }
   }
}

//*****************************************************************************
void CWsfTableView::OnDestroy() 
{
	CView::OnDestroy();
}

//*****************************************************************************
void CWsfTableView::OnInitialUpdate() 
{
//   m_pTbl->ResetLayout();
}

/*****************************************************************************/
void CWsfTableView::OnSetFocus(CWnd* pOldWnd) 
{
	m_pTbl->SetFocus();
}

//*****************************************************************************
void CWsfTableView::OnDraw(CDC* pDC) 
{
}

/*****************************************************************************/
void CWsfTableView::RecalcLayout()
{
   int y = 0;

   if (m_pToolBar)
   {
      y = 30;
      m_pToolBar->MoveWindow(0,0,m_nW,y);
      m_pTbl->MoveWindow(-2,y,m_nW+2+2,m_nH-y+2);
   }
   else
   {
      m_pTbl->MoveWindow(0,0,m_nW,m_nH);
   }
}

//*****************************************************************************
void CWsfTableView::OnSize(UINT nType, int cx, int cy) 
{
   m_nW = cx;
   m_nH = cy;
   RecalcLayout();

	CView::OnSize(nType, cx, cy);
}

//*****************************************************************************
BOOL CWsfTableView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
   if (m_pTbl->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return true;
   return GetDocument()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//*****************************************************************************
void CWsfTableView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	TRACE1("CWsfTableView::OnUpdate(): lHint = %d\n", lHint);

	switch (lHint)
	{
	case WSF_UV_DB_HAS_CHANGED:
   	ASSERT(pHint);
      m_pTbl->ProcessDbEvent(*((CWsfDbEvent*)pHint));
      break;

	case CWsfTableView::SAVE_LAYOUT:
		m_pTbl->SaveLayout();
		break;

//	case CWsfTableView::RESET:
//		m_pTbl->ResetLayout();
//		break;

	case CWsfTableView::REFRESH:
		m_pTbl->Refresh();
		break;
	}
}	

/*****************************************************************************/
BOOL CWsfTableView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	switch (((NMHDR*)lParam)->code)
	{
   case TTN_NEEDTEXT:
      *pResult = WsfGetFrame()->SendMessage(WM_NOTIFY, wParam, lParam);
      return true;
	
   default:	
		return CView::OnNotify(wParam, lParam, pResult);
	}

	return true;
}
