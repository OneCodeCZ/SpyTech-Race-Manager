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

#include "stdafx.h"
#include "WslHttpSession.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*****************************************************************************/
CWslHttpFile::CWslHttpFile(CWslHttpSession& Session, CWslHttpConnection& Connection)
: m_Session(Session), m_Connection(Connection)
{
   m_Connection.IsOpened();
   m_nPos = 0;
   m_nLength = HTTP_UNKNOWN_LENGTH; 
}

/*****************************************************************************/
CWslHttpFile::~CWslHttpFile()
{
   Close();
}

/*****************************************************************************/
void CWslHttpFile::Close() throw (CWslException*)
{
}

/*****************************************************************************/
int CWslHttpFile::Read(void* pBuffer, int nSize) throw (CWslException*)
{
   ASSERT(!IsClosed());
   DWORD dwRead;

   if (!WinHttpReadData(m_Connection.GetRequestHandle(), pBuffer, nSize, &dwRead))
      throw new CWslWin32Exception();

   if (dwRead == 0)
   {
      // EOF reached
      m_nLength = m_nPos;
   }
   else
   {
      m_nPos += (int) dwRead;
   }

   return (int) dwRead;
}

/*****************************************************************************/
int CWslHttpFile::GetLength() throw (CWslException*)
{
   if (m_nLength != -1) return m_nLength;

   ASSERT(!IsClosed());

   DWORD dwSize = sizeof(dwSize);
   DWORD dwSizeSize = sizeof(dwSizeSize);

   BOOL b = WinHttpQueryHeaders(m_Connection.GetRequestHandle(), 
      WINHTTP_QUERY_CONTENT_LENGTH,
      NULL,
      &dwSize,
      &dwSizeSize,
      WINHTTP_NO_HEADER_INDEX);

   if (!b)
   {
      if (GetLastError() == ERROR_WINHTTP_HEADER_NOT_FOUND)
      {
         m_nLength = HTTP_UNKNOWN_LENGTH;
         ASSERT(m_nLength = HTTP_UNKNOWN_LENGTH);
      }
      else throw new CWslWin32Exception();
   }
   else
   {
      m_nLength = (int) dwSize;
   }

   return m_nLength;
}









