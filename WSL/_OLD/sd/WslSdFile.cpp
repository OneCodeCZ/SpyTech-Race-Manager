///////////////////////////////////////////////////////////////////////////////
//
// CWslSdFile 
// ~~~~~~~~~~
// Class for handling SD5 files
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

//*****************************************************************************
CWslSdFile::CWslSdFile()
{
	m_pcFatEntry = NULL;
	m_pCoder = NULL;
}

//*****************************************************************************
CWslSdFile::~CWslSdFile()
{
	CloseSD();
}

//*****************************************************************************
void CWslSdFile::OpenSD(const char *pcszName, 
												DWORD dwAccess /*= GENERIC_READ | GENERIC_WRITE*/, 
												int nOffset /*= 0*/)
{
	SDFILE_HEADER Hdr;

	if (!f.IsClosed()) new throw CWslWin32Exception(ERROR_TOO_MANY_OPEN_FILES);

	// Init

	m_nOffset = nOffset;
	m_dwAccess = dwAccess;

	// Read header

	f.Open(pcszName, FILE_ATTRIBUTE_NORMAL, OPEN_EXISTING, 0, dwAccess);
	f.Seek(m_nOffset);
	f.Read(&Hdr, sizeof(Hdr));

	// Check SD file header
	
	bool bHdrOk = true;

	if (Hdr.chFileId[0]!='S' || Hdr.chFileId[1]!='D' || Hdr.nVersion > SD_VERSION) bHdrOk = false;
	if (Hdr.nFatSize < 0) bHdrOk = false;

	if (!bHdrOk)
	{
		f.Close(false);
 		throw new CWslWin32Exception(ERROR_INVALID_DATA);
	}

	// Read the FAT block & construct it in memory

	SDFILE_FAT *pFat = (SDFILE_FAT*) new BYTE[Hdr.nFatSize];
	_ASSERT(pFat);

	try
	{
		f.Seek(Hdr.nFatPos + m_nOffset);
		f.Read(pFat, Hdr.nFatSize);
	}
	catch (CWslWin32Exception*)
	{
		f.Close(false);
		delete pFat;
		throw;
	}

	m_Fat.Init(pFat);
	delete pFat;
}

//*****************************************************************************
void CWslSdFile::CreateSD(const char *pcszName, int nOffset /*= 0*/)
{
	if (!f.IsClosed()) throw new CWslWin32Exception(ERROR_TOO_MANY_OPEN_FILES);

	// Init

	m_Fat.Init(NULL);
	strcpy(m_szSdName, pcszName);
	m_nOffset = nOffset;
	m_dwAccess = GENERIC_READ | GENERIC_WRITE;

	// Create SD file

	f.Create(pcszName);
}

//*****************************************************************************
void CWslSdFile::CloseSD()
{
	if (f.IsClosed()) return;
	Close();

	if (m_dwAccess & GENERIC_WRITE)
	{
		try
		{
			Flush();
		}
		catch (...)
		{
			m_Fat.Deinit();
			f.Close(false);
			throw;
		}
	}

	m_Fat.Deinit();
	f.Close();
}

//*****************************************************************************
int CWslSdFile::GetTotalLength()
{
	const CWslSdFatEntry *pc = m_Fat.FindFirst();
	int nLen = 0;

	while (pc)
	{
		nLen += pc->m_nSize;
		pc = m_Fat.FindNext(pc);
	}

	return nLen;
}

//*****************************************************************************
void CWslSdFile::Open(const char *pcszName, 
											DWORD dwAttributes /*= FILE_ATTRIBUTE_NORMAL*/,
											DWORD dwCreateMode /*= OPEN_EXISTING*/, 
											DWORD dwShareMode /*= FILE_SHARE_READ|FILE_SHARE_WRITE*/,
											DWORD dwAccessMode /*= GENERIC_READ | GENERIC_WRITE*/)
{
	// xxxxxModes are not supported

	_ASSERT(dwShareMode == FILE_SHARE_READ + FILE_SHARE_WRITE);
	_ASSERT(dwAccessMode == GENERIC_READ + GENERIC_WRITE);

	// Check if no file is currently opened

	if (m_pcFatEntry) throw new CWslWin32Exception(ERROR_TOO_MANY_OPEN_FILES);
	
	// Translate attributes

	switch (dwAttributes)
	{
	case FILE_ATTRIBUTE_NORMAL		 : dwAttributes = 0; break;
	case FILE_ATTRIBUTE_COMPRESSED : dwAttributes = SDFILE_PROP_COMPRESSED; break;
	case FILE_ATTRIBUTE_ENCRYPTED  : dwAttributes = SDFILE_PROP_ENCRYPTED; break;
	default: throw new CWslWin32Exception(ERROR_INVALID_PARAMETER);
	}

	// Find file in the FAT

	m_pcFatEntry = m_Fat.FindEntry(pcszName);

	// Open or create

	switch (dwCreateMode)
	{
		case OPEN_EXISTING:
			if (!m_pcFatEntry) throw new CWslWin32Exception(ERROR_FILE_NOT_FOUND);
			break;

		case CREATE_NEW:
			if (m_pcFatEntry) throw new CWslWin32Exception(ERROR_FILE_EXISTS);
			// no break

		case CREATE_ALWAYS:
			m_pcFatEntry = m_Fat.AddEntry(pcszName, dwAttributes);
			_ASSERT(m_pcFatEntry);
			break;

		default:
			m_pcFatEntry = NULL;
			throw new CWslWin32Exception(ERROR_INVALID_PARAMETER);
	}

	// Init coder

	switch (m_pcFatEntry->m_dwProperties & (SDFILE_PROP_COMPRESSED | SDFILE_PROP_ENCRYPTED))
	{
	case 0:
		_ASSERT(m_pCoder==NULL);
		m_bSeekable = true;
		break;

	case SDFILE_PROP_COMPRESSED:
		m_pCoder = new CWslSdLZ78Coder();
		m_bSeekable = false;
		break;

	case SDFILE_PROP_ENCRYPTED:
		_ASSERT(false);
		m_bSeekable = false;
		break;

	default:
		_ASSERT(false);
	}

	// Read & write together is not supported!

	if (m_pCoder && m_dwAccess == GENERIC_READ+GENERIC_WRITE)
	{
		delete m_pCoder;
		m_pCoder = NULL;
		m_pcFatEntry = NULL;
		throw new CWslWin32Exception(ERROR_INVALID_PARAMETER);
	}

	// Start coder

	if (m_pCoder) m_pCoder->Start(m_dwAccess & GENERIC_WRITE);

	// Init other stuff

  m_nDecodedPos = 0;
	m_nRawPos = 0;
}

//*****************************************************************************
void CWslSdFile::SetEndOfFile()
{
	if (!m_pcFatEntry) throw new CWslWin32Exception(ERROR_INVALID_HANDLE);

	// Set end

	m_Fat.SetEntrySize(m_pcFatEntry, m_nDecodedPos);
}

//*****************************************************************************
void CWslSdFile::Close(bool bThrowException /*= true*/)
{
	if (!m_pcFatEntry) return;

	if (m_pCoder) 
	{
		try
		{
			if (m_pCoder->IsEncodeMode() && m_nBufferPos) __WriteCodingBuffer();
			m_pCoder->Finish(m_Buffer);
			if (m_Buffer.GetSize()) __WriteRaw(m_Buffer.GetData(), m_Buffer.GetSize());
		}
		catch (CWslWin32Exception *pErr) { if (bThrowException) throw; else pErr->Delete(); }
		
		delete m_pCoder;
		delete m_pbyBuffer;

		m_pCoder = NULL;
		m_pbyBuffer = NULL;
	}

	m_pcFatEntry = NULL;
}

//*****************************************************************************
bool CWslSdFile::IsClosed()
{
	return !m_pcFatEntry;
}

//*****************************************************************************
void CWslSdFile::SeekToBegin()
{
	if (!m_pcFatEntry) throw new CWslWin32Exception(ERROR_INVALID_HANDLE);
  if (m_bSeekable) throw new CWslWin32Exception(ERROR_INVALID_FUNCTION);

	m_nDecodedPos = 0;
}

//*****************************************************************************
void CWslSdFile::SeekToEnd()
{
	if (!m_pcFatEntry) throw new CWslWin32Exception(ERROR_INVALID_HANDLE);
  if (m_bSeekable) throw new CWslWin32Exception(ERROR_INVALID_FUNCTION, m_pcFatEntry->m_pszFileName);

	m_nDecodedPos = m_pcFatEntry->m_nSize;
}

//*****************************************************************************
void CWslSdFile::Seek(int nPos, int nMode /*= FILE_BEGIN*/)
{
	if (!m_pcFatEntry) throw new CWslWin32Exception(ERROR_INVALID_HANDLE);
  if (m_bSeekable) throw new CWslWin32Exception(ERROR_INVALID_FUNCTION, m_pcFatEntry->m_pszFileName);

	// Seek

	switch (nMode)
	{
		case FILE_BEGIN		: m_nDecodedPos = nPos; break;
		case FILE_CURRENT	: m_nDecodedPos += nPos;
		case FILE_END			:	m_nDecodedPos = m_pcFatEntry->m_nSize - nPos; break;
		default						: _ASSERT(false);
	}

	// Check if the current pos is within the file

	if (m_nDecodedPos >= m_pcFatEntry->m_nSize) 
		throw new CWslWin32Exception(ERROR_SEEK_ON_DEVICE, m_pcFatEntry->m_pszFileName);
}

//*****************************************************************************
int CWslSdFile::GetLength()
{
	if (!m_pcFatEntry) throw new CWslWin32Exception(ERROR_INVALID_HANDLE);
	return m_pcFatEntry->m_nSize;
}

//*****************************************************************************
int CWslSdFile::GetPosition()
{
	if (!m_pcFatEntry) throw new CWslWin32Exception(ERROR_INVALID_HANDLE);
	return m_nDecodedPos;
}

//*****************************************************************************
void CWslSdFile::__ReadRaw(void *pBuffer, int nSize)
{
	BYTE *pbyBuffer = (BYTE*)pBuffer;
	int nToDo, x;

	_ASSERT(pbyBuffer && pbyBuffer);

	while (1)
	{
		x = m_pcFatEntry->FilePos2SdPos(m_nRawPos);
		if (x == -1) throw new CWslWin32Exception(ERROR_READ_FAULT, m_pcFatEntry->m_pszFileName);
		f.Seek(x + sizeof(SDFILE_HEADER) + m_nOffset);

		nToDo = m_pcFatEntry->GetMaxContinuosAccessSize(m_nRawPos);
		if (nToDo > nSize) nToDo = nSize;

		f.Read(pbyBuffer, nToDo);

		nSize -= nToDo;
		m_nRawPos += nToDo;
		pbyBuffer += nToDo;

		if (!nSize) return;
	}
}

//*****************************************************************************
void CWslSdFile::Read(void *pBuffer, int nSize)
{
	BYTE *pbyBuffer = (BYTE*)pBuffer;
	int nToDo, nSizeBak = nSize, x;

	// Init

	_ASSERT(pbyBuffer);
	_ASSERT(m_dwAccess & GENERIC_READ);
	if (!nSize) return;
	if (!pbyBuffer) throw new CWslWin32Exception(ERROR_INVALID_PARAMETER);
	if (!m_pcFatEntry) throw new CWslWin32Exception(ERROR_INVALID_HANDLE);

	// Read data

	if (m_pCoder)
	{
		m_nDecodedPos += nSize;
		while (nSize)
		{
			if (m_nBufferPos == m_Buffer.GetSize())
			{
				x = m_pcFatEntry->m_nSize - m_nRawPos;
				nToDo = x > SD_TMP_BUFFER_SIZE ? SD_TMP_BUFFER_SIZE : x;
				m_nBufferPos = 0;

				__ReadRaw(m_pbyBuffer, nToDo);
				m_pCoder->Decode(m_pbyBuffer, m_Buffer, nToDo);
			}

			x = m_Buffer.GetSize() - m_nBufferPos;
			nToDo = nSize < x ? nSize : x;
			nSize -= nToDo;
			
			memcpy(pbyBuffer, &m_Buffer[m_nBufferPos], nToDo);
			pbyBuffer += nToDo;
			m_nBufferPos += nToDo;
		}
	}
	else
	{ 
		m_nRawPos = m_nDecodedPos;
		__ReadRaw(pBuffer, nSize);
		m_nDecodedPos = m_nRawPos;
	}
}

//*****************************************************************************
void CWslSdFile::__WriteRaw(const void *pcData, int nSize)
{
	int nToDo, x;
	const BYTE *pcbyBuffer = (const BYTE*)pcData;
	
	_ASSERT(pcbyBuffer && m_pcFatEntry);

	// Alloc new space if needed
	
	if (nSize > m_pcFatEntry->m_nSize - m_nRawPos) m_Fat.SetEntrySize(m_pcFatEntry, m_nRawPos + nSize);
		
	// Write data
		
	while (1)
	{
		x = m_pcFatEntry->FilePos2SdPos(m_nRawPos);
		_ASSERT(x!=-1);
		f.Seek(x + sizeof(SDFILE_HEADER) + m_nOffset);
		
		nToDo = m_pcFatEntry->GetMaxContinuosAccessSize(m_nRawPos);
		if (nToDo > nSize) nToDo = nSize;
		
		f.Write(pcbyBuffer, nToDo);
		
		nSize -= nToDo;
		m_nRawPos += nToDo;
		pcbyBuffer += nToDo;
		
		if (!nSize) return;
	}
}

//*****************************************************************************
void CWslSdFile::Write(const void *pcData, int nSize)
{
	int nToDo, x;
	const BYTE *pcbyBuffer = (const BYTE*)pcData;

	// Init

	_ASSERT(pcbyBuffer);
	_ASSERT(m_dwAccess & GENERIC_WRITE);
	if (!nSize) return;
	if (!pcbyBuffer) throw new CWslWin32Exception(ERROR_INVALID_PARAMETER);
	if (!m_pcFatEntry) throw new CWslWin32Exception(ERROR_INVALID_HANDLE);

	// Write

	if (m_pCoder)
	{
		m_nDecodedPos += nSize;
		while (nSize)
		{
			x = SD_TMP_BUFFER_SIZE - m_nBufferPos;
			nToDo = nSize < x ? nSize : x;
			nSize -= nToDo;

			memcpy(m_pbyBuffer + m_nBufferPos, pcbyBuffer, nToDo);

			pcbyBuffer += nToDo;
			m_nBufferPos += nToDo;

			if (m_nBufferPos == SD_TMP_BUFFER_SIZE) __WriteCodingBuffer();
		}
	}
	else
	{
		m_nRawPos = m_nDecodedPos;
		__WriteRaw(pcData, nSize);
		m_nDecodedPos = m_nRawPos;
		((CWslSdFatEntry*)m_pcFatEntry)->m_nDecodedSize = m_pcFatEntry->m_nSize;	//:TODO: better	
	}
}

//*****************************************************************************
void CWslSdFile::Write(const char *pcszText)
{
	Write(pcszText, strlen(pcszText));
}

//*****************************************************************************
void CWslSdFile::WriteLn(const char *pcszText /*= NULL*/)
{
	Write(pcszText, strlen(pcszText));
	WORD x = 0x0A0D;
	Write(&x, 2);
}

//*****************************************************************************
void CWslSdFile::Delete()
{
	if (!m_pcFatEntry) throw new CWslWin32Exception(ERROR_INVALID_HANDLE);
	
	m_Fat.RemoveEntry(m_pcFatEntry);
	m_pcFatEntry = NULL;
}

//*****************************************************************************
void CWslSdFile::Delete(const char *pcszName)
{
	if (m_pcFatEntry) throw new CWslWin32Exception(ERROR_TOO_MANY_OPEN_FILES, m_pcFatEntry->m_pszFileName);
	if (f.IsClosed()) throw new CWslWin32Exception(ERROR_INVALID_HANDLE);

	const CWslSdFatEntry *pcEntry = m_Fat.FindEntry(pcszName);
	if (!pcEntry) throw new CWslWin32Exception(ERROR_FILE_NOT_FOUND, pcszName);

	m_Fat.RemoveEntry(pcEntry);
}

//*****************************************************************************
bool CWslSdFile::Exists(const char *pcszName)
{
	if (m_pcFatEntry) throw new CWslWin32Exception(ERROR_TOO_MANY_OPEN_FILES, m_pcFatEntry->m_pszFileName);
	if (f.IsClosed()) throw new CWslWin32Exception(ERROR_INVALID_HANDLE);

	return m_Fat.FindEntry(pcszName) != NULL;
}

//*****************************************************************************
void CWslSdFile::Rename(const char *pcszSource, const char *pcszDest)
{
	if (m_pcFatEntry) throw new CWslWin32Exception(ERROR_TOO_MANY_OPEN_FILES, m_pcFatEntry->m_pszFileName);
	if (f.IsClosed()) throw new CWslWin32Exception(ERROR_INVALID_HANDLE);

	const CWslSdFatEntry *pcEntry = m_Fat.FindEntry(pcszSource);
	if (!pcEntry) throw new CWslWin32Exception(ERROR_FILE_NOT_FOUND, pcszSource);

	m_Fat.SetEntryName(pcEntry, pcszDest);
}

//*****************************************************************************
void CWslSdFile::__WriteCodingBuffer()
{
	_ASSERT(m_pCoder && m_pCoder->IsEncodeMode() && m_nBufferPos);

	m_pCoder->Encode(m_pbyBuffer, m_Buffer, m_nBufferPos);
	__WriteRaw(m_Buffer.GetData(), m_Buffer.GetSize());
	((CWslSdFatEntry*)m_pcFatEntry)->m_nDecodedSize += m_nBufferPos; //:TODO: better
	m_nBufferPos = 0;
}

//*****************************************************************************
void CWslSdFile::Flush()
{
	SDFILE_HEADER Hdr;
	SDFILE_FAT *pFat;

	// Build header & FAT

	ZeroMemory(&Hdr, sizeof(SDFILE_HEADER));

	Hdr.chFileId[0] = 'S';
	Hdr.chFileId[1] = 'D';
	Hdr.nVersion = SD_VERSION;
	Hdr.nFatPos = m_Fat.GetDataSize() + sizeof(SDFILE_HEADER);

	pFat = m_Fat.BuildFileFatBlock(&Hdr.nFatSize);
	_ASSERT(pFat);

	// Write FAT

	try
	{
		f.Seek(m_nOffset);
		f.Write(&Hdr, sizeof(SDFILE_HEADER));

		f.Seek(Hdr.nFatPos + m_nOffset);
		f.Write(pFat, Hdr.nFatSize);
		f.SetEndOfFile();
		f.Flush();
	}
	catch (...) { delete pFat; throw; }

	delete pFat;
}

