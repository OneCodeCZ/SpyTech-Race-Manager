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
#include "WslDialog.h"
#include "WslPropertyPage.h"
#include "WslPropertySheet.h"

//*****************************************************************************
CWslPropertyPage::CWslPropertyPage(int nDialogId)
{
	//{{AFX_DATA_INIT(CWslPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nDialogId = nDialogId;
}

//*****************************************************************************
CWslPropertyPage::~CWslPropertyPage()
{
}

//*****************************************************************************
void CWslPropertyPage::SetPageTitle(const char* pcszTitle)
{
	m_pps->SetPageTitle(this, pcszTitle);
}

//*****************************************************************************
BOOL CWslPropertyPage::OnKillActive(int& nNewPage) 
{
	return UpdateData(true);
}

//*****************************************************************************
BOOL CWslPropertyPage::OnSetActive() 
{
	return true;
}

//*****************************************************************************
BOOL CWslPropertyPage::OnOkButton() 
{
	int nNewPage = -1;
	return OnKillActive(nNewPage);
}

//*****************************************************************************
BOOL CWslPropertyPage::OnCancelButton() 
{
	return true;
}

//*****************************************************************************
void CWslPropertyPage::OnHelpButton() 
{
	WslMsgBox("No help available for this topic.", MB_ICONINFORMATION|MB_OK);
	return;
}

