///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2001 09 ??
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"

// Static class members
CWslWnd*	CWslWnd::m_pWndBeingCreated					= NULL;
BOOL			CWslWnd::m_bGenericClassRegistered	= false;
HHOOK			CWslWnd::m_hCreateWindowHook        = NULL;

/*****************************************************************************
*
* CREATE & DESTROY Stuff
*
*****************************************************************************/

//*****************************************************************************
CWslWnd::CWslWnd()
{
	_ASSERT(g_pApp);
	m_pfnDefWndProc = NULL;
	m_hWnd = NULL;
}

//*****************************************************************************
CWslWnd::CWslWnd(HWND hWnd)
{
	_ASSERT(g_pApp);

	m_pfnDefWndProc = NULL;
	m_hWnd = NULL;
	Attach(hWnd);
}

//*****************************************************************************
CWslWnd::~CWslWnd()
{
	if (m_hWnd)
	{
		_RPT0(0, "CWslWnd::~CWslWnd: Calling DestroyWindow(). \n"\
			"          OnDestroy in derived class will not be called.\n");
		DestroyWindow();
	}	
}

//*****************************************************************************
BOOL CWslWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!cs.lpszClass)
	{
		if (!m_bGenericClassRegistered)
		{
			WNDCLASSEX c;
			c.cbSize = sizeof(c);
			c.hIcon = NULL;
			c.hIconSm = NULL;
			c.hbrBackground = NULL;
			c.hInstance = g_pApp->m_hInst;
			c.lpszClassName = "WSL_UI_GENERIC_WNDPROC";
			c.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
			c.lpszMenuName = NULL;
			c.lpfnWndProc = ::DefWindowProc;
			c.cbWndExtra = 0;
			c.cbClsExtra = 0;
			c.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	
			if (!RegisterClassEx(&c)) return false;
			m_bGenericClassRegistered = true;
		}

		cs.lpszClass = "WSL_UI_GENERIC_WNDPROC";
	}

	return true;
}

//*****************************************************************************
LRESULT CALLBACK CWslWnd::CBTHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode != HCBT_CREATEWND) 
	{
		return CallNextHookEx(m_hCreateWindowHook, nCode, wParam, lParam);
	}

	// Attach hwnd to CWslWnd class 

	ASSERT(m_pWndBeingCreated);
	HWND hWnd = (HWND) wParam;
	ASSERT(hWnd);
//	_RPT2(0, "Creating window HWND=%d, CLASS=%d\n", hWnd, m_pWndBeingCreated);

	m_pWndBeingCreated->Attach(hWnd);
	m_pWndBeingCreated = NULL;

	// Unhook hook
	
	UnhookWindowsHookEx(m_hCreateWindowHook);
	m_hCreateWindowHook = NULL;
	return 0;
}

//*****************************************************************************
HHOOK CWslWnd::HookCreateWindow()
{
	ASSERT(!m_hCreateWindowHook);
	m_pWndBeingCreated = this;
	m_hCreateWindowHook = SetWindowsHookEx(WH_CBT, CBTHook, g_pApp->m_hInst, GetCurrentThreadId());
	ASSERT(m_hCreateWindowHook);
	return m_hCreateWindowHook;
}

//*****************************************************************************
BOOL CWslWnd::Create(CREATESTRUCT& cs)
{
	if (!PreCreateWindow(cs)) return false;
	
	HHOOK hHook = HookCreateWindow();

	HWND h = CreateWindowEx(cs.dwExStyle,cs.lpszClass,
		cs.lpszName,cs.style,
		cs.x,cs.y,cs.cx,cs.cy,
		cs.hwndParent,cs.hMenu,
		cs.hInstance,cs.lpCreateParams);

	ASSERT(h);				// Window creation failed
	_ASSERT(h == m_hWnd);

	return m_hWnd != NULL;
}

//*****************************************************************************
BOOL CWslWnd::Create(const char* pcszName, int nStyles, int nExStyles, 
										 RECT &r, CWslWnd* pParent, int nId)
{
	CREATESTRUCT cs;

	cs.x = r.left;
	cs.y = r.top;
	cs.cx = r.right - r.left;
	cs.cy = r.bottom - r.top;
	cs.style = nStyles;
	cs.dwExStyle = nExStyles;
	cs.hInstance = g_pApp->m_hInst;
	cs.hMenu = (HMENU) nId;
	cs.hwndParent = pParent ? *pParent : NULL;
	cs.lpCreateParams = NULL;
  cs.lpszClass = NULL;
	cs.lpszName = pcszName;

	return Create(cs);
}

//*****************************************************************************
BOOL CWslWnd::Create(const char* pcszName, int nStyles, int nExStyles,
										 int x, int y, int w, int h, CWslWnd* pParent, int nId)
{
	CREATESTRUCT cs;

	cs.x = x;
	cs.y = y;
	cs.cx = w;
	cs.cy = h;
	cs.style = nStyles;
	cs.dwExStyle = nExStyles;
	cs.hInstance = g_pApp->m_hInst;
	cs.hMenu = (HMENU) nId;
	cs.hwndParent = pParent ? *pParent : NULL;
	cs.lpCreateParams = NULL;
  cs.lpszClass = NULL;
	cs.lpszName = pcszName;

	return Create(cs);
}

//*****************************************************************************
BOOL CWslWnd::Create(CWslWnd* pParent /*= NULL*/, int nId /*= 0*/)
{
	CREATESTRUCT cs;

	cs.x = 0;
	cs.y = 0;
	cs.cx = 0;
	cs.cy = 0;
	cs.style = 0;
	cs.dwExStyle = 0;
	cs.hInstance = g_pApp->m_hInst;
	cs.hMenu = (HMENU) nId;
	cs.hwndParent = pParent ? pParent->m_hWnd : NULL;
	cs.lpCreateParams = NULL;
  cs.lpszClass = NULL;
	cs.lpszName = NULL;
	
	return Create(cs);
}

//*****************************************************************************
BOOL CWslWnd::DestroyWindow()
{
	_ASSERT(m_hWnd);
	BOOL b = ::DestroyWindow(m_hWnd);
	_ASSERT(!b || !m_hWnd);
	return b;
}

//*****************************************************************************
void CWslWnd::Attach(HWND hWnd)
{
	if (!hWnd) return;
	ASSERT(!m_hWnd);
	ASSERT(!g_pApp->GetWnd(hWnd)); // HWND has already a class

	m_hWnd = hWnd;
	g_pApp->AddWnd(this);

	// Get old wndproc, set new wndproc, remember old :-)

	WNDPROC pfnWndProc = (WNDPROC) GetWindowLong(hWnd, GWL_WNDPROC);
	ASSERT(pfnWndProc);
	ASSERT(pfnWndProc != __WndProc);
	m_pfnDefWndProc = pfnWndProc;
	SetWindowLong(hWnd, GWL_WNDPROC, (DWORD) &__WndProc);
}

//*****************************************************************************
void CWslWnd::Detach()
{
	if (!m_hWnd) return;
	ASSERT(g_pApp->GetWnd(m_hWnd)); // must have a class

	// Set old wndproc

	ASSERT(m_pfnDefWndProc);
	SetWindowLong(m_hWnd, GWL_WNDPROC, (DWORD) m_pfnDefWndProc);

	g_pApp->RemoveWnd(this);
	m_hWnd = NULL;
}

//*****************************************************************************
BOOL CWslWnd::OnCreate(CREATESTRUCT* pcs)
{
	return DefWndProc(WM_CREATE, 0, (LPARAM) pcs) == -1 ? false : true;
}

//*****************************************************************************
void CWslWnd::OnDestroy()
{
}

/*****************************************************************************
*
* COMMANDS & NOTIFY
*
*****************************************************************************/

//*****************************************************************************
BOOL CWslWnd::OnCommand(int nCmdId)
{
	return false;
}

//*****************************************************************************
DWORD CWslWnd::OnNotify(int nId, NMHDR* pData)
{
	return DefWndProc(WM_NOTIFY, (WPARAM) nId, (LPARAM) pData);
}

/*****************************************************************************
*
* MESSAGES HANDLING
*
*****************************************************************************/

//*****************************************************************************
int CWslWnd::DefWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	_ASSERT(m_hWnd); 
	_ASSERT(m_pfnDefWndProc);
	return ::CallWindowProc(m_pfnDefWndProc,m_hWnd, uMsg, wParam, lParam);
}

//*****************************************************************************
int CWslWnd::WndProc(int nMsg, DWORD wParam, DWORD lParam)
{
	int n;
	_ASSERT(m_hWnd);

	switch (nMsg)
	{
	case WM_CREATE:
		return OnCreate((CREATESTRUCT*) lParam) ? 0 : -1;

	case WM_DESTROY:
		OnDestroy();
		return DefWndProc(nMsg, wParam, lParam);

	case WM_NCDESTROY:
		// Last message sent, remove class-hwnd mapping
		n =  DefWndProc(nMsg, wParam, lParam);
		//_RPT2(0,"Destroying window HWND=%d, CLASS=%d\n", m_hWnd, this);
		g_pApp->RemoveWnd(this);
		m_hWnd = NULL;
		return n;

	case WM_COMMAND:
		if (HIWORD(wParam) > 1)
		{
			// Some messages are notifications, so process them in OnNotify
			NMHDR s;
			s.code = HIWORD(wParam);
			s.hwndFrom = (HWND) lParam;
			s.idFrom = LOWORD(wParam);
			OnNotify(LOWORD(wParam), &s);
		}
		else
		{
			if (!OnCommand(LOWORD(wParam)))
				return DefWndProc(nMsg, wParam, lParam);
		}
		return 0;

	case WM_NOTIFY:
		return OnNotify(wParam, (NMHDR*) lParam);

	default:
		return DefWndProc(nMsg, wParam, lParam);
	}
}

//*****************************************************************************
LRESULT CALLBACK CWslWnd::__WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWslWnd* pWnd;

	pWnd = g_pApp->GetWnd(hWnd);

	_ASSERT(pWnd);
//	_RPT4(0,"wndproc MSG=%d, HWND=%d, CLASS=%d, CLASS HWND=%d\n",uMsg, hWnd,pWnd,pWnd->m_hWnd);
	_ASSERT(pWnd->m_hWnd == hWnd);

	return pWnd ? pWnd->WndProc(uMsg, wParam, lParam) : 0;
}

/*****************************************************************************
*
* WINDOW MANAGEMENT
*
*****************************************************************************/

//*****************************************************************************
void CWslWnd::CenterWindow()
{
	RECT r,r2;

	HWND hDesktop = GetDesktopWindow();
	ASSERT(hDesktop);
	VERIFY(GetWindowRect(hDesktop, &r));
	VERIFY(GetWindowRect(m_hWnd, &r2));

	VERIFY(::SetWindowPos(m_hWnd, NULL,
		((r.right-r.left)-(r2.right-r2.left))/2,
		((r.bottom-r.top)-(r2.bottom-r2.top))/2,
		0,0,
		SWP_NOSIZE|SWP_NOZORDER));

}













