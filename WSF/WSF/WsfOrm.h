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

#pragma once

#include <db/wsldb.h>

class CWsfOrmObject;
class CWsfOrmTransaction;
class CWsfOrmObjectPtr;

/*****************************************************************************/
class WSL_API CWsfOrm : public CObject
{
private:
   CWslDatabase*        m_pDb;
   class CWsfOrmCache&  m_Cache;   
   class CWsfOrmModel&  m_Model;
   CWsfOrmTransaction*  m_pTransaction;
   BOOL                 m_bOpened;

public:
   CWsfOrm();
   virtual ~CWsfOrm();

   // Init the ORM
   void Open(CWslDatabase& db) throw (CException*);
   void Close();
   BOOL IsOpened() const { return m_bOpened; }

   void AddClass(const CString& sClassName) throw (CException*);

   // The DB used to store the objects
   CWslDatabase* GetDb() { return m_pDb; }

   // Checks if the mapping is valid (tables, names) and if all stored objects are valid.
   void CheckMapping() throw (CException*);

   //---------------------------------------------------------------------------
   // IDs
   //---------------------------------------------------------------------------

   // Return the next unique ID in the given sequence.
   int GetNextId(const char* pcszSequence) throw (CException*);

   //---------------------------------------------------------------------------
   // Object management
   //---------------------------------------------------------------------------

   // Creates a new object of the given class.
   // NOTE: For each user object create a CreateUserObject() method, which
   // uses this method to create the object and then constructs the object properly. 
   // After constructed, call object's SetConstructed() method.
   CWsfOrmObjectPtr CreateObject(CRuntimeClass* pClass) throw (CException*);

   // Loads an object with the specified ID from the DB
   // If you don't specify the class, it is determinated automatically (1 more SELECT operation).
   CWsfOrmObjectPtr GetObject(int nId, CRuntimeClass* pClass = NULL) throw (CException*);

   // Loads an object from a recordset. It must contain 
   // columns from the tblOrmObjects table and columns from the object's tables.
   // If you don't specify the class, it is determinated automatically (1 more SELECT operation).
   CWsfOrmObjectPtr GetObject(CWslDbRecordset& rs, CRuntimeClass* pClass = NULL) throw (CException*);

   //---------------------------------------------------------------------------
   // Transaction
   //---------------------------------------------------------------------------

   void Begin(CWsfOrmTransaction& Transaction) throw (CException*);
   void Commit() throw (CException*);
   void Rollback();

   BOOL IsTransaction() const { return m_pTransaction != NULL; }

   //---------------------------------------------------------------------------
   // Queries
   //---------------------------------------------------------------------------

   // Must be called before any query on the DB.
   // It saves the changed objects to the DB in order to query
   // the latest data.
   void PrepareForQuery() throw (CException*);

   //---------------------------------------------------------------------------
   // Internals
   //---------------------------------------------------------------------------

   // Saves all modified objects to the DB
   void SaveObjects() throw (CException*);

   // Object cache (in-memory objects)
   CWsfOrmCache& GetCache() { return m_Cache; }

   // Object model
   CWsfOrmModel& GetModel() { return m_Model; }
};


/*****************************************************************************/
#include "WsfOrmProperty.h"
#include "WsfOrmObject.h"
#include "WsfOrmPtr.h"
#include "WsfOrmCollection.h"
#include "WsfOrmTransaction.h"
#include "WsfOrmObjects.h"

#include "WsfOrmModelClass.h"
#include "WsfOrmModelClassProperty.h"
#include "WsfOrmObjectModel.h"

