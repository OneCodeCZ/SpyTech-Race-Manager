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
#include "WsfOrmObjectModel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CWsfOrmModel::CWsfOrmModel()
{
}

/*****************************************************************************/
CWsfOrmModelClass* CWsfOrmModel::GetClass(const CString& sName) const
{
   if (sName.IsEmpty()) return NULL;
   CWsfOrmModelClassPtr p;
   return m_mapNameToClass.Lookup(sName, p) ? (CWsfOrmModelClass*)p : NULL;
}

/*****************************************************************************/
CWsfOrmModelClass* CWsfOrmModel::GetClass(CRuntimeClass* pRuntimeClass) const
{
   CWsfOrmModelClassPtr p;
   return m_mapRuntimeClassToClass.Lookup(pRuntimeClass, p) ? (CWsfOrmModelClass*)p : NULL;
}

/*****************************************************************************/
CRuntimeClass* CWsfOrmModel::GetRuntimeClass(const CString& sClass)
{
   CRuntimeClass* pClass;

   // NOTE: Copied from MFC file ARCCORE.CPP

	// Search app specific classes

	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
//	AfxLockGlobals(CRIT_RUNTIMECLASSLIST);
	
   for (pClass=pModuleState->m_classList; pClass != NULL; pClass=pClass->m_pNextClass)
	{
		if (lstrcmpA(sClass, pClass->m_lpszClassName) == 0)
		{
			//AfxUnlockGlobals(CRIT_RUNTIMECLASSLIST);
			return pClass;
		}
	}
//	AfxUnlockGlobals(CRIT_RUNTIMECLASSLIST);

	// Search classes in shared DLLs

#ifdef _AFXDLL
//	AfxLockGlobals(CRIT_DYNLINKLIST);
	for (CDynLinkLibrary* pDLL = pModuleState->m_libraryList; pDLL != NULL;
		pDLL = pDLL->m_pNextDLL)
	{
		for (pClass = pDLL->m_classList; pClass != NULL;
			pClass = pClass->m_pNextClass)
		{
			if (lstrcmpA(sClass, pClass->m_lpszClassName) == 0)
			{
//				AfxUnlockGlobals(CRIT_DYNLINKLIST);
				return pClass;
			}
		}
	}
//	AfxUnlockGlobals(CRIT_DYNLINKLIST);
#endif

   // Class not found!

   //ASSERT(false);
   return NULL;
}

/*****************************************************************************/
void CWsfOrmModel::RemoveAll()
{
   m_mapNameToClass.RemoveAll();
   m_mapRuntimeClassToClass.RemoveAll();
}

/*****************************************************************************/
CWsfOrmModelClass* CWsfOrmModel::AddClass(const CString& sClassName, const CString& sTableName) throw (CException*)
{
   if (GetClass(sClassName)) throw new CWslTextException("CWsfOrmModel::AddClass(): Class '%s' already exists!", sClassName);

   // Get RC

   CRuntimeClass* pRC = GetRuntimeClass(sClassName);
   if (!pRC) throw new CWslTextException("CWsfOrmModel::AddClass(): The class \"%s\" has no CRuntimeClass!", sClassName);

   // Get parent RC and model class

   ASSERT(pRC->m_pfnGetBaseClass);
   CRuntimeClass* pParentRC = pRC->m_pfnGetBaseClass();
   if (!pParentRC) throw new CWslTextException("CWsfOrmModel::AddClass(): The parent class of the class \"%s\" has no CRuntimeClass!", sClassName);

   if (pParentRC == RUNTIME_CLASS(CWsfOrmObject)) pParentRC = NULL;

   CWsfOrmModelClass* pParentClass = NULL;
   
   if (pParentRC) 
   {
      pParentClass = GetClass(pParentRC);
      if (!pParentClass) throw new CWslTextException("CWsfOrmModel::AddClass(): The parent class \"%s\" is not added to the model! Add it first.", pParentRC->m_lpszClassName);
   }

   // Add class

   CWsfOrmModelClassPtr p = new CWsfOrmModelClass(sClassName, pParentClass, sTableName, *pRC);

   m_mapNameToClass.SetAt(sClassName, p);
   m_mapRuntimeClassToClass.SetAt(p->m_pRuntimeClass, p);
  
   return p; 
}

/*****************************************************************************/
void CWsfOrmModel::CheckMapping(CWslDatabase& db) throw (CException*)
{
   // TEST 1-3 for all the model classes proofs that all objects in the DB are properly and completely stored
   // Weak points: It's not detected, if an object doesn't stores information accidentally in other tables than its own
   // down the inheritance path

   POSITION pos = m_mapNameToClass.GetStartPosition();

   while (pos)
   {
      CWsfOrmModelClassPtr pClass;
      CString sName;
      
      m_mapNameToClass.GetNextAssoc(pos, sName, pClass);

      TRACE1("CWsfOrmModel::CheckMapping(): Checking %s...\n", sName);

      // TEST 1: Select an object from the DB to check if its tables and columns are there.
      // This ensures that the mapping is correctly specified for the classes: the DB and the C++ match.

      CWslDbRecordsetPtr rs = db.CreateRecordsetObject();
      rs->Open("SELECT %s FROM %s WHERE %s AND tblOrmObjects.id=0", pClass->GetColumnList(), pClass->GetTableList(), pClass->GetJoinCondition());

      // TEST 2: Check if objects in tblOrmObjects have also a row in their particular tables
      // This ensures that all tblOrmObjects are stored properly

      int nCount, nCount2;

      rs->Open("SELECT COUNT(id) AS obj_count FROM tblOrmObjects WHERE orm_class='%s'", CWslDatabase::Escape(pClass->m_sName));
      rs->GetFieldValue("obj_count", nCount);

      rs->Open("SELECT COUNT(tblOrmObjects.id) AS obj_count FROM %s WHERE %s AND orm_class='%s'", pClass->GetTableList(), pClass->GetJoinCondition(), pClass->m_sName);
      rs->GetFieldValue("obj_count", nCount2);

      if (nCount != nCount2) throw new CWslTextException("CWsfOrmModel::CheckMapping(): TEST #2 for the class %s failed!", pClass->m_sName);

      // TEST 3: Check if all objects in the current class's table have their row in tables up in the inheritance chain
      // This ensures that there is no object which is not present in tblOrmObjects

      rs->Open("SELECT COUNT(id) AS obj_count FROM %s", CWslDatabase::Escape(pClass->m_sTable));
      rs->GetFieldValue("obj_count", nCount);

      rs->Open("SELECT COUNT(tblOrmObjects.id) AS obj_count FROM %s WHERE %s", pClass->GetTableList(), pClass->GetJoinCondition(), pClass->m_sName);
      rs->GetFieldValue("obj_count", nCount2);

      if (nCount != nCount2) throw new CWslTextException("CWsfOrmModel::CheckMapping(): TEST #3 for the class %s failed!", pClass->m_sName);
   }
 
}
