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

#include "stdafx.h"
#include "main.h"
#include "WsfOrm.h"
#include "WsfOrmTransaction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CWsfOrmTransaction::CWsfOrmTransaction(CWsfOrm& Orm) : m_Orm(Orm)
{
   ASSERT_VALID(&m_Orm);
   m_bFinished = true;
}

/*****************************************************************************/
CWsfOrmTransaction::~CWsfOrmTransaction()
{
   Rollback();
}

/*****************************************************************************/
void CWsfOrmTransaction::Begin() throw (CException*)
{
   ASSERT(m_bFinished);

   m_Orm.Begin(*this);
   
   m_dwDurationInMS = GetTickCount();
   m_bFinished = false;
}

/*****************************************************************************/
void CWsfOrmTransaction::Commit() throw (CException*)
{
   if (!m_bFinished) 
   {
      m_dwDurationInMS = GetTickCount() - m_dwDurationInMS;
      m_Orm.Commit();
      m_bFinished = true;
   }
}

/*****************************************************************************/
void CWsfOrmTransaction::Rollback()
{
   if (!m_bFinished) 
   {
      m_dwDurationInMS = GetTickCount() - m_dwDurationInMS;
      m_Orm.Rollback();
      m_bFinished = true;
   }
}
