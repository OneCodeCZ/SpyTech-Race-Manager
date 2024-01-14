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
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CWsfListCtrl)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfListCtrl::CWsfListCtrl()
{
}

//*****************************************************************************
CWsfListCtrl::~CWsfListCtrl()
{
}

//*****************************************************************************
void CWsfListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	NMHDR s;

	switch (nChar)
	{
	case VK_DELETE:
		s.code = KEY_DELETE;
		break;

	case VK_BACK:
		s.code = KEY_BACKSPACE;
		break;

	case VK_RETURN:
		s.code = KEY_ENTER;
		break;

	default:
		CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	// Send notify

	s.hwndFrom = m_hWnd;
	s.idFrom = GetWindowLong(m_hWnd, GWL_ID);

	GetParent()->SendMessage(WM_NOTIFY, s.idFrom, (LPARAM) &s);		
}

//*****************************************************************************
void CWsfListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar != 13) CListCtrl::OnChar(nChar, nRepCnt, nFlags);
}

/*****************************************************************************/
UINT CWsfListCtrl::OnGetDlgCode() 
{
   return CListCtrl::OnGetDlgCode();
}
