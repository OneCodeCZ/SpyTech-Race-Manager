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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTIRSFolder, CWsfTINamedFolder)
	//{{AFX_MSG_MAP(CTIRSFolder)
	ON_COMMAND(ID_NEW_RS, OnNewRs)
	ON_UPDATE_COMMAND_UI(ID_NEW_RS, OnUpdateNewRs)
	ON_COMMAND(ID_WSF_OPEN, OnWsfOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CTIRSFolder, CWsfTINamedFolder, 1)
		
//*****************************************************************************
CTIRSFolder::CTIRSFolder()
{
	Init(IDR_CONTEXT_MENUS,4, IDB_SMALL_ICONS,3, IDB_SMALL_ICONS,3, 0);
}

//*****************************************************************************
CTIRSFolder::CTIRSFolder(const char *pcszTitle)
	: CWsfTINamedFolder(pcszTitle)
{
	Init(IDR_CONTEXT_MENUS,4, IDB_SMALL_ICONS,3, IDB_SMALL_ICONS,3, 0);
}

//*****************************************************************************
CTIRS *CTIRSFolder::FindRS(CString &strName)
{
	CTIRS *pItem = (CTIRS*) GetChild();

	while (pItem)
	{
		if (strName == pItem->GetTitle()) return pItem;
		pItem = (CTIRS*) pItem->GetNextSibling();
	}

	return NULL;
}

//*****************************************************************************
int CTIRSFolder::OnCompare(CWsfTreeItem *pItem1, CWsfTreeItem *pItem2)
{
	return strcmp(pItem1->GetTitle(),pItem2->GetTitle());
}

//*****************************************************************************
void CTIRSFolder::OnNewRs() 
{
	CTIRS *pItem = new CTIRS();

	if (pItem->DisplayProperties(true) != IDOK)
	{
		delete pItem;
		return;
	}

	AddChild(pItem);
	WsfGetApp()->TreeHasChanged(pItem, CWsfApp::ETreeChanges::ITEM_ADDED);
}

//*****************************************************************************
void CTIRSFolder::OnUpdateNewRs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

//*****************************************************************************
void CTIRSFolder::OnWsfOpen() 
{
	CWsfViewInfo* pvi = WsfGetCApp()->GetViewInfo(WsfGetCApp()->m_nDbTreeItemViewId);

	if (pvi)
	{
		CWsfTreeItemListView* pView = (CWsfTreeItemListView*) pvi->pView;
		pView->SetTreeItem(this);
		WsfGetApp()->ActivateView(pvi);
	}
	else
	{
		CWsfTIFolder::OnWsfOpen();
		WsfGetCApp()->m_nDbTreeItemViewId = m_nViewID;
	}
}
