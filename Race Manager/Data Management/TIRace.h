///////////////////////////////////////////////////////////////////////////////
//
// Race Commander
// --------------
// 
// Copyright (C)2000 SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#define RACE_NAME_MAX_CHARS						63
#define RACE_ORGANIZER_MAX_CHARS			63
#define RACE_TRACK_MAX_CHARS					63

// Race parts (states)

#define RACE_PART_NONE			1			// tvorba
#define RACE_PART_QUAL			2
#define RACE_PART_CONSI			3
#define RACE_PART_QUATER		4
#define RACE_PART_SEMI			5				
#define RACE_PART_FINAL			6
#define RACE_PART_DONE			7

#define LANE_PAUSE					100

//*****************************************************************************
class RMDM_API CTIRace : public CWsfTIFolder, public CSetupTrack
{
public:
	// Race properties 
	CString					m_strName;
	CString					m_strOrganizer;
	CString         m_strNumber;
	CString         m_strJudge;
	CString         m_strPlace;
	BOOL						m_bTeams;
	CWslDate				m_Date;
	int							m_nLapMinTime;		// in ms
	int							m_nMinMembers;		
	int							m_nMaxMembers;

	// Other
	DWORD						m_dwIdCounter;
	int							m_nRacePart;					// The part the is about to be messured
	BOOL						m_bPartInProgress;		// true = something has been messured
	BOOL						m_bPartFinished;			// true if whole part has been meassured

	int							m_nNumGroups;
	CEntityArray		m_aGroup[MAX_GROUPS];
	int							m_nActiveGroup;				// -1 = none
	CWslArray<int>	m_aFirstLanes;				// Indexes into m_aActiveGroupChanging;
	CWslArray<int>	m_aActiveGroupChanging;

	CEntityArray		m_aPartEntities;			// Entities in the current part 
	CEntityArray		m_aTrackMappedEntities;	// Entities on each track

protected:
	CTIRS			*m_pRS;

protected:
	CTIRace();		// Called from serialization only
public:
	CTIRace(CSetupTrack *pSetupTrack);

	// --------------------------------------------------------------------------
	//  Race building
	// --------------------------------------------------------------------------

	// Sets RS for the race. If there is already a RS, it will
	// be replaced.
	void SetRS(CTIRS *pRS);
	inline CTIRS *GetRS() { return m_pRS; }

	// Adds driver to the race. It can be moved.
	// Returns Win32 error code.
	DWORD AddDriver(CTIDriver *pTIDrv, CTIRaceTeam *pTITeam, BOOL bMove);

	// Finds race entity in race. strName must be in NICKNAME format.
	CTIRaceEntity *FindEntity(CString &strName);

	// Fills the list with race entities
	void BuildEntityList(CListCtrl &List);

	// Finds the driver in teams
	CTIDriver *FindTeamDrv(const char *pcszName, const char *pcszSurname);

	// Returns a free entity ID.
	inline DWORD GetNextEntityId() { return m_dwIdCounter++; }

	// Converts race part code to string
	CString GetRacePartName(int nRacePart) const;

	// --------------------------------------------------------------------------
	//  Methods for running the race
	// --------------------------------------------------------------------------

	// Builds the group scheme = calculates the number of groups and number of
	// entities in each group.
	// Used internally by the BuildGroups() method.

	void BuildGroupScheme(CWslArray<int> &aGroupScheme);

	// Clears groups;
	void ClearGroups();

	// Creates groups from entities in m_aPartEntities array.
	// Builds m_aGroup array and sets m_nNumGroups
	void BuildGroups();
	void BuildGroups_WorstBest(CWslArray<int> &aGroupScheme);
	void BuildGroups_Even(CWslArray<int> &aGroupScheme);
	void BuildGroups_Random(CWslArray<int> &aGroupScheme);

	// Returns number of groups. Obsolete now. m_nNumGrouos can be used
	// directly.
	inline int GetNumGroups() { return m_nNumGroups; }

	// Fills the m_aPartEntities array for the next part.
	void BuildPartEntities(int nLastPart, int nNewPart);

	// Sets direct advances from qual and removes such entities from
	// the m_aPartEntities array.
	void HandleQualFinish();

	// Called when race part is changed from RACE_PART_NONE to something else
	// If bEntitiesDescOrder is FALSE, entities are sorted in the ascending
	// order, otherwise in the descending order.
	void HandleFirstPart(BOOL bEntitiesDescOrder);

	// Called to return to the "tvorba zavodu" level.
	virtual void OnResetRace();

	// Called to start or continue measuring. 
	// Checks the race and if it can be measured returns TRUE.
	virtual BOOL OnRaceNextPart();

	// Called when the user finishes building the race ("Ukoncit tvorbu zavodu")
	// and before the race is saved.
	// If it returns FALSE, the operation is cancelled.
	virtual BOOL OnRaceStart();

	// Called when the measuring of the race part is done.
	// Method will prepare the race for the next part.
	virtual void OnRacePartFinished();

	// Returns points (in thousendths) of given position.
	// nPos is zero based.
	int GetPoints(int nPos, int nNumEntities);

	// --------------------------------------------------------------------------
	//  Methods for managing a group
	// --------------------------------------------------------------------------

	// Sets active group.
	// paFirstLanes = zero based lane indexes or LANE_PAUSE.
	// If NULL than dialog DlgFirstLanes is displayed.
	// Returns 0 if there was error (incorrect group index).
	// Does not contain the ASSERT anymore.
	// (Change made by AmJ 00/08/27)
	virtual int SetActiveGroup(int nGroup, CWslArray<int> *paFirstLanes);

	virtual void AssignLanes(int nRide);

	int	GetNumRides();

	// --------------------------------------------------------------------------
	//  Methods for measuring 
	// --------------------------------------------------------------------------

	virtual void OnRunQualification() { ASSERT(false); /* override */ }
   virtual void OnRunRace() { ASSERT(false); /* override */}

	// --------------------------------------------------------------------------
	//  Helpers
	// --------------------------------------------------------------------------

	// Fills the given array with all entities in the race.
	void BuildEntitiesArray(CEntityArray &a);

	// --------------------------------------------------------------------------
	//  From CWsfTIFolder
	// --------------------------------------------------------------------------

	virtual CString GetTitle() const;
	virtual int OnCompare(CWsfTreeItem *pItem1, CWsfTreeItem *pItem2);
	virtual void Serialize(CArchive &ar);
	virtual DROPEFFECT OnAcceptDataObject(COleDataObject *pDataObject);
	virtual void OnDropDataObject(COleDataObject *pDataObject, DROPEFFECT dropEffect);
	virtual BOOL OnInitProperties(CWsfPropertySheet &s);
	virtual BOOL OnCloseProperties(CWsfPropertySheet &s, BOOL bOk);

protected:
	DECLARE_SERIAL(CTIRace);
	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CTIRace)
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
	afx_msg void OnNewTeam();
	afx_msg void OnUpdateNewTeam(CCmdUI* pCmdUI);
	afx_msg void OnWsfOpen();
	//}}AFX_MSG
};
