///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslWin32Exception
// | P |  /  |  |  ~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1999-2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |   Last changed: 2001 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslWin32Exception.h"

//*****************************************************************************
CWslWin32Exception::CWslWin32Exception(BOOL bAutoDelete /*= true*/) 
: CWslException(bAutoDelete)
{ 
   //ASSERT(false);       // just to catch the point of exception

   m_dwErrorCode = GetLastError();
}

//*****************************************************************************
CWslWin32Exception::CWslWin32Exception(DWORD dwErrCode, BOOL bAutoDelete /*= true*/) 
: CWslException(bAutoDelete)
{ 
//	ASSERT(false);       // just to catch the point of exception

   m_dwErrorCode = dwErrCode;
}

//*****************************************************************************
CWslWin32Exception::~CWslWin32Exception()
{
}

//*****************************************************************************
CWslString CWslWin32Exception::GetErrorMsg()
{
	char szTemp[1024];
	CWslString s;
	
	DWORD dw = ::FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,    //flags
		NULL,                                           //source   
		m_dwErrorCode,                                  //message id 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),      //Default language
		szTemp,                                         //buffer for msg      
		1024-32,  //size of buffer (32 for my extra info added)
		NULL);                                          //arguments

	s.Format("Win32 Error Code %d: %s", 
		m_dwErrorCode, 
		dw ? szTemp : "No error description available!");
	
	return s;
}


