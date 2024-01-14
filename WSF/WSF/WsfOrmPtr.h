///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 12
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

// Pointer to CWsfOrmObject objects and 
// to objects derived from CWsfOrmObject class.
// - usage counting
// - polymorphism 
// - lazy loading
// - automation

// NOTE: When property's object is OFFLINE, m_pOrm is INVALID!!! To be fixed sometime.

/*****************************************************************************/
class CWsfOrmObjectPtr : public IWsfOrmPropertyPersistance
{
protected:
   CWsfOrmInt  m_nId;
   CWsfOrm*    m_pOrm;
   CWsfOrmObject* m_pObject;
   CComPtr<IDispatch> m_pIDispatch;

public:
   //---------------------------------------------------------------------------
   // Construction
   //---------------------------------------------------------------------------
   
   // Default and from NULL
   CWsfOrmObjectPtr(int nNULL=0) 
   { 
      ASSERT(nNULL==NULL); 
      m_pOrm = NULL;
      m_pObject = NULL;
   }

   // From CWsfOrmObjectPtr (copy constructor)
   CWsfOrmObjectPtr(const CWsfOrmObjectPtr& p) { operator =(p); }

   // From ID or from NULL
   CWsfOrmObjectPtr(int nId, CWsfOrm& orm) 
   { 
      m_pOrm = &orm; 
      m_pObject = NULL;
      if (nId) m_nId = nId;
   }

   // From IDispatch*
   CWsfOrmObjectPtr(IDispatch* p) { operator =(p); }

   //---------------------------------------------------------------------------
   // State
   //---------------------------------------------------------------------------

   BOOL IsNull() const { return m_nId.IsNull(); }

   BOOL IsLoaded() const { return m_pObject != NULL; }
   BOOL ToBeLoaded() const { return !IsNull() && !IsLoaded(); }

   int GetId() const { return m_nId; }

   //---------------------------------------------------------------------------
   // Dereferencing
   //---------------------------------------------------------------------------

   // Get object. 
   // If it's not loaded, load it.
	CWsfOrmObject* operator ->() throw (CException*)
	{ 
      if (m_pObject) 
      {
         ASSERT_VALID(m_pObject);
         return m_pObject;
      }
      if (!m_nId) throw new CWslTextException("CWsfOrmObjectPtr: Can't dereference a NULL pointer!");

      TRACE1("CWsfOrmObjectPtr: Lazy loading object %d.\n", (int) m_nId);
      CWsfOrmObjectPtr p = m_pOrm->GetObject(m_nId);
      m_pObject = p.m_pObject;
      m_pIDispatch = p.m_pIDispatch;

      return (CWsfOrmObject*) m_pObject;
	}

	CWsfOrmObject& operator *() throw (CException*) { return * operator ->(); }

   //---------------------------------------------------------------------------
   // Assigning
   //---------------------------------------------------------------------------

   // Assigning a NULL value
   CWsfOrmObjectPtr& operator =(int nNULL)
   {
      m_nId.SetNull();
      m_pObject = NULL;
      m_pIDispatch = NULL;
      return *this;
   }

   // Assigning an ORM object
   CWsfOrmObjectPtr& operator =(const CWsfOrmObjectPtr& p)
   {
      if (*this == p) return *this;

      m_pOrm = p.m_pOrm;
      m_nId = p.m_nId;
      m_pObject = p.m_pObject;
      m_pIDispatch = p.m_pIDispatch;

      // If the object is to be lazy loaded, the ORM must be specified!
      ASSERT(!ToBeLoaded() || m_pOrm);

      return *this;
   }

   // From plain object without increasing the reference count
   void Attach(CWsfOrmObject* p) 
   { 
      m_pOrm = NULL;
      m_pObject = p;

      if (p) 
      {
         ASSERT(p->GetId());
         m_nId = p->GetId();
         m_pIDispatch.Attach(p->GetIDispatch(false));
      }
      else 
      {
         m_nId.SetNull();
         m_pIDispatch = NULL;
      }
   }

   //---------------------------------------------------------------------------
   // Comparsion
   //---------------------------------------------------------------------------

   BOOL operator ==(int nNULL) const { ASSERT(nNULL==NULL); return m_nId.IsNull(); }
   BOOL operator ==(const CWsfOrmObjectPtr& p) const  { return m_nId == p.m_nId; }
   BOOL operator !=(int nNULL) const { ASSERT(nNULL==NULL); return !m_nId.IsNull(); }
   BOOL operator !=(const CWsfOrmObjectPtr& p) const  { return m_nId != p.m_nId; }

   //---------------------------------------------------------------------------
   // IWsfOrmPropertyPersistance
   //---------------------------------------------------------------------------

   virtual void Load(CWslDbRecordset& rs, const CString& sColumn, CWsfOrm& Orm) throw (CException*)
   {
      ASSERT(m_pObject == NULL);
      m_nId.Load(rs, sColumn, Orm);
      m_pOrm = &Orm;    // for lazy loading
   }

   virtual void Save(CWslDbCommand& c, const CString& sColumn) throw (CException*)
   {
      m_nId.Save(c, sColumn);
   }

   //---------------------------------------------------------------------------
   // Automation
   //---------------------------------------------------------------------------

   IDispatch* GetIDispatch() { return IsNull() ? NULL : operator ->()->GetIDispatch(true); }
   
   void operator =(IDispatch* p)
   {
      m_pObject = dynamic_cast<CWsfOrmObject*>(CCmdTarget::FromIDispatch(p));
      
      if (m_pObject)
      {
         ASSERT_VALID(m_pObject);
         m_nId = m_pObject->GetId();
         m_pOrm = &m_pObject->GetOrm();
         ASSERT_VALID(m_pOrm);
         m_pIDispatch = p;
      }
      else
      {
         m_nId.SetNull();
         m_pOrm = NULL;
         m_pIDispatch = NULL;
      }
   }
};


/*****************************************************************************
*
* CWsfOrmPtr
*
*****************************************************************************/

template<class C> class CWsfOrmPtr : public CWsfOrmObjectPtr
{
public:
   //---------------------------------------------------------------------------
   // Construction
   //---------------------------------------------------------------------------

   CWsfOrmPtr(int nNULL=0)                : CWsfOrmObjectPtr(nNULL)   { }
   CWsfOrmPtr(int nId, CWsfOrm& orm)      : CWsfOrmObjectPtr(nId, orm)  { }
   CWsfOrmPtr(const CWsfOrmObjectPtr& p)  : CWsfOrmObjectPtr(p)         { }
   CWsfOrmPtr(const CWsfOrmPtr<C>& p)     : CWsfOrmObjectPtr(p)         { }
   CWsfOrmPtr(IDispatch* p)               : CWsfOrmObjectPtr(p)         { }

   //---------------------------------------------------------------------------
   // Dereferencing
   //---------------------------------------------------------------------------

   // Get the object. Type safe.
	C* operator ->() throw (CException*)
	{ 
      CWsfOrmObject* p = CWsfOrmObjectPtr::operator ->();
      
      if (!dynamic_cast<C*>(p))
         throw new CWslTextException("CWsfOrmPtr<C>: Can't dereference, class type mismatch!");

      return (C*) p;
	}

   C& operator *() throw (CException*)     { return * operator ->(); }
   operator C*() throw (CException*)       { return IsNull() ? NULL : operator ->(); }

   //---------------------------------------------------------------------------
   // Assigning
   // (they are not inherited from parent -> call parent)
   //---------------------------------------------------------------------------

   CWsfOrmPtr<C>& operator =(int nNULL)                 { return (CWsfOrmPtr<C>&) CWsfOrmObjectPtr::operator =(nNULL); }
   CWsfOrmPtr<C>& operator =(const CWsfOrmObjectPtr& p) { return (CWsfOrmPtr<C>&) CWsfOrmObjectPtr::operator =(p); }
   CWsfOrmPtr<C>& operator =(const CWsfOrmPtr<C>& p)    { return (CWsfOrmPtr<C>&) CWsfOrmObjectPtr::operator =(p); }
   void operator =(IDispatch* p)                        { CWsfOrmObjectPtr::operator =(p); }
};
