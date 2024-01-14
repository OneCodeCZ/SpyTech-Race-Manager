///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  InfoBoard
// | P |  /  |  |  ~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik & Adam J. Sporka
// | C |  |__|  |  Last changed: 2003-09-18
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>

/******************************************************************************/

void Debug(const char *pcsz)
{
   CWslDateTime dt;
   dt.SetNow();
   CWslString s;
   s = dt.Format("Y-M-D H:m:S ");

   CString sFileName = WsfGetCApp()->GetAppDataPath() + "Logs/eb.txt";

   FILE *pf = fopen(sFileName, "at");
   if (!pf) {
      // Beep(1000, 100);
      return;
   }
   fputs(s, pf);
   fputs(pcsz, pf);
   fclose(pf);
}

/******************************************************************************/

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
  return TRUE;
}

/******************************************************************************/

extern "C" void  __declspec(dllexport) EBPing()
{
  // Beep(1000,100);
  Debug("--- Ping\n");
}

/******************************************************************************/

extern "C" void  __declspec(dllexport) EBReset()
{
  // Beep(1500,100);
  Debug("--- Reset\n");
}

/******************************************************************************/

extern "C" void __declspec(dllexport) EBUpdateLights(BYTE flags)
{
   // Beep(2000,10);
   CWslString s;
   s.Format("EBUpdateLights %d\n", flags);
   Debug(s);
}

/******************************************************************************/

extern "C" void __declspec(dllexport) EBUpdateLaps(short lane, short num_laps)
{
   // Beep(3000,10);
   CWslString s;
   s.Format("EBUpdateLaps lane %d num_laps %d\n", lane, num_laps);
   Debug(s);
}

/******************************************************************************/

extern "C" void __declspec(dllexport) EBOnRaceEvent(BYTE event)
{
   // Beep(4000,10);
   CWslString s;

   switch (event)
   {
   case 0: s = "dummy"; break;
   case 1: s = "start"; break;
   case 2: s = "stop"; break;
   case 3: s = "interrupted"; break;
   case 4: s = "pause"; break;
   case 5: s = "pause_end"; break;
   }

   s.Format("EBUpdateRaceStart, new state: '%s'\n", s);
   Debug(s);
}

/******************************************************************************/

extern "C" void __declspec(dllexport) EBUpdateTime(short h, short m, short s, short hdt)
{
   // Beep(5000,10);
   CWslString sss;
   sss.Format("EBUpdateTime %02d:%02d:%02d.%02d\n", h, m, s, hdt);
   Debug(sss);
}

/*********************************************************************** eof **/
