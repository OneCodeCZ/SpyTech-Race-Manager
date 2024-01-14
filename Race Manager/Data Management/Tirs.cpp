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
#include "PgRSBasic.h"
#include "PgRSAdvances.h"
#include "PgRSTimes.h"
#include "PgRSPoints.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CTIRS, CWsfTreeItem)
	//{{AFX_MSG_MAP(CTIRS)
	ON_UPDATE_COMMAND_UI(ID_WSF_DELETE, OnUpdateWsfDelete)
	ON_UPDATE_COMMAND_UI(ID_WSF_PROPERTIES, OnUpdateProperties)
	ON_COMMAND(ID_ADD_TO_RACE, OnAddToRace)
	ON_UPDATE_COMMAND_UI(ID_ADD_TO_RACE, OnUpdateAddToRace)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CTIRS, CWsfTreeItem, 1)

//*****************************************************************************
CTIRS::CTIRS() 
: CWsfTreeItem(IDR_CONTEXT_MENUS,1, IDB_SMALL_ICONS,1, IDB_SMALL_ICONS,1, 0)
{
	m_bQual = false;
	m_nFirstPart = RACE_PART_FINAL;
	m_nDistributionType = RS_DISTR_TYPE_WORST_BEST;
	
	m_nNumRides = 1;
	m_nNumAttempts = 1;
	m_nPrimaryEval = RS_EVAL_FIRST_TIME;

	m_nPointsType = 0;
	m_bEnablePoints = false;

	for (int i=3;i--;) m_nAdvancesQual[i] = 0;
	for (i=3;i--;) { m_nAdvances[i][0] = 1; m_nAdvances[i][1] = 0; }

	for (i = 0; i<5; i++)
	{
		m_Times[i][0] = MAKETIMET(0,0,15);
		m_Times[i][1] = MAKETIMET(0,0,15);
		m_Times[i][2] = MAKETIMET(0,0,15);
	}

	m_bInRace = false;
}

//*****************************************************************************
void CTIRS::operator =(CTIRS &RS)
{
	ASSERT(this != &RS);

	m_strName = RS.m_strName;
	m_bQual = RS.m_bQual;
	m_nFirstPart = RS.m_nFirstPart;
	m_nDistributionType = RS.m_nDistributionType;
	m_nPrimaryEval = RS.m_nPrimaryEval;
	m_nNumRides = RS.m_nNumRides;
	m_nNumAttempts = RS.m_nNumAttempts;
	m_strChanging = RS.m_strChanging;

	memcpy(&m_nAdvancesQual, &RS.m_nAdvancesQual, sizeof(m_nAdvancesQual));
	memcpy(&m_nAdvances, &RS.m_nAdvances, sizeof(m_nAdvances));
	memcpy(&m_Times, &RS.m_Times, sizeof(m_Times));

	m_bEnablePoints = RS.m_bEnablePoints;
	m_nPointsType = RS.m_nPointsType;
	m_aPoints = RS.m_aPoints;
}

//*****************************************************************************
// Versions
// ~~~~~~~~
//  0|2001 03 02|the first version used
//*****************************************************************************
void CTIRS::Serialize(CArchive &ar)
{
	int nVersion;

	CWsfTreeItem::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << (int) 0;
		ar << m_strName;
		ar << m_bQual;
		ar << m_nFirstPart;
		ar << m_nDistributionType;
		ar << m_nPrimaryEval;
		ar << m_nNumRides;
		ar << m_nNumAttempts;
		ar.Write(&m_nAdvancesQual, sizeof(m_nAdvancesQual));
		ar.Write(&m_nAdvances, sizeof(m_nAdvances));
		ar << m_bInRace;
		ar << m_strChanging;
		ar << m_bEnablePoints;			
		ar << m_nPointsType;
		ar << m_aPoints;
		for (int i=0;i<5;i++) for (int t=0;t<3;t++) ar << m_Times[i][t];
	}
	else
	{
		ar >> nVersion;
		ASSERT(nVersion>=0 && nVersion<=0);
		ar >> m_strName;
		ar >> m_bQual;
		ar >> m_nFirstPart;
		ar >> m_nDistributionType;
		ar >> m_nPrimaryEval;
		ar >> m_nNumRides;
		ar >> m_nNumAttempts;
		ar.Read(&m_nAdvancesQual, sizeof(m_nAdvancesQual));
		ar.Read(&m_nAdvances, sizeof(m_nAdvances));
		ar >> m_bInRace;
		ar >> m_strChanging;
		ar >> m_bEnablePoints; 
		ar >> m_nPointsType; 
		ar >> m_aPoints; 
		for (int i=0;i<5;i++) for (int t=0;t<3;t++) ar >> m_Times[i][t];
	}
}

//*****************************************************************************
int CTIRS::GetAdvanceGroupBest(int nRacePart)
{
	ASSERT(nRacePart >= RACE_PART_CONSI && nRacePart <= RACE_PART_SEMI);
	return m_nAdvances[nRacePart - RACE_PART_CONSI][1];
}

//*****************************************************************************
int CTIRS::GetAdvanceTotal(int nRacePart)
{
	ASSERT(nRacePart >= RACE_PART_CONSI && nRacePart <= RACE_PART_SEMI);
	return m_nAdvances[nRacePart - RACE_PART_CONSI][0];
}

//*****************************************************************************
int CTIRS::GetTimeSettings(int nPart, int nType) 
{
	ASSERT(nPart >= RACE_PART_QUAL && nPart <= RACE_PART_FINAL);
	ASSERT(nType >= 0 && nType < 3);
	return m_Times[nPart - RACE_PART_QUAL][nType].GetTotalSeconds() * 1000;
}

//*****************************************************************************
BOOL CTIRS::OnSubTreeDelete()
{
	int n = AfxMessageBox(IDS_DELETE_RS, MB_ICONEXCLAMATION|MB_YESNO);
	return n == IDYES;
}

//*****************************************************************************
BOOL CTIRS::OnInitProperties(CWsfPropertySheet &s)
{
	CPgRSBasic *p = new CPgRSBasic(this, m_bInRace);
	s.AddPage(p);
	s.AddPage(new CPgRSAdvances(this, p));
	s.AddPage(new CPgRSTimes(this));
	s.AddPage(new CPgRSPoints(this));

	s.SetTitle(s.IsWizard() ? IDS_NEW_RS : IDS_RS_PROPERTIES);
	return true;
}

//*****************************************************************************
BOOL CTIRS::OnCloseProperties(CWsfPropertySheet &s, BOOL bOk)
{
	if (bOk && m_bInRace)
	{
		int n = WsfGetCWorkspace()->m_pTIRace->m_nRacePart;
		if (n > RACE_PART_NONE && n < RACE_PART_DONE)
			AfxMessageBox(IDS_SOME_CHANGES_AFTER_RESET, MB_OK|MB_ICONINFORMATION);
		WsfGetWorkspace()->SetModifiedFlag();
	}
	return true;
}

//*****************************************************************************
void CTIRS::OnUpdateWsfDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bInRace);
}

//*****************************************************************************
void CTIRS::OnUpdateProperties(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!IsMultiselection());
}

//*****************************************************************************
void CTIRS::OnAddToRace() 
{
	ASSERT(WsfGetApp()->IsWorkspaceOpened());
	WsfGetCWorkspace()->m_pTIRace->SetRS(this);
}

//*****************************************************************************
void CTIRS::OnUpdateAddToRace(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bInRace && 
		WsfGetApp()->IsWorkspaceOpened() && 
		WsfGetCWorkspace()->m_pTIRace->m_nRacePart == RACE_PART_NONE);
}










