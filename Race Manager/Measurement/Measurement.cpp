/******************
*                 *  Measurement.cpp
*   __|           *  __________________________________________________________
*  / 9 \          *  
*  \__, \         *  Coded by Adam J. Sporka & Radek Tetik / SpyTech
*      \ \___     *  AUG 2000
*       \    '\   *  __________________________________________________________
*      / ____  \  *  
*    ,^ /   ,^ /  *  A part of RM source code.
*     ^^     ^^   *  This file is property of SpyTech Company and in no way
*                 *  may be used by anyone else.
*******************                                                          */

#include "stdafx.h"
#include <afxdllx.h>
#include "main.h"
#include "ScreenCfgCodes.h"
#include "version.h"
#include "TIMeasuredRace.h"
#include "DlgTrainingSettings.h"
#include "DlgRemapSingleTrack.h"
#include "RidesView.h"
#include "threads.h"
#include "AppPlugin.h"
#include "DlgEbTest.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CAppPlugin g_AppPlugin;

/*******************************************************************************
*
*  DllMain
*
*******************************************************************************/

static AFX_EXTENSION_MODULE MeasurementDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("MEASUREMENT.DLL Initializing!\n");

      LoadBoardLib();
      CallEBPing();
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(MeasurementDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(MeasurementDLL);

		CWsfApp::RegisterPlugin(&g_AppPlugin);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("MEASUREMENT.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(MeasurementDLL);
	}
	return 1;   // ok
}

DWORD g_dwTrainThreadID;

/*******************************************************************************
*
*  IMPLEMENTATION OF API FUNCTIONS
*
*******************************************************************************/

MEASURE_API void MAPI_InitStmCriticalSection()
{
//	InitializeCriticalSection(&CSlotTrackMonitor::m_CriSec);
}

MEASURE_API void MAPI_RunTraining()
{
	WsfGetFrame();
	CRaceUIWnd *pWnd = new CRaceUIWnd;
	pWnd->Create(WsfGetFrame(), FALSE, NULL);
	pWnd->InitScreen();
	pWnd->SetUpScreen(CFG_TRAINING);

	if (pWnd->m_TrS.DoModal()==IDCANCEL) {
		pWnd->ShowWindow(SW_HIDE);
		pWnd->DestroyWindow();
		delete pWnd;
		return;
	}

	switch(pWnd->m_TrS.m_nMode) {
	case 0:
		CreateThread(NULL, 0, &TrainingThreadProc, pWnd, 0, &g_dwTrainThreadID);
		break;
	case 1:
		CreateThread(NULL, 0, &SemiautoTrainingThreadProc, pWnd, 0, &g_dwTrainThreadID);
		break;
	}
}

/******************************************************************************/

MEASURE_API void MAPI_RunCalibration()
{
}

/******************************************************************************/

MEASURE_API int MAPI_RemapSingleTrack(int nVirtualTrack)
{
	CDlgRemapSingleTrack hd;
	hd.m_nVirtualTrack = nVirtualTrack;
	if (hd.DoModal()==IDOK) return hd.m_nPhysicalTrack;
	return -1;
}

/******************************************************************************/

MEASURE_API int MAPI_TestSoundCard()
{
	CString sReport;

	CSoundStream snd;
	if (snd.Init()) {
		sReport = "Sound port opened\n";
		sReport += snd.GetDebugString();
		snd.PlaySnd(0);
		snd.PlaySnd(1);
		snd.PlaySnd(2);
	}
	else sReport = "Sound port failed to open\n";
	
	AfxMessageBox(sReport);
	return 1;
}

/******************************************************************************/

MEASURE_API int MAPI_PrintGroups(CTIRace *pRace)
{
	CTIResults results;
   CString sDocument;
//	CArticle *pDoc = new CArticle;

	CTIMeasuredRace *pMeasuredRace = dynamic_cast<CTIMeasuredRace*>(pRace);
	results.GatherDataGroups(pMeasuredRace);

   CString sTemp;
   sTemp.LoadString(IDS_SHEET_GROUP_SEPARATION);

   // Write HTML opening
   results.WriteHtmlHeader(dynamic_cast<CTIMeasuredRace*>(pRace), sDocument, sTemp);
   results.WriteHtmlStandardHeader(dynamic_cast<CTIMeasuredRace*>(pRace), sDocument);
   results.CreateHtmlGroupList(pMeasuredRace, sDocument);
   results.WriteHtmlFooter(sDocument);

   results.ShowHtmlPage(sDocument, sTemp);
	return 1;
}

/******************************************************************************/
int g_nRidesViewID = 0;

MEASURE_API int MAPI_EditRides(CTIRace *pRace)
{
	if (WsfGetApp()->ActivateView(g_nRidesViewID)) return 1;

	CRidesView *pView = new CRidesView();
	pView->m_pRace=dynamic_cast<CTIMeasuredRace*>(pRace);

  CString s((char*)IDS_EDITING_OF_RESULTS); 
	g_nRidesViewID = WsfGetApp()->OpenView(pView, WsfGetWorkspace(), s, false, true, -1, true, IDB_SMALL_ICONS, 0);
	VERIFY(g_nRidesViewID);

	return 1;
}

/******************************************************************************/

MEASURE_API int MAPI_TestSound(int nId)
{
	CSoundStream snd;
	if (!snd.Init(nId))
   {
		AfxMessageBox(IDS_ERROR_OPENING_SOUND_OUTPUT);
		return 0;
	}

   CheckForEnableAcclerationFile();

	snd.PlaySnd(2);
	Sleep(3000);
	snd.Close();

	return 1;
}

/******************************************************************************/

MEASURE_API int MAPI_Stm2008Probe()
{
	CSlotTrackMonitor m;
//	EnterCriticalSection(&m.m_CriSec);
	m.RetrieveSettings();
	if (!m.Connect(TRUE)) return 0;
	int nResult = m.GetControls();
	m.Disconnect();
	if (nResult == 0) return 0;
	return -1;
}

/******************************************************************************/

MEASURE_API void MAPI_TestEB()
{
   CDlgEbTest h;
   h.DoModal();
}  

/*********************************************************************** eof **/
