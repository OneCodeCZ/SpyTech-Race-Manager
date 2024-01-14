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

BEGIN_MESSAGE_MAP(CTIRaceEntity, CWsfTIFolder)
	//{{AFX_MSG_MAP(CTIRaceEntity)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CTIRaceEntity, CWsfTIFolder, 1)

//*****************************************************************************
CTIRaceEntity::CTIRaceEntity()
{
}

//*****************************************************************************
CTIRaceEntity::CTIRaceEntity(DWORD dwId) 
{
	m_nStartRanking = 0;
	m_nRanking = 0;
	m_dwId = dwId;
}

//*****************************************************************************
void CTIRaceEntity::Serialize(CArchive &ar)
{
	CWsfTIFolder::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_nStartRanking;
		ar << m_nRanking;
		ar << m_dwId;
	}
	else
	{
		ar >> m_nStartRanking;
		ar >> m_nRanking;
		ar >> m_dwId;
	}
}

//*****************************************************************************
CString CTIRaceEntity::GetTitle() const 
{ 
	CString sTitle;
	sTitle.Format("%s", GetEntityName(NICKNAME));
	return sTitle;
}

//*****************************************************************************
void CTIRaceEntity::Reset()
{
	m_nRanking = m_nStartRanking;
}

