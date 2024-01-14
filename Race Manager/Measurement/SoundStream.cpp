/******************
*                 *  SoundStream.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka / SpyTech
*      \ \___     *  Sometimes during 1998-1999.
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of RM source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include "main.h"
#include "SoundStream.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*******************************************************************************
*
*  VARIABLES
*
*******************************************************************************/

DWORD gsndFrequency = 44100;
DWORD gsndChannels = 1;
DWORD gsndBitsPerSample = 16;
DWORD gsndBufferLength = 4096;
DWORD gsndTimePosition = 0;
DWORD gsndBuffersDone = 0;

/*******************************************************************************
*
*  CSample
*
*******************************************************************************/

CSample::CSample()
{
	m_pBuffer = 0;
	m_nLength = 0;
}

CSample::~CSample()
{
	if (m_pBuffer) free(m_pBuffer);
}

/******************************************************************************/

int CSample::Load(const char *pcszFileName)
{
	CFile file;
	if (file.Open(pcszFileName, CFile::modeRead | CFile::typeBinary)) {
		int nSize = file.GetLength();
		m_pBuffer = (void*) malloc(nSize);
		if (m_pBuffer) file.Read(m_pBuffer, nSize);
		else {
			file.Close();
			return 0;
		}
		m_nLength = nSize;
		return 1;
	}
	return 0;
}

/******************************************************************************/

int CSample::LoadWAV(const char *pcszFileName)
{
	char temp[1024];
	CFile file;
	if (file.Open(pcszFileName, CFile::modeRead | CFile::typeBinary)) {
		int nSize = file.GetLength();
		int nOffset = 0x08 + 0x18;
		m_nLength = nSize - nOffset;
		m_pBuffer = (void*) malloc(m_nLength);
		file.Read(temp, nOffset);
		if (m_pBuffer) file.Read(m_pBuffer, m_nLength);
		else {
			file.Close();
			return 0;
		}
		return 1;
	}
	return 0;
}

/*******************************************************************************
*
*  CSoundStream
*
*******************************************************************************/

CSoundStream::CSoundStream(){
	status = 0;
   nUsedDefault = 0;
}

/******************************************************************************/

CSoundStream::~CSoundStream(){
	if (status) Close();
}

/******************************************************************************/

CString CSoundStream::GetDebugString() {
	CString sReport;

	CString s;
	int nNumDevices = waveOutGetNumDevs();
	s.Format("Num of sound devices: %d\n", nNumDevices);
	sReport = s;
	
	WAVEOUTCAPS woc;
	result = waveOutGetDevCaps (0, &woc, sizeof (WAVEOUTCAPS));
	if (result == MMSYSERR_NOERROR)	{
		s.Format("Name==%s; formats==%d; channels==%d\n", woc.szPname, woc.dwFormats, woc.wChannels);
		sReport = sReport + s;
	}
	return sReport;
}

/******************************************************************************/

MMRESULT CSoundStream::Open(DWORD freq, DWORD bits, DWORD channels, HWND m_hWnd) {

	// No sound :-)
//	status = 0;
//	return 0;

	status = 0;
	result = waveOutGetDevCaps (nId, &woc, sizeof (WAVEOUTCAPS));
	if (result)	return result;

	WAVEFORMATEX wf;
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = 1;
	wf.nSamplesPerSec = 44100;
	wf.wBitsPerSample = 16;
	wf.nBlockAlign = 1;
	wf.nAvgBytesPerSec = 44100;
	wf.cbSize = 0;

//	PCMWAVEFORMAT wf;
//	wf.wBitsPerSample = 8;
//	wf.wf.wFormatTag = WAVE_FORMAT_PCM;
//	wf.wf.nChannels = 1;
//	wf.wf.nSamplesPerSec = 44100;
//	wf.wf.nBlockAlign = 1;
//	wf.wf.nAvgBytesPerSec = 44100;

	gsndFrequency = freq;
	gsndBitsPerSample = bits;
	gsndChannels = channels;

//	result = waveOutOpen (&hwo, 0, &wf, (DWORD) 0, 0, CALLBACK_NULL);
	result = waveOutOpen (&hwo, nId, &wf, (DWORD) 0, 0, CALLBACK_NULL);

   if (!nUsedDefault) {
	   switch (result) {
	   case MMSYSERR_ALLOCATED:
//		   AfxMessageBox("ERROR: Sound card already allocated.");
		   break;
	   case MMSYSERR_BADDEVICEID:
//		   AfxMessageBox("ERROR: Bad device ID.");
		   break;
	   case MMSYSERR_NODRIVER:
//		   AfxMessageBox("ERROR: No driver present.");
		   break;
	   case MMSYSERR_NOMEM:
//		   AfxMessageBox("ERROR: Not enough memory.");
		   break;
	   case WAVERR_BADFORMAT:
//		   AfxMessageBox("ERROR: Unsupported wave format.");
		   break;
	   case WAVERR_SYNC:
//		   AfxMessageBox("ERROR: Sync error.");
		   break;
	   }
   }

	if (result == MMSYSERR_NOERROR) status=1;
	return result;
}

/******************************************************************************/

MMRESULT CSoundStream::Enqueue (void *ptr, DWORD length) {
	if (!status) return 0;
	wh.lpData = (char *) ptr;
	wh.dwBufferLength = length;
	wh.dwFlags = 0;
	wh.dwLoops = 0;

	waveOutPrepareHeader (hwo, &wh, sizeof (WAVEHDR));
	result = waveOutWrite (hwo, &wh, sizeof (WAVEHDR));
	return result;
}

/******************************************************************************/

MMRESULT CSoundStream::Stop()
{
   return waveOutReset(hwo);
}

/******************************************************************************/

MMRESULT CSoundStream::Close () {
	if (!status) return 0;
	waveOutReset (hwo);
	waveOutClose (hwo);
	waveOutUnprepareHeader (hwo, &wh, sizeof (WAVEHDR));
	return result;
}

/******************************************************************************/

int CSoundStream::Init(int id) 
{
   if (id == -1) {
      id = WsfGetCApp()->m_nWaveOutDevId;
      nUsedDefault = 1;
   }
   nId = id;
	if (Open(44100, 16, 1, NULL) == MMSYSERR_NOERROR) 
   {
		AddWavFile(WsfGetAppPath() + "/data/sound/16b_start.pcm");
		AddWavFile(WsfGetAppPath() + "/data/sound/16b_end.pcm");
		AddWavFile(WsfGetAppPath() + "/data/sound/16b_prepare.pcm");
		AddWavFile(WsfGetAppPath() + "/data/sound/16b_interrupt.pcm");
		AddWavFile(WsfGetAppPath() + "/data/sound/16b_start_short.pcm");
		AddWavFile(WsfGetAppPath() + "/data/sound/16b_high_beep.pcm");
		status = 1;
		return 1;
	}
	else return 0;
}

/******************************************************************************/

int CSoundStream::AddSampleFile(const char *pcszFileName) {
	CSample *pSample = new CSample;
	int nResult = pSample->Load(pcszFileName);
	m_pArrSamples.Append(pSample);
	return nResult;
}

/******************************************************************************/

int CSoundStream::AddWavFile(const char *pcszFileName)
{
	CSample *pSample = new CSample;
	int nResult = pSample->LoadWAV(pcszFileName);
	m_pArrSamples.Append(pSample);
	return nResult;
}

/******************************************************************************/

int CSoundStream::PlaySnd(int nIdx) {
	if (!status) return 0;
	Enqueue(m_pArrSamples[nIdx]->m_pBuffer, m_pArrSamples[nIdx]->m_nLength);
	return 1;
}

/*********************************************************************** eof **/
