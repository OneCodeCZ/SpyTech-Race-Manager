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
#include "TestCreateObjects.h"
#include "OrmTest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CTestCreateObjects::CTestCreateObjects(IWsfTestMsgOutput* o) : CWsfTest(o)
{

}

/*****************************************************************************/
CTestCreateObjects::~CTestCreateObjects()
{

}

/*****************************************************************************/
void CTestCreateObjects::Run() throw (CException*)
{
   CSession session;
   
   // Create objects

   {
   CWsfOrmTransaction t(session);
   t.Begin();
   session.CreateTestData();
   t.Commit();
   }

   // Load all shapes and check classes.
   // This tests inheritance: we load CShape objects but we get CArea etc.

   {
   CWsfOrmTransaction t(session);
   t.Begin();
   CWsfOrmObjects c(session, RUNTIME_CLASS(CShape));

   ASSERT(c.GetCount()==4);
   ASSERT_KINDOF(CArea, c[0]);
   ASSERT_KINDOF(CTriangle, c[1]);
   ASSERT_KINDOF(CTriangle, c[2]);
   ASSERT_KINDOF(CArea, c[3]);

   // Check properties, if they loaded right

   CAreaPtr a;
   a = c[0]; 
   ASSERT(a->m_sName=="area1"); 
   ASSERT(a->m_nBorderWidth==67);

   CShapePtr s;
   s = c[1]; ASSERT(s->m_sName=="triangle1");
   s = c[2]; ASSERT(s->m_sName=="triangle2");
   s = c[3]; ASSERT(s->m_sName=="area2");

   // Test cache - must be taken from cache, not loaded from DB

   ASSERT(&*session.GetOrm().GetObject(c[0]->GetId()) == &*c[0]);

   // Test lazy loading and CWsfOrmPtr

   s = c[2];
   CColorPtr col = s->m_pColor;
   ASSERT(s->m_pColor.ToBeLoaded());
   ASSERT(s->m_pColor->m_sName == "green");  //loading

   ASSERT(s->m_pColor.IsLoaded());
   ASSERT(s->m_pColor->m_sName == "green");  //loaded

   a = c[0];
   ASSERT(a->m_pParent == NULL);   // NULL test

   s = c[2];
   ASSERT(s->m_pParent == a);

   ASSERT(s);
   s = NULL;
   ASSERT(!s);

   t.Commit();
   }
}

