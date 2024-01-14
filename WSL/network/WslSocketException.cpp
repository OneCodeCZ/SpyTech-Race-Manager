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

#include "stdafx.h"
#include "WslSocketException.h"

/*****************************************************************************/
CWslSocketException::CWslSocketException() : CWslException(true)
{
   m_nErrorCode = WSAGetLastError();
   ASSERT(false); // to catch the point of exception
}

/*****************************************************************************/
CWslSocketException::CWslSocketException(int nErrorCode) : CWslException(true)
{
   m_nErrorCode = nErrorCode;
   ASSERT(false); // to catch the point of exception
}

/*****************************************************************************/
CWslSocketException::~CWslSocketException()
{

}

/*****************************************************************************/
CWslString CWslSocketException::GetErrorMsg()
{
   CWslString s;
   s.Format("Windows Sockets error code: %d.", m_nErrorCode);
   return s;
}
