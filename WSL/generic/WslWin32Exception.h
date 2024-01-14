///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslWin32Exception
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1999-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// NOTES:
// This exception encapsulates the Win32 errors, which are
// handled using the GetLastError() API.

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

#include "WslException.h"
#include "WslString.h"

//******************************************************************************
class WSL_API CWslWin32Exception : public CWslException
{
public:
  DWORD m_dwErrorCode;

public:
	// Constructs object with error code returned by the GetLastError() API.
	// See CWslException for details.
  CWslWin32Exception(BOOL bAutoDelete = true);

	// Specify directly the Win32 error code.
	// See CWslException for details.
	CWslWin32Exception(DWORD dwErrCode, BOOL bAutoDelete = true);

	// Destructor. See CWslException for details.
protected:
  virtual ~CWslWin32Exception();
public:

	// From CWslException
	virtual CWslString GetErrorMsg();
};

/*****************************************************************************/
// Helpers for throwing CWslWin32Exception

#define WsfThrowWin32Exception WslThrowWin32Exception

inline void WslThrowWin32Exception() { throw new CWslWin32Exception(); }
inline void WslThrowWin32Exception(DWORD dwErr) { throw new CWslWin32Exception(dwErr); }











