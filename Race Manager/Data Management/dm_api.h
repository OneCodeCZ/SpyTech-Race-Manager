///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager DM.DLL API
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#define RMDM_API __declspec(dllimport)

class CTIMeasuredEntity;
typedef CWslArray<CTIMeasuredEntity*> CEntityArray;
class CTIRaceTeam;
class CTIMeasuredRace;

#include "constants.h"
#include "DbDrv.h"
#include "SetupTrack.h"
#include "TIDriver.h"
#include "TIRS.h"
#include "TIRaceEntity.h"
#include "TIRace.h"
#include "DocWorkspace.h"
#include "App.h"






