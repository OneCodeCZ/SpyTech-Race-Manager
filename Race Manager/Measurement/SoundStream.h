/******************
*                 *  SoundStream.h
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

#pragma once

/*******************************************************************************
*
*  VARIABLES
*
*******************************************************************************/

extern DWORD gsndFrequency;
extern DWORD gsndChannels;
extern DWORD gsndBitsPerSample;
extern DWORD gsndBufferLength;
extern DWORD gsndTimePosition;
extern DWORD gsndBuffersDone;

#define SND_8_BIT 8
#define SND_16_BIT 16

#define is8bit (gsndBitsPerSample==SND_8_BIT)
#define is16bit (gsndBitsPerSample==SND_16_BIT)

/*******************************************************************************
*
*  CSoundStream
*
*******************************************************************************/

class CSample
{
public:
	void *m_pBuffer;
	int m_nLength;
	
	int Load(const char *pcszFileName);
	int LoadWAV(const char *pcszFileName);

	CSample();
	~CSample();
};

/******************************************************************************/

#define SND_START_SIGNAL       0
#define SND_RIDE_END           1
#define SND_START_AHEAD        2
#define SND_MINUTE_SIGNAL      1
#define SND_INTERRUPTED        3
#define SND_START_SIGNAL_SHORT 4
#define SND_HIGH_BEEP          5

class CSoundStream  
{
public:
	WAVEOUTCAPS woc;
	MMRESULT result;
	WAVEFORMATEX m_wf;
	HWAVEOUT hwo;
	WAVEHDR wh;

   int nUsedDefault;
   int nId;

	int wavewriter;
	int status;

	CWslArray <CSample*> m_pArrSamples;

	MMRESULT Open(DWORD freq, DWORD bits, DWORD channels, HWND m_hWnd);	// Init soundcard
	MMRESULT Enqueue(void * ptr, DWORD length);	// Sends buffer at address ptr to wave output buffer
	MMRESULT Close();	// Deinit soundcard
   MMRESULT Stop();
	CSoundStream();

	// Call this in order to open the sound port.
	int Init(int id = -1);
	int AddSampleFile(const char *pcszFileName);
	int AddWavFile(const char *pcszFileName);
	int PlaySnd(int nIdx);

	CString GetDebugString();

	virtual ~CSoundStream();

};

/*********************************************************************** eof **/
