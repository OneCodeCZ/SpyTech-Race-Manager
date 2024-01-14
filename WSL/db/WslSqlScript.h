///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslSqlScript
// | P |  /  |  |  ~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2002-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// CWslSqlScript provides support for parametrized SQL scripts. 
//
// Parameters are in the form $param_name$. 
// If you want to write $ character, write $$.
//
// Command is consider as complete when ; is encountered.
// Multiline comments: /* comment */
// Comments to the end of the line: "# comment" or "// comment" or "-- comment"

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

//class CWslString;

#include "..\generic\WslException.h"
#include "..\generic\WslWin32Exception.h"

//*****************************************************************************
class WSL_API CWslSqlScriptException : public CWslException
{
protected:
	int					m_nErrCode;
	int					m_nLine;

public:
	CWslSqlScriptException(int nErrCode, int nLine);
	
	// From CWslException
	virtual CWslString GetErrorMsg();
	// NOTE: This one is implemented for MFC compatibility.
	virtual BOOL GetErrorMessage( LPTSTR lpszError, UINT nMaxError, PUINT pnHelpContext = NULL );
};

//*****************************************************************************
class WSL_API CWslSqlScript  
{
private:
	char*		m_pszScript;
	int			m_nSize;
	int			m_nCurPos;
	int			m_nCurLine;
	BOOL		m_bParseVariables;

public:
	CWslSqlScript();
	virtual ~CWslSqlScript();

  //---------------------------------------------------------------------------
	// API
	//---------------------------------------------------------------------------

	// Loads the given SQL script
	void LoadScript(const char* pcszFile) throw (CWslWin32Exception*);

	// Frees all memory.
	void ReleaseScript();

	// Performs the script on the given database.
	// If bUseVariables is TRUE, variables in this format $name$
	// are substituted for their values using OnGetParam() callback.
	void Perform(CWslDatabase& db, BOOL bUseVariables) 
		throw (CWslException*, CWslSqlScriptException*);

	// Called when an parameter is encoutered.
	// sParam contains the parameter name.v Set sParam to a parameter value.
	// Return FALSE, if the parameter wasn't recognized.
	virtual BOOL OnGetParam(CWslString& sParam) { return false; }

protected:
  //---------------------------------------------------------------------------
	// Internal stuff
	//---------------------------------------------------------------------------

	// Gets the next SQL command from the SQL script
	// Returns an error code, see .cpp. 
	int GetNextCmd(CWslString& sCmd);

	// Gets the next char. If FALSE is returned, EOF has been reached.
	BOOL GetNextChar(char& ch);

	// Peeks the next char. If EOF has been reached, 0 is returned.
	char PeekNextChar();

	// Parse command. Returns an error code, see .cpp
	// sParam will be set to a parameter's value.
	int ParseParam(CWslString& sParam);
};







