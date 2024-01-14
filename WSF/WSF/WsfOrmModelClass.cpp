///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  SpyTech Application Framework
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2000-2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2004 04
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "main.h"
#include "WsfOrm.h"
#include "WsfOrmModelClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CWsfOrmModelClass::CWsfOrmModelClass(const CString& sClassName, 
                                     CWsfOrmModelClass* pParentClass, 
                                     const CString& sTableName,
                                     CRuntimeClass& RuntimeClass)
{
   m_sName = sClassName;
   m_pParent = pParentClass;
   m_sTable = sTableName;
   m_pRuntimeClass = &RuntimeClass;

   // Cache the inheritance chain

   for (CWsfOrmModelClass* u=this; u; u=u->m_pParent) 
   {
      m_aInheritanceChain.InsertAt(0, u);
   }
}

/*****************************************************************************/
CString CWsfOrmModelClass::GetTableList()
{
   CString sTbls, s; 
   CWsfOrmModelClass* p;
   int i;
  
   for (p=this,i=1; p; p=p->m_pParent, i++)
   {
      s.Format("%c%s t%d", (i>1 ? ',':' '), CWslDatabase::Escape(p->m_sTable), i);
      sTbls += s;
   }

   sTbls += ", tblOrmObjects";

   return sTbls;
}

/*****************************************************************************/
CString CWsfOrmModelClass::GetJoinCondition()
{
   CString sCond, s; 
   CWsfOrmModelClass* p;
   int i;

   for (p=this,i=1; p; p=p->m_pParent, i++)
   {
      if (i > 1) sCond += " AND ";
      s.Format("t%d.id=tblOrmObjects.id", i);
      sCond += s;
   }

   return sCond;
}

/*****************************************************************************/
CString CWsfOrmModelClass::GetColumnList()
{
   CString s; 
   CWsfOrmModelClass* p;
  
   for (p=this; p; p=p->m_pParent)
   {
      for (int i=0; i<p->m_aProperties.GetSize(); i++)
      {
         if (!s.IsEmpty()) s += ',';
         s += p->m_aProperties[i]->m_sColumnName;
      }
   }

   return s;
}

/*****************************************************************************/
void CWsfOrmModelClass::AddProperty(const CString& sColumnName)
{
   m_aProperties += new CWsfOrmModelClassProperty();
   m_aProperties.GetLast()->m_sColumnName = sColumnName;
}

/*****************************************************************************/
void CWsfOrmModelClass::AddRelationship(const CString& sColumnName, 
                                        CWsfOrmModelClass& MasterClass, 
                                        bool bOnDeleteCascade)
{
   // Add property
   
   m_aProperties += new CWsfOrmModelClassProperty();
   m_aProperties.GetLast()->m_sColumnName = sColumnName;

   // Add relationship

   CWsfOrmModelDependencyPtr p = new CWsfOrmModelDependency();
   p->m_sColumn = sColumnName;
   p->m_bOnDeleteSetNull = !bOnDeleteCascade;
   p->m_pSlaveClass = this;
   p->m_pMasterClass = &MasterClass;

   m_aSlaveRole += p;
   MasterClass.m_aMasterRole += p;
}
