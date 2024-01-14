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
#include "WslWinEventLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CWslWinEventLog::CWslWinEventLog()
{
   m_sSourceName = "Application";
}

/*****************************************************************************/
CWslWinEventLog::~CWslWinEventLog()
{

}

/*****************************************************************************/
void CWslWinEventLog::SetEventSource(const CString& sEventSource)
{
   m_sSourceName = sEventSource;
}

/*****************************************************************************/
void CWslWinEventLog::Report(ETypes eType, const char* pcszMsg)
{
   HANDLE hLog = RegisterEventSource(NULL, m_sSourceName);   
   if (!hLog) { ASSERT(false); return; }
      
	_RPT1(0,"CWslWinEventLog: %s\n", pcszMsg);

   VERIFY(ReportEvent(hLog,
      (WORD) eType,
      1,
      1,
      NULL,
      1,
      0,
      &pcszMsg,
      NULL));

   VERIFY(DeregisterEventSource(hLog)); 
}

/*****************************************************************************/
void CWslWinEventLog::ReportError(const char* pcszMsg, CException* e, BOOL bDeleteException)
{
   char temp[1024];
   VERIFY(e->GetErrorMessage(temp, 1024));

   CWslString s;
   s.Format("%s\n\n%s", pcszMsg, temp);

   ReportError(s);

   if (bDeleteException) e->Delete();
}
