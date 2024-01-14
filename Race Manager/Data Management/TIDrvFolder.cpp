///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTIDrvFolder, CWsfTINamedFolder)
	//{{AFX_MSG_MAP(CTIDrvFolder)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_UPDATE_COMMAND_UI(ID_NEW_FOLDER, OnUpdateNewFolder)
	ON_UPDATE_COMMAND_UI(ID_WSF_RENAME, OnUpdateWsfRename)
	ON_UPDATE_COMMAND_UI(ID_WSF_DELETE, OnUpdateWsfDelete)
	ON_COMMAND(ID_NEW_DRIVER, OnNewDriver)
	ON_UPDATE_COMMAND_UI(ID_NEW_DRIVER, OnUpdateNewDriver)
	ON_COMMAND(ID_EXPORT_DRV_FOLDER, OnExportDrvFolder)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_DRV_FOLDER, OnUpdateExportDrvFolder)
	ON_COMMAND(ID_WSF_OPEN, OnWsfOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CTIDrvFolder, CWsfTINamedFolder, 1)

//*****************************************************************************
CTIDrvFolder::CTIDrvFolder()
{
	Init(IDR_CONTEXT_MENUS,0, IDB_SMALL_ICONS,3, IDB_SMALL_ICONS,3, 0);
}

//*****************************************************************************
CTIDrvFolder::CTIDrvFolder(const char *pcszTitle) : CWsfTINamedFolder(pcszTitle)
{
	Init(IDR_CONTEXT_MENUS,0, IDB_SMALL_ICONS,3, IDB_SMALL_ICONS,3, 0);
}

//*****************************************************************************
CTIDriver *CTIDrvFolder::FindDriver(const char *pcszName, 
                                    const char *pcszSurname,
                                    const char* pcszNickname, 
                                    BOOL bSubTree)
{
	ASSERT(pcszName && pcszSurname && pcszNickname);

	CWsfTreeItem *pItem = GetChild();
	CTIDriver *pDrv;

	while (pItem)
	{
		if (pItem->IsKindOf(RUNTIME_CLASS(CTIDriver)))
		{
			pDrv = (CTIDriver*) pItem;
			if (!stricmp(pDrv->m_Drv.m_szSurname, pcszSurname) && 
				!stricmp(pDrv->m_Drv.m_szName, pcszName) &&
				!stricmp(pDrv->m_Drv.m_szNickname, pcszNickname))
				return pDrv;
		}
		else if (bSubTree && pItem->GetChild()) 
		{
			ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CTIDrvFolder)));
			CTIDriver *pDrv = ((CTIDrvFolder*)pItem)->FindDriver(pcszName, pcszSurname, pcszNickname, true);
			if (pDrv) return pDrv;
		}

		pItem = pItem->GetNextSibling();
	}
	
	return NULL;
}

//*****************************************************************************
CTIDrvFolder* CTIDrvFolder::FindFolder(const char *pcszName, BOOL bSubTree)
{
	CString sTitle;

	for (CWsfTreeItem *pItem = GetChild(); pItem; pItem = pItem->GetNextSibling())
	{
		if (pItem->IsKindOf(RUNTIME_CLASS(CTIDrvFolder)))
		{
			CTIDrvFolder *pFolder = (CTIDrvFolder*) pItem;
			sTitle = pFolder->GetTitle();

			if (sTitle == pcszName) return pFolder;
			if (bSubTree && (pFolder = pFolder->FindFolder(sTitle, false))) return pFolder;
		}
	}
	
	return NULL;
}

//*****************************************************************************
int CTIDrvFolder::OnCompare(CWsfTreeItem *pItem1, CWsfTreeItem *pItem2)
{
	BOOL bDrv1 = pItem1->IsKindOf(RUNTIME_CLASS(CTIDriver));
	BOOL bDrv2 = pItem2->IsKindOf(RUNTIME_CLASS(CTIDriver));

	if (bDrv1 && !bDrv2) return 1;
	if (!bDrv1 && bDrv2) return -1;

	return strcmp(pItem1->GetTitle(), pItem2->GetTitle());
}

//*****************************************************************************
DROPEFFECT CTIDrvFolder::OnAcceptDataObject(COleDataObject *pDataObject)
{
	CWsfTreeItem *pItem, **ppItem;
	HGLOBAL hData;
	BOOL b;

	if (!pDataObject->IsDataAvailable(WsfGetApp()->m_cfTreeItemList)) return false;

	hData = pDataObject->GetGlobalData(WsfGetApp()->m_cfTreeItemList);
	ppItem = (CWsfTreeItem**) GlobalLock(hData);
	ASSERT(ppItem);

	while (pItem = *ppItem++)
	{
		ASSERT_VALID(pItem);
		if (pItem == this) break;
		if (pItem->GetParent() == this) break;
		b = pItem->IsKindOf(RUNTIME_CLASS(CTIDriver));
		b |= pItem->IsKindOf(RUNTIME_CLASS(CTIDrvFolder));
		if (!b) break;
	}

	GlobalUnlock(hData);
	return pItem ? DROPEFFECT_NONE : DROPEFFECT_MOVE;
}

//*****************************************************************************
void CTIDrvFolder::OnDropDataObject(COleDataObject *pDataObject, DROPEFFECT dropEffect)
{
	CWsfTreeItem *pItem, **ppItem, *pParent;
	HGLOBAL hData;

	hData = pDataObject->GetGlobalData(WsfGetApp()->m_cfTreeItemList);
	ppItem = (CWsfTreeItem**) GlobalLock(hData);
	ASSERT(ppItem);
	pParent = (*ppItem)->GetParent();

	while (pItem = *ppItem++)
	{
		pItem->RemoveFromTree();
		AddChild(pItem);
	}
	
	GlobalUnlock(hData);
	WsfGetApp()->TreeHasChanged(pParent, CWsfApp::ETreeChanges::CHILDERN_DELETED);
	WsfGetApp()->TreeHasChanged(this, CWsfApp::ETreeChanges::CHILDERN_ADDED);
}

//*****************************************************************************
BOOL CTIDrvFolder::OnRename(CEdit *pEditCtrl, const char *pcszNewTitle)
{
	if (!pEditCtrl)
	{
		if (!strlen(pcszNewTitle))
		{
			AfxMessageBox(IDS_EMPTY_NAME, MB_OK|MB_ICONERROR);
			return false;
		}

		CWsfTreeItem *p = GetParent()->FindChildItem(pcszNewTitle, false);
		if (p)
		{
			AfxMessageBox(IDS_FOLDER_EXISTS, MB_OK|MB_ICONERROR);
			return false;
		}
		SetTitle(pcszNewTitle);
	}

	else 
	
	{
		pEditCtrl->SendMessage(EM_LIMITTEXT, DRV_FOLDER_MAX_CHARS, 0);
	}

	return true;
}

//*****************************************************************************
void CTIDrvFolder::OnNewFolder() 
{
   WsfGetCApp()->CreateDriverFolder(this);
}

//*****************************************************************************
void CTIDrvFolder::OnUpdateNewFolder(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsMultiselection());
}

//*****************************************************************************
void CTIDrvFolder::OnNewDriver() 
{
   WsfGetCApp()->CreateDriver(this);
}

//*****************************************************************************
void CTIDrvFolder::OnUpdateNewDriver(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsMultiselection());
}

//*****************************************************************************
void CTIDrvFolder::OnUpdateWsfRename(CCmdUI* pCmdUI) 
{
	bool b = this != (WsfGetCApp())->m_pTIDbDrvFolder ? true : false;
	b &= ! IsMultiselection();
	pCmdUI->Enable(b);
}

//*****************************************************************************
void CTIDrvFolder::OnUpdateWsfDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(this != (WsfGetCApp())->m_pTIDbDrvFolder ? true : false);
}

/*****************************************************************************/
void CTIDrvFolder::OnExportDrvFolder() 
{
	WsfGetCApp()->ExportDrivers(this);
}

/*****************************************************************************/
void CTIDrvFolder::OnUpdateExportDrvFolder(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

/*****************************************************************************/
void CTIDrvFolder::OnWsfOpen() 
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
