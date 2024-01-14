///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
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
#include "TIDB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CTIDB, CWsfTINamedFolder)
	//{{AFX_MSG_MAP(CTIDB)
	ON_COMMAND(ID_WSF_OPEN, OnWsfOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CTIDB, CWsfTINamedFolder, 1)

/*****************************************************************************/
CTIDB::CTIDB() : CWsfTINamedFolder(CString((char*)IDS_DATABASE))
{
	Init(IDR_CONTEXT_MENUS,8, IDB_SMALL_ICONS,5, IDB_SMALL_ICONS,5, CWsfTreeItem::EXPANDED);
}

/*****************************************************************************/
CTIDB::~CTIDB()
{
}

/*****************************************************************************/
void CTIDB::OnWsfOpen() 
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
