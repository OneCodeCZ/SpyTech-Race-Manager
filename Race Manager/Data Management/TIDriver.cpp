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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTIDriver, CWsfTreeItem)
	//{{AFX_MSG_MAP(CTIDriver)
	ON_UPDATE_COMMAND_UI(ID_WSF_DELETE, OnUpdateWsfDelete)
	ON_UPDATE_COMMAND_UI(ID_WSF_PROPERTIES, OnUpdateProperties)
	ON_COMMAND(ID_ADD_TO_RACE, OnAddToRace)
	ON_UPDATE_COMMAND_UI(ID_ADD_TO_RACE, OnUpdateAddToRace)
	ON_COMMAND(ID_NEW_DRIVER, OnNewDriver)
	ON_UPDATE_COMMAND_UI(ID_NEW_DRIVER, OnUpdateNewDriver)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_UPDATE_COMMAND_UI(ID_NEW_FOLDER, OnUpdateNewFolder)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CTIDriver, CWsfTreeItem, 1)

//*****************************************************************************
CTIDriver::CTIDriver() 
  : CWsfTreeItem(IDR_CONTEXT_MENUS,1, IDB_SMALL_ICONS,2, IDB_SMALL_ICONS,2, 0)
{
	m_Drv.Init();
	m_bInRace = false;
}

//*****************************************************************************
CTIDriver::CTIDriver(CTIDriver *pTIDrv, BOOL bInRace)
  : CWsfTreeItem(IDR_CONTEXT_MENUS,1, IDB_SMALL_ICONS,2, IDB_SMALL_ICONS,2, 0)
{
	m_Drv = pTIDrv->m_Drv;
	m_bInRace = bInRace;
}

//*****************************************************************************
CTIDriver::CTIDriver(const CTIDriver &drv) : CWsfTreeItem(drv)
{
	m_Drv = drv.m_Drv;
	m_bInRace = drv.m_bInRace;
}

//*****************************************************************************
CString CTIDriver::GetTitle() const
{
	CString sTitle;
	sTitle.Format("%s %s", m_Drv.m_szSurname, m_Drv.m_szName);
	return sTitle;
}

//*****************************************************************************
void CTIDriver::Serialize(CArchive &ar)
{
	CWsfTreeItem::Serialize(ar);

	if (ar.IsStoring())
	{
		ar.Write(&m_Drv, sizeof(CDbDrv));
		ar << m_bInRace;
	}
	else
	{
		ar.Read(&m_Drv, sizeof(CDbDrv));
		ar >> m_bInRace;
	}
}

//*****************************************************************************
BOOL CTIDriver::OnInitProperties(CWsfPropertySheet &s)
{
	s.SetTitle(s.IsWizard() ? IDS_NEW_DRV : IDS_DRV_PROPERTIES);
	s.AddPage(new CPgDrvBasic(&m_Drv, m_bInRace, this));
	return true;
}

//*****************************************************************************
BOOL CTIDriver::OnCloseProperties(CWsfPropertySheet &s, BOOL bOk)
{
	if (bOk && m_bInRace)
	{
		WsfGetCWorkspace()->SetModifiedFlag(true);
	}

	return true;
}

//*****************************************************************************
BOOL CTIDriver::OnSubTreeDelete()
{
	int n = AfxMessageBox(IDS_DELETE_DRV, MB_ICONEXCLAMATION|MB_YESNO);
	return n == IDYES;
}

//*****************************************************************************
void CTIDriver::OnUpdateWsfDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bInRace || WsfGetCWorkspace()->m_pTIRace->m_nRacePart == RACE_PART_NONE);
}

//*****************************************************************************
void CTIDriver::OnUpdateProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsMultiselection());
}

//*****************************************************************************
void CTIDriver::OnAddToRace() 
{
	// We select team only for the first item and the team must be remembered.
	static CTIRaceTeam *pTITeam;  
	CTIRace& Race = *WsfGetCWorkspace()->m_pTIRace;

	// === TEAMS ================================================================
	
	if (Race.m_bTeams)
	{
		// Select team if this is the first item in multiselection

		if (IsFirstSelected())
		{
			CWslArray<CRuntimeClass*> aClasses;
			aClasses += RUNTIME_CLASS(CTIRaceTeam);
				
			pTITeam = (CTIRaceTeam*) WsfSelectTreeItem(IDS_SELECT_TEAM, IDS_SELECT_TEAM_DETAIL,
				&Race,
				&aClasses,
				WsfGetFrame());
				
			if (!pTITeam)
			{
				CancelCommand();
				return;
			}
		}

		// Add driver

		switch (Race.AddDriver(this, pTITeam, false))
		{
		case ERROR_DATABASE_FULL:
		case ERROR_CANCELLED:
			CancelCommand();
			break;
		}

		// Update UI 

		if (IsLastSelected()) 
		{
			g_pApp->TreeHasChanged(pTITeam, CWsfApp::ETreeChanges::CHILDERN_ADDED);
			g_pApp->SetWorkspaceModified();
		}
	}
	else
	// === SINGLE DRIVERS =======================================================
	{
		Race.AddDriver(this, NULL, false);

		if (IsLastSelected()) 
		{
			g_pApp->TreeHasChanged(&Race, CWsfApp::ETreeChanges::CHILDERN_ADDED);
			g_pApp->SetWorkspaceModified();
		}
	}
}

//*****************************************************************************
void CTIDriver::OnUpdateAddToRace(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bInRace && 
		WsfGetApp()->IsWorkspaceOpened() &&
		WsfGetCWorkspace()->m_pTIRace->m_nRacePart == RACE_PART_NONE);
}

//*****************************************************************************
void CTIDriver::OnNewDriver() 
{
	CTIDrvFolder *p = (CTIDrvFolder*) GetParent();
	ASSERT_KINDOF(CTIDrvFolder, p);
	p->OnNewDriver();
}

//*****************************************************************************
void CTIDriver::OnUpdateNewDriver(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsMultiselection() && !m_bInRace);
}

//*****************************************************************************
void CTIDriver::OnNewFolder() 
{
	CTIDrvFolder *p = (CTIDrvFolder*) GetParent();
	ASSERT_KINDOF(CTIDrvFolder, p);
	p->OnNewFolder();
}

//*****************************************************************************
void CTIDriver::OnUpdateNewFolder(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsMultiselection() && !m_bInRace);
}
