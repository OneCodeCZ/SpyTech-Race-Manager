///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfOrm.h"
#include "WsfOrmCache.h"
#include "WsfOrmObjectModel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define OBJECT_ID_SEQUENCE  "orm_object_id"

/*****************************************************************************
*
* ORM
*
*****************************************************************************/

/*****************************************************************************/
CWsfOrm::CWsfOrm() 
: m_Model(*new CWsfOrmModel()), 
  m_Cache(*new CWsfOrmCache())
{
   m_pDb = NULL;
   m_pTransaction = NULL;
   m_bOpened = false;
}

/*****************************************************************************/
CWsfOrm::~CWsfOrm()
{
   Close();

   delete &m_Model;
   delete &m_Cache;
}

/*****************************************************************************/
void CWsfOrm::Open(CWslDatabase& db) throw (CException*)
{
   if (!db.IsOpened()) throw new CWslTextException("CWsfOrm::Open(): The DB must be opened!");
   if (m_Model.IsEmpty()) throw new CWslTextException("CWsfOrm::Open(): There are no class in the model!");

   Close();

   m_pDb = &db;
   m_bOpened = true;
}

/*****************************************************************************/
void CWsfOrm::Close()
{
   if (!IsOpened()) return;

   if (IsTransaction()) Rollback();
   m_pDb = NULL;

   m_bOpened = false;
}

/*****************************************************************************/
void CWsfOrm::CheckMapping() throw (CException*)
{
   if (!IsOpened()) throw new CWslTextException("The ORM object is not opened!");
   m_Model.CheckMapping(*m_pDb);
}


/*****************************************************************************/
void CWsfOrm::AddClass(const CString& sClassName) throw (CException*)
{
   CRuntimeClass* pClass = m_Model.GetRuntimeClass(sClassName);
   if (!pClass) throw new CWslTextException("CWsfOrm::AddClass(): The class '%s' has no CRuntimeClass!", sClassName);

   CObject* pObj = pClass->CreateObject();
   CWsfOrmObject* pOrmObj = dynamic_cast<CWsfOrmObject*>(pObj);
   
   if (!pOrmObj) 
   {
      delete pObj;
      throw new CWslTextException("CWsfOrm::AddClass(): The class '%s' is not derived from CWsfOrmObject!", sClassName);
   }

   pOrmObj->RegisterPropertiesAndRelationships(*this);
   
   delete pObj;
}


/*****************************************************************************
*
* TRANSACTIONS
*
*****************************************************************************/

/*****************************************************************************/
void CWsfOrm::Begin(CWsfOrmTransaction& Transaction) throw (CException*)
{
   if (!IsOpened()) throw new CWslTextException("The ORM object is not opened!");
   if (m_pTransaction) throw new CWslTextException("CWsfOrm::Begin(): Can't start a new transaction! There is another unfinished transaction in progress.");

//   m_pDb->Execute("BEGIN TRANSACTION");
  
   ASSERT(m_Cache.IsEmpty());
   m_pTransaction = &Transaction;

   TRACE0("--- BEGIN ---------------------------------------------------------------\n");
}

/*****************************************************************************/
void CWsfOrm::Commit() throw (CException*)
{
   ASSERT_VALID(this);
   if (!IsOpened()) throw new CWslTextException("The ORM object is not opened!");
   if (!m_pTransaction) throw new CWslTextException("CWsfOrm::Commit(): Can't commit! There is no transaction in progress.");

   m_Cache.Save(*m_pDb);
   m_Cache.Empty();
   
   m_pDb->Commit();
   
   TRACE1("Transaction duration: %d ms\n", m_pTransaction->m_dwDurationInMS);
   TRACE0("--- COMMIT ---------------------------------------------------------------\n");
   
   m_pTransaction = NULL;
}

/*****************************************************************************/
void CWsfOrm::Rollback()
{
   if (!IsOpened()) throw new CWslTextException("The ORM object is not opened!");
   if (!IsTransaction()) throw new CWslTextException("CWsfOrm::Commit(): Can't roll back! There is no transaction in progress.");

   m_Cache.Empty();
   
   try
   {
      m_pDb->Rollback();
   }
   catch (CWslException* e)
   {
      e->Delete();
      TRACE0("CWsfOrm::Rollback(): Exception ignored...\n");
   }

   TRACE0("--- ROLLBACK ---------------------------------------------------------------\n");
   m_pTransaction = NULL;

#ifdef _DEBUG
   Beep(1000,50);
   Beep(500,50);
   Beep(250,50);
#endif
}

/*****************************************************************************
*
* IDs
*
*****************************************************************************/

/*****************************************************************************/
int CWsfOrm::GetNextId(const char* pcszSequence) throw (CException*)
{
   if (!IsOpened()) throw new CWslTextException("The ORM object is not opened!");
   
   int n;
   CString s = CWslDatabase::Escape(pcszSequence);
   CWslDbRecordsetPtr rs = m_pDb->CreateRecordsetObject();
   
   rs->Open("SELECT * FROM tblOrmIDs WHERE name='%s' FOR UPDATE", s);
   ASSERT(!rs->IsEOF());
   rs->GetFieldValue("next_id", n);
   rs->Close();

   m_pDb->Execute("UPDATE tblOrmIDs SET next_id=next_id+1 WHERE name='%s'", s);
   return n;
}


/*****************************************************************************
*
* OBJECT MANAGEMENT
*
*****************************************************************************/

/*****************************************************************************/
CWsfOrmObjectPtr CWsfOrm::CreateObject(CRuntimeClass* pClass) throw (CException*)
{
   if (!IsOpened()) throw new CWslTextException("The ORM object is not opened!");
   ASSERT(pClass);
   
   if (!m_Model.GetClass(pClass)) 
      throw new CWslTextException("[] CWsfOrm::CreateObject(): Class '%s' is not in the object model!", pClass->m_lpszClassName);
   
   // Create the object

   CWslPtr<CWsfOrmObject> pObj = (CWsfOrmObject*) pClass->CreateObject();
   ASSERT(pObj);

   pObj->Init(*this, GetNextId(OBJECT_ID_SEQUENCE));

   // Create the ORM pointer and add it to the cache

   CWsfOrmObjectPtr p;
   p.Attach(pObj);
   pObj.Detach();

   m_Cache.Add(p);

   // Done

   return p;
}

/*****************************************************************************/
CWsfOrmObjectPtr CWsfOrm::GetObject(CWslDbRecordset& rs, 
                                    CRuntimeClass* pClass) throw (CException*)
{
   if (!IsOpened()) throw new CWslTextException("CWsfOrm::GetObject(): The ORM object is not opened!");
   if (!IsTransaction()) throw new CWslTextException("CWsfOrm::GetObject(): No transaction has been started!");

   // Try the cache

   int nId;
   rs.GetFieldValue("id", nId);
   CWsfOrmObjectPtr OrmPtr = m_Cache.Get(nId);
   if (OrmPtr != NULL) return OrmPtr;

   // Load it
   // Get/check object's class

   CString sClass;
   rs.GetFieldValue("orm_class", sClass);

   if (!pClass)
   {
      pClass = m_Model.GetRuntimeClass(sClass);
      if (!pClass) throw new CWslTextException("CWsfOrm::GetObject(): Class '%s' has no CRuntimeClass!", sClass);;
   }
   else
   {
      if (pClass->m_lpszClassName != sClass)
      {
         throw new CWslTextException("[] CWsfOrm::LoadObject(): Object's C++ class '%s' doesn't match the object's ORM class '%s'!", 
            pClass->m_lpszClassName,
            sClass);
      }
   }

   if (!m_Model.GetClass(pClass)) 
      throw new CWslTextException("[] CWsfOrm::LoadObject(): Class '%s' is not in the object model!", pClass->m_lpszClassName);

   // Create the object

   CWslPtr<CWsfOrmObject> pObj = (CWsfOrmObject*) pClass->CreateObject();
   ASSERT(pObj);
   pObj->Init(*this, nId);

   // Load properties

   pObj->Load(rs);

   // Create the ORM pointer and add it to the cache

   OrmPtr.Attach((CWsfOrmObject*) pObj); 
   pObj.Detach();

   m_Cache.Add(OrmPtr);

   // Done

   return OrmPtr;
}

/*****************************************************************************/
CWsfOrmObjectPtr CWsfOrm::GetObject(int nId, 
                                    CRuntimeClass* pClass) 
                                    throw (CException*)
{
   if (!IsOpened()) throw new CWslTextException("CWsfOrm::GetObject(): The ORM object is not opened!");
   if (!IsTransaction()) throw new CWslTextException("CWsfOrm::GetObject(): No transaction has been started!");

   if (!nId) return NULL;

   // Try the cache

   CWsfOrmObjectPtr OrmPtr = m_Cache.Get(nId);
   if (OrmPtr != NULL) return OrmPtr;

   // Load object
   // Get object's class

   if (!pClass)
   {
      CString sClass;
      CWslDbRecordsetPtr rs = m_pDb->CreateRecordsetObject();
      rs->Open("SELECT orm_class FROM tblOrmObjects WHERE id=%d", nId);
      if (rs->IsEOF()) return NULL;
      rs->GetFieldValue("orm_class", sClass);
      pClass = m_Model.GetRuntimeClass(sClass);
      if (!pClass) throw new CWslTextException("[] CWsfOrm::LoadObject(): Class '%s' has no CRuntimeClass!", sClass);;
   }

   if (!m_Model.GetClass(pClass)) 
      throw new CWslTextException("[] CWsfOrm::LoadObject(): Class '%s' is not in the object model!", pClass->m_lpszClassName);

   // Create the object

   CWslPtr<CWsfOrmObject> pObj = (CWsfOrmObject*) pClass->CreateObject();
   ASSERT(pObj);
   pObj->Init(*this, nId);
      
   // Load it

   if (!pObj->Load(*m_pDb)) return NULL;

   // Create the ORM pointer and add it to the cache

   OrmPtr.Attach((CWsfOrmObject*) pObj); 
   pObj.Detach();

   m_Cache.Add(OrmPtr);

   // Done

   return OrmPtr;
}

/*****************************************************************************/
void CWsfOrm::SaveObjects() throw (CException*)
{
   if (!IsOpened()) throw new CWslTextException("The ORM object is not opened!");
   m_Cache.Save(*m_pDb);
}


/*****************************************************************************
*
* QUERIES
*
*****************************************************************************/

/*****************************************************************************/
void CWsfOrm::PrepareForQuery() throw (CException*)
{
   SaveObjects();
}
