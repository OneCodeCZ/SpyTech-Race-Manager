///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslSqlScript
// | P |  /  |  |  ~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 010
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\generic\WslString.h"
#include "WslDb.h"
#include "WslSqlScript.h"
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define ERROR_OK							0
#define ERROR_EOF							1
#define ERROR_SYNTAX_ERROR		2
#define ERROR_UNKNOWN_PARAM		3

/*****************************************************************************
*
* CWslSqlSriptException
*
*****************************************************************************/

//*****************************************************************************
CWslSqlScriptException::CWslSqlScriptException(int nErrCode, int nLine)
{
	m_nErrCode = nErrCode;
	m_nLine = nLine;
}

//*****************************************************************************
CWslString CWslSqlScriptException::GetErrorMsg()
{
	CWslString s;

	switch (m_nErrCode)
	{
	case ERROR_SYNTAX_ERROR:
		s.Format("Syntax error on the line %d!", m_nLine);
		break;

	case ERROR_UNKNOWN_PARAM:
		s.Format("Unknown parameter on the line %d!", m_nLine);
		break;

	default: ASSERT(false);
	}

	return s;
}

//*****************************************************************************
BOOL CWslSqlScriptException::GetErrorMessage(LPTSTR lpszError, 
																						 UINT nMaxError, 
																						 PUINT pnHelpContext /*= NULL*/)
{
	ASSERT(nMaxError > 0);
	CWslString s = GetErrorMsg();
	if (s.GetLength()+1 > (int)nMaxError) { lpszError[0]=0; return false; }
	strcpy(lpszError, s);
	return true;
}


/*****************************************************************************
*
* CWslSqlScript
*
*****************************************************************************/

//*****************************************************************************
CWslSqlScript::CWslSqlScript()
{
	m_pszScript = NULL;
}

//*****************************************************************************
CWslSqlScript::~CWslSqlScript()
{
	ReleaseScript();
}

//*****************************************************************************
void CWslSqlScript::LoadScript(const char* pcszFile) throw (CWslWin32Exception*)
{
	delete m_pszScript;
	m_pszScript = NULL;

	int f = _open(pcszFile, _O_RDONLY|_O_BINARY);
	if (f==-1) throw new CWslWin32Exception(ERROR_FILE_NOT_FOUND);
	m_nSize = _filelength(f);
	m_pszScript = new char[m_nSize];
	_read(f, m_pszScript, m_nSize);
	_close(f);
}

//*****************************************************************************
void CWslSqlScript::ReleaseScript()
{
	delete m_pszScript;
	m_pszScript = NULL;
}

//*****************************************************************************
void CWslSqlScript::Perform(CWslDatabase& db, BOOL bUseVariables) 
														throw (CWslException*, CWslSqlScriptException*)
{
	CWslString sCmd;
	int n;

	m_nCurLine = 1;
	m_nCurPos = 0;
	m_bParseVariables = bUseVariables;

	while (1)
	{
		n = GetNextCmd(sCmd);

		if (n == ERROR_EOF)
			return;
		else if (n != ERROR_OK)
			throw new CWslSqlScriptException(n, m_nCurLine);

      if (!sCmd.IsEmpty()) 
      {
   		TRACE1("CWslSqlScript::Perfrom(): Executing command at line %d...\n", m_nCurLine);
         db.ExecuteNoFormat(sCmd);
      }
	}
}

/*****************************************************************************
*
* SQL SCRIPT PARSER
*
*****************************************************************************/

//*****************************************************************************
BOOL CWslSqlScript::GetNextChar(char& ch)
{
	if (m_nCurPos == m_nSize) return false;
	ch = m_pszScript[m_nCurPos++];
	return true;
}

//*****************************************************************************
char CWslSqlScript::PeekNextChar()
{
	return m_nCurPos == m_nSize ? 0 : m_pszScript[m_nCurPos];
}

//*****************************************************************************
int CWslSqlScript::ParseParam(CWslString& sParam)
{
	char ch;
	sParam.Empty();

	while (1)
	{	
		if (!GetNextChar(ch)) return ERROR_EOF;
		if (ch == '$') break;
		sParam += ch;
	}

	_RPT1(0,"CWslSqlScript::ParseParam(): $%s$ = ", sParam);
	int n = OnGetParam(sParam) ? ERROR_OK : ERROR_UNKNOWN_PARAM;
	_RPT1(0,"%s\n", n==ERROR_OK ? sParam : "ERROR_UNKNOWN_PARAM!");

	return  n;
}

//*****************************************************************************
int CWslSqlScript::GetNextCmd(CWslString& sCmd)
{
	int nState, n;
	char ch;
	CWslString sParam;

	sCmd.Empty();
	nState = 0;

	while (1)
	{
		if (!GetNextChar(ch)) return ERROR_EOF;

		switch (nState)
		{
		case 0:
			// NOTE: BUG: " and ' must be separated into two states!!!!!!!
			if (ch=='"' || ch=='\'') { nState = 1; sCmd += ch; }
			else if (ch==';') { return ERROR_OK; }
			else if (ch=='#') { nState = 2; }
			else if (ch==13) { nState = 7; }
			else if (ch==10) { m_nCurLine++; }
			else if (ch=='/')
			{
				if (PeekNextChar()=='*') { nState = 4; GetNextChar(ch); }
				else if (PeekNextChar()=='/') { nState = 2; GetNextChar(ch); }
				else sCmd += ch;
			}
			else if (ch=='$' && m_bParseVariables) 
			{ 
				if (PeekNextChar()!='$')  
				{
					if ((n = ParseParam(sParam)) != ERROR_OK) return n;
					sCmd += sParam;
				}
				else { sCmd += ch; GetNextChar(ch); }
			}
			else if (ch=='-')
			{
				if (PeekNextChar()=='-') { nState = 2; GetNextChar(ch); } 
				else sCmd += ch;
			}
			else sCmd += ch;
			break;

		case 1: 
			if (ch=='"' || ch=='\'') { nState = 0; sCmd += ch; }
			else if (ch=='\\')
			{
				sCmd += ch; 
				if (!GetNextChar(ch)) return ERROR_SYNTAX_ERROR;
				sCmd += ch;
			}
			else if (ch=='$' && m_bParseVariables) 
			{ 
				if (PeekNextChar()!='$')  
				{
					if ((n = ParseParam(sParam)) != ERROR_OK) return n;
					sCmd += sParam;
				}
				else { sCmd += ch; GetNextChar(ch); }
			}
			else sCmd += ch;
			break;

		case 2:
			if (ch==13) 
				{ nState=7; }
			else if (ch==10) 
				{ nState=0; m_nCurLine++; }
			break;

		case 4:
			if (ch=='*' && PeekNextChar()=='/') { nState=0; GetNextChar(ch); }
			else if (ch==10) { m_nCurLine++; }
			break;

		case 7:
			if (ch==10) { nState=0; m_nCurLine++; }
			else return ERROR_SYNTAX_ERROR;
			break;


		default: ASSERT(false);
		}
	}
}



