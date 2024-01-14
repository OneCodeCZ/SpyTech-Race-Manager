/******************
*                 *  SrcTitle.cpp
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

#include "stdafx.h"
#include "main.h"
// #include "WndRace.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  CSrcRacePartTitle
*
*******************************************************************************/

CSrcRacePartTitle::CSrcRacePartTitle() : CWslWatchSource() {

	SE(C_WHITE, 3, W_ALIGN_LEFT, C_GREY1, C_WHITE, TRUE);
	RW(NEW_TEXT("", 80, 80, 640, 440, this));

	SE(C_WHITE, 5, W_ALIGN_CENTER, C_GREY1, C_WHITE, FALSE);
	RW(NEW_TEXT("", 90, 90, 620, 60, this));

	SE(C_WHITE, 8, W_ALIGN_CENTER, C_GREY1, C_WHITE, FALSE);
	RW(NEW_TEXT("", 90, 225, 620, 120, this));
	
	SE(C_WHITE, 5, W_ALIGN_LEFT, C_GREY1, C_WHITE, FALSE);
	RW(NEW_TEXT("", 100, 470, 300, 40, this));

	SE(C_WHITE, 5, W_ALIGN_RIGHT, C_GREY1, C_WHITE, FALSE);
	RW(NEW_TEXT("", 400, 470, 300, 40, this));

	SE(C_WHITE, 5, W_ALIGN_CENTER, C_GREY1, C_WHITE, FALSE);
	RW(NEW_TEXT("", 90, 350, 620, 40, this));

//	SE(C_WHITE, 3, W_ALIGN_CENTER, C_GREY1, C_WHITE, FALSE);
//	RW(NEW_TEXT("Popis zavodu", 90, 310, 620, 30, this));
}

/******************************************************************************/

void CSrcRacePartTitle::SetText(const char *pcszRaceName, const char *pcszPartName, const char *pcszLeftBottom, const char *pcszRightBottom) {
	GetText(1)->SetText(pcszRaceName);
	GetText(2)->SetText(pcszPartName);
	GetText(3)->SetText(pcszLeftBottom);
	GetText(4)->SetText(pcszRightBottom);
	GetText(5)->SetText(" ");
}

/******************************************************************************/

void CSrcRacePartTitle::SetGroupName(int nGroup, int nNumGroups)
{
	CString s = GetIds(IDS_RACE_CAPTION_GROUP) + " " + GetGroupName(nGroup, nNumGroups);
	GetText(5)->SetText(s);
}

/******************************************************************************/

CSrcRacePartTitle::~CSrcRacePartTitle() {
	CWslWatchSource::~CWslWatchSource();
}

/*********************************************************************** eof **/
