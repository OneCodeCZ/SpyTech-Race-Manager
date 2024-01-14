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

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTIRaceDrv, CTIMeasuredEntity)
	//{{AFX_MSG_MAP(CTIRaceDrv)
	ON_UPDATE_COMMAND_UI(ID_WSF_DELETE, OnUpdateWsfDelete)
	ON_UPDATE_COMMAND_UI(ID_WSF_PROPERTIES, OnUpdateProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CTIRaceDrv, CTIMeasuredEntity, 1)

//*****************************************************************************
CTIRaceDrv::CTIRaceDrv()
{
	Init(IDR_CONTEXT_MENUS,7, IDB_SMALL_ICONS,2, IDB_SMALL_ICONS,2, 0);
}

//*****************************************************************************
CTIRaceDrv::CTIRaceDrv(DWORD dwId, CTIDriver *pTIDrv) 
	: CTIMeasuredEntity(dwId)
{
	ASSERT_VALID(pTIDrv);
	Init(IDR_CONTEXT_MENUS,7, IDB_SMALL_ICONS,2, IDB_SMALL_ICONS,2, 0);
	m_Drv = pTIDrv->m_Drv;
}

//*****************************************************************************
void CTIRaceDrv::Serialize(CArchive &ar)
{
	CTIMeasuredEntity::Serialize(ar);

	if (ar.IsStoring())
	{
		ar.Write(&m_Drv, sizeof(CDbDrv));
	}
	else
	{
		ar.Read(&m_Drv, sizeof(CDbDrv));
	}
}

//*****************************************************************************
BOOL CTIRaceDrv::OnSubTreeDelete()
{
	int n = AfxMessageBox(IDS_REMOVE_DRV_FROM_RACE, MB_ICONEXCLAMATION|MB_YESNO);
	return n == IDYES;
}

//*****************************************************************************
BOOL CTIRaceDrv::OnInitProperties(CWsfPropertySheet &s)
{
	s.SetTitle(s.IsWizard() ? IDS_NEW_DRV_IN_RACE : IDS_DRV_IN_RACE_PROPERTIES);
	s.AddPage(new CPgDrvBasic(&m_Drv, true, this));
	s.AddPage(new CPgREStartData(this));
//	AddDebugPropertySheet(s);
	return true;
}

//*****************************************************************************
BOOL CTIRaceDrv::OnCloseProperties(CWsfPropertySheet &s, BOOL bOk)
{
	if (!bOk) return true;	
	WsfGetWorkspace()->SetModifiedFlag(true);
	return true;
}

//*****************************************************************************
void CTIRaceDrv::OnUpdateWsfDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(WsfGetCWorkspace()->m_pTIRace->m_nRacePart == RACE_PART_NONE);
}

//*****************************************************************************
void CTIRaceDrv::OnUpdateProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsMultiselection());
}

//*****************************************************************************
CString CTIRaceDrv::GetEntityName(FORMAT eFormat) const
{
	CString s;

	ASSERT(m_Drv.m_szName[0] && m_Drv.m_szSurname[0]);
	ASSERT(strlen(m_Drv.m_szName) <= DB_DRV_NAME_CHARS);
	ASSERT(strlen(m_Drv.m_szSurname) <= DB_DRV_SURNAME_CHARS);
	ASSERT(strlen(m_Drv.m_szNickname) <= DB_DRV_NICKNAME_CHARS);

	switch (eFormat)
	{
	case NAME_SURNAME: 
		s = m_Drv.m_szName;
		s += ' ';
		s += m_Drv.m_szSurname;
		break;

	case SURNAME_NAME: 
		s = m_Drv.m_szSurname;
		s += ' ';
		s += m_Drv.m_szName;
		break;

	case NICKNAME:
		if (m_Drv.m_szNickname[0])
		{
			s = m_Drv.m_szNickname;
			break;
		}
		// fall through

	case SURNAME_SHORT_NAME: 
		s = m_Drv.m_szSurname;
		s += ' ';
		s += m_Drv.m_szName[0];
		s += '.';
		break;

	default:
		ASSERT(false);
	}

	return s;
}













