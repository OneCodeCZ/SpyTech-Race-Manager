///////////////////////////////////////////////////////////////////////////////
//
// CWslSdFileSearch 
// ----------------
// Class for easy file searching in SD
// NOT DEPENDENT ON MFC :-)
//
// Copyright (C)1999-2000 Radek Tetik / SpyTech
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

class CWslSdFile;
class CWslSdFatEntry;

//******************************************************************************
class WSL_API CWslSdFileSearch : public CWslGenericFileSearch
{
private:
	CWslSdFile *m_pSd;
	const CWslSdFatEntry *m_pcFatEntry;

public:
  CWslSdFileSearch();
	CWslSdFileSearch(CWslSdFile &sd);
  ~CWslSdFileSearch();

	// Sets SD to operate on

	void SetSD(CWslSdFile &sd);

	// ---- SEARCH FUNCTIONS -----------------

	// Searches for the first file that matches the given filename
	// NOTE: only "*.*" is supported

  virtual void FindFirst(const char *pcszName);

	// Search for the next file.

  virtual void FindNext();

	// Closes the search

  virtual void Close();
};


