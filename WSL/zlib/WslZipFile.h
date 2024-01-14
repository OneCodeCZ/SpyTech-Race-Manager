///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Library
// | P |  /  |  |  ~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 03 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// CWslZipFile represents a file inside a ZIP archive.
// The file is read-only.
// Include <zlib\WslZlib.h> to your project in order to use this class.

#pragma once
#include "zlib\unzip.h"
#include "..\generic\WslString.h"
#include "..\generic\WslWin32Exception.h"
#include "..\files\WslGenericFile.h"

class CWslZipArchive;

/*****************************************************************************/
class WSL_API CWslZipFile : public CWslGenericFile
{
protected:
	unzFile		m_hZipArchive;
	
public:
	CWslZipFile();
	virtual ~CWslZipFile();

	// Opens the current file in the given ZIP archive
	void Open(CWslZipArchive& ZipArchive) throw (CWslException*);
	void Open(unzFile hZipArchive) throw (CWslException*);

  //---------------------------------------------------------------------------
	// From parent
	//---------------------------------------------------------------------------

	virtual CWslString GetName() const;
  virtual void Close() throw (CWslException*);
	virtual BOOL IsClosed() { return m_hZipArchive==NULL; }
  virtual int GetLength() throw (CWslException*);
	virtual int GetPosition() throw (CWslException*);

	// Unsupported
  virtual void SetEndOfFile() throw (CWslException*) { ASSERT(false); }
  virtual void Seek(int nPos, int nMode = FILE_BEGIN) { ASSERT(false); }
  virtual int Read(void *pBuffer, int nSize) throw (CWslException*);
  virtual int Write(const void *pcData, int nSize) throw (CWslException*) { ASSERT(false); return 0; }
  virtual void Flush() throw (CWslException*) { ASSERT(false); }
};
