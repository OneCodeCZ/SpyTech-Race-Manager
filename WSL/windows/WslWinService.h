///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech C++ Library
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz
// | H |  _||   |
// |___|________|  Last changed: 2004 11
//
///////////////////////////////////////////////////////////////////////////////

/*
CWslWinService
- represents a single own-process Windows service
- handles all the communication with the Service Control Manager
- it's a singleton
*/

#pragma once
#include <Winsvc.h>
#include "..\generic\WslException.h"
#include "..\generic\WslWin32Exception.h"


/*****************************************************************************/
class WSL_API CWslWinService  
{
protected:
   SERVICE_STATUS_HANDLE  m_hStatus;
   static CWslWinService* m_pServiceSingleton;
   
public:
	CWslWinService();
	virtual ~CWslWinService();

   // Dispatches the Service Control Manager commands.
   // Must be called from the main application's thread.
   // Returns when all services are stopped and the proccess shoud be terminated.
   static void DispatchSCMCommands() throw (CWslException*);

   // Installs a new service. 
   // The module name is the module that created this process.
   // Service runs under the LocalService account for better security.
   // pcszDependencies = name1\0name2\0name3\0\0 or NULL if none
   static void InstallService(const char* pcszName,
      const char* pcszDisplayName,
      const char* pcszDesription,
      const char* pcszCmdLineParams,
      const char* pcszDependencies) throw (CWslException*); 

   // Remove the given service.
   static void RemoveService(const char* pcszName) throw (CWslException*); 

   //------------------------------------------------------------------------
   // OVERRIDABLES
   //------------------------------------------------------------------------
protected:

   // Called when the service is started.
   // The method runs in a new thread created by the Service Control Manager.
   // When the service finishes its work quit the method with
   // an exit code. If the code is other than 0, a message is written
   // into the system log indicating the service failure.
   virtual int OnRunService() = 0;

   // Called by the Service Control Manager to request the service to stop.
   // The method is called by the thread that called the StartServiceCtrlDispatcher() API.
   virtual void OnStopService() = 0;

   //------------------------------------------------------------------------
   // INTERNALS
   //------------------------------------------------------------------------

protected:

   static DWORD WINAPI __HandlerEx(DWORD dwControl,
      DWORD dwEventType,
      LPVOID lpEventData,
      LPVOID lpContext);

   static void WINAPI __ServiceMain(DWORD dwArgc,
      LPTSTR* lpszArgv);

};
