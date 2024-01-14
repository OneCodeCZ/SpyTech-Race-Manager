/******************
*                 *  TIMeasuredEntity.h
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

#pragma once

/*******************************************************************************
*
*  CRideData
*
*******************************************************************************/

class CRideData : public CObject{
public:

	// --------------------------------------------------------------------------
	// VARIABLES
	
	int m_nTrackAssigned;    // 0 - 7 .. track number, -1 .. pause (no track)
                            // Remark: if -1 is written, all data are invalid!
	DWORD m_dwNumLaps;
	DWORD m_dwNumHdts;
	DWORD m_dwBestTime;
	DWORD m_dwLastTime;

	// --------------------------------------------------------------------------
	// METHODS

	CRideData();
	~CRideData();

	void StoreLap() { m_dwNumLaps++; };
	void StoreTime(DWORD dwTime);
	void SetTrack(int nTrack) { m_nTrackAssigned = nTrack; };
	DWORD GetBestTime() { return m_dwBestTime; };

	void Load(CArchive &ar);
	void Save(CArchive &ar);

	virtual void Serialize(CArchive &ar);

	DECLARE_SERIAL(CRideData);
};

/*******************************************************************************
*
*  CEventData
*
*******************************************************************************/

class CEventData : public CObject {

	DWORD m_dwTempTotalLaps;
	DWORD m_dwTempTotalHdts;

public:

	// --------------------------------------------------------------------------
	// VARIABLES

	CWslArray<CRideData*> m_pArrRides;
	int m_nPenalization;

	// --------------------------------------------------------------------------
	// METHODS

	CEventData(); // Destructor is a standard one
	~CEventData();

	CRideData *GetCurrentRide();
	CRideData *GetPreviousRide();

	DWORD GetNumLaps();
	DWORD GetNumHdts();
	DWORD GetTotalLaps();
	DWORD GetTotalHdts();   // A GetTotalLaps() call MUST precede a call of this

	DWORD GetBestLap();

	void ForgetCurrentRide();
	void AppendNewRide(int nTrack);
	virtual void Serialize(CArchive &ar);

	void Reset();

	DECLARE_SERIAL(CEventData);
};

/*******************************************************************************
*
*  CTIMeasuredEntity
*
*******************************************************************************/

class MEASURE_API CTIMeasuredEntity : public CTIRaceEntity
{
public:

	// --------------------------------------------------------------------------
	// CONSTRUCTION / DESTRUCTION
	
	CTIMeasuredEntity();
	CTIMeasuredEntity(DWORD dwId);
	virtual ~CTIMeasuredEntity();

	DECLARE_SERIAL(CTIMeasuredEntity);
	DECLARE_MESSAGE_MAP()

public:

	// --------------------------------------------------------------------------
	// OVERALL 

	// Set TRUE for all parts whose data are valid for this (Radek's idx)
	int m_nDataAreValid[8];

	// 0 .. consi
	// 1 .. quarter
	// 2 .. semi
	// 3 .. final
	// (-1 .. qual)
	int m_nCurrentEvent;   // Contains the current event idx (AmJ's idx)

	// Reset all the data gathered.
	virtual void Reset();
	void AddDebugPropertySheet(CWsfPropertySheet &s);
	CTIMeasuredRace *GetRace();

	// --------------------------------------------------------------------------
	// QUALIFICATION -- VARIABLES

	CWslArray<DWORD> m_dwArrTimes;    // List of times in one attempt
	CWslArray<DWORD> m_dwArrNumLaps;  // List of num rounds in one ride
	CWslArray<DWORD> m_dwArrNumHdts;  // List of num hdts in one ride
	DWORD   m_dwTimeElapsed;          // Ammount of time that has been already elapsed
	BOOL    m_bWasAction;             // At least one ride or attempt has been stored
	int     m_nCurrentRide;
	int     m_nDisplayLine;           // Line at which this entity resides

   int     m_dwQualTransTimeElapsed;
	int     m_nQualTransCurrentRide;
	int     m_bQualTransWasAction;
	int     m_nQualTransPointer;      // Pointer for transaction rollbacker.

	// --------------------------------------------------------------------------
	// QUALIFICATION -- METHODS

	// Compares with given race entity
	// RETURNS:
	//   -1 .. this one is worse than the other
	//   0 .. they are equal
	//   1 .. this one is better than the other
public:
	int QualCompareWith(CTIMeasuredEntity *pOther, int nEvalMethod);

	void QualBeginTransaction();
	// Store a new time data for the best time resolution - result of one round
	void QualStoreTime(DWORD dwTime);
	// Store a result of one whole ride
	void QualStoreLaps(DWORD dwNumLaps, DWORD dwHdts);
	// Retrieve best times of the racer
	void QualGetBestTimes(DWORD *dw1, DWORD *dw2);
	// Retrieve number of rounds
	void QualGetNumLaps(DWORD *dwLaps, DWORD *dwHdts);
	// Call this at the start of racer's ride
	void OnQualNewRide() { m_nCurrentRide++; };
	void QualRollback();

   int GetCurrentQualRank();

	// Get information string on qualification
	CString QualReport(int nEvalMethod);

	// Resets all the data as if no qualification has been ever performed
	void QualReset();

	// --------------------------------------------------------------------------
	// RACE -- VARIABLES

	CWslArray<CEventData*> m_pArrEvents;
	int m_nCurrentTrack;
	int m_nCurrentOrder;
	DWORD m_dwPrognosis;

	// --------------------------------------------------------------------------
	// RACE -- METHODS

	CEventData *GetCurrentEvent();

	void ResetIntoEvent(int nIdx);
	void ResetBeforeRide(int nIdx);

	// RETURNS:
	//   -1 .. this one is worse than the other
	//    0 .. they are equal
	//    1 .. this one is better than the other
	int RaceCompareWith(CTIMeasuredEntity *pOther);
	int RaceFinalCompareWith(CTIMeasuredEntity *pOther);

	// Call this to swith the measurement to gather data for given race event.
	// (No affect of qualification function calls.)
	void AdvanceToEvent(int nIdx);

	// Data of the current event. Set by AdvanceToEvent.
	CEventData *m_pED;

	// --------------------------------------------------------------------------
	// SERIALIZATION

	virtual void Serialize(CArchive &ar);
	
	//{{AFX_MSG(CTIMeasuredEntity)
	//}}AFX_MSG
};

/*********************************************************************** eof **/
