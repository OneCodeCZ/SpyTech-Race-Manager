///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 05
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

// The CWsfDispatchPtr class provides:
// - pointer to a CCmdTarget derived class
// - usage counting based on CCmdTarget's IUnknown usage counting

/*****************************************************************************/
template<class C> class CWsfDispatchPtr
{
protected:
   C* m_pObject;
   CComPtr<IDispatch> m_pIDispatch;

public:   
   //---------------------------------------------------------------------------
   // Construction
   //---------------------------------------------------------------------------

   CWsfDispatchPtr() { m_pObject = NULL; }

   // From CWsfDispatchPtr (copy constructor)
   CWsfDispatchPtr(const CWsfDispatchPtr<C>& p) { operator =(p); }

   // From plain object without increasing the reference count
   CWsfDispatchPtr(C* p) { Attach(p); }

   //---------------------------------------------------------------------------
   // Dereferencing
   //---------------------------------------------------------------------------

   // Get the object.
	C* operator ->() throw (CException*)
	{ 
      if (!m_pObject) throw new CWslTextException("CWsfDispatchPtr: Can't dereference a NULL pointer!");
      return m_pObject;
	}

   C& operator *() throw (CException*)     { return * operator ->(); }
   operator C*() throw (CException*)       { return m_pObject; }

   //---------------------------------------------------------------------------
   // Assigning
   //---------------------------------------------------------------------------

   // Assigning a NULL value
   CWsfDispatchPtr<C>& operator =(int nNULL)
   {
      m_pObject = NULL;
      m_pIDispatch = NULL;
      return *this;
   }

   // Assigning an ORM object
   CWsfDispatchPtr<C>& operator =(const CWsfDispatchPtr<C>& p)
   {
      if (*this != p)
      {
         m_pObject = p.m_pObject;
         m_pIDispatch = p.m_pIDispatch;
      }
      return *this;
   }

   // From plain object without increasing the reference count
   void Attach(C* p) 
   { 
      m_pObject = p;

      if (p) 
         m_pIDispatch.Attach(p->GetIDispatch(false));
      else 
         m_pIDispatch = NULL;
   }

   //---------------------------------------------------------------------------
   // Various
   //---------------------------------------------------------------------------

   // Gets the IDispatch* and increases the usage count.
   IDispatch* GetIDispatch() 
   { 
      IDispatch* p;
   
      if (m_pIDispatch.CopyTo(&p) != S_OK)
         throw new CWslTextException("CWsfDispatchPtr: Error getting the IDispatch pointer!");
      
      return p;
   }

   // Comparing
   bool operator ==(const CWsfDispatchPtr<C>& p) { return m_pObject == p.m_pObject; }
   bool operator !=(const CWsfDispatchPtr<C>& p) { return m_pObject != p.m_pObject; }
};
