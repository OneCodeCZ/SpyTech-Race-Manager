/******************
*                 *  SlotTrackMonitor.h
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

#if !defined(AFX_SLOTTRACKMONITOR_H__89564CD0_0FA9_11D4_AAEF_004F49094CF9__INCLUDED_)
#define AFX_SLOTTRACKMONITOR_H__89564CD0_0FA9_11D4_AAEF_004F49094CF9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WslStdSerialCom.h"

#define CONTROLS_INVERT 0x00000001
#define CONTROLS_TIMER  0x00000100
#define CONTROLS_CLOCK  0x00010000
#define CONTROLS_POWER  0x01000000

#define NUMTRACKS 8
#define PACKSIZE 6

BYTE inline CRC8(BYTE *pBuffer);

/*******************************************************************************
*
*  CSlotTrackMonitor
*
*******************************************************************************/

class CSlotTrackMonitor  
{
public:
	CWslStdSerialCom m_Com;

	BYTE m_Request[PACKSIZE];
	BYTE m_Answer[6144];
	BYTE m_TrackMap[8];
	BOOL m_bTrackMappingEnabled;

	int m_nCycleCounter;
	int m_bEmulation;

	BOOL m_bQualMode;
	BOOL m_bIgnoreSensors;

//	static CRITICAL_SECTION m_CriSec;

private:
	// Emulation variables
	DWORD m_dwEmuClockOrigin;
	DWORD m_dwEmuClockElapsed;
	BOOL m_bEmuClock;
	BOOL m_bEmuTimer;
	BOOL m_bEmuInvert;
	BOOL m_bEmuPower;

	BOOL m_bFirst;

	BOOL m_bArrNoPassYet[NUMTRACKS];
	DWORD m_dwArrEmuSensors[NUMTRACKS];

	DWORD m_dwMinLapTime;

	// zeroed at successfull MeasureAll function call.
	DWORD m_dwFailureCounter;

public:
	BOOL m_bClock;
	BOOL m_bTimer;
	BOOL m_bInvert;
	BOOL m_bPower;

	BOOL m_bInvertPending;

	DWORD m_dwArrLastAccepted[NUMTRACKS]; // Last times when the pass has been accepted
	DWORD m_dwArrLastValues[NUMTRACKS];   // Values of sensors
	BOOL  m_bArrWasMotion[NUMTRACKS];     // There was a motion
	BOOL  m_bArrWasIllegal[NUMTRACKS];    // There was a threspass
	DWORD m_dwArrDelta[NUMTRACKS];        // Delta from the last motion
	DWORD m_dwArrNumPulses[NUMTRACKS];    // Number of pulses detected

	DWORD m_dwControls;                   // Current state of controls
	DWORD m_dwClock;                      // etc.
	DWORD m_dwTimer;                      // etc.

	DWORD m_dwLastTickOfMeasurement;

	// --------------------------------------------------------------------------
	// STRANGE EVENTS LOG

	BOOL m_bEventLog;
	CFile m_File;

	// --------------------------------------------------------------------------
	// CONSTRUCTION / DESTRUCTION

	CSlotTrackMonitor();
	virtual ~CSlotTrackMonitor();

	// --------------------------------------------------------------------------
	// INITIALIZATION CALLS
	
	int SelectPort(int nPortIdx); // Specify COM port to use. The index is zero based
	// PARAMETERS:
	//   bDisplayError .. display an error message, if any error occures.
	int Connect(BOOL bDisplayError = TRUE); // Connect to the COM port
	int Emulate(); // Enter to emulation mode. Call it instead of Connect()
	int Disconnect(); // Disconnect from the COM port

	int StartUp();

	// Invoke COM configuration window
	int Setup(HWND hWnd) { return m_Com.SetupDialog(hWnd); };
	// Detects the device.
	// RETURNS:
	//   0 .. not detected
	//   1 .. detected
	int DetectPresence();
	void RetrieveSettings(); // Retrieve settings from the application
	void ResetVariables(); // Resets all counter variables

	void SetMinLapTime(DWORD dwMinLapTime) { m_dwMinLapTime=dwMinLapTime; }	;

	void EnableMap(BOOL bEnable=TRUE) { m_bTrackMappingEnabled=bEnable; };
	int Map(int nVirtualTrack) { return m_bTrackMappingEnabled ? m_TrackMap[nVirtualTrack] : nVirtualTrack; };

	// --------------------------------------------------------------------------
	// COMMON STUFF


	// Construct packet for STM/2008
	int BuildPacket(BYTE nCmd, DWORD dwValue=0);
	// Get data from STM/2008 packet received
	BOOL DecrunchPacket(int nPacketIdx, BYTE *nCmd=NULL, DWORD *dwValue=NULL);

	// Execute command. That is -- issue a request to STM/2008 and wait for
	// answer. If anything fails, this function returns zero. The data that are
	// stored in member variables cannot be used.
	int Command(int nNumPacketsExpected=1);
	// Execute commands for each variable that can be measured.
	int MeasureAll();

	// Returns TRUE if there were more than 4 rides simultaneous
	int WasSimultThrespass(int nMaxNumSim);

	void WriteDebugInfo(CString s);

	// --------------------------------------------------------------------------
	// EMULATION CALLS

	int EmulatePass(int nTrackIdx);

	// --------------------------------------------------------------------------
	// STATE RETRIEVAL CALLS

	int GetControls();
	int SetControls(DWORD dwEnable, DWORD dwDisable);
	int SetControls(DWORD dwNewState);
	int SetControlsEx(DWORD dwEnable, DWORD dwDisable);
	int SetControlsEx(DWORD dwNewState);

	int GetClock(DWORD *dwClock);
	int SetClock(DWORD dwClock);

	int GetTimer(DWORD *dwTimer);
	int SetTimer(DWORD dwTimer);

	int ResetSensors();
	int SetMask(DWORD dwMask);

	void OnRide();
	void OnCalibration();
	void OnQualification();
	void OnResetTrainingLane(int nTrack=-1);

	int GetAllSensors();
	int GetSensor(int nIdx, DWORD *dwClock);

	// --------------------------------------------------------------------------

	// Call this when entering a pause mode
	void IgnoreSensors(BOOL bNewMode=TRUE) { m_bIgnoreSensors=bNewMode; };
};

#endif // !defined(AFX_SLOTTRACKMONITOR_H__89564CD0_0FA9_11D4_AAEF_004F49094CF9__INCLUDED_)
