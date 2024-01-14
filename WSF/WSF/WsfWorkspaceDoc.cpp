///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfWorkspaceDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CWsfWorkspaceDoc, CDocument)
	//{{AFX_MSG_MAP(CWsfWorkspaceDoc)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
CWsfWorkspaceDoc::CWsfWorkspaceDoc()
{
}

//*****************************************************************************
CWsfWorkspaceDoc::~CWsfWorkspaceDoc()
{
}

//*****************************************************************************
BOOL CWsfWorkspaceDoc::SaveModified() 
{
	ASSERT(IsModified());

	DWORD dw = AfxMessageBox(IDS_WORKSPACE_MODIFIED_SAVE, MB_ICONQUESTION|MB_YESNOCANCEL);
	switch (dw)
	{
	case IDYES: return DoSave(false);
	case IDNO: SetModifiedFlag(false); return true;
	}
	return false;
}

//*****************************************************************************
void CWsfWorkspaceDoc::SetTitle(LPCTSTR lpszTitle) 
{
	CDocument::SetTitle(lpszTitle);

	// Update frame title

	CWsfFrame *pFrame = WsfGetFrame();
	if (pFrame) pFrame->OnUpdateFrameTitle(true);
}

//*****************************************************************************
void CWsfWorkspaceDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
	// NOTE: Default implementation sets also the doc's title, but
	// don't want that.
	ASSERT(lpszPathName);
	m_strPathName = lpszPathName;
	if (bAddToMRU) WsfGetApp()->AddToRecentFileList(lpszPathName);
}

/*****************************************************************************/
void CWsfWorkspaceDoc::OnUpdateFileClose(CCmdUI* pCmdUI) 
{
	// The workspace cannot be close like a document
	pCmdUI->Enable(false);
}
