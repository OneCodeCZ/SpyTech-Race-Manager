///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Library
// | P |  /  |  |  ~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 01 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslZipArchive.h"
#include "WslZipFile.h"

/*****************************************************************************/
CWslZipArchive::CWslZipArchive()
{
	m_hZipArchive = NULL;
}

/*****************************************************************************/
CWslZipArchive::~CWslZipArchive()
{
	Close();
}

/*****************************************************************************/
void CWslZipArchive::Open(const char* pcszPathName) throw (CWslException*)
{
	ASSERT(pcszPathName);
	Close();
	m_hZipArchive = unzOpen(pcszPathName);
	if (!m_hZipArchive) throw new CWslWin32Exception(ERROR_FILE_NOT_FOUND);
	unzGoToFirstFile(m_hZipArchive);
}

/*****************************************************************************/
void CWslZipArchive::Close()
{
	if (m_hZipArchive)
	{
		unzClose(m_hZipArchive);
		m_hZipArchive = NULL;
	}
}



