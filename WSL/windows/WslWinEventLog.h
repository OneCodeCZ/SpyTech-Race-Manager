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

#pragma once
#include "..\generic\wslstring.h"

/*****************************************************************************/
class WSL_API CWslWinEventLog  
{
protected:
   enum ETypes 
   { 
      TYPE_INFO = EVENTLOG_INFORMATION_TYPE, 
      TYPE_ERROR = EVENTLOG_ERROR_TYPE,
      TYPE_WARNING = EVENTLOG_WARNING_TYPE
   };

protected:
   CWslString  m_sSourceName;

public:
	CWslWinEventLog();
	~CWslWinEventLog();

   void SetEventSource(const CString& sEventSource);

   void Report(ETypes eType, const char* pcszMsg);

   void ReportInfo(const char* pcszMsg) { Report(TYPE_INFO, pcszMsg); }
   void ReportError(const char* pcszMsg) { Report(TYPE_ERROR, pcszMsg); }
   void ReportWarning(const char* pcszMsg) { Report(TYPE_WARNING, pcszMsg); }

   void ReportError(const char* pcszMsg, CException* e, BOOL bDeleteException);
};
