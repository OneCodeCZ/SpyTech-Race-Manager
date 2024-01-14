///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - SpyTech C++ Library
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1998-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2004
//
///////////////////////////////////////////////////////////////////////////////

// An exception representig a WSA exception

#pragma once
#include "..\generic\WslException.h"

/*****************************************************************************/
class WSL_API CWslSocketException : public CWslException
{
public:
   int        m_nErrorCode;

public:
   // Gets the error code by calling the WSAGetLastError() method.
   CWslSocketException();
   
   CWslSocketException(int nErrorCode);

protected:
   ~CWslSocketException();

   virtual CWslString GetErrorMsg();
};