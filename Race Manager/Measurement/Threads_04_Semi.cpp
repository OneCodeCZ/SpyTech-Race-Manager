/******************
*                 *  Threads_04_semi.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  Dec 25, 2001, revised 06/05/04
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
*  SEMIAUTOMATIC TRAINING THREAD
*
*******************************************************************************/

int STTP_Pause(CRaceUIWnd *pWnd, int nRideGroup, CSlotTrackMonitor &monitor, CSoundStream &snd)
{	
	DWORD dwTotalPauseLen;

   CWslDateTime d;
   if (nRideGroup == 0) d.Set("1.1.2004 " + pWnd->m_TrS.m_sBeforeRide);
   else d.Set("1.1.2004 " + pWnd->m_TrS.m_sPauseDuration);
   dwTotalPauseLen = d.GetSecondsSinceMidnight() * 1000;

	pWnd->m_pSrcTime->SetMode(TIME_MODE_SMALL, TIME_FORMAT_HMS, TIME_MODE_COUNTDOWN);
	pWnd->m_pSrcTime->SetCaption(TIME_CAPTION_TIME_TO_START);
	pWnd->m_pSrcTime->SetTime(0, dwTotalPauseLen);
	pWnd->m_pSrcBar->SetMode(BAR_PROGRESS);
	pWnd->m_pSrcBar->SetTime(0, dwTotalPauseLen, 1);
	
   if (!monitor.SetClock(0)) {
   	AfxMessageBox(IDS_STM2008_DEAD);
   	pWnd->PostMessage(WM_COMMAND, WM_SIMPLE_END, 0);
      return 0;
   }
	monitor.SetControlsEx(CONTROLS_CLOCK);

	BOOL bDontQuit = TRUE;
	BOOL bChangeDone = FALSE;
	BOOL bPaused = FALSE;

	do
   {
		int nResult = monitor.MeasureAll();
		if (nResult == 0) MessageBeep(0xffffffff);
		if (nResult == -1)
      {
			// Connection broken.
			AfxMessageBox(IDS_STM2008_DEAD);
			bDontQuit = FALSE;
		}

		pWnd->m_pSrcBar->SetMonitor(&monitor);
		pWnd->m_pSrcTime->SetTime(monitor.m_dwClock, dwTotalPauseLen);
		pWnd->m_pSrcBar->SetTime(monitor.m_dwClock, dwTotalPauseLen, 1);

		if ((monitor.m_dwClock > (dwTotalPauseLen - NEW_SCREEN_TIME_AHEAD)) && !bChangeDone)
      {
         CString sGroup; sGroup.LoadString(IDS_RACE_CAPTION_GROUP);
			CString s;
			s.Format("%s %d/%d", sGroup, nRideGroup + 1, pWnd->m_TrS.m_nNumRides);
			pWnd->m_pSrcHeader->SetCaptions(pWnd->m_TrS.m_sTitle, s, " ");
			pWnd->m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SEMAPHORE);
			pWnd->m_pSrcTraining->SetLines();
			bChangeDone = TRUE;
		}

		if (bChangeDone)
      {
			pWnd->m_pSrcSponsorSemaphore->SetTimeRemains(dwTotalPauseLen - monitor.m_dwClock, snd);
		}

		// Timeout / keyboard events handling
		WaitForAnyKey(50);
		if (g_dwThreadKey == VK_SPACE || g_dwThreadKey == VK_ESCAPE)
      {
         if (bPaused) {
            if (g_dwThreadKey == VK_ESCAPE) {
               if (AskForQuit()) bDontQuit = FALSE;
            }
            else {
               monitor.SetControlsEx(CONTROLS_CLOCK);
      			bPaused = FALSE;
            }
         }
         else {
   			bPaused = TRUE;
            monitor.SetControlsEx(0);
            snd.Stop();
				snd.PlaySnd(SND_INTERRUPTED);
         }
		}
      // Yes, here yes
		if (g_dwThreadKey == 'A')
      {
			monitor.SetClock(dwTotalPauseLen - 5000);
		}
		g_dwThreadKey = 0;

	}
	while (monitor.m_dwClock < dwTotalPauseLen && bDontQuit);
	
	return bDontQuit;
}

/******************************************************************************/

int STTP_Ride(CRaceUIWnd *pWnd, int nRideGroup, CSlotTrackMonitor &monitor, CSoundStream &snd)
{
   CWslDateTime d;
   d.Set("1.1.2004 " + pWnd->m_TrS.m_sRideDuration);
	DWORD dwTotalRideLen = d.GetSecondsSinceMidnight() * 1000;

	pWnd->m_pSrcTime->SetMode(TIME_MODE_SMALL, TIME_FORMAT_HMS, TIME_MODE_COUNTDOWN);
	pWnd->m_pSrcTime->SetCaption(TIME_CAPTION_TIME_TO_END);
	pWnd->m_pSrcTime->SetTime(0, dwTotalRideLen);
	pWnd->m_pSrcBar->SetMode(BAR_PROGRESS);
	pWnd->m_pSrcBar->SetTime(0, dwTotalRideLen, 1);
	pWnd->m_pSrcSponsorSemaphore->SetLights(5);
	
	monitor.SetClock(0);
	monitor.SetTimer(dwTotalRideLen);
	monitor.SetControlsEx(CONTROLS_CLOCK | CONTROLS_TIMER | CONTROLS_POWER);

   CallEBOnRaceEvent(RE_START);

	BOOL bDontQuit = TRUE;
	BOOL bPaused = FALSE;
	BOOL bChanged = FALSE;

	do {
		monitor.MeasureAll();
		pWnd->m_pSrcTime->SetTime(monitor.m_dwClock, dwTotalRideLen);
		pWnd->m_pSrcBar->SetTime(monitor.m_dwClock, dwTotalRideLen, 1);
		pWnd->m_pSrcBar->SetMonitor(&monitor);

		for (int a=0; a<NUMTRACKS; a++) {
			if (monitor.m_bArrWasMotion[monitor.Map(a)]) {
				pWnd->m_pSrcTraining->CountLap(a);
				pWnd->m_pSrcTraining->DisplayLap(a, monitor.m_dwArrDelta[monitor.Map(a)]);
			}
		}

		if (!bChanged && (monitor.m_dwClock > OLD_SCREEN_KEEP_TIME)) {
			pWnd->m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SPONSOR);
      	pWnd->m_pSrcSponsorSemaphore->SetLights(0);
         bChanged = TRUE;
		}

		// Timeout / keyboard events handling
		WaitForAnyKey(50);
		if (g_dwThreadKey == VK_SPACE || g_dwThreadKey == VK_ESCAPE) {
         if (!bPaused) {
            monitor.SetControlsEx(0, CONTROLS_CLOCK | CONTROLS_TIMER | CONTROLS_POWER);
            CallEBOnRaceEvent(RE_PAUSE);
            bPaused = TRUE;
            snd.Stop();
				snd.PlaySnd(SND_INTERRUPTED);
         }
         else {
            if (g_dwThreadKey == VK_ESCAPE) {
               if (AskForQuit()) bDontQuit = FALSE;
            }
            else {
               monitor.SetControlsEx(CONTROLS_CLOCK | CONTROLS_TIMER | CONTROLS_POWER);
               CallEBOnRaceEvent(RE_PAUSEEND);
               bPaused = FALSE;
            }
         }
		}

      if (g_bAccelerationEnabled && (g_dwThreadKey == 'A'))
         monitor.SetClock(dwTotalRideLen - 1000);

		g_dwThreadKey = 0;

	}
	while (monitor.m_dwClock < dwTotalRideLen && bDontQuit);

   CallEBOnRaceEvent(RE_STOP);

	return bDontQuit;
}

/******************************************************************************/

DWORD WINAPI SemiautoTrainingThreadProc(LPVOID lpParameter) {

	CSoundStream snd;
	snd.Init();

	CreateEvents();

	CRaceUIWnd *pWnd = (CRaceUIWnd*) lpParameter;

	BOOL bPauseMode = TRUE;
	g_dwThreadKey = 0;

	CSlotTrackMonitor monitor;
	monitor.ResetVariables();
	monitor.RetrieveSettings();

   CallEBReset();

	if (!monitor.Connect()) {
		AfxMessageBox(IDS_STM2008_NOT_CONNECTED);
		pWnd->PostMessage(WM_COMMAND, WM_SIMPLE_END, 0);
		return 0;
	}
//	monitor.Setup(pWnd->m_hWnd);
	Watch_RedrawAll();
	monitor.SetControlsEx(0);
	monitor.SetClock(0);

	int nCurrentRide = 0;
	BOOL bDontQuit = TRUE;

   CString sGroup; sGroup.LoadString(IDS_RACE_CAPTION_GROUP);
	CString s;
	s.Format("%s 1/%d", sGroup, pWnd->m_TrS.m_nNumRides);
	pWnd->m_pSrcHeader->SetCaptions(pWnd->m_TrS.m_sTitle, s, " ");

	while (nCurrentRide < pWnd->m_TrS.m_nNumRides && bDontQuit) {
		bDontQuit = STTP_Pause(pWnd, nCurrentRide, monitor, snd);
		if (bDontQuit) bDontQuit = STTP_Ride(pWnd, nCurrentRide, monitor, snd);
		nCurrentRide++;
	}

   monitor.SetControlsEx(0);
	monitor.Disconnect();

	pWnd->m_pSrcTime->SetCaption(TIME_CAPTION_END);
	WaitForAnyKey(INFINITE);

	pWnd->PostMessage(WM_COMMAND, WM_SIMPLE_END, 0);

	return 0;
}

/*********************************************************************** eof **/
