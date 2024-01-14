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
#include "WsfPropertyPage.h"
#include "WsfPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfPropertyPage, CWsfDialog)
	//{{AFX_MSG_MAP(CWsfPropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CWsfPropertyPage, CWsfDialog)

//*****************************************************************************
CWsfPropertyPage::CWsfPropertyPage(int nDialogId)
{
	m_nDialogId = nDialogId;
}


/*****************************************************************************/
BOOL CWsfPropertyPage::Create(CWsfPropertySheet* pps)
{
   ASSERT(m_pps);
   m_pps = pps;
   return CWsfDialog::CreateAsControl(m_nDialogId, pps, 0);
}

//*****************************************************************************
BOOL CWsfPropertyPage::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
   CWnd* pWnd = GetFocus();

   if (pWnd && IsChild(pWnd)) 
   {
      if (pWnd->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) return true;
   }

	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

//*****************************************************************************
void CWsfPropertyPage::SetPageTitle(const char* pcszTitle)
{
	m_pps->SetPageTitle(this, pcszTitle);
}

/*****************************************************************************/
void CWsfPropertyPage::HideControlBorders(int nId, BOOL bBottom)
{
   CWnd* p = GetDlgItem(nId);
   ASSERT(p);
 
   CRect r;
   p->GetWindowRect(&r);
   ScreenToClient(&r);
   r.InflateRect(2,0,2,2);
   
   p->MoveWindow(&r);
}

/*****************************************************************************/
CWsfPropertySheet* CWsfPropertyPage::GetSheet() const
{ 
   ASSERT_VALID(m_pps); 
   return m_pps; 
}

/*****************************************************************************/
BOOL CWsfPropertyPage::IsPageActive() const
{
   ASSERT_VALID(m_pps);
   return m_pps->GetPage(m_pps->GetActivePage()) == this;
}

//*****************************************************************************
BOOL CWsfPropertyPage::OnKillActive(int& nNewPage) 
{
	return UpdateData();
}

//*****************************************************************************
BOOL CWsfPropertyPage::OnSetActive() 
{
	return true;
}

//*****************************************************************************
BOOL CWsfPropertyPage::OnOkButton() 
{
   return true;
}

//*****************************************************************************
BOOL CWsfPropertyPage::OnCancelButton() 
{
	return true;
}

//*****************************************************************************
void CWsfPropertyPage::OnHelpButton() 
{
	AfxMessageBox(IDS_HELP_NOT_AVAILABLE, MB_ICONINFORMATION|MB_OK);
	return;
}

//*****************************************************************************
void CWsfPropertyPage::OnCancel() 
{
}

//*****************************************************************************
void CWsfPropertyPage::OnOK() 
{
}
