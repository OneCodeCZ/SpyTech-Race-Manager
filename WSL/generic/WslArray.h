///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  CWslArray and derivatives
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)1999-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 02
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "WslPtr.h"

// Array of user definable items that can dynamically grow and shrink.
// CWslArray<T>: Items must not require construction.
// CWslObArray<T>: Items that require construction.

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

// Macros for easy handling of various things
#define ITERATE(array, var) for (var = 0; var < array.GetSize(); var++)
#define IS_FIRST(array, idx) ((idx)==0)
#define IS_LAST(array, idx) ((idx)==(array).GetSize()-1)

/*****************************************************************************
*
* CWslBaseArray
*
*****************************************************************************/

class WSL_API CWslBaseArray
{
public:
	// Callback is called when order of two items must be known.
	// If item1 should be after item2, it must swap the items.
	// Returns TRUE if items were swapt, otherwise FALSE;
	typedef BOOL SORT_CALLBACK(void* pItem1, void* pItem2, void* pUserData);

	// Callback is called whenever comparison of two items due to the binary
	// halving find algorithm is to be performed.
	// Returns 0 if the item is found, -1 if left half is to be searched, 1 if rt.
	typedef int BHFIND_CALLBACK(void* pObject, void* pArrayItem, void* pUserData);

protected:
	BYTE* m_pFirstItem;
	int	m_nSize;				// Num of items used (includes gaps)
	int	m_nMaxSize;			// Num of items allocated.
	int	m_nItemSize;		// Size of array items.

protected:
	CWslBaseArray();			// From serialization only
	CWslBaseArray(int nItemSize);
	virtual ~CWslBaseArray();

public:
	//--------------------------------------------------------------------------
	// Buffer management
	//--------------------------------------------------------------------------

	// Sets array size.
	inline void SetSize(int nSize) { __SetSize(nSize, true); }

	// Grow or shrink the array by the given number of items
	inline void GrowBy(int nNumItems) { ASSERT(nNumItems >= 0); SetSize(m_nSize + nNumItems); }
	inline void ShrinkBy(int nNumItems) { ASSERT(nNumItems >= 0 && m_nSize >= nNumItems); SetSize(m_nSize - nNumItems); }

	// Returns max occupied index in the array
	inline int GetSize() const { return m_nSize; }

	// Returns TRUE if the array is empty.
	inline BOOL IsEmpty() const { return m_nSize == 0; }

	// Sets the size of the buffer for array items. Tha actual array size
	// is not changed (if the buffer size is smaller than the array size, the array size is adjusted
	// to the buffer size)
	// The memory for the buffer will be allocated in one API call, so there is 
	// no need to allocate memory when new item is appended.
	void SetBufferSize(int nSize);

	// Returns the maximum count of items that fit to the allocated memory
	inline int GetBufferSize() const { return m_nMaxSize; }

	// Attaches a buffer to the array. 
	// The array will have: (int) (nSize / m_nItemSize) items.
	// NOTE: If the buffer was allocated using malloc(), the array
	// is resizable. Otherwise the array is READ-ONLY and the buffer
	// must be detached before the array is destroyed. This is left
	// up to the user!
	void Attach(void* pBuffer, int nSize);

	// Detaches the item buffer from the array and
	// returns a pointer at it. The array will be initialized to be emtpty.
	// If the array is empty, NULL is returned.
	void* Detach();

  //---------------------------------------------------------------------------
	// Managing items
	//---------------------------------------------------------------------------

	// Adding
	void InsertAt(int nPos);							// Inserts space at given position
	void operator +=(const CWslBaseArray& a);		// Adds the array to this array

	// Removing
	void RemoveAll();
	void RemoveAt(int nItem);
	void RemoveRange(int nFirst, int nCount);
	void FreeExtra();						// Frees extra allocated memory

	// Copy the array
	void operator =(const CWslBaseArray& a);

	// Sets directly (by memcpy) items
	void SetItems(int nFirstItem, int nNumItems, void *pItems);

	//--------------------------------------------------------------------------
	// Searching & sorting
	// NOTE: Methods are not dependent on the item type!
	//--------------------------------------------------------------------------

	// Sorts items. See callback definition above for more details.
	void Sort(SORT_CALLBACK* pfnCallback, void* pUserData);

   // Perfoms binary halving search. The array MUST be sorted otherwise the
   // result is NOT defined!
	// See callback definition above for more details.
	// BUG: Won't find the last record!
   int FindBinHalving(BHFIND_CALLBACK* pfnCallback, void* pObject, void* pUserData=NULL);

   // OBSOLETE 2005-02
	// Searches the array for the dwData value. nOffset specifies from where within the
	// item the data for comparing are read.
	// Returns index of an item or -1 of not found.
	//int Find(int nOffset, DWORD dwData);

   // OBSOLETE 2005-02
	// The same as previous method, but the specified string is searched for.
	// If nBufferLen == -1, the buffer length is gained as a result
	// of strlen(nBufferLen) call.
	//int Find(int nOffset, const char* pcszBuffer, int nBufferLen = -1);

	//---------------------------------------------------------------------------
	// Serialization (MFC only)
	//---------------------------------------------------------------------------

// Serializes the object to archive
#ifdef _MFC_VER
	virtual void Serialize(CArchive &ar);
	friend WSL_API CArchive& operator <<(CArchive &ar, const CWslBaseArray &a);
	friend WSL_API CArchive& operator >>(CArchive &ar, CWslBaseArray &a);
#endif

  //---------------------------------------------------------------------------
	// Internal stuff
	//---------------------------------------------------------------------------
protected:
  // Copying items
	// The default implementation uses memcpy() function.
	virtual void CopyItems(void* pDest, const void* pSrc, int nCount);

	// Construction & desctruction of array items.
	virtual void ConstructItems(void* pElement, int nCount);
	virtual void DestructItems(void* pElement, int nCount);

	// Internal memory management
	void __SetSize(int nSize, BOOL bConstructDesctruct);
};

/*****************************************************************************
*
* CWslArray<T>
*
*****************************************************************************/

// NOTE: Some operations are not effective if T is a class type, because
// T& instead of T should be used. We however cannot change it, because
// the array must be compatible also with primitive types like int.
// TODO: Sometimes this should be fixed by adding the second 
// template parameter specifying if a reference or direct value 
// should be used (as in MFC).

template<class T> class CWslArray : public CWslBaseArray
{
public:
   // Callback for comparing two items. It's called for example
   // by sorting algorithms.
   // Return -1,0 or 1 if the first item is less, equal or greater than
   // the second item.
   typedef int COMPARE_CALLBACK(const T& item1, const T& item2, DWORD dwUserData);
   
public:
   CWslArray() : CWslBaseArray(sizeof(T)) { }
   
   CWslArray(int nSize) : CWslBaseArray(sizeof(T)) 
   { 
      SetSize(nSize); 
   }
   
   CWslArray(const CWslArray<T>& a) : CWslBaseArray(sizeof(T)) 
   {
      *this = a;
   };
   
   virtual ~CWslArray()
   {
      RemoveAll(); // Our virtual methods are called
   }
   
   // Copying the array
   inline void operator =(const CWslArray<T> &a) { CWslBaseArray::operator =(a); }
   
   //---------------------------------------------------------------------------
   // Basic operations on items
   //---------------------------------------------------------------------------
   
   // Sets/gets the items or item values
   inline T& operator[](int i) { ASSERT(i>=0 && i<m_nSize); return ((T*)m_pFirstItem)[i]; }
   inline T& operator[](int i) const  { ASSERT(i>=0 && i<m_nSize); return ((T*)m_pFirstItem)[i]; }
   inline T& GetAt(int i) { ASSERT(i>=0 && i<m_nSize); return ((T*)m_pFirstItem)[i]; }
   inline T& GetAt(int i) const { ASSERT(i>=0 && i<m_nSize); return ((T*)m_pFirstItem)[i]; }
   
   // Returns the reference to the last item
   inline T& GetLast() { return ((T*)m_pFirstItem)[m_nSize - 1]; }
   
   // Sets the given range of items
   inline void SetItems(int nFirstItem, int nNumItems, T *pItems) { CWslBaseArray::SetItems(nFirstItem, nNumItems, (void*) pItems); }
   
   // Appends an item/array at the end 
   inline void Append(T item) { GrowBy(1); ((T*)m_pFirstItem)[m_nSize - 1] = item; }
   inline void operator +=(T item) { Append(item); }
   inline void operator +=(const CWslBaseArray &a) { CWslBaseArray::operator +=(a); }
   
   // Inserts item at specified position
   void InsertAt(int nPos, T item) { CWslBaseArray::InsertAt(nPos); ((T*)m_pFirstItem)[nPos] = item; }
   void InsertAt(int nPos) { CWslBaseArray::InsertAt(nPos); }
   
   // Returns address of the array
   inline T* GetData() const { return (T*) m_pFirstItem; }
   
   // Stack simulation with the stack top at the end of the array.
   inline void Push(T item) { Append(item); }
   inline T Pop() { ASSERT(m_nSize > 0); T item = ((T*)m_pFirstItem)[m_nSize-1]; ShrinkBy(1); return item; }
   
   //---------------------------------------------------------------------------
   // Searching & sorting
   //---------------------------------------------------------------------------
   
   // Finds an item. The operator == is used for the comparsion.
   // Returns index of the found item or -1 if not found.
   // Complexity: O(n).
   int Find(const T& item) const
   {
      const T* p = (T*) m_pFirstItem;
      for (int i = 0; i<m_nSize; i++, p++) if (*p == item) return i;
      return -1;
   }
   
   // Sorts in the given item. 
   // Returns the position, where the item was inserted.
   // Complexity: O(n).
   int SortIn(const T& item, COMPARE_CALLBACK* pfnCompare, DWORD dwUserData=0)
   {
	   int i;
      for (i=0; i<GetSize(); i++) 
      {
         if (pfnCompare(item, GetAt(i), dwUserData) < 0) break;
      }
      InsertAt(i, item);
      return i;
   }
   
   // Sorts in the given item. The operator < is used for item
   // comparing.
   // Returns the position, where the item was inserted.
   // Complexity: O(n).
   int SortIn(const T& item)
   {
      int i;
      for (i=0; i<GetSize(); i++) if (item < GetAt(i)) break;
      InsertAt(i, item);
      return i;
   }
};


/*****************************************************************************
*
* CWslObArray<T>
*
*****************************************************************************/

// NOTE: placement new required!
#include <new.h>

template<class T> class CWslObArray : public CWslArray<T>
{
public:
   CWslObArray() : CWslArray<T>() 
   { 
      //TRACE1("CWslObArray: Constructing %x...\n", this);
   }
   
   CWslObArray(int nSize) : CWslArray<T>() 
   { 
      SetSize(nSize);
   }
   
   CWslObArray(const CWslObArray<T>& a) : CWslArray<T>()
   { 
      //TRACE1("CWslObArray: Copy-constructing %x...\n", this);
      *this = a;
   };
   
   virtual ~CWslObArray()
   {
      // NOTE: We must destruct all items from the destructor of this class
      // because the top-level destructor doesn't see our virtual methods!
      //TRACE2("CWslObArray: Destructing %x, items=%d\n", this, GetSize());
      RemoveAll();
   }

protected:
	virtual void ConstructItems(void* pElement, int nCount) 
	{ 
		T* p = (T*) pElement;
		for (; nCount--; p++) ::new((void*)p) T;
	}
	
	virtual void DestructItems(void* pElement, int nCount) 
	{ 
		T* p = (T*) pElement;
		for (; nCount--; p++) p->~T();
	}

	virtual void CopyItems(void* pDest, const void* pSrc, int nCount)
	{
    //TRACE2("CWslObArray: Copying %x, items=%d\n", this, GetSize());
		T* s = (T*) pSrc;
		T* d = (T*) pDest;
		for (;nCount--;) *d++ = *s++;
	}
};

/*****************************************************************************
*
* CWslPtrArray<T> 
* Array of smart pointer at instances of class T
*
*****************************************************************************/

template<class T> class CWslPtrArray : public CWslObArray<CWslPtr<T> >
{
public:
};


