///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslFile
// | P |  /  |  |  ~~~~~~~~
// | Y | /-  |  |  Copyright (C)1998-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "WslGenericFile.h"
#include "..\generic\WslString.h"

//******************************************************************************
class WSL_API CWslFile : public CWslGenericFile
{
public:
   HANDLE			m_hHandle;
   
protected:
   CWslString	m_sName;
   CWslString	m_sPath;
   
public:
   CWslFile() { m_hHandle = INVALID_HANDLE_VALUE; };
   ~CWslFile() { CloseNoException(); };
   
   //---------------------------------------------------------------------------
   // File-system specific API
   //---------------------------------------------------------------------------
   
   // Opens a file 
   void Open(const char* pcszName, 
      DWORD dwAttributes = FILE_ATTRIBUTE_NORMAL,
      DWORD dwCreateMode = OPEN_EXISTING, 
      DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE,
      DWORD dwAccessMode = GENERIC_READ | GENERIC_WRITE)
      throw (CWslWin32Exception*);
   
   // Opens a file as read-only and shared for reading
   inline void OpenReadOnly(const char* pcszName) throw (CWslWin32Exception*)
   { 
      Open(pcszName, FILE_ATTRIBUTE_NORMAL, OPEN_EXISTING, FILE_SHARE_READ, GENERIC_READ); 
   } 
   
   // Creates a new file
   inline void Create(const char* pcszName, 
      DWORD dwAttributes = FILE_ATTRIBUTE_NORMAL,
      DWORD dwCreateMode = CREATE_ALWAYS, 
      DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE)  throw (CWslWin32Exception*)
   {
      Open(pcszName, dwAttributes, dwCreateMode, dwShareMode, GENERIC_READ | GENERIC_WRITE);
   }
   
   // Creates a file in the TEMP folder. The file has the attribute
   // FILE_ATTRIBUTE_TEMPORARY set.
   // NOTE: The file should be deleted as soon as possible.
   void CreateTemporary(const char* pcszName) throw (CWslWin32Exception*);
   
   // Opens a file and sets the current position at the end of file
   inline void Append(const char* pcszName, 
      DWORD dwAttributes = FILE_ATTRIBUTE_NORMAL,
      DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE)
   {
      Open(pcszName, dwAttributes, OPEN_EXISTING, dwShareMode, GENERIC_READ | GENERIC_WRITE);
      SeekToEnd();
   }
   
   // Deletes the given file
   static void Delete(const char* pcszName) throw (CWslWin32Exception*);
   
   // Returns TRUE, if the given file exists
   static BOOL Exists(const char* pcszName);
   
   // Renames the given file
   static void Rename(const char* pcszSource, const char* pcszDest) throw (CWslWin32Exception*);
   
   // Copies the given file
   static void Copy(const char* pcszWhat, const char* pcszDest, BOOL bFailIfExists = false);
   
   // Returns length of the given file
   static int GetLength(const char* pcszFileName) throw (CWslWin32Exception*);
   
   // Linux-like touch
   void Touch() throw (CWslWin32Exception*);
   static void Touch(const char* pcszFile) throw (CWslWin32Exception*);
   
   // Converts file URL to an UNC path (file://c:/a/b -> c:\a\b)
   static CWslString UrlToPathName(const CWslString& str);


   //---------------------------------------------------------------------------
   // From parent, see CWslGenericFile for details
   //---------------------------------------------------------------------------
   
   virtual CWslString GetPath() const { return m_sPath; }
   virtual CWslString GetName() const { return m_sName; }
   virtual void SetEndOfFile() throw (CWslException*);
   virtual void Close()  throw (CWslException*);
   virtual BOOL IsClosed() { return m_hHandle == INVALID_HANDLE_VALUE; };
   virtual void Seek(int nPos, int nMode = FILE_BEGIN) throw (CWslException*);
   virtual int GetLength() throw (CWslException*);
   virtual int GetPosition() throw (CWslException*);
   virtual int Read(void* pBuffer, int nSize) throw (CWslException*);
   virtual int Write(const void* pcData, int nSize) throw (CWslException*);
   virtual void Flush() throw (CWslException*);
   
   // Stupid VC++ forgets methods from the class parent when there
   // are defined methods with the same name in the child but
   // different parameters
   inline void Write(const char* pcszText) throw (CWslException*) { CWslGenericFile::Write(pcszText); }
};
