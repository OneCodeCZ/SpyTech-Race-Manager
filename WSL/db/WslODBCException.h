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

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

#include "..\generic\WslException.h"
#include "..\generic\WslString.h"

//*****************************************************************************
class WSL_API CWslOdbcException : public CWslException
{
protected:
	CWslString	m_sError;
public:			
	int			m_nNativeError;	

public:
	CWslOdbcException(SQLHANDLE hHandle, int nHandleType, BOOL bAutoDelete = true);

   //---------------------------------------------------------------------------
	// From parents
	//---------------------------------------------------------------------------

	virtual CWslString GetErrorMsg();

#ifdef _MFC_VER
	virtual BOOL GetErrorMessage(LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL);
#endif
};

//*****************************************************************************
// Throws the DB exception. A helper.
inline void WslThrowDbException(SQLHANDLE hHandle, int nHandleType) 
{ 
	throw new CWslOdbcException(hHandle, nHandleType); 
}

