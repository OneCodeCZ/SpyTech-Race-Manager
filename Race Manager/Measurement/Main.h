/******************
*                 *  main.h
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka and Radek Tetik / SpyTech
*      \ \___     *  17-AUG-2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of Race Manager source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                   */

#pragma once

#define MEASURE_API __declspec(dllexport)

#define RACE_PART_QUARTER RACE_PART_QUATER

#include "mmsystem.h"
#include "resource.h"
#include "wsf.h"

#include "..\Data Management\dm_api.h"
// #include "..\\ArtEditCtrl\\ae.h"

#include "SoundStream.h"
#include "utilities.h"
#include "WatchGeneric.h"
#include "WndRaceSources.h"
#include "DlgTrainingSettings.h"
#include "RaceUIWnd.h"
#include "TIResults.h"
#include "TIMeasuredEntity.h"
#include "TIMeasuredRace.h"
#include "WslTextPrinter.h"
#include "ExternalBoard.h"

extern int g_nRidesViewID;
/*********************************************************************** eof **/
