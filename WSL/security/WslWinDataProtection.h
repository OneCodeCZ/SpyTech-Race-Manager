///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - Windows Data Protection API Wrapper
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz
// | H |  _||   |
// |___|________|  Last changed: 2005 04
//
///////////////////////////////////////////////////////////////////////////////

#include "..\generic\WslException.h"
#include "..\generic\WslString.h"
#include "..\generic\WslWin32Exception.h"

/*****************************************************************************/
class WSL_API CWslWinDataProtection
{
public:
   // Encrypts the given text using the given password
   static CWslString Protect(const CWslString& sText, const CWslString& sPassword) throw (CWslException*);
      
   // Decrypts the given text using the given password
   static CWslString Unprotect(const CWslString& sEncryptedText, const CWslString& sPassword) throw (CWslException*);
};
  