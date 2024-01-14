///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 03
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

BEGIN_MESSAGE_MAP(CWsfTIFolder, CWsfTreeItem)
	//{{AFX_MSG_MAP(CWsfTIFolder)
	ON_UPDATE_COMMAND_UI(ID_WSF_OPEN, OnUpdateWsfOpen)
	ON_COMMAND(ID_WSF_OPEN, OnWsfOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

WSL_IMPLEMENT_SERIAL_ABSTRACT(CWsfTIFolder, CWsfTreeItem, 1)

//*****************************************************************************
CWsfTIFolder::CWsfTIFolder()
{
	m_nViewID = 0;
}

/*****************************************************************************/
BOOL CWsfTIFolder::OnDelete(DWORD dwData)
{
	// If the item's view is opened, close it.
	CWsfViewInfo* pvi = WsfGetApp()->GetViewInfo(m_nViewID);
	if (pvi) return WsfGetApp()->CloseView(pvi->pView, false);
	return true;
}

//*****************************************************************************
void CWsfTIFolder::OnWsfOpen() 
{
	// If the item has already a view, activate it.

	if (WsfGetApp()->ActivateView(m_nViewID)) return;

	// Find the item's tree view and its document.
	// The new view will then use this document.
	// If the item is not in a tree view create a dummy document.
	
	BOOL bCreatedDoc;
	CDocument* pDoc;
	const CWslArray<CWsfDocInfo*>& aDocs = WsfGetApp()->GetDocInfo();

	for (int i=0; i<aDocs.GetSize(); i++)
	{
		for (int t=0; t<aDocs[i]->aViews.GetSize(); t++)
		{
			CWsfTreeItemTreeView* pView = dynamic_cast<CWsfTreeItemTreeView*>(aDocs[i]->aViews[t]->pView);
			if (pView && pView->GetRootItem() == this->GetRoot())
			{
				pDoc = pView->GetDocument();
				ASSERT(pDoc);
				bCreatedDoc = false;
				goto L1;
			}
		}
	}

	// Dummy document
	// CWsfTreeItemDoc has disabled the ID_FILE_SAVE command
	pDoc = new CWsfTreeItemDoc();
	WsfGetApp()->OpenDoc(pDoc);
	bCreatedDoc = true;

L1:
	// Open the tree item view
		
	CView* pView = new CWsfTreeItemListView(this);
	m_nViewID = WsfGetApp()->OpenView(pView, pDoc, GetPathTitle(), 
		false, true, -1, true, GetExpandedIconBitmapId(), GetExpandedIconIdx());

	ASSERT(m_nViewID);
}

//*****************************************************************************
void CWsfTIFolder::OnUpdateWsfOpen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}
