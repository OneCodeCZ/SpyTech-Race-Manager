///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - CWslException
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1999-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslException.h"

/*****************************************************************************
*
* CWslException
*
*****************************************************************************/

#ifndef _MFC_VER

//*****************************************************************************
CWslException::CWslException(BOOL bAutoDelete /*= true*/)
{
   //ASSERT(false);    // To catch the the point of exception
   m_bAutoDelete = bAutoDelete;
}

//*****************************************************************************
CWslException::~CWslException()
{
}

//*****************************************************************************
void CWslException::Delete()
{
	if (m_bAutoDelete) delete this;
}

#endif

/*****************************************************************************
*
* CWslTextException
*
*****************************************************************************/

/*****************************************************************************/
CWslTextException::CWslTextException(const char* pcszFormat, ...)
  : CWslException(true)
{
//   ASSERT(false);       // just to catch the point of exception

   ASSERT(pcszFormat);
	va_list l;
	va_start(l, pcszFormat);

#ifdef _MFC_VER
	char temp[1024];

	int n = _vsnprintf(temp, 1024, pcszFormat, l);
	if (n < 0) { n = 1024; temp[1023] = 0; }

	char* p = m_sText.GetBuffer(n+1);
	ASSERT(p);
	strcpy(p, temp);
	m_sText.ReleaseBuffer(n);

#else
	m_sText.Format(pcszFormat, l);

#endif
}

/*****************************************************************************/
CWslTextException::CWslTextException(int nResId, ...) : CWslException(true)
{
//   ASSERT(false);       // just to catch the point of exception

   CWslString sFormat(MAKEINTRESOURCE(nResId));
	va_list l;
	va_start(l, nResId);

#ifdef _MFC_VER
	char temp[1024];

	int n = _vsnprintf(temp, 1024, sFormat, l);
	if (n < 0) { n = 1024; temp[1023] = 0; }

	char* p = m_sText.GetBuffer(n+1);
	ASSERT(p);
	strcpy(p, temp);
	m_sText.ReleaseBuffer(n);

#else
	m_sText.Format(sFormat, l);

#endif
}











