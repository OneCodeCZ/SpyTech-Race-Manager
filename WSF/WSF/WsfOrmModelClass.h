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
#include "WsfOrmModelDependency.h"
#include "WsfOrmModelClassProperty.h"

class CWsfOrmModelClass;
typedef CWslPtr<CWsfOrmModelClass> CWsfOrmModelClassPtr;

/*****************************************************************************/
class WSL_API CWsfOrmModelClass 
{
public:
   CWsfOrmModelClass* m_pParent;    // NULL = parent is CWsfOrmObject
   CString            m_sName;
   CString            m_sTable;
   CRuntimeClass*     m_pRuntimeClass;

   CWslPtrArray<CWsfOrmModelClassProperty> m_aProperties;

   // Relationships of the class (cached for speed)
   CWslPtrArray<CWsfOrmModelDependency> m_aMasterRole;
   CWslPtrArray<CWsfOrmModelDependency> m_aSlaveRole;

   // Top-down inheritance chain cached for speed.
   CWslArray<CWsfOrmModelClass*>  m_aInheritanceChain;  

public:
   CWsfOrmModelClass(const CString& sClassName, 
      CWsfOrmModelClass* pParentClass, 
      const CString& sTableName,
      CRuntimeClass& RuntimeClass);

   void AddProperty(const CString& sColumnName);
   void AddRelationship(const CString& sColumnName, CWsfOrmModelClass& MasterClass, bool bOnDeleteCascade);

   CString GetTableList();
   CString GetJoinCondition();
   CString GetColumnList();
};
