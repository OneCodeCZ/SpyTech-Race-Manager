/******************
*                 *  ExternalBoard.h
*   __|           *  ___________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  
*       \    '\   *  ___________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of <PROJECTS> source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else :-).
*******************                                                           */

#pragma once

/*******************************************************************************
*
*  EXTERNAL PANEL
*
*******************************************************************************/

extern BOOL g_bExtBoard;
extern HINSTANCE g_hExtBoardLib;
void LoadBoardLib();

/******************************************************************************/

/*
// CALL AS: (*EBPing)()
extern VOID_VOID EBPing;
// CALL AS: (*EBUpdateLights)(BYTE lights)
// FLAGS:
//   1 .. green
//   2 .. red #3
//   4 .. red #2
//   8 .. red #1
extern VOID_BYTE EBUpdateLights;
// CALL AS: (*EBUpdateLights)(BYTE new_state, SHORT num_laps)
// PARAMETERS:
//   0 .. inactive
//   1 .. start
//   2 .. stop
//   3 .. pause_start
//   4 .. pause_end
extern VOID_BYTE_SHORT EBUpdateLaps;
// CALL AS: (*EBUpdateRaceState)(BYTE new_state)
// PARAMETERS:
//   0 .. inactive
//   1 .. start
//   2 .. stop
//   3 .. pause_start
//   4 .. pause_end
extern VOID_BYTE EBUpdateRaceState;
*/

#define RE_INACT       0
#define RE_START       1
#define RE_STOP        2
#define RE_INTERRUPTED 3
#define RE_PAUSE       4
#define RE_PAUSEEND    5

void CallEBPing();
void CallEBReset();
void CallEBUpdateLights(BYTE lights);
void CallEBUpdateLaps(BYTE lane, short laps);
void CallEBOnRaceEvent(BYTE event);
void CallEBUpdateTime(short h, short m, short s, short hdt);

/*********************************************************************** eof **/
