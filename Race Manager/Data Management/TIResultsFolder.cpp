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

BEGIN_MESSAGE_MAP(CTIResultsFolder, CWsfTIFolder)
	//{{AFX_MSG_MAP(CTIResultsFolder)
	ON_COMMAND(ID_WSF_OPEN, OnWsfOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CTIResultsFolder, CWsfTIFolder, 1)

//*****************************************************************************
CTIResultsFolder::CTIResultsFolder()
{
	Init(IDR_CONTEXT_MENUS,10, IDB_SMALL_ICONS,8, IDB_SMALL_ICONS,8, 0);
	VERIFY(m_sName.LoadString(IDS_DOCUMENTS));
}

/*****************************************************************************/
void CTIResultsFolder::OnWsfOpen() 
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
