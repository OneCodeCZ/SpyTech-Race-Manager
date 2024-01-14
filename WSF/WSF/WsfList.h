///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWsfList
// | P |  /  |  |  ~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 11
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

// Double linked list of items of custom defined type. 
// Not dependant on MFC :-) 

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

//*****************************************************************************
template<class T> class CWsfList
{
public:
	struct WSL_API CItem
	{
		T		   m_Item;
		CItem*   m_pNext;
		CItem*   m_pPrev;
	};
	
protected:
	int		m_nNumItems;
	CItem*   m_pHead;
   CItem*   m_pTail;

public:
   CWsfList();
	~CWsfList();

	// Add/remove stuff

	void AddHead(T Item);
	void AddTail(T Item);
	void InsertBefore(POSITION pos, T NewItem);
	void Remove(POSITION pos);
	void RemoveAll();

	// Iteration stuff

	inline int GetSize() const { return m_nNumItems; }
	inline POSITION GetHead() const { return (POSITION)m_pHead; }
	inline POSITION GetTail() const { return (POSITION)m_pTail; }
	T& GetPrev(POSITION& pos, BOOL bCyclic = false);
	T& GetNext(POSITION& pos, BOOL bCyclic = false);

	// Retrieving data

	inline T& GetAt(POSITION pos) { ASSERT(pos); return ((CItem*)pos)->m_Item; }
   T& PeekNext(POSITION pos, BOOL bCyclic = false);

   // Find item
   inline POSITION FindItem(T Item);
};

/*****************************************************************************
*
* IMPLEMENTATION
*
*****************************************************************************/

//*****************************************************************************
template<class T> 
POSITION CWsfList<T>::FindItem(T Item)
{
   CItem *pItem = m_pHead;
   while (pItem) {
      if (pItem->m_Item == Item) return (POSITION)pItem;
      pItem = pItem->m_pNext;
      if (pItem == m_pHead) pItem = NULL;
   }
   return NULL;
}

//*****************************************************************************
template<class T> 
CWsfList<T>::CWsfList()
{
	m_pHead = NULL;
	m_pTail = NULL;
	m_nNumItems = 0;
}

//*****************************************************************************
template<class T> 
CWsfList<T>::~CWsfList()
{
	RemoveAll();
}

//*****************************************************************************
template<class T> 
T& CWsfList<T>::GetPrev(POSITION& pos, BOOL bCyclic)
{ 
	ASSERT(pos);
	T& item = ((CItem*)pos)->m_Item;
	pos = (POSITION)((CItem*)pos)->m_pPrev;
   if ((!pos) && bCyclic) pos = GetTail();
	return item;
}

//*****************************************************************************
template<class T> 
T& CWsfList<T>::GetNext(POSITION& pos, BOOL bCyclic)
{
	ASSERT(pos); 
	T& item = ((CItem*)pos)->m_Item;
	pos = (POSITION)((CItem*)pos)->m_pNext; 
   if ((!pos) && bCyclic) pos = GetHead();
	return item;
}

//*****************************************************************************
template<class T> 
T& CWsfList<T>::PeekNext(POSITION pos, BOOL bCyclic)
{
	ASSERT(pos); 
	pos = (POSITION)((CItem*)pos)->m_pNext; 
   if ((!pos) && bCyclic) pos = GetHead();
	T& item = ((CItem*)pos)->m_Item;
   return item;
}

//*****************************************************************************
template<class T> 
void CWsfList<T>::AddHead(T Item)
{
	CItem *pItem = new CItem();
	pItem->m_Item = Item;
	m_nNumItems++;

	if (m_pHead)
	{
      /*
      if (m_bCyclic == TRUE) {
         m_pHead->m_pPrev = pItem;
         m_pTail->m_pNext = pItem;
         pItem->m_pNext = m_pHead;
         pItem->m_pPrev = m_pTail;
         m_pHead = pItem;
      }
      else {
      */
		   m_pHead->m_pPrev = pItem;
		   pItem->m_pNext = m_pHead;
		   pItem->m_pPrev = NULL;
		   m_pHead = pItem;
      /*
      }
      */
	}
	else
	{
      /*
      if (m_bCyclic == TRUE) {
		   pItem->m_pPrev = pItem;
		   pItem->m_pNext = pItem;
      }
      else {
      */
		   pItem->m_pPrev = NULL;
		   pItem->m_pNext = NULL;
      /*
      }
      */
		m_pHead = m_pTail = pItem;
	}
}

//*****************************************************************************
template<class T> 
void CWsfList<T>::AddTail(T Item)
{
	CItem *pItem = new CItem();
	pItem->m_Item = Item;
	m_nNumItems++;

	if (m_pTail)
	{
      /*
      if (m_bCyclic == TRUE) {
         m_pTail->m_pNext = pItem;
         m_pHead->m_pPrev = pItem;
         pItem->m_pNext = m_pHead;
         pItem->m_pPrev = m_pTail;
		   m_pTail = pItem;
      }
      else {
      */
		   m_pTail->m_pNext = pItem;
		   pItem->m_pPrev = m_pTail;
		   pItem->m_pNext = NULL;
		   m_pTail = pItem;
      /*
      }
      */
	}
	else
	{
      /*
      if (m_bCyclic == TRUE) {
         pItem->m_pPrev = pItem;
         pItem->m_pNext = pItem;
      }
      else {
      */
		   pItem->m_pPrev = NULL;
		   pItem->m_pNext = NULL;
      /*
      }
      */
      m_pHead = m_pTail = pItem;
	}
}

//*****************************************************************************
template<class T> 
void CWsfList<T>::InsertBefore(POSITION pos, T NewItem)
{
	CItem *pItem = (CItem*) pos;
	CItem *pNewItem = new CItem();
	pNewItem->m_Item = NewItem;

	if (pItem == m_pHead)
	{
      delete pNewItem;
		AddHead(NewItem);
	}
	else if (pItem)
	{
		pNewItem->m_pNext = pItem;
		pNewItem->m_pPrev = pItem->m_pPrev;
		pItem->m_pPrev->m_pNext = pNewItem;
		pItem->m_pPrev = pNewItem;
		m_nNumItems++;
	}
   else {
      delete pNewItem;
      AddTail(NewItem);
   }
}

//*****************************************************************************
template<class T> 
void CWsfList<T>::Remove(POSITION pos)
{
	CItem *pItem = (CItem*) pos;
	ASSERT(pItem && m_nNumItems > 0);

	if (pItem == m_pHead) 
	{
		if (pItem == m_pTail) 
		{
			m_pTail = m_pHead = NULL;
		}
		else
		{
			m_pHead = pItem->m_pNext;
			m_pHead->m_pPrev = NULL;
		}
	}
	else if (pItem == m_pTail)
	{
		m_pTail = pItem->m_pPrev;
		m_pTail->m_pNext = NULL;
	}
	else
	{
		pItem->m_pPrev->m_pNext = pItem->m_pNext;
		pItem->m_pNext->m_pPrev = pItem->m_pPrev;
	}

	delete pItem;
	m_nNumItems--;
}

//*****************************************************************************
template<class T> 
void CWsfList<T>::RemoveAll()
{
	CItem *x, *p = m_pHead;
	m_nNumItems = 0;
	m_pHead = m_pTail = NULL;

	while (p)
	{
		x = p->m_pNext;
		delete p;
		p = x;
	}
}
