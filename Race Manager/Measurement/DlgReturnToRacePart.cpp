// DlgReturnToRacePart.cpp : implementation file
//

#include "stdafx.h"
#include "main.h"
#include "DlgReturnToRacePart.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgReturnToRacePart dialog


CDlgReturnToRacePart::CDlgReturnToRacePart(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReturnToRacePart::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgReturnToRacePart)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgReturnToRacePart::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgReturnToRacePart)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgReturnToRacePart, CDialog)
	//{{AFX_MSG_MAP(CDlgReturnToRacePart)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgReturnToRacePart message handlers
