///////////////////////////////////////////////////////////////////////////////
//
// CWslFileSearch 
// --------------
// Class for easy file searching
// NOT DEPENDENT ON MFC :-)
//
// (C)1998-2000 Radek Tetik/SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../generic\WslWin32Exception.h"
#include "WslGenericFileSearch.h"
#include "WslFileSearch.h"

//*****************************************************************************
CWslFileSearch::CWslFileSearch()
{ 
  m_hSearch = INVALID_HANDLE_VALUE; 
  m_bFound = false; 
}

//*****************************************************************************
CWslFileSearch::~CWslFileSearch() 
{ 
  Close(); 
}

//*****************************************************************************
void CWslFileSearch::FindFirst(const char *pcszName) throw (CWslWin32Exception*)
{ 
  m_hSearch = FindFirstFile(pcszName, &m_FileInfo); 

  if (m_hSearch != INVALID_HANDLE_VALUE)
  {
    m_bFound = true;
  }
  else
  {
    m_bFound = false;
    DWORD dwErrCode = GetLastError();
    if (dwErrCode != ERROR_FILE_NOT_FOUND) throw new CWslWin32Exception(dwErrCode);
  }
}

//*****************************************************************************
void CWslFileSearch::FindNext()  throw (CWslWin32Exception*)
{ 
#pragma warning(push, 1)
  m_bFound = FindNextFile(m_hSearch, &m_FileInfo);
#pragma warning(pop)
	
  if (!m_bFound)
  {
    DWORD dwErrCode = GetLastError();
    if (dwErrCode != ERROR_NO_MORE_FILES) throw new CWslWin32Exception(dwErrCode);
  }
}

//*****************************************************************************
void CWslFileSearch::Close() 
{ 
  if (m_hSearch != INVALID_HANDLE_VALUE)
  {
    FindClose(m_hSearch);
    m_hSearch = INVALID_HANDLE_VALUE; 
  }
}
