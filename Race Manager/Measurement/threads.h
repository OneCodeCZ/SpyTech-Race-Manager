/******************
*                 *  Threads.h
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  DEC 25, 2001
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

/*******************************************************************************
*
*  COMMON STUFF
*
*******************************************************************************/

#define PAUSE_SAFE_TIME                  2000
#define SENSOR_DELAY                     1500
#define OLD_SCREEN_KEEP_TIME             5000
#define NEW_SCREEN_TIME_AHEAD            5000
#define TRAINING_INACTIVITY_RESET_TIME  60000
#define BYE_BAN_TIME                    30000
#define START_ANNOUNCEMENT_AHEAD_TIME   20000
#define PAUSE_ACCELERATION               7000

/******************************************************************************/

extern BOOL g_bAccelerationEnabled;
void CheckForEnableAcclerationFile();

int WaitForAnyKey(int nTimeout=INFINITE);
UINT WaitForEvents(int nTimeout);

int PauseTimer(int nDispMaxMs, int nStartAtMs, int nEndAtMs, CTIMeasuredRace *pRace);

// PARAMETERS:
//   nMode .. 0 == long _ _ _ ^ sound, 1 == short _ _ _^ sound
void RTP_DismissPause(CTIMeasuredRace *pRace, CSoundStream &snd, int nMode=1);
void RTP_WaitForEnterAllowPower(CTIMeasuredRace *pRace, CSlotTrackMonitor &monitor, CSoundStream &snd);

BOOL AskForQuit();

/*********************************************************************** eof **/
