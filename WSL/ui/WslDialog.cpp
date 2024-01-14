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

//*****************************************************************************
CWslDialog::CWslDialog()
{
}

//*****************************************************************************
CWslDialog::~CWslDialog()
{
}

//*****************************************************************************
int CWslDialog::DoModal(int nResId, CWslWnd* pParent)
{
	ASSERT(nResId > 0 && nResId <= 0xFFFF);
	
	HHOOK hHook = HookCreateWindow();

	int n = DialogBox(g_pApp->m_hInst, 
		MAKEINTRESOURCE(nResId),
		pParent ? pParent->m_hWnd : NULL, 
		(DLGPROC) __DlgProc);

	ASSERT(n!=-1);			// Creation failed
	return n;
}

//*****************************************************************************
int CWslDialog::DoModal(DLGTEMPLATE* pDlgTmp, CWslWnd* pParent)
{
	ASSERT(pDlgTmp);
	
	HHOOK hHook = HookCreateWindow();

	int n = DialogBoxIndirect(g_pApp->m_hInst, 
		pDlgTmp,
		pParent ? pParent->m_hWnd : NULL, 
		(DLGPROC) __DlgProc);

	ASSERT(n!=-1);			// Creation failed
	return n;
}

//*****************************************************************************
BOOL CWslDialog::Create(int nResId, CWslWnd* pParent)
{
	ASSERT(nResId > 0 && nResId <= 0xFFFF);

	HHOOK hHook = HookCreateWindow();

	HWND h = CreateDialog(g_pApp->m_hInst,
		MAKEINTRESOURCE(nResId),
		pParent ? pParent->m_hWnd : NULL,
		(DLGPROC) __DlgProc);

//	_RPT1(0,"%d\n", GetLastError());
	ASSERT(h == m_hWnd);
	return m_hWnd != NULL;
}

//*****************************************************************************
BOOL CWslDialog::Create(DLGTEMPLATE* pDlgTmp, CWslWnd* pParent)
{
	ASSERT(pDlgTmp);

	HHOOK hHook = HookCreateWindow();

	HWND h = CreateDialogIndirect(g_pApp->m_hInst,
		pDlgTmp,
		pParent ? pParent->m_hWnd : NULL,
		(DLGPROC) __DlgProc);

//	_RPT1(0,"%d\n", GetLastError());
	ASSERT(h == m_hWnd);
	return m_hWnd != NULL;
}

//*****************************************************************************
BOOL CWslDialog::EndDialog(int nResult)
{
	ASSERT(m_hWnd);
	return ::EndDialog(m_hWnd, nResult);
}

//*****************************************************************************
BOOL CWslDialog::OnInitDialog()
{
	return false;
}

/*****************************************************************************
*
* DDX
*
*****************************************************************************/

//*****************************************************************************
void CWslDialog::DoDataExchange(CWslDataExchange* pDX) throw (int)
{
}

//*****************************************************************************
void CWslDialog::CWslDataExchange::Fail(HWND hWnd, BOOL bEditCtrl) throw (int)
{
	ASSERT(hWnd);
	::SetFocus(hWnd);
	if (bEditCtrl) ::SendMessage(hWnd, EM_SETSEL, -1,0);
	throw 1;
}

//*****************************************************************************
BOOL CWslDialog::UpdateData(BOOL bSave)
{
	CWslDataExchange dx;

	ASSERT(m_hWnd);

	dx.bSave = bSave;
	dx.pDlg = this;
	
	try
	{
		DoDataExchange(&dx);
	}
	catch (int)
	{
		return false;
	}

	return true;
}

/*****************************************************************************
*
* MESSAGE HANDLING
*
*****************************************************************************/

//*****************************************************************************
BOOL CALLBACK CWslDialog::__DlgProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
	{
		CWslWnd* pWnd = g_pApp->GetWnd(hwndDlg);
		ASSERT(pWnd);
		CWslDialog* pDlg = (CWslDialog*) pWnd;
		return pDlg->OnInitDialog();
	}

	return 0;
}




