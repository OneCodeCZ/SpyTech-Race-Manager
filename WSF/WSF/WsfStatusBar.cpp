///////////////////////////////////////////////////////////////////////////////
//
// SpyTech Application Framework 
// -----------------------------
// 
// Copyright (C)2000 Radek Tetik 
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CWsfStatusBar)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfStatusBar::CWsfStatusBar()
{
}

//*****************************************************************************
CWsfStatusBar::~CWsfStatusBar()
{
}

//*****************************************************************************
void CWsfStatusBar::OnSize(UINT nType, int cx, int cy) 
{
	RECT r;
	POSITION p;
	WORD wPane;
	CWnd *pWnd;

	CStatusBar::OnSize(nType, cx, cy);
	
	// Move all controls

	p = m_Map.GetStartPosition();

	while (p)
	{
		m_Map.GetNextAssoc(p, wPane, (void*&) pWnd);

		GetItemRect(wPane, &r);
		InflateRect(&r, -1, -1);

		pWnd->MoveWindow(&r);
	}
}

//*****************************************************************************
void CWsfStatusBar::OnDestroy() 
{
	POSITION p;
	WORD wPane;
	CWnd *pWnd;

	// Destroy all controls

	p = m_Map.GetStartPosition();

	while (p)
	{
		m_Map.GetNextAssoc(p, wPane, (void*&) pWnd);
		pWnd->DestroyWindow();
		delete pWnd;
	}

	m_Map.RemoveAll();

	CStatusBar::OnDestroy();
}

//*****************************************************************************
CStatic *CWsfStatusBar::SetPaneIcon(int nPane, HICON hIcon, int nWidth /*=-1*/)
{
	CStatic *pIcon;
	RECT r;

	ASSERT(hIcon && nPane >= 0);

	// Destroy previous control

	RemovePaneCtrl(nPane);

	// Get pane info

	if (nWidth != -1) SetPaneInfo(nPane, ID_SEPARATOR, SBPS_NORMAL, nWidth);
	GetItemRect(nPane, &r);
	InflateRect(&r, -1, -1);

	// Create the icon

	pIcon = new CStatic();
	ASSERT(pIcon);
	pIcon->Create(NULL, WS_CHILD | WS_VISIBLE |	SS_ICON | SS_CENTERIMAGE, r, this);
	pIcon->SetIcon(hIcon);

	m_Map.SetAt(nPane, pIcon);
	return pIcon;
}

//*****************************************************************************
CAnimateCtrl *CWsfStatusBar::SetPaneAnimation(int nPane, int nId, int nWidth /*=-1*/)
{
	CAnimateCtrl *pAnim;
	RECT r;

	ASSERT(nId > 0 && nPane >= 0);

	// Destroy previous control

	RemovePaneCtrl(nPane);

	// Get pane info

	if (nWidth != -1) SetPaneInfo(nPane, ID_SEPARATOR, SBPS_NORMAL, nWidth);
	GetItemRect(nPane, &r);
	InflateRect(&r, -1, -1);

	// Create the icon

	pAnim = new CAnimateCtrl();
	ASSERT(pAnim);
	pAnim->Create(WS_CHILD | WS_VISIBLE |	ACS_CENTER | ACS_TRANSPARENT | ACS_AUTOPLAY, r, this, 0);

  pAnim->SendMessage(ACM_OPEN, 
    (WPARAM) AfxFindResourceHandle(MAKEINTRESOURCE(nId), "AVI"),
    (LPARAM) MAKEINTRESOURCE(nId) );

	// End

	m_Map.SetAt(nPane, pAnim);
	return pAnim;
}

//*****************************************************************************
void CWsfStatusBar::RemovePaneCtrl(int nPane)
{
	CWnd *pWnd;

	if (m_Map.Lookup(nPane, (void*&)pWnd)) 
	{
		pWnd->DestroyWindow();
		delete pWnd;
		m_Map.RemoveKey(nPane);
	}
}






