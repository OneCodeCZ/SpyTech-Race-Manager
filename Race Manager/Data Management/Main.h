///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 07 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h" 
#include "wsf.h"

#define RMDM_API	__declspec(dllexport)

// Includes

class CApp;
class CTIRaceEntity;
class CTIRaceDrv;
class CTIRaceTeam;
class CTIMeasuredEntity;

typedef CWslArray<CTIMeasuredEntity*> CEntityArray;

#include "constants.h"

#include "DbDrv.h"
#include "SetupTrack.h"

#include "TIResultsFolder.h"
#include "TIRS.h"
#include "TIDriver.h"
#include "TIDrvFolder.h"
#include "TIRSFolder.h"
#include "TIRaceEntity.h"
#include "TIRace.h"
#include "..\\Measurement\\Measurement API.h"
#include "TIRaceDrv.h"
#include "TIRaceTeam.h"

#include "MainFrm.h"
#include "DocWorkspace.h"
#include "App.h"

// Global variables

extern char temp[1024], temp2[1024];
extern CApp *g_pApp;
extern COLORREF g_rgbColors[8];




