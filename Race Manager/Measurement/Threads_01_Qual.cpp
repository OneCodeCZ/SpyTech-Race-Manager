/******************
*                 *  Threads_01_Qual.cpp
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
*  QUALIFICATION THREAD
*
*******************************************************************************/

int QTP_Pause(CTIMeasuredRace *pRace,       // race
				  CSlotTrackMonitor &monitor,   // monitor
				  CSoundStream &snd,        // snd
				  BOOL bFirstPause,             // TRUE if yes.
				  int nTimeFrom)                // amount ms already elapsed
{

	int nDuration = pRace->GetRS()->GetTimeSettings(pRace->m_nRacePart, 2);

	pRace->m_pWnd->SetUpScreen(CFG_QUAL_LIST_CURRENT);
	pRace->m_pWnd->m_pSrcRaceMain->SetLines(pRace, FALSE);
	pRace->m_pWnd->m_pSrcHeader->SetCaptionRacePart(pRace);

	monitor.SetControlsEx(0);
	monitor.SetClock(nTimeFrom);

	int nPauseState = 0;
	BOOL bSpseChanged = FALSE;
	int bDontQuit = 1;
	BOOL bScreenChanged = FALSE;
	BOOL bPower = FALSE;
	BOOL bPlayedStartSignal = FALSE;

	monitor.SetControlsEx(CONTROLS_CLOCK);

	g_dwThreadKey = 0;

	do {
		int nResult = monitor.MeasureAll();
		if (nResult == 0) MessageBeep(0xffffffff);
		if (nResult == -1) {
			AfxMessageBox(IDS_STM2008_DEAD);
			bDontQuit = -1;
		}

		pRace->m_pWnd->m_pSrcBar->SetMonitor(&monitor);
		pRace->m_pWnd->m_pSrcTime->SetTime(monitor.m_dwClock, nDuration);
		pRace->m_pWnd->m_pSrcBar->SetTime(monitor.m_dwClock, nDuration, 1);

		if (monitor.m_dwClock > nDuration - NEW_SCREEN_TIME_AHEAD && !bSpseChanged) {
			pRace->m_pWnd->SetUpScreen(CFG_QUAL_MAIN);
			pRace->m_pWnd->m_pSrcTime->SetCaption(TIME_CAPTION_TIME_TO_START);
			pRace->m_pWnd->m_pSrcSponsorSemaphore->SetTimeRemains(OLD_SCREEN_KEEP_TIME, snd);
			pRace->m_pWnd->m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SEMAPHORE);
			pRace->m_pWnd->m_pSrcSponsorSemaphore->SetLights(0);
			bSpseChanged = TRUE;
		}

		if (nDuration - monitor.m_dwClock <= 3000 && !bPlayedStartSignal) {
			snd.PlaySnd(SND_START_SIGNAL);
			bPlayedStartSignal = TRUE;
		}

		if (monitor.m_dwClock > nDuration - 10000) 
			pRace->m_pWnd->m_pSrcSponsorSemaphore->SetTimeRemains(nDuration - monitor.m_dwClock, snd);

		// Timeout / keyboard events handling
		WaitForAnyKey(50);
      // Yes, here yes.
		if (g_dwThreadKey == 'A') monitor.SetClock(nDuration - 5000);

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

		if ((g_dwThreadKey == VK_SPACE || g_dwThreadKey == VK_ESCAPE) && !bPower) {
			if (nPauseState == 1) {
            if (g_dwThreadKey == VK_ESCAPE) {
               if (AskForQuit()) bDontQuit = 0;
            }
            else {
				   monitor.SetControlsEx(CONTROLS_CLOCK);
				   nPauseState = 0;
            }
			}
			else if (nPauseState == 0) {
				monitor.SetControlsEx(0, CONTROLS_CLOCK);
				nPauseState = 1;
            snd.Stop();
				snd.PlaySnd(SND_INTERRUPTED);
			}
		}
		g_dwThreadKey = 0;

	}
	while (monitor.m_dwClock < nDuration && (bDontQuit == 1));
	
	return bDontQuit;
}

/******************************************************************************/

int QTP_Ride(CTIMeasuredRace *pRace, CSlotTrackMonitor &monitor, CSoundStream &snd)
{
	// --------------------------------------------------------------------------
	// Ride inititalization routines

	CTIMeasuredEntity *pEntity = pRace->QualGetCurrentEntity();

	// Get maximum duration possible
	int nDuration = pRace->GetRS()->GetTimeSettings(pRace->m_nRacePart, 1);

	monitor.SetControlsEx(0);
	monitor.SetTimer(nDuration);
	monitor.SetControlsEx(CONTROLS_POWER | CONTROLS_TIMER | CONTROLS_CLOCK);
	monitor.ResetVariables();
	monitor.MeasureAll();

	// Clock of the STM/2008 is set by the qualification mode
	switch (pRace->GetRS()->m_nPrimaryEval) {
	case RS_EVAL_NUM_LAPS:
		monitor.SetClock(0);
		break;
	case RS_EVAL_FIRST_TIME:
		monitor.SetClock(pEntity->m_dwTimeElapsed);
		break;
	default:
		// Undetermined primary evaluation method. No one should ever get here!
		ASSERT(FALSE);
	}

	pRace->m_pWnd->m_pSrcTime->SetMode(TIME_MODE_SMALL, TIME_FORMAT_HMS, TIME_MODE_COUNTDOWN);
	pRace->m_pWnd->m_pSrcTime->SetCaption(TIME_CAPTION_TIME_TO_END);
	pRace->m_pWnd->m_pSrcTime->SetTime(0, nDuration);

   CTIRS *rs = pRace->GetRS();

	DWORD dwNumLaps=0;
	int nPauseState=0;
	int nTrackEmployed = -1;

	BOOL bSpseChanged = FALSE;
	int bDontQuit = 1;
	BOOL bDontBreak = TRUE;
	BOOL bCanceled = FALSE;
	BOOL bPlayed = FALSE;

   double fSpd;

	g_dwThreadKey = 0;
	DWORD dwPauseLaunched;

	// Transactional approach. Thats good, is not it?
	// 16.10.2000 -- huh. wha' didya say, man?
	// 21.06.2001 -- oh, nuffin' spesha, man!!
	// 26.12.2001 -- nyce!

	// --------------------------------------------------------------------------
	// The main measurement cycle

	do
   {
		int nResult = monitor.MeasureAll();
		if (nResult == 0) MessageBeep(0xffffffff);
		if (nResult == -1) {
			AfxMessageBox(IDS_STM2008_DEAD);
			bDontQuit = -1;
		}

		if (!bPlayed && monitor.m_dwClock > nDuration) { snd.PlaySnd(SND_RIDE_END); bPlayed = TRUE; }

		for (int a=0; a<NUMTRACKS; a++) {
			int nMapped = monitor.Map(a);
			if (monitor.m_bArrWasMotion[nMapped])
         {
            int nRankDisp = pEntity->GetCurrentQualRank();

				if (nTrackEmployed == -1) {
					DWORD dw1, dw2;
					pEntity->QualGetBestTimes(&dw1, &dw2);
					nTrackEmployed = a;
					dwNumLaps++;
               switch (rs->m_nPrimaryEval)
               {
               case RS_EVAL_NUM_LAPS: pRace->m_pWnd->m_pSrcQualMain->UpdateDisplays(0, dw1, dwNumLaps, 0); break;
               case RS_EVAL_FIRST_TIME: pRace->m_pWnd->m_pSrcQualMain->UpdateDisplays(0, dw1, dwNumLaps, nRankDisp); break;
               default: ASSERT(FALSE);
               }
				}
				else {
					if (a == nTrackEmployed)
               {
						dwNumLaps++;
						DWORD dwLap = monitor.m_dwArrDelta[nMapped];
						pEntity->QualStoreTime(dwLap);
						DWORD dw1, dw2;

						pEntity->QualGetBestTimes(&dw1, &dw2);

                  switch (rs->m_nPrimaryEval)
                  {
                  case RS_EVAL_NUM_LAPS:
				         fSpd = 3.6 * (1000 * (double)pRace->m_nTrackLength / 100.0 / (double)dwLap);
				         nRankDisp = fSpd;
                     break;
                  case RS_EVAL_FIRST_TIME:
             	      pRace->QualSortRacers();
                     nRankDisp = pEntity->GetCurrentQualRank();
            	      pRace->m_pWnd->m_pSrcBest->QualSetLines(pRace);
                     break;
                  }

						pRace->m_pWnd->m_pSrcQualMain->UpdateDisplays(dwLap, dw1, dwNumLaps, nRankDisp);
					}
				}
			}
		}

		pRace->m_pWnd->m_pSrcBar->SetMonitor(&monitor);
		pRace->m_pWnd->m_pSrcTime->SetTime(min (monitor.m_dwClock, nDuration), nDuration);
		pRace->m_pWnd->m_pSrcBar->SetTime(min (monitor.m_dwClock, nDuration), nDuration, 1);

		if (monitor.m_dwClock > OLD_SCREEN_KEEP_TIME && !bSpseChanged) {
			pRace->m_pWnd->m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SPONSOR);
			bSpseChanged = TRUE;
		}

		// Timeout / keyboard events handling
		WaitForAnyKey(50);

	   if (g_bAccelerationEnabled && (g_dwThreadKey == 'A')) monitor.SetClock(nDuration - 1000);
		if (g_dwThreadKey == 'B') { if (monitor.m_dwClock < nDuration - BYE_BAN_TIME) bDontBreak = FALSE; }
		if (g_dwThreadKey == 'C') bCanceled = TRUE;

		if (g_dwThreadKey == 'P') {
			if (nPauseState == 1) {
				nPauseState = 3;
				monitor.SetControlsEx(CONTROLS_POWER);
			}
			else if (nPauseState == 3) {
				nPauseState = 1;
				monitor.SetControlsEx(0, CONTROLS_POWER);
			}
		}

//		if (g_dwThreadKey == 'Q') {
//			CDlgQualRepair hd;
//			hd.DoModal();
//			pEntity->QualStoreTime(hd.m_dw1);
//			pEntity->QualStoreTime(hd.m_dw2);
//		}

		if ((g_dwThreadKey == VK_SPACE || g_dwThreadKey == VK_ESCAPE) && monitor.m_dwClock > 500) {
			if (nPauseState == 0 || nPauseState == 2)
         {
            snd.Stop();
				snd.PlaySnd(SND_INTERRUPTED);
				monitor.SetControlsEx(0);
				nPauseState = 1;
				dwPauseLaunched = monitor.m_dwClock;
			}
			else if (nPauseState == 1)
         {
            if (g_dwThreadKey == VK_ESCAPE) {
               if (AskForQuit()) bDontQuit = 0;
            }
            else {
				   RTP_DismissPause(pRace, snd);
				   monitor.SetControlsEx(CONTROLS_POWER | CONTROLS_TIMER | CONTROLS_CLOCK);
				   nPauseState = 2;
            }
			}
		}

		if (monitor.m_dwClock > dwPauseLaunched + 2000 && nPauseState == 2) {
			pRace->m_pWnd->m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SPONSOR);
			nPauseState = 0;
		}

		g_dwThreadKey = 0;

	}
	while (
		monitor.m_dwClock < (nDuration + SENSOR_DELAY) &&
		(bDontQuit == 1) && 
		bDontBreak && 
		!bCanceled
	);

	// --------------------------------------------------------------------------
	// Data storage procedure

	// remember, how much time has been elapsed
	pEntity->m_dwTimeElapsed = monitor.m_dwClock;
	// If the qualification has been canceled, store the whole duration
	if (bCanceled) pEntity->m_dwTimeElapsed = nDuration + 1000;

	if (!bDontBreak) {
		// If the qualification has been byed, add 30s.
		pEntity->m_dwTimeElapsed += BYE_BAN_TIME;
		// Set the caption of the time watch
		pRace->m_pWnd->m_pSrcTime->SetCaption(TIME_CAPTION_ATTEMPT_END);
		// ... and wait 1.5 s.
		Sleep(1500);
	}

	if (bCanceled) {
		pRace->m_pWnd->m_pSrcTime->SetCaption(TIME_CAPTION_QUAL_LEAVE);
		Sleep(1500);
	}

	// Remember number of laps.
	pEntity->QualStoreLaps(dwNumLaps, 0);

	if (
		nTrackEmployed != -1 && // there was at least one sensor activated ...
		pRace->GetRS()->m_nPrimaryEval == RS_EVAL_NUM_LAPS // ... and the correct mode is on
		)
	{
		DWORD dwLaps;
		DWORD dwHdts;
		
		CDlgHdts hdts;
		pEntity->QualGetNumLaps(&dwLaps, &dwHdts);
		hdts.m_dwNumRacers = 1;
		hdts.m_sArrNames[0] = pEntity->GetEntityName(CTIRaceEntity::NICKNAME);
		hdts.m_dwArrNumLaps[0] = dwLaps;
		hdts.m_dwArrNumHdts[0] = 0;
		hdts.m_nArrPenalization[0] = 0;
		hdts.m_crArrColors.Append(pRace->GetLaneColor(nTrackEmployed));

      CString s;
      s.LoadString(IDS_FILL_HUNDREDS);

		hdts.DoModal(s, -1);
		pEntity->QualStoreLaps(0, hdts.m_dwArrNumHdts[0]);
	}

   if (!pRace->ReportInit())
   {
	   pRace->QualReportRide(pEntity);
	   pRace->ReportDone();
   }

	return bDontQuit;
}

/******************************************************************************/

DWORD WINAPI QualThreadProc(LPVOID lpParameter)
{
   CheckForEnableAcclerationFile();

	CSoundStream snd;
	snd.Init();

	CTIMeasuredRace *pRace = (CTIMeasuredRace*) lpParameter;
	CTIMeasuredEntity *pEntity;

	CSlotTrackMonitor monitor;
	monitor.m_bQualMode = TRUE;
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

	pRace->m_pWnd->SetUpScreen(CFG_QUAL_LIST_CURRENT);
	WaitForAnyKey();

	BOOL bFirstEntry = TRUE;
	BOOL bDontQuit = TRUE;

	int nResult;

	do
   {
		pRace->QualSortRacers();

		// Prepare the next entity to its qualification
		pEntity = pRace->QualGetCurrentEntity();

		pEntity->QualBeginTransaction();
		pEntity->OnQualNewRide();

		// Make the entity to wait
		bDontQuit = QTP_Pause(pRace, monitor, snd, bFirstEntry, bFirstEntry ? 0 : 1500);

		if (bDontQuit)
      {
			// Measure its qualification
			bDontQuit = QTP_Ride(pRace, monitor, snd);

			if (bDontQuit) {
				WsfGetCApp()->OnRaceSave();
				nResult = pRace->QualAdvance();
			}

			bFirstEntry = FALSE; // Chg screen avoidance
		}

		if (!bDontQuit) pEntity->QualRollback();
	}
	while (nResult && (bDontQuit == 1));

	if (bDontQuit) {
		pRace->QualSortRacers(1);
		pRace->QualSortRacers();
		pRace->QualReportResults();
		pRace->m_pWnd->SetUpScreen(CFG_QUAL_FINAL);
		WaitForAnyKey();

		int nIdx;
		ITERATE(pRace->m_aPartEntities, nIdx) {
			pRace->m_aPartEntities[nIdx]->m_nRanking=nIdx;
		}

		pRace->m_pWnd->PostMessage(WM_COMMAND, WM_SUCCESS, 0);
	}
	else {
		pRace->m_bNeedRecovery = TRUE;
		WsfGetCApp()->OnRaceSave();
		pRace->m_pWnd->PostMessage(WM_COMMAND, WM_INTERRUPTED, 0);
	}

	monitor.Disconnect();

	return bDontQuit;
}

/*********************************************************************** eof **/
