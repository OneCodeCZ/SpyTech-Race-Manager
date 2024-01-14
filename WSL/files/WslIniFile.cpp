//-------------------------------------------------
// CWslIniFile - class for easy INI files handling
//
// (C)1999 Radek Tetik/WSS
//-------------------------------------------------

#include "stdafx.h"
#include "../generic\WslWin32Exception.h"
#include "WslGenericFile.h"
#include "WslFile.h"
#include "WslIniFile.h"
#include "generic\WslTextStuff.h"

//***************************************************************
CWslIniFile::CWslIniFile()
{
	m_pcszData = NULL;
}

//***************************************************************
CWslIniFile::~CWslIniFile()
{
	if (m_pcszData)
	{
		delete (char*)m_pcszData;
		_RPT0(_CRT_WARN,"WSL: INI file hadn't been closed before destructor was called!");
	}
}

//***************************************************************
void CWslIniFile::Open(const char *pcszName)
{
	char *p = (char*)m_pcszData;
	
	// Read INI file
	
	f.Open(pcszName);
	
	m_dwSize = f.GetLength();
	p = new char[m_dwSize + 1];
	if (!p) throw new CWslWin32Exception(ERROR_NOT_ENOUGH_MEMORY);
	
	f.Read(p, m_dwSize);
	p[m_dwSize] = 0;           // Mark the end of file
	
	f.Close();
	
	// Init
	
	m_pcszData = (const char*)p;
	m_pcszCurSecStart = m_pcszData;
}

//***************************************************************
void CWslIniFile::Close()
{
	if (m_pcszData)
	{
		delete (char*)m_pcszData;
		m_pcszData = NULL;
	}
	else _RPT0(_CRT_WARN,"WSL: Attempt to close already closed INI file!");
}

//***************************************************************
bool CWslIniFile::SetSection(const char *pcszName)
{
	char temp[1024];
	const char *p = m_pcszData;
	
	while (*p)
	{
		p = WslSkipSpaces(p);
		if (*p=='[')
		{
			p = WslGetWord(p+1, temp);
			p = WslSkipSpaces(p);
			if (*p==']' && (!pcszName || !stricmp(temp, pcszName)))
			{
				m_pcszCurSecStart = WslNextLine(p);
				return true;
			}
		}
		p = WslNextLine(p);
	}
	return false;
}

//***************************************************************
bool CWslIniFile::NextSection(const char *pcszName)
{
	const char *p = m_pcszData;
	m_pcszData = m_pcszCurSecStart;
	bool b = SetSection(pcszName);
	m_pcszData = p;
	return b;
}

//***************************************************************
char *CWslIniFile::GetValue(const char *pcszFieldName, 
                            char *pszBuffer, 
                            int pos/*=0*/)
{
	char temp[1024];
	const char *p = m_pcszCurSecStart;
	
	//find field
	while (*p)
	{
		p = WslGetWord(p, temp);
		if (*p=='[') return NULL;
		p = WslSkipSpaces(p);
		if (!stricmp(temp, pcszFieldName)  && *p=='=') goto GET_VALUE;
		p = WslNextLine(p);
	}
	return NULL;
	
	//get value
GET_VALUE:
	p++;
	while (1)
	{
		p = WslGetWord(p, pszBuffer);
		if (!pos--) return pszBuffer;
		p = WslSkipSpaces(p);
		if (*p++ != ',') return NULL;
	}
}

//***************************************************************
int CWslIniFile::GetValue(const char *pcszFieldName, 
                          int pos/*=0*/)
{
	char temp[1024];
	if (!GetValue(pcszFieldName, temp, pos)) return 0;
	return atoi(temp);
}



