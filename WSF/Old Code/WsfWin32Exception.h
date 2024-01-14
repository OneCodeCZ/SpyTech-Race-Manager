///////////////////////////////////////////////////////////////////////////////
//
// CWsfWin32Exception
// ~~~~~~~~~~~~~
// Copyright (C)2000 Radek Tetik/SpyTech
// Last updated: 2000 11 22
//
// Exception encapsulating an Win32 error or generic text error.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

//*****************************************************************************
class WSL_API CWsfWin32Exception : public CException
{
protected:
	DWORD		m_dwError;
	CString m_sText;

public:
	CWsfWin32Exception(DWORD dwErrCode);
	CWsfWin32Exception(const char *pcszText);			// ID or pointer to text
	CWsfWin32Exception();              // The err code is obtained by GetLastError()
	virtual ~CWsfWin32Exception();

	// Returns message explaining the error code. Details in MSDN.
	virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL);
};

// Helpers for throwing WslException

inline void WsfThrowWin32Exception() { throw new CWsfWin32Exception(); }
inline void WsfThrowWin32Exception(DWORD dwErr) { throw new CWsfWin32Exception(dwErr); }
inline void WsfThrowWin32Exception(const char *pcszText) { throw new CWsfWin32Exception(pcszText); }

