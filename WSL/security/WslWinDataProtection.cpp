///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL
// | P |  /  |  |  ~~~
// | Y | /-  |  |  Copyright (C)2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz
// | H |  _||   |
// |___|________|  Last changed: 2005 04
//
///////////////////////////////////////////////////////////////////////////////

// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnsecure/html/windataprotection-dpapi.asp

#include "stdafx.h"
#include "WslWinDataProtection.h"

#define DESC L"Encrypted by CWslWinDataProtection"

/*****************************************************************************/
CWslString CWslWinDataProtection::Protect(const CWslString& sText, 
                                          const CWslString& sPassword) throw (CWslException*)
{
   DATA_BLOB i;
   i.pbData = (BYTE*) (const char*) sText;
   i.cbData = sText.GetLength();

   DATA_BLOB e;
   e.pbData = (BYTE*) (const char*) sPassword;
   e.cbData = sPassword.GetLength();

   CRYPTPROTECT_PROMPTSTRUCT p;
   p.cbSize = sizeof(p);
   p.dwPromptFlags = 0;
   p.hwndApp = NULL;
   p.szPrompt = NULL;

   DATA_BLOB o;
   
   BOOL b = CryptProtectData(&i, DESC, &e, NULL, &p, CRYPTPROTECT_UI_FORBIDDEN, &o);

   if (!b) throw new CWslWin32Exception();

   // Create string

   CWslString s;
   char* psz = s.GetBuffer(o.cbData+1);
   memcpy(psz, o.pbData, o.cbData);
   psz[o.cbData] = 0;
   s.ReleaseBuffer(o.cbData);

   // Securely clear the buffer

   SecureZeroMemory(o.pbData, o.cbData);
   LocalFree(o.pbData);

   return s;
}
      
/*****************************************************************************/
CWslString CWslWinDataProtection::Unprotect(const CWslString& sEncryptedText,
                                            const CWslString& sPassword) throw (CWslException*)
{
   DATA_BLOB i;
   i.pbData = (BYTE*) (const char*) sEncryptedText;
   i.cbData = sEncryptedText.GetLength();

   DATA_BLOB e;
   e.pbData = (BYTE*) (const char*) sPassword;
   e.cbData = sPassword.GetLength();

   CRYPTPROTECT_PROMPTSTRUCT p;
   p.cbSize = sizeof(p);
   p.dwPromptFlags = 0;
   p.hwndApp = NULL;
   p.szPrompt = NULL;

   DATA_BLOB o;
   LPWSTR swDesc;
   
   BOOL b = CryptUnprotectData(&i, &swDesc, &e, NULL, &p, CRYPTPROTECT_UI_FORBIDDEN, &o);

   if (!b) throw new CWslWin32Exception();

   // Create string

   CWslString s;
   char* psz = s.GetBuffer(o.cbData+1);
   memcpy(psz, o.pbData, o.cbData);
   psz[o.cbData] = 0;
   s.ReleaseBuffer(o.cbData);

   // Securely clear the buffer

   SecureZeroMemory(o.pbData, o.cbData);
   LocalFree(o.pbData);
   LocalFree(swDesc);

   return s;
}     
