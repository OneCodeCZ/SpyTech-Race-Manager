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

/*****************************************************************************/
class WSL_API CWslHttpConnection
{
protected:
   CWslHttpSession&  m_Session;
   HINTERNET         m_hConnection;
   HINTERNET         m_hRequest;
   BOOL              m_bUseSSL;
   DWORD             m_dwProxyAuthScheme;

public:
   CWslHttpConnection(CWslHttpSession& Session, 
      const CWslString& sServer, 
      bool bUseSSL, 
      int nPort = INTERNET_DEFAULT_PORT) throw (CWslException*);

   virtual ~CWslHttpConnection();

   void Close();

   void OpenGetRequest(const CWslString& sPath) throw (CWslException*);   

   CWslHttpFilePtr GetData();

   bool IsOpened() const { return m_hRequest != NULL; }
   HINTERNET GetRequestHandle() const { return m_hRequest; }

protected:

   void HandleProxyAuthentication() throw (CWslException*);
};
