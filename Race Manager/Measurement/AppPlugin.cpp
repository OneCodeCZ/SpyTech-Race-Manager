///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Race Manager
// | P |  /  |  |  ~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "AppPlugin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CAppPlugin::CAppPlugin()
{

}

/*****************************************************************************/
CAppPlugin::~CAppPlugin()
{

}

/*****************************************************************************/

bool g_FileExists(const char *pcszFile)
{
	CFileFind f;
	if (f.FindFile(pcszFile)) return TRUE;
	return FALSE;
}

/*****************************************************************************/

BOOL CAppPlugin::OnInitInstance()
{
   CString sDir;
   try {
	   sDir = WsfGetApp()->GetAppDataPath() + "Logs";
   }
   catch (CException *pE) {
      WsfReportError(pE, IDS_FAILED_LOG_DIR_CREATION, TRUE);
      return FALSE;
   }
	// Test whether we may write there...
	if (!g_FileExists(sDir)) {
		//AfxMessageBox("Directory for ride logging is about to be created");
		if (!CreateDirectory(sDir, NULL)) {
			AfxMessageBox(IDS_FAILED_LOG_DIR_CREATION);
			return FALSE;
		}
	}

   WsfGetApp()->AddIcons(IDB_SMALL_ICONS, IDB_NORMAL_ICONS);
	return true;//CWsfPlugin::OnInitInstance();
}
