///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  ORM TEST SUITE
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
#include "OrmTest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CSession::CSession()
{
	m_db.Open(DB_ODBC_DRIVER, "localhost", DB_USER, DB_PWD, NULL);
   m_orm.Open(m_db);

   m_orm.AddClass("CColor");
   m_orm.AddClass("CShape");
   m_orm.AddClass("CTriangle");
   m_orm.AddClass("CArea");

   CWslSqlScript s;
   s.LoadScript("c:/projects/wsf/doc/orm_test_database.sql");
   s.Perform(m_db, false);
}

/*****************************************************************************/
CSession::~CSession()
{
   m_orm.Close();
   m_db.Close();
}

/*****************************************************************************
*
* CREATING OBJECTS
*
*****************************************************************************/

/*****************************************************************************/
CColorPtr CSession::CreateColor(const char* pcszName, int r, int g, int b) throw (CException*)
{
   CWsfOrmObjectPtr p1 = m_orm.CreateObject(RUNTIME_CLASS(CColor));
   CColorPtr p = p1;
   p->Construct(pcszName, r,g,b);
   p->Constructed();
   return p;
}

/*****************************************************************************/
CTrianglePtr CSession::CreateTriangle(const char* pcszName, int x, int y, int alpha, int beta, int base, CShapePtr pParent) throw (CException*)
{
   CTrianglePtr p = m_orm.CreateObject(RUNTIME_CLASS(CTriangle));
   p->Construct(pcszName, x,y,alpha,beta,base,pParent);
   p->Constructed();
   return p;
}

/*****************************************************************************/
CAreaPtr CSession::CreateArea(const char* pcszName, int x, int y, int nBorderWidth, CShapePtr pParent) throw (CException*)
{
   CAreaPtr p = m_orm.CreateObject(RUNTIME_CLASS(CArea));
   p->Construct(pcszName, x,y,nBorderWidth,pParent);
   p->Constructed();
   return p;
}


/*****************************************************************************
*
* TEST DATA
*
*****************************************************************************/

/*****************************************************************************/
void CSession::CreateTestData() throw (CException*)
{
   CColorPtr red = CreateColor("red",255,0,0);
   CColorPtr green = CreateColor("green",0,255,0);
   CColorPtr blue = CreateColor("blue",0,0,255);

   CAreaPtr a1 = CreateArea("area1",0,0,67,NULL);
   ASSERT(a1);

   CTrianglePtr t1 = CreateTriangle("triangle1",0,0,45,45,100, a1);
   t1->m_pColor = red;

   CTrianglePtr t2 = CreateTriangle("triangle2",0,0,45,45,100,a1);
   t2->m_pColor = green;

   CAreaPtr a2 = CreateArea("area2",0,0,1,NULL);

   // Must be valid

   ASSERT(red->IsValid());

   // Test cache - must be taken from cache, not loaded from DB

   ASSERT(&*m_orm.GetObject(red->GetId()) == &*red);
}