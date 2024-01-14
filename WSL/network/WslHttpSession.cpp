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

#ifdef _MFC_VER
#include "..\..\wsf\wsf\main.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*****************************************************************************/
CWslHttpSession::CWslHttpSession()
{
   m_hOpen = NULL;
   m_bAutoLogonEnabled = true;
   
   AllowProxyAuthScheme(AUTH_NEGOTIATE);
   AllowProxyAuthScheme(AUTH_NTML);
   AllowProxyAuthScheme(AUTH_PASSPORT);
   AllowProxyAuthScheme(AUTH_DIGEST);
   AllowProxyAuthScheme(AUTH_BASIC);
}

/*****************************************************************************/
CWslHttpSession::~CWslHttpSession()
{
   Close();
}

/*****************************************************************************/
void CWslHttpSession::Open() throw (CWslException*)
{
   Close();

   if (m_sAgent.IsEmpty())
   {
#ifdef _MFC_VER
      m_sAgent = WsfGetApp()->GetAppNameVersion();
#else
      m_sAgent = "SpyTech WSL HTTP Client";
#endif
   }

   BSTR bstrAgent = m_sAgent.AllocSysString();

   m_hOpen = WinHttpOpen(L"SpyTech WSL HTTP Client",  
      WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
      WINHTTP_NO_PROXY_NAME, 
      WINHTTP_NO_PROXY_BYPASS, 0);

   SysFreeString(bstrAgent);

   if (!m_hOpen) throw new CWslWin32Exception();
}


/*****************************************************************************/
void CWslHttpSession::OpenWithProxy(const CWslString& sProxyServer) throw (CWslException*)
{
   Close();

   if (m_sAgent.IsEmpty())
   {
#ifdef _MFC_VER
      m_sAgent = WsfGetApp()->GetAppNameVersion();
#else
      m_sAgent = "SpyTech WSL HTTP Client";
#endif
   }

   BSTR bstrAgent = m_sAgent.AllocSysString();
   BSTR bstrProxy = sProxyServer.AllocSysString();

   m_hOpen = WinHttpOpen(L"SpyTech WSL HTTP Client",  
      WINHTTP_ACCESS_TYPE_NAMED_PROXY,
      bstrProxy, 
      L"<local>", 0);

   SysFreeString(bstrAgent);
   SysFreeString(bstrProxy);

   if (!m_hOpen) throw new CWslWin32Exception();
}

/*****************************************************************************/
void CWslHttpSession::Close()
{
   if (m_hOpen)
   {
      WinHttpCloseHandle(m_hOpen);
      m_hOpen = NULL;
   }
}

/*****************************************************************************/
void CWslHttpSession::EnableAutoLogon(bool bEnable)
{
   m_bAutoLogonEnabled = bEnable;
}

/*****************************************************************************/
void CWslHttpSession::SetProxyCredentials(const CWslString& sUser, const CWslString& sPwd)
{
   m_sProxyUser = sUser;
   m_sProxyPwd = sPwd;
}

/*****************************************************************************/
void CWslHttpSession::ClearProxyAuthSchemes()
{
   m_aProxyAuthSchemes.RemoveAll();
}

/*****************************************************************************/
void CWslHttpSession::AllowProxyAuthScheme(EAuthSchemes eScheme)
{
   m_aProxyAuthSchemes += eScheme;
}

/*****************************************************************************/
void CWslHttpSession::ProhibitProxyAuthScheme(EAuthSchemes eScheme)
{
   int n = m_aProxyAuthSchemes.Find(eScheme);
   if (n != -1) m_aProxyAuthSchemes.RemoveAt(n);
}

/*****************************************************************************/
CWslHttpSession::EAuthSchemes CWslHttpSession::GetProxyAuthScheme(DWORD dwSupportedSchemes)
{
   for (int i=0; i < m_aProxyAuthSchemes.GetSize(); i++)
   {
      if (dwSupportedSchemes & m_aProxyAuthSchemes[i]) return m_aProxyAuthSchemes[i];
   }

   return AUTH_NONE;
}

