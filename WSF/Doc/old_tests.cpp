/*****************************************************************************
*
* ORM TEST
*
*****************************************************************************/

/*****************************************************************************/
COrmTest::COrmTest() 
{
}

/*****************************************************************************/
COrmTest::~COrmTest()
{

}

/*****************************************************************************/
void COrmTest::Run() throw (CException*)
{
	m_Db.Open(DB_ODBC_DRIVER, "localhost", DB_USER, DB_PWD, NULL);

	m_Db.Execute("SET SESSION TRANSACTION ISOLATION LEVEL SERIALIZABLE");
	m_Db.Execute("SET AUTOCOMMIT = 0");   

   m_Db.Execute("DROP DATABASE IF EXISTS wsf_test");
   m_Db.Execute("CREATE DATABASE wsf_test");
   m_Db.Execute("USE wsf_test");

   m_Db.Execute("CREATE TABLE tblIDs (name VARCHAR(255), value INT) TYPE = InnoDB");
   m_Db.Execute("INSERT INTO tblIDs VALUES ('id',1)");
   m_Db.Execute("CREATE TABLE tblOrmObjects (id INT NOT NULL PRIMARY KEY, orm_class VARCHAR(64) NOT NULL, UNIQUE INDEX (id)) TYPE = InnoDB");
   m_Db.Execute("CREATE TABLE tblObjects (id INT, pid INT, name VARCHAR(255), count INT, UNIQUE INDEX (id), FOREIGN KEY (id) REFERENCES tblOrmObjects (id) ON DELETE CASCADE) TYPE = InnoDB");
   m_Db.Execute("COMMIT");

   TestProperty();

   CreateObjects();
   TestCollection();

   m_Db.Execute("COMMIT");
   m_Db.Execute("DROP DATABASE wsf_test");
}

/*****************************************************************************/
void COrmTest::TestProperty() throw (CException*)
{
   m_Db.Execute("CREATE TABLE PropertyTest (s VARCHAR(255), b INT, b2 INT, d DATE) TYPE = InnoDB");

   CWsfOrmString s;
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
   m_Db.Execute(c);

   CWslDbRecordsetPtr rs = m_Db.CreateRecordsetObject();
   rs->OpenNoFormat("SELECT * FROM PropertyTest");
   ASSERT(!rs->IsEOF());
   
   s.Load(*rs, "s");
   ASSERT(s=="xxx");
   ASSERT(!s.IsModified());
   
   b.Load(*rs, "b");
   ASSERT(b==TRUE);

   d.Load(*rs, "d");
   ASSERT((const CWslDate&)d == CWslDate(2004,1,1));

   m_Db.Execute("DELETE FROM PropertyTest");
   }

   // Save / Load -- NULL test

   {
   CWslDbCommand c;
   
   s.SetNull();
   s.Save(c, "s");
   
   b.SetNull();
   b.Save(c, "b");
   
   c.BuildInsert("PropertyTest");
   m_Db.Execute(c);

   CWslDbRecordsetPtr rs = m_Db.CreateRecordsetObject();
   rs->OpenNoFormat("SELECT * FROM PropertyTest");
   ASSERT(!rs->IsEOF());
   
   s.Load(*rs, "s");
   ASSERT(s.IsNull());
   ASSERT(!s.IsModified());
   
   b.Load(*rs, "b");
   ASSERT(b.IsNull());
   }

   m_Db.Execute("COMMIT");
}

/*****************************************************************************/
void COrmTest::CreateObjects() throw (CException*)
{
   int nId;

   {
   CWsfOrmTransaction t(m_Orm);

   COrmObjectPtr p = m_Orm.CreateTestObject();
   p->m_nCount = 3;
   nId = p->GetId();
    
   COrmObjectPtr p2 = m_Orm.LoadObject(nId);
   ASSERT(p2);
   ASSERT((DWORD)(&*p) == (DWORD)(&*p2));  // taken form cache
   ASSERT(p2->m_nCount==3);
   ASSERT(p2->m_nCount.IsModified());

   t.Commit();
   ASSERT(!p2->m_nCount.IsModified());
   }

   {
   CWsfOrmTransaction t(m_Orm);

   COrmObjectPtr p2 = m_Orm.LoadObject(nId);
   ASSERT(p2);
   ASSERT(p2->m_nCount==3);

   COrmObjectPtr p3 = m_Orm.LoadObject(nId);
   ASSERT(p3);
   ASSERT((DWORD)(&*p2) == (DWORD)(&*p3));
   ASSERT(p3->m_nCount==3);

   t.Commit();
   }

   {
   CWsfOrmTransaction t(m_Orm);
   
   COrmObjectPtr p = m_Orm.LoadObject(nId);
   ASSERT(p);
   ASSERT(p->m_nCount==3);
   p->Delete();
   ASSERT(!p->IsValid());

   COrmObjectPtr p2 = m_Orm.LoadObject(nId);
   ASSERT(!p2);
   
   t.Commit();
   }

   {
   CWsfOrmTransaction t(m_Orm);

   COrmObjectPtr p = m_Orm.LoadObject(nId);
   ASSERT(!p);
   
   t.Commit();
   }
}

/*****************************************************************************/
void COrmTest::TestCollection() throw (CException*)
{
   COrmObjectPtr p;
   COrmObjectCollection c(m_Orm);

   // Delete all objects

   {
   CWsfOrmTransaction t(m_Orm);
   
   for (int i=0; i<c.GetCount(); i++)
   {
      c[i]->Delete();
      ASSERT(!c[i]->IsValid());
   }
   
   c.Refresh();
   ASSERT(c.IsEmpty());

   t.Commit();
   }

   // Search just created objects

   {
   CWsfOrmTransaction t(m_Orm);

   p = m_Orm.CreateTestObject();
   p->m_nCount = 1;
   p = m_Orm.CreateTestObject();
   p->m_nCount = 2;
   p = m_Orm.CreateTestObject();
   p->m_nCount = 3;
   
   c.Refresh();
   ASSERT(c.GetCount()==3);
   ASSERT(!c.IsEmpty());
   ASSERT(c[0]->m_nCount==1);
   ASSERT(c[1]->m_nCount==2);
   ASSERT(c[2]->m_nCount==3);

   t.Commit();
   }

   // Search loaded objects

   {
   CWsfOrmTransaction t(m_Orm);

   c.Refresh();
   ASSERT(c.GetCount()==3);
   ASSERT(!c.IsEmpty());
   ASSERT(c[0]->m_nCount==1);
   ASSERT(c[1]->m_nCount==2);
   ASSERT(c[2]->m_nCount==3);

   t.Commit();
   }
}






