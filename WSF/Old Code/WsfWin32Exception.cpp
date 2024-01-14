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

#include "stdafx.h"
#include "WsfWin32Exception.h"

//*****************************************************************************
CWsfWin32Exception::CWsfWin32Exception(DWORD dwErrCode) : CException(true)
{
	m_dwError = dwErrCode;
}

//*****************************************************************************
CWsfWin32Exception::CWsfWin32Exception() : CException(true)
{
	m_dwError = GetLastError();
}

//*****************************************************************************
CWsfWin32Exception::CWsfWin32Exception(const char *pcszText) : CException(true)
{
	if ((DWORD)pcszText < 0xFFFF)
	{
		VERIFY(m_sText.LoadString((UINT) pcszText));
	}
	else
	{
		m_sText = pcszText;
	}
}

//*****************************************************************************
CWsfWin32Exception::~CWsfWin32Exception()
{
}

//*****************************************************************************
BOOL CWsfWin32Exception::GetErrorMessage(LPTSTR lpszError, 
																		UINT nMaxError, 
																		PUINT pnHelpContext /*= NULL*/)
{
	if (m_sText.IsEmpty())
	{
		char temp[1024]; temp[0]=0;

		BOOL b = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,    //flags
			NULL,                                           //source   
			m_dwError,																			//message id 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),			//Default language
			temp,																						//buffer for msg      
			1023,			                                      //size of buffer  
			NULL);                                          //arguments
		
		ASSERT(nMaxError > strlen(temp) + 20);
		if (!b) strcpy(temp, "N/A");
		wsprintf(lpszError, "#%d: %s", m_dwError, temp);
		return true;
	}
	else
	{
		strncpy(lpszError, m_sText, nMaxError);
		lpszError[nMaxError-1] = 0;
		return true;
	}
}
