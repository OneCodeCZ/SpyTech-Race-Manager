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

// CWslZipArchive represents a file inside a ZIP archive.
// The file is read-only.
// Include <zlib\WslZlib.h> to your project in order to use this class.

#pragma once
#include "zlib\unzip.h"
#include "..\generic\WslWin32Exception.h"

class CWslZipFile;

/*****************************************************************************/
class WSL_API CWslZipArchive
{
protected:
	unzFile		m_hZipArchive;
	
public:
	CWslZipArchive();
	virtual ~CWslZipArchive();

	void Open(const char* pcszPathName) throw (CWslException*);
	void Close();
	CWslZipFile* OpenCurrent() throw (CWslException*);
	inline unzFile GetHandle() const { return m_hZipArchive; }
};
