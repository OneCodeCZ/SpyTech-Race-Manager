///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslString
// | P |  /  |  |  ~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslString.h"
#include <malloc.h>
#include <stdarg.h>
#include "WslTextStuff.h"

/*****************************************************************************
*
* CWslString (only for non-MFC compiles)
*
*****************************************************************************/

#ifndef _MFC_VER

CWslString::CData	CWslString::m_EmptyString = {0,1,1};
// NOTE: This must be immediately after the empty string definition.
// The next byte must be zero to simulate an empty string data.
// However we must put here a non-zero value to prevent transfering the data into 
// uninitialized segment. It's achieved by putting here an integer value
// whose second byte is non-zero ;-)
int CWslString::m_nDummy = 256;

//*****************************************************************************
CWslString::CWslString()
{
	SetCData(&m_EmptyString);
	GetCData()->nRefCount++;
}

//*****************************************************************************
CWslString::CWslString(const char* pcsz)
{
	SetCData(&m_EmptyString);
	GetCData()->nRefCount++;
	operator=(pcsz);
}

//*****************************************************************************
CWslString::CWslString(const unsigned short *pcwsz, int nCharSet)
{
#ifdef _WIN_32
   int nSize = wcslen(pcwsz);
   Alloc(nSize+1, false);
	GetCData()->nLen = nSize;
   memset(GetCData()->GetData(), 0, nSize);
   WideCharToMultiByte(nCharSet, 0, pcwsz, -1, GetCData()->GetData(), 1024, "?", NULL);
#else
   _ASSERT(FALSE);
#endif
}

//*****************************************************************************
CWslString::CWslString(char ch)
{
	SetCData(&m_EmptyString);
	GetCData()->nRefCount++;
	operator=(ch);
}

//*****************************************************************************
CWslString::CWslString(const CWslString& s)
{
	SetCData(s.GetCData());
	GetCData()->nRefCount++;
}

//*****************************************************************************
CWslString::~CWslString()
{
	if (!--GetCData()->nRefCount) free(GetCData());
}

/*****************************************************************************
*
* BUFFER MANAGEMENT
*
*****************************************************************************/

//*****************************************************************************
void CWslString::Alloc(int nSize, BOOL bKeepData)
{
	BOOL bFree;
	CData* pOldData = GetCData();

	// Allocate a new buffer?

	if (pOldData->nRefCount > 1)
	{
		pOldData->nRefCount--;
		bFree = false;
	}
	else if (pOldData->nBufferSize < nSize)
	{
		bFree = true;
	}
	else 
	{
		return;		// buffer is OK, no change needed :-)
	}

	// Allocate a new buffer
	// Allocate more for better performance
	// The grow area is 1/8 of the required size and
	// must fit into the range <32,200KB>
	
	int nGrowBuffer = max(min(nSize/8, 200*1024), 32);
	//TRACE2("CWslString::Alloc(): nSize=%d, nGrowBuffer=%d\n", nSize, nGrowBuffer);
	int nNewSize = nSize+nGrowBuffer;//((nSize+1023)/1024)*1024;

	SetCData((CData*) malloc(sizeof(CData) + nNewSize));
	GetCData()->nBufferSize = nNewSize;
	GetCData()->nRefCount = 1;

	// Copy the current data to the new buffer

	if (bKeepData) 
	{
		ASSERT(nSize >= pOldData->nLen + 1);
		strcpy(GetCData()->GetData(), pOldData->GetData());
		GetCData()->nLen = pOldData->nLen;
	}

	// Free the old buffer

	if (bFree) free(pOldData);
}

//*****************************************************************************
char* CWslString::GetBuffer(int nSize)
{
	// Include space for terminater
	Alloc(nSize+1, nSize >= GetLength());
	GetCData()->nLen = 0;
	return GetCData()->GetData();
}

//*****************************************************************************
void CWslString::ReleaseBuffer(int nSize /*= -1*/)
{
	if (nSize != -1)
	{
//		Alloc(nSize+1, true);
		GetCData()->GetData()[nSize] = 0;
		GetCData()->nLen = nSize;
	}
	else 
	{
		GetCData()->nLen = strlen(GetCData()->GetData());
		ASSERT(GetCData()->nLen < GetCData()->nBufferSize);
	}
}

/*****************************************************************************
*
* SETTING A NEW VALUE
*
*****************************************************************************/

//*****************************************************************************
void CWslString::Empty()
{
	if (!--GetCData()->nRefCount) free(GetCData());
	SetCData(&m_EmptyString);
	GetCData()->nRefCount++;
}

//*****************************************************************************
const CWslString& CWslString::operator =(const char* pcsz)
{
	if (pcsz)
	{
		int nSize = strlen(pcsz);
		Alloc(nSize+1, false);
		GetCData()->nLen = nSize;
		strcpy(GetCData()->GetData(), pcsz);
	}
	else Empty();

	return *this;
}

//*****************************************************************************
const CWslString& CWslString::operator =(char ch)
{
	Alloc(1+1, false);
	GetCData()->nLen = 1;
	*(WORD*)(&GetCData()->GetData()[0]) = (WORD) (BYTE) ch;
	return *this;
}

//*****************************************************************************
const CWslString& CWslString::operator =(const CWslString& s)
{
	if (this==&s) return *this;
	if (!--GetCData()->nRefCount) free(GetCData());
	SetCData(s.GetCData());
	GetCData()->nRefCount++;
	return *this;
}

//*****************************************************************************
const CWslString& CWslString::operator +=(const char* pcsz)
{
	if (!pcsz) return *this;
	int n = strlen(pcsz);
	int xx = GetCData()->nLen;
	Alloc(GetCData()->nLen+n+1, true);
	strcpy(GetCData()->GetData() + GetCData()->nLen, pcsz);
	GetCData()->nLen += n;
	return *this;
}

//*****************************************************************************
const CWslString& CWslString::operator +=(const CWslString& s)
{
	ASSERT(&s != this);  // not supported yet
	Alloc(GetCData()->nLen + s.GetCData()->nLen + 1, true);
	strcpy(GetCData()->GetData() + GetCData()->nLen, s.GetCData()->GetData());
	GetCData()->nLen += s.GetCData()->nLen;
	return *this;
}

//*****************************************************************************
const CWslString& CWslString::operator +=(char ch)
{
	Alloc(GetCData()->nLen+1+1, true);
	*(WORD*)(&GetCData()->GetData()[GetCData()->nLen]) = (WORD) (BYTE) ch;
	GetCData()->nLen++;
	return *this;
}

//*****************************************************************************
void CWslString::Format(const char* pcszFormat, ...)
{
	ASSERT(pcszFormat);
	va_list l;
	va_start(l, pcszFormat);
	Format(pcszFormat, l);
	va_end(l);
}

//*****************************************************************************
void CWslString::Format(const char* pcszFormat, va_list& l)
{
	ASSERT(pcszFormat);
	char temp[1024];

	int n = _vsnprintf(temp, 1024, pcszFormat, l);
	if (n < 0) { n = 1024; temp[1023] = 0; }

	char* p = GetBuffer(n+1);
	ASSERT(p);
	strcpy(p, temp);
	ReleaseBuffer(n);
}

//*****************************************************************************
void CWslString::AppendFormat(const char* pcszFormat, ...)
{
	ASSERT(pcszFormat);
	va_list l;
	char temp[1024];

	va_start(l, pcszFormat);
	_vsnprintf(temp, 1024, pcszFormat, l);
	temp[1023] = 0;
	va_end(l);

	*this += temp;
}

/*****************************************************************************
*
* CONCATENATION OF TWO STRINGS
*
*****************************************************************************/

//*****************************************************************************
const CWslString operator +(const CWslString& l, const CWslString& r)
{
	CWslString o;
	int n = l.GetCData()->nLen + r.GetCData()->nLen;
	o.Alloc(n+1, false);
	o.GetCData()->nLen = n;
	strcpy(o.GetCData()->GetData(), l.GetCData()->GetData());
	strcpy(o.GetCData()->GetData() + l.GetCData()->nLen, r.GetCData()->GetData());
	return o;
}

//*****************************************************************************
const CWslString operator +(const CWslString& s, const char* pcsz)
{
	CWslString o;
	if (!pcsz) return o=s;
	int n = strlen(pcsz);
	int n2 = s.GetCData()->nLen + n;
	o.Alloc(n2+1, false);
	o.GetCData()->nLen = n2;
	strcpy(o.GetCData()->GetData(), s.GetCData()->GetData());
	strcpy(o.GetCData()->GetData() + s.GetCData()->nLen, pcsz);
	return o;
}

//*****************************************************************************
const CWslString operator +(const char* pcsz, const CWslString& s)
{
	CWslString o;
	if (!pcsz) return o=s;
	int n = strlen(pcsz);
	int n2 = s.GetCData()->nLen + n;
	o.Alloc(n2+1, false);
	o.GetCData()->nLen = n2;
	strcpy(o.GetCData()->GetData(), pcsz);
	strcpy(o.GetCData()->GetData() + n, s.GetCData()->GetData());
	return o;
}

//*****************************************************************************
const CWslString operator +(const CWslString& s, char ch)
{
	CWslString o;
	o.Alloc(s.GetCData()->nLen+2, false);
	o.GetCData()->nLen = s.GetCData()->nLen+1;
	strcpy(o.GetCData()->GetData(), s.GetCData()->GetData());
	*(WORD*)(&o.GetCData()->GetData()[s.GetCData()->nLen]) = (WORD) (BYTE) ch;
	return o;
}

//*****************************************************************************
const CWslString operator +(char ch, const CWslString& s)
{
	CWslString o;
	o.Alloc(s.GetCData()->nLen+2, false);
	o.GetCData()->nLen = s.GetCData()->nLen+1;
	o.GetCData()->GetData()[0] = ch;
	strcpy(o.GetCData()->GetData()+1, s.GetCData()->GetData());
	return o;
}

/*****************************************************************************
*
* MODIFICATIONS
*
*****************************************************************************/

//*****************************************************************************
void CWslString::MakeLower()
{
	if (GetCData()->nRefCount > 1) *this = (const char*) *this;
	_strlwr(GetCData()->GetData());
}

//*****************************************************************************
void CWslString::MakeUpper()
{
	if (GetCData()->nRefCount > 1) *this = (const char*) *this;
	_strupr(GetCData()->GetData());
}

/*****************************************************************************/
void CWslString::Translate(const void* pTable)
{
	ASSERT(pTable);
	if (GetCData()->nRefCount > 1) *this = (const char*) *this;

	BYTE* p = (BYTE*) m_pszData;
	const BYTE* pbyTable = (const BYTE*) pTable;
	for (int i=GetLength();i--;) *p++ = pbyTable[*p];
}

/*****************************************************************************/
int CWslString::Replace(char chOld, char chNew)
{
	// Find first replacement
	char *p = m_pszData;
	for (int i=GetLength();i--;p++) if (*p==chOld) break;
	if (i==-1) return 0;

	// Make string copy if needed
	if (GetCData()->nRefCount > 1) 
	{
		*this = (const char*) *this;
		p =  m_pszData + GetLength()-i-1;
	}

	// Replace all
	*p++ = chNew;
	for (int n=1;i--;p++) if (*p==chOld) { *p=chNew; n++; }
	
	return n;
}

/*****************************************************************************
*
* COMPARING
*
*****************************************************************************/

//*****************************************************************************
bool CWslString::operator ==(const char* pcsz)
{
	if (!pcsz) return false;
	return !strcmp(GetCData()->GetData(), pcsz) ? true:false;
}
 
//*****************************************************************************
bool CWslString::operator ==(const CWslString& s)
{
	if (s.GetCData() == GetCData()) return true;
	return !strcmp(GetCData()->GetData(), s.GetCData()->GetData()) ? true:false;
}

//*****************************************************************************
bool CWslString::operator ==(char ch)
{
	return GetCData()->nLen==1 && GetCData()->GetData()[0]==ch;
}

//*****************************************************************************

bool CWslString::operator < (const CWslString& s)
{
   if (strcmp(GetCData()->GetData(), s.GetCData()->GetData()) < 0) return true;
   return false;
}

//*****************************************************************************
bool CWslString::operator > (const CWslString& s)
{
   if (strcmp(GetCData()->GetData(), s.GetCData()->GetData()) > 0) return true;
   return false;
}

//*****************************************************************************
bool CWslString::operator <= (const CWslString& s)
{
   if (strcmp(GetCData()->GetData(), s.GetCData()->GetData()) <= 0) return true;
   return false;
}

//*****************************************************************************
bool CWslString::operator >= (const CWslString& s)
{
   if (strcmp(GetCData()->GetData(), s.GetCData()->GetData()) >= 0) return true;
   return false;
}

/*****************************************************************************
*
* EXTRACTION
*
*****************************************************************************/

/*****************************************************************************/
CWslObArray<CWslString> CWslString::Explode(const char* pcszDelimiters) const
{	
   CWslObArray<CWslString> a;
   char temp[1024];
   const char* p = (const char*) m_pszData;
   
   if (*p)
   {
      while (1)
      {
         p = WslGetToken(p, temp, 1024, pcszDelimiters);
         a += temp;
         if (!*p++) break;
      }
   }
   
   return a;
}

/*****************************************************************************/
CWslString CWslString::Left(int nCount) const
{
	CWslString s;
	int n = min(nCount, GetLength());
	char* p = s.GetBuffer(nCount);
	strncpy(p, m_pszData, n);
	s.ReleaseBuffer(n);
	return s;
}

/*****************************************************************************/
CWslString CWslString::Mid(int nFirst, int nCount)
{
   if (nCount == -1) nCount = GetLength() - nFirst;
   nCount = min(nCount, GetLength() - nFirst);

   CWslString s;
   char* p = s.GetBuffer(nCount);
   strncpy(p, m_pszData + nFirst, nCount);
   s.ReleaseBuffer(nCount);
   return s;
}

/*****************************************************************************/
int CWslString::Find(const char *pcszPattern)
{
   int nStrLength = GetLength();
   int nPatLength = strlen(pcszPattern);

   if (nPatLength == 0) return 0;
   if (nPatLength > nStrLength) return -1;

   int ppos = 0;
   for (int a=0; a<=nStrLength - nPatLength; a++) 
   {
      for (int b=0; b<nPatLength; b++) 
      {
         if (pcszPattern[b] != GetAt(a + b)) break;
      }
      if (b == nPatLength) return a;
   }

   return -1;
}

#endif

/*****************************************************************************
*
* CWslStringArray
*
*****************************************************************************/

/*****************************************************************************/
void CWslStringArray::ExplodeString(const char* pcszText, const char* pcszDelimiters)
{	
   char temp[1024];
   RemoveAll();
   
   while (1)
   {
      pcszText = WslGetToken(pcszText, temp, 1024, pcszDelimiters);
      *this += temp;
      if (!*pcszText++) break;
   }
}
