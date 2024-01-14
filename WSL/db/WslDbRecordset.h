///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL - ODBC support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2003 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2003 10
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef WSL_API
#define WSL_API
#endif

class CWslDatabase;
class CWslVariant;
class CWslDbCommand;

#include "..\generic\WslArray.h"
#include "..\generic\WslDate.h"
#include "..\generic\WslDateTime.h"

//*****************************************************************************
class WSL_API CWslDbRecordset
{
friend CWslDbCommand;

protected:
	class CColumn
	{
	public:
		int					nSQLType;   // SQL_?????
		CWslString	sName;
	public:
		CColumn(const char* pcszName, int nSQLType_) : sName(pcszName), nSQLType(nSQLType_) {};
		CColumn(const CWslString& sName_, int nSQLType_) : sName(sName_), nSQLType(nSQLType_) {};
	};

protected:
	CWslDatabase*				m_pDb;
	BOOL								m_bEOF;
	CWslArray<CColumn*>	m_aCols;

public:
  //---------------------------------------------------------------------------
	// Openning the recordset
	//---------------------------------------------------------------------------

	CWslDbRecordset();
	CWslDbRecordset(CWslDatabase* pDb);
	CWslDbRecordset(CWslDatabase& db);
	virtual ~CWslDbRecordset();

	// Set/get the DB
	void SetDb(CWslDatabase* pDb);
	void SetDb(CWslDatabase& db);
	inline CWslDatabase* GetDb() const { return m_pDb; }

	// Opens the RS with formatted SQL (same as sprintf(), don't
  // forget to escape % if neccessary!)
	// or with prepared command.
  // Use OpenNoFormat() if no formatting is needed to avoid a need
  // to escape %.
	void Open(const char* pcszSQL, ...) throw (CWslException*);
	void OpenNoFormat(const char* pcszSQL) throw (CWslException*);
	void Open(CWslDbCommand& cmd) throw (CWslException*);

	// Closes the RS.
	void Close();
	
  //---------------------------------------------------------------------------
	// Fetching data
	//---------------------------------------------------------------------------

	// Returns TRUE if there are no more rows available.
	inline BOOL IsEOF() const { return m_bEOF; }

	// If IsEOF() is FALSE, you can fetch another row of data.
	// After opening the RS, if IsEOF() is FALSE the
	// first row is already fetched, so you don't have to call MoveNext().
	_public virtual void MoveNext() throw (CWslException*) = 0;

	// Retrieving values. Call just once for each column per row.
	void GetFieldValue(const char* pcszCol, CWslVariant& v) throw (CWslException*);
	void GetFieldValue(const char* pcszCol, CWslString& s) throw (CWslException*);
	void GetFieldValue(const char* pcszCol, int& nVal, int nNullVal = 0) throw (CWslException*);
	void GetFieldValue(const char* pcszCol, bool& bVal, bool bNullVal = false) throw (CWslException*);
	void GetFieldValue(const char* pcszCol, __int64& qVal, __int64 qNullVal = 0) throw (CWslException*);
	void GetFieldValue(const char* pcszCol, float& fVal, float fNullVal = 0) throw (CWslException*);
	void GetFieldValue(const char* pcszCol, double& dblVal, double dblNullVal = 0) throw (CWslException*);
	void GetFieldValue(const char* pcszCol, CWslDateTime& dt) throw (CWslException*);
	void GetFieldValue(const char* pcszCol, CWslLongBinary& lb) throw (CWslException*);
	void GetFieldValue(const char* pcszCol, CWslDate& date) throw (CWslException*);

   void GetFieldValue(int nCol, CWslVariant& v) throw (CWslException*);
	void GetFieldValue(int nCol, CWslString& s) throw (CWslException*);
	void GetFieldValue(int nCol, int& nVal, int nNullVal = 0) throw (CWslException*);
	void GetFieldValue(int nCol, __int64& qVal, __int64 qNullVal = 0) throw (CWslException*);
	void GetFieldValue(int nCol, CWslDateTime& dt) throw (CWslException*);

  //---------------------------------------------------------------------------
	// Columns
	//---------------------------------------------------------------------------

	// Returns index of given column / name of given column index.
	int	GetColIdx(const char* pcszCol) throw (CWslException*);
	const CWslString& GetColName(int nCol) throw (CWslException*);
	
	// Returns column type of the given column.
	int GetColType(int nCol) throw (CWslException*);

	// Returns the column count.
	inline int GetColCount() const { return m_aCols.GetSize(); }

  //---------------------------------------------------------------------------
	// API for childern
	//---------------------------------------------------------------------------

	// Called when recordset is opened.
	// Init here the number of columns. The column
	// structures are init later on-demand using OnInitColumn().
	// Initially store NULL pointer for the CColumn structure for each column.
	_protected virtual void OnOpen() throw (CWslException*) = 0;

	// Creates and fills CColumn structure for the given column index.
	_protected virtual void OnInitColumn(int nCol) throw (CWslException*) = 0;

	// Retrieves a value for given column. The column must be inited!
	_protected virtual void OnGetFieldValue(int nCol, CWslVariant& v) throw (CWslException*) = 0;

	// Called when the RS is closing.
	// Close the handle here.
	_protected virtual void OnClose() = 0;
};


