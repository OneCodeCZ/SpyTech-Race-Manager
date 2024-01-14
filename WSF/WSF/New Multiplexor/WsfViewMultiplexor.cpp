///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfViewMultiplexor.h"

BEGIN_MESSAGE_MAP(CWsfViewMultiplexor, CView)
	//{{AFX_MSG_MAP(CWsfViewMultiplexor)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfViewMultiplexor::CWsfViewMultiplexor()
{
	m_nActiveView = -1;
}

//*****************************************************************************
CWsfViewMultiplexor::~CWsfViewMultiplexor()
{
}

//*****************************************************************************
BOOL CWsfViewMultiplexor::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs)) return FALSE;

	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, 
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);

	return TRUE;
}

//*****************************************************************************
void CWsfViewMultiplexor::OnDraw(CDC* pDC)
{
	// No drawing necessary if a view is active

	if (m_nActiveView != -1) return;

	// Draw background and info

	pDC->FillSolidRect(0,0,m_nW,m_nH,GetSysColor(COLOR_3DFACE));
//	pDC->SetTextColor(RGB(0,0,0));
//	pDC->DrawText(m_sNoViewText, CRect(0,0,m_nW,m_nH), DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

//*****************************************************************************
void CWsfViewMultiplexor::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	m_nW = cx;
	m_nH = cy;

	if (m_nActiveView != -1) 
   {
		m_aViews[m_nActiveView]->SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);
   }
}

/*****************************************************************************/
void CWsfViewMultiplexor::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	if (bActivate)
   {
      if (m_nActiveView != -1) WsfGetFrame()->SetActiveView(m_aViews[m_nActiveView]);
   }
	else CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}


/*****************************************************************************
*
* VIEW MANAGEMENT
*
*****************************************************************************/

/*****************************************************************************/
int CWsfViewMultiplexor::GetViewIdx(CView* pView) const
{
	for (int i=m_aViews.GetSize(); i--;) 
   {
		if (m_aViews[i] == pView) return i;
   }

   return -1;
}

//*****************************************************************************
void CWsfViewMultiplexor::AddView(CView* pView, const CString& sTitle)
{
	ASSERT_VALID(pView);

	CCreateContext cc;
	cc.m_pCurrentDoc = GetDocument();
	cc.m_pNewViewClass = NULL;
	cc.m_pCurrentFrame = WsfGetFrame();
	cc.m_pLastView = NULL;
	cc.m_pNewDocTemplate = NULL;
	
	pView->Create(NULL, NULL, WS_CHILD, WsfZeroRect, this, 1, &cc);
	pView->SendMessage(0x0364, 0,0);			// = WM_INITIALUPDATE
   pView->SetWindowText(sTitle);
	m_aViews += pView;
}

//*****************************************************************************
void CWsfViewMultiplexor::SetActiveView(CView* pView)
{
   SetActiveView(pView ? GetViewIdx(pView) : -1);
}

//*****************************************************************************
void CWsfViewMultiplexor::SetActiveView(int nView)
{
	ASSERT(nView>=-1 && nView<m_aViews.GetSize());
	if (nView == m_nActiveView) return;

	if (nView >= 0)
	{
		if (m_nActiveView != -1) 
		{
			m_aViews[m_nActiveView]->ShowWindow(SW_HIDE);
			if (WsfGetFrame()->GetActiveView() == m_aViews[m_nActiveView]) 
				WsfGetFrame()->SetActiveView(m_aViews[nView]);
		}
		m_nActiveView = nView;

      CString s;
      m_aViews[nView]->GetWindowText(s);
      CWsfViewInfo* pvi = WsfGetApp()->GetViewInfo(this);
      WsfGetApp()->SetViewTitleIcon(pvi, s); 

		m_aViews[nView]->SetWindowPos(NULL,0,0,m_nW,m_nH,SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW);
   }
	else
	{
		if (m_nActiveView != -1) 
		{
			m_aViews[m_nActiveView]->ShowWindow(SW_HIDE);
			if (WsfGetFrame()->GetActiveView() == m_aViews[m_nActiveView]) WsfGetFrame()->SetActiveView(NULL);
		}
		m_nActiveView = -1;
	}
}

/*****************************************************************************/
void CWsfViewMultiplexor::RemoveView(CView* pView)
{
   RemoveView(GetViewIdx(pView));
}

//*****************************************************************************
void CWsfViewMultiplexor::RemoveView(int nIdx)
{
	ASSERT(nIdx>=0 && nIdx<m_aViews.GetSize());

	if (m_nActiveView > nIdx)
		m_nActiveView--;
	else if (m_nActiveView == nIdx)
	{
		SetActiveView(m_nActiveView ? 0 : 1);
		m_nActiveView = 0;
	}

	CView* p = m_aViews[nIdx];
	m_aViews.RemoveAt(nIdx);

	VERIFY(p->DestroyWindow());  // = delete p;
}








