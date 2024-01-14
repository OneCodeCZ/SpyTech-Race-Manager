///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 02 25
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

BEGIN_MESSAGE_MAP(CWsfTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CWsfTreeCtrl)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfTreeCtrl::CWsfTreeCtrl()
{
}

//*****************************************************************************
CWsfTreeCtrl::~CWsfTreeCtrl()
{
}

//*****************************************************************************
void CWsfTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	NMHDR s;

	switch (nChar)
	{
	case VK_DELETE:
		s.code = WSF_TREECTRL_NM_DELETE;
		break;

	case VK_RETURN:
		s.code = WSF_TREECTRL_NM_ENTER;
		break;

	/*case VK_TAB:
		WsfGetFrame()->ActivatePane(true);
		break;*/

	default:
		CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	// Send notify

	s.hwndFrom = m_hWnd;
	s.idFrom = GetWindowLong(m_hWnd, GWL_ID);

	GetParent()->SendMessage(WM_NOTIFY, s.idFrom, (LPARAM) &s);		
}

//*****************************************************************************
void CWsfTreeCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == 13 || nChar==9) return;
	
	CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
}
