/******************
*                 *  Threads_02_Race.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  Dec 25, 2001
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "threads.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  RACE THREAD
*
*******************************************************************************/

int RTP_WarmUp(CTIMeasuredRace *pRace, CSlotTrackMonitor &monitor, CSoundStream &snd, int nWarmUpMode)
{
	int nDuration = pRace->GetRS()->GetTimeSettings(pRace->m_nRacePart, 0);

	// 21.06.2001: Removing the cycles. This approach has been told to be
	// unacceptable.
	// 02.10.2002: Don't know, what I ment by the 21.06.2001 remark :-)
	// Deleting all the outcommented code :-)
   // 05.06.2004: Trying to figure out, what the hell is going on here...

//	pRace->m_pWnd->m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SPONSOR);

	BOOL bDontQuit = TRUE;
	BOOL bNotSet = 1;

   CString sGroup; sGroup.LoadString(IDS_RACE_CAPTION_GROUP);
   CString sAux; sAux.LoadString(IDS_RACE_CAPTION_AUXILIARY_WARMUP);
   CString sWarmup; sWarmup.LoadString(IDS_RACE_CAPTION_WARMUP);

	// Construct the header caption
	CString t="\0";
	CString s;
	s.Format("%s", GetRacePartName(pRace->m_nRacePart));
	CString u;
	u.Format("%s %s", sGroup, GetGroupName(pRace->m_nActiveGroup + 1, pRace->GetNumGroups()));
	CString w;
	if (nWarmUpMode > 1) w = sAux;
   else w = sWarmup;
	pRace->m_pWnd->m_pSrcHeader->SetCaptions(s, u, w);
	
	int bPaused = 0;
	
	monitor.SetControlsEx(0);
	monitor.SetClock(0);
	monitor.SetTimer(nDuration);
	monitor.SetControlsEx(CONTROLS_CLOCK | CONTROLS_TIMER | CONTROLS_POWER);
	
   snd.PlaySnd(SND_HIGH_BEEP);
   CallEBOnRaceEvent(RE_START);

   do
	{
		int nResult = monitor.MeasureAll();
		if (nResult == 0) MessageBeep(0xffffffff);
		if (nResult == -1) {
			AfxMessageBox(IDS_STM2008_DEAD);
			bDontQuit = FALSE;
		}
		
		pRace->m_pWnd->m_pSrcBar->SetMonitor(&monitor);
		pRace->m_pWnd->m_pSrcTime->SetCaption(TIME_CAPTION_TIME_TO_END_WARMUP);
		pRace->m_pWnd->m_pSrcTime->SetTime(monitor.m_dwClock, nDuration);
		pRace->m_pWnd->m_pSrcBar->SetTime(monitor.m_dwClock, nDuration, 1);

		if (monitor.m_dwClock >= 5000 && bNotSet) {
			pRace->m_pWnd->m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SPONSOR);
			bNotSet = 0;
		}

		// Timeout / keyboard events handling
		WaitForAnyKey(50);
      // Yes, here yes.
		if (g_dwThreadKey == 'A') monitor.SetClock(nDuration - 5000);
		if (g_dwThreadKey == VK_SPACE || g_dwThreadKey == VK_ESCAPE)
      {
         if (bPaused == 1) {
            if (g_dwThreadKey == VK_ESCAPE) {
               if (AskForQuit()) bDontQuit = FALSE;
            }
            else {
               monitor.SetControlsEx(CONTROLS_CLOCK | CONTROLS_TIMER | CONTROLS_POWER);
      			bPaused = 0;
            }
         }
         else if (bPaused == 0) {
            monitor.SetControlsEx(0);
   			bPaused = 1;
            snd.Stop();
				snd.PlaySnd(SND_INTERRUPTED);
         }
		}

      if (g_dwThreadKey == 'P')
      {
         if (bPaused == 1) {
            bPaused = 2;
            monitor.SetControlsEx(CONTROLS_POWER);
         }
         else if (bPaused == 2) {
            bPaused = 1;
            monitor.SetControlsEx(0);
         }
      }

		g_dwThreadKey = 0;
	}
	while (monitor.m_dwClock < nDuration && bDontQuit);

   if (bDontQuit) snd.PlaySnd(SND_RIDE_END);
   CallEBOnRaceEvent(RE_STOP);

	// Turn off everything
	monitor.SetControlsEx(0);
	if (!monitor.MeasureAll()) MessageBeep(0xffffffff);
	pRace->m_pWnd->m_pSrcBar->SetMonitor(&monitor);

	// If there is one more warm up, wait for a keystroke
	// WaitForAnyKey(INFINITE);
	// g_dwThreadKey = 0;

	return bDontQuit;
}

/******************************************************************************/

int RTP_Pause(CTIMeasuredRace *pRace,       // race
				  CSlotTrackMonitor &monitor,   // monitor
				  CSoundStream &snd,            // sound
				  int nNewRideIdx,              // idx of the new segment to chg to
				  int nTimeFrom,                // amount ms already elapsed
				  int nTimerCaption,
				  BOOL bUpdateBar=TRUE)
{

	pRace->m_bPauseBetweenRides = TRUE;

	int nDuration = pRace->GetRS()->GetTimeSettings(pRace->m_nRacePart, 2);

	pRace->m_pWnd->m_pSrcTime->SetMode(TIME_MODE_SMALL, TIME_FORMAT_HMS, TIME_MODE_COUNTDOWN);
	pRace->m_pWnd->m_pSrcTime->SetCaption(nTimerCaption);
	pRace->m_pWnd->m_pSrcTime->SetTime(nTimeFrom, nDuration);
	pRace->m_pWnd->m_pSrcSponsorSemaphore->SetLights(0);

	monitor.SetControlsEx(0);
	monitor.SetClock(nTimeFrom);

	int nPauseState = 0;
	BOOL bScreenChanged = FALSE;
	BOOL bEbReseted = FALSE;
	BOOL bSpseChanged = FALSE;
	BOOL bDontQuit = TRUE;
	BOOL bPlayedStartSignal = FALSE;

	BOOL bPlayed30Secs = FALSE;
	int nLastMinuteSound = -1;
	int nTimeToStart = 0;

	HDC hDC = ::GetDC(NULL);
		
	monitor.SetControlsEx(CONTROLS_CLOCK);

	if (!bUpdateBar) pRace->m_pWnd->m_pSrcBar->SetTime(0, nDuration, 1);

	do {

		int nResult = monitor.MeasureAll();
		if (nResult == 0) MessageBeep(0xffffffff);
		if (nResult == -1) {
			AfxMessageBox(IDS_STM2008_DEAD);
			bDontQuit = FALSE;
		}

		nTimeToStart = nDuration - monitor.m_dwClock;

		/*
		::SetTextColor(hDC, RGB(0, 0, 0));
		CString s; s.Format("To start: %6d, Clock: %6d, Duration: %6d, Time from: %6d", nTimeToStart, monitor.m_dwClock, nDuration, nTimeFrom);
		::TextOut(hDC, 0, 0, s, s.GetLength());
		*/

		if (
			 (nTimeToStart < START_ANNOUNCEMENT_AHEAD_TIME) &&
			 (!bPlayed30Secs) &&
			 (nDuration > START_ANNOUNCEMENT_AHEAD_TIME + 5000) &&
			 (nTimeFrom < 5000)
			)
		{
			bPlayed30Secs = TRUE;
			snd.PlaySnd(SND_START_AHEAD);
		}

//		if ((nTimeToStart + 500) % 60000 < 1000 && (nLastMinuteSound == -1 || monitor.m_dwClock - nLastMinuteSound > 10000)) {
//			nLastMinuteSound = monitor.m_dwClock;
//			snd.PlaySnd(SND_RIDE_END);
//		}

		pRace->m_pWnd->m_pSrcBar->SetMonitor(&monitor);
		pRace->m_pWnd->m_pSrcTime->SetTime(monitor.m_dwClock, nDuration);
		if (bUpdateBar) pRace->m_pWnd->m_pSrcBar->SetTime(monitor.m_dwClock, nDuration, 1);

      if ((monitor.m_dwClock > OLD_SCREEN_KEEP_TIME - 1000) && !bEbReseted)
      {
         for (int a=0; a<8; a++)
         {
            CallEBUpdateLaps(a+1, 0);
         }
         bEbReseted = TRUE;
      }

		if (monitor.m_dwClock > OLD_SCREEN_KEEP_TIME && !bScreenChanged) {
			// Set lines with names etc.
			pRace->m_pWnd->m_pSrcRaceMain->SetLines(pRace, FALSE);
			// Update loss for every entity in the group
			pRace->m_pWnd->m_pSrcRaceMain->UpdateLoss(pRace);
			// Update prognosis and position for each entity in the group;
			int nIdx;
			ITERATE(pRace->m_aGroup[pRace->m_nActiveGroup], nIdx)
         {
				CTIMeasuredEntity *pEntity;
				pEntity = pRace->m_aGroup[pRace->m_nActiveGroup][nIdx];
				pRace->m_pWnd->m_pSrcRaceMain->UpdatePrognosis(pRace, pEntity, 0);
				pRace->m_pWnd->m_pSrcRaceMain->UpdatePosition(pRace);
            // pRace->m_pWnd->m_pSrcRaceMain->UpdateRacerResults(pEntity);
			}
			pRace->m_pWnd->m_pSrcHeader->SetCaptionRacePart(pRace);
			bScreenChanged = TRUE;
		}

		if (monitor.m_dwClock > nDuration - NEW_SCREEN_TIME_AHEAD && !bSpseChanged) {
			pRace->m_pWnd->m_pSrcSponsorSemaphore->SetTimeRemains(NEW_SCREEN_TIME_AHEAD, snd);
			pRace->m_pWnd->m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SEMAPHORE);
			bSpseChanged = TRUE;
		}

		if (bSpseChanged) pRace->m_pWnd->m_pSrcSponsorSemaphore->SetTimeRemains(nDuration - monitor.m_dwClock, snd);
		if (nDuration - monitor.m_dwClock <= 3000 && !bPlayedStartSignal) {
			snd.PlaySnd(SND_START_SIGNAL);
			bPlayedStartSignal = TRUE;
		}

		// Timeout / keyboard events handling
		WaitForAnyKey(50);

      // Debug acceleration
      if (g_dwThreadKey == 'A') {
			monitor.SetClock(nDuration - PAUSE_ACCELERATION);
		}
      if (g_bAccelerationEnabled && (g_dwThreadKey == 'Q')) monitor.SetClock(nDuration - 500);

		if (g_dwThreadKey == 'P') {
			if (nPauseState == 1) {
				nPauseState = 2;
				monitor.SetControlsEx(CONTROLS_POWER);
			}
			else if (nPauseState == 2) {
				nPauseState = 1;
				monitor.SetControlsEx(0, CONTROLS_POWER);
			}
		}

		if (g_dwThreadKey == VK_SPACE || g_dwThreadKey == VK_ESCAPE) {
			if (nPauseState == 1) {
            if (g_dwThreadKey == VK_ESCAPE) {
               if (AskForQuit()) bDontQuit = FALSE;
            }
            else {
				   monitor.SetControlsEx(CONTROLS_CLOCK);
				   nPauseState = 0;
            }
			}
			else {
				monitor.SetControlsEx(0);
				nPauseState = 1;
            snd.Stop();
				snd.PlaySnd(SND_INTERRUPTED);
			}
		}
		g_dwThreadKey = 0;

	}
	while (monitor.m_dwClock < nDuration && bDontQuit);
	
	return bDontQuit;
}

/******************************************************************************/

int RTP_Ride(CTIMeasuredRace *pRace, 
	          CSlotTrackMonitor &monitor,
			  CSoundStream &snd,   // monitor
			 int nRideIdx)
{
	pRace->m_bPauseBetweenRides = FALSE;

	int nDuration = pRace->GetRS()->GetTimeSettings(pRace->m_nRacePart, 1);

	monitor.SetControlsEx(0);
	monitor.SetClock(0);
	monitor.SetTimer(nDuration);
	monitor.SetControlsEx(CONTROLS_POWER | CONTROLS_TIMER | CONTROLS_CLOCK);
	monitor.ResetVariables();
	monitor.SetMinLapTime(pRace->m_nLapMinTime);
	monitor.MeasureAll();

	pRace->m_pWnd->m_pSrcTime->SetMode(TIME_MODE_SMALL, TIME_FORMAT_HMS, TIME_MODE_COUNTDOWN);
	pRace->m_pWnd->m_pSrcTime->SetCaption(TIME_CAPTION_TIME_TO_END);
	pRace->m_pWnd->m_pSrcTime->SetTime(0, nDuration);

	BOOL bSpseChanged = FALSE;
	BOOL bDontQuit=TRUE;
	BOOL bPlayed = FALSE;

	int nPauseState = 0; // 0 .. run, 1 .. transient, 2 .. full pause
	int nPauseLaunchTime = 0;

   DWORD dwLastPrognosisUpdateTime = 0;

   CallEBOnRaceEvent(RE_START);
//   for (int a=0; a<8; a++) CallEBUpdateLaps(a+1, -1);

//	int nIdx;
//	ITERATE(pRace->m_pArrSortedEntities, nIdx) {
//		pRace->m_pWnd->m_pSrcRaceMain->UpdateRacerResults(pRace->m_pArrSortedEntities[nIdx]);
//	}

	pRace->m_pWnd->m_pSrcRaceMain->PrepareForNextRide(pRace);

   BOOL bFirstPass = 1;

	do {

		int nResult = monitor.MeasureAll();
		if (nResult == 0) MessageBeep(0xffffffff);
		if (nResult == -1) {
			AfxMessageBox(IDS_STM2008_DEAD);
			bDontQuit = FALSE;
		}

      // The ride ends before we quit this cycle because of the additional 2 seconds
      // we are to wait for any racers who pass the finish within that time.
      // (Power is taken care of by the stm/2008 device.)
		if (!bPlayed && monitor.m_dwClock > nDuration) { 
         snd.PlaySnd(SND_RIDE_END); 
         bPlayed = TRUE; 
         CallEBOnRaceEvent(RE_STOP);
      }
		
		for (int a=0; a<NUMTRACKS; a++) 
      {
			int nMapped = monitor.Map(a);
			
			if (monitor.m_bArrWasMotion[nMapped]) {
				DWORD dwLap = monitor.m_dwArrDelta[nMapped];
				CTIMeasuredEntity *pEntity = DC_ME(pRace->m_aTrackMappedEntities[a]);
				if (pEntity) {
					CRideData *pData = pEntity->GetCurrentEvent()->GetCurrentRide();
					pData->StoreLap();
					if (pData->m_dwNumLaps > 1) pData->StoreTime(dwLap);
					pRace->m_pWnd->m_pSrcRaceMain->UpdateRacerResults(pEntity);
					pRace->m_pWnd->m_pSrcRaceMain->UpdateLoss(pRace);
					// pRace->m_pWnd->m_pSrcRaceMain->UpdatePrognosis(pRace, pEntity, monitor.m_dwClock);
					pRace->m_pWnd->m_pSrcRaceMain->UpdatePosition(pRace);
				}
			}
		}

      // dwLastPrognosisUpdateTime
      // 

      for (a=0; a<NUMTRACKS; a++) 
      {
         CTIMeasuredEntity *pEntity = DC_ME(pRace->m_aTrackMappedEntities[a]);
         if (pEntity) pRace->m_pWnd->m_pSrcRaceMain->UpdatePrognosis(pRace, pEntity, monitor.m_dwClock);
      }

		pRace->m_pWnd->m_pSrcBar->SetMonitor(&monitor, nPauseState == 1);

      // Only if there's no pause, update the time bar & the status bar
      if (nPauseState == 0 || nPauseState == 3)
      {
   		pRace->m_pWnd->m_pSrcTime->SetTime(min (monitor.m_dwClock, nDuration), nDuration);
		   pRace->m_pWnd->m_pSrcBar->SetTime(min (monitor.m_dwClock, nDuration), nDuration, 1);
      }

//		if (monitor.m_dwClock>NEW_SCREEN_TIME_AHEAD+1000 && nPauseState != 3)
//			pRace->m_pWnd->m_pSrcSponsorSemaphore->SetTimeRemains(nDuration - monitor.m_dwClock, snd);

		if (monitor.m_dwClock > OLD_SCREEN_KEEP_TIME && !bSpseChanged) {
			pRace->m_pWnd->m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SPONSOR);
         CallEBUpdateLights(0);
			bSpseChanged = TRUE;
		}

		// Timeout / keyboard events handling
		WaitForAnyKey(50);

      if (g_bAccelerationEnabled && (g_dwThreadKey == 'A')) monitor.SetClock(nDuration - 1000);
      if (g_bAccelerationEnabled && (g_dwThreadKey == 'Q')) monitor.SetClock(nDuration - 500);

		if (g_dwThreadKey == 'P') {
			if (nPauseState == 2) {
				nPauseState = 3;
				monitor.SetControlsEx(CONTROLS_POWER);
			}
			else if (nPauseState == 3) {
				nPauseState = 2;
				monitor.SetControlsEx(0, CONTROLS_POWER);
			}
		}

      // If SPACE pressed & at least 500 ms till the end of the ride
		if ((g_dwThreadKey == VK_SPACE || g_dwThreadKey == VK_ESCAPE) && monitor.m_dwClock > 500)
      {
         // No pause yet or pause recently dismissed
			// Pause has to be launched.
			if (nPauseState == 0 || nPauseState == 3)
         {
				// First, decide whether it is possible to launch a pause mode here.
				// more than 2s before end.
				if (monitor.m_dwClock < nDuration - PAUSE_SAFE_TIME)
            {
					monitor.SetControlsEx(0, CONTROLS_POWER | CONTROLS_TIMER);
					nPauseLaunchTime = monitor.m_dwClock; // Remember the launch time ...
					nPauseState = 1; // ... and change the state
               snd.Stop();
				   snd.PlaySnd(SND_INTERRUPTED);
				}
			}
			// Pause has to be dismissed
			if (nPauseState == 2)
         {
            if (g_dwThreadKey == VK_ESCAPE) {
               if (AskForQuit()) bDontQuit = FALSE;
            }
            else {
               RTP_DismissPause(pRace, snd);
				   monitor.IgnoreSensors(FALSE);
				   monitor.SetClock(nPauseLaunchTime);
				   monitor.SetControlsEx(CONTROLS_POWER | CONTROLS_TIMER | CONTROLS_CLOCK);
				   nPauseState = 3;
            }
			}
		}

		if (nPauseState == 3 && monitor.m_dwClock > nPauseLaunchTime + 2000)
      {
			pRace->m_pWnd->m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SPONSOR);
			nPauseState = 0;
		}

		if (nPauseState == 1 && (monitor.m_dwClock > nPauseLaunchTime + SENSOR_DELAY))
      {
			monitor.SetControlsEx(0);
			monitor.IgnoreSensors(TRUE);
         monitor.SetClock(nPauseLaunchTime);
			nPauseState = 2;
		}

		g_dwThreadKey = 0;

	}
	while (monitor.m_dwClock < (nDuration + SENSOR_DELAY) && bDontQuit);

	return bDontQuit;
}

/******************************************************************************/

// This stuff has been written in room seven of a nice hotel in some place
// near Zlin. I had a beer that evening, so I could not concentrate to my
// work so deeply :-)
// (09/2000)

// Well, tomorrow (22.06.2001) is the second european championship driven by our
// software :-). Well, let's see, what we can do with that.

// Today, 13.3.2002, our spruce tree has been chopped down.

// 2.10.2002. No more spruce trees since about four months ago.

// 19.9.2003. Already got MSc. Now lying in bed, had 39C of temperature
// yesterady...

DWORD WINAPI RaceThreadProcEx(LPVOID lpParameter)
{
   CheckForEnableAcclerationFile();

   CSoundStream snd;
	snd.Init();

   CallEBReset();

	// -------------------------------------------------------------------------
	// STEP ONE: INIT THE GROUP STUFF

	CTIMeasuredRace *pRace = (CTIMeasuredRace*) lpParameter;

	if (pRace->m_nActiveGroup == -1) pRace->m_nActiveGroup = 0;

	CSlotTrackMonitor monitor;
	monitor.RetrieveSettings();
	if (!monitor.StartUp()) {
      AfxMessageBox(IDS_STM2008_DEAD);
		pRace->m_pWnd->PostMessage(WM_COMMAND, WM_SIMPLE_END, 0);
		return 0;
	}
	monitor.ResetVariables();

	pRace->m_bPartInProgress = TRUE;

	pRace->m_pWnd->SetUpScreen(CFG_TITLE);
	WaitForAnyKey();

	// -------------------------------------------------------------------------
	// STEP TWO: MEASURE THE GROUP

	// To contain the proper number of ride that is to be measured as the first
	// one after calling this function.
	int nStartRideNum;

	// Result of group selection
	int nResult;

	// If not in recovery mode
	if (!pRace->m_bNeedRecovery) {
		// Reset current ride counter
		pRace->m_nCurrentRide=0;
		// try to change to active group
		nResult = pRace->SetActiveGroup(pRace->m_nActiveGroup, NULL);
		nStartRideNum = 0;
	}
	// If in recovery mode, continue unchanged
	else {
//		AfxMessageBox("needs recovery");
		nResult = pRace->SetActiveGroup(pRace->m_nActiveGroup, NULL);
		ASSERT(nResult);

		// Starting ride is the one that has been marked as the current one
		nStartRideNum = pRace->m_nCurrentRide;
	}

	// ------------------------------------------------------------------------
	// STEP THREE: MEASURE ALL THE RIDES TO BE MEASURED

	int nNumRides = pRace->GetNumRides(); // Get num rides

	// User does not want to quit right now.
	BOOL bDontQuit = TRUE;
	BOOL bShrinkPauseTo5Secs = 1;

	for (int a=nStartRideNum; a<nNumRides && bDontQuit; a++) // For each ride
	{
		// --------------------------------------------------------------------
		// SUBSTEP ONE: Init the segment

		// Set current ride in the race
		pRace->m_nCurrentRide = a;
		// Assign lanes by the current ride
		pRace->AssignLanes(pRace->m_nCurrentRide);
		// Sort entities according to their results etc.
		pRace->InitGroupRacers();

		// Set the screen
		if (a==nStartRideNum) {
			pRace->m_pWnd->SetUpScreen(CFG_RACE_MAIN, 1);
		}

		// -----------------------------------------------------------------------
		// WARM-UP for the first segment

		// Let's spawn warmup here
		BOOL bWarmupAllowed = pRace->GetRS()->GetTimeSettings(pRace->m_nRacePart, 0);
		int nWarmupMode = 0;
		if (bWarmupAllowed) {
			if (a==0) nWarmupMode = 1;
		}
		if (nWarmupMode != 0) {
			WaitForAnyKey();
			g_dwThreadKey = 0;
         if (RTP_WarmUp(pRace, monitor, snd, 0) == 0) {
            bDontQuit = 0;
            continue;
         }
         RTP_WaitForEnterAllowPower(pRace, monitor, snd);
			pRace->m_pWnd->SetUpScreen(CFG_RACE_MAIN);
			bShrinkPauseTo5Secs = 1;
		}

		// -----------------------------------------------------------------------
		// Decide what pause to use

		nWarmupMode = 0;
		if (bWarmupAllowed) {
			if (a==1 && pRace->GetNumRides() >= 9) nWarmupMode = 2;
			if (a==2 && pRace->GetNumRides() >= 10) nWarmupMode = 3;
		}

		DWORD dwDuration = pRace->GetRS()->GetTimeSettings(pRace->m_nRacePart, 2);
		int nPauseLowLimit = bShrinkPauseTo5Secs ? dwDuration - 5000 : SENSOR_DELAY;
		bDontQuit = RTP_Pause(pRace, monitor, snd, a, nPauseLowLimit, 
			nWarmupMode ? TIME_CAPTION_TIME_TO_WARMUP : TIME_CAPTION_TIME_TO_START);
		bShrinkPauseTo5Secs = 0;
		
		// -----------------------------------------------------------------------
		// WARM-UP for the other segments

		if (nWarmupMode != 0) {
			g_dwThreadKey = 0;
			bShrinkPauseTo5Secs = 0;
			RTP_WarmUp(pRace, monitor, snd, nWarmupMode);
			pRace->m_pWnd->SetUpScreen(CFG_RACE_MAIN);
			bDontQuit = RTP_Pause(pRace, monitor, snd, a, dwDuration - 5000, TIME_CAPTION_TIME_TO_START);
		}

		// -----------------------------------------------------------------------
		// Take a ride

		if (bDontQuit) bDontQuit = RTP_Ride(pRace, monitor, snd, a);

		// -----------------------------------------------------------------------
		// Well, let's do some termination stuff...

		if (!bDontQuit) {
			pRace->ForgetLastRide();
			pRace->m_bNeedRecovery = TRUE;
			AfxMessageBox(IDS_USER_STOPPED_MEASUREMENT);
		}
		else {

			// And now, let's save the results. Recovery mode is stored into
			// race in order to properly handle the return into the race.
			pRace->m_bNeedRecovery = TRUE;
			int nOldCurrentRide = pRace->m_nCurrentRide;
			int nOldCurrentGroup = pRace->m_nActiveGroup;
			pRace->m_nCurrentRide++;
//			if (pRace->m_nCurrentRide == 1) {
//				pRace->m_nActiveGroup--;
//			}
			if (pRace->m_nCurrentRide == nNumRides) {
				pRace->m_nCurrentRide = 0;
				pRace->m_bNeedRecovery=FALSE;
				pRace->m_nActiveGroup ++;
				if (pRace->m_nActiveGroup == pRace->m_nNumGroups) {
					pRace->m_bPartEndedButNotAccepted = TRUE;
					pRace->m_bNeedRecovery = TRUE;
				}
			}
			WsfGetCApp()->OnRaceSave();
			pRace->m_nCurrentRide = nOldCurrentRide;
			pRace->m_nActiveGroup = nOldCurrentGroup;
			pRace->m_bNeedRecovery = FALSE;

			// Debug reports into external file

			CString s;
			s.Format("%s group %d, segment %d",
				pRace->GetRacePartName(pRace->m_nRacePart),
				pRace->m_nActiveGroup+1,
				pRace->m_nCurrentRide+1
			);

         if (!pRace->RaceReportGroup(pRace->m_aGroup[pRace->m_nActiveGroup], TRUE, s)) {
            AfxMessageBox(IDS_COULD_NOT_REPORT);
         }
		}
	}

	nStartRideNum = 0;

	BOOL bThisWasLastGroup = (pRace->m_nActiveGroup+1 == pRace->m_nNumGroups);

	if (bDontQuit) {

		// Sort racers in the current group
		pRace->m_pArrSortedEntities = pRace->m_aGroup[pRace->m_nActiveGroup];
		pRace->RaceSortRacers(pRace->m_pArrSortedEntities);

		// HDTS fill-in
		CDlgHdts hdts;
		hdts.PrepareData(pRace->m_pArrSortedEntities, pRace);
      CString s;
      s.LoadString(IDS_FILL_HUNDREDS);
		hdts.DoModal(s, pRace->m_pArrSortedEntities.GetSize());
		hdts.GatherData(pRace->m_pArrSortedEntities);
		WsfGetCApp()->OnRaceSave();

		// Sort them once again
		pRace->RaceSortRacers(pRace->m_pArrSortedEntities);
		// Show group results
		pRace->m_pWnd->SetUpScreen(CFG_RACE_GROUP_RESULTS);
		WaitForAnyKey();

//		nResult = pRace->SetActiveGroup(pRace->m_nActiveGroup+1, NULL);
	}

	// -------------------------------------------------------------------------
	// STEP FOUR: FINISH THE THINGS UP

	// Disconnect from the STM/2008 in every fall.
	monitor.Disconnect();
	// Sort the racers. Why not, anyway...
	pRace->RaceSortRacers(pRace->m_aPartEntities);

	// HERE WAS THE QUESTION ABOUT CORRECTNESS OF THE MEASUREMENT

	if (bThisWasLastGroup) {

		// Write down the ranking.
		int nIdx;
		for (nIdx=0; nIdx < pRace->m_aGroup->GetSize(); nIdx++) pRace->RaceSortRacers(pRace->m_aGroup[nIdx]);
		for (nIdx=0; nIdx < pRace->m_aPartEntities.GetSize(); nIdx++) {
			CTIRaceEntity *pEntity;
			pEntity = pRace->m_aPartEntities[nIdx];
			pEntity->m_nRanking = nIdx;
		}
	}

	if (bDontQuit) pRace->m_pWnd->PostMessage(WM_COMMAND, WM_GROUP_SUCCESS, 0);
	else pRace->m_pWnd->PostMessage(WM_COMMAND, WM_SIMPLE_END, 0);

	return bDontQuit;
}

/*********************************************************************** eof **/
