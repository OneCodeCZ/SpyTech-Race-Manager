///////////////////////////////////////////////////////////////////////////////
//
// CWsfFolderDialog
// ~~~~~~~~~~~~~~~~
// Copyright (C)2000 Radek Tetik / SpyTech
// Based on code from Mihai Filimon.
// Last updated: 2000-08-07
//
// Displays dialog for folder selection.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfFolderDialog.h"
#include <dlgs.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static WNDPROC g_wndOldProc;

//*****************************************************************************
static LRESULT CALLBACK WindowProcNew(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message ==  WM_COMMAND)
		if (HIWORD(wParam) == BN_CLICKED)
			if (LOWORD(wParam) == IDOK)
			{
				if (CFileDialog* pDlg = (CFileDialog*)CWnd::FromHandle(hwnd))
				{
					char szPath[MAX_PATH];
					GetCurrentDirectory(MAX_PATH, szPath);
					((CWsfFolderDialog*)pDlg->GetDlgItem(0))->m_strPath = szPath;
					pDlg->EndDialog(IDOK);
					return NULL;
				}
			}
	return CallWindowProc(g_wndOldProc, hwnd, message, wParam, lParam);
}


BEGIN_MESSAGE_MAP(CWsfFolderDialog, CFileDialog)
	//{{AFX_MSG_MAP(CWsfFolderDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfFolderDialog::CWsfFolderDialog(CString &strPath) 
	: CFileDialog(TRUE, NULL, _T("*..*")), m_strPath(strPath)
{
}

//*****************************************************************************
void CWsfFolderDialog::OnInitDone()
{
	CRect rcCancel, rcOk, rcList;
	CWnd* pFD = GetParent();

	// Hide not used controls

	HideControl(edt1);
	HideControl(stc3);
	HideControl(cmb1);
	HideControl(stc2);

	// Reposition of OK and resize of the list ctrl

	CString s((char*)IDS_CHOOSE_FOLDER);
	pFD->SetWindowText(s);

	pFD->GetDlgItem(IDCANCEL)->GetWindowRect(rcCancel);
	pFD->ScreenToClient(rcCancel);
	pFD->GetDlgItem(IDOK)->GetWindowRect(rcOk);
	pFD->ScreenToClient(rcOk);

	pFD->GetDlgItem(IDOK)->SetWindowPos(0, rcCancel.left - rcOk.Width() - 4, rcCancel.top, 0,0, SWP_NOZORDER | SWP_NOSIZE);
	SetControlText(IDOK, "OK");

	pFD->GetDlgItem(lst1)->GetWindowRect(rcList);
	pFD->ScreenToClient(rcList);
	pFD->GetDlgItem(lst1)->SetWindowPos(0,0,0,rcList.Width(), abs(rcList.top - (rcCancel.top - 8)), SWP_NOMOVE | SWP_NOZORDER);

	// Set new wndproc

	g_wndOldProc = (WNDPROC)SetWindowLong(pFD->m_hWnd, GWL_WNDPROC, (long)WindowProcNew);
	
	CFileDialog::OnInitDone();
}







