/******************
*                 *  SlotTrackMonitor.cpp
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
#include "SlotTrackMonitor.h"
#include "new protocol.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  STM CRC
*
*  (This part has been written by Jakub Ahmadyar)
*
*******************************************************************************/

const unsigned char gpLookupTable[] = {
	0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
	0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
	0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
	0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
	0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
	0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
	0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
	0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
	0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
	0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
	0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
	0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
	0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
	0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
	0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
	0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
	0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
	0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
	0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
	0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
	0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
	0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
	0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
	0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
	0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
	0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
	0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
	0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
	0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
	0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
	0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
	0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};
	
/******************************************************************************/

BYTE inline CRC8(BYTE *pBuffer)
{
	return gpLookupTable[gpLookupTable[gpLookupTable[gpLookupTable[
		gpLookupTable[pBuffer[0]]^pBuffer[1]]^pBuffer[2]]^pBuffer[3]]^
		pBuffer[4]]; // A tak dale, mozno expandovat do For smycky
};

/*******************************************************************************
*
*  CONTRUCTION / DESTRUCTION
*
*******************************************************************************/

CSlotTrackMonitor::CSlotTrackMonitor()
{
	for (int nIdx=0; nIdx<NUMTRACKS; nIdx++) {
		m_dwArrDelta[nIdx]=0;
		m_dwArrNumPulses[nIdx]=0;
		m_dwArrLastValues[nIdx]=0;
      m_dwArrLastAccepted[nIdx]=0;
	}
	m_nCycleCounter=0;
	m_bEmulation=FALSE;
	m_bIgnoreSensors=FALSE;
	m_dwMinLapTime=0;
	m_bEventLog = FALSE;
	m_bQualMode = FALSE;

	m_dwLastTickOfMeasurement=0;
	m_dwFailureCounter = 0;

	EnableMap(FALSE);

	try {
		if (m_File.Open(WsfGetApp()->GetAppDataPath() + "Logs\\comm_log.txt", CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeBinary)) {
			m_File.SeekToEnd();
			CTime time = time.GetCurrentTime();
			CString sTime = time.FormatGmt("%H:%M:%S %d.%m.%Y GMT");
			CString s;
			s.Format("\r\nSTM/2008 Event Log\r\n%12d: Started at %s\r\n", GetTickCount(), sTime);
			m_File.Write(s, s.GetLength());
			m_bEventLog = TRUE;
		}
	}
	catch (CException *pE) {
		WsfReportError(pE, "Error opening log file", true);
	}
}

/******************************************************************************/

CSlotTrackMonitor::~CSlotTrackMonitor()
{
	if (m_bEventLog) {
		CString s;
		s.Format("%12d: End of event log\r\n", GetTickCount());
		m_File.Write(s, s.GetLength());
		m_File.Close();
	}
}

/*******************************************************************************
*
*  INITIALIZE
*
*******************************************************************************/

int CSlotTrackMonitor::Emulate() {
	m_bEmulation=TRUE;
	for (int a=0; a<NUMTRACKS; a++) m_dwArrEmuSensors[a]=0;
	return 1;
}

/******************************************************************************/

int CSlotTrackMonitor::Connect(BOOL bDisplayError) {
	m_bFirst=TRUE;
	int nResult = m_Com.Connect(bDisplayError);
	if (nResult) {
		if (m_Com.Setup()) return 1;
	}
	return 0;
}

/******************************************************************************/

int CSlotTrackMonitor::StartUp() {
	RetrieveSettings();
	int nError = -2;
	if (Connect()) {
		nError = -1;
		if (DetectPresence()) {
			nError =0;
		}
	}

	if (nError != 0) {
		Disconnect();
	}

	switch (nError) {
	case -1:
//		AfxMessageBox(IDS_STM2008_NOT_FOUND);
		break;
	case -2:
//		AfxMessageBox(IDS_ERROR_OPENIN_COMPORT);
		break;
	}
	return (nError == 0);
}

/******************************************************************************/

int CSlotTrackMonitor::Disconnect() {
	return m_Com.Disconnect();
}

/******************************************************************************/

int CSlotTrackMonitor::SelectPort(int nPortIdx) {
	return m_Com.SelectPort(nPortIdx);
}

/******************************************************************************/

int CSlotTrackMonitor::EmulatePass(int nTrackIdx) {
	GetClock(&m_dwArrEmuSensors[nTrackIdx]);
	return 1;
}

/******************************************************************************/

int CSlotTrackMonitor::DetectPresence() {
	if (m_bEmulation) return 1;

	BuildPacket(GET_CONTROLS);	
	if (Command()) {
		if (!DecrunchPacket(0)) return 0;
		return 1;
	}
	else return 0;
}

/******************************************************************************/

void CSlotTrackMonitor::RetrieveSettings() {
	CApp *p = WsfGetCApp();

	SelectPort(p->m_nComPort);
	m_bInvertPending = false;
	EnableMap(p->m_bTrackMappingEnabled);
	for (int a=0; a<8; a++) m_TrackMap[a]=p->m_TrackMap[a];
}

/*******************************************************************************
*
*  COMMON STUFF
*
*******************************************************************************/

void CSlotTrackMonitor::WriteDebugInfo(CString s)
{
	if (m_bEventLog) {
		m_File.Write(s, s.GetLength());
	}
}

/******************************************************************************/

int CSlotTrackMonitor::Command(int nNumPacketsExpected) {
	if (m_bEmulation) return 1;

	if (m_Com.Write(m_Request, PACKSIZE) < PACKSIZE) return 0;
	int nNumRead=m_Com.Read(m_Answer, nNumPacketsExpected*PACKSIZE);
	if (nNumRead < nNumPacketsExpected*PACKSIZE) return 0;
	if (nNumRead > nNumPacketsExpected*PACKSIZE) AfxMessageBox("More than we wanted");
	return 1;
}

/******************************************************************************/

int CSlotTrackMonitor::BuildPacket(BYTE nCmd, DWORD dwValue) {
	BYTE b1, b2, b3, b4;
	b4 = (dwValue % 256); dwValue /= 256;
	b3 = (dwValue % 256); dwValue /= 256;
	b2 = (dwValue % 256); dwValue /= 256;
	b1 = (dwValue % 256);
	m_Request[0]=nCmd;
	m_Request[1]=b1;
	m_Request[2]=b2;
	m_Request[3]=b3;
	m_Request[4]=b4;
	m_Request[5]=CRC8(&m_Request[0]);
	return 1;
}

/******************************************************************************/

BOOL CSlotTrackMonitor::DecrunchPacket(int nPacketIndex, BYTE *nCmd, DWORD *dwValue) {
	BYTE *pPacket=&m_Answer[nPacketIndex*PACKSIZE];
	if (nCmd) *nCmd=pPacket[0];

	DWORD dw=0;
	dw += pPacket[1] * 0x01000000;
	dw += pPacket[2] * 0x00010000;
	dw += pPacket[3] * 0x00000100;
	dw += pPacket[4] * 0x00000001;
	if (dwValue) *dwValue=dw;

	BYTE crc=CRC8(pPacket);

	BOOL bCrcOK = (pPacket[5]==crc);
	return bCrcOK;
}

/*******************************************************************************
*
*  COMMANDS
*
*******************************************************************************/

int CSlotTrackMonitor::GetControls() {
	if (m_bEmulation) {
		m_dwControls = 
			( m_bEmuInvert ? CONTROLS_INVERT : 0 ) |
			(  m_bEmuTimer ?  CONTROLS_TIMER : 0 ) | 
			(  m_bEmuClock ?  CONTROLS_CLOCK : 0 ) |
			(  m_bEmuPower ?  CONTROLS_POWER : 0 ) ;
		return 1;
	}

	BuildPacket(GET_CONTROLS);	
	if (Command()) {
		if (!DecrunchPacket(0, NULL, &m_dwControls)) {
//			AfxMessageBox("Get controls: Not decrunched");
			return 0;
		}
		return 1;
	}
	else return 0;
}

/******************************************************************************/

int CSlotTrackMonitor::SetControls(DWORD dwEnable, DWORD dwDisable) {

	char temp[1024];
	CString s;

	if (m_bEmulation) {
		if (dwDisable & CONTROLS_CLOCK) {
			GetClock(&m_dwEmuClockElapsed);
			m_bEmuClock = FALSE;
		}
		if (dwEnable & CONTROLS_CLOCK) {
			m_bEmuClock = TRUE;
			SetClock(m_dwEmuClockElapsed);
		}
		if (dwDisable & CONTROLS_INVERT) m_bEmuInvert=FALSE;
		if (dwDisable & CONTROLS_POWER) {
			s.Format("Power OFF (%d)", GetTickCount());
			CWslTextPrinter p;
			p.BeginPrint();
			wsprintf(temp, "Power is OFF (%d)", GetTickCount());
			p.Writeln(temp);
			p.EndPrint();
			m_bEmuPower=FALSE;
		}
		if (dwDisable & CONTROLS_TIMER) m_bEmuTimer=FALSE;
		if (dwEnable & CONTROLS_INVERT) m_bEmuInvert=TRUE;
		if (dwEnable & CONTROLS_POWER) {
			CWslTextPrinter p;
			p.BeginPrint();
			wsprintf(temp, "Power is ON  (%d)", GetTickCount());
			p.Writeln(temp);
			p.EndPrint();
			m_bEmuPower=TRUE;
		}
		if (dwEnable & CONTROLS_TIMER) m_bEmuTimer=TRUE;
		return 1;
	}

	m_dwControls = m_dwControls | dwEnable;
	m_dwControls = m_dwControls & ~dwDisable;

//	BYTE bCmd;

	BuildPacket(SET_CONTROLS, m_dwControls);

	if (Command()) {
		if (!DecrunchPacket(0)) return 0;
		return 1;
	}
	else return 0;
}

/******************************************************************************/

int CSlotTrackMonitor::SetControlsEx(DWORD dwEnable, DWORD dwDisable) {
	DWORD dwEnEx = dwEnable | (m_bInvertPending * CONTROLS_INVERT);
	DWORD dwDiEx = dwDisable | (m_bInvertPending * (!CONTROLS_INVERT));
	return SetControls(dwEnEx, dwDiEx);
}

/******************************************************************************/

int CSlotTrackMonitor::SetControlsEx(DWORD dwNewState) {
	DWORD dwNewEx = dwNewState | (m_bInvertPending * CONTROLS_INVERT);
	return SetControls(dwNewEx);
}

/******************************************************************************/

int CSlotTrackMonitor::SetControls(DWORD dwNewState) {
	char temp[1024];
	if (m_bEmulation) {
		if (dwNewState & CONTROLS_CLOCK) {
			m_bEmuClock = TRUE;
			SetClock(m_dwEmuClockElapsed);
		}
		else {
			GetClock(&m_dwEmuClockElapsed);
			m_bEmuClock = FALSE;
		}
		if (dwNewState & CONTROLS_INVERT) m_bEmuInvert=TRUE;
		else m_bEmuInvert=FALSE;
		if (dwNewState & CONTROLS_POWER) {
			CWslTextPrinter p;
			p.BeginPrint();
			wsprintf(temp, "Power is ON  (%d)", GetTickCount());
			p.Writeln(temp);
			p.EndPrint();
			m_bEmuPower=TRUE;
		}
		else {
			CWslTextPrinter p;
			p.BeginPrint();
			wsprintf(temp, "Power is OFF (%d)", GetTickCount());
			p.Writeln(temp);
			p.EndPrint();
			m_bEmuPower=FALSE;
		}
		if (dwNewState & CONTROLS_TIMER) m_bEmuTimer=TRUE;
		else m_bEmuTimer=FALSE;
		return 1;
	}

	m_dwControls=dwNewState;
	BuildPacket(SET_CONTROLS, m_dwControls);
	if (Command()) {
		if (!DecrunchPacket(0)) return 0;
		return 1;
	}
	else return 0;
}

/******************************************************************************/

int CSlotTrackMonitor::GetClock(DWORD *dwClock) {
	if (m_bEmulation) {
		if (!m_bEmuClock) *dwClock=m_dwEmuClockElapsed;
		else *dwClock=GetTickCount()-m_dwEmuClockOrigin;
		return 1;
	}

	BYTE nCmd;
	BuildPacket(GET_CLOCK);
	if (Command()) {
		if (!DecrunchPacket(0, &nCmd, dwClock)) return 0;
		return 1;
	}
	else return 0;
}

/******************************************************************************/

int CSlotTrackMonitor::SetClock(DWORD dwClock) {
	if (m_bEmulation) {
		if (!m_bEmuClock) m_dwEmuClockElapsed=dwClock;
		m_dwEmuClockOrigin=GetTickCount() - dwClock;
		return 1;
	}

	BuildPacket(SET_CLOCK, dwClock);
	if (Command()) {
		if (!DecrunchPacket(0)) return 0;
		return 1;
	}
	else return 0;
}

/******************************************************************************/

int CSlotTrackMonitor::GetTimer(DWORD *dwTimer) {
	if (m_bEmulation) {
		*dwTimer=0;
		return 1;
	}

	BYTE nCmd;
	BuildPacket(GET_TIMER);
	if (Command()) {
		if (!DecrunchPacket(0, &nCmd, dwTimer)) return 0;
		return 1;
	}
	else return 0;
}

/******************************************************************************/

int CSlotTrackMonitor::SetTimer(DWORD dwTimer) {
	if (m_bEmulation) {
		return 1;
	}

	BuildPacket(SET_TIMER, dwTimer);
	if (Command()) {
		if (!DecrunchPacket(0)) return 0;
		return 1;
	}
	else return 0;
}

/******************************************************************************/

int CSlotTrackMonitor::ResetSensors() {
	if (m_bEmulation) {
		return 1;
	}

	BuildPacket(RESET_SENSORS, 0);
	if (Command()) {
		if (!DecrunchPacket(0)) return 0;
		return 1;
	}
	else return 0;
}

/******************************************************************************/

int CSlotTrackMonitor::SetMask(DWORD dwMask) {
	if (m_bEmulation) {
		return 1;
	}

	BuildPacket(SET_MASK, dwMask);
	if (Command()) {
		if (!DecrunchPacket(0)) return 0;
		return 1;
	}
	else return 0;
}

/******************************************************************************/

int CSlotTrackMonitor::GetAllSensors() {

/* BUGA START

// If all the sensors are to be ignored (race is paused), no readout will be
// performed at all and the proc will immediately exit.

	When a pass within the ignore phase occures, the pass will
	be recorded and applied whenever the application returns to the normal mode

   if (m_bIgnoreSensors) return 1;

   BUGA END */

	DWORD dwTicks = GetTickCount();
	DWORD dwDelta = dwTicks - m_dwLastTickOfMeasurement;
	m_dwLastTickOfMeasurement = dwTicks;

	if (dwDelta > 1000) {
		CString s;
		s.Format("%12d: Intertick delta %d\r\n", dwTicks, dwDelta);
		WriteDebugInfo(s);
	}
	int nDelta;

	for (int nIdx=0; nIdx<NUMTRACKS; nIdx++) {

//		int nIdx = m_bQualMode ? 0 : nIdx;

		int nNewValue;

		// First, let's get the new value. It can be either from emulated sensors
		// or from the real STM/2008 device.

		if (m_bEmulation) nNewValue=m_dwArrEmuSensors[nIdx];
		else {

			BOOL bNotOK = TRUE;
			for (int b=0; b<3 && bNotOK; b++) {
				BuildPacket(GET_SENSOR0 + nIdx);
				if (Command()) {
					if (!DecrunchPacket(0, NULL, (DWORD*)&nNewValue)) {
						CString s;
						s.Format("%12d: Decrunch error (sensor idx%d)\r\n", GetTickCount(), nIdx);
						WriteDebugInfo(s);
					}
					else bNotOK = FALSE;
				}
			}
			if (bNotOK) return 0;
		}

		// And now, detect the motion

		if (m_bFirst && (!m_bEmulation)) {
			m_dwArrLastValues[nIdx]=nNewValue;
			m_bArrWasMotion[nIdx]=FALSE;
		}
		else {
			// First, reset the flag
			m_bArrWasMotion[nIdx]=FALSE;
			// If some new value in sensors compared to the old sensor state...
			if (nNewValue != m_dwArrLastValues[nIdx]) {

				// If we are in pause mode,
				if (m_bIgnoreSensors) {
					// Just remember the change but don't do anything else
					m_dwArrLastValues[nIdx]=nNewValue;
				}
				else {
					// Calculate the difference
					nDelta = nNewValue - (int)m_dwArrLastAccepted[nIdx];
					// Test whether there is reason for enabling the pass
					if ((nDelta > m_dwMinLapTime) || (m_bArrNoPassYet[nIdx])) {
						
						if (m_bArrNoPassYet[nIdx]) m_dwArrDelta[nIdx]=0;
						else m_dwArrDelta[nIdx]=nDelta;
						
						m_bArrWasIllegal[nIdx]=FALSE;
						m_bArrWasMotion[nIdx]=TRUE;
						m_bArrNoPassYet[nIdx]=FALSE;
						
						m_dwArrLastValues[nIdx]=nNewValue;
						m_dwArrLastAccepted[nIdx]=nNewValue;
						
						m_dwArrNumPulses[nIdx]++;
					}
					if (nDelta < m_dwMinLapTime) {
						m_bArrWasIllegal[nIdx]=TRUE;
						m_dwArrLastValues[nIdx]=nNewValue;

						CString s;
						s.Format("%12d: Trespass at idx%d, last %d, delta %d\r\n", GetTickCount(), nIdx, m_dwArrLastAccepted[nIdx], nDelta);
						WriteDebugInfo(s);
					}
				}
			}
		}
	}
	m_bFirst=FALSE;
	return 1;
}

/******************************************************************************/

int CSlotTrackMonitor::GetSensor(int nIdx, DWORD *dwClock) {
	if (m_bEmulation) {
		*dwClock=0;
		return 1;
	}

	BuildPacket(GET_SENSOR0 + nIdx);
	if (Command()) {
		if (!DecrunchPacket(0, NULL, dwClock)) return 0;
		return 1;
	}
	else return 0;
}

/******************************************************************************/

void CSlotTrackMonitor::OnRide()
{
	for (int a=0; a<NUMTRACKS; a++) {
		m_bArrNoPassYet[a]=TRUE;
	}
//	ResetVariables();
}

void CSlotTrackMonitor::OnCalibration()
{
}

void CSlotTrackMonitor::OnQualification()
{
}

void CSlotTrackMonitor::OnResetTrainingLane(int nTrack)
{
	// If -1, reset all of them
	if (nTrack==-1) for (int a=0; a<NUMTRACKS; a++) m_bArrNoPassYet[a]=TRUE; 
	else m_bArrNoPassYet[nTrack]=TRUE;
}

/******************************************************************************/

int CSlotTrackMonitor::MeasureAll() {
	m_nCycleCounter++;

	BOOL bSuccess = TRUE;

	if (!GetControls()) { bSuccess = FALSE; }
	if (!GetAllSensors() && bSuccess ) { bSuccess = FALSE; }
	if (!GetClock(&m_dwClock) && bSuccess) { bSuccess = FALSE; }
	if (!GetTimer(&m_dwTimer) && bSuccess) { bSuccess = FALSE; }

	if (!bSuccess) m_dwFailureCounter++;
	if (m_dwFailureCounter == 3) return -1;
	if (m_dwFailureCounter > 0) return 0;

	m_bInvert=m_dwControls&CONTROLS_INVERT;
	m_bTimer=m_dwControls&CONTROLS_TIMER;
	m_bClock=m_dwControls&CONTROLS_CLOCK;
	m_bPower=m_dwControls&CONTROLS_POWER;

	m_dwFailureCounter = 0;
	return 1;
}

/******************************************************************************/

void CSlotTrackMonitor::ResetVariables() {
	for (int a=0; a<NUMTRACKS; a++) {
		m_dwArrDelta[a]=0;
		m_dwArrNumPulses[a]=0;
		m_dwArrLastAccepted[a]=0;
		m_dwArrLastValues[a]=0;
		m_bArrWasMotion[a]=FALSE;
		m_bArrWasIllegal[a]=FALSE;
		m_bArrNoPassYet[a]=TRUE; 
	}
}

/******************************************************************************/

int CSlotTrackMonitor::WasSimultThrespass(int nMaxNumSim) {
	int nSimultNum = 0;
	for (int a=0; a<NUMTRACKS; a++) {
		if (m_bArrWasMotion[a]) nSimultNum++;
	}
	if (nSimultNum > nMaxNumSim) return TRUE;
	return FALSE;
}

/*********************************************************************** eof **/
