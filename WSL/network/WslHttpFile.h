///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - SpyTech C++ Library
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2005 05
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "..\files\WslGenericFile.h"

/*****************************************************************************/
class WSL_API CWslHttpFile : public CWslGenericFile
{
public:
   enum { HTTP_UNKNOWN_LENGTH = 2^31-1 };

protected:
   CWslHttpConnection&  m_Connection;
   CWslHttpSession&  m_Session;
   int               m_nPos;
   int               m_nLength;

public:
   CWslHttpFile(CWslHttpSession& Session, CWslHttpConnection& Connection);
   virtual ~CWslHttpFile();

   //---------------------------------------------------------------------------
   // FROM WSL
   //---------------------------------------------------------------------------

   virtual int Read(void* pBuffer, int nSize) throw (CWslException*);
   virtual void Close() throw (CWslException*);
   virtual BOOL IsClosed() { return !m_Connection.IsOpened(); }
   virtual int GetPosition() throw (CWslException*) { return m_nPos; }
   virtual int GetLength() throw (CWslException*);

   // UNSUPPORTED
   virtual void Flush() throw (CWslException*) { ASSERT(false); }
   virtual int Write(const void* pcData, int nSize) throw (CWslException*) { ASSERT(false); return 0; }
   virtual void SetEndOfFile() throw (CWslException*) { ASSERT(false); }
   virtual void Seek(int nPos, int nMode = FILE_BEGIN) throw (CWslException*) { ASSERT(false); }
};