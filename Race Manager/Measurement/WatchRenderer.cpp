/******************
*                 *  WatchRenderer.cpp
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
#include "WatchGeneric.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// Input from..
volatile CWslWatchGeneric *Watch_pCurrentSource;  // Current source of watches
volatile CWslWatchGeneric *Watch_pNewSource;      // The new source of watches

// Output to..
CWnd *Watch_pWnd;        // Destination of displays

// Status and ctrl variables
volatile BOOL Watch_bKill;		// The thread is to be shut down
volatile BOOL Watch_bFull;		// The thread ought to redraw all the watches
volatile DWORD Watch_dwMessage;	// A command for the thread to change behaviour
volatile DWORD Watch_dwParam;	    // A parameter for the command

// Required by CreateThread function
DWORD Watch_dwReturnVal; // A value that will be returned by the threadproc
HANDLE Watch_hThread;	// Thread handle
DWORD Watch_dwThreadID;	// Required by CreateThread function

CWslWatchSource::CWslWatchSource() {
	CWslWatchGeneric::CWslWatchGeneric();
	m_apWatches.SetSize(0);
}

CWslWatchSource::~CWslWatchSource() {
	DestroyAll();
}

CWslWatchSource::CWslWatchSource(CWslWatchSource *pOwner) {
	m_nBrushIdx=pOwner->m_nBrushIdx;
	m_nAlignMode=pOwner->m_nAlignMode;
	m_nFontColorIdx=pOwner->m_nFontColorIdx;
	m_nFontIdx=pOwner->m_nFontIdx;
	m_nPenIdx=pOwner->m_nPenIdx;
	m_bFramed=pOwner->m_bFramed;
}

int CWslWatchSource::RegisterWatch(CWslWatchGeneric *pWatch) {
	m_apWatches.Append(pWatch);
	return m_apWatches.GetSize()-1;
}

/*******************************************************************************
*
*  Implementation of display thread functions
* 
*******************************************************************************/

DWORD gWatchCounter;

DWORD WINAPI Watch_ThreadProc(LPVOID lpParameter) {
//           ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	TRACE0("Watch_ThreadProc\n");

	// While the application does not want to terminate this thread..
	while (Watch_dwMessage!=W_CMD_END) {

		// Analyze incomming message
		switch(Watch_dwMessage) {

		case W_CMD_CHANGE_SRC:	// App wants to change the source of watches
			// Change variable
			Watch_pCurrentSource=Watch_pNewSource;

			// Next redraw will be the full
			Watch_bFull=TRUE;

			// Reset command variable
			Watch_dwMessage=W_CMD_NOTHING;
			break;

		case W_CMD_FULL_REDRAW:
			Watch_bFull=TRUE;
			Watch_dwMessage=W_CMD_NOTHING;
			break;
		}

		// If there is a output viewport specified
		if (Watch_pCurrentSource) {
			// Get appropriate device context
			CDC *pDC = Watch_pWnd->GetDC();
			// If full, draw black screen first
			if (Watch_bFull) pDC->FillSolidRect(0, 0, 800, 600, RGB(0,0,0));
			// Render watches using this context
			CWslWatchGeneric *pWatch=(CWslWatchGeneric*)Watch_pCurrentSource;

			pWatch->Draw(pDC, Watch_bFull);
			// Reset full redraw flag
			Watch_bFull=FALSE;
			// Release the DC obtained
			Watch_pWnd->ReleaseDC(pDC);
		}

		// Just debug stuff / rem-unrem cycles count .. 2 :-) 
//		MessageBeep(0xffffffff);

		// Step into sweet dreams
		Sleep(50);

		gWatchCounter++;
	}

	// Reset command variable
	Watch_dwMessage=W_CMD_NOTHING;

	TRACE0("Watch_ThreadProc left\n");

	// Return with success
	return 1;
};

/******************************************************************************/

int Watch_CreateThread(CWnd *pWnd) {
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	TRACE0("Watch_CreateThread\n");

	gWatchCounter=0;

	// Set the destination viewport
	Watch_pWnd=pWnd;

	// No source yet specified
	Watch_pCurrentSource=NULL;

	// This will spawn a new thread within the current process
	Watch_hThread=
		CreateThread(
			NULL,				// No or default security descriptor
			0,					// Default stack size
			&Watch_ThreadProc,	// Pointer to thread proc
			NULL,				// Parameters for the thread proc
			0,					// Don't create suspended
			&Watch_dwThreadID
		);

	// The thread has not been created.
	if (Watch_hThread == NULL) return 0;

	TRACE0("Watch_CreateThread left\n");

	// The thread has been created
	return 1;
};

/******************************************************************************/

void Watch_WaitOneUpdate() {
	if (Watch_pCurrentSource) {
		DWORD dwOld=gWatchCounter;
		while (dwOld==gWatchCounter);
	}
}

/******************************************************************************/

int Watch_KillThread() {
//  ~~~~~~~~~~~~~~~~~~

	// Set the message to ask the thread to die
	Watch_dwMessage=W_CMD_END;

	// Sleep for a while
	Sleep(100);

	// Reset timeout variable
	int nTimeOut=0;

	// Wait for the thread to notify us that he's over or until timeout
	while (Watch_dwMessage!=W_CMD_NOTHING && nTimeOut<20) {
		Sleep(100);
		nTimeOut++;
	}

	// If timeout, error asking thread to die has occured
	if (nTimeOut==20) return 0;

	// Otherwise it is OK.
	return 1;
};

/******************************************************************************/

int Watch_SetSource(CWslWatchGeneric *pSource) {
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// See the previous function for comments. The equal ones would be here.
	Watch_pNewSource=pSource;
	Watch_dwMessage=W_CMD_CHANGE_SRC;

	Sleep(100);
	int nTimeOut=0;
	while (Watch_dwMessage!=W_CMD_NOTHING && nTimeOut<20) {
		Sleep(100);
		nTimeOut++;
	}
	if (nTimeOut==20) return 0;

	Watch_RedrawAll();

	return 1;
};

/******************************************************************************/

int Watch_RedrawAll() {
//  ~~~~~~~~~~~~~~~~~
	Watch_dwMessage=W_CMD_FULL_REDRAW;
	return 1;
}

/*********************************************************************** eof **/
