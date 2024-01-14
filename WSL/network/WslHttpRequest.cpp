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
CWslHttpConnection::CWslHttpConnection(CWslHttpSession& Session, 
                                       const CWslString& sServer, 
                                       bool bUseSSL, 
                                       int nPort) throw (CWslException*)
: m_Session(Session)
{
   m_hRequest = NULL;
   m_bUseSSL = bUseSSL;
   BSTR bstrServer = sServer.AllocSysString();
   
   m_hConnection = WinHttpConnect(m_Session.GetHandle(), bstrServer, nPort, 0);
   
   SysFreeString(bstrServer);
   if (!m_hConnection) throw new CWslWin32Exception();
}

/*****************************************************************************/
CWslHttpConnection::~CWslHttpConnection()
{
   Close();
   WinHttpCloseHandle(m_hConnection);
}

/*****************************************************************************/
void CWslHttpConnection::Close()
{
   if (m_hRequest)
   {
      WinHttpCloseHandle(m_hRequest);
      m_hRequest = NULL;
   }
}

/*****************************************************************************/
CWslHttpFilePtr CWslHttpConnection::GetData()
{
   ASSERT(IsOpened());
   return new CWslHttpFile(m_Session, *this);
}

/*****************************************************************************/
void CWslHttpConnection::OpenGetRequest(const CWslString& sPath) throw (CWslException*)
{
   DWORD dw;
   BOOL b;
 
   // Open request
   
   Close();
   BSTR bstrPath = sPath.AllocSysString();
   
   m_hRequest = WinHttpOpenRequest(m_hConnection,
      L"GET", 
      bstrPath,
      NULL, 
      WINHTTP_NO_REFERER, 
      WINHTTP_DEFAULT_ACCEPT_TYPES,
      m_bUseSSL ?  WINHTTP_FLAG_SECURE : 0 
      );
   
   SysFreeString(bstrPath);
   
   if (!m_hRequest) throw new CWslWin32Exception();
   
   // Auto logon

   dw = m_Session.IsAutoLogon() ? WINHTTP_AUTOLOGON_SECURITY_LEVEL_MEDIUM : WINHTTP_AUTOLOGON_SECURITY_LEVEL_HIGH;
   b = WinHttpSetOption(m_hRequest, WINHTTP_OPTION_AUTOLOGON_POLICY, &dw, sizeof(dw));
   if (!b) throw new CWslWin32Exception();

   // Continue to send a request until status code is not 401 or 407.

   bool bDone = false;
   DWORD dwLastStatusCode = 0;
   m_dwProxyAuthScheme = 0;
  
   while (!bDone)
   {
      //  If a proxy authentication challenge was responded to, reset those
      //  credentials before each SendRequest, because the proxy may require 
      //  re-authentication after responding to a 401 or to a redirect. 
      //  If you don't, you can get into a 407-401-407-401- loop.

      if (m_dwProxyAuthScheme != 0)
      {
         BSTR bstrUser = m_Session.GetProxyUser().AllocSysString();
         BSTR bstrPwd = m_Session.GetProxyPassword().AllocSysString();

         b = WinHttpSetCredentials(m_hRequest, WINHTTP_AUTH_TARGET_PROXY, 
            m_dwProxyAuthScheme, 
            bstrUser,
            bstrPwd,
            NULL);

         SysFreeString(bstrUser);
         SysFreeString(bstrPwd);
         
         if (!b) 
         {
            DWORD dw = GetLastError();
            Close();
            throw new CWslWin32Exception(dw);
         }
      }
      
      // Send the request

      b = WinHttpSendRequest(m_hRequest,
         WINHTTP_NO_ADDITIONAL_HEADERS,
         0,
         WINHTTP_NO_REQUEST_DATA,
         0, 
         0, 
         0);
     
      if (!b)
      {
         DWORD dw = GetLastError();
         
         // When using SSL, if the proxy requires auth, we find out here.
         // NOTE : Only Win2003 --> disabled now

         /*if (dw == ERROR_WINHTTP_PROXY_AUTH_REQUIRED)
         {
            HandleProxyAuthentication();
            continue;
         }
         else*/
         {
            Close();
            throw new CWslWin32Exception(dw);
         }
      }
      
      // Get the response
      
      b = WinHttpReceiveResponse(m_hRequest, NULL);
      
      if (!b)
      {
         DWORD dw = GetLastError();
         
         if (dw == ERROR_WINHTTP_RESEND_REQUEST)
         {
            continue;
         }
         else
         {
            Close();
            throw new CWslWin32Exception(dw);
         }
      }
            
      // Check the status code     
      
      DWORD dwStatusCode = 0;
      DWORD dwSize = sizeof(DWORD);

      b = WinHttpQueryHeaders(m_hRequest, 
         WINHTTP_QUERY_STATUS_CODE |
         WINHTTP_QUERY_FLAG_NUMBER,
         NULL, 
         &dwStatusCode, 
         &dwSize, 
         NULL);

      if (!b)
      {
         DWORD dw = GetLastError();
         Close();
         throw new CWslWin32Exception(dw);
      }

      if (dwLastStatusCode == dwStatusCode)
      {
         Close();
         throw new CWslTextException("CWslHttpConnection::OpenGetRequest(): The same HTTP status code %s received twice!", dwStatusCode);
      }

      dwLastStatusCode = dwStatusCode;

      switch (dwStatusCode)
      {
      case 200: 
         bDone = true;
         break;
         
      case 401:
         ASSERT(false);
         Close();
         throw new CWslTextException("CWslHttpConnection::OpenGetRequest(): 401 not supported yet!");
         /*         
         // Obtain the supported and preferred schemes.
         bResults = WinHttpQueryAuthSchemes( hRequest, 
            &dwSupportedSchemes, 
            &dwFirstScheme, 
            &dwTarget );
         
         // Set the credentials before resending the request.
         if( bResults )
         {
            dwSelectedScheme = ChooseAuthScheme( dwSupportedSchemes );
            
            if( dwSelectedScheme == 0 )
               bDone = TRUE;
            else
               bResults = WinHttpSetCredentials( hRequest, dwTarget, 
               dwSelectedScheme,
               pGetRequest->szServerUsername,
               pGetRequest->szServerPassword,
               NULL );
         }
         */
         break;
         
      case 407:
         HandleProxyAuthentication();
         break;
         
      default:
         Close();
         throw new CWslTextException("CWslHttpConnection::OpenGetRequest(): Unexpected HTTP status code %d!", dwStatusCode);
      }
  }
}

/*****************************************************************************/
void CWslHttpConnection::HandleProxyAuthentication() throw (CWslException*)
{
   DWORD dwSupportedSchemes;
   DWORD dwFirstScheme;
   DWORD dwTarget;
   
   // Obtain the supported and preferred schemes.
   
   BOOL b = WinHttpQueryAuthSchemes(m_hRequest, 
      &dwSupportedSchemes, 
      &dwFirstScheme, 
      &dwTarget);

   if (!b)
   {
      DWORD dw = GetLastError();
      Close();
      throw new CWslWin32Exception(dw);
   }

   ASSERT(dwTarget == WINHTTP_AUTH_TARGET_PROXY);

   m_dwProxyAuthScheme = m_Session.GetProxyAuthScheme(dwSupportedSchemes);

   if (!m_dwProxyAuthScheme) 
   {
      DWORD dw = GetLastError();
      Close();
      throw new CWslTextException("CWslHttpConnection::HandleProxyAuthentication(): The server doesn't support any allowed authentication scheme!");
   }

}


