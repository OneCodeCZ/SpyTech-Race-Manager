/******************
*                 *  EventData.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  17-AUG-2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"
#include "TIMeasuredRace.h"
#include "TIMeasuredEntity.h"
#include "version.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  CRideData
*
*******************************************************************************/

IMPLEMENT_SERIAL(CRideData, CObject, 1)

CRideData::CRideData() {
	m_dwBestTime = 0;
	m_dwNumHdts = 0;
	m_dwNumLaps = 0;
	m_nTrackAssigned = -1;
	m_dwLastTime = -1;
}

void CRideData::StoreTime(DWORD dwTime) {
	if (dwTime > 3600000) return;
	m_dwLastTime = dwTime;
	if (!m_dwBestTime) m_dwBestTime = dwTime;
	else m_dwBestTime = min (dwTime, m_dwBestTime);
}

CRideData::~CRideData() {
}

void CRideData::Serialize(CArchive &ar) {

	CObject::Serialize(ar);

	DWORD dwVersion;

	// --------------------------------------------------------------------------
	// STORE

	if (ar.IsStoring()) {
		ar << (DWORD) MEASUREMENT_FILES_VERSION;
		ar << m_nTrackAssigned;
		if (m_nTrackAssigned != -1) {
			ar << m_dwBestTime;
			ar << m_dwNumHdts;
			ar << m_dwNumLaps;
		}
	}

	// --------------------------------------------------------------------------
	// LOAD

	if (ar.IsLoading()) {
		ar >> dwVersion;
		ar >> m_nTrackAssigned;
		if (m_nTrackAssigned != -1) {
			ar >> m_dwBestTime;
			ar >> m_dwNumHdts;
			ar >> m_dwNumLaps;
		}
	}

}

/*******************************************************************************
*
*  CEventData
*
*******************************************************************************/

IMPLEMENT_SERIAL(CEventData, CObject, 1)

CEventData::CEventData() {
	Reset();
	m_pArrRides.SetSize(0);
}

/******************************************************************************/

void CEventData::Serialize(CArchive &ar) {
	
	CObject::Serialize(ar);

	DWORD dwVersion;
	int nNum;

	CString s;

	// --------------------------------------------------------------------------
	// SAVE

	if (ar.IsStoring()) {
		ar << (DWORD) MEASUREMENT_FILES_VERSION;
		ar << m_dwTempTotalHdts;
		ar << m_dwTempTotalLaps;

		ar << m_nPenalization;

		nNum = m_pArrRides.GetSize();
		ar << nNum;
		for (int a=0; a<nNum; a++) ar << m_pArrRides[a];
	}

	// --------------------------------------------------------------------------
	// LOAD

	if (ar.IsLoading()) {
		m_pArrRides.SetSize(0);

		ar >> dwVersion;
		ar >> m_dwTempTotalHdts;
		ar >> m_dwTempTotalLaps;

		if (dwVersion >= 0x20010621) ar >> m_nPenalization;

		ar >> nNum;
		for (int a=0; a<nNum; a++) {
			CRideData *pRide;
			ar >> pRide;
			m_pArrRides.Append(pRide);
		}
		ASSERT(m_pArrRides.GetSize() == nNum);
	}
}

/******************************************************************************/

void CEventData::Reset() {
	int nIdx;
	m_nPenalization = 0;
	ITERATE(m_pArrRides, nIdx) delete m_pArrRides[nIdx];
	m_pArrRides.SetSize(0);
}

/******************************************************************************/

CEventData::~CEventData() {
	Reset();
}

/******************************************************************************/

CRideData *CEventData::GetCurrentRide() { 
	return m_pArrRides.GetSize() ? m_pArrRides[m_pArrRides.GetSize()-1] : 0; 
};

/******************************************************************************/

CRideData *CEventData::GetPreviousRide() { 
	int nIdx = m_pArrRides.GetSize()-2;
	if (nIdx >= 0) return m_pArrRides[nIdx];
	else return 0;
};

/******************************************************************************/

DWORD CEventData::GetNumHdts() {
	if (!GetCurrentRide()) return 0;
	return GetCurrentRide()->m_dwNumHdts;
}

/******************************************************************************/

DWORD CEventData::GetNumLaps() {
	if (!GetCurrentRide()) return 0;
	return GetCurrentRide()->m_dwNumLaps;
}

/******************************************************************************/

DWORD CEventData::GetTotalLaps() {
	int nIdx;
	m_dwTempTotalLaps=0;
	m_dwTempTotalHdts=0;
	ITERATE (m_pArrRides, nIdx) {
		m_dwTempTotalLaps+=m_pArrRides[nIdx]->m_dwNumLaps;
		m_dwTempTotalHdts+=m_pArrRides[nIdx]->m_dwNumHdts;
	}
	m_dwTempTotalLaps -= m_nPenalization;
	return m_dwTempTotalLaps;
}

/******************************************************************************/

DWORD CEventData::GetBestLap() {
	int nIdx;
	DWORD dwBestLap = 100000;
	ITERATE (m_pArrRides, nIdx) {
		DWORD dwLapOfThis = m_pArrRides[nIdx]->m_dwBestTime;
		if (dwLapOfThis < dwBestLap && dwLapOfThis > 0) {
			dwBestLap = dwLapOfThis;
		}
	}
	return dwBestLap;
}

/******************************************************************************/

DWORD CEventData::GetTotalHdts() {
	return m_dwTempTotalHdts;
}

/******************************************************************************/

void CEventData::ForgetCurrentRide() {
	if (!GetCurrentRide()) return;
	m_pArrRides.ShrinkBy(1);
}

/******************************************************************************/

void CEventData::AppendNewRide(int nTrack) {
	CRideData *pRide = new CRideData;
	pRide->m_nTrackAssigned = nTrack;
	m_pArrRides.Append(pRide);
}

/*********************************************************************** eof **/
