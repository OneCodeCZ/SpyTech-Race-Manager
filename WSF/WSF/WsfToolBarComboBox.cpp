///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 11
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfToolBarComboBox.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CWsfToolBarComboBox, CComboBox)
	//{{AFX_MSG_MAP(CWsfToolBarComboBox)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelchange)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CWsfToolBarComboBox, CComboBox)

//*****************************************************************************
CWsfToolBarComboBox::CWsfToolBarComboBox()
{
	m_pEdit = NULL;
}

//*****************************************************************************
CWsfToolBarComboBox::~CWsfToolBarComboBox()
{
	delete m_pEdit;
}

//*****************************************************************************
BOOL CWsfToolBarComboBox::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= CBS_DROPDOWN | CBS_SORT | CBS_AUTOHSCROLL;
	return CComboBox::PreCreateWindow(cs);
}

/*****************************************************************************/
BOOL CWsfToolBarComboBox::CToolEdit::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		m_pCombo->OnPostCommand();
		return true;
	}
	else return CEdit::PreTranslateMessage(pMsg);
}

//*****************************************************************************
int CWsfToolBarComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1) return -1;
	
	SendMessage(WM_SETFONT, (WPARAM) GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(false,0));
	
	// Subclass the combo-box's edit box to be able
	// to handle pressing of ther "enter" key.
   // NOTE: Only if the combo-box has an editable edit-box

   if ((GetStyle() & (CBS_DROPDOWN|CBS_DROPDOWNLIST)) == CBS_DROPDOWN)
   {
	   COMBOBOXINFO cbi;
	   cbi.cbSize = sizeof(cbi);
	   VERIFY(GetComboBoxInfo(*this, &cbi));  // MFC 8.0: remove *this

	   m_pEdit = new CToolEdit();
	   m_pEdit->m_pCombo = this;
      m_pEdit->SubclassWindow(cbi.hwndItem);
   }

	return true;
}

//*****************************************************************************
void CWsfToolBarComboBox::OnPostCommand()
{
	GetParent()->SendMessage(WM_COMMAND, GetWindowLong(*this, GWL_ID), 0);
}

//*****************************************************************************
void CWsfToolBarComboBox::OnSelchange() 
{
	OnPostCommand();
   
   // Return focus to the active view

   CView* pView = WsfGetFrame()->GetActiveView();
   if (pView) 
   {
      WsfGetFrame()->SetActiveView(NULL);
      WsfGetFrame()->SetActiveView(pView);
   }
}
