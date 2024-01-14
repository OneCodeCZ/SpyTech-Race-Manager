/******************
*                 *  Threads_00_Common.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  27-AUG-2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"
#include "ScreenCfgCodes.h"
#include "DlgTrainingSettings.h"
#include "resource.h"
#include "DlgHdts.h"
#include "DlgQualRepair.h"
#include "threads.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  COMMON STUFF
*
*******************************************************************************/

#define PAUSE_SAFE_TIME                  2000
#define SENSOR_DELAY                     1500
#define OLD_SCREEN_KEEP_TIME             5000
#define NEW_SCREEN_TIME_AHEAD            5000
#define BYE_BAN_TIME                    30000

/******************************************************************************/

BOOL g_bAccelerationEnabled = FALSE;

/******************************************************************************/

BOOL AskForQuit()
{
   if (AfxMessageBox(IDS_INTERRUPT_MEASUREMENT, MB_YESNO) == IDYES) return TRUE;
   return FALSE;
}

/******************************************************************************/

void CheckForEnableAcclerationFile()
{
   g_bAccelerationEnabled = FALSE;

   CFile file;
   if (file.Open("data\\enable_acceleration", CFile::typeBinary | CFile::modeRead))
   {
      AfxMessageBox("DEBUG MODE: Acceleration with key 'A' is enabled");
      file.Close();
      g_bAccelerationEnabled = TRUE;
   }
}

/******************************************************************************/

int WaitForAnyKey(int nTimeout) {
	int nResult = WaitForSingleObject(g_hEventKeyPressed, nTimeout);
	return nResult;
}

/******************************************************************************/

UINT WaitForEvents(int nTimeout) {
	int nResult = WaitForSingleObject(g_hEventKeyPressed, nTimeout);
	switch (nResult) {
	case WAIT_OBJECT_0:
		return g_dwThreadKey;
		break;
	case WAIT_TIMEOUT:
		return 0;
		break;
	default:
		return -1;
		break;
	}
	g_dwThreadKey = 0;
}

/******************************************************************************/

int PauseTimer(int nDispMaxMs, int nStartAtMs, int nEndAtMs, CTIMeasuredRace *pRace)
{
	DWORD dwStart = GetTickCount();
	DWORD dwNow = GetTickCount();
	DWORD dwTotalTime = nEndAtMs - nStartAtMs;
	int nResult = 0;

	CSrcTime *pTime = pRace->m_pWnd->m_pSrcTime;
	CSrcBar *pBar = pRace->m_pWnd->m_pSrcBar;

	do {

		dwNow = GetTickCount();

		pBar->SetTime(nStartAtMs + dwNow - dwStart, nDispMaxMs, 1);
		pTime->SetTime(nStartAtMs + dwNow - dwStart, nDispMaxMs);

		nResult = WaitForEvents(50);
		if (nResult > 0) {
			if (nResult == 32) AfxMessageBox("Pause between rides not yet implemented. Sorry.");
		}
	}
	while ((dwNow - dwStart < dwTotalTime) && (nResult != -1));

	return nResult;
}

/******************************************************************************/

void RTP_DismissPause(CTIMeasuredRace *pRace, CSoundStream &snd, int nMode)
{
   ASSERT(nMode == 0 || nMode == 1);

   switch (nMode) {
   case 0:
   	snd.PlaySnd(SND_START_SIGNAL);
      break;
   case 1:
	   snd.PlaySnd(SND_START_SIGNAL_SHORT);
      break;
   }

   int nDelay = nMode == 0 ? 1000 : 500;
	pRace->m_pWnd->m_pSrcSponsorSemaphore->SetMode(SPSE_MODE_SEMAPHORE);
	pRace->m_pWnd->m_pSrcSponsorSemaphore->SetLights(2);
	Sleep(nDelay);
	pRace->m_pWnd->m_pSrcSponsorSemaphore->SetLights(3);
	Sleep(nDelay);
	pRace->m_pWnd->m_pSrcSponsorSemaphore->SetLights(4);
	Sleep(nDelay);
	pRace->m_pWnd->m_pSrcSponsorSemaphore->SetLights(5);
}

/******************************************************************************/

void RTP_WaitForEnterAllowPower(CTIMeasuredRace *pRace, CSlotTrackMonitor &monitor, CSoundStream &snd)
{
   BOOL bPower = FALSE;
   do { 
      WaitForAnyKey(INFINITE);
      if (g_dwThreadKey == 'P')
      {
         bPower = !bPower;
         if (bPower) monitor.SetControlsEx(CONTROLS_POWER);
         else monitor.SetControlsEx(0);
         pRace->m_pWnd->m_pSrcBar->SetMonitor(&monitor);
      }
   }
   while (g_dwThreadKey != VK_RETURN);
}

/*******************************************************************************
*
*  EXTERNAL BOARD
*
*******************************************************************************/

BOOL g_bExtBoard;
HINSTANCE g_hExtBoardLib;

typedef void (*VOID_VOID)();
typedef void (*VOID_BYTE)(BYTE);
typedef void (*VOID_BYTE_SHORT)(BYTE, SHORT);
typedef void (*VOID_SHORT_SHORT_SHORT_SHORT)(SHORT, SHORT, SHORT, SHORT);

// External Board functions
VOID_VOID EBPing = 0;
VOID_VOID EBReset = 0;
VOID_BYTE EBUpdateLights = 0;
VOID_BYTE_SHORT EBUpdateLaps = 0;
VOID_BYTE EBOnRaceEvent = 0;
VOID_SHORT_SHORT_SHORT_SHORT EBUpdateTime = 0;

/******************************************************************************/
// Wrapper functions

void CallEBPing()
{
   if (EBPing) (*EBPing)();
}

void CallEBReset()
{
   if (EBReset) (*EBReset)();
}

void CallEBUpdateLights(BYTE lights)
{
   if (EBUpdateLights) (*EBUpdateLights)(lights);
}

void CallEBUpdateLaps(BYTE lane, short laps)
{
   if (EBUpdateLaps) (*EBUpdateLaps)(lane, laps);
}

void CallEBOnRaceEvent(BYTE new_state)
{
   if (EBOnRaceEvent) (*EBOnRaceEvent)(new_state);
}

void CallEBUpdateTime(short h, short m, short s, short hdt)
{
   if (EBUpdateTime) (*EBUpdateTime)(h, m, s, hdt);
}

/******************************************************************************/

void LoadBoardLib()
{
   g_hExtBoardLib = LoadLibrary("InfoBoard.dll");
   g_bExtBoard = (g_hExtBoardLib != 0);

   if (!g_bExtBoard) return;

   EBPing = (VOID_VOID)GetProcAddress(g_hExtBoardLib, "EBPing");
   if (!EBPing) AfxMessageBox("EBPing function not found");

   EBReset = (VOID_VOID)GetProcAddress(g_hExtBoardLib, "EBReset");
   if (!EBPing) AfxMessageBox("EBReset function not found");

   EBUpdateLights = (VOID_BYTE)GetProcAddress(g_hExtBoardLib, "EBUpdateLights");
   if (!EBUpdateLights) AfxMessageBox("EBUpdateLights function not found");

   EBUpdateLaps = (VOID_BYTE_SHORT)GetProcAddress(g_hExtBoardLib, "EBUpdateLaps");
   if (!EBUpdateLaps) AfxMessageBox("EBUpdateLaps function not found");

   EBOnRaceEvent = (VOID_BYTE)GetProcAddress(g_hExtBoardLib, "EBOnRaceEvent");
   if (!EBOnRaceEvent) AfxMessageBox("EBOnRaceEvent function not found");

   EBUpdateTime = (VOID_SHORT_SHORT_SHORT_SHORT)GetProcAddress(g_hExtBoardLib, "EBUpdateTime");
   if (!EBUpdateTime) AfxMessageBox("EBUpdateTime function not found");
}

/*********************************************************************** eof **/
