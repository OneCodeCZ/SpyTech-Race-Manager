///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 12
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WslDialog.h"

CWslApp* g_pApp = NULL;
char wsltemp[1024], wsltemp2[1024];

int			CWslApp::m_nWaitCursorRef;
HCURSOR	CWslApp::m_hWaitCursor;
HCURSOR	CWslApp::m_hOldCursor;

//*****************************************************************************
CWslApp::CWslApp()
{
	g_pApp = this;
	m_hBoldDlgFont = NULL;
   m_hUnderlinedDlgFont = NULL;
	m_hMainWnd = NULL;
	m_nWaitCursorRef = 0;
	m_hWaitCursor = 0;
}

//*****************************************************************************
CWslApp::~CWslApp()
{
	if (m_hBoldDlgFont) ::DeleteObject(m_hBoldDlgFont);
	if (m_hUnderlinedDlgFont) ::DeleteObject(m_hUnderlinedDlgFont);
}

/*****************************************************************************
*
* APP START-UP
*
*****************************************************************************/

//*****************************************************************************
BOOL CWslApp::OnInitInstance(HINSTANCE hInst, const char* pcszCmdLine)
{
	_ASSERT(hInst);
	m_hInst = hInst;

	VERIFY(m_hWaitCursor = ::LoadCursor(NULL, IDC_WAIT));

	// Get appllication path

	VERIFY(::GetModuleFileName((HMODULE) hInst, wsltemp, 1024));
	ASSERT(wsltemp[0]);
	char* p = strrchr(wsltemp, '\\');
	ASSERT(p);
	p[1] = 0;
	m_sAppPath = wsltemp;
	_RPT1(0,"CWslApp::OnInitInstance: apppath=%s\n", (const char*)m_sAppPath);

	return true;
}

//*****************************************************************************
void CWslApp::OnExitInstance()
{
}

//*****************************************************************************
void CWslApp::Run()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!PreTranslateMessage(msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

//*****************************************************************************
BOOL CWslApp::PreTranslateMessage(MSG& m)
{
//	switch (m.message)
	{
/*	case WM_SETCURSOR:
		if (m_nWaitCursorRef > 0)
		{
			::SetCursor(m_hWaitCursor);
			return true;
		}
		break;*/
	}
	return false;
}

/*****************************************************************************
*
* Window Management
*
*****************************************************************************/

//*****************************************************************************
void CWslApp::AddWnd(CWslWnd* pWnd)
{
	_ASSERT(pWnd);
	_ASSERT(pWnd->m_hWnd);
	_ASSERT(GetWnd(pWnd->m_hWnd) == NULL);

	m_aWnds.GrowBy(1);
	CWndInfo& wi = m_aWnds.GetLast();

	wi.hWnd = pWnd->m_hWnd;
	wi.pWnd = pWnd;
}

//*****************************************************************************
void CWslApp::RemoveWnd(CWslWnd* pWnd)
{
	_ASSERT(pWnd);
	
	for (int i=m_aWnds.GetSize(); i--;)
	{
		if (m_aWnds[i].pWnd == pWnd)
		{
			ASSERT(pWnd->m_hWnd == m_aWnds[i].hWnd);
			m_aWnds.RemoveAt(i);
//			_RPT1(0,"Size = %d\n", m_aWnds.GetSize());
			break;
		}
	}
}

//*****************************************************************************
CWslWnd* CWslApp::GetWnd(HWND hWnd)
{
	_ASSERT(hWnd);
	
	for (int i=m_aWnds.GetSize(); i--;)
	{
		if (m_aWnds[i].hWnd == hWnd) 
		{
			ASSERT(m_aWnds[i].pWnd->m_hWnd == hWnd);
			return m_aWnds[i].pWnd;
		}
	}
	return NULL;
}

/*****************************************************************************
*
* CWslWaitCursor
*
*****************************************************************************/

//*****************************************************************************
CWslWaitCursor::CWslWaitCursor()
{
	CWslApp::m_nWaitCursorRef++;
	CWslApp::m_hOldCursor = ::SetCursor(CWslApp::m_hWaitCursor);
}

//*****************************************************************************
CWslWaitCursor::~CWslWaitCursor()
{
	CWslApp::m_nWaitCursorRef--;
	ASSERT(CWslApp::m_nWaitCursorRef >= 0);

	if (!CWslApp::m_nWaitCursorRef)
	{
		::SetCursor(CWslApp::m_hOldCursor);
	}
}

/*****************************************************************************
*
* VARIOUS
*
*****************************************************************************/

//*****************************************************************************
void CWslApp::SetBoldFont(CWslDialog* pDlg, int nChildId)
{
	ASSERT(pDlg);

	HWND hWnd = pDlg->GetDlgItem(nChildId);
	ASSERT(hWnd);			// child doesn't exist!

	// Create bold font if it doesn't exist

  if (!m_hBoldDlgFont)
  {
    LOGFONT lf;
		HFONT hFont = (HFONT) ::SendMessage(hWnd, WM_GETFONT, 0,0);
		ASSERT(hFont);

    VERIFY(::GetObject(hFont, sizeof(lf), &lf));
    lf.lfWeight = 1000;		
    m_hBoldDlgFont = ::CreateFontIndirect(&lf);
		ASSERT(m_hBoldDlgFont);
  }
  
	// Set font

	::SendMessage(hWnd, WM_SETFONT, (WPARAM) m_hBoldDlgFont, 0);
}

//*****************************************************************************
void CWslApp::SetUnderlinedFont(CWslDialog* pDlg, int nChildId)
{
   ASSERT(pDlg);
   
   HWND hWnd = pDlg->GetDlgItem(nChildId);
   ASSERT(hWnd);			// child doesn't exist!
   
   // Create bold font if it doesn't exist
   
   if (!m_hUnderlinedDlgFont)
   {
      LOGFONT lf;
      HFONT hFont = (HFONT) ::SendMessage(hWnd, WM_GETFONT, 0,0);
      ASSERT(hFont);
      
      VERIFY(::GetObject(hFont, sizeof(lf), &lf));
      lf.lfUnderline = true;
      m_hUnderlinedDlgFont = ::CreateFontIndirect(&lf);
      ASSERT(m_hUnderlinedDlgFont);
   }
   
   // Set font
   
   ::SendMessage(hWnd, WM_SETFONT, (WPARAM) m_hUnderlinedDlgFont, 0);
}
