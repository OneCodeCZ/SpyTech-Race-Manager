///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslMemFile
// | P |  /  |  |  ~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 12
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// CWslMemFile represents a chunk of memory that is accesible
// via the standard file interface.

#pragma once

#include "WslGenericFile.h"
#include "..\generic\WslArray.h"

/*****************************************************************************/
class WSL_API CWslMemFile : public CWslGenericFile
{
protected:
	CWslArray<BYTE> m_aData;
	BOOL						m_bReadOnly;
	BOOL						m_bIsOpened;
	BOOL						m_bBufferAttached;
	int							m_nPos;

public:
	CWslMemFile();
	virtual ~CWslMemFile();

	// Creates a new empty file
	void Create() throw (CWslException*);

	// Opens a mem file and attaches the given memory chunk to it.
	// The file size cannot be changed.
	void Open(void* pData, int nSize) throw (CWslException*);
	void OpenReadOnly(const void* pcData, int nSize) throw (CWslException*);

	// Returns the file's data.
	const CWslArray<BYTE>& GetData() const { ASSERT(m_bIsOpened); return m_aData; }

  //---------------------------------------------------------------------------
	// From parent, see CWslGenericFile for details
	//---------------------------------------------------------------------------

  virtual void Close() throw (CWslException*);
  virtual int Read(void* pBuffer, int nSize) throw (CWslException*);
  virtual int Write(const void* pcData, int nSize) throw (CWslException*);
  virtual void Seek(int nPos, int nMode = FILE_BEGIN) throw (CWslException*);
  virtual void SetEndOfFile() throw (CWslException*);

	virtual BOOL IsClosed() { return !m_bIsOpened; }
  virtual int GetLength() throw (CWslException*) { return m_aData.GetSize(); }
  virtual int GetPosition() throw (CWslException*) { return m_nPos; }
  virtual void Flush() throw (CWslException*) { };
};

