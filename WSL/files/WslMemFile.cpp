///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslMemFile
// | P |  /  |  |  ~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 09
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslMemFile.h"

/*****************************************************************************/
CWslMemFile::CWslMemFile()
{
	m_bIsOpened = false;
}

/*****************************************************************************/
CWslMemFile::~CWslMemFile()
{
	CloseNoException();
}

/*****************************************************************************/
void CWslMemFile::Create()
{
	Close();
	m_bReadOnly = false;
	m_bIsOpened = true;
	m_bBufferAttached = false;
	m_nPos = 0;
}

/*****************************************************************************/
void CWslMemFile::Open(void* pData, int nSize)
{
	Close();

	m_bIsOpened = true;
	m_bReadOnly = false;
	m_bBufferAttached = true;
	m_nPos = 0;

	m_aData.Attach(pData, nSize);
}

/*****************************************************************************/
void CWslMemFile::OpenReadOnly(const void* pcData, int nSize)
{
	Close();

	m_bIsOpened = true;
	m_bReadOnly = true;
	m_bBufferAttached = true;
	m_nPos = 0;

	m_aData.Attach((void*) pcData, nSize);
}

/*****************************************************************************/
void CWslMemFile::Close() throw (CWslException*)
{
	if (!m_bIsOpened) return;
	
	if (m_bBufferAttached)
		m_aData.Detach();
	else
		m_aData.SetSize(0);

	m_bIsOpened = false;
}

/*****************************************************************************/
int CWslMemFile::Read(void* pBuffer, int nSize) throw (CWslException*)
{
	int nToDo = min(nSize, m_aData.GetSize() - m_nPos);
	memcpy(pBuffer, m_aData.GetData() + m_nPos, nToDo);
	m_nPos += nToDo;
	return nToDo;
}

/*****************************************************************************/
int CWslMemFile::Write(const void* pcData, int nSize) throw (CWslException*)
{
	if (m_bReadOnly)
		throw new CWslWin32Exception(ERROR_FILE_READ_ONLY);

	if (m_nPos+nSize > m_aData.GetSize())
	{
		if (m_bBufferAttached)
			throw new CWslWin32Exception(ERROR_WRITE_FAULT);
		else
			m_aData.SetSize(m_nPos+nSize);
	}

	memcpy(m_aData.GetData() + m_nPos, pcData, nSize);
	m_nPos += nSize;

	return nSize;
}

/*****************************************************************************/
void CWslMemFile::Seek(int nPos, int nMode /*= FILE_BEGIN*/) throw (CWslException*)
{
	int n;

	switch (nMode)
	{
	case FILE_BEGIN: n = nPos; break;
	case FILE_END: n = m_aData.GetSize() - nPos; break;
	case FILE_CURRENT: n = m_nPos + nPos; break;
	default: ASSERT(false); n = 0; break;
	}

  // NOTE: position can be set 1 byte after the file --> EOF

	if (n < 0 || n > m_aData.GetSize())
		throw new CWslWin32Exception(ERROR_SEEK_ON_DEVICE);

	m_nPos = n;
}

/*****************************************************************************/
void CWslMemFile::SetEndOfFile() throw (CWslException*)
{
	if (m_bReadOnly)
		throw new CWslWin32Exception(ERROR_FILE_READ_ONLY);
	if (m_bBufferAttached)
		throw new CWslWin32Exception(ERROR_WRITE_FAULT);
	m_aData.SetSize(m_nPos);	
}









