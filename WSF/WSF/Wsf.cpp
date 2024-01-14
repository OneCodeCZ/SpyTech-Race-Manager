///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxdllx.h>
#include "main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HINSTANCE g_hInstance;
RECT WsfZeroRect = {0,0,0,0};
char temp[1024], temp2[1024], temp3[1024];
CWsfPluginsArray	g_aAppPlugins;

static AFX_EXTENSION_MODULE WSFDLL = { NULL, NULL };

//*****************************************************************************
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);
	g_hInstance = hInstance;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("WSF.DLL Initializing!\n");
		TRACE1("_WIN32_WINDOWS = 0x%x\n", _WIN32_WINDOWS);
		//TRACE1("_WIN32_WINNT = 0x%x\n", _WIN32_WINNT);
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(WSFDLL, hInstance))
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

		new CDynLinkLibrary(WSFDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("WSF.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(WSFDLL);
	}
	return 1;   // ok
}







