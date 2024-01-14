///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - ODBC support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 08
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslLongBinary.h"

/*****************************************************************************/
CWslLongBinary::CWslLongBinary() 
{ 
	m_pbyData = NULL; 
	m_nSize = 0; 
}

/*****************************************************************************/
CWslLongBinary::CWslLongBinary(int nSize) 
{ 
	m_pbyData = NULL; 
	Alloc(nSize); 
}

/*****************************************************************************/
CWslLongBinary::~CWslLongBinary() 
{ 
	delete m_pbyData; 
}

/*****************************************************************************/
void CWslLongBinary::Alloc(int nSize) 
{  
	ASSERT(nSize>=0); 
	delete m_pbyData; 
	m_pbyData = new BYTE[nSize]; 
	m_nSize = nSize; 
}

/*****************************************************************************/
void CWslLongBinary::Realloc(int nSize)
{
	ASSERT(nSize>=0); 
	BYTE* p = new BYTE[nSize]; 
	if (m_pbyData) memcpy(p, m_pbyData, min(nSize,m_nSize));
	delete m_pbyData; 
	m_pbyData = p;
	m_nSize = nSize; 
}

/*****************************************************************************/
void CWslLongBinary::Free() 
{ 
	delete m_pbyData;
	m_pbyData = NULL; 
	m_nSize = 0; 
}

/*****************************************************************************/
void CWslLongBinary::Attach(void* pData, int nSize)
{
	ASSERT(pData);
	ASSERT(nSize>=0);

	delete m_pbyData;
	m_pbyData = (BYTE*) pData;
	m_nSize = nSize;
}

/*****************************************************************************/
BYTE* CWslLongBinary::Detach()
{
	BYTE* p = m_pbyData;
	m_pbyData = NULL;
	m_nSize = 0;
	return p;
}





