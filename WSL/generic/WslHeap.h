///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Heap Data Structure 
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2001 
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "WslArray.h"

//*****************************************************************************
template<class T> class CWslHeap  
{
protected:
	CWslArray<T>	m_aHeap;

public:
	CWslHeap();
	~CWslHeap();

	// Inserts new item to the heap (log n)

	void Insert(T item);

	// Extracts the minimum item from the heap (log n)
	
	T ExtractMin();

	// Sets the buffer size of the CWslArray used for the heap.
	// See CWslArray for details. 

	void SetBufferSize(int nSize);

	// Removes all items from the heap

	void RemoveAll();

	// Returns number of items in the heap

	inline int GetSize() const { return m_aHeap.GetSize(); }
};


/*****************************************************************************
*
* IMPLEMENTATION
*
*****************************************************************************/

//*****************************************************************************
template<class T> CWslHeap<T>::CWslHeap()
{
}

//*****************************************************************************
template<class T> CWslHeap<T>::~CWslHeap()
{
}

//*****************************************************************************
template<class T> void CWslHeap<T>::SetBufferSize(int nSize)
{
	_ASSERT(nSize>=0);
	m_aHeap.SetBufferSize(nSize);
}

//*****************************************************************************
template<class T> void CWslHeap<T>::RemoveAll()
{
	m_aHeap.RemoveAll();
}

//*****************************************************************************
template<class T>void CWslHeap<T>::Insert(T item)
{
	int i,t;
	T x;

	// Add item at "the end of the tree"

	m_aHeap += item;	

	// Adjust the heap

	for (i=m_aHeap.GetSize()-1; i; i=t)
	{
		t = (i+1)/2-1;

		if (m_aHeap[i] < m_aHeap[t])
		{
			x = m_aHeap[i];
			m_aHeap[i] = m_aHeap[t];
			m_aHeap[t] = x;
		}
	}

//	for (t=0;t<m_aHeap.GetSize();t++) _ASSERT(m_aHeap[0] <= m_aHeap[t]);
}

//*****************************************************************************
template<class T> T CWslHeap<T>::ExtractMin()
{
	int t,i,nSize;
	T x,x2;

	// Get minimum

	x = m_aHeap[0];

	// Adjust the heap

	nSize = m_aHeap.GetSize()-1;
	m_aHeap[0] = m_aHeap[nSize];
	m_aHeap.ShrinkBy(1);
	
	for (i=0; true; i=t)
	{
		t = (i+1)*2-1;

		if (t >= nSize) break;
		
		if (m_aHeap[i] < m_aHeap[t])
		{
			if (t+1 >= nSize || m_aHeap[i] < m_aHeap[t+1]) break;
			if (m_aHeap[t+1] < m_aHeap[t]) t++;
		}
		else
		{ 
			if (t+1 < nSize && m_aHeap[t+1] < m_aHeap[t]) t++;
		}

		x2 = m_aHeap[i];
		m_aHeap[i] = m_aHeap[t];
		m_aHeap[t] = x2;
	}

//	for (t=0;t<m_aHeap.GetSize();t++) _ASSERT(m_aHeap[0] <= m_aHeap[t]);
	return x;
}




















