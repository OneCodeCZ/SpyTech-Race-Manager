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

#pragma once

#include "..\generic\WslWin32Exception.h"
#include "WslGenericFileSearch.h"

//******************************************************************************
class WSL_API CWslFileSearch : public CWslGenericFileSearch
{
private:
  HANDLE m_hSearch;

public:
  CWslFileSearch();
  ~CWslFileSearch();

	// Searches for the first file that matches the given filename
  virtual void FindFirst(const char *pcszName) throw (CWslWin32Exception*);

	// Search for the next file.
  virtual void FindNext() throw (CWslWin32Exception*);

	// Closes the search
  virtual void Close();
};

