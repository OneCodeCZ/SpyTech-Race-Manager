///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// m_dwFlag values
#define MOVE_X			1
#define MOVE_Y			2
#define RESIZE_X		4
#define RESIZE_Y		8

//*****************************************************************************
CWsfChildrenResizer::CWsfChildrenResizer()
{
	m_hParent = NULL;
}

//*****************************************************************************
CWsfChildrenResizer::~CWsfChildrenResizer()
{
}

//*****************************************************************************
void CWsfChildrenResizer::SetParent(CWnd* pWnd)
{
	RECT r;
	ASSERT(pWnd);

	m_hParent = pWnd->m_hWnd;

	GetWindowRect(m_hParent, &r);
	m_nMinPW = r.right - r.left;
	m_nMinPH = r.bottom - r.top;
}

/*****************************************************************************/
void CWsfChildrenResizer::Add(HWND hWnd, 
                              float fMoveX, float fResizeX, 
                              float fMoveY, float fResizeY)
{
	ASSERT(m_hParent);
	ASSERT(hWnd);

   // Get window pos & size within its parent

   RECT r1, r2;
	GetWindowRect(hWnd, &r1);
	GetWindowRect(m_hParent, &r2);

	int nX = r1.left - r2.left;
	int nY = r1.top - r2.top;
	int nW = r1.right - r1.left;
	int nH = r1.bottom - r1.top;

   // Add window

	CChildInfo* p = new CChildInfo();
	m_aWnds += p;
   
   p->Construct(hWnd,
		nX, nY, nW, nH, 
		fMoveX, fResizeX, fMoveY, fResizeY);

}

//*****************************************************************************
void CWsfChildrenResizer::Add(int nId, float fMoveX, float fResizeX, 
                              float fMoveY, float fResizeY)
{
	ASSERT(m_hParent);
   Add(::GetDlgItem(m_hParent, nId), fMoveX, fResizeX, fMoveY, fResizeY);
}

/*****************************************************************************/
void CWsfChildrenResizer::Remove(HWND hWnd)
{
   for (int i=0; i<m_aWnds.GetSize(); i++)
   {
      if (m_aWnds[i]->GetWnd() == hWnd) 
      {
         m_aWnds.RemoveAt(i);
         return;
      }
   }
}

//*****************************************************************************
void CWsfChildrenResizer::Update(int nParentW, int nParentH)
{
	for (int i = m_aWnds.GetSize(); i--;) 
	{
		ASSERT(m_hParent);
		m_aWnds[i]->Update(m_nMinPW, m_nMinPH, nParentW, nParentH);
	}
}

/*****************************************************************************/
void CWsfChildrenResizer::Update()
{
   CRect r;
   GetClientRect(m_hParent, &r);
   Update(r.Width(), r.Height());
}

/*****************************************************************************
*
* CChildInfo
*
*****************************************************************************/

/*****************************************************************************/
CWsfChildrenResizer::CChildInfo::CChildInfo()
{
   m_hWnd = NULL;
}

//*****************************************************************************
void CWsfChildrenResizer::CChildInfo::Construct(HWND hWnd, 
                                                int nX, int nY, int nW, int nH,
                                                float fMoveX, float fResizeX,
                                                float fMoveY, float fResizeY)
{
	ASSERT(!m_hWnd);
	ASSERT(hWnd);

	m_dwFlags = 0;
	m_hWnd = hWnd;
	m_nX = nX; m_nY = nY;
	m_nW = nW; m_nH = nH;
	
	m_fMoveX = fMoveX; m_fResizeX = fResizeX;
	m_fMoveY = fMoveY; m_fResizeY = fResizeY;

	if (fMoveX) m_dwFlags |= MOVE_X;
	if (fMoveY) m_dwFlags |= MOVE_Y;
	if (fResizeX)	m_dwFlags |= RESIZE_X;
	if (fResizeY) m_dwFlags |= RESIZE_Y;
}

//*****************************************************************************
void CWsfChildrenResizer::CChildInfo::Update(int nMinPW, int nMinPH, int nPW, int nPH)
{
	int nX, nY, nW, nH;

	nX = m_nX; nY = m_nY;
	nW = m_nW; nH = m_nH;

	// Limits

	if (nPW < nMinPW) nPW = nMinPW;
	if (nPH < nMinPH) nPH = nMinPH;

	// Calculate new values

	if (m_dwFlags & MOVE_X)    nX += (int)((float)(nPW - nMinPW) * m_fMoveX);
	if (m_dwFlags & RESIZE_X)  nW += (int)((float)(nPW - nMinPW) * m_fResizeX);
	if (m_dwFlags & MOVE_Y)	   nY += (int)((float)(nPH - nMinPH) * m_fMoveY);
	if (m_dwFlags & RESIZE_Y)	 nH += (int)((float)(nPH - nMinPH) * m_fResizeY);

	SetWindowPos(m_hWnd, NULL, nX, nY, nW, nH, SWP_NOZORDER);
}


























