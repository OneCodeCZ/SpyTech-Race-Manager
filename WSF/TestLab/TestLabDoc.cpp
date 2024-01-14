// TestLabDoc.cpp : implementation of the CTestLabDoc class
//

#include "stdafx.h"
#include "TestLab.h"

#include "TestLabDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestLabDoc

IMPLEMENT_DYNCREATE(CTestLabDoc, CDocument)

BEGIN_MESSAGE_MAP(CTestLabDoc, CDocument)
	//{{AFX_MSG_MAP(CTestLabDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestLabDoc construction/destruction

CTestLabDoc::CTestLabDoc()
{
	// TODO: add one-time construction code here

}

CTestLabDoc::~CTestLabDoc()
{
}

BOOL CTestLabDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTestLabDoc serialization

void CTestLabDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTestLabDoc diagnostics

#ifdef _DEBUG
void CTestLabDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTestLabDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestLabDoc commands
