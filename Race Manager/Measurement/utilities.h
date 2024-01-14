/******************
*                 *  utilities.h
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
*  UTILITIES
*
*******************************************************************************/

// Formats time into required format
//   INPUT    nMs       3600000       5520
//   OUTPUT   nFmt==1   1:00:00    0:00:05
//            nFmt==2     00:00      00:05
//            nFmt==3     0.000      5.520
//            nFmt==4      3600          5
char *TimeFormat(int nMs, char *pDest, int nFmt);
//    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Formats lap count into required format
//   INPUT    dwNumLaps==13    dwNumHdts==17
//   OUTPUT   nFmt==0    "13.17"
//            nFmt==1    "13/01.7"
char *LapsFormat(DWORD dwNumLaps, DWORD dwNumHdts, int nFmt, char *pDest);
//    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Convert Radek's event codes into my event indices
int GetEventIdx(int nEventCode);

// Get name of given race part
const char *GetRacePartName(int nRacePart);

// Get total number of miliseconds
int GetNumMs(CTime time);
int GetNumMs(CTimeSpan time);

CString GetGroupName(int nGroup, int nTotalGroups);

CString GetIds(int nIds);

/*********************************************************************** eof **/
