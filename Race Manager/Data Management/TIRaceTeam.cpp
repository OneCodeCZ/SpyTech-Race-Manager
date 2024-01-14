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
#include "DrvPgBasic.h"
#include "PgREStartData.h"
#include "PgTeamBasic.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTIRaceTeam, CTIMeasuredEntity)
	//{{AFX_MSG_MAP(CTIRaceTeam)
	ON_UPDATE_COMMAND_UI(ID_WSF_DELETE, OnUpdateWsfDelete)
	ON_UPDATE_COMMAND_UI(ID_WSF_PROPERTIES, OnUpdateProperties)
	ON_UPDATE_COMMAND_UI(ID_WSF_OPEN, OnUpdateWsfOpen)
	ON_COMMAND(ID_WSF_OPEN, OnWsfOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CTIRaceTeam, CTIMeasuredEntity, 1)

//*****************************************************************************
CTIRaceTeam::CTIRaceTeam()
{
	Init(IDR_CONTEXT_MENUS,6, IDB_SMALL_ICONS,6, IDB_SMALL_ICONS,6, 0);
}

//*****************************************************************************
CTIRaceTeam::CTIRaceTeam(DWORD dwId) : CTIMeasuredEntity(dwId)
{
	Init(IDR_CONTEXT_MENUS,6, IDB_SMALL_ICONS,6, IDB_SMALL_ICONS,6, 0);
}

//*****************************************************************************
void CTIRaceTeam::Serialize(CArchive &ar)
{
	CTIMeasuredEntity::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_strName;
	}
	else
	{
		ar >> m_strName;
	}
}

//*****************************************************************************
int CTIRaceTeam::OnCompare(CWsfTreeItem *pItem1, CWsfTreeItem *pItem2)
{
	return strcmp(pItem1->GetTitle(),pItem2->GetTitle());
}

//*****************************************************************************
DROPEFFECT CTIRaceTeam::OnAcceptDataObject(COleDataObject *pDataObject)
{
	CWsfTreeItem *pItem, **ppItem;
	HGLOBAL hData;
	int nEffect = DROPEFFECT_COPY;

	if (!pDataObject->IsDataAvailable(WsfGetApp()->m_cfTreeItemList)) return false;

	hData = pDataObject->GetGlobalData(WsfGetApp()->m_cfTreeItemList);
	ppItem = (CWsfTreeItem**) GlobalLock(hData);
	ASSERT(ppItem);

	while (pItem = *ppItem++)
	{
		ASSERT_VALID(pItem);
		if (pItem == this) break;
		if (pItem->GetParent() == this) break;
		if (!pItem->IsKindOf(RUNTIME_CLASS(CTIDriver))) break;
		if (((CTIDriver*)pItem)->m_bInRace) nEffect = DROPEFFECT_MOVE;
	}

	GlobalUnlock(hData);
	return pItem ? DROPEFFECT_NONE : nEffect;
}

//*****************************************************************************
void CTIRaceTeam::OnDropDataObject(COleDataObject *pDataObject, DROPEFFECT dropEffect)
{
	CWsfTreeItem *pItem, **ppItem;
	HGLOBAL hData;
	bool bAdded = false;
	DWORD dwRes;

	hData = pDataObject->GetGlobalData(WsfGetApp()->m_cfTreeItemList);
	ppItem = (CWsfTreeItem**) GlobalLock(hData);
	ASSERT(ppItem);

	while (pItem = *ppItem++)
	{
		dwRes = WsfGetCWorkspace()->m_pTIRace->AddDriver((CTIDriver*)pItem, this, dropEffect == DROPEFFECT_MOVE);
		if (dwRes == ERROR_DATABASE_FULL)
			break;
		else if (dwRes == ERROR_SUCCESS)
			bAdded = true;
	}

	GlobalUnlock(hData);
	if (bAdded) g_pApp->TreeHasChanged(this, CWsfApp::ETreeChanges::CHILDERN_ADDED);
}

//*****************************************************************************
BOOL CTIRaceTeam::OnSubTreeDelete()
{
	int n = AfxMessageBox(IDS_REMOVE_TEAM_FROM_RACE, 
		MB_ICONEXCLAMATION|MB_YESNO);
	return n == IDYES;
}

//*****************************************************************************
CTIDriver *CTIRaceTeam::FindDrv(const char *pcszName, const char *pcszSurname)
{
	ASSERT(pcszName);
	CTIDriver *pItem = (CTIDriver*) GetChild();

	while (pItem)
	{
		ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CTIDriver)));
		
		if (!stricmp(pcszSurname, pItem->m_Drv.m_szSurname) &&
			!stricmp(pcszName, pItem->m_Drv.m_szName)) 
			return pItem;

		pItem = (CTIDriver*) pItem->GetNextSibling();
	}

	return NULL;
}

//*****************************************************************************
int CTIRaceTeam::GetDrvCount()
{
	CWsfTreeItem *pItem = (CTIDriver*) GetChild();
	int n = 0;

	while (pItem)
	{
		n++;	
		pItem = pItem->GetNextSibling();
	}

	return n;
}

//*****************************************************************************
BOOL CTIRaceTeam::OnInitProperties(CWsfPropertySheet &s)
{
	s.SetTitle(s.IsWizard() ? IDS_NEW_TEAM : IDS_TEAM_PROPERTIES);
	s.AddPage(new CPgTeamBasic(this));
	s.AddPage(new CPgREStartData(this));
//	AddDebugPropertySheet(s);
	return true;
}

//*****************************************************************************
BOOL CTIRaceTeam::OnCloseProperties(CWsfPropertySheet &s, BOOL bOk)
{
	if (bOk)
	{
		WsfGetWorkspace()->SetModifiedFlag(true);
	}

	return true;
}

//*****************************************************************************
void CTIRaceTeam::OnUpdateWsfDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

//*****************************************************************************
void CTIRaceTeam::OnUpdateProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsMultiselection());
}

//*****************************************************************************
void CTIRaceTeam::OnWsfOpen() 
{
	CWsfViewInfo* pvi = WsfGetCApp()->GetViewInfo(WsfGetCApp()->m_nRaceTreeItemViewId);

	if (pvi)
	{
		CWsfTreeItemListView* pView = (CWsfTreeItemListView*) pvi->pView;
		pView->SetTreeItem(this);
		WsfGetApp()->ActivateView(pvi);
	}
	else
	{
		CWsfTIFolder::OnWsfOpen();
		WsfGetCApp()->m_nRaceTreeItemViewId = m_nViewID;
	}
}

//*****************************************************************************
void CTIRaceTeam::OnUpdateWsfOpen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

