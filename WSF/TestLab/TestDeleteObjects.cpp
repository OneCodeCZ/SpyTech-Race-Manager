///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  session TEST SUITE
// | P |  /  |  |  ~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2004 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  http://www.spytech.cz 
// | H |  _||   |
// |___|________|  Last changed: 2004
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestLab.h"
#include "TestDeleteObjects.h"
#include "OrmTest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CTestDeleteObjects::CTestDeleteObjects(IWsfTestMsgOutput* o) : CWsfTest(o)
{

}

/*****************************************************************************/
void CTestDeleteObjects::Run() throw (CException*)
{
   CSession session;
   
   // Create objects

   {
   CWsfOrmTransaction t(session);
   t.Begin();
   session.CreateTestData();
   t.Commit();
   }

   // Delete master object CColor causing slave object CShape
   // is updated - its color is set to NULL

   {
   CWsfOrmTransaction t(session);
   t.Begin();

   CWsfOrmObjects c(session, RUNTIME_CLASS(CColor));
   CColorPtr red = c[0];
   ASSERT(red->m_sName=="red");

   red->Delete();
   ASSERT(!red->IsValid());

   CWsfOrmObjects c2(session, RUNTIME_CLASS(CTriangle));
   CTrianglePtr tr = c2[0];
   ASSERT(tr->m_sName=="triangle1");
   ASSERT(tr->m_pColor==NULL);
   
   t.Commit();
   }

   // Delete master shape CArea causing slave shapes to be deleted
   // There remains only one shape: area2

   {
   CWsfOrmTransaction t(session);
   t.Begin();

   CWsfOrmObjects c(session, RUNTIME_CLASS(CArea));
   CAreaPtr a1 = c[0];
   ASSERT(a1->m_sName=="area1");

   a1->Delete();
   ASSERT(!a1->IsValid());

   CWsfOrmObjects c2(session, RUNTIME_CLASS(CShape));
   ASSERT(c2.GetCount()==1);
   a1 = c2[0];
   ASSERT(a1->m_sName=="area2");

   t.Commit();
   }
}
