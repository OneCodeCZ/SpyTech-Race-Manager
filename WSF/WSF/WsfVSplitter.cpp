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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SPLITTER_WIDTH   5

BEGIN_MESSAGE_MAP(CWsfVSplitter, CWnd)
	//{{AFX_MSG_MAP(CWsfVSplitter)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfVSplitter::CWsfVSplitter()
{
}

//*****************************************************************************
CWsfVSplitter::~CWsfVSplitter()
{
}

//*****************************************************************************
CWnd *CWsfVSplitter::SetPane(EPanes ePane, CWnd *pWnd, bool bRedraw /*= true*/)
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
void CWsfVSplitter::SetSplitterPos(int nX)
{
	ASSERT(nX >= 0);
	m_nSplitterX = nX;

	RecalcLayout();
	DrawSplitter();
}

//*****************************************************************************
BOOL CWsfVSplitter::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs)) return FALSE;

	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS|CS_OWNDC, 
		::LoadCursor(NULL, IDC_SIZEWE), NULL, NULL);
   cs.lpszName = "CWsfVSplitter";

	return TRUE;
}

//*****************************************************************************
int CWsfVSplitter::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd ::OnCreate(lpCreateStruct) == -1) return -1;
	
	// Init stuff

	m_pPane[0] = m_pPane[1] = NULL;
	m_bTracking = false;
	m_nSplitterX = 300;

	// Init DC

	m_hDC = ::GetDC(m_hWnd);
	CBrush* pBrush = CDC::GetHalftoneBrush();
	ASSERT(pBrush);
	SelectObject(m_hDC, pBrush->m_hObject);

	return 0;
}

//*****************************************************************************
void CWsfVSplitter::DrawSplitter()
{
	if (!m_pPane[0] || !m_pPane[1]) return;

	RECT r;
	int nMaxX = m_nWW - SPLITTER_WIDTH;

	r.left = m_nSplitterX > nMaxX ? nMaxX : m_nSplitterX;
	r.top = 0;
	r.bottom = m_nWH;
	r.right = r.left + SPLITTER_WIDTH;

	FillRect(m_hDC, &r, (HBRUSH)(COLOR_3DFACE+1));
}

//*****************************************************************************
void CWsfVSplitter::RecalcLayout()
{
	if (m_pPane[0] && m_pPane[1])
	{
		int nMaxX = m_nWW - SPLITTER_WIDTH;
		int nLW = m_nSplitterX > nMaxX ? nMaxX : m_nSplitterX;
		int nRX = nLW + SPLITTER_WIDTH;
		int nRW = m_nWW - nRX;

		m_pPane[0]->SetWindowPos(NULL, 0, 0, nLW, m_nWH, SWP_NOZORDER);
		m_pPane[1]->SetWindowPos(NULL, nRX, 0, nRW, m_nWH, SWP_NOZORDER);
	}
	else if (m_pPane[0] || m_pPane[1])
	{
		
		CWnd *pWnd = m_pPane[0] ? m_pPane[0] : m_pPane[1];
		pWnd->SetWindowPos(NULL, 0, 0, m_nWW, m_nWH, SWP_NOZORDER);
	}
}

//*****************************************************************************
void CWsfVSplitter::InvertTracker()
{
	PatBlt(m_hDC, m_nSplitterX , 0, SPLITTER_WIDTH, m_nWH, PATINVERT);
}

//*****************************************************************************
void CWsfVSplitter::OnPaint() 
{
	CPaintDC p(this);
	DrawSplitter();
}

//*****************************************************************************
void CWsfVSplitter::OnSize(UINT nType, int cx, int cy) 
{
	CWnd ::OnSize(nType, cx, cy);

	m_nWW = cx;
	m_nWH = cy;

	RecalcLayout();
}

//*****************************************************************************
void CWsfVSplitter::OnLButtonDown(UINT nFlags, CPoint point) 
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
void CWsfVSplitter::OnLButtonUp(UINT nFlags, CPoint point) 
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
void CWsfVSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bTracking)
	{
		// Erase previous ghost

		InvertTracker();

		// Set new tree width

		int nMaxX = m_nWW - SPLITTER_WIDTH- 25;
		m_nSplitterX = point.x;
		if (m_nSplitterX < 25) m_nSplitterX = 25;
		if (m_nSplitterX > nMaxX) m_nSplitterX = nMaxX;
		
		// Draw new ghost

		InvertTracker();
	}	

	CWnd ::OnMouseMove(nFlags, point);
}
