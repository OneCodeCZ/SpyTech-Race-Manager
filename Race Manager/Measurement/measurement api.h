
#pragma once

#ifdef MEASURE_API
#undef MEASURE_API
#define MEASURE_API
#else
#define MEASURE_API __declspec(dllimport)
#endif

class CTIRace;
class CTIRaceEntity;

#include "TIMeasuredRace.h"
#include "TIMeasuredEntity.h"

MEASURE_API void MAPI_RunTraining();
MEASURE_API void MAPI_RunCalibration();
MEASURE_API int MAPI_RemapSingleTrack(int nVirtualTrack);
MEASURE_API int MAPI_TestSoundCard();
MEASURE_API int MAPI_PrintGroups(CTIRace *pRace);
MEASURE_API int MAPI_EditRides(CTIRace *pRace);
MEASURE_API int MAPI_TestSound(int nId);
MEASURE_API void MAPI_TestEB();

// RETURNS:
//   0 .. disconnected
//   1 .. online, power off
//   2 .. offline, power on
MEASURE_API int MAPI_Stm2008Probe();

MEASURE_API void MAPI_InitStmCriticalSection();