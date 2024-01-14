///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslArray
// | P |  /  |  |  ~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1999-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslArray.h"

//******************************************************************************
CWslBaseArray::CWslBaseArray()
{
	m_pFirstItem = NULL;
	m_nSize = 0;
	m_nMaxSize = 0;
	m_nItemSize = 0;
}

//******************************************************************************
CWslBaseArray::CWslBaseArray(int nItemSize)
{
	ASSERT(nItemSize > 0);

	m_pFirstItem = NULL;
	m_nSize = 0;
	m_nMaxSize = 0;
	m_nItemSize = nItemSize;
}

//******************************************************************************
CWslBaseArray::~CWslBaseArray()
{
  // NOTE: Call RemoveAll() in the descendant class' destructor
  // because we don't see its virtual methods here!
  ASSERT(GetSize()==0);
}

/*****************************************************************************/
void CWslBaseArray::CopyItems(void* pDest, const void* pSrc, int nCount) 
{ 
	if (!nCount) return;
	ASSERT(pDest);
	ASSERT(pSrc);
	memcpy(pDest, pSrc, nCount*m_nItemSize);
}

/*****************************************************************************/
void CWslBaseArray::ConstructItems(void* pElement, int nCount)
{
  // Simple data types don't need any construction
}

/*****************************************************************************/
void CWslBaseArray::DestructItems(void* pElement, int nCount)
{
  // Simple data types don't need any construction
}

//******************************************************************************
void CWslBaseArray::operator =(const CWslBaseArray &a)
{
	if (this != &a) 
	{
		ASSERT(m_nItemSize == a.m_nItemSize);
		__SetSize(a.m_nSize, true);
		CopyItems(m_pFirstItem, a.m_pFirstItem, m_nSize);
	}
}
	
//******************************************************************************
void CWslBaseArray::operator +=(const CWslBaseArray &a)
{
	ASSERT(m_nItemSize == a.m_nItemSize);
	int nOldSize = m_nSize;
	GrowBy(a.GetSize());
	CopyItems(m_pFirstItem+nOldSize*m_nItemSize, a.m_pFirstItem, m_nSize);
}

//******************************************************************************
void CWslBaseArray::__SetSize(int nSize, BOOL bConstructDesctruct)
{
	ASSERT(nSize >=0);
	
	if (nSize==0)
	{
		if (bConstructDesctruct) DestructItems(m_pFirstItem, m_nSize);
		if (m_pFirstItem) { free(m_pFirstItem); m_pFirstItem = NULL; }
		m_nMaxSize = 0;
	} 
	else if (nSize > m_nSize)
	{
		if (nSize > m_nMaxSize)
		{
			int nGrowBy = min(1024, max(4,nSize/8));
			m_nMaxSize = ((nSize + nGrowBy - 1) / nGrowBy) * nGrowBy;
			m_pFirstItem = (BYTE*) realloc(m_pFirstItem, m_nMaxSize * m_nItemSize);
			ASSERT(m_pFirstItem);
		}
		if (bConstructDesctruct) ConstructItems(m_pFirstItem+m_nSize*m_nItemSize, nSize-m_nSize);
	}
	else if (nSize < m_nSize)
	{
		if (bConstructDesctruct) DestructItems(m_pFirstItem+nSize*m_nItemSize, m_nSize-nSize);
	}

	m_nSize = nSize;
}

//******************************************************************************
void CWslBaseArray::SetBufferSize(int nSize)
{
	int n = m_nSize;
	SetSize(nSize);
	m_nSize = min(n, nSize);
}

/*****************************************************************************/
void CWslBaseArray::Attach(void* pBuffer, int nSize)
{
	ASSERT(pBuffer);
	SetSize(0);
	
	m_nSize = nSize / m_nItemSize;
	if (!m_nSize) return;
	m_nMaxSize = m_nSize;
	m_pFirstItem = (BYTE*) pBuffer;
}

/*****************************************************************************/
void* CWslBaseArray::Detach()
{
	void* p = m_pFirstItem;
	m_pFirstItem = NULL;
	m_nSize = m_nMaxSize = 0;
	return p;
}

//******************************************************************************
void CWslBaseArray::InsertAt(int nPos)
{
	ASSERT(nPos <= m_nSize && nPos >= 0);

	int nPrevSize = m_nSize;
	__SetSize(m_nSize+1, false);
	void* p = m_pFirstItem + nPos * m_nItemSize;

	if (nPos < nPrevSize)
	{
		memmove(m_pFirstItem + (nPos + 1) * m_nItemSize, p, 
			m_nItemSize * (nPrevSize - nPos));
	}

	ConstructItems(p, 1);
}

//******************************************************************************
void CWslBaseArray::RemoveAt(int nItem)
{
	ASSERT(nItem < m_nSize && nItem >= 0);
	
	void* p = m_pFirstItem + nItem * m_nItemSize;
	DestructItems(p, 1);

	if (nItem < --m_nSize) 
	{
		memcpy(p, m_pFirstItem + (nItem + 1) * m_nItemSize, 
			m_nItemSize * (m_nSize - nItem));
	}
}

//******************************************************************************
void CWslBaseArray::RemoveAll()
{
	if (m_pFirstItem)
	{
		DestructItems(m_pFirstItem, m_nSize);
		free(m_pFirstItem);
		m_pFirstItem = NULL;
		m_nSize = 0;
		m_nMaxSize = 0;
	}
}

//******************************************************************************
void CWslBaseArray::RemoveRange(int nFirst, int nCount)
{
	ASSERT(nFirst >= 0  &&  nFirst + nCount <= m_nSize && nCount >= 0);

	void* p = m_pFirstItem + nFirst*m_nItemSize;
	DestructItems(p, nCount);
	m_nSize -= nCount;

	if (nFirst < m_nSize)
	{
		memcpy(p, m_pFirstItem + (nFirst + nCount) * m_nItemSize, 
			m_nItemSize * (m_nSize - nFirst));
	}
}

//******************************************************************************
void CWslBaseArray::FreeExtra()
{
	if (m_nMaxSize)
	{
		ASSERT(m_nSize);
		m_pFirstItem = (BYTE*) realloc(m_pFirstItem, m_nSize * m_nItemSize);
		ASSERT(m_pFirstItem);
		m_nMaxSize = m_nSize;
	}
}

//******************************************************************************
void CWslBaseArray::SetItems(int nFirstItem, int nNumItems, void *pItems)
{
	ASSERT(nFirstItem >= 0  &&  nFirstItem + nNumItems <= m_nSize && nNumItems);
	CopyItems(m_pFirstItem + nFirstItem, pItems, nNumItems);
};

//******************************************************************************
/*int CWslBaseArray::Find(int nOffset, DWORD dwData)
{
	BYTE *pItem = m_pFirstItem;
	ASSERT(nOffset + (int)sizeof(DWORD) <= m_nItemSize);

	for (int i = m_nSize; i--;) 
	{
		if (dwData == *(DWORD*)(pItem + nOffset)) return m_nSize - i - 1;
		pItem += m_nItemSize;
	}

	return -1;
}*/

//******************************************************************************
/*int CWslBaseArray::Find(int nOffset, 
												const char* pcszBuffer, 
												int nBufferLen)
{
	BYTE *pItem = m_pFirstItem;
	if (nBufferLen == -1) nBufferLen = strlen(pcszBuffer);
	ASSERT(nOffset + nBufferLen <= m_nItemSize);

	for (int i = m_nSize; i--;)
	{
		if (!memcmp(pItem + nOffset, pcszBuffer, nBufferLen)) return m_nSize - i - 1;
		pItem += m_nItemSize;
	}

	return -1;
}*/

//******************************************************************************
void CWslBaseArray::Sort(SORT_CALLBACK* pfnCallback, void* pUserData)
{
	ASSERT(pfnCallback);
	BYTE *p1, *p2;
	int nSize = m_nItemSize;

	for (int i = m_nSize; i--;)
	{
		p1 = m_pFirstItem;
		p2 = m_pFirstItem + m_nItemSize;

		for (int t = m_nSize - 1; t--;)
		{	
			(*pfnCallback)((void*)p1, (void*)p2, pUserData); // swaps items if needed
			p1 += m_nItemSize;
			p2 += m_nItemSize;
		}
	}
}

//******************************************************************************
int CWslBaseArray::FindBinHalving(BHFIND_CALLBACK* pfnCallback, 
																	void* pObject, 
																	void* pUserData)
{
   ASSERT(pfnCallback);
	 ASSERT(pObject);

   int nNum = m_nSize;
   int nLeft = 0;
   int nRight = nNum - 1;

   int nOldPos = -1;
   while (nLeft < nRight) {
      int nPos = (nRight + nLeft) / 2;
      if (nOldPos == nPos) return -1;
      nOldPos = nPos;

      BYTE *p = m_pFirstItem + (nPos * m_nItemSize);
      int nResult = (*pfnCallback)(pObject, (void*)p, pUserData);
      switch (nResult) {
      case 0:
         return nPos;
      case -1:
         nRight = nPos;
         break;
      case 1:
         nLeft = nPos;
         break;
      }
   }

   return -1;
}

/*****************************************************************************
*
* SERIALIZATION
*
*****************************************************************************/

#ifdef _MFC_VER

//******************************************************************************
WSL_API CArchive& operator <<(CArchive &ar, const CWslBaseArray &a)
{
	ar << a.m_nItemSize;
	ar << a.m_nSize;
	ar.Write(a.m_pFirstItem, a.m_nSize * a.m_nItemSize);
	return ar;
}

//******************************************************************************
WSL_API CArchive& operator >>(CArchive &ar, CWslBaseArray &a)
{
	int nSize;

	ar >> a.m_nItemSize;
	ar >> nSize;
	a.SetSize(nSize);
	ar.Read(a.m_pFirstItem, a.m_nSize * a.m_nItemSize);
	return ar;
}

//******************************************************************************
void CWslBaseArray::Serialize(CArchive &ar)
{
//	CObject::Serialize(ar);

	if (ar.IsStoring())
		ar << *this;
	else
		ar >> *this;
}

#endif


