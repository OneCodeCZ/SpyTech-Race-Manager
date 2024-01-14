///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2001 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfDocument.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfDocument, CDocument)
	//{{AFX_MSG_MAP(CWsfDocument)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfDocument::CWsfDocument()
{
}

//*****************************************************************************
CWsfDocument::~CWsfDocument()
{
}

//*****************************************************************************
CString CWsfDocument::GetTitle(BOOL bUseModifiedFlag)
{
	CString s;
	s = CDocument::GetTitle();
	if (bUseModifiedFlag && IsModified()) s += " *";
	return s;
}

//*****************************************************************************
void CWsfDocument::SetModifiedFlag(BOOL bModified /*= true*/)
{
	if (IsModified() == bModified) return;
	CDocument::SetModifiedFlag(bModified);

	// Update tab-window and frame title
	
	CWsfDocInfo* p = WsfGetApp()->GetDocInfo(this);
	if (p) WsfGetFrame()->m_pDocTabWnd->UpdateWndTitle(p->pView, GetTitle(true));

	WsfGetFrame()->OnUpdateFrameTitle(true);
}

//*****************************************************************************
void CWsfDocument::SetTitle(LPCTSTR lpszTitle)
{
	CDocument::SetTitle(lpszTitle);

	// Update tab-window
	
	CWsfDocInfo* p = WsfGetApp()->GetDocInfo(this);
	if (p) WsfGetFrame()->m_pDocTabWnd->UpdateWndTitle(p->pView, GetTitle(true));
}











