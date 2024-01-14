///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 07
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "PgRaceProperties.h"
#include "PgSetupTrack.h"
#include "PgSetupSound.h"
#include "DlgFirstLanes.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTIRace, CWsfTIFolder)
	//{{AFX_MSG_MAP(CTIRace)
	ON_UPDATE_COMMAND_UI(ID_WSF_PROPERTIES, OnUpdateProperties)
	ON_COMMAND(ID_NEW_TEAM, OnNewTeam)
	ON_UPDATE_COMMAND_UI(ID_NEW_TEAM, OnUpdateNewTeam)
	ON_COMMAND(ID_WSF_OPEN, OnWsfOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CTIRace, CWsfTIFolder, 1)

//*****************************************************************************
static BOOL __EntityRankingAscSortCallback(CTIRaceEntity **ppRE1, 
                                           CTIRaceEntity **ppRE2,
                                           void* pUserData)
{
	if ((**ppRE1).m_nRanking > (**ppRE2).m_nRanking)
	{
		CTIRaceEntity *p = *ppRE1;
		*ppRE1 = *ppRE2;
		*ppRE2 = p;
		return true;
	}
	else return false;
}	

//*****************************************************************************
static BOOL __EntityRankingDescSortCallback(CTIRaceEntity **ppRE1, 
                                            CTIRaceEntity **ppRE2,
                                            void* pUserData)
{
	if ((**ppRE1).m_nRanking < (**ppRE2).m_nRanking)
	{
		CTIRaceEntity *p = *ppRE1;
		*ppRE1 = *ppRE2;
		*ppRE2 = p;
		return true;
	}
	else return false;
}	
		
//*****************************************************************************
CTIRace::CTIRace()
{
	Init(IDR_CONTEXT_MENUS,5, IDB_SMALL_ICONS,5, IDB_SMALL_ICONS,0, 0);
	m_pRS = NULL;
   m_nRacePart = 0xffffffff;
}

//*****************************************************************************
CTIRace::CTIRace(CSetupTrack *pSetupTrack) : CSetupTrack(*pSetupTrack)
{
	Init(IDR_CONTEXT_MENUS,5, IDB_SMALL_ICONS,5, IDB_SMALL_ICONS,5, CWsfTreeItem::EXPANDED);

	m_strOrganizer = m_strOwner;
	m_bTeams = false;
	m_Date.SetNow();
	m_nLapMinTime = 0;
	m_nMinMembers = 1;
	m_nMaxMembers = 100;
	m_nRacePart = RACE_PART_NONE;
	m_pRS = NULL;
	m_dwIdCounter = 0;
	m_bPartInProgress = false;
	m_bPartFinished = false;
}

//*****************************************************************************
CString CTIRace::GetTitle() const
{
	CString sTitle;
	sTitle.LoadString(IDS_RACE_SLASH);
	sTitle += GetRacePartName(m_nRacePart);
	return sTitle;
}

//*****************************************************************************
int CTIRace::OnCompare(CWsfTreeItem *pItem1, CWsfTreeItem *pItem2)
{
	if (pItem1->IsKindOf(RUNTIME_CLASS(CTIResultsFolder))) 
		return -1;
	else if (pItem2->IsKindOf(RUNTIME_CLASS(CTIResultsFolder)))
		return 1;

	BOOL bRSTS1 = pItem1->IsKindOf(RUNTIME_CLASS(CTIRS));

	BOOL bRSTS2 = pItem2->IsKindOf(RUNTIME_CLASS(CTIRS));

	if (bRSTS1) 
		return -1;
	else if (bRSTS2)
		return 1;
	else
		return strcmp(pItem1->GetTitle(),pItem2->GetTitle());
}

//*****************************************************************************
void CTIRace::Serialize(CArchive &ar)
{
	int i,t,n;
	DWORD dwId;
	int nVersion;
	CWsfTreeItem *pItem;

	CWsfTIFolder::Serialize(ar);
	CSetupTrack::Serialize(ar);

	// ========= SAVE ===========================================================

	if (ar.IsStoring())
	{
		TRACE0("\nSAVE\n");

		ar << (int) 0;		// zero based versioning

		ar << m_strOrganizer;
		ar << m_strName;
		ar << m_strNumber;
		ar << m_strJudge;
		ar << m_strPlace;
		
		ar << m_bTeams;
		ar << m_nLapMinTime;
		ar << m_nMinMembers;
		ar << m_nMaxMembers;
		ar << m_bPartInProgress;
		ar << m_bPartFinished;
		n = m_Date.Get();
		ar.Write(&n, sizeof(DWORD));
		
		ar << m_dwIdCounter;
		ar << m_nRacePart;
		ar << m_nNumGroups;
		ar << m_nActiveGroup;

		// Save part entities

		ar << m_aPartEntities.GetSize();
		for (i = m_aPartEntities.GetSize(); i--; ) ar << m_aPartEntities[i]->GetId();

		// Save groups

		for (i = 0; i < m_nNumGroups; i++) 
		{
			TRACE1("Group %d: ", i);
			ar << m_aGroup[i].GetSize();

			for (t = m_aGroup[i].GetSize(); t--; ) 
			{
				TRACE1("%d,", m_aGroup[i][t]->GetId());
				ar << m_aGroup[i][t]->GetId();
			}
			TRACE0("\n");
		}
	}	

	// ========= LOAD ===========================================================
	else
	{
		TRACE0("\nLOAD\n");

		ar >> nVersion;

		ar >> m_strOrganizer;
		ar >> m_strName;
		ar >> m_strNumber;
		ar >> m_strJudge;
		ar >> m_strPlace;

		ar >> m_bTeams;
		ar >> m_nLapMinTime;
		ar >> m_nMinMembers;
		ar >> m_nMaxMembers;
		ar >> m_bPartInProgress;
		ar >> m_bPartFinished;
		ar.Read(&n, sizeof(DWORD)); m_Date.Set(n);

		ar >> m_dwIdCounter;
		ar >> m_nRacePart;
		ar >> m_nNumGroups;
		ar >> m_nActiveGroup;

		// Load m_aPartEntities array

		ar >> n;
		m_aPartEntities.SetSize(n);

		for (i = n; i--;)
		{
			ar >> dwId;
			pItem = FindChildItem(dwId, false);
			ASSERT_VALID(pItem);
			m_aPartEntities[i] = (CTIMeasuredEntity*) pItem; 
		}

		// Load m_aGroup[] array
		
		for (i = 0; i < m_nNumGroups; i++) 
		{
			ar >> t;
			m_aGroup[i].SetSize(t);
			TRACE1("Group %d: ", i);

			for (;t--;)
			{
				ar >> dwId;
				TRACE1("%d,", dwId);
				pItem = FindChildItem(dwId, false);
				ASSERT_VALID(pItem);
				m_aGroup[i][t] = (CTIMeasuredEntity*) pItem; 
			}
			TRACE0("\n");
		}

		// Get pointers to RS

		CWsfTreeItem *pItem = GetChild();
		n = 0;

		while (pItem && n < 2)
		{
			if (pItem->IsKindOf(RUNTIME_CLASS(CTIRS))) 
			{
				m_pRS = (CTIRS*) pItem;
				n++;
			}	
			pItem = pItem->GetNextSibling();
		}
	}
}

//*****************************************************************************
CTIRaceEntity *CTIRace::FindEntity(CString &strName)
{
	CWsfTreeItem *pItem = GetChild();

	while (pItem)
	{
		if (pItem->IsKindOf(RUNTIME_CLASS(CTIRaceEntity)))
		{
			if (strName == ((CTIRaceEntity*)pItem)->GetEntityName(CTIRaceEntity::NICKNAME)) 
				return (CTIRaceEntity*)pItem;
		}
		pItem = pItem->GetNextSibling();
	}

	return NULL;
}

//*****************************************************************************
CTIDriver *CTIRace::FindTeamDrv(const char *pcszName, const char *pcszSurname)
{
	ASSERT(pcszName);
	CWsfTreeItem *pItem = GetChild();

	while (pItem)
	{
		if (pItem->IsKindOf(RUNTIME_CLASS(CTIRaceTeam)))
		{
			CTIDriver *pDrv = ((CTIRaceTeam*)pItem)->FindDrv(pcszName, pcszSurname);
			if (pDrv) return pDrv;
		}
		pItem = pItem->GetNextSibling();
	}

	return NULL;
}

//*****************************************************************************
BOOL CTIRace::OnInitProperties(CWsfPropertySheet &s)
{
	s.SetTitle(s.IsWizard() ? IDS_NEW_RACE : IDS_RACE_PROPERTIES);
	s.AddPage(new CPgRaceProperties(this));
	if (!s.IsWizard()) s.AddPage(new CPgSetupTrack(this, true));
	return true;
}

//*****************************************************************************
BOOL CTIRace::OnCloseProperties(CWsfPropertySheet &s, BOOL bOk)
{
	if (!bOk) return true;

	if (m_nRacePart > RACE_PART_NONE && m_nRacePart < RACE_PART_DONE)
			AfxMessageBox(IDS_SOME_CHANGES_AFTER_RESET, MB_OK|MB_ICONINFORMATION);
	
	WsfGetWorkspace()->SetModifiedFlag(true);
	return true;
}

//*****************************************************************************
DROPEFFECT CTIRace::OnAcceptDataObject(COleDataObject *pDataObject)
{
	CWsfTreeItem *pItem, **ppItem;
	HGLOBAL hData;
	BOOL b;

	if (m_nRacePart > RACE_PART_NONE) return false;
	if (!pDataObject->IsDataAvailable(WsfGetApp()->m_cfTreeItemList)) return false;

	hData = pDataObject->GetGlobalData(WsfGetApp()->m_cfTreeItemList);
	ppItem = (CWsfTreeItem **) GlobalLock(hData);
	ASSERT(ppItem);

	while (pItem = *ppItem++)
	{
		ASSERT_VALID(pItem);
		if (pItem == this) break;
		if (pItem->GetParent() == this) break;

		b = pItem->IsKindOf(RUNTIME_CLASS(CTIDriver));
		if (b && WsfGetCWorkspace()->m_pTIRace->m_bTeams) break;
		b |= pItem->IsKindOf(RUNTIME_CLASS(CTIRS));
		if (!b) break;
	}

	GlobalUnlock(hData);
	return pItem ? DROPEFFECT_NONE : DROPEFFECT_COPY;
}

//*****************************************************************************
void CTIRace::OnDropDataObject(COleDataObject *pDataObject, DROPEFFECT dropEffect)
{
	CWsfTreeItem  *pItem, **ppItem;
	HGLOBAL hData;
	bool bAdded = false;

	hData = pDataObject->GetGlobalData(WsfGetApp()->m_cfTreeItemList);
	ppItem = (CWsfTreeItem **) GlobalLock(hData);
	ASSERT(ppItem);

	while (pItem = *ppItem++)
	{
		if (pItem->IsKindOf(RUNTIME_CLASS(CTIDriver)))
		{
			AddDriver((CTIDriver*)pItem, NULL, false);
			bAdded = true;
		}
		else if (pItem->IsKindOf(RUNTIME_CLASS(CTIRS)))
		{
			WsfGetCWorkspace()->m_pTIRace->SetRS((CTIRS*) pItem);
		}
	}

	GlobalUnlock(hData);
	if (bAdded) g_pApp->TreeHasChanged(this, CWsfApp::ETreeChanges::CHILDERN_ADDED);
}

//*****************************************************************************
void CTIRace::OnUpdateProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
}

//*****************************************************************************
void CTIRace::OnNewTeam() 
{
	CTIRaceTeam *pItem = new CTIRaceTeam(GetNextEntityId());
	ASSERT_VALID(pItem);
	
	if (pItem->DisplayProperties(true) != IDOK)
	{
		delete pItem;
		return;
	}

	AddChild(pItem);
	g_pApp->TreeHasChanged(pItem, CWsfApp::ETreeChanges::ITEM_ADDED);
}

//*****************************************************************************
void CTIRace::OnUpdateNewTeam(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bTeams && m_nRacePart == RACE_PART_NONE);
}

//*****************************************************************************
void CTIRace::SetRS(CTIRS *pRS)
{
	bool b = false;

	// Add to the tree

	if (!m_pRS)
	{
		m_pRS = new CTIRS();
		m_pRS->m_bInRace = true;
		AddChild(m_pRS);
		b = true;
	}

	*m_pRS = *pRS;
	g_pApp->SetWorkspaceModified();
	g_pApp->TreeHasChanged(m_pRS, b ? CWsfApp::ETreeChanges::ITEM_ADDED : CWsfApp::ETreeChanges::TITLE_ICON);
}

//*****************************************************************************
DWORD CTIRace::AddDriver(CTIDriver *pTIDrv, CTIRaceTeam *pTITeam, BOOL bMove)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pTIDrv);

	// ==== Team Race ==========================================================

	if (m_bTeams)
	{
		ASSERT(pTITeam);

		// Check for duplicity. Not if we are moving.

		if (!bMove && FindTeamDrv(pTIDrv->m_Drv.m_szName, pTIDrv->m_Drv.m_szSurname))
		{
			CString s;
			s.Format(IDS_DRV_IS_ALREADY_IN_RACE, pTIDrv->GetTitle());
			AfxMessageBox(s);
			return ERROR_FILE_EXISTS;
		}
		
		// Check maximum of members

		if (pTITeam->GetDrvCount() == m_nMaxMembers)
		{
			AfxMessageBox(IDS_TEAM_FULL);
			return ERROR_DATABASE_FULL;
		}

		// Add it

		CTIDriver *pItem = new CTIDriver(pTIDrv, true);
		ASSERT_VALID(pItem);
		pTITeam->AddChild(pItem);
	}

	// ==== Driver Race =========================================================
	else
	{
		CTIRaceDrv* pItem = new CTIRaceDrv(GetNextEntityId(), pTIDrv);
		ASSERT_VALID(pItem);

		if (pItem->DisplayProperties(true) != IDOK)  
		{
			delete pItem;
			return ERROR_CANCELLED;
		}

		AddChild(pItem);
	}

	// End

	if (bMove) WsfGetApp()->DeleteTreeItem(pTIDrv, true, 0);

	g_pApp->SetWorkspaceModified();
	return ERROR_SUCCESS;
}

//*****************************************************************************
void CTIRace::BuildEntityList(CListCtrl &List)
{
	LVITEM i;
	CString str;

	i.iSubItem = 0;
	i.iItem = 0;
	i.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	i.iImage = 6;

	for (CWsfTreeItem  *pItem = GetChild(); pItem; pItem = pItem->GetNextSibling())
	{
		if (pItem->IsKindOf(RUNTIME_CLASS(CTIRaceEntity)))
		{
			str = ((CTIRaceEntity*)pItem)->GetEntityName(CTIRaceEntity::SURNAME_NAME);
			i.pszText = (char*) (const char*) str;
			i.lParam = (LPARAM) pItem;
			i.iItem++;
			List.InsertItem(&i);
		}
	}
}

//*****************************************************************************
void CTIRace::BuildEntitiesArray(CEntityArray &a)
{
	a.RemoveAll();

	for (CWsfTreeItem *pItem = GetChild(); pItem; pItem = pItem->GetNextSibling())
	{
		if (pItem->IsKindOf(RUNTIME_CLASS(CTIRaceEntity))) 
			a += (CTIMeasuredEntity*) pItem;
	}
}

//*****************************************************************************
int CTIRace::GetPoints(int nPos, int nNumEntities)
{
	ASSERT(m_pRS->m_bEnablePoints);		// points should be enabled

	return nPos < m_pRS->m_aPoints.GetSize() ? 
		m_pRS->m_aPoints[nPos] * 1000 :
		nNumEntities - nPos;
}

//*****************************************************************************
CString CTIRace::GetRacePartName(int nRacePart) const
{
	CString s;
	int n;

	switch (nRacePart)
	{
	case RACE_PART_NONE		: n = IDS_BUILDING; break;
	case RACE_PART_QUAL		: n = IDS_QUAL; break;
	case RACE_PART_CONSI	: n =	IDS_CONSI; break;
	case RACE_PART_QUATER	: n = IDS_QUATER; break;
	case RACE_PART_SEMI		: n = IDS_SEMI; break;
	case RACE_PART_FINAL	: n = IDS_FINAL; break;
	case RACE_PART_DONE		: n = IDS_FINISHED; break;
	}

   BOOL bResult = s.LoadString(n);
	ASSERT (bResult);
   if (!bResult) return "";
	return s;
}

//*****************************************************************************
BOOL CTIRace::OnRaceStart()  // Called when "Ukoncit tvorbu" is selected
{
	CWsfTreeItem  *pItem;

	// Race must have an RS

	if (!m_pRS)
	{
		AfxMessageBox(IDS_NO_RS);
		return false;
	}
	ASSERT_VALID(m_pRS);

   // The installed track must have the same lane count as
   // the race's track

   if (g_pApp->GetNumLanes() != GetNumLanes())
	{
		AfxMessageBox(IDS_BAD_NUM_OF_LANES);
		return false;
	}

   // Check changing string to match the count of lanes
   // NOTE: This limitation is to be removed in the future.
   //       However, we must check any consequences!!!

	if (m_pRS->m_strChanging.GetLength() != GetNumLanes())
	{
		AfxMessageBox(IDS_BAD_CHANGING);
		return false;
	}

	// Check minimum number of members in team

	if (m_bTeams)
	{
		for (pItem = GetChild(); pItem; pItem = pItem->GetNextSibling())
		{
			if (pItem->IsKindOf(RUNTIME_CLASS(CTIRaceTeam)))
			{
				if ( ((CTIRaceTeam*)pItem)->GetDrvCount() < m_nMinMembers)
				{
					AfxMessageBox(IDS_BAD_TEAM_MEMBER_COUNT);
					return false;
				}
			}
		}
	}

	// Check that there are some drivers

	for (pItem = GetChild(); pItem; pItem = pItem->GetNextSibling())
	{
		if (pItem->IsKindOf(RUNTIME_CLASS(CTIRaceEntity))) break;
	}

	if (!pItem)
	{
		AfxMessageBox(IDS_NO_DRIVERS);
		return false;
	}

	// ----------------

	OnRacePartFinished();
	return true;
}

//*****************************************************************************
void CTIRace::HandleFirstPart(BOOL bEntitiesDescOrder)
{
	CTIMeasuredEntity *pRE;
	CWsfTreeItem  *pItem;
	int nMaxRanking;

	// Init part entities array, entities and get max ranking

	m_aPartEntities.RemoveAll();
	nMaxRanking = 0;

	for (pItem = GetChild(); pItem; pItem = pItem->GetNextSibling())
	{
		if (pItem->IsKindOf(RUNTIME_CLASS(CTIRaceEntity)))
		{
			pRE = (CTIMeasuredEntity*) pItem;
			m_aPartEntities += pRE;

			pRE->RemoveFlags(REF_FINAL | REF_SEMI | REF_QUATER);
			pRE->m_nRanking = pRE->m_nStartRanking;
			if (nMaxRanking < pRE->m_nRanking) nMaxRanking = pRE->m_nRanking;
		}
	}

	// Set ranking that is not set 

	for (pItem = GetChild(); pItem; pItem = pItem->GetNextSibling())
	{
		if (pItem->IsKindOf(RUNTIME_CLASS(CTIRaceEntity)))
		{
			pRE = (CTIMeasuredEntity*) pItem;
			if (pRE->m_nRanking == 0) pRE->m_nRanking = nMaxRanking + rand()%1000;
		}
	}

	// Sort by ranking

	if (bEntitiesDescOrder)
		m_aPartEntities.Sort((CWslBaseArray::SORT_CALLBACK*) __EntityRankingDescSortCallback, NULL);
	else
		m_aPartEntities.Sort((CWslBaseArray::SORT_CALLBACK*) __EntityRankingAscSortCallback, NULL);
}

//*****************************************************************************
BOOL CTIRace::OnRaceNextPart() 
{
	int n, i;

	// Check groups

	if (m_nRacePart > RACE_PART_QUAL && m_nRacePart < RACE_PART_DONE)
	{
		// Is there any group

		if (!m_nNumGroups)
		{
			AfxMessageBox(IDS_GROUPS_NOT_DONE);
			return false;
		}

		// Count members and check if none is empty

		for (i=m_nNumGroups, n=0; i--;)
		{
			if (!m_aGroup[i].GetSize())
			{
				AfxMessageBox(IDS_GROUPS_NOT_DONE);
				return false;
			}
			n += m_aGroup[i].GetSize();
		}

		// All entites are in groups?

		if (n != m_aPartEntities.GetSize())
		{
			AfxMessageBox(IDS_GROUPS_NOT_DONE);
			return false;
		}

		// Close group view

		WsfGetApp()->CloseView(WsfGetCApp()->m_nGroupsViewID, FALSE);
	}

	// NOTE: Measuring is called through inheritance!
	
	return true;
}

//*****************************************************************************
void CTIRace::OnResetRace() 
{
	CWsfTreeItem *pItem;

	// Close views

	WsfGetApp()->CloseAllPaneViews(false);

	// Reset entities

	for (pItem = GetChild(); pItem; pItem = pItem->GetNextSibling())
	{
		if (pItem->IsKindOf(RUNTIME_CLASS(CTIRaceEntity)))
		{
			((CTIRaceEntity*)pItem)->Reset();
		}
	}

	// Clear stuff

	m_aPartEntities.RemoveAll();
	ClearGroups();
	m_nRacePart = RACE_PART_NONE;
	m_bPartFinished = false;
	m_bPartInProgress = false;

	// Copy current track settings

	(*(CSetupTrack*)this) = (*(CSetupTrack*)WsfGetCApp());

	// Update UI

	g_pApp->SetWorkspaceModified();
	g_pApp->TreeHasChanged(this, CWsfApp::ETreeChanges::SUBTREE_TITLE_ICON);
}

//*****************************************************************************
void CTIRace::OnRacePartFinished()
{
	int nLastPart, nNewPart;

	// --------------------------------------------------------------------------
	// Except if called before first part:
	// - m_aPartEntities array is sorted by result & each entity in this array
	//   has m_nRanking set according the result.
	// - groups are sorted
	// --------------------------------------------------------------------------

	nLastPart = m_nRacePart;

	// Get next part

	switch (nLastPart)
	{
	case RACE_PART_NONE:
		nNewPart = m_pRS->m_bQual ? RACE_PART_QUAL : m_pRS->m_nFirstPart;
		break;

	case RACE_PART_QUAL:
		nNewPart = m_pRS->m_nFirstPart;
		break;

	default:
		nNewPart = nLastPart + 1;
		break;
	}

	// Build m_aPartEntities array for the next part

	switch (nLastPart)
	{
	case RACE_PART_NONE:
		HandleFirstPart(nNewPart == RACE_PART_QUAL);
		break;
	
	case RACE_PART_FINAL:
		m_aPartEntities.RemoveAll();
		break;

	case RACE_PART_QUAL:
		HandleQualFinish();
		break;

	default:
		BuildPartEntities(nLastPart, nNewPart);
		break;
	}

	// Build groups

	if (nNewPart > RACE_PART_QUAL && nNewPart < RACE_PART_DONE)
	{
		BuildGroups();
	}
	else
	{
		ClearGroups();
	}

	// Stuff

	m_bPartInProgress = false;
	m_bPartFinished = false;
	m_nRacePart = nNewPart;
	g_pApp->TreeHasChanged(this, CWsfApp::ETreeChanges::SUBTREE_TITLE_ICON);

	// NOTE: Race should be save here. It is done by TIMeasured verison
	// of this method.
}

//*****************************************************************************
void CTIRace::HandleQualFinish()
{
	int i,n,nFirstPart, nIndex;

	nFirstPart = m_pRS->m_nFirstPart;
	nIndex = 0;

	// Mark entities for direct advance to final

	if (nFirstPart < RACE_PART_FINAL)
	{
		n = min(m_pRS->m_nAdvancesQual[RS_ADV_QUAL_FINAL], m_aPartEntities.GetSize());
		for (i = n; i--;) m_aPartEntities[i]->AddFlags(REF_FINAL);
		nIndex += n;
	}

	// Mark entities for direct advance to final

	if (nFirstPart < RACE_PART_SEMI)
	{
		n = min(m_pRS->m_nAdvancesQual[RS_ADV_QUAL_SEMI], m_aPartEntities.GetSize());
		for (i = n; i--;) m_aPartEntities[nIndex + i]->AddFlags(REF_SEMI);
		nIndex += n;
	}

	// Mark entities for direct advance to final

	if (nFirstPart < RACE_PART_QUATER)
	{
		n = min(m_pRS->m_nAdvancesQual[RS_ADV_QUAL_QUATER], m_aPartEntities.GetSize());
		for (i = n; i--;) m_aPartEntities[nIndex + i]->AddFlags(REF_QUATER);
		nIndex += n;
	}

	// Remove all entities marked for direct advance

	m_aPartEntities.RemoveRange(0, nIndex);

	// Set ranking to (1..?)

	for (i = m_aPartEntities.GetSize(); i--;) 
		m_aPartEntities[i]->m_nRanking = i + 1;
}

//*****************************************************************************
void CTIRace::BuildPartEntities(int nLastPart, int nNewPart)
{
	CEntityArray aDirect, aBestones, aRest;
	DWORD dwFlag;
	CTIMeasuredEntity *pRE;
	CWsfTreeItem  *pItem;
	int nCount, t, i, n, nTotalEntities, nGroupBest;

	ASSERT(nLastPart < nNewPart);
	TRACE0("\nBUILD PART ENTITIES\n----------------------\n");

	nCount = 0;						
	nGroupBest = m_pRS->GetAdvanceGroupBest(nLastPart);
	nTotalEntities = m_pRS->GetAdvanceTotal(nLastPart);

	// Add entities with direct advance from qual
	
	if (m_pRS->m_bQual && nNewPart > m_pRS->m_nFirstPart)
	{
		switch (nNewPart)
		{
		case RACE_PART_QUATER	: dwFlag = REF_QUATER; break;
		case RACE_PART_SEMI		: dwFlag = REF_SEMI; break;
		case RACE_PART_FINAL	: dwFlag = REF_FINAL; break;
		default: ASSERT(false); break;
		}

		for (pItem = GetChild(); pItem; pItem = pItem->GetNextSibling())
		{
			if (pItem->IsKindOf(RUNTIME_CLASS(CTIRaceEntity)))
			{
				pRE = (CTIMeasuredEntity*) pItem;
				if (pRE->GetFlags() & dwFlag) aDirect += pRE;
			}
		}

		nCount += aDirect.GetSize();
		nTotalEntities += nCount;
	}

	// Add n best drivers from each group

	for (i = GetNumGroups(); i--;)
	{
		TRACE1("Best from group #%d: ", i+1);
		n = m_aGroup[i].GetSize();
		if (nGroupBest < n) n = nGroupBest;
		
		for (t = n; t--;) 
		{
			aBestones += m_aGroup[i][t]; 
			TRACE2("%s (%d),", m_aGroup[i][t]->GetEntityName(CTIRaceEntity::SURNAME_NAME), m_aGroup[i][t]->m_nRanking);
		}

		nCount += n;
		TRACE0("\n");
	}

	// There should be less or equal entities than allowed in RS

	if (nCount > nTotalEntities)
	{
		AfxMessageBox(IDS_MORE_ENTITIES_THAN_ALLOWED, MB_ICONINFORMATION|MB_OK);
	}

	// Add rest to the total count

	TRACE0("Rest entities:  ");
	for (i = 0; (i < m_aPartEntities.GetSize()) && (nCount < nTotalEntities); i++)
	{
		CTIMeasuredEntity *p = m_aPartEntities[i];
		// If it has direct access to the new part, it shouldn't be in the last part
		ASSERT(aDirect.FindItem(p) == -1); 

		if (aBestones.FindItem(p) == -1) 
		{
			TRACE2("%s (%d),", p->GetEntityName(CTIRaceEntity::SURNAME_NAME), p->m_nRanking);
			aRest += p;
			nCount++;
		}
	}
	TRACE0("\n");

	// If nCount is less, there was not enough entities in the
	// m_aPartEntities array (= not enough ent. in the last part)

	if (nCount < nTotalEntities)
	{
		AfxMessageBox(IDS_LESS_ENTITIES_THAN_WANTED, MB_ICONINFORMATION|MB_OK);
	}

	// Build the final array

	// We sort by ranking from qual. (Entity.m_nRanking = ranking from qual, because
	// the driver hasn't raced unil now.)
	aDirect.Sort((CWslBaseArray::SORT_CALLBACK*) __EntityRankingAscSortCallback, NULL);

	aBestones += aRest;
	aBestones.Sort((CWslBaseArray::SORT_CALLBACK*) __EntityRankingAscSortCallback, NULL);

	m_aPartEntities = aDirect;
	m_aPartEntities += aBestones;

	// Set ranking to (1..?)

	for (i = m_aPartEntities.GetSize(); i--;) 
		m_aPartEntities[i]->m_nRanking = i + 1;
}

//*****************************************************************************
void CTIRace::BuildGroupScheme(CWslArray<int> &aGroupScheme)
{
	int nNumLanes, nNumEntities, i;

	nNumLanes = GetNumLanes();
	nNumEntities = m_aPartEntities.GetSize();
	aGroupScheme.RemoveAll();
	
	i = nNumEntities % nNumLanes;
	if (i) aGroupScheme += i;

	for (i = nNumEntities / nNumLanes; i--;) aGroupScheme += nNumLanes;
}

//*****************************************************************************
void CTIRace::ClearGroups()
{	
	m_nActiveGroup = -1;
	m_nNumGroups = 0;
	for (int i = MAX_GROUPS; i--;) m_aGroup[i].RemoveAll();
}

//*****************************************************************************
void CTIRace::BuildGroups()
{
	CWslArray<int>	aGroupScheme;				// Division into the groups	 (88881, 888A)

	ClearGroups();

	// Build group scheme

	BuildGroupScheme(aGroupScheme);
	m_nNumGroups = aGroupScheme.GetSize();

	// Build groups

	switch (m_pRS->m_nDistributionType)
	{
	case RS_DISTR_TYPE_WORST_BEST:
		BuildGroups_WorstBest(aGroupScheme);
		break;
		
	case RS_DISTR_TYPE_EVEN:
		BuildGroups_Even(aGroupScheme);
		break;
		
	case RS_DISTR_TYPE_RANDOM:
		BuildGroups_Random(aGroupScheme);
		break;
	}

	// Sort entities in groups by ranking

	for (int i=m_nNumGroups; i--;)
	{
		m_aGroup[i].Sort((CWslBaseArray::SORT_CALLBACK*) __EntityRankingAscSortCallback, NULL);
	}

	// Update UI

	g_pApp->UpdateAllDocs(NULL, UV_GROUPS_HAS_CHANGED, NULL);	
	g_pApp->SetWorkspaceModified();
}

//*****************************************************************************
void CTIRace::BuildGroups_WorstBest(CWslArray<int> &aGroupScheme)
{
	int i, nGroup, nMaxCount, nCurCount;

	nGroup = -1;
	nMaxCount = 0;
	nCurCount = 0;
	TRACE0("\nBUILD GROUPS - WORST BEST\n");

	for (i = m_aPartEntities.GetSize(); i--;)
	{
		if (nCurCount == nMaxCount)
		{
			nGroup++;
			nCurCount = 0;
			nMaxCount = aGroupScheme[nGroup];
			TRACE0("---------------------------\n");
		}
		
		m_aGroup[nGroup] += m_aPartEntities[i];
		nCurCount ++;
		TRACE1("%s\n", m_aPartEntities[i]->GetEntityName(CTIRaceEntity::SURNAME_NAME));
	}
}

//*****************************************************************************
void CTIRace::BuildGroups_Even(CWslArray<int> &aGroupScheme)
{
	int n, i, nGroup;

	n = m_aPartEntities.GetSize();
	nGroup = -1;
	TRACE0("\nBUILD GROUPS - EVEN\n");

	for (i=0; i<n; i++)
	{
		// Get next free group

		while (1)
		{
			if (++nGroup >= aGroupScheme.GetSize()) nGroup = 0;
			if (m_aGroup[nGroup].GetSize() < aGroupScheme[nGroup]) break;
		}

		// Add entity

		m_aGroup[nGroup] += m_aPartEntities[i];
	}
}

//*****************************************************************************
void CTIRace::BuildGroups_Random(CWslArray<int> &aGroupScheme)
{
	int n, i, nGroup;

	n = m_aPartEntities.GetSize();
	nGroup = -1;
	TRACE0("\nBUILD GROUPS - RANDOM\n");

	for (i=0; i<n; i++)
	{
		// Get free group

		while (1)
		{
			nGroup = rand() % aGroupScheme.GetSize();
			if (m_aGroup[nGroup].GetSize() < aGroupScheme[nGroup]) break;
		}

		// Add entity

		m_aGroup[nGroup] += m_aPartEntities[i];
	}
}

//*****************************************************************************
int CTIRace::SetActiveGroup(int nGroup, CWslArray<int> *paFirstLanes)
{
	int i,t,n,nLen, nPauseStartIdx = -1;
	CString str;
	CDlgFirstLanes d(this);
	CWslArray<int> aFirstLanes;

	if (nGroup < 0) return 0;
	if (nGroup >= GetNumGroups()) return 0;
//	ASSERT(nGroup >=0 && nGroup < GetNumGroups());

	m_nActiveGroup = nGroup;

	// Build changing string with pausing (134pp2)

	m_aActiveGroupChanging.RemoveAll();
	str = m_pRS->m_strChanging;
	nLen = str.GetLength();
	ASSERT(nLen == GetNumLanes());
	TRACE0("Changing string: ");

	for (i=0; i<nLen; i++)
	{
		n = str[i] - '0';	
		m_aActiveGroupChanging += n - 1;
		TRACE1("%d", n);

		// We must place the pausing symbols after the last lane.
		// By only if there is more drivers than lanes!

		if (n == GetNumLanes())	
		{
			n -= m_aGroup[nGroup].GetSize();
			if (n < 0) 
			{
				nPauseStartIdx = i + 1;
				for (t=-n; t--;) { m_aActiveGroupChanging += LANE_PAUSE; TRACE0("P"); }
			}
		}
	}
	TRACE0("\n");

	// Init paFirstLanes array

	if (!paFirstLanes)
	{
		//if (m_nRacePart == RACE_PART_FINAL)
		{
			d.DoModal();
			d.GetLanes(aFirstLanes);
		}
		/*else
		{
			// Auto generate first lanes. The better you are the lesser lane you have.
			t = m_aGroup[nGroup].GetSize();
			n = GetNumLanes();
			for (i = 0; i<t; i++) aFirstLanes += (i>=n ? LANE_PAUSE : i);
		}*/
		paFirstLanes = &aFirstLanes;
	}
	
	n = paFirstLanes->GetSize();
	ASSERT(n == m_aGroup[nGroup].GetSize());

	// Convert lane indexes to m_aActiveGroupChanging indexes

	m_aFirstLanes.SetSize(n);
	
	for (i=n; i--;)
	{
		n = paFirstLanes->GetAt(i);		
		if (n == LANE_PAUSE) 
		{
			ASSERT(nPauseStartIdx != -1);		// we must support pausing
			m_aFirstLanes[i] = nPauseStartIdx++; // move to the next pause index
		}
		else
		{
			m_aFirstLanes[i] = m_aActiveGroupChanging.FindItem(n);
			ASSERT(m_aFirstLanes[i] != -1);
		}
	}

	return 1;

}

//*****************************************************************************
int	CTIRace::GetNumRides()
{
	ASSERT(m_nActiveGroup != -1);
	return max(GetNumLanes(), m_aGroup[m_nActiveGroup].GetSize());
}

//*****************************************************************************
void CTIRace::AssignLanes(int nRide)
{
	int nLen, n, i;

	ASSERT(m_nActiveGroup != -1);
	nLen = m_aActiveGroupChanging.GetSize();
	
	m_aTrackMappedEntities.SetSize(MAX_LANES);
	for (i = MAX_LANES; i--;) m_aTrackMappedEntities[i] = NULL;

	for (i = m_aFirstLanes.GetSize(); i--;)
	{
		n = m_aFirstLanes[i] + nRide;
		if (n >= nLen) n -= nLen;
		n = m_aActiveGroupChanging[n];
		m_aGroup[m_nActiveGroup][i]->m_nLane = n;
		if (n != LANE_PAUSE) m_aTrackMappedEntities[n] = m_aGroup[m_nActiveGroup][i];
	}
}

/*****************************************************************************
*
* COMMANDS
*
*****************************************************************************/

/*****************************************************************************/
void CTIRace::OnWsfOpen() 
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
