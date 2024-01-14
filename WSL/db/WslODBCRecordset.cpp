///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - ODBC support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2002 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2002 09
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WslDb.h"

//*****************************************************************************
CWslOdbcRecordset::CWslOdbcRecordset()
{
	m_hstmt = NULL;
}

//*****************************************************************************
CWslOdbcRecordset::CWslOdbcRecordset(CWslDatabase* pDb) : CWslDbRecordset(pDb)
{
	ASSERT(pDb);
	ASSERT(dynamic_cast<CWslOdbcDatabase*>(pDb));
	m_hstmt = NULL;
}

//*****************************************************************************
CWslOdbcRecordset::CWslOdbcRecordset(CWslDatabase& db) : CWslDbRecordset(db)
{
	ASSERT(dynamic_cast<CWslOdbcDatabase*>(&db));
	m_hstmt = NULL;
}

//*****************************************************************************
CWslOdbcRecordset::~CWslOdbcRecordset()
{
	Close();
}

//*****************************************************************************
void CWslOdbcRecordset::OnOpen() throw (CWslException*)
{
	ASSERT(m_hstmt);
	
	// Get number of columns

	short n;
	SQLRETURN res = SQLNumResultCols(m_hstmt, &n);

	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) 
	{
		CWslOdbcException *e = new CWslOdbcException(m_hstmt, SQL_HANDLE_STMT);
		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
		m_hstmt = NULL;
		throw e;
	}

	// Init column array

	m_aCols.SetSize(n);
	for (int i=n; i--;) m_aCols[i] = NULL;
}

//*****************************************************************************
void CWslOdbcRecordset::OnClose()
{
	if (m_hstmt)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
		m_hstmt = NULL;
	}
}

//*****************************************************************************
void CWslOdbcRecordset::MoveNext() throw (CWslException*)
{
	ASSERT(m_hstmt);

	SQLRETURN res = SQLFetch(m_hstmt);
	
	if (res == SQL_NO_DATA)
	{
		m_bEOF = true;
	}
	else if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) 
	{
		CWslOdbcException* e = new CWslOdbcException(m_hstmt, SQL_HANDLE_STMT);
		SQLFreeHandle(SQL_HANDLE_STMT, m_hstmt);
		m_hstmt = NULL;
		throw e;
	}
}

//*****************************************************************************
void CWslOdbcRecordset::GetLongCharData(short nCol, long nActualSize, int nLen, CWslString& sValue)
{
	SQLRETURN res;
	char* pszString;

	ASSERT(nLen <= nActualSize);

	// Release the buffer now that data has been fetched
	// NOTE: The buffer is without the trailing '0'!
	sValue.ReleaseBuffer(nActualSize < nLen ? nActualSize-1 : nLen-1);

	// Reallocate the size (this will copy the data)
	pszString = sValue.GetBufferSetLength(nActualSize + 1);

	// Get pointer, skipping over original data, but not the NULL
	int nOldLen = nLen - 1;
	pszString += nOldLen;
	nLen = nActualSize + 1 - nOldLen;

	// Retrieve the column in question
	res = SQLGetData(m_hstmt, nCol+1, SQL_C_CHAR, (void*)pszString, nLen, &nActualSize);
	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) 
   {
      sValue.ReleaseBuffer(0);
      WslThrowDbException(m_hstmt, SQL_HANDLE_STMT);
   }

	// Release the buffer now that data has been fetched
	sValue.ReleaseBuffer(nActualSize + nOldLen);
}

//*****************************************************************************
void CWslOdbcRecordset::GetLongBinaryData(short nCol, long nActualSize, int nLen, CWslLongBinary *pBin)
{
	ASSERT(nLen < nActualSize);

	// Reallocate a bigger buffer
	pBin->Realloc(nActualSize);

	// Get pointer, skipping over original data
	BYTE* pbyData = pBin->GetData() + nLen;
	int nOldLen = nLen;
	nLen = nActualSize - nOldLen;

	// Retrieve the column in question
	SQLRETURN res = SQLGetData(m_hstmt, nCol+1, SQL_C_BINARY, pbyData, nLen, &nActualSize);

	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) 
   {
      pBin->Free();
      WslThrowDbException(m_hstmt, SQL_HANDLE_STMT);
   }

	ASSERT(pBin->GetSize() == nActualSize + nOldLen);
}

//*****************************************************************************
void CWslOdbcRecordset::OnGetFieldValue(int nCol, CWslVariant &v) throw (CWslException*)
{
	ASSERT(m_hstmt);
	ASSERT(nCol >=0 && nCol < m_aCols.GetSize()); 

	if (!m_aCols[nCol]) OnInitColumn(nCol);

   // Set variant type

   v.SetType(CWslOdbcDatabase::SQLDataType2VariantType(m_aCols[nCol]->nSQLType));

   // Get buffer for the data and its size

	int nBufferSize;
	void* pBuffer;

   if (v.GetType() == CWslVariant::VT_STRING)
   {
      // Buffer must have at least 2 chars to work with the Firebird ODBC
      // For MySQL 1 is enough.
      pBuffer = (void*) v.pString->GetBuffer(256);
      nBufferSize = 256;
   }
   else if (v.GetType() == CWslVariant::VT_BINARY)
   {
      // Buffer must have at least 1 char to work with the MySQL and Firebird ODBC
      v.pBinary->Alloc(1);
      pBuffer = v.GetData(&nBufferSize);
   }
   else
   {
      pBuffer = v.GetData(&nBufferSize);
   }

	// Get data

   int nActualSize;
   
	SQLRETURN res = SQLGetData(m_hstmt, 
		(short) nCol+1, 
		(short) CWslOdbcDatabase::VariantType2CDataType(v.GetType()), 
		(BYTE*) pBuffer, 
		nBufferSize, 
		(long*) &nActualSize);

   // Check result....

   switch (res)
   {
   case SQL_SUCCESS:
	   if (nActualSize == SQL_NULL_DATA)
	   {
		   v.SetNull();
	   }
	   else 
	   {
		   if (v.GetType() == CWslVariant::VT_STRING)
         {
            v.pString->ReleaseBuffer(nActualSize);  // nActualSize is without the NULL
         }
		   else if (v.GetType() == CWslVariant::VT_BINARY)
         {
			   if (nActualSize==0) v.pBinary->Free();
         }
	   }
      break;
	
	// No data found
   case SQL_NO_DATA_FOUND:
      v.SetNull();
		_RPT1(0,"CWslOdbcRecordset: Data has been already fetched for column #%d", nCol);
		WslThrowDbException(m_hstmt, SQL_HANDLE_STMT);
      break;

	// Variable length data -> fetch the rest
   case SQL_SUCCESS_WITH_INFO:
		if (v.GetType() == CWslVariant::VT_STRING)
		{
			GetLongCharData(nCol, nActualSize, nBufferSize, *v.pString);
		}
		else if (v.GetType() == CWslVariant::VT_BINARY)
		{
			GetLongBinaryData(nCol, nActualSize, nBufferSize, v.pBinary);
		}
		else
		{
         v.SetNull();
			_RPT1(0,"CWslOdbcRecordset: SQL_SUCCESS_WITH_INFO on non-variable-length column #%d", nCol);
			WslThrowDbException(m_hstmt, SQL_HANDLE_STMT);
		}
      break;

   default:
      v.SetNull();
		WslThrowDbException(m_hstmt, SQL_HANDLE_STMT);
   }
}

//*****************************************************************************
void CWslOdbcRecordset::OnInitColumn(int nCol)
{
	SQLRETURN res;
	char szName[256];
	short wType, wNameSize, wDecDigits, wNullable;
	DWORD dwColSize;

	ASSERT(m_hstmt);
	ASSERT(nCol >=0 && nCol < m_aCols.GetSize()); 

	res = SQLDescribeCol(m_hstmt, nCol+1, 
		(BYTE*) szName, 256, &wNameSize,
		&wType, 
		&dwColSize, &wDecDigits,
		&wNullable);
	if (res != SQL_SUCCESS && res != SQL_SUCCESS_WITH_INFO) WslThrowDbException(m_hstmt, SQL_HANDLE_STMT);


	ASSERT(wNameSize < 256);		// column name longer than buffer
	m_aCols[nCol] = new CColumn(CWslString(szName), wType);
}




