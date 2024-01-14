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

#include "stdafx.h"
#include "../generic\WslWin32Exception.h"
#include "WslFileMapping.h"

//**********************************************************************************
CWslFileMapping::CWslFileMapping()
{
   m_hMapping = INVALID_HANDLE_VALUE;
   m_hFile = INVALID_HANDLE_VALUE;
}

//**********************************************************************************
CWslFileMapping::~CWslFileMapping()
{
   try 
   { 
      UnmapFile();
   }
   catch (CWslWin32Exception* pErr)
   {
      ASSERT(false);
      pErr->Delete();
   }
} 

//**********************************************************************************
void CWslFileMapping::MapFile(const char *pcszFileName, 
                              DWORD dwFileAccess /*= GENERIC_READ|GENERIC_WRITE*/,
                              DWORD dwFileShareMode /*= FILE_SHARE_READ|FILE_SHARE_WRITE*/, 
                              int nSize /*= 0*/,          
                              const char *pcszMappingName /*= NULL*/)
{
   HANDLE hFile;
   m_hMapping = INVALID_HANDLE_VALUE;
   
   // Open the file
   
   hFile = CreateFile(pcszFileName, 
      dwFileAccess, 
      dwFileShareMode, 
      NULL,            //default secutiry desc
      OPEN_EXISTING, 
      FILE_ATTRIBUTE_NORMAL, 
      0);
   if (hFile==INVALID_HANDLE_VALUE) throw new CWslWin32Exception();
   
   // Create mapping
   
   MapFile(hFile, dwFileAccess, nSize, pcszMappingName);
   m_bMyHandle = true;
}

//**********************************************************************************
void CWslFileMapping::MapFile(HANDLE hFile, 
                              DWORD dwFileAccess /*= GENERIC_READ|GENERIC_WRITE*/,
                              int	nSize /*= 0*/,											
                              const char *pcszMappingName /*= NULL*/)
{	
   ASSERT(hFile);
   m_hFile = hFile;
   m_bMyHandle = false;
   
   // Don't allow to map files with 0 length (but don't throw any exception)
   
   if (GetFileSize(m_hFile,NULL)==0) return;
   
   // Create mapping
   
   m_dwMappingAccess = dwFileAccess & GENERIC_WRITE ? PAGE_READWRITE : PAGE_READONLY;
   
   m_hMapping = CreateFileMapping(m_hFile, 
      NULL,   //default secutiry desc
      m_dwMappingAccess,
      0,      //hiSize        
      nSize,
      NULL);  //noname object
   if (!m_hMapping) throw new CWslWin32Exception();
}

//**********************************************************************************
CWslFileMappingViewPtr CWslFileMapping::MapView(int nOffset/*=0*/, int nSize/*=0*/)
{
   // Quit without exception in case that mapped file has size=0, m_hMapping=INVALIDE...
   
   if (m_hMapping==INVALID_HANDLE_VALUE) return NULL;
   
   // Map new view
   
   void* p = MapViewOfFile(m_hMapping,
      m_dwMappingAccess==PAGE_READWRITE ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ,
      0,
      nOffset,
      nSize);
   if (!p) throw new CWslWin32Exception();
   
   return new CWslFileMappingView(p);
}

//**********************************************************************************
void CWslFileMapping::UnmapFile()
{
   if (m_hMapping != INVALID_HANDLE_VALUE)
   {
      if (!CloseHandle(m_hMapping)) throw new CWslWin32Exception();
      m_hMapping = INVALID_HANDLE_VALUE;
   }
   
   if (m_bMyHandle && m_hFile != INVALID_HANDLE_VALUE)
   {
      if (!CloseHandle(m_hFile)) throw new CWslWin32Exception();
      m_hFile = INVALID_HANDLE_VALUE;
   }
}

//**********************************************************************************
//saves changed bytes to the file
void CWslFileMapping::Flush(void *pAddr, int nNumBytesToFlush /*= 0*/)
{
   ASSERT(pAddr);
   if (pAddr && !FlushViewOfFile(pAddr, nNumBytesToFlush)) throw new CWslWin32Exception();
}

/*****************************************************************************
*
* VIEW
*
*****************************************************************************/

/*****************************************************************************/
CWslFileMappingView::CWslFileMappingView(void* pView)
{
   m_pView = pView;
}

/*****************************************************************************/
CWslFileMappingView::~CWslFileMappingView()
{
   Unmap();
}

/*****************************************************************************/
void CWslFileMappingView::Unmap()
{
   if (m_pView)
   {
      VERIFY(UnmapViewOfFile(m_pView));
      m_pView = NULL;
   }
}

