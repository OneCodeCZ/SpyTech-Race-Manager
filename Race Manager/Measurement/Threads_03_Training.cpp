/******************
*                 *  Threads_03_Training.cpp
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
*  TRAINING THREAD
*
*******************************************************************************/

DWORD WINAPI TrainingThreadProc(LPVOID lpParameter) {

	CSoundStream snd;
	snd.Init();

	CreateEvents();

	CRaceUIWnd *pWnd = (CRaceUIWnd*) lpParameter;
   int nLaneClearedAfter = pWnd->m_TrS.m_nClearAfter * 1000;

	BOOL bDontQuit = TRUE;
	BOOL bPauseMode = TRUE;
	g_dwThreadKey = 0;

	CSlotTrackMonitor monitor;
   monitor.ResetSensors();
	monitor.ResetVariables();
	monitor.RetrieveSettings();
	if (!monitor.StartUp()) {
//		pWnd->PostMessage(WM_COMMAND, WM_SIMPLE_END, 0);
//	return 0;
	}
	monitor.SetControlsEx(0);
	monitor.SetControlsEx(0);

   if (!monitor.SetClock(0)) {
   	AfxMessageBox(IDS_STM2008_DEAD);
   	pWnd->PostMessage(WM_COMMAND, WM_SIMPLE_END, 0);
      return 0;
   }

   CallEBReset();
   CallEBOnRaceEvent(RE_START);

	do
   {
		int nResult = monitor.MeasureAll();
		if (nResult == 0) MessageBeep(0xffffffff);
		if (nResult == -1) {
			// Connection broken.
			AfxMessageBox(IDS_STM2008_DEAD);
			bDontQuit = FALSE;
		}

		for (int a=0; a<NUMTRACKS; a++) {
			if (monitor.m_bArrWasMotion[monitor.Map(a)]) {
				pWnd->m_pSrcTraining->CountLap(a);
				pWnd->m_pSrcTraining->DisplayLap(a, monitor.m_dwArrDelta[monitor.Map(a)]);
			}
			if (monitor.m_dwClock - monitor.m_dwArrLastValues[monitor.Map(a)] > nLaneClearedAfter) {
				pWnd->m_pSrcTraining->ResetLine(a);
			}
		}
		pWnd->m_pSrcBar->SetMonitor(&monitor);
		pWnd->m_pSrcTime->SetTime(monitor.m_dwClock);

		WaitForAnyKey(50);
		switch (g_dwThreadKey) {
      case VK_ESCAPE:
		case VK_SPACE:
			if (bPauseMode) {
            if (g_dwThreadKey == VK_ESCAPE) {
               if (AskForQuit()) bDontQuit = FALSE;
            }
            else {
				   monitor.SetControlsEx(CONTROLS_CLOCK | CONTROLS_POWER);
               CallEBOnRaceEvent(RE_PAUSEEND);
               bPauseMode = FALSE;
            }
			}
			else {
				monitor.SetControlsEx(0);
            CallEBOnRaceEvent(RE_PAUSE);
            bPauseMode = TRUE;
            snd.Stop();
				snd.PlaySnd(SND_INTERRUPTED);
			}
			break;
		case 'R':
			for (a=0; a<NUMTRACKS; a++) pWnd->m_pSrcTraining->ResetLine(a);
			break;
		}

		if (g_dwThreadKey >= '1' && g_dwThreadKey <= '8') {
			pWnd->m_pSrcTraining->ResetLine(g_dwThreadKey - '1');
		}

		g_dwThreadKey = 0;
	}
	while (bDontQuit);

   CallEBOnRaceEvent(RE_STOP);

	monitor.SetControlsEx(0);
	monitor.Disconnect();

	pWnd->PostMessage(WM_COMMAND, WM_SIMPLE_END, 0);

	return 0;
}

/*********************************************************************** eof **/
