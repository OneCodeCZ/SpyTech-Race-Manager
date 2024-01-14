/******************
*                 *  WndRaceSources.h - sources of watches
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  05/2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

/*******************************************************************************
*
*  HEADER
*
*******************************************************************************/

#include "SlotTrackMonitor.h"

class CSrcHeader : public CWslWatchSource {
//    ~~~~~~~~~~
public:
	CSrcHeader(CWslWatchSource *pOwner);
//	void SetCaptionQualification(const char *pcszRacerName, int nEvalMethod, int nThis, int nTotal);
	void SetCaptionQualification(CTIMeasuredRace *pRace, CTIMeasuredEntity *pEntity);
	void SetCaptionRacePart(const char *pcszPartName, int nGroup, int nTotalGroups, int nSegment, int nTotalSegments);
	void SetCaptionRacePart(CTIMeasuredRace *pRace, BOOL bDispSegment=TRUE);
	void SetCaptionRacePartPause(CTIMeasuredRace *pRace);
	void SetCaptions(const char *s1, const char *s2, const char *s3);
	~CSrcHeader();
};

/*******************************************************************************
*
*  TIME
*
*******************************************************************************/

#define TIME_FORMAT_HMS  1
#define TIME_FORMAT_S    4

#define TIME_MODE_SMALL  0
#define TIME_MODE_LARGE  1

#define TIME_CAPTION_TIME_TO_START           0
#define TIME_CAPTION_TIME_TO_END             1
#define TIME_CAPTION_TIME_TO_WARMUP          9
#define TIME_CAPTION_TRAINING_TIME_ELAPSED   2
#define TIME_CAPTION_TIME_TO_END_WARMUP      3
#define TIME_CAPTION_END                     4
#define TIME_CAPTION_ATTEMPT_END             5
#define TIME_CAPTION_QUAL_LEAVE              6
#define TIME_CAPTION_NOTHING                 7
#define TIME_CAPTION_ZEROES                  8

#define TIME_MODE_COUNTUP    0
#define TIME_MODE_COUNTDOWN  1

class CSrcTime : public CWslWatchSource {
//    ~~~~~~~~
	int m_nMode;
	int m_nFormat;
	int m_nCountMode;

   int m_nLastTimeSeconds;
   int m_nLastTimeTotalMs;
   int m_nLastCountMode;
public:
	CSrcTime(CWslWatchSource *pOwner);
	void SetCaption(int nCaption);
	void SetTime(int nTimeMs, int nTimeTotalMs=0);
	void SetMode(int nMode, int nFormat, int nCountMode=TIME_MODE_COUNTUP);
	~CSrcTime();
};

/*******************************************************************************
*
*  BAR
*
*******************************************************************************/

#define BAR_PROGRESS  0
#define BAR_MONITOR   1

class CSrcBar : public CWslWatchSource {
//    ~~~~~~~
	int m_nStatProgress;
	int m_nStatPowerLed;
	int m_nStatFirst;
	int m_nStatTime;
	int m_nStatDelta;
	int m_nStatCount;
	int m_nStatClock;
	int m_nStatTimer;
	int m_nStatPower;
	int m_nStatInverted;
	int m_nNumStatWatches;

public:
	int m_nMode;

	CSrcBar(CWslWatchSource *pOwner);
	void SetMode(int nMode);
	void SetTime(int nElapsed, int nTotal, int nDiv);
	void SetMonitor(CSlotTrackMonitor *pMonitor, BOOL bWillPause = FALSE);

	~CSrcBar();
};

/*******************************************************************************
*
*  COMMENTS
*
*******************************************************************************/

class CSrcBarComments : public CWslWatchSource {
public:
	CSrcBarComments(CWslWatchSource *pOwner);
	void SetLine(CString s);
	~CSrcBarComments();
};

/*******************************************************************************
*
*  QUALIFICATION -- MAIN SCREEN
*
*******************************************************************************/

class CSrcQualMain : public CWslWatchSource {
//    ~~~~~~~~~~~~
	int m_nLapTime;
	int m_nBestTime;
	int m_nLapCount;
	int m_nPlacement;

	int m_nBest;
public:
	CSrcQualMain(CWslWatchSource *pOwner, CTIMeasuredRace *pRace);

	void UpdateDisplays(int nLapTime, int nBestTime, int nLapCount, int nPlacement);
	void ResetDisplays(CTIMeasuredEntity *pEntity);

	~CSrcQualMain();
};

/*******************************************************************************
*
*  BEST RACERS
*
*******************************************************************************/

#define QUAL_BEST_SMALL   0
#define QUAL_BEST_LARGE   1
#define QUAL_BEST_RESULTS 2

class CSrcBest : public CWslWatchSource {
//    ~~~~~~~~
private:
	int m_nMode;
public:
	CSrcNiceTable *m_pSrcTable;

	// Construct the table of qualification placement;
	// nMode -- QUAL_BEST_SMALL or QUAL_BEST_LARGE
	// nNumRacers -- for QUAL_BEST_LARGE .. number of racers that are to
	// be shown
	CSrcBest(CWslWatchSource *pOwner, int nMode, int nNumRacers=0);

	void SetMode(int nMode, int nNumRacers);

	// Set the line
	void SetLine(int nIdx, const char *pcszRacerName, CString s);

	// This function ASSUMES SORTED ENTITIES!!!
	void QualSetLines(CTIMeasuredRace *pTIRace);

	~CSrcBest();

};

/*******************************************************************************
*
*  NEXT RACERS
*
*******************************************************************************/

#define NEXT_MODE_SMALL  0		// Just one racer only
#define NEXT_MODE_MORE   1		// Three subsequent racers
#define NEXT_MODE_GROUP  2    // The whole group

class CSrcNextRacers : public CWslWatchSource {
//    ~~~~~~~~~~~~~~
private:
	int m_nMode;
public:
	CSrcNextRacers(CWslWatchSource *pOwner);
	void SetLine(int nIdx, const char *pcszRacerName);
	void SetMode(int nNewMode);
	void SetLines(int nNewMode, CTIMeasuredRace *pRace);
	void Reset();
	~CSrcNextRacers();
};

/*******************************************************************************
*
*  RACE PART TITLE
*
*******************************************************************************/

class CSrcRacePartTitle : public CWslWatchSource {
//    ~~~~~~~~~~~~~~~~~
public:
	CSrcRacePartTitle();
	void SetText(const char *pcszRaceName, const char *pcszPartName, const char *pcszLeftBottom, const char *pcszRightBottom);
	void SetGroupName(int nGroup, int nNumGroups);
	~CSrcRacePartTitle();
};

/*******************************************************************************
*
*  SPONSOR BITMAP / SEMAPHORE
*
*******************************************************************************/

#define SPSE_MODE_SPONSOR   0
#define SPSE_MODE_SEMAPHORE 1

class CSrcSponsorSemaphore : public CWslWatchSource {
//    ~~~~~~~~~~~~~~~~~~~~
	int m_nLastLit;
public:
	CSrcSponsorSemaphore(CWslWatchSource *pOwner);
	void SetMode(int nNewMode);
	void SetLights(int nNumLit);
	void SetTimeRemains(int nNumMs, CSoundStream &snd);
	~CSrcSponsorSemaphore();
};

/*******************************************************************************
*
*  MAIN RACE WINDOW
*
*******************************************************************************/

class CSrcRaceMain : public CWslWatchSource {
//    ~~~~~~~~~~~~
private:
	int m_nMode;
	int m_nNumRacers;
public:
	CSrcNiceTable *m_pSrcTable;

	void SetRacerData(int nLine, int nTrack, COLORREF crColor, const char *pcszRacer);
	void SetRacerResults(int nLine, int nLastLap, int nBestLap, int nNumLaps, int nTotNumLaps, int nLoss, int nPrognosis, int nOrder, int nCurrentLane, bool bEbUpdate);
	void UpdateRacerResults(CTIMeasuredEntity *pEntity);
	void Reset(int nLine);
	void ResetLaps(int nLine, BOOL bClear);

	void SetLines(CTIMeasuredRace *pRace, BOOL bCalledFromPNR);
	void UpdateLoss(CTIMeasuredRace *pRace);
	void UpdatePrognosis(CTIMeasuredRace *pRace, CTIMeasuredEntity *pEntity, int nCurrentTime);
	void UpdatePosition(CTIMeasuredRace *pRace);
	void PrepareForNextRide(CTIMeasuredRace *pRace);

	CSrcRaceMain(CWslWatchSource *pOwner, int nNumRacers);
	virtual ~CSrcRaceMain();
};

/*******************************************************************************
*
*  TRAINING
*
*******************************************************************************/

class CSrcTraining : public CWslWatchSource {
//    ~~~~~~~~~~~~	
public:
	CSrcNiceTable *m_pSrcTable;

	DWORD dwBestLaps[8];
	DWORD dwTimes[8][16];
	DWORD dwNumLaps[8];
	BOOL bWasMotion[8];

	void SetLaneData(int nLine, int nTrack, COLORREF crColor);
	void DisplayLap(int nLine, DWORD dwTime);
	void CountLap(int nLine);
	void ResetLine(int nLine);
	void UpdateWatches(int nLine);

	void SetLines();

	CSrcTraining(CWslWatchSource *pOwner);
	virtual ~CSrcTraining();
};

/*******************************************************************************
*
*  GROUP RESULTS
*
*******************************************************************************/

class CSrcRaceGroupResults : public CWslWatchSource {
//    ~~~~~~~~~~~~~~~~~~~~
private:
	int m_nMode;
	int m_nNumRacers;
	int m_nNumColsUsed;
public:
	CSrcNiceTable *m_pSrcTable;

	void SetNumColsUsed(int nNum);
	void SetRacerData(int nLine, int nRank, const char *pcszRacer);
	// nNum=-1 -> has paused
	void SetLaps(int nLine, int nCol, int nNum);
	void SetRacerResults(int nLine, int nTotNumLaps, int nTotNumHdts, int nFmt);
	void Reset(int nLine);

	void SetLines(CTIMeasuredRace *pRace);

	CSrcRaceGroupResults(CWslWatchSource *pOwner, int nNumRacers, int nNumRides);
	~CSrcRaceGroupResults();
};

/*********************************************************************** eof **/
