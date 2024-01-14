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

//*****************************************************************************
void CSetupTrack::Serialize(CArchive &ar)
{
	if (ar.IsStoring())
	{
		ar << m_strOwner;
		ar << m_strTrackName;
		ar << m_nTrackLength;
		ar << m_nNumLanes;
		ar << m_nDivisionType;
		ar.Write(&m_nLaneColor, sizeof(m_nLaneColor));
	}
	else
	{
		ar >> m_strOwner;
		ar >> m_strTrackName;
		ar >> m_nTrackLength;
		ar >> m_nNumLanes;
		ar >> m_nDivisionType;
		ar.Read(&m_nLaneColor, sizeof(m_nLaneColor));
	}
}

//*****************************************************************************
COLORREF CSetupTrack::GetLaneColor(int nLane)
{
//	ASSERT(nLane >=0 && nLane < GetNumLanes());
	return g_rgbColors[m_nLaneColor[nLane]];
}

//*****************************************************************************
CSetupTrack& CSetupTrack::operator= (const CSetupTrack &st)
{
	m_strOwner = st.m_strOwner;
	m_strTrackName = st.m_strTrackName;
	m_nTrackLength = st.m_nTrackLength;
	m_nNumLanes = st.m_nNumLanes;
	m_nDivisionType = st.m_nDivisionType;
	memcpy(m_nLaneColor, st.m_nLaneColor, sizeof(m_nLaneColor));

	return *this;
}







