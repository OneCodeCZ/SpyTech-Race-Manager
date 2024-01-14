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
#include "PropertyTest.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*****************************************************************************/
CTestProperties::CTestProperties(IWsfTestMsgOutput* o) : CWsfTest(o)
{

}

/*****************************************************************************/
CTestProperties::~CTestProperties()
{

}

/*****************************************************************************/
void CTestProperties::Run() throw (CException*)
{
   CWslOdbcDatabase db;
   CWsfOrm orm;

	db.Open(DB_ODBC_DRIVER, "localhost", DB_USER, DB_PWD, NULL);
   db.ExecuteNoFormat("DROP DATABASE IF EXISTS PROPERTY_TEST");
   db.ExecuteNoFormat("CREATE DATABASE PROPERTY_TEST");
   db.ExecuteNoFormat("USE PROPERTY_TEST");
   db.Execute("CREATE TABLE PropertyTest (s VARCHAR(255), b INT, b2 INT, d DATE) TYPE = InnoDB");

   CWsfOrmString s, s1;
   CWsfOrmBool b;
   CWsfOrmDate d;

   // General template test (on CString version)

   ASSERT(s.IsNull());
   ASSERT(!s.IsModified());

   s = CString("xxx");
   ASSERT(s=="xxx");
   ASSERT(!s.IsNull());
   ASSERT(s.IsModified());

   s.SetModified(false);
   ASSERT(!s.IsModified());

   s.SetNull();
   ASSERT(s.IsNull());
   ASSERT(s.IsModified());

   s = CString("xxx");

   // BOOL Test

   b = true;
   ASSERT(b==TRUE);

   // Save / Load --  Value test

   {
   CWslDbCommand c;

   ASSERT(s.IsModified());
   s.Save(c, "s");
   ASSERT(!s.IsModified());

   b.Save(c, "b");

   d = CWslDate(2004,1,1);
   ASSERT((const CWslDate&)d == CWslDate(2004,1,1));
   d.Save(c, "d");

   c.BuildInsert("PropertyTest");
   db.Execute(c);

   CWslDbRecordsetPtr rs = db.CreateRecordsetObject();
   rs->OpenNoFormat("SELECT * FROM PropertyTest");
   ASSERT(!rs->IsEOF());
   
   s.Load(*rs, "s", orm);
   ASSERT(s=="xxx");
   ASSERT(!s.IsModified());
   

   b.Load(*rs, "b", orm);
   ASSERT(b==TRUE);

   d.Load(*rs, "d", orm);
   ASSERT((const CWslDate&)d == CWslDate(2004,1,1));

   db.Execute("DELETE FROM PropertyTest");
   }

   // Save / Load -- NULL test

   {
   CWslDbCommand c;
   
   s.SetNull();
   s.Save(c, "s");
   
   b.SetNull();
   b.Save(c, "b");
   
   c.BuildInsert("PropertyTest");
   db.Execute(c);

   CWslDbRecordsetPtr rs = db.CreateRecordsetObject();
   rs->OpenNoFormat("SELECT * FROM PropertyTest");
   ASSERT(!rs->IsEOF());
   
   s.Load(*rs, "s", orm);
   ASSERT(s.IsNull());
   ASSERT(!s.IsModified());
   
   b.Load(*rs, "b", orm);
   ASSERT(b.IsNull());
   }

   db.ExecuteNoFormat("DROP DATABASE IF EXISTS PROPERTY_TEST");

   // Assigning NULL

   s = "ABC";
   ASSERT(!s.IsNull());
   s1.SetNull();
   s = s1;
   ASSERT(s.IsNull());
}
