///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  Smart Pointer
// | P |  /  |  |  ~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 11
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

// Ignore the following warning when
// a built in type is used that doesn't support -> operator
// not a UDT or reference to a UDT.  Will produce errors if applied using infix notation
#pragma warning( disable : 4284)

// NOTE: Beware of cycles in the graph of object dependencies 
// (= they "smart" point at each other)! In such a case, deleting the your
// primary smart pointers at these objects does NOT delete the objects!

/*****************************************************************************/
template<class T> class CWslPtr
{
protected:
	struct CData
	{
		T*	pObject;
		int	nRefCount;
		BOOL bDetached;

		CData(T* p) { pObject = p; nRefCount = 1; bDetached = false; }
		~CData() { if (!bDetached) delete pObject; }
	};

protected:
	CData*	pData; 

public:
	CWslPtr() { pData = NULL; }
	CWslPtr(T* pObject) { pData = new CData(pObject); }

	/*****************************************************************************/
	CWslPtr(const CWslPtr<T>& p)
	{
		pData = p.pData;
		if (pData) pData->nRefCount++;
	}

	/*****************************************************************************/
	~CWslPtr() 
	{ 
		if (pData && --pData->nRefCount==0) delete pData; 
	}

	/*****************************************************************************/
	CWslPtr<T>& operator =(const CWslPtr<T>& p) 
	{ 
		if (pData == p.pData) return *this;
		if (pData && --pData->nRefCount==0) delete pData;
		pData = p.pData;
		pData->nRefCount++;
		return *this; 
	}

	/*****************************************************************************/
	T* operator =(T* pObject) 
	{ 
		if (pData && --pData->nRefCount==0) delete pData; 
		pData = new CData(pObject); 
		return pObject; 
	}
	
	/*****************************************************************************/
	T& operator *() 
	{ 
		ASSERT(pData);
		return *pData->pObject; 
	}

	/*****************************************************************************/
	T* operator ->() const
	{ 
		ASSERT(pData); 
		ASSERT(pData->pObject); 
		return pData->pObject; 
	}

	/*****************************************************************************/
	operator T*() 
	{ 
		return pData ? pData->pObject : NULL; 
	}

	/*****************************************************************************/
	operator const T*() const
	{ 
		return pData ? pData->pObject : NULL; 
	}

	/*****************************************************************************/
	T* Detach()
	{
		if (pData) { pData->bDetached = true; return pData->pObject; }
		return NULL;
	}
};








