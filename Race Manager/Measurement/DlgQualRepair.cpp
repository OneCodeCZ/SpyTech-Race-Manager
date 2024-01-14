// DlgQualRepair.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgQualRepair.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgQualRepair dialog


CDlgQualRepair::CDlgQualRepair(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgQualRepair::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgQualRepair)
	m_dw1 = 0;
	m_dw2 = 0;
	//}}AFX_DATA_INIT
}


void CDlgQualRepair::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgQualRepair)
	DDX_Text(pDX, IDC_EDIT1, m_dw1);
	DDX_Text(pDX, IDC_EDIT2, m_dw2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgQualRepair, CDialog)
	//{{AFX_MSG_MAP(CDlgQualRepair)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgQualRepair message handlers
