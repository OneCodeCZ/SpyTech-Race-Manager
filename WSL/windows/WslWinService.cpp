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

#include "stdafx.h"
#include "WslWinService.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CWslWinService* CWslWinService::m_pServiceSingleton = NULL;

/*****************************************************************************/
CWslWinService::CWslWinService()
{
   ASSERT(!m_pServiceSingleton); // There can be only one service in the process
   m_pServiceSingleton = this;
}

/*****************************************************************************/
CWslWinService::~CWslWinService()
{
   m_pServiceSingleton = NULL;
}

/*****************************************************************************/
void CWslWinService::DispatchSCMCommands() throw (CWslException*)
{
   ASSERT(m_pServiceSingleton);

   SERVICE_TABLE_ENTRY t[] = 
   { 
      { "", m_pServiceSingleton->__ServiceMain }, 
      { NULL, NULL } 
   };

   if (!StartServiceCtrlDispatcher(t))
      throw new CWslWin32Exception();
}

/*****************************************************************************/
DWORD WINAPI CWslWinService::__HandlerEx(DWORD dwControl,
                                         DWORD dwEventType,
                                         LPVOID lpEventData,
                                         LPVOID lpContext)
{
   CWslWinService* pSvc = (CWslWinService*) lpContext;
   ASSERT(pSvc);

   switch (dwControl)
   {
   case SERVICE_CONTROL_STOP:
      {
         pSvc->OnStopService();

         SERVICE_STATUS s;
         s.dwServiceType        = SERVICE_WIN32_OWN_PROCESS; 
         s.dwCurrentState       = SERVICE_STOP_PENDING; 
         s.dwControlsAccepted   = SERVICE_ACCEPT_STOP;
         s.dwWin32ExitCode      = 0; 
         s.dwServiceSpecificExitCode = 0; 
         s.dwCheckPoint         = 0; 
         s.dwWaitHint           = 0; 

         if (!SetServiceStatus(pSvc->m_hStatus, &s)) { ASSERT(false); }
      }
      return NO_ERROR;

   default:
      return ERROR_CALL_NOT_IMPLEMENTED;
   }
}

/*****************************************************************************/
void WINAPI CWslWinService::__ServiceMain(DWORD dwArgc,
                                          LPTSTR* lpszArgv)
{
#ifdef _DEBUG
   Beep(800, 100);
#endif

   ASSERT(dwArgc > 0);
   ASSERT(m_pServiceSingleton);

   CWslWinService* pSvc = m_pServiceSingleton; 

   // Register command handler

   pSvc->m_hStatus = RegisterServiceCtrlHandlerEx(lpszArgv[0], 
      pSvc->__HandlerEx, 
      (void*) pSvc); 

   if (!pSvc->m_hStatus) { ASSERT(false); return; }

   // Mark as RUNNING

   SERVICE_STATUS s;
   s.dwServiceType        = SERVICE_WIN32_OWN_PROCESS; 
   s.dwCurrentState       = SERVICE_RUNNING; 
   s.dwControlsAccepted   = SERVICE_ACCEPT_STOP;
   s.dwWin32ExitCode      = 0; 
   s.dwServiceSpecificExitCode = 0; 
   s.dwCheckPoint         = 0; 
   s.dwWaitHint           = 0; 

   if (!SetServiceStatus(pSvc->m_hStatus, &s)) { ASSERT(false); return; }

   // RUN!

   s.dwWin32ExitCode = pSvc->OnRunService();

#ifdef _DEBUG
   Beep(800, 100);
#endif

   // Mark as STOPPED

   s.dwCurrentState = SERVICE_STOPPED; 

   if (!SetServiceStatus(pSvc->m_hStatus, &s)) { ASSERT(false); }
}

/*****************************************************************************
*
* INSTALL/REMOVE
*
*****************************************************************************/

/*****************************************************************************/
void CWslWinService::InstallService(const char* pcszName,
                                    const char* pcszDisplayName,
                                    const char* pcszDescription,
                                    const char* pcszCmdLineParams,
                                    const char* pcszDependencies)
                                    throw (CWslException*) 
{
   ASSERT(pcszDescription);
   ASSERT(pcszCmdLineParams);
   ASSERT(pcszDisplayName);
   ASSERT(pcszName);

   SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE | GENERIC_WRITE);
   if (!hSCM) throw new CWslWin32Exception();

   // EXE path

   char temp[1024];
	HMODULE h = GetModuleHandle(NULL);
	VERIFY(GetModuleFileName(h, temp, 1024));

   CString sPathName;
   sPathName.Format("\"%s\" %s", temp, pcszCmdLineParams);

   // Create   

   SC_HANDLE hService = CreateService(hSCM, 
      pcszName, 
      pcszDisplayName, 
      SERVICE_ALL_ACCESS,        // desired access 
      SERVICE_WIN32_OWN_PROCESS, // service type 
      SERVICE_AUTO_START,        // start type 
      SERVICE_ERROR_NORMAL,      // error control type 
      sPathName,                 // service's binary 
      NULL,                      // no load ordering group 
      NULL,                      // no tag identifier 
      pcszDependencies,          // no dependencies 
      "NT AUTHORITY\\LocalService",
      NULL);                     // no password 

   if (!hService)
   {
      DWORD dwErr = GetLastError();
      VERIFY(CloseServiceHandle(hSCM)); 
      throw new CWslWin32Exception(dwErr);
   }

   // Description

   SERVICE_DESCRIPTION sd;
   sd.lpDescription = (char*) pcszDescription;

   ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, (void*) &sd);

   // Done

   VERIFY(CloseServiceHandle(hService));
   VERIFY(CloseServiceHandle(hSCM));
}

/*****************************************************************************/
void CWslWinService::RemoveService(const char* pcszName) throw (CWslException*)
{
   SC_HANDLE hSCM = OpenSCManager(NULL, NULL, DELETE);
   if (!hSCM) throw new CWslWin32Exception();

   SC_HANDLE hService = OpenService(hSCM, pcszName, SERVICE_ALL_ACCESS);
   
   if (!hService)
   {
      DWORD dwErr = GetLastError();
      VERIFY(CloseServiceHandle(hSCM)); 
      throw new CWslWin32Exception(dwErr);
   }

   BOOL b = DeleteService(hService);
   DWORD dwErr = GetLastError();

   VERIFY(CloseServiceHandle(hService)); 
   VERIFY(CloseServiceHandle(hSCM)); 

   if (!b) throw new CWslWin32Exception(dwErr);
}

