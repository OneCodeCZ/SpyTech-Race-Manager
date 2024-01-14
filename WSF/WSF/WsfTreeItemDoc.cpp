///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2001 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfTreeItemDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CWsfTreeItemDoc, CDocument)
	//{{AFX_MSG_MAP(CWsfTreeItemDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
END_MESSAGE_MAP()

//*****************************************************************************
CWsfTreeItemDoc::CWsfTreeItemDoc()
{
}

//*****************************************************************************
CWsfTreeItemDoc::~CWsfTreeItemDoc()
{
}

//*****************************************************************************
void CWsfTreeItemDoc::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(false);
}
