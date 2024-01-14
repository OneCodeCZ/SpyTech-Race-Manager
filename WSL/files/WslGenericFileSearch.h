///////////////////////////////////////////////////////////////////////////////
//
// CWslGenericFileSearch
// ~~~~~~~~~~~~~~~~~~~~~
// Abstract class for any file search - like classes.
// NOT DEPENDENT ON MFC :-)
//
// Copyright (C)1999-2000 Radek Tetik / SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

//******************************************************************************
class WSL_API CWslGenericFileSearch
{
protected:
  WIN32_FIND_DATA m_FileInfo;	// Info about the found file
	bool m_bFound;							// = TRUE if previous call to the FindFirst/FindNext
															// method found a new file
public:
	// ---- SEARCH FUNCTIONS -----------------

	// Searches for the first file that matches the given filename
  virtual void FindFirst(const char *pcszName) throw (CWslWin32Exception*) = 0;

	// Search for the next file.
  virtual void FindNext() throw (CWslWin32Exception*) = 0;

	// Closes the search
  virtual void Close() = 0;

	// Returns true if file was found
	inline bool IsFound() const { return m_bFound; };

	// ---- FILE FUNCTIONS -----------------

	// Returns filename of the found file
  const char *GetFileName() const { _ASSERT(m_bFound); return m_FileInfo.cFileName; }; 

	// Returns true if current found file is a directory
  inline bool IsDir() const { _ASSERT(m_bFound); return m_FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? true:false; };

	// Returns info about the found file
	inline const WIN32_FIND_DATA *GetFileInfo() const { _ASSERT(m_bFound); return &m_FileInfo; };
};


