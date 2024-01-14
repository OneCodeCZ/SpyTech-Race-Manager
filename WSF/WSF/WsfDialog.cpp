///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 04
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWsfDialog, CDialog)

BEGIN_MESSAGE_MAP(CWsfDialog, CDialog)
	//{{AFX_MSG_MAP(CWsfDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
CWsfDialog::CWsfDialog() : CDialog()
{
}

/*****************************************************************************/
CWsfDialog::CWsfDialog(int nTemplateId) : CDialog(nTemplateId)
{
}

/*****************************************************************************/
BOOL CWsfDialog::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CDialog::PreCreateWindow(cs)) return false;

//   cs.dwExStyle |= WS_EX_COMPOSITED;	

   return true;
}

/*****************************************************************************/
BOOL CWsfDialog::CreateAsControl(int nDialogTemplateId, 
                                 CWnd* pParentWnd, 
                                 int nPositionTemplateId)
{
   // Get the template window and its position (if any)
   
   CRect TemplatePosition;
   CWnd* pTemplateWnd;

   if (nPositionTemplateId)
   {
      ASSERT(pParentWnd);
      pTemplateWnd = pParentWnd->GetDlgItem(nPositionTemplateId);
      ASSERT(pTemplateWnd);
      pTemplateWnd->GetWindowRect(&TemplatePosition);
      pParentWnd->ScreenToClient(&TemplatePosition);
   }

   // Create the dialog

   if (!Create(nDialogTemplateId, pParentWnd)) return false;

   ASSERT_VALID(this);
   ASSERT(GetStyle() & WS_CHILD);

   DWORD dw = GetStyle();
   SetWindowLong(*this, GWL_STYLE, dw & ~(WS_DLGFRAME|WS_SYSMENU|WS_CAPTION));
   SetWindowLong(*this, GWL_ID, nPositionTemplateId);

   // Set the dialog's position and according the template (if any)

   if (nPositionTemplateId)
   {
      SetWindowLong(*this, GWL_ID, nPositionTemplateId);
      SetWindowPos(pTemplateWnd, 
         TemplatePosition.left, TemplatePosition.top,
         TemplatePosition.Width()+1, TemplatePosition.Height()+1,
         0);
      pTemplateWnd->DestroyWindow();
   }

   ShowWindow(SW_SHOW);
   return true;
}

/*****************************************************************************/
BOOL CWsfDialog::CreateAsControl(CWnd* pParentWnd, int nPositionTemplateId)
{
   ASSERT(m_lpszTemplateName);
   return CreateAsControl((int) m_lpszTemplateName, pParentWnd, nPositionTemplateId);
}

/*****************************************************************************/
int CWsfDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1) return -1;
	
	m_Resizer.SetParent(this);
	
	return 0;
}

/*****************************************************************************/
void CWsfDialog::OnSize(UINT nType, int cx, int cy) 
{
   m_Resizer.Update(cx, cy);
	CDialog::OnSize(nType, cx, cy);
}

/*****************************************************************************/
BOOL CWsfDialog::IsEditCtrl(CWnd* pWnd)
{
  if (pWnd)
  {
     ::GetClassName(*pWnd, temp, 1024);
     if (!stricmp(temp, "EDIT")) return true;
  }
  return false;
}

/*****************************************************************************/
void CWsfDialog::OnSetFocus(CWnd* pOldWnd) 
{
   CWnd* p = GetNextDlgTabItem(NULL);

   // NOTE: Edit-box assumed. However, the control must not be an edit-box
   // but it should be no problem.

	if (p) WsfPrepareForEdit(p);  
}

/*****************************************************************************/
void CWsfDialog::SetControlsReadOnly() 
{ 
   ASSERT_VALID(this);
   WsfSetDialogReadOnly(this); 
}

/*****************************************************************************/
void CWsfDialog::SetControlBoldFont(int nID) 
{ 
   ASSERT_VALID(this);
   WsfSetBoldFont(this, nID); 
}


/*****************************************************************************
*
* CLIPBOARD SUPPORT
*
*****************************************************************************/

/*****************************************************************************/
void CWsfDialog::OnEditCopy() 
{
  GetFocus()->SendMessage(WM_COPY);	
}

void CWsfDialog::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(IsEditCtrl(GetFocus()));	
}

/*****************************************************************************/
void CWsfDialog::OnEditCut() 
{
  GetFocus()->SendMessage(WM_CUT);	
}

void CWsfDialog::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(IsEditCtrl(GetFocus()));	
}

/*****************************************************************************/
void CWsfDialog::OnEditPaste() 
{
  GetFocus()->SendMessage(WM_PASTE);	
}

void CWsfDialog::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(IsEditCtrl(GetFocus()));	
}

/*****************************************************************************/
void CWsfDialog::OnEditSelectAll() 
{
   GetFocus()->SendMessage(EM_SETSEL, 0, -1);
}

void CWsfDialog::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(IsEditCtrl(GetFocus()));	
}

