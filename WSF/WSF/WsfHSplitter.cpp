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
#include "WsfHSplitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SPLITTER_WIDTH		9

BEGIN_MESSAGE_MAP(CWsfHSplitter, CWnd)
	//{{AFX_MSG_MAP(CWsfHSplitter)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfHSplitter::CWsfHSplitter()
{
}

//*****************************************************************************
CWsfHSplitter::~CWsfHSplitter()
{
}

//*****************************************************************************
CWnd *CWsfHSplitter::SetPane(EPanes ePane, CWnd *pWnd, bool bRedraw /*= true*/)
{
	// Parent of the window must be this splitter!

	ASSERT(pWnd==NULL || pWnd->GetParent()->m_hWnd == m_hWnd);

	// Set pane
	
	CWnd *p = m_pPane[ePane];
	m_pPane[ePane] = pWnd;

	// Redraw

	if (bRedraw) RecalcLayout();
	return p;
}

//*****************************************************************************
void CWsfHSplitter::SetSplitterPos(int nY)
{
	ASSERT(nY >= 0);
	m_nSplitterY = nY;

	RecalcLayout();
	DrawSplitter();
}

//*****************************************************************************
BOOL CWsfHSplitter::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs)) return FALSE;

	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_OWNDC, 
		::LoadCursor(NULL, IDC_SIZENS), NULL, NULL);

	return TRUE;
}

//*****************************************************************************
int CWsfHSplitter::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd ::OnCreate(lpCreateStruct) == -1) return -1;
	
	// Init stuff

	m_pPane[0] = m_pPane[1] = NULL;
	m_bTracking = false;
	m_nSplitterY = 200;

	// Init DC

	m_hDC = ::GetDC(m_hWnd);
	CBrush* pBrush = CDC::GetHalftoneBrush();
	ASSERT(pBrush);
	SelectObject(m_hDC, pBrush->m_hObject);

	return 0;
}

//*****************************************************************************
void CWsfHSplitter::DrawSplitter()
{
	if (!m_pPane[0] || !m_pPane[1]) return;

	RECT r;
	int nMaxY = m_nWH - SPLITTER_WIDTH;

	r.top = m_nSplitterY > nMaxY ? nMaxY : m_nSplitterY;
	r.left = 0;
	r.right = m_nWW;
	r.bottom = r.top + SPLITTER_WIDTH;

   DrawEdge(m_hDC, &r, BDR_RAISEDINNER|BDR_RAISEDOUTER, BF_TOP|BF_BOTTOM|BF_MIDDLE);
}

//*****************************************************************************
void CWsfHSplitter::RecalcLayout()
{
	if (m_pPane[0] && m_pPane[1])
	{
		int nMaxY = m_nWH - SPLITTER_WIDTH;
		int nLH = m_nSplitterY > nMaxY ? nMaxY : m_nSplitterY;
		int nRY = nLH + SPLITTER_WIDTH;
		int nRH = m_nWH - nRY;

		m_pPane[0]->SetWindowPos(NULL, 0, 0, m_nWW, nLH, SWP_NOZORDER);
		m_pPane[1]->SetWindowPos(NULL, 0, nRY, m_nWW, nRH, SWP_NOZORDER);
	}
	else if (m_pPane[0] || m_pPane[1])
	{
		
		CWnd *pWnd = m_pPane[0] ? m_pPane[0] : m_pPane[1];
		pWnd->SetWindowPos(NULL, 0, 0, m_nWW, m_nWH, SWP_NOZORDER);
	}
}

//*****************************************************************************
void CWsfHSplitter::InvertTracker()
{
	PatBlt(m_hDC, 0, m_nSplitterY , m_nWW, SPLITTER_WIDTH, PATINVERT);
}

//*****************************************************************************
void CWsfHSplitter::OnPaint() 
{
	CPaintDC p(this);
	DrawSplitter();
}

//*****************************************************************************
void CWsfHSplitter::OnSize(UINT nType, int cx, int cy) 
{
	CWnd ::OnSize(nType, cx, cy);

	m_nWW = cx;
	m_nWH = cy;

	RecalcLayout();
}

//*****************************************************************************
void CWsfHSplitter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd ::OnLButtonDown(nFlags, point);

	if (!m_bTracking)
	{
		m_bTracking = true;
		SetCapture();
		InvertTracker();
	}
}

//*****************************************************************************
void CWsfHSplitter::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CWnd ::OnLButtonUp(nFlags, point);

	if (m_bTracking)
	{
		m_bTracking = false;
		ReleaseCapture();

		InvertTracker();
		RecalcLayout();
		DrawSplitter();
	}
}

//*****************************************************************************
void CWsfHSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bTracking)
	{
		// Erase previous ghost

		InvertTracker();

		// Set new tree width

		int nMaxY = m_nWH - SPLITTER_WIDTH;
		m_nSplitterY = point.y;
		if (m_nSplitterY < 0) m_nSplitterY = 0;
		if (m_nSplitterY > nMaxY) m_nSplitterY = nMaxY;
		
		// Draw new ghost

		InvertTracker();
	}	

	CWnd ::OnMouseMove(nFlags, point);
}
