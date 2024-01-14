///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSF Test Laboratory
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2004 01
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestLab.h"
#include "MainFrm.h"
#include "TestLabDoc.h"
#include "TestLabView.h"

#include "TestCreateObjects.h"
#include "PropertyTest.h"
#include "TestDeleteObjects.h"
#include "TestDispatchPtr.h"
#include "TestXML.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CApp theApp;

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	//{{AFX_MSG_MAP(CApp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/*****************************************************************************/
CApp::CApp()
{
}

/*****************************************************************************/
BOOL CApp::InitInstance()
{
	AfxEnableControlContainer();

	SetRegistryKey(_T("WSF Test Laboratory"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTestLabDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CTestLabView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();


   try
   {
      { CTestProperties t(NULL); t.Run(); }
      { CTestDispatchPtr t(NULL); t.Run(); }
      { CTestCreateObjects t(NULL); t.Run(); }
      { CTestDeleteObjects t(NULL); t.Run(); }
     // { CTestXML t(NULL); t.Run(); }

      AfxMessageBox("Success");
   }
   catch (CException* e)
   {
      WsfReportError(e, "Error", true);
   }

	return TRUE;
}


