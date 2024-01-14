///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL Generic SQL Database Support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 03
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslDb.h"

//*****************************************************************************
CWslDatabase::CWslDatabase()
{
}

//*****************************************************************************
CWslDatabase::~CWslDatabase()
{
}

/*****************************************************************************/
void CWslDatabase::Close()
{
	OnClose();
}

/*****************************************************************************/
void CWslDatabase::Execute(const char* pcszSQL, ...) throw (CWslException*)
{
	ASSERT(pcszSQL);

	char temp[4096];
	va_list l;
	va_start(l, pcszSQL);
	BOOL b = _vsnprintf(temp, 4096, pcszSQL, l) >= 0;
	temp[4096-1] = 0;
	va_end(l);

   if (!b) throw new CWslTextException("CWslDatabase::Execute(): SQL command too long!");

	OnExecute(temp, NULL);
}

/*****************************************************************************/
void CWslDatabase::ExecuteNoFormat(const char* pcszSQL) throw (CWslException*)
{
	ASSERT(pcszSQL);
	OnExecute(pcszSQL, NULL);
}

//*****************************************************************************
void CWslDatabase::Execute(CWslDbCommand& cmd) throw (CWslException*)
{
   if (!cmd.IsEmpty()) OnExecute(cmd, NULL);
}

/*****************************************************************************/
CWslString CWslDatabase::Escape(const char* pcsz, BOOL bEscapePercent /*= false*/)
{
	CWslString s;
	char ch;
	
	// Pessimistically assume that all chars need to be escaped
	char* d = s.GetBufferSetLength(strlen(pcsz)*2+1);
	char* bak = d;

	for (const char* src=pcsz; ch=*src++; *d++=ch)
	{
		switch (ch)
		{
		case '%': if (bEscapePercent) *d++='%'; break;
		case '\\':
		case '\'':
		case '\"': *d++='\\'; break;
		}
	}

	s.ReleaseBuffer(d-bak);
	return s;
}









