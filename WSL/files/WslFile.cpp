///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslFile
// | P |  /  |  |  ~~~~~~~~
// | Y | /-  |  |  Copyright (C)1998-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslFile.h"

//******************************************************************************
void CWslFile::Open(const char* pcszName, 
					DWORD dwAttributes /*= FILE_ATTRIBUTE_NORMAL*/,
					DWORD dwCreateMode /*= OPEN_EXISTING*/, 
					DWORD dwShareMode /*= FILE_SHARE_READ|FILE_SHARE_WRITE*/,
					DWORD dwAccessMode /*= GENERIC_READ|GENERIC_WRITE*/)
{
	ASSERT(pcszName);

	// Get the absolute file path and file name

	char szPathName[MAX_PATH+1];
	char* pszFileName;
	if (!GetFullPathName(pcszName, MAX_PATH, szPathName, &pszFileName)) throw new CWslWin32Exception();
	ASSERT(pszFileName);
	
	// Open file

	m_hHandle = CreateFile(
		szPathName, 
		dwAccessMode,
		dwShareMode,
		NULL,
		dwCreateMode,
		dwAttributes,
		NULL
		);
	if (m_hHandle==INVALID_HANDLE_VALUE) throw new CWslWin32Exception();

	// Remember file and path

	m_sName = pszFileName;
	pszFileName[0] = 0;
	m_sPath = szPathName;
}

/*****************************************************************************/
void CWslFile::CreateTemporary(const char* pcszName) throw (CWslWin32Exception*)
{
	ASSERT(pcszName);
	char temp[MAX_PATH+1];

	if (!GetTempPath(MAX_PATH, temp)) throw new CWslWin32Exception();
	strcat(temp, pcszName);
	
	Open(temp, FILE_ATTRIBUTE_TEMPORARY, CREATE_ALWAYS, 0, GENERIC_READ|GENERIC_WRITE);
}

//******************************************************************************
void CWslFile::Close()
{
	if (m_hHandle!=INVALID_HANDLE_VALUE) 
	{ 
		if (!CloseHandle(m_hHandle)) throw new CWslWin32Exception();
		m_hHandle = INVALID_HANDLE_VALUE; 
		m_sName.Empty();
		m_sPath.Empty();
	}
}

//******************************************************************************
int CWslFile::GetPosition()
{
	int nPos = SetFilePointer(m_hHandle, 0, NULL, FILE_CURRENT); 
	if (nPos==-1) throw new CWslWin32Exception();
	return nPos;
}

//******************************************************************************
int CWslFile::GetLength()
{
	int nSize = GetFileSize(m_hHandle, NULL);
	if (nSize == -1) throw new CWslWin32Exception();
	return nSize;
}

//******************************************************************************
void CWslFile::Seek(int nPos, int nMode)
{
	if (SetFilePointer(m_hHandle, nPos, NULL, nMode)==0xFFFFFFFF) throw new CWslWin32Exception();
}

//******************************************************************************
int CWslFile::Read(void *pBuffer, int nSize)
{
	int nRead;
	ASSERT(nSize >= 0);
	if (!nSize) return 0;
	if (ReadFile(m_hHandle, pBuffer, nSize, (DWORD*)&nRead, NULL)==-1) throw new CWslWin32Exception();
	//if (nSize != nRead) throw new CWslWin32Exception(ERROR_READ_FAULT);
	return nRead;
}

//******************************************************************************
int CWslFile::Write(const void *pcData, int nSize)
{
	int nWritten;
	if (!nSize) return 0;
	if (WriteFile(m_hHandle, pcData, nSize, (DWORD*)&nWritten, NULL)==-1) throw new CWslWin32Exception();
	//if (nSize != nWritten) throw new CWslWin32Exception(ERROR_WRITE_FAULT);
	return nWritten;
}

//******************************************************************************
void CWslFile::Copy(const char *pcszWhat, const char *pcszDest, BOOL bFailIfExists /*false*/)
{
	if (!CopyFile(pcszWhat, pcszDest, bFailIfExists)) throw new CWslWin32Exception();
}

//******************************************************************************
void CWslFile::Delete(const char* pcszName)
{
	ASSERT(pcszName);
	if (!DeleteFile(pcszName)) throw new CWslWin32Exception();
}

//******************************************************************************
void CWslFile::Rename(const char *pcszSource, const char *pcszDest)
{
	if (!MoveFile(pcszSource, pcszDest)) throw new CWslWin32Exception();
}

//******************************************************************************
BOOL CWslFile::Exists(const char *pcszName)
{
	try
	{
		CWslFile f;
		f.Open(pcszName, FILE_ATTRIBUTE_NORMAL, OPEN_EXISTING, FILE_SHARE_READ|FILE_SHARE_WRITE, GENERIC_READ);
		f.Close();
		return true;
	}
	catch (CWslWin32Exception* e) 
	{ 
		_RPT1(0,"CWslFile::Exists(): error=%s", e->GetErrorMsg());
		e->Delete(); 
		return false;
	}
}

//******************************************************************************
void CWslFile::SetEndOfFile()
{
	if (!::SetEndOfFile(m_hHandle)) throw new CWslWin32Exception();
}

//******************************************************************************
void CWslFile::Flush()
{
	if (!FlushFileBuffers(m_hHandle)) throw new CWslWin32Exception();
}

/*****************************************************************************/
void CWslFile::Touch() throw (CWslWin32Exception*)
{
	SYSTEMTIME st;
	FILETIME ft;
	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);

	if (!SetFileTime(m_hHandle, &ft,&ft,&ft)) throw new CWslWin32Exception();
}

/*****************************************************************************/
void CWslFile::Touch(const char* pcszFile) throw (CWslWin32Exception*)
{
	ASSERT(pcszFile);
	CWslFile f;
	f.OpenReadOnly(pcszFile);
	f.Touch();
	f.Close();
}

/*****************************************************************************/
int CWslFile::GetLength(const char* pcszFileName) throw (CWslWin32Exception*)
{
	ASSERT(pcszFileName);
	CWslFile f;
	f.OpenReadOnly(pcszFileName);
	return f.GetLength();
}

/*****************************************************************************/
CWslString CWslFile::UrlToPathName(const CWslString& str)
{
   #define __WSLFILE_GET_HEX(x) (\
     ((x) >= '0' && (x) <= '9')\
     ? (x) - '0' \
     : ((x) >= 'a' && (x) <= 'f'\
       ? (x) - 'a' + 10\
       : ((x) >= 'A' && (x) <= 'F'\
         ? (x) - 'A' + 10\
         : 0)\
       )\
     );   

   // See AdamJ's coding book page 21332

   int start = 0;
   if (str.Left(8) == "file:///") start = 8;

   int nState = 0;
   CWslString res;
   int nCharId;

   for (int a=start; a<str.GetLength(); a++) {
      char c = str.GetAt(a);
      switch (nState) {
      case 0:
         if (c == '%') nState = 1;
         else if (c == '/') res += '\\';
         else res += c;
         break;
      case 1:
         nCharId = __WSLFILE_GET_HEX(c);
         nState = 2;
         break;
      case 2:
         nCharId = nCharId * 16 + __WSLFILE_GET_HEX(c);
         nState = 0;
         ASSERT(nCharId);
         res += (char) nCharId;
         break;
      }
   }
   return res;
}
