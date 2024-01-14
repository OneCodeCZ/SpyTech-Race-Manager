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
#include "WsfOrmObject.h"
#include "WsfOrmCache.h"
#include "WsfOrmObjectModel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CWsfOrmObject, CCmdTarget, 1)

/*****************************************************************************/
CWsfOrmObject::CWsfOrmObject()
{ 
   m_nId = 0; 
   m_pOrm = NULL;
   m_eState = CPP_CONSTRUCTED;
}

/*****************************************************************************/
void CWsfOrmObject::Init(CWsfOrm& Orm, int nId)
{
   ASSERT(m_eState==CPP_CONSTRUCTED);
   ASSERT(nId);

   m_nId = nId;
   m_pOrm = &Orm;

   RegisterPropertiesAndRelationships(Orm);
}

/*****************************************************************************/
void CWsfOrmObject::Constructed() throw (CException*) 
{ 
   ASSERT(m_eState == CPP_CONSTRUCTED);
   ASSERT(!m_aClasses.IsEmpty());
   ASSERT(m_nId);

   m_eState = CREATED; 

   ASSERT(m_aClasses.GetSize() == GetModelClass()->m_aInheritanceChain.GetSize());


   // Create the object in the DB now to lock the object's table
   Save();
}

/*****************************************************************************/
void CWsfOrmObject::AddClass()
{
   ASSERT(m_eState == CPP_CONSTRUCTED);

   m_aClasses += new CRuntimeClassInfo();
}

/*****************************************************************************/
void CWsfOrmObject::AddProperty(IWsfOrmPropertyPersistance& PropertyPersistance)
{
   ASSERT(m_eState == CPP_CONSTRUCTED);
   ASSERT(!m_aClasses.IsEmpty());

   m_aClasses.GetLast()->m_aProperties += &PropertyPersistance;
}

/*****************************************************************************/
CWsfOrmObjectPtr CWsfOrmObject::GetPtr() throw (CException*)
{
   // NOTE: You cannot use this method in Construct() method
   ASSERT(IsValid());      
   // Will be taken from the cache
   return m_pOrm->GetObject(m_nId);
}

/*****************************************************************************/
CWsfOrmModelClass* CWsfOrmObject::GetModelClass()
{
   ASSERT(m_pOrm);
   CWsfOrmModelClass* p;
   
   p = m_pOrm->GetModel().GetClass(GetRuntimeClass()->m_lpszClassName);
   ASSERT(p);
   
   return p;
}

/*****************************************************************************/
void CWsfOrmObject::MakeOffline()
{
   ASSERT(IsValid());

   m_pOrm = NULL;
   m_eState = OFFLINE;
}

/*****************************************************************************/
void CWsfOrmObject::Load(CWslDbRecordset& rs) throw (CException*)
{
   ASSERT(m_eState == CPP_CONSTRUCTED);
   ASSERT(m_pOrm);
   ASSERT(m_nId);

   CWsfOrmModelClass* pModelClass = GetModelClass();
   ASSERT(pModelClass);

   for (int i=0; i<m_aClasses.GetSize(); i++)
   {
      m_aClasses[i]->Load(rs, *pModelClass->m_aInheritanceChain[i], *m_pOrm);
   }

   m_eState = LOADED;
}

/*****************************************************************************/
BOOL CWsfOrmObject::Load(CWslDatabase& db) throw (CException*)
{
   ASSERT(m_eState == CPP_CONSTRUCTED);
   ASSERT(m_pOrm);
   ASSERT(m_nId);

   // Load object

   CString s;
   s.Format("SELECT * FROM %s WHERE %s AND tblOrmObjects.id=%d FOR UPDATE",
      GetModelClass()->GetTableList(),
      GetModelClass()->GetJoinCondition(),
      GetId());

   CWslDbRecordsetPtr rs = db.CreateRecordsetObject();
   ASSERT(rs);
   rs->OpenNoFormat(s);
   if (rs->IsEOF()) return false;    // doesn't exist

   // Check class

   rs->GetFieldValue("orm_class", s);
 
   if (GetRuntimeClass()->m_lpszClassName != s)
   {
      throw new CWslTextException("[] CWsfOrmObject::Load(): Object's C++ class '%s' doesn't match the object's ORM class '%s'!", 
         GetRuntimeClass()->m_lpszClassName,
         s);
   }

   // Init class in the object's inheritance chain

   CWsfOrmModelClass* pModelClass = GetModelClass();
   ASSERT(pModelClass);

   for (int i=0; i<m_aClasses.GetSize(); i++)
   {
      m_aClasses[i]->Load(*rs, *pModelClass->m_aInheritanceChain[i], *m_pOrm);
   }

   // Done

   m_eState = LOADED;
   return true;
}

/*****************************************************************************/
void CWsfOrmObject::Save() throw (CException*)
{
   ASSERT(m_pOrm);
   ASSERT(IsValid());

   // Save ORM object info

   if (IsNew())   
   {
      CWslDbCommand c;
      c.AddCol("orm_class", GetRuntimeClass()->m_lpszClassName);
      c.BuildInsert("tblOrmObjects", "id", GetId());
      m_pOrm->GetDb()->Execute(c);
   }
   
   // Save all classes on the inheritance path starting at the root

   CWsfOrmModelClass* pClass = GetModelClass();
   ASSERT(pClass);

   for (int i=0; i<pClass->m_aInheritanceChain.GetSize(); i++)
   {
      CWslDbCommand c;
      m_aClasses[i]->Save(c, *pClass->m_aInheritanceChain[i]);
      c.BuildSave(IsNew(), pClass->m_aInheritanceChain[i]->m_sTable, "id", GetId());
      m_pOrm->GetDb()->Execute(c);
   }

   // If the object was created in memory (CREATED) and this is the first time
   // it is saved, mark it as loaded (=present in the db)

   m_eState = LOADED;
}

/*****************************************************************************/
void CWsfOrmObject::Delete() throw (CException*)
{
   ASSERT(m_pOrm);
   ASSERT(IsValid());

   // NOTE: not checked for recursive dependency!!!

   // Delete dependent objects (slaves) of all classes (masters)
   // in the inheritance chain

   CWsfOrmModelClass* pMaster = GetModelClass();
   ASSERT(pMaster);

   for (; pMaster; pMaster=pMaster->m_pParent)
   {
      for (int i=0; i<pMaster->m_aMasterRole.GetSize(); i++)
      {
         // ON DELETE SET NULL will be done automatically by the SQL engine
         if (pMaster->m_aMasterRole[i]->m_bOnDeleteSetNull) continue;

         // Delete all slave objects pointing at this master object

         CWsfOrmModelClass* pSlave = pMaster->m_aMasterRole[i]->m_pSlaveClass;

         CString s;
         s.Format("SELECT tblOrmObjects.id FROM %s WHERE %s AND %s=%d FOR UPDATE",
            pSlave->GetTableList(),
            pSlave->GetJoinCondition(),
            pMaster->m_aMasterRole[i]->m_sColumn,
            GetId());
 
         CWslDbRecordsetPtr rs = GetOrm().GetDb()->CreateRecordsetObject();
  
         for (rs->Open(s); !rs->IsEOF(); rs->MoveNext())
         {
            int nId;
            rs->GetFieldValue("id", nId);
            GetOrm().GetObject(nId)->Delete();
         }   
      }
   }

   // Delete this object in DB

   TRACE2("CWsfOrmObject::Delete(): Deleting id=%d, class=%s\n", GetId(), GetRuntimeClass()->m_lpszClassName);
   m_pOrm->GetDb()->Execute("DELETE FROM tblOrmObjects WHERE id=%d", GetId());
   
   // Invalidate memory object

   int nId = m_nId;
   CWsfOrm* pOrm = m_pOrm;

   m_nId = 0;
   m_eState = DELETED;
   m_pOrm = NULL;
   
   // Remove from the cache

   pOrm->GetCache().Remove(nId);
   // NOTE: This object can be deleted here!
}


/*****************************************************************************
*
* CWsfOrmObject::CRuntimeClassInfo
*
*****************************************************************************/

/*****************************************************************************/
void CWsfOrmObject::CRuntimeClassInfo::Load(CWslDbRecordset& rs, 
                                            CWsfOrmModelClass& ModelClass,
                                            CWsfOrm& Orm) throw (CException*)
{
   for (int i=0; i<m_aProperties.GetSize(); i++)
   {
      m_aProperties[i]->Load(rs, ModelClass.m_aProperties[i]->m_sColumnName, Orm);
   }
}

/*****************************************************************************/
void CWsfOrmObject::CRuntimeClassInfo::Save(CWslDbCommand& cmd,
                                            CWsfOrmModelClass& ModelClass) throw (CException*)
{
   for (int i=0; i<m_aProperties.GetSize(); i++)
   {
      m_aProperties[i]->Save(cmd, ModelClass.m_aProperties[i]->m_sColumnName);
   }
}

