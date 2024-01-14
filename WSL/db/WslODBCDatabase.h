///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL ODBC Support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 05
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "WslDbVariant.h"

class CWslOdbcException;

//*****************************************************************************
class WSL_API CWslOdbcDatabase : public CWslDatabase
{
protected:
	SQLHENV  m_henv;
	SQLHDBC	m_hdbc;

public:
	CWslOdbcDatabase();
	virtual ~CWslOdbcDatabase();

	// Opens the given database.
   // - SQLDriverConnect() is used if the connection string contains "driver=". 
   //   Connection string must contain all the  needed information).
   // - SQLConnect() is used in other cases.
	// If bDiableTracing is TRUE ODBC tracing is disabled.
	// NOTE: For better security tracing should be disabled in release version!
   void Open(const char* pcszConnectionString, 
      BOOL bDisableTracing = true)
      throw (CWslOdbcException);

   // Simplified Open() version. pcszDb can be NULL -> no DB is used after open.
   void Open(const char* pcszDriver, 
      const char* pcszServer,
      const char* pcszUser,
      const char* pcszPwd,
      const char* pcszDb,
      const char* pcszExtraConnectionString = NULL,
      BOOL bDisableTracing = true)
		throw (CWslOdbcException);

   // Transactions
   virtual void SetTransactionIsolationLevel(ETransactionIsolationLevels e) throw (CWslException*);
   virtual void Commit() throw (CWslException*);
   virtual void Rollback() throw (CWslException*);

   //---------------------------------------------------------------------------
	// Type stuff
	//---------------------------------------------------------------------------
	
   static CWslVariant::ETypes SQLDataType2VariantType(int nSQLDataType);
	static int VariantType2CDataType(CWslVariant::ETypes eVariantType);
	static int VariantType2SQLDataType(CWslVariant::ETypes eVariantType);

	// Returns "column size" as defined in ODBC.
   // See Platform SDK "Data Services\MDAC\ODBC\Appendixes\Appendix D\Column Size"
	static int GetColumnSize(const CWslVariant& v);

   //---------------------------------------------------------------------------
	// Direct ODBC Access
	//---------------------------------------------------------------------------

   SQLHENV GetEnvironmentHandle() const { return m_henv; }
   SQLHDBC GetConnectionHandle() const { return m_hdbc; }
   
   void Attach(CWslOdbcDatabase& db);
   void Detach();

   //---------------------------------------------------------------------------
	// From CWslDatabase
	//---------------------------------------------------------------------------

   virtual void OnClose();	
	virtual BOOL IsOpened() const { return m_hdbc!=SQL_NULL_HDBC || m_henv!=SQL_NULL_HENV; }
	virtual CWslDbRecordsetPtr CreateRecordsetObject();
	virtual void OnExecute(const char* pcszSQL, CWslDbRecordset* pRS) throw (CWslException*);
	virtual void OnExecute(CWslDbCommand& cmd, CWslDbRecordset* pRS) throw (CWslException*);
};
