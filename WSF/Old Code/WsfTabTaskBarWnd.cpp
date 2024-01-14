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
#include "WsfTabTaskBarWnd.h"
#include <Shobjidl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfTabTaskBarWnd, CWnd)
	//{{AFX_MSG_MAP(CWsfTabTaskBarWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
CWsfTabTaskBarWnd::CWsfTabTaskBarWnd()
{
   m_pTabWnd = NULL;
   m_pWnd = NULL;
}

/*****************************************************************************/
CWsfTabTaskBarWnd::~CWsfTabTaskBarWnd()
{
}

/*****************************************************************************/
BOOL CWsfTabTaskBarWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs)) return false;

	cs.lpszClass = AfxRegisterWndClass(0, 
		NULL, NULL, (HICON)GetClassLong(*WsfGetFrame(), GCL_HICON));

   return true;
}

/*****************************************************************************/
BOOL CWsfTabTaskBarWnd::Create(CWsfTabWnd* pTabWnd, CWnd* pWnd)
{
   m_pTabWnd = pTabWnd;
   m_pWnd = pWnd;

   CString s;
   m_pWnd->GetWindowText(s);

   return CreateEx(WS_EX_APPWINDOW, NULL, s, WS_VISIBLE|WS_CAPTION|WS_SYSMENU,
      0,0,0,0,
      *m_pTabWnd, NULL, NULL);
}

/*****************************************************************************/
void CWsfTabTaskBarWnd::SetWnd(CWnd* pWnd)
{
   m_pWnd = pWnd;

   CString s;
   m_pWnd->GetWindowText(s);
   SetWindowText(s);

/*	ITaskbarList* pTaskbar;
	CoCreateInstance(CLSID_TaskbarList,0, CLSCTX_INPROC_SERVER,IID_ITaskbarList,(void**)&pTaskbar);
	pTaskbar->HrInit();
	pTaskbar->DeleteTab(*this);
	pTaskbar->AddTab(*this);
   pTaskbar->Release();*/
}

/*****************************************************************************/
int CWsfTabTaskBarWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	/*ITaskbarList* pTaskbar;
	CoCreateInstance(CLSID_TaskbarList,0, CLSCTX_INPROC_SERVER,IID_ITaskbarList,(void**)&pTaskbar);
	pTaskbar->HrInit();
	pTaskbar->AddTab(*this);
   pTaskbar->ActivateTab(*this);
   pTaskbar->Release();*/
	
	return 0;
}

/*****************************************************************************/
void CWsfTabTaskBarWnd::OnDestroy() 
{
	ITaskbarList* pTaskbar;
	CoCreateInstance(CLSID_TaskbarList,0, CLSCTX_INPROC_SERVER,IID_ITaskbarList,(void**)&pTaskbar);
	pTaskbar->HrInit();
	pTaskbar->DeleteTab(*this);
   pTaskbar->Release();
	
	CWnd::OnDestroy();
}

/*****************************************************************************/
void CWsfTabTaskBarWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);
	
	m_pTabWnd->SetActiveWnd(m_pWnd, true);

   ITaskbarList* pTaskbar;
	CoCreateInstance(CLSID_TaskbarList,0, CLSCTX_INPROC_SERVER,IID_ITaskbarList,(void**)&pTaskbar);
	pTaskbar->HrInit();
	pTaskbar->SetActiveAlt(*this);
   pTaskbar->Release();

}
