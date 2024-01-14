///////////////////////////////////////////////////////////////////////////////
//  ____________
// |   |    _   |
// | S | __|_|__|  WSL Generic SQL Database Support
// | P |  /  |  |  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// | Y | /-  |  |  Copyright (C)2001-2005 SpyTech
// | T |  |  \  |
// | E |  | _/  |  Author: Radek Tetik
// | C |  |__|  |  Last changed: 2005 01
// | H |  _||   |
// |___|________|
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\generic\WslPtr.h"
#include "..\generic\WslString.h"

class CWslDbCommand;
class CWslDbRecordset;
class CWslDatabase;
class CWslException;

typedef CWslPtr<CWslDbRecordset> CWslDbRecordsetPtr;

//*****************************************************************************
class WSL_API CWslDatabase
{
public:
   enum ETransactionIsolationLevels 
   { 
      READ_UNCOMMITTED,
      READ_COMMITTED,
      REPEATABLE_READ,
      SERIALIZABLE
   };

public:
	CWslDatabase();
	virtual ~CWslDatabase();

  //---------------------------------------------------------------------------
	// User API
	//---------------------------------------------------------------------------

	// Closes the DB
	void Close();

	// Returns TRUE if the DB is opened.
	_public virtual BOOL IsOpened() const = 0;

	// Executes SQL command or a prepared CWslDbCommand
	// The resulting SQL command must be < 1024 chars!
	void Execute(const char* pcszSQL, ...) throw (CWslException*);
	void Execute(CWslDbCommand& cmd) throw (CWslException*);

	// No limit on SQL command size
	void ExecuteNoFormat(const char* pcszSQL) throw (CWslException*);

	// Returns the recordset of the same "inheritence depth"
	// eg. for CWslODBCDatabase it returns CWslODBCRecordset
	_public virtual CWslDbRecordsetPtr CreateRecordsetObject() = 0;

   // Transactions
   virtual void SetTransactionIsolationLevel(ETransactionIsolationLevels e) throw (CWslException*) = 0;
   virtual void Commit() throw (CWslException*) = 0;
   virtual void Rollback() throw (CWslException*) = 0;

  //---------------------------------------------------------------------------
	// Helpers
	//---------------------------------------------------------------------------

	// Converts characters like ' " \ % to escape sequencies.
	// NOTE: MySQL assumed.
	// NOTE: % is escaped because of printf on the SQL string
	static CWslString Escape(const char* pcsz, BOOL bEscapePercent = false);

	// NOTE: % is escaped because of printf on the SQL string
	
	//---------------------------------------------------------------------------
	// API for childern classes
	//---------------------------------------------------------------------------

	// Called when the given SQL should be executed.
	// If the command should init a recordset, pRS is not null.
	virtual void OnExecute(const char* pcszSQL, CWslDbRecordset* pRS) throw (CWslException*) = 0;
	virtual void OnExecute(CWslDbCommand& cmd, CWslDbRecordset* pRS) throw (CWslException*) = 0;

	// Called when the DB should be closed.
	virtual void OnClose() = 0;
};









