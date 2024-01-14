///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslGenericFile
// | P |  /  |  |  ~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1999-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 10
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// Abstract class for all file-like classes

#pragma once

#include "..\generic\WslObject.h"
#include "..\generic\WslWin32Exception.h"

//******************************************************************************
class WSL_API CWslGenericFile : public CWslObject
{
public:
   // Callback from the copy routine
   // nLength = total bytes to copy
   // nCopiedLength = number if just copied bytes
   typedef void COPY_CALLBACK(int nLength, int nCopiedLength, void* pData);
   
public:
   //---------------------------------------------------------------------------
   // Overridables
   //---------------------------------------------------------------------------
   
   // Returns the name/path of the file.
   virtual CWslString GetName() const { return CWslString(); }
   virtual CWslString GetPath() const { return CWslString(); }
   
   // Sets the current position as a new end of the file
   virtual void SetEndOfFile() throw (CWslException*) = 0;
   
   // Closes the file
   virtual void Close() throw (CWslException*) = 0;
   
   // Tests if the file is closed
   virtual BOOL IsClosed() = 0;
   
   // Seek functions. Seek mode can be FILE_BEGIN, FILE_END, FILE_CURRENT.
   virtual void Seek(int nPos, int nMode = FILE_BEGIN) throw (CWslException*) = 0;
   
   // Returns the length if the file
   virtual int GetLength() throw (CWslException*) = 0;
   
   // Returns the current posiotion
   virtual int GetPosition() throw (CWslException*) = 0;
   
   // Reading file data. It returns the number of bytes read.
   virtual int Read(void* pBuffer, int nSize) throw (CWslException*) = 0;
   
   // Writing data to the file. It returns the number of bytes written.
   virtual int Write(const void* pcData, int nSize) throw (CWslException*) = 0;
   
   // Writes any cached data to the disk
   virtual void Flush() throw (CWslException*) = 0;
   
   //---------------------------------------------------------------------------
   // Helper functions common to all implementations
   //---------------------------------------------------------------------------
   
   // Calls Close() but catches all exceptions
   void CloseNoException();
   
   //
   CWslString GetPathName() const { return GetPath()+'\\'+GetName(); }
   
   // Returns TRUE, if we are at the end of the file.
   bool IsEOF() throw (CWslException*) { return GetPosition() >= GetLength(); }
   
   // Seeks to the file start ot end
   inline void SeekToBegin() throw (CWslException*) { Seek(0, FILE_BEGIN); }
   inline void SeekToEnd() throw (CWslException*) { Seek(0, FILE_END); }
   
   // Writes text and formated text to the file.
   // The length of the resulting string is truncated to
   // 1024 chars including the terminating character.
   void Write(const char* pcszText);
   void WriteLn(const char* pcszText = NULL);
   void WriteUnixLn(const char* pcszText = NULL);
   void Format(const char* pcszFormat, ...);
   void FormatLn(const char* pcszFormat, ...);
   
   // Reads a line of text. Detects both EOLs - Wins (13,10) and Unix (10).
   void ReadLn(CWslString& sBuffer);
   
   // Easy data writing using operator <<
   inline CWslGenericFile& operator << (int n) throw (CWslException*) { Write(&n, 4); return *this; }
   inline CWslGenericFile& operator << (DWORD dw) throw (CWslException*) { Write(&dw, 4); return *this; }
   inline CWslGenericFile& operator << (WORD w) throw (CWslException*) { Write(&w, 2); return *this; }
   inline CWslGenericFile& operator << (BYTE by) throw (CWslException*) { Write(&by, 1); return *this; }
   inline CWslGenericFile& operator << (char ch) throw (CWslException*) { Write(&ch, 1); return *this; }
   inline CWslGenericFile& operator << (float f) throw (CWslException*) { Write(&f, 4); return *this; }
   inline CWslGenericFile& operator << (double dbl) throw (CWslException*) { Write(&dbl, 8); return *this; }
   inline CWslGenericFile& operator << (const char* pcsz) throw (CWslException*) { Write(pcsz, strlen(pcsz)); return *this; }
   inline CWslGenericFile& operator << (const CWslString& s) throw (CWslException*) { Write((const char*) s, s.GetLength()+1); return *this; }
   
   // Copies the file to the given target file
   void Copy(CWslGenericFile& TargetFile, 
      int nLengthToCopy = -1, 
      BOOL* pbCancel = NULL,
      COPY_CALLBACK* pfnCallback = NULL,
      void* pData = NULL)
      throw (CWslException*);
};


