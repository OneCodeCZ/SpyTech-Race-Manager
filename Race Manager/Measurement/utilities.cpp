/******************
*                 *  utilities.cpp
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

#include "stdafx.h"
#include "main.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  UTILITIES
*
*******************************************************************************/

char *TimeFormat(int nMs, char *pDest, int nFmt) {
//    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int nt=nMs%1000;     // number of ms in current second
	int nS=nMs/1000;     // number of seconds in total
	int ns=nS%60;        // number of seconds in current minute
	int nM=nMs/60000;    // number of minutes in total
	int nm=nM%60;        // number of minutes in current hour
	int nH=nMs/3600000;  // number of hours in total

	switch(nFmt) {
	case 1:
//		wsprintf(pDest, "%2d:%2d:%2d.%1d", nH, nm, ns, (nMs%1000)/100);
		wsprintf(pDest, "%2d:%2d:%2d", nH, nm, ns);
		break;
	case 2:
		wsprintf(pDest, "%2d:%2d", nM, ns);
		break;
	case 3:
		if (nMs == 100000) strcpy(pDest, "–");
		else wsprintf(pDest, "%d.%3d", nS, nt);
		break;
	case 4:
		wsprintf(pDest, "%d", nS);
		break;
	default:
		// Invalid option !!
		ASSERT(FALSE);
	}

	for (unsigned int a=0; a<strlen(pDest); a++) {
		if (pDest[a]==' ') pDest[a]='0';
	}

	return pDest;
}

char *LapsFormat(DWORD dwNumLaps, DWORD dwNumHdts, int nFmt, char *pDest) {

	char temp[256];

	switch(nFmt) {
	case 0:
		wsprintf(pDest, "%d.%2d", dwNumLaps, dwNumHdts);
		break;
	case 1:
		wsprintf(temp, "%2d.%d", dwNumHdts / 10, dwNumHdts % 10);
		wsprintf(pDest, "%d/%s", dwNumLaps, temp);
		break;
	}

	for (int a=0; a<strlen(pDest); a++) {
		if (pDest[a]==' ') pDest[a]='0';
	}

	return pDest;
}

/******************************************************************************/

int GetEventIdx(int nEventCode) {
	switch(nEventCode) {
	case RACE_PART_NONE: return -1;
	case RACE_PART_QUAL: return -1;
	case RACE_PART_CONSI: return 0;
	case RACE_PART_QUARTER: return 1;
	case RACE_PART_SEMI: return 2;
	case RACE_PART_FINAL: return 3;
	case RACE_PART_DONE: return -1;
	}
	return -1;
}

/******************************************************************************/

const char *GetRacePartName(int nRacePart) {
	switch (nRacePart) {
	case RACE_PART_QUAL: return GetIds(IDS_PART_QUAL);
	case RACE_PART_CONSI: return GetIds(IDS_PART_HEATS);
	case RACE_PART_QUARTER: return GetIds(IDS_PART_QUARTER);
	case RACE_PART_SEMI: return GetIds(IDS_PART_SEMI);
	case RACE_PART_FINAL: return GetIds(IDS_PART_FINAL);
	}
	return GetIds(IDS_PART_ERROR);
}

/******************************************************************************/

int GetNumMs(CTime time) {
	DWORD dw=0;
	dw = time.GetSecond();
	dw += (time.GetMinute() * 60);
	dw += (time.GetHour() * 3600);
	dw *= 1000;
	return dw;
}

/******************************************************************************/

int GetNumMs(CTimeSpan time) {
	DWORD dw=0;
	dw = time.GetTotalSeconds() * 1000;
	return dw;
}

/******************************************************************************/

CString GetGroupName(int nGroup, int nTotalGroups)
{
	CString s;
	s = (char) 'A' + nTotalGroups - nGroup;
	return s;
}

CString GetIds(int nIds)
{
   CString s;
   s.LoadString(nIds);
   return s;
}

/*********************************************************************** eof **/
