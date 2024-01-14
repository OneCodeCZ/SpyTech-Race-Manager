///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslRegKey
// | P |  /  |  |  ~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslString.h"
#include "WslWin32Exception.h"
#include "WslRegKey.h"
#include "WslDate.h"

//*****************************************************************************
CWslRegKey::CWslRegKey()
{
	m_hKey = NULL;
}

//*****************************************************************************
CWslRegKey::~CWslRegKey()
{
	if (m_hKey) Close();
}

//*****************************************************************************
bool CWslRegKey::Open(HKEY hKey, const char* pcszSubKey, bool bReadOnly) throw (CWslWin32Exception*)
{
	Close();
	
	DWORD dw = RegOpenKeyEx(hKey, pcszSubKey, 0, 
      bReadOnly ? KEY_READ : KEY_ALL_ACCESS, &m_hKey);

	if (dw == ERROR_FILE_NOT_FOUND) return false;
	if (dw != ERROR_SUCCESS) throw new CWslWin32Exception(dw);

   return true;
}

//*****************************************************************************
void CWslRegKey::Create(HKEY hKey, const char* pcszSubKey) throw (CWslWin32Exception*)
{
	DWORD dw, dwRes;

	Close();

   // Build the key path

   CWslStringArray a;
   a.ExplodeString(pcszSubKey, "\\");

   for (int i=0; i<a.GetSize(); i++)
   {
   	dwRes = RegCreateKeyEx(hKey, a[i], 0, NULL, 0, KEY_ALL_ACCESS, NULL, &m_hKey, &dw);
	   
      if (dwRes != ERROR_SUCCESS) throw new CWslWin32Exception(dwRes);

      if (i > 0) RegCloseKey(hKey);

      ASSERT(m_hKey);
      hKey = m_hKey;
   }
}

//*****************************************************************************
void CWslRegKey::Close()
{
	if (m_hKey)
	{
		RegCloseKey(m_hKey);
		m_hKey = NULL;
	}
}

//*****************************************************************************
int CWslRegKey::GetNumSubKeys() throw (CWslWin32Exception*)
{
	int i;
	ASSERT(m_hKey);

	DWORD dw = RegQueryInfoKey(m_hKey, NULL, NULL, NULL, (DWORD*)&i, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	if (dw != ERROR_SUCCESS) throw new CWslWin32Exception(dw);

	return i;
}

//*****************************************************************************
BOOL CWslRegKey::GetSubKeyName(int nIndex, CWslString &sName) throw (CWslWin32Exception*)
{
	char temp[1024];
	DWORD dwLen = 1024;
	
	ASSERT(m_hKey);
	ASSERT(nIndex >=0);

	DWORD dw = RegEnumKeyEx(m_hKey, nIndex, temp, &dwLen, NULL, NULL, NULL, NULL);
	
	if (dw == ERROR_NO_MORE_ITEMS) 
		return false;
	else if (dw != ERROR_SUCCESS) 
		throw new CWslWin32Exception(dw);
	
	sName = temp;
	return true;
}

//*****************************************************************************
BOOL CWslRegKey::GetValueName(int nIndex, 
                              CWslString& sName, 
                              DWORD* pdwType /*= NULL*/) 
                              throw (CWslWin32Exception*)
{
   char temp[1024];
   DWORD dwLen = 1024;
   
   ASSERT(m_hKey);
   ASSERT(nIndex >=0);
   
   DWORD dw = RegEnumValue(m_hKey, nIndex, temp, &dwLen, NULL, pdwType, NULL, NULL);
   
   if (dw == ERROR_NO_MORE_ITEMS) 
      return false;
   else if (dw != ERROR_SUCCESS) 
      throw new CWslWin32Exception(dw);
   
   sName = temp;
   return true;
}

/*****************************************************************************
*
* SETTING VALUES
*
*****************************************************************************/

//*****************************************************************************
void CWslRegKey::Set(const char* pcszValName, int nValue) throw (CWslWin32Exception*)
{
	ASSERT(IsOpened());
	ASSERT(pcszValName);

	DWORD dw = RegSetValueEx(m_hKey, 
      pcszValName[0] ? pcszValName : NULL,
		NULL,
		REG_DWORD,
		(const BYTE*) &nValue, sizeof(nValue));

	if (dw != ERROR_SUCCESS) throw new CWslWin32Exception(dw);
}

//*****************************************************************************
void CWslRegKey::Set(const char* pcszValName, const CWslString& sValue) throw (CWslWin32Exception*)
{
	ASSERT(IsOpened());
	ASSERT(pcszValName);

	DWORD dw = RegSetValueEx(m_hKey, 
      pcszValName[0] ? pcszValName : NULL,
		NULL,
		REG_SZ,
		(const BYTE*) (const char*) sValue, sValue.GetLength() + 1);

	if (dw != ERROR_SUCCESS) throw new CWslWin32Exception(dw);
}

/*****************************************************************************/
void CWslRegKey::Set(const char* pcszValName, const CWslDate& Date) throw (CWslWin32Exception*)
{
   ASSERT(IsOpened());
   
   Set(pcszValName, Date.Get());
}

/*****************************************************************************
*
* GETTING VALUES
*
*****************************************************************************/

//*****************************************************************************
CWslString CWslRegKey::GetString(const char* pcszValName, const char* pcszDefVal) throw (CWslWin32Exception*)
{
   if (!IsOpened()) return pcszDefVal;

	DWORD dw, dwSize, dwType;
   CWslString sValue;

	ASSERT(pcszValName);
	ASSERT(m_hKey);

	// Get string length
	dw = RegQueryValueEx(m_hKey, pcszValName, NULL, &dwType, NULL, &dwSize);
	if (dw == ERROR_FILE_NOT_FOUND) return pcszDefVal;
	if (dw != ERROR_SUCCESS) throw new CWslWin32Exception(dw);
	if (dwType != REG_SZ) throw new CWslWin32Exception(ERROR_INVALID_DATA);
	
	// Get string
	dw = RegQueryValueEx(m_hKey,pcszValName,NULL,&dwType,(BYTE*) sValue.GetBuffer(dwSize),&dwSize);
	sValue.ReleaseBuffer();
	if (dw != ERROR_SUCCESS) throw new CWslWin32Exception(dw);

   return sValue;
}

//*****************************************************************************
int CWslRegKey::GetInt(const char* pcszValName, int nDefValue)  throw (CWslWin32Exception*)
{
   if (!IsOpened()) return nDefValue;

   DWORD dw, dwSize, dwType;
	int n;

	ASSERT(pcszValName);
	ASSERT(m_hKey);

	dwSize = sizeof(int);
	dw = RegQueryValueEx(m_hKey,pcszValName,NULL,&dwType,(BYTE*)&n,&dwSize);
	if (dw == ERROR_FILE_NOT_FOUND) return nDefValue;
	if (dw != ERROR_SUCCESS) throw new CWslWin32Exception(dw);
	if (dwType != REG_DWORD) throw new CWslWin32Exception(ERROR_INVALID_DATA);
	
	return n;
}

/*****************************************************************************/
CWslDate CWslRegKey::GetDate(const char* pcszValName, const CWslDate& DefDate) throw (CWslWin32Exception*)
{
   CWslDate d;
   d.Set((DWORD) GetInt(pcszValName, 0));
   return d;
}

/*****************************************************************************
*
* DELETING 
*
*****************************************************************************/

//*****************************************************************************
void CWslRegKey::DeleteKey(HKEY hKey, const char* pcszSubKey) throw (CWslWin32Exception*)
{
   DWORD dw = SHDeleteKey(hKey, pcszSubKey);
	if (dw != ERROR_SUCCESS) throw new CWslWin32Exception(dw);
}

//*****************************************************************************
void CWslRegKey::DeleteValue(const char* pcszName) throw (CWslWin32Exception*)
{
	ASSERT(IsOpened());
	ASSERT(pcszName);

	DWORD dw = ::RegDeleteValue(m_hKey, pcszName);
	if (dw != ERROR_SUCCESS) throw new CWslWin32Exception(dw);
}
