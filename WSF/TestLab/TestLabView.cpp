// TestLabView.cpp : implementation of the CTestLabView class
//

#include "stdafx.h"
#include "TestLab.h"

#include "TestLabDoc.h"
#include "TestLabView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestLabView

IMPLEMENT_DYNCREATE(CTestLabView, CView)

BEGIN_MESSAGE_MAP(CTestLabView, CView)
	//{{AFX_MSG_MAP(CTestLabView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestLabView construction/destruction

CTestLabView::CTestLabView()
{
	// TODO: add construction code here

}

CTestLabView::~CTestLabView()
{
}

BOOL CTestLabView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTestLabView drawing

void CTestLabView::OnDraw(CDC* pDC)
{
	CTestLabDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CTestLabView printing

BOOL CTestLabView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTestLabView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTestLabView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CTestLabView diagnostics

#ifdef _DEBUG
void CTestLabView::AssertValid() const
{
	CView::AssertValid();
}

void CTestLabView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestLabDoc* CTestLabView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestLabDoc)));
	return (CTestLabDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestLabView message handlers
