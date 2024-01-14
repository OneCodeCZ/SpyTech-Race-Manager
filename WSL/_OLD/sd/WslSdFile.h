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

#pragma once

// Forward declarations of classes

class CWslSdFatEntry;
class CWslSdFat;
class CWslSdFile;
class CWslSdFileSearch;
	
// Various stuff

#define SD_VERSION									500											// xyz = x.yz * 100
#define SD_CHUNK_SIZE_SHIFT					12											// 2^12 = 4Kb
#define SD_CHUNK_SIZE								(1 << SD_CHUNK_SIZE_SHIFT)
#define SD_POS_IN_CHUNK_MASK				(SD_CHUNK_SIZE - 1)			// e.g. 00000011111111
#define SD_TMP_BUFFER_SIZE					(32*1024)	

// SD_HEADER.dwProperties

// SD_FAT_ENTRY.dwProperties

#define	SDFILE_PROP_COMPRESSED				1			// Files is compressed -> when opened, file is read-only
#define SDFILE_PROP_ENCRYPTED					2			// Files is encrypted

// Structures - disable warnings

#pragma warning(push, 1)

//******************************************************************************
struct SDFILE_HEADER
{
	char	chFileId[2];			// Text "SD"
	int 	nVersion;					// SD_VERSION value
	int		nFatPos;					// Offset of the FAT block
	int		nFatSize;					// Size of the FAT block
};

//******************************************************************************
struct SDFILE_FAT_ENTRY
{
	char *pszFileName;						// offset from the beginnig if this entry
	int		nSize;									// Real size of the file (=encoded size)
	int   nDecodedSize;						// Size after decoding
	DWORD dwProperties;
	int   nNumUsedChunks;
	int  *pnChunkList;						// offset from the beginnig if this entry
};

//******************************************************************************
struct SDFILE_FAT
{
	int	nNumFiles;							// Number of files in SD file = num FAT records
	int nNumUsedChunks;					// Number of used chunks
	SDFILE_FAT_ENTRY Entry[];		
};

#pragma warning(pop)


//******************************************************************************
class CWslSdFatEntry
{
friend class CWslSdFat;

public:
	// Only these members can be directly accessed from CWslSdFile methods
	char *m_pszFileName;	
	int   m_nSize;														// Real size of the file (=encoded size)
	int   m_nDecodedSize;											// Size after decoding
	DWORD m_dwProperties;

private:
	CWslArray<int> m_ChunkArray;							// List of chunks.	
	CWslArray<int> m_ConnectedChunksArray;		// List of counts of connected chunks.
	CWslSdFatEntry *m_pNext;
	
public:
	CWslSdFatEntry(const SDFILE_FAT_ENTRY *pcEntry);
	CWslSdFatEntry(const char *pcszName, DWORD dwProperties);
	~CWslSdFatEntry();

	// Adds class data to the file FAT 
	// RETURNS size of data added to the FAT at *pbyData position

	int AddToFileFat(SDFILE_FAT_ENTRY *pEntry, BYTE *pbyData);

	// Returns position in SD for a given file position.
	// Returns -1 when the given position is not valid (behind/before the file)

	int FilePos2SdPos(int nPos) const;

	// Returns the size of a continuos chunk that can be accessed in one IO oeration.
	// Returns -1 when the given position is not valid (behind/before the file)

	int GetMaxContinuosAccessSize(int nPos) const;

	// Updates the m_ConnectedChunksArray

	void UpdateConnectedChunksArray();
};


//******************************************************************************
class CWslSdFat
{
//private:
public:
	CWslSdFatEntry *m_pFirstEntry;
	CWslSdFatEntry *m_pLastEntry;
	int m_nFileNamesSize;              // Size of all file names including #0 chars.
	int m_nNumUsedChunks;
	int m_nNumFiles;
	const CWslSdFatEntry *m_hFile;

public:
	CWslSdFat();
	~CWslSdFat();

	// Init FAT in memory. 
	// If the pcFat==NULL then an empty FAT is created.

	void Init(const SDFILE_FAT *pcFat);

	// Frees all resource 

	void Deinit();

	// Creates FAT block suitable for storing on disk.
	// The returned pointer should be deleted after the usage.

	SDFILE_FAT *BuildFileFatBlock(int *pnFatSize);

	// Adds new entry to the FAT

	void AddEntry(const SDFILE_FAT_ENTRY *pcEntry);
	const CWslSdFatEntry *AddEntry(const char *pcszName, DWORD dwProperties);

	// Returns entry of the FAT of a given file 

	const CWslSdFatEntry *FindEntry(const char *pcszName) const;

	// Sets new size of an FAT entry

	void SetEntrySize(const CWslSdFatEntry *pcFatEntry, int nSize);

	// Removes an entry from the FAT

	void RemoveEntry(const CWslSdFatEntry *pcFatEntry);

	// Returns size of the filedata

	int GetDataSize() const { return m_nNumUsedChunks * SD_CHUNK_SIZE; }

	// Sets new file name for the given FAT entry.

	void SetEntryName(const CWslSdFatEntry *pcFatEntry, const char *pcszName);

	// Returns first fat entry

	inline const CWslSdFatEntry *FindFirst() const { return m_pFirstEntry; };

	// Returns next fat entry

	inline const CWslSdFatEntry *FindNext(const CWslSdFatEntry *pcFatEntry) const { return pcFatEntry->m_pNext; };

	// Converts FAT entry info to the WIN32_FIND_DATA structure

	void FatEntry2Win32FindData(const CWslSdFatEntry *pcFatEntry, WIN32_FIND_DATA *p);
};

//******************************************************************************
class CWslSdGenericCoder
{
protected:
	bool m_bEncode;								// true=encoding mode, false=decoding mode

public:
	virtual void Start(BOOL bEncode) = 0;
	virtual void Finish(CWslArray<BYTE> &Buffer) = 0;
	virtual void Encode(const void *pcData, CWslArray<BYTE> &Buffer, int nDataSize) = 0;
	virtual void Decode(const void *pcData, CWslArray<BYTE> &Buffer, int nDataSize) = 0;

	inline bool IsEncodeMode() const { return m_bEncode; }
};

//******************************************************************************
class CWslSdLZ78Coder : public CWslSdGenericCoder
{
private:

public:
	virtual void Start(BOOL bEncode) {};
	virtual void Finish(CWslArray<BYTE> &Buffer) {};
	virtual void Encode(const void *pcData, CWslArray<BYTE> &Buffer, int nDataSize) {};
	virtual void Decode(const void *pcData, CWslArray<BYTE> &Buffer, int nDataSize) {};
};

//******************************************************************************
class CWslSdFile : public CWslGenericFile
{
friend class CWslSdFileSearch;

private:
  CWslFile f;
  char m_szSdName[MAX_PATH + 1];
	int m_nDecodedPos;				  					// Position in the opened file (in decoded data)
	int m_nRawPos;												// Raw position in the file (in encoded data)
	const CWslSdFatEntry *m_pcFatEntry;		// FAT entry of the opened file
	CWslSdFat m_Fat;											// FAT manager
	int m_nOffset;												// Offset of the SD header from the file begin
	DWORD m_dwAccess;											// Access to the SD (GENERIC_READ, GENERIC_WRITE)
	CWslSdGenericCoder *m_pCoder;					// Encode/decode class
	CWslArray<BYTE> m_Buffer;							// Resizable buffer
	BYTE *m_pbyBuffer;										// Fixed size buffer
	int m_nBufferPos;											// Position in m_Buffer or m_pbyBuffer
	bool m_bSeekable;											// Seek enabled/disabled

public:
	CWslSdFile();
	~CWslSdFile();

	// ------ SD file functions ------------------------------------------

	// Opens SD file
	
  void OpenSD(const char *pcszName, 
		DWORD dwAccess = GENERIC_READ | GENERIC_WRITE,
		int nOffset = 0);

	// Creates new SD file

	void CreateSD(const char *pcszName, int nOffset = 0);

	// Close SD file

	void CloseSD();

	// Returns total size of all files in SD

	int GetTotalLength();

	// ------ VIRTUAL FUNCTIONS FROM CWslGenericFile --------------------

	// Open or create file
	// dwAttributes supports FILE_ATTRIBUTE_COMPRESSED and FILE_ATTRIBUTE_ENCRYPTED

  virtual void Open(const char *pcszName, 
		DWORD dwAttributes = FILE_ATTRIBUTE_NORMAL,
    DWORD dwCreateMode = OPEN_EXISTING, 
    DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE,
    DWORD dwAccessMode = GENERIC_READ | GENERIC_WRITE);

	// Sets new end of the file

  virtual void SetEndOfFile();

	// Closes file

  virtual void Close(bool bThrowException = true);

	// Tests if file is closed

	virtual bool IsClosed();

	// Seek & position functions

  virtual void SeekToBegin();
  virtual void SeekToEnd();
  virtual void Seek(int nPos, int nMode = FILE_BEGIN);
  virtual int GetLength();
  virtual int GetPosition();
	
	// Reading file

  virtual void Read(void *pBuffer, int nSize);

	// Writing file

  virtual void Write(const void *pcData, int nSize);
  virtual void Write(const char *pcszText);
  virtual void WriteLn(const char *pcszText = NULL);

	// File system functions

  virtual void Delete();
  virtual void Delete(const char *pcszName);
  virtual bool Exists(const char *pcszName);
	virtual void Rename(const char *pcszSource, const char *pcszDest);

	// Writes any cached data to the disk

  virtual void Flush();

private:

	// Raw access to the file

	void __ReadRaw(void *pBuffer, int nSize);
	void __WriteRaw(const void *pcData, int nSize);

	// Writes any data buffered for encode

	void __WriteCodingBuffer();
	
public:
};

