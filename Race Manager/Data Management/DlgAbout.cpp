///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "DlgAbout.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CDlgAbout, CDialog)
	//{{AFX_MSG_MAP(CDlgAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CDlgAbout::CDlgAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAbout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAbout)
	//}}AFX_DATA_INIT
}

//*****************************************************************************
void CDlgAbout::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAbout)
	//}}AFX_DATA_MAP
}

//*****************************************************************************
BOOL CDlgAbout::OnInitDialog() 
{
	CString s;
	CDialog::OnInitDialog();

	DWORD dw = WsfGetDllVersion(WsfGetApp()->GetAppPath()+"dm.dll");
	s.Format("Race Manager %s", WsfFormatDllVersion(dw));
	GetDlgItem(IDC_TITLE)->SetWindowText(s);
	
	WsfSetBoldFont(this, IDC_TITLE);
	return TRUE;
}
