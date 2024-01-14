///////////////////////////////////////////////////////////////////////////////
//
// Race Manager
// ------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

// CSetupTrack = common setup stuff for race manager and race

#pragma once

#define SETUP_NUM_LANES_4			0
#define SETUP_NUM_LANES_6			1
#define SETUP_NUM_LANES_8			2

#define SETUP_DIVISION_TYPE_UNITS			0
#define SETUP_DIVISION_TYPE_HALFS			1

//*****************************************************************************
class RMDM_API CSetupTrack
{
public:
	// Track page
	CString	m_strOwner;
	CString	m_strTrackName;
	int			m_nTrackLength;		// in CM
	int			m_nNumLanes;
	int			m_nLaneColor[8];
	int			m_nDivisionType;

public:
	void Serialize(CArchive &ar);
	int GetNumLanes() { return m_nNumLanes * 2 + 4; } 
	COLORREF GetLaneColor(int nLane);
	CSetupTrack& operator= (const CSetupTrack &st);
};
