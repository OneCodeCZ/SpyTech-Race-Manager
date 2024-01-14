///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - ODBC support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 01
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslDb.h"

//*****************************************************************************
CWslOdbcException::CWslOdbcException(SQLHANDLE hHandle, int nHandleType, BOOL bAutoDelete)
: CWslException(bAutoDelete)
{
   char szSqlState[5+1], szErrorMsg[1024];
	long nNativeErr;
	short nTextLen;
	SQLRETURN res;

	res = SQLGetDiagRec(nHandleType, hHandle, 1, (BYTE*)&szSqlState, &nNativeErr, (BYTE*)&szErrorMsg, 1024, &nTextLen);
	ASSERT(res == SQL_SUCCESS || res == SQL_SUCCESS_WITH_INFO || res == SQL_NO_DATA);

	if (res == SQL_NO_DATA)
	{
		m_sError = "SQL_NO_DATA -> no error description available";
		m_nNativeError = 0;
	}
	else
	{
		szSqlState[6-1] = 0;
		m_sError.Format("%s\nNative Error Code: %d, SQL State: %s", 
         szErrorMsg,
         nNativeErr,
         szSqlState);
		m_nNativeError = (int) nNativeErr;
	}

//   ASSERT(false);       // just to catch the point of exception
}

//*****************************************************************************
CWslString CWslOdbcException::GetErrorMsg()
{
	return CWslString(m_sError);
}

#ifdef _MFC_VER
//*****************************************************************************
BOOL CWslOdbcException::GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext)
{
	ASSERT(nMaxError > 0);
	CWslString s = GetErrorMsg();
	if (s.GetLength()+1 > (int)nMaxError) { lpszError[0]=0; return false; }
	strcpy(lpszError, s);
	return true;
}
#endif




