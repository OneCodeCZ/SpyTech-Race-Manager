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
#include "WsfOrmObjects.h"
#include "WsfOrmObjectModel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/*****************************************************************************/
CWsfOrmObjects::CWsfOrmObjects(CWsfOrm& orm, CRuntimeClass* pClass)
: CWsfOrmCollection<CWsfOrmObjectPtr>(orm)
{
   ASSERT(pClass);
   m_pClass = pClass;
}

/*****************************************************************************/
CWsfOrmObjects::~CWsfOrmObjects()
{
}

/*****************************************************************************/
void CWsfOrmObjects::OnLoad(CWslDatabase& db) throw (CException*)
{
   CWslDbRecordsetPtr rs = db.CreateRecordsetObject();
   CWsfOrmModelClass* pClass = GetOrm().GetModel().GetClass(m_pClass);
  
   rs->Open("SELECT tblOrmObjects.id FROM %s WHERE %s ORDER BY tblOrmObjects.id ASC",
      pClass->GetTableList(),
      pClass->GetJoinCondition());

   for (; !rs->IsEOF(); rs->MoveNext())
   {
      int n;
      rs->GetFieldValue("id", n);
      Append(GetOrm().GetObject(n));
   }   
}
