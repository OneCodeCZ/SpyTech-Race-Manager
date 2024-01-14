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
#include <Winhttp.h>
#include "..\generic\WslPtr.h"
#include "..\generic\WslString.h"
#include "..\generic\WslArray.h"
#include "..\generic\WslException.h"

typedef CWslPtr<class CWslHttpFile> CWslHttpFilePtr;
typedef CWslPtr<class CWslHttpRequest> CWslHttpRequestPtr;

/*****************************************************************************/
class WSL_API CWslHttpSession
{
public:
   enum EAuthSchemes 
   {
      AUTH_NONE = 0,
      AUTH_BASIC = WINHTTP_AUTH_SCHEME_BASIC,
      AUTH_NTML = WINHTTP_AUTH_SCHEME_NTLM,
      AUTH_PASSPORT = WINHTTP_AUTH_SCHEME_PASSPORT,
      AUTH_DIGEST = WINHTTP_AUTH_SCHEME_DIGEST,
      AUTH_NEGOTIATE = WINHTTP_AUTH_SCHEME_NEGOTIATE
   };

protected:
   HINTERNET         m_hOpen;
   CWslString        m_sProxyUser, m_sProxyPwd;
   CWslArray<EAuthSchemes>  m_aProxyAuthSchemes;
   CWslString        m_sAgent;
   bool              m_bAutoLogonEnabled;

public:
   CWslHttpSession();
   virtual ~CWslHttpSession();

   void Open() throw (CWslException*);
   void OpenWithProxy(const CWslString& sProxyServer) throw (CWslException*);

   void Close();

   bool IsOpened() const { return m_hOpen != NULL; }

   HINTERNET GetHandle() const { return m_hOpen; }

   // Enables auto logon to servers and proxy servers in the LAN
   // - Auto login uses the current user's credentials.
   // - By default auto logon is enabled
   // - NOTE: If the process runs under the LocalService or NetworkService user
   //   the computer credentials are used --> usualy the access is denied by the proxy
   void EnableAutoLogon(bool bEnable);

   bool IsAutoLogon() const { return m_bAutoLogonEnabled; }

   //---------------------------------------------------------------------------
   // PROXY
   //---------------------------------------------------------------------------

   void SetProxyCredentials(const CWslString& sUser, const CWslString& sPwd);

   const CWslString& GetProxyUser() const { return m_sProxyUser; }
   const CWslString& GetProxyPassword() const { return m_sProxyPwd; }

   void ClearProxyAuthSchemes();
   void AllowProxyAuthScheme(EAuthSchemes eScheme);
   void ProhibitProxyAuthScheme(EAuthSchemes eScheme);

   EAuthSchemes GetProxyAuthScheme(DWORD dwSupportedSchemes);
};

#include "WslHttpRequest.h"
#include "WslHttpFile.h"
