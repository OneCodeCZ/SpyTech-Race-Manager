///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 05
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfWaitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfWaitDialog, CWsfDialog)
//{{AFX_MSG_MAP(CWsfWaitDialog)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfWaitDialog::CWsfWaitDialog(int nIDD) : CWsfDialog(nIDD)
{
   //{{AFX_DATA_INIT(CWsfWaitDialog)
   //}}AFX_DATA_INIT
}

//*****************************************************************************
int CWsfWaitDialog::DoModal(void* pClass, 
                         WORKER_CALLBACK* pCallback, 
                         const char* pcszText,
                         bool bCanBeCanceled, 
                         DWORD dwAnimId)  
{
   CWsfWaitDialog d(bCanBeCanceled ? IDD_WAIT_DIALOG : IDD_WAIT_DIALOG2);
   
   d.m_pClass = pClass;
   d.m_pCallback = pCallback;
   d.m_bCanBeCanceled = bCanBeCanceled;
   d.m_sText = pcszText;
   d.m_dwAnimId = dwAnimId ? dwAnimId : IDR_GLOBE;
   
   return ((CWsfDialog&)d).DoModal();
}

//*****************************************************************************
BOOL CWsfWaitDialog::OnInitDialog() 
{
   CWsfDialog::OnInitDialog();
   
   // Init
   
   m_bCancelFlag = false;
   
   // Init dlg
   
   SetWindowText(AfxGetApp()->m_pszAppName);
   if (!m_sText.IsEmpty()) GetDlgItem(IDC_TEXT)->SetWindowText(m_sText);
   if (!m_bCanBeCanceled) GetDlgItem(IDCANCEL)->EnableWindow(false);
   
   // Init anim
   
   CAnimateCtrl *pAnim = (CAnimateCtrl*) GetDlgItem(IDC_ANIMATE);
   
   pAnim->SendMessage(ACM_OPEN, 
      (WPARAM) AfxFindResourceHandle(MAKEINTRESOURCE(m_dwAnimId), "AVI"),
      (LPARAM) MAKEINTRESOURCE(m_dwAnimId) );
   
   pAnim->Play(0,-1,-1);
   
   // Create thread
   
   m_pThread = AfxBeginThread( (AFX_THREADPROC) __ThreadEntry, this);
   VERIFY(m_pThread);
   
   // End
   
   return TRUE;
}

//*****************************************************************************
void CWsfWaitDialog::OnCancel() 
{
   ASSERT(m_bCanBeCanceled);
   m_bCancelFlag = true;
}

//*****************************************************************************
void CWsfWaitDialog::OnOK() 
{
   EndDialog(m_nResult);
}

/*****************************************************************************/
DWORD CWsfWaitDialog::__ThreadEntry(CWsfWaitDialog* p)
{
   p->DoIt();	
   return 0;
}

//*****************************************************************************
void CWsfWaitDialog::DoIt()
{
   // Init context
   // NOTE: This is another thread!
   
   CContext ctx(m_bCancelFlag);
   
   ctx.Progress.Attach(*GetDlgItem(IDC_PROGRESS));
   ctx.Text.Attach(*GetDlgItem(IDC_TEXT));
   
   // Call the user routine
   
   m_nResult = m_pCallback(m_pClass, ctx);
   
   // Deinit
   
   ctx.Progress.Detach();
   ctx.Text.Detach();
   
   // Quit the dialog
   
   PostMessage(WM_COMMAND, IDOK);
}


