///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL ODBC Support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 06
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslDb.h"   

//*****************************************************************************
CWslOdbcDatabase::CWslOdbcDatabase()
{
	m_henv = SQL_NULL_HENV;
	m_hdbc = SQL_NULL_HDBC;
}

//*****************************************************************************
CWslOdbcDatabase::~CWslOdbcDatabase()
{
	// you should close the db!
	//ASSERT(m_hdbc==SQL_NULL_HDBC && m_henv==SQL_NULL_HENV); 
	Close();
}

/*****************************************************************************/
CWslDbRecordsetPtr CWslOdbcDatabase::CreateRecordsetObject()
{
	CWslDbRecordsetPtr p;
	p = new CWslOdbcRecordset(this);
	return p;
}

//*****************************************************************************
void CWslOdbcDatabase::Attach(CWslOdbcDatabase& db)
{
	m_hdbc = db.GetConnectionHandle();
	m_henv = db.GetEnvironmentHandle();
}

//*****************************************************************************
void CWslOdbcDatabase::Detach()
{
	m_hdbc = SQL_NULL_HDBC;
	m_henv = SQL_NULL_HENV;
}

//*****************************************************************************
void CWslOdbcDatabase::Open(const char *pcszConnectionString, 
                            BOOL bDisableTracing)
                            throw (CWslOdbcException)
{
	SQLRETURN res;

	ASSERT(m_hdbc==SQL_NULL_HDBC && m_henv==SQL_NULL_HENV);		// call close
	ASSERT(pcszConnectionString);

	Close();

	// Environment

//   SQLSetEnvAttr(NULL, SQL_ATTR_CONNECTION_POOLING, (void*)SQL_CP_ONE_PER_DRIVER, 0);

	res = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_henv);
	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) WslThrowDbException(m_henv, SQL_HANDLE_ENV);

	res = SQLSetEnvAttr(m_henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0); 
	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) goto ERR0;

   // Connection

	res = SQLAllocHandle(SQL_HANDLE_DBC, m_henv, &m_hdbc);
	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) goto ERR0;

   // Connection parameters

	res = SQLSetConnectAttr(m_hdbc, SQL_ATTR_LOGIN_TIMEOUT, (void*)10, 0);
	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) goto ERR1;

	res = SQLSetConnectAttr(m_hdbc, SQL_ATTR_AUTOCOMMIT, (void*) SQL_AUTOCOMMIT_OFF, 0);
	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) goto ERR1;
   
	if (bDisableTracing)
	{
		res = SQLSetConnectAttr(m_hdbc, SQL_ATTR_TRACE, (void*)SQL_OPT_TRACE_OFF, 0);
		if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) goto ERR1;
	}

	// Connect

   if (strstr(pcszConnectionString, "driver="))
   {
	   short wOutSize;
	   char szOut[1024];

      res = SQLDriverConnect(m_hdbc, NULL, 
		   (BYTE*)pcszConnectionString, SQL_NTS,
		   (BYTE*)szOut, 1024, &wOutSize, SQL_DRIVER_NOPROMPT);
   }
   else
   {
   	res = SQLConnect(m_hdbc, (BYTE*)pcszConnectionString, SQL_NTS, 
	   	NULL, 0, NULL, 0);
   }

	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) goto ERR1;

	// Check capabilities

   SQLUINTEGER x;
   if (SQLGetInfo(m_hdbc,SQL_GETDATA_EXTENSIONS, &x, sizeof(x), NULL) != SQL_SUCCESS) goto ERR1;

   if (!(x & SQL_GD_ANY_ORDER)) 
   {
      Close();
      throw new CWslTextException("CWslOdbcDatabase: The ODBC driver doesn't support SQL_GD_ANY_ORDER!");
   }   

	return;

	// Error handling

ERR0:
	{
	CWslOdbcException *e = new CWslOdbcException(m_henv, SQL_HANDLE_ENV);
	SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
	m_henv = SQL_NULL_HENV;
	throw e;
	}
ERR1:
	{
	CWslOdbcException *e = new CWslOdbcException(m_hdbc, SQL_HANDLE_DBC);
	SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, m_henv);
	m_hdbc = SQL_NULL_HDBC;
	m_henv = SQL_NULL_HENV;
	throw e;
	}
}

/*****************************************************************************/
void CWslOdbcDatabase::Open(const char* pcszDriver, 
                            const char* pcszServer,
                            const char* pcszUser,
                            const char* pcszPwd,
                            const char* pcszDb,
                            const char* pcszExtraConnectionString,
                            BOOL bDisableTracing)
            		          throw (CWslOdbcException)
{
   CWslString s, s2;
   
   s.Format("driver=%s;server=%s;port=3306;user=%s;password=%s", 
      pcszDriver,
      pcszServer,
      pcszUser,
      pcszPwd);
   
   if (pcszDb) { s2.Format(";database=%s", pcszDb); s += s2; }
   if (pcszExtraConnectionString) { s2.Format(";%s", pcszExtraConnectionString); s += s2; }
   
   Open(s, bDisableTracing);
}

//*****************************************************************************
void CWslOdbcDatabase::OnClose()
{
	if (m_hdbc==SQL_NULL_HDBC && m_henv==SQL_NULL_HENV) return;

	VERIFY(SQLDisconnect(m_hdbc) == SQL_SUCCESS);
	VERIFY(SQLFreeHandle(SQL_HANDLE_DBC, m_hdbc) == SQL_SUCCESS);
	VERIFY(SQLFreeHandle(SQL_HANDLE_ENV, m_henv) == SQL_SUCCESS);

	m_hdbc = SQL_NULL_HDBC;
	m_henv = SQL_NULL_HENV;
}

/*****************************************************************************
*
* TRANSACTIONS 
*
*****************************************************************************/

/*****************************************************************************/
void CWslOdbcDatabase::SetTransactionIsolationLevel(ETransactionIsolationLevels e) throw (CWslException*)
{
	ASSERT(IsOpened());
	
   int n;

   switch (e)
   {
   default: ASSERT(false);
   case SERIALIZABLE:      n = SQL_TXN_SERIALIZABLE; break;
   case READ_UNCOMMITTED:  n = SQL_TXN_READ_UNCOMMITTED; break;
   case READ_COMMITTED:    n = SQL_TXN_READ_COMMITTED; break;
   case REPEATABLE_READ:   n = SQL_TXN_REPEATABLE_READ; break;
   }

   SQLRETURN res = ::SQLSetConnectAttr(m_hdbc, SQL_ATTR_TXN_ISOLATION, (void*) n, 0);
	
   if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) WslThrowDbException(m_hdbc, SQL_HANDLE_DBC);
}

/*****************************************************************************/
void CWslOdbcDatabase::Commit() throw (CWslException*)
{
	ASSERT(IsOpened());
   _RPT0(0, "CWslOdbcDatabase::Commit()\n");

   SQLRETURN res = ::SQLEndTran(SQL_HANDLE_DBC, m_hdbc, SQL_COMMIT);

   if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) WslThrowDbException(m_hdbc, SQL_HANDLE_DBC);
}

/*****************************************************************************/
void CWslOdbcDatabase::Rollback() throw (CWslException*)
{
	ASSERT(IsOpened());
   _RPT0(0, "CWslOdbcDatabase::Rollback()\n");

   SQLRETURN res = ::SQLEndTran(SQL_HANDLE_DBC, m_hdbc, SQL_ROLLBACK);

   if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) WslThrowDbException(m_hdbc, SQL_HANDLE_DBC);
}


/*****************************************************************************
*
* EXECUTING
*
*****************************************************************************/

//#include "..\files\wslfile.h"

/*****************************************************************************/
void CWslOdbcDatabase::OnExecute(const char* pcszSQL, CWslDbRecordset* pRS) throw (CWslException*)
{
	ASSERT(pcszSQL);
	ASSERT(m_hdbc!=SQL_NULL_HDBC && m_henv!=SQL_NULL_HENV);	// call open() before executesql()
	SQLRETURN res;
	SQLHSTMT hstmt;

	_RPT1(0,"CWslOdbcDatabase: %s...", Escape(pcszSQL, true));

   if (pRS) pRS->Close();

   /*CWslFile f;
   f.Append("c:\\!\\dump.sql");
   f.WriteLn(pcszSQL);
   f.Close();*/

	int nTime = GetTickCount();

	res = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt);
	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) WslThrowDbException(m_hdbc, SQL_HANDLE_DBC);

	// Execute command.

	res = SQLExecDirect(hstmt, (BYTE*)pcszSQL, SQL_NTS);

	if (res != SQL_NO_DATA && res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) 
	{
		CWslOdbcException* e = new CWslOdbcException(hstmt, SQL_HANDLE_STMT);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		throw e;
	}

	// DONE!

	if (pRS)
	{
		CWslOdbcRecordset* p = dynamic_cast<CWslOdbcRecordset*>(pRS);
		ASSERT(p);
		p->m_hstmt = hstmt;
	}
	else SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	TRACE1("%d ms\n", GetTickCount()-nTime);
}

//*****************************************************************************
void CWslOdbcDatabase::OnExecute(CWslDbCommand& cmd, CWslDbRecordset* pRS) throw (CWslException*)
{
	SQLRETURN res;
	SQLHSTMT hstmt;
	int i,nDataSize;
	void *pData;

	ASSERT(m_hdbc!=SQL_NULL_HDBC && m_henv!=SQL_NULL_HENV);	// call open() before executesql()
	ASSERT(!cmd.m_sSql.IsEmpty());

	_RPT1(0,"CWslOdbcCommand: %s...", Escape(cmd.m_sSql, true));

   if (pRS) pRS->Close();

	int nTime = GetTickCount();

	// Allocate handle

	res = SQLAllocHandle(SQL_HANDLE_STMT, m_hdbc, &hstmt);
	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) WslThrowDbException(m_hdbc, SQL_HANDLE_DBC);

	// Bind parameters

	for (i=0; i<cmd.m_aParams.GetSize(); i++)
	{
		if (!cmd.m_aParams[i]->bInsertAsParam) continue;

		pData = cmd.m_aParams[i]->Value.GetData(&nDataSize);
      cmd.m_aParams[i]->nLen = CWslOdbcDatabase::GetColumnSize(cmd.m_aParams[i]->Value);

		res = SQLBindParameter(hstmt,
			i+1, 
			SQL_PARAM_INPUT, 
			VariantType2CDataType(cmd.m_aParams[i]->Value.GetType()),		// value type
			VariantType2SQLDataType(cmd.m_aParams[i]->Value.GetType()),	// param type
			cmd.m_aParams[i]->nLen,
			0,
			(SQLPOINTER) pData, 
			0,
			&cmd.m_aParams[i]->nLen);

		if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) 
		{
			CWslOdbcException* e = new CWslOdbcException(hstmt, SQL_HANDLE_STMT);
			SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
			throw e;
		}
	}

	// Execute SQL

	res = SQLExecDirect(hstmt, (BYTE*)(const char*)cmd.m_sSql, SQL_NTS);

	if (res != SQL_NO_DATA && res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) 
	{
		CWslOdbcException* e = new CWslOdbcException(hstmt, SQL_HANDLE_STMT);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		throw e;
	}

	// DONE!

	if (pRS)
	{
		CWslOdbcRecordset* p = dynamic_cast<CWslOdbcRecordset*>(pRS);
		ASSERT(p);
		p->m_hstmt = hstmt;
	}
	else SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	TRACE1("%d ms\n", GetTickCount()-nTime);
}

/*****************************************************************************
*
* TYPE STUFF
*
*****************************************************************************/

//*****************************************************************************
int CWslOdbcDatabase::VariantType2CDataType(CWslVariant::ETypes eVariantType)
{
  // See Platform SDK "Data Services\MDAC\ODBC\Appendixes\Appendix D\C Data Types"

	switch (eVariantType)
	{
	default: ASSERT(false);
   case CWslVariant::VT_NULL	: return SQL_C_UTINYINT;
	case CWslVariant::VT_BOOL	: return SQL_C_UTINYINT;
	case CWslVariant::VT_BYTE	: return SQL_C_UTINYINT;	
	case CWslVariant::VT_SHORT	: return SQL_C_SSHORT;
	case CWslVariant::VT_LONG	: return SQL_C_SLONG;
	case CWslVariant::VT_BIGINT: return SQL_C_SBIGINT;
	case CWslVariant::VT_FLOAT	: return SQL_C_FLOAT;
	case CWslVariant::VT_DOUBLE: return SQL_C_DOUBLE;
	case CWslVariant::VT_DATE	: return SQL_C_TYPE_TIMESTAMP;
	case CWslVariant::VT_STRING: return SQL_C_CHAR;
	case CWslVariant::VT_BINARY: return SQL_C_BINARY;
	}
}

//*****************************************************************************
int CWslOdbcDatabase::VariantType2SQLDataType(CWslVariant::ETypes eVariantType)
{
	switch (eVariantType)
	{
	default: ASSERT(false);
	case CWslVariant::VT_NULL	: return SQL_TINYINT;
	case CWslVariant::VT_BOOL	: return SQL_TINYINT;
	case CWslVariant::VT_BYTE	: return SQL_TINYINT;	
	case CWslVariant::VT_SHORT	: return SQL_SMALLINT;
	case CWslVariant::VT_LONG	: return SQL_INTEGER;
   case CWslVariant::VT_BIGINT: return SQL_BIGINT;
	case CWslVariant::VT_FLOAT	: return SQL_FLOAT;
	case CWslVariant::VT_DOUBLE: return SQL_DOUBLE;
	case CWslVariant::VT_DATE	: return SQL_TYPE_TIMESTAMP;
	case CWslVariant::VT_STRING: return SQL_LONGVARCHAR;
	case CWslVariant::VT_BINARY: return SQL_LONGVARBINARY;
	}
}

//*****************************************************************************
CWslVariant::ETypes CWslOdbcDatabase::SQLDataType2VariantType(int nSQLDataType)
{
	switch (nSQLDataType)
	{
	case SQL_TINYINT				: return CWslVariant::VT_BYTE;
	case SQL_SMALLINT				: return CWslVariant::VT_SHORT;
	case SQL_INTEGER				: return CWslVariant::VT_LONG;
   case SQL_BIGINT            : return CWslVariant::VT_BIGINT;

	case SQL_REAL:
	case SQL_FLOAT: 
		return CWslVariant::VT_FLOAT;

	case SQL_DOUBLE:	
		return CWslVariant::VT_DOUBLE;

	case SQL_TYPE_DATE:
	case SQL_TYPE_TIME:
	case SQL_TYPE_TIMESTAMP:
		return CWslVariant::VT_DATE;

	case SQL_CHAR:
	case SQL_VARCHAR:
	case SQL_LONGVARCHAR:	
		return CWslVariant::VT_STRING;	

	case SQL_BINARY:
	case SQL_VARBINARY:
	case SQL_LONGVARBINARY:	
		return CWslVariant::VT_BINARY;

	default: 
		ASSERT(false); 
		return CWslVariant::VT_NULL;
	}
}

//*****************************************************************************
int CWslOdbcDatabase::GetColumnSize(const CWslVariant& v)
{
  // See Platform SDK "Data Services\MDAC\ODBC\Appendixes\Appendix D\Column Size"

	switch (v.GetType())
	{
	default: ASSERT(false);
	case CWslVariant::VT_NULL	 : return SQL_NULL_DATA;
	case CWslVariant::VT_BOOL	 : return 3;
	case CWslVariant::VT_BYTE	 : return 3;	
	case CWslVariant::VT_SHORT	 : return 5;
	case CWslVariant::VT_LONG	 : return 10;
	case CWslVariant::VT_FLOAT	 : return 15;
	case CWslVariant::VT_DOUBLE : return 15;
	case CWslVariant::VT_DATE	 : return 20;
   case CWslVariant::VT_BIGINT : return 19;
	case CWslVariant::VT_STRING : return v.pString->GetLength(); 
	case CWslVariant::VT_BINARY : return v.pBinary->GetSize();
	}
}