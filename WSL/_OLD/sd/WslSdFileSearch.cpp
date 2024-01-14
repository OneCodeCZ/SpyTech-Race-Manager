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

#include "stdafx.h"
#include <WslArray.h>
#include <WslError.h>
#include <WslGenericFile.h>
#include <WslFile.h>
#include <WslSdFile.h>
#include <WslGenericFileSearch.h>
#include <WslSdFileSearch.h>

//*****************************************************************************
CWslSdFileSearch::CWslSdFileSearch()
{ 
	m_pSd = NULL;
  m_bFound = false; 
	m_pcFatEntry = NULL;
	ZeroMemory(&m_FileInfo, sizeof(m_FileInfo));
}

//*****************************************************************************
CWslSdFileSearch::CWslSdFileSearch(CWslSdFile &sd)
{ 
	m_pSd = &sd;
  m_bFound = false; 
	m_pcFatEntry = NULL;
	ZeroMemory(&m_FileInfo, sizeof(m_FileInfo));
}

//*****************************************************************************
CWslSdFileSearch::~CWslSdFileSearch() 
{ 
  Close(); 
}

//*****************************************************************************
void CWslSdFileSearch::SetSD(CWslSdFile &sd)
{ 
	m_pSd = &sd;
}

//*****************************************************************************
void CWslSdFileSearch::FindFirst(const char *pcszName) 
{ 
	_ASSERT(m_pSd);
	_ASSERT(m_pcFatEntry==NULL);
	m_pcFatEntry = m_pSd->m_Fat.FindFirst();
	
	if (m_pcFatEntry)
	{
		m_bFound = true;
		m_pSd->m_Fat.FatEntry2Win32FindData(m_pcFatEntry, &m_FileInfo);
	}
	else m_bFound = false;
}

//*****************************************************************************
void CWslSdFileSearch::FindNext() 
{ 
	_ASSERT(m_pSd);
	_ASSERT(m_pcFatEntry);
	m_pcFatEntry = m_pSd->m_Fat.FindNext(m_pcFatEntry);

	if (m_pcFatEntry)
	{
		m_bFound = true;
		m_pSd->m_Fat.FatEntry2Win32FindData(m_pcFatEntry, &m_FileInfo);
	}
	else m_bFound = false;
}

//*****************************************************************************
void CWslSdFileSearch::Close() 
{ 
	m_bFound = false;
	m_pcFatEntry = NULL;
}
