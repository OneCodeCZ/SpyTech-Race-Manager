///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 10
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfHtmlDoc.h"
#include "WsfHtmlView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfHtmlDoc, CDocument)
	//{{AFX_MSG_MAP(CWsfHtmlDoc)
	ON_COMMAND(ID_WSF_REFRESH, OnWsfRefresh)
	ON_UPDATE_COMMAND_UI(ID_WSF_REFRESH, OnUpdateWsfRefresh)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*****************************************************************************/
CWsfHtmlDoc::CWsfHtmlDoc()
{
}

/*****************************************************************************/
CWsfHtmlDoc::~CWsfHtmlDoc()
{
}

/*****************************************************************************/
#ifdef _DEBUG
void CWsfHtmlDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWsfHtmlDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/*****************************************************************************
*
* COMMANDS
*
*****************************************************************************/

/*****************************************************************************/
void CWsfHtmlDoc::OnWsfRefresh() 
{
	POSITION p = GetFirstViewPosition();
	while (p)
	{
		CHtmlView* pView = dynamic_cast<CWsfHtmlView*>(GetNextView(p));
		if (pView) pView->Refresh();
	}
}

/*****************************************************************************/
void CWsfHtmlDoc::OnUpdateWsfRefresh(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

/*****************************************************************************/
void CWsfHtmlDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(false);
}
