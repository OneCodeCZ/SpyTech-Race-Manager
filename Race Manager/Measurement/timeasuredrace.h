/******************
*                 *  TIMeasuredRace.h
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
*  Thread stuff
*
*******************************************************************************/

extern UINT g_dwThreadCmd;                   // Zero if no command
extern UINT g_dwThreadKey;                   // Zero if no key
extern HANDLE g_hEventKeyPressed;

DWORD WINAPI QualThreadProc(LPVOID lpParameter);
DWORD WINAPI RaceThreadProcEx(LPVOID lpParameter);
DWORD WINAPI TrainingThreadProc(LPVOID lpParameter);
DWORD WINAPI SemiautoTrainingThreadProc(LPVOID lpParameter);
void CreateEvents();

/*******************************************************************************
*
*  CTIMeasuredRace
*
*******************************************************************************/

#define DC_ME(x) (dynamic_cast<CTIMeasuredEntity*>(x))

#define WM_SUCCESS       (WM_USER+0)
#define WM_INTERRUPTED   (WM_USER+1)
#define WM_INCORRECT     (WM_USER+2)
#define WM_SIMPLE_END    (WM_USER+3)
#define WM_GROUP_SUCCESS (WM_USER+4)

class CRaceUIWnd;
class CTIResults;

#undef AFX_API
#define AFX_API AFX_EXT_CLASS

class MEASURE_API CTIMeasuredRace : public CTIRace
{
public:

	// In this array, the sorted entities will be stored for the displays
	CEntityArray m_pArrSortedEntities;

	BOOL m_bNeedRecovery;
	BOOL m_bPartEndedButNotAccepted;
	BOOL m_bPauseBetweenRides;

	CTIResults *m_pToBeAdded;

	// --------------------------------------------------------------------------
	// CONSTRUCTION / DESTRUCTION

	CTIMeasuredRace();
	CTIMeasuredRace(CSetupTrack *pSetupTrack);
	virtual ~CTIMeasuredRace();

	CString GetRacePartResultSheetTitle(int nRacePart) const;

	// --------------------------------------------------------------------------
	// OVERLOADED FUNCTINOS FROM CTIRace

	virtual void OnRunQualification();
	virtual void OnRunRace();
	virtual void OnResetRace();
	virtual void OnRacePartFinished();
	virtual BOOL OnRaceNextPart();
	virtual BOOL OnRaceStart();

	// --------------------------------------------------------------------------
	// NATIVE FUNCTIONS -- COMMON STUFF

	int GetEntityIdx(CTIMeasuredEntity *pEntity);
	CTIMeasuredEntity* GetIdxEntity(int nIdx);

	CWsfTreeItem *GetResultsFolder();
	CWsfTreeItem *GetFirstEntity();
	void AddResultChild(CTIResults *pResults);

	void InitFullScreen();
	void DestroyEvents();

	void ResetCurrentEvent();

	DWORD m_dwThreadID;
	HANDLE m_hThread;

	// --------------------------------------------------------------------------
	// NATIVE FUNCTIONS -- QUALIFICATION

	void SpawnQualificationThread();

	// These variables point to the current racer that is measured or is going to
	// be measured after the pause.
	int m_nQualRacer, m_nQualRacerHld;
	int m_nQualAttempt, m_nQualAttemptHld;
	int m_nQualRide, m_nQualRideHld;

	int QualSortRacers(int nMode = 0);
	void QualLoadPosition();
	void QualSavePosition();

	// QualAdvance:
	//   Advance that QSVs point to the next racer in list. The proper TS/RS
	//   settings are considered.
	// RETURNS:
	//   0 .. if no more racers are available
	//   >0 .. if the QSVs point to a racer that has to be measured
	// Moreover, these values are returned:
	//   1 .. if the same racer is on the start (the next ride)
	//   2 .. if next racer is on the start
	int QualAdvance();

	// QualGetNextRacers
	//   Get list of racers that will follow this one.
	// PARAMETERS:
	//   ppNextEntities .. array where these entities are to be stored
	//   dwNumEntities .. number of entities that we want to retrieve
	void QualGetNextRacers(CTIRaceEntity** ppNextEntities, DWORD dwNumEntities);

	CTIMeasuredEntity *QualGetCurrentEntity() { return m_aPartEntities[m_nQualRacer]; };

	// --------------------------------------------------------------------------
	// REPORT FUNCTIONS

	CFile file;
	BOOL ReportInit(const char *pcszReportName=NULL);
	void ReportDone();
	BOOL QualReportResults();
	void QualReportRide(CTIMeasuredEntity *pEntity);
	BOOL RaceReportGroup(CEntityArray &rArray, BOOL bLastLap, const char *pcszReportName);

	// --------------------------------------------------------------------------
	// NATIVE FUNCTIONS -- RACE

	void SpawnRaceThread();

	int m_nCurrentRide;
	CEntityArray m_pArrSortedGroup;

	// ASSUMED: Current group (m_aGroup) already copied into m_pArrSortedGroup.
	// USAGE: To enter the screen setting up and measurement with m_pArrSortedGroup
	//        variable properly initialized.
	void RaceSortRacers(CEntityArray &rArray);

	void InitGroupRacers();
	virtual int SetActiveGroup(int nGroup, CWslArray<int> *paFirstLanes);
	virtual void AssignLanes(int nRide);

	void ForgetLastRide();

	void CreateFinalResultsList();

	// --------------------------------------------------------------------------
	// NATIVE FUNCTIONS -- DISPLAY STUFF

	CRaceUIWnd *m_pWnd;
	void ShowFullScreen();
	void HideFullScreen();

	// OnSuccess
	//   Causes the race to store results and end the race part
	// PARAMETERS:
	//   bHideScreen .. only when the full screen is shown, this parameter might
	//   be TRUE. Otherwise the app crashes
	void OnSuccess(BOOL bHideScreen=TRUE);
	void OnInterrupt(BOOL bHideScreen=TRUE);
	void OnIncorrect(BOOL bHideScreen=TRUE);

	void OnGroupSuccess();
	void OnFinalResultsList();

  //---------------------------------------------------------------------------
	// From WSF
	//---------------------------------------------------------------------------

	virtual void Serialize(CArchive &ar);

protected:
	DECLARE_SERIAL(CTIMeasuredRace)

	DECLARE_MESSAGE_MAP()

	//{{AFX_MSG(CTIMeasuredRace)
	//}}AFX_MSG
};

#undef AFX_API
#define AFX_API

/*********************************************************************** eof **/
