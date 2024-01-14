///////////////////////////////////////////////////////////////////////////////
//
// CWslSdFat
// ~~~~~~~~~
// Class for managing FAT in SD files.
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
#include <WslTextStuff.h>

//*****************************************************************************
//*****************************************************************************
//** CWslSdFatEntry ***********************************************************
//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
CWslSdFatEntry::CWslSdFatEntry(const SDFILE_FAT_ENTRY *pcEntry)
{
	// Init data

	int i = strlen(pcEntry->pszFileName + (DWORD) pcEntry);
	_ASSERT(i > 0);
	m_pszFileName = new char[i+1];
	_ASSERT(m_pszFileName);
	strcpy(m_pszFileName, pcEntry->pszFileName + (DWORD) pcEntry);

	m_nSize = pcEntry->nSize;
	m_nDecodedSize = pcEntry->nDecodedSize;
	m_dwProperties = pcEntry->dwProperties;
	m_pNext = NULL;

	// Init chunk list

	m_ChunkArray.SetSize(pcEntry->nNumUsedChunks);
	m_ChunkArray.SetItems(0, m_ChunkArray.GetSize(), (int*)((DWORD)pcEntry->pnChunkList + (DWORD) pcEntry));

	// Init connected chunks list

	m_ConnectedChunksArray.SetSize(pcEntry->nNumUsedChunks);
	UpdateConnectedChunksArray();
}

//*****************************************************************************
CWslSdFatEntry::CWslSdFatEntry(const char *pcszName, DWORD dwProperties)
{
	// Init data

	int i = strlen(pcszName);
	_ASSERT(i > 0);
	m_pszFileName = new char[i+1];
	_ASSERT(m_pszFileName);
	strcpy(m_pszFileName, pcszName);

	m_nSize = 0;
	m_nDecodedSize = 0;
	m_dwProperties = dwProperties;
	m_pNext = NULL;
}

//*****************************************************************************
CWslSdFatEntry::~CWslSdFatEntry()
{
	delete m_pszFileName;
}


//*****************************************************************************
int CWslSdFatEntry::AddToFileFat(SDFILE_FAT_ENTRY *pEntry, BYTE *pbyData)
{
	int nNameLen = strlen(m_pszFileName) + 1;
	int nNumUsedChunks = m_ChunkArray.GetSize();
	
	// Init structure members

	pEntry->nSize = m_nSize;
	pEntry->nDecodedSize = m_nDecodedSize;
	pEntry->dwProperties = m_dwProperties;
	pEntry->nNumUsedChunks = nNumUsedChunks;
	pEntry->pszFileName = (char*) (pbyData - (BYTE*)pEntry);
	pEntry->pnChunkList = (int*) (pbyData - (BYTE*)pEntry + nNameLen);

	// Copy data to the data part of the FAT

	strcpy((char*)pbyData, m_pszFileName);
	if (nNumUsedChunks) memcpy(pbyData + nNameLen, m_ChunkArray.GetData(), nNumUsedChunks * sizeof(int));

	// Return size of just added data

	return nNameLen + sizeof(int) * nNumUsedChunks;
}

//*****************************************************************************
int CWslSdFatEntry::FilePos2SdPos(int nPos) const
{
	int x, i = nPos >> SD_CHUNK_SIZE_SHIFT;

	if (i < m_ChunkArray.GetSize())
		x = m_ChunkArray[i] * SD_CHUNK_SIZE + (nPos & SD_POS_IN_CHUNK_MASK); 
	else
		x = -1;

	return x;
}

//*****************************************************************************
int CWslSdFatEntry::GetMaxContinuosAccessSize(int nPos) const
{
	int x, i = nPos >> SD_CHUNK_SIZE_SHIFT;

	if (i < m_ConnectedChunksArray.GetSize())
		x = m_ConnectedChunksArray[i] * SD_CHUNK_SIZE - (nPos & SD_POS_IN_CHUNK_MASK); 
	else
		x = -1;

	return x;
}

//*****************************************************************************
void CWslSdFatEntry::UpdateConnectedChunksArray()
{
	_ASSERT(m_ChunkArray.GetSize() == m_ConnectedChunksArray.GetSize());

	int i = m_ChunkArray.GetSize() - 1;
	if (i < 0) return;

	m_ConnectedChunksArray[i] = 1;

	while (i--)
	{
		if (m_ChunkArray[i] + 1 == m_ChunkArray[i+1])
			m_ConnectedChunksArray[i] = m_ConnectedChunksArray[i+1] + 1;
		else
			m_ConnectedChunksArray[i] = 1;
	}
}

//*****************************************************************************
//*****************************************************************************
//** CWslSdFat - FAT Manager **************************************************
//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
CWslSdFat::CWslSdFat()
{
	m_pFirstEntry = NULL;
}

//*****************************************************************************
CWslSdFat::~CWslSdFat()
{
	Deinit();
}

//*****************************************************************************
void CWslSdFat::Init(const SDFILE_FAT *pcFat)
{
	// Init FAT

	m_pFirstEntry = NULL;
	m_nFileNamesSize = 0;
	m_nNumFiles = 0;
	m_nNumUsedChunks = 0;

	if (!pcFat) return;

	// Build FAT

	m_nNumUsedChunks = pcFat->nNumUsedChunks;

	SDFILE_FAT_ENTRY *pFatEntry = (SDFILE_FAT_ENTRY*) pcFat->Entry;

	for (int i = pcFat->nNumFiles; i--; pFatEntry++) AddEntry(pFatEntry);
}

//*****************************************************************************
void CWslSdFat::Deinit()
{
	CWslSdFatEntry *x, *p = m_pFirstEntry;
 
	while (p)
	{
		x = p->m_pNext;
		delete p;
		p = x;
	}

	m_pFirstEntry = NULL;
}

//*****************************************************************************
SDFILE_FAT *CWslSdFat::BuildFileFatBlock(int *pnFatSize)
{
	// Alloc memory
	// Here the FAT size includes even those chunks that are not used (=includes gaps)

	int nFatSize = sizeof(SDFILE_FAT) +
		m_nNumFiles * sizeof(SDFILE_FAT_ENTRY) + 
		m_nNumUsedChunks * sizeof(int) + 
		m_nFileNamesSize;

 	SDFILE_FAT *pFat = (SDFILE_FAT*) new BYTE[nFatSize];

	// Init header

	pFat->nNumFiles = m_nNumFiles;
	pFat->nNumUsedChunks = m_nNumUsedChunks;

	// Build entries & data

	BYTE *pbyData = (BYTE*)(&pFat->Entry[m_nNumFiles]);
	int iEntry = 0;
	CWslSdFatEntry *pEntry = m_pFirstEntry;

	while (pEntry)
	{
		pbyData += pEntry->AddToFileFat(&pFat->Entry[iEntry++], pbyData);
		_ASSERT((pbyData - (BYTE*)pFat) <= nFatSize);
		pEntry = pEntry->m_pNext;
	}

	// Update the FAT size according the number of really used chunks (=without gaps)

	*pnFatSize = pbyData - (BYTE*)pFat;

	return pFat;
}

//*****************************************************************************
void CWslSdFat::AddEntry(const SDFILE_FAT_ENTRY *pcEntry)
{
	// Add item to the list

	CWslSdFatEntry *p = new CWslSdFatEntry(pcEntry);
	_ASSERT(p);

	if (m_pFirstEntry) m_pLastEntry->m_pNext = p; else m_pFirstEntry = p;
	m_pLastEntry = p;

	// Update stuff

	m_nNumFiles++;
	m_nFileNamesSize += strlen(p->m_pszFileName) + 1;

	return;
}

//*****************************************************************************
const CWslSdFatEntry *CWslSdFat::AddEntry(const char *pcszName, DWORD dwProperties)
{
	// Add item to the list

	CWslSdFatEntry *p = new CWslSdFatEntry(pcszName, dwProperties);
	_ASSERT(p);

	if (m_pFirstEntry) m_pLastEntry->m_pNext = p; else m_pFirstEntry = p;
	m_pLastEntry = p;

	// Update stuff

	m_nNumFiles++;
	m_nFileNamesSize += strlen(p->m_pszFileName) + 1;
	
	return p;
}

//*****************************************************************************
void CWslSdFat::RemoveEntry(const CWslSdFatEntry *pcFatEntry)
{
	CWslSdFatEntry *pEntry = (CWslSdFatEntry*) pcFatEntry;
	CWslSdFatEntry *pPrev = NULL;
	CWslSdFatEntry *x = m_pFirstEntry;

	_ASSERT(pEntry);

	while (x)
	{
		if (x==pEntry)
		{
			if (pPrev) pPrev->m_pNext = x->m_pNext; else m_pFirstEntry = x->m_pNext;
			if (x==m_pLastEntry) m_pLastEntry = pPrev;

			m_nNumFiles--;
			m_nFileNamesSize -= strlen(x->m_pszFileName) + 1;

			delete x;
			return;
		}
		pPrev = x;
		x = x->m_pNext;
	}

	// Entry was not found -> BUG!

	_ASSERT(false);
}

//*****************************************************************************
const CWslSdFatEntry *CWslSdFat::FindEntry(const char *pcszName) const
{
	CWslSdFatEntry *p = m_pFirstEntry;

	while (p)
	{
		if (!WslStricmp(p->m_pszFileName, pcszName)) break;
		p = p->m_pNext;
	}

	return p;
}

//*****************************************************************************
void CWslSdFat::SetEntrySize(const CWslSdFatEntry *pcFatEntry, int nSize)
{
	int nNumChunksToAdd, nOldNumChunks;
	CWslSdFatEntry *pEntry = (CWslSdFatEntry*) pcFatEntry;

	_ASSERT(nSize>=0 && pEntry);

	// Init

	pEntry->m_nSize = nSize;
	nOldNumChunks = pEntry->m_ChunkArray.GetSize();

	// Count how much new chunks we need to add or remove

	nNumChunksToAdd = ((nSize + SD_CHUNK_SIZE - 1) >> SD_CHUNK_SIZE_SHIFT) - nOldNumChunks;

	// ---- Set zero size -------------------------------------------------------

	if (nNumChunksToAdd==0)
	{
		pEntry->m_ChunkArray.RemoveAll();
	}

	// ---- Set smaller size -> remove chunks -----------------------------------

	else if (nNumChunksToAdd < 0)

	{
		pEntry->m_ChunkArray.ShrinkBy(- nNumChunksToAdd);
		pEntry->m_ConnectedChunksArray.ShrinkBy(- nNumChunksToAdd);
	}

	// ---- Set greater size -> add chunks --------------------------------------

	else

	{
		// Grow the chunk array

		pEntry->m_ChunkArray.GrowBy(nNumChunksToAdd);
		pEntry->m_ConnectedChunksArray.GrowBy(nNumChunksToAdd);

		// Try to reuse once used chunks

		// :TODO:

		// If we still need some, allocate new chunks

		while (nNumChunksToAdd--)
		{
			pEntry->m_ChunkArray[nOldNumChunks] = (WORD)m_nNumUsedChunks;
			nOldNumChunks++;
			m_nNumUsedChunks++;
		}
	
	}

	// Recalculate connected chunks

	pEntry->UpdateConnectedChunksArray();
}

//*****************************************************************************
void CWslSdFat::SetEntryName(const CWslSdFatEntry *pcFatEntry, const char *pcszName)
{
	_ASSERT(pcszName && pcFatEntry);
	CWslSdFatEntry *pEntry = (CWslSdFatEntry*) pcFatEntry;

	m_nFileNamesSize -= strlen(pEntry->m_pszFileName);
	m_nFileNamesSize += strlen(pcszName);

	strcpy(pEntry->m_pszFileName, pcszName);
}

//*****************************************************************************
void CWslSdFat::FatEntry2Win32FindData(const CWslSdFatEntry *pcFatEntry, WIN32_FIND_DATA *p)
{
	if (pcFatEntry->m_dwProperties & SDFILE_PROP_COMPRESSED) p->dwFileAttributes |= FILE_ATTRIBUTE_COMPRESSED;
	if (pcFatEntry->m_dwProperties & SDFILE_PROP_ENCRYPTED) p->dwFileAttributes |= FILE_ATTRIBUTE_ENCRYPTED;
	strcpy(p->cFileName, pcFatEntry->m_pszFileName);
	p->nFileSizeHigh = 0;
	p->nFileSizeLow = pcFatEntry->m_nDecodedSize;
}








