///////////////////////////////////////////////////////////////////////////////
//
// CWslFileMapping
// ~~~~~~~~~~~~~~~
// (C)1998-2004 Radek Tetik/WSS/SpyTech
// Last updated: 2004 11
//
// Maps a file into the address space. The file can be accessed via pointer.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

/*****************************************************************************/
class WSL_API CWslFileMappingView
{
protected:
   void* m_pView;
public:
   CWslFileMappingView() { m_pView = NULL; }
   CWslFileMappingView(void* pView);
   ~CWslFileMappingView();

   void* GetPtr() const { return m_pView; }
   void Unmap();
};

typedef CWslPtr<CWslFileMappingView> CWslFileMappingViewPtr;

//*****************************************************************************
class WSL_API CWslFileMapping  
{
protected:
   HANDLE	m_hMapping;
   HANDLE	m_hFile;
   DWORD		m_dwMappingAccess;
   BOOL     m_bMyHandle;			// true if the file handle was created by this object
   
public:
   CWslFileMapping();
   ~CWslFileMapping();
   
   // Select a file for mapping	
   void MapFile(const char* pFileName, 
      DWORD dwFileAccess = GENERIC_READ|GENERIC_WRITE,		
      DWORD dwFileShareMode = FILE_SHARE_READ|FILE_SHARE_WRITE,
      int	nSize = 0,														
      const char* pcszMappingName = NULL) throw (CWslException*);
   
   void MapFile(HANDLE hFile,	
      DWORD dwFileAccess = GENERIC_READ|GENERIC_WRITE,
      int	nSize = 0,														
      const char *pcszMappingName = NULL) throw (CWslException*);
   
   // Map certain part of the file.
   // If size = 0 then whole file is mapped
   CWslFileMappingViewPtr MapView(int nOffset = 0, int nSize = 0) throw (CWslException*);
   
   // Unmap stuff
   void UnmapFile() throw (CWslException*);
   
   // Writes any changes to the file
   void Flush(void* pAddr, int nNumBytesToFlush=0) throw (CWslException*);
};



